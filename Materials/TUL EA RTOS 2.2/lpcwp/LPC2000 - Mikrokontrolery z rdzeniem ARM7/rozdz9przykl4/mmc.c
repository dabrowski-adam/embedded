//----- Include Files ---------------------------------------------------------
#include "lpc214x.h"
#include "mmc.h"
//------- Pin Defs ----------------------------------------------------------
//Ustawienia kontrolera VIC
#define SCK1_P017_SEL (2<<2)
#define MISO1_P018_SEL (2<<4) 
#define MOSI1_P019_SEL (2<<6)


#define MMC_SET		IO0SET
#define MMC_CLR		IO0CLR
#define MMC_DIR		IO0DIR
#define MMC_CS		(1<<21)


//Inicjalizacja protokolu mmc i spi
void mmcInit(void)
{
	//Piny jako funkcja alternatywna SPI
	PINSEL1 |= SCK1_P017_SEL|MISO1_P018_SEL|MOSI1_P019_SEL;
	//SPI Master 8 bitow CPOL 0
	SSPCR0 = 0x07;
	//Podzielnik
	SSPCPSR = 150;
	//Zalacz kontroler SPI
	SSPCR1 = 2;
	// CS nieaktywny
	MMC_DIR |= MMC_CS;
	MMC_SET = MMC_CS;			// CS wysoki
}


//Nadaje i odbiera znak po SPI
static u08 spiTransferByte(u08 byte)
{
	SSPDR = byte;
	while(SSPSR & 0x10);		//Czekaj na koniec nadaw
	byte = SSPDR;
	return byte;
}

//Funkcja odczytujaca bajt w powietrze
static void flushByte(u08 count)
{
	while(count--)
		spiTransferByte(0xFF);
}


//Reset reset i uruchimienie karty mmc
u08 mmcReset(void)
{
	u16 retry;
	u08 r1=0;

	retry = 0;
	do
	{
		// Zapisz 10 pustych bajtow
		flushByte(10); 
		// Zerowanie karty 
		r1 = mmcSendCommand(MMC_GO_IDLE_STATE, 0);
		// Licznik wznowien
		retry++;
		if(retry>50) return 0xff;
	} while(r1 != 0x01);

	retry = 0;
	do
	{
		// Inicjalizacja karty
		r1 = mmcSendCommand(MMC_SEND_OP_COND, 0);
		// Licznik wznowien
		retry++;
		if(retry>500) return 0xff;
	} while(r1);
		
	// Wylacz sprawdzenie CRC
	r1 = mmcSendCommand(MMC_CRC_ON_OFF, 0);
	// Ustaw rozmiar bloku na 512 bajtow
	r1 = mmcSendCommand(MMC_SET_BLOCKLEN, 512);
	//Teraz mozna zwiekszyc predkosc SPI z 400khz do 20MHz
	//Wylacz kontroler SPI
	SSPCR1 = 0;
	//Predkosc maksymalna
	SSPCPSR = 4;
	//Zalacz kontroler SPI
	SSPCR1 = 2;
	// Wszystko ok
	return 0;
}

//Wysyla komende do karty MMC
u08 mmcSendCommand(u08 cmd, u32 arg)
{
	u08 r1;
	// Wlacz Chip Select
	MMC_CLR = MMC_CS;
	// Wyslij komende
	r1 = mmcCommand(cmd, arg);
	// Zwolnij Chip Select
	MMC_SET = MMC_CS;
	return r1;
}

/* Odczytuje sektor o numerze sector do bufora buffer
 * z karty MMC zwraca 0 gdy OK
 */
u08 mmcRead(u08* buffer,u32 sector)
{
	u08 r1;
	u16 i;
	// Wlacz CS
	MMC_CLR = MMC_CS;
	// Wyslij komende odczytu bloku
	r1 = mmcCommand(MMC_READ_SINGLE_BLOCK, sector<<9);
	// Sprawdz odpowiedz
	if(r1 != 0x00)
	{
		MMC_SET = MMC_CS;
		return r1;
	}
	// Czekaj na gotowosc karty
	while(1)
	{
		r1 = spiTransferByte(0xFF);
		if( r1 == MMC_STARTBLOCK_READ) 
			break;
		if((r1&MMC_DE_CHECK_MASK)==MMC_DE_ERROR)
		{
			MMC_SET = MMC_CS;
			return r1;
		}
	}

	// Odczytuj dane
	for(i=0; i<0x200; i++)
	{
		*buffer++ = spiTransferByte(0xFF);
	}
	// Nie sprawdzaj CRC
	flushByte(2);
	// Zwolnij CS
	MMC_SET = MMC_CS;
	// Wszystko OK
	return 0;
}

/* Zapisuje dane z bufora buffer do sektora o numerze
 * sector zwraca 0 gdy wszystko w porzadku */
u08 mmcWrite(u08* buffer,u32 sector)
{
	u08 r1;
	u16 i;
	// Uruchom CS
	MMC_CLR = MMC_CS;
	// Wyslij komende odczytu
	r1 = mmcCommand(MMC_WRITE_BLOCK, sector<<9);
	// Sprawdz odpowiedz karty
	if(r1 != 0x00)
	{
		MMC_SET = MMC_CS;
		return r1;
	}
	// Wyslij pusty bajt
	spiTransferByte(0xFF);
	// Wyslij kod rozpoczecia transferu
	spiTransferByte(MMC_STARTBLOCK_WRITE);
	// Zapisz dane z bufora
	for(i=0; i<0x200; i++)
	{
		spiTransferByte(*buffer++);
	}
	// zapisz 16-bitowy CRC - nieistotny
	spiTransferByte(0xFF);
	spiTransferByte(0xFF);
	// Sprawdz token odpowiedzi
	r1 = spiTransferByte(0xFF);
	if( (r1&MMC_DR_MASK) != MMC_DR_ACCEPT)
	{
		MMC_SET = MMC_CS;
		return r1;
	}
	// Czekaj az karta bedzie wolna
	while(!spiTransferByte(0xFF));
	// Zwolnij CS
	MMC_SET = MMC_CS;
	// Zwroc OK
	return 0;
}

//Wysyla komende do SPI 
u08 mmcCommand(u08 cmd, u32 arg)
{
	u08 r1;
	u08 retry=0;
	// wyslij komende
	spiTransferByte(cmd | 0x40);
	spiTransferByte(arg>>24);
	spiTransferByte(arg>>16);
	spiTransferByte(arg>>8);
	spiTransferByte(arg);
	spiTransferByte(0x95);	
	// czekaj na odpowiedz
	// Po osmiu probach koniec - blad
	while((r1 = spiTransferByte(0xFF)) == 0xFF)
		if(retry++ > 8) break;
	// Zwroc odpwiedz
	return r1;
}


//Zwraca calkowita pojemnosc dysku mmc
u64 mmcCapacity(void)
{
	u16 size,blen;
	u08 mult,r1;
	// wlacz CS
	MMC_CLR = MMC_CS;
	r1 = mmcCommand(MMC_SEND_CSD, 0);
	if(r1 != 0x00)
	{
		MMC_SET = MMC_CS;
		return 0;
	}
	u16 retry=0;
	while(1)
	{
		r1 = spiTransferByte(0xFF);
		if( r1 == MMC_STARTBLOCK_READ) 
			break;
		if((r1&MMC_DE_CHECK_MASK)==MMC_DE_ERROR)
		{
			MMC_SET = MMC_CS;
			return 0;
		}
		retry++;
		if(retry>500) 
		{
			MMC_SET = MMC_CS;
			return 0;
		}
	}
	flushByte(5);				// odrzuc pierwsze 6 bajtow rejestru CSD
	blen = spiTransferByte(0xff) & 0x0f;
	blen = 1<<blen;
	size = (spiTransferByte(0xff) & 3) << 10;	// najstarsze 2 bity C_SIZE
	size |= spiTransferByte(0xff) << 2;		// kolejne 8 bitow C_SIZE
	size |= (spiTransferByte(0xff) >> 6) & 3;	// najm-odsze 2 bity C_SIZE
	mult = (spiTransferByte(0xff) & 3) << 1;	// starsze 2 bity C_SIZE_MULT
	mult |= (spiTransferByte(0xff) >> 7) & 1;	// najm-odszy bit C_SIZE_MULT
	flushByte(7);				// odrzuæ resztê rejestru CSD i bajty CRC
	//Koniec transakcji MMC
	MMC_SET = MMC_CS;
	return ((u64)(size+1)<<(mult+2))*blen;		// pojemnosc
}

