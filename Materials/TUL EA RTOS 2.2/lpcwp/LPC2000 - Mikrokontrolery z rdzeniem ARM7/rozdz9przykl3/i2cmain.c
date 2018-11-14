#include "lpc214x.h"
#include "uart.h"
#include <stdlib.h>
#include <string.h>
/***************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki *
 * Autor: Lucjan Bryndza			   *				
 * Przyklad obslugi I2C				   *
***************************************************/

#define I2C0CONSET_STA 0x20
#define I2C0CONSET_AA 0x4
#define I2C0CONCLR_SIC 0x8
#define I2C0CONCLR_STAC 0x20
#define I2C0CONCLR_AAC 0x4
#define I2C0CONSET_I2EN 0x40
#define I2C0CONSET_STO 0x10


//Ustawienia kontrolera VIC
#define SCL0_P02_SEL (1<<4) 
#define SDA0_P03_SEL (1<<6)

//Adres urzadzenia na magistrali I2C
#define I2C_MEMADDR 0xA0


//Bufor odbiornika
char buf[128];


/* Inicjalizuje interfejs I2C pamiec AT24C128*/
void EepromInit(void)
{
	PINSEL0 |= SCL0_P02_SEL | SDA0_P03_SEL;
	//FI2C = PCLK/(SCLL+SCLH) - 100KHz
	I2C0SCLL = 300;
	I2C0SCLH = 300;
	//Wyzeruj wszystkie flagi
	I2C0CONCLR = 0x6C;
	//Wlacz interfejs I2C - MASTER
	I2C0CONSET = I2C0CONSET_I2EN;
}


/* Zapisuje komorke pamieci AT24C128 
 * addr - adres komorki pamieci do zapisania
 * val - vartosc liczbowa do zapisania 
 * Zwraca 0 dla sukces lub wartosc ujemna dla bledu 
 */
int EepromWrite(unsigned short addr,unsigned char val)
{
	int tmp;
	//Licznik nadanych bajtow
	int cnt = 0;
	//Rozpocznij nadawanie bitu start
	I2C0CONSET = I2C0CONSET_STA;
	//Petla oczekiwania
	while(1)
	{
		//Czekaj na zdarzenia
		while(I2C0STAT==0xF8);
		//Status magistrali
		switch(I2C0STAT)
		{
		//Bit start zostal nadany
		case 0x08:
			//Wyslij adres pamieci I2C
			I2C0DAT = I2C_MEMADDR;
			I2C0CONSET = I2C0CONSET_AA;
			I2C0CONCLR = I2C0CONCLR_SIC|I2C0CONCLR_STAC;
			break;
		//Adres I2C zostal nadany
		case 0x18:
			//Wyslij starszy bajt adresu
			I2C0DAT = addr>>8;
			I2C0CONSET = I2C0CONSET_AA;
			I2C0CONCLR = I2C0CONCLR_SIC;
			cnt=0;
			break;
		//Dane zostaly nadane
		case 0x28:
			//Pierwszy raz 
			if(cnt==0) 
			{
				//Mlodsza czesc adresu
				I2C0DAT = addr;
				I2C0CONSET = I2C0CONSET_AA;
				I2C0CONCLR = I2C0CONCLR_SIC;
			}
			//Drugi raz
			else if(cnt==1)
			{
				//Dana do umiesczenia w komorce pamieci
				I2C0DAT = val;
				I2C0CONSET = I2C0CONSET_AA;
				I2C0CONCLR = I2C0CONCLR_SIC;
			}
			//Trzeci raz
			else if(cnt==2)
			{
				//Wyslij STOP
				I2C0CONSET = I2C0CONSET_AA|I2C0CONSET_STO;
				I2C0CONCLR = I2C0CONCLR_SIC;
				return 0;
			}
			cnt++;
			break;
		//Jezeli blad zatrzymaj i wyjdz
		default:
			tmp = I2C0STAT;
			I2C0CONSET = I2C0CONSET_AA|I2C0CONSET_STO;
			I2C0CONCLR = I2C0CONCLR_SIC;
			return -tmp;
		}
	}
}

/* Odczytuje komorke pamieci AT24C128 
 * addr - adres komorki pamieci
 * Zwraca zawartosc komorki gdy OK w przypadku bledu
 * zwraca wartosc mniejsza od zera
 */
int EepromRead(unsigned short addr)
{
	int tmp;
	int cnt = 0;
	//Transmit Start BIT
	I2C0CONSET = I2C0CONSET_STA;
	//Petla oczekiwania
	while(1)
	{
		//Czekaj na zdarzenie
		while(I2C0STAT==0xF8);
		//Okresl rodzaj
		switch(I2C0STAT)
		{
		//Bit startu nadany
		case 0x08:
			//Wyslij Adres pamieci I2C
			I2C0DAT = I2C_MEMADDR;
			I2C0CONSET = I2C0CONSET_AA;
			I2C0CONCLR = I2C0CONCLR_SIC|I2C0CONCLR_STAC;
			break;
		//Adres zostal nadany
		case 0x18:
			//Wyslij starsza czesc adresu
			I2C0DAT = addr>>8;
			I2C0CONSET = I2C0CONSET_AA;
			I2C0CONCLR = I2C0CONCLR_SIC;
			cnt=0;
			break;
		//Dane zostaly nadane
		case 0x28:
			//Pierwszy raz
			if(cnt==0) 
			{
				//Wyslij mlodsza czesc adresu
				I2C0DAT = addr;
				I2C0CONSET = I2C0CONSET_AA;
				I2C0CONCLR = I2C0CONCLR_SIC;
			}
			else
			{ 
				//Wyslij ponowny bit startu
				I2C0CONSET = I2C0CONSET_STA;
				I2C0CONCLR = I2C0CONCLR_SIC;
			}
			cnt++;
			break;
		//Kolejny bit startu zostal nadany
		case 0x10:
			//Nadaj adres I2C w trybie read
			I2C0DAT = I2C_MEMADDR|1;
			I2C0CONSET = I2C0CONSET_AA;
			I2C0CONCLR = I2C0CONCLR_SIC|I2C0CONCLR_STAC;
			break;		
		//Wyslano adres pomyslnie
		case 0x40:
			//Rozpocznik odbieranie danej
			I2C0CONCLR = I2C0CONCLR_SIC|I2C0CONCLR_AAC;
			break;
		//Odebrano dana - pierwsza i ostatnia
		case 0x58:
			//Wyslij stop
			tmp = I2C0DAT;
			I2C0CONSET = I2C0CONSET_AA|I2C0CONSET_STO;		
			I2C0CONCLR = I2C0CONCLR_SIC;
			return tmp;
		//Jezeli blad
		default:
			//Zatrzymaj i wyjdz
			tmp = I2C0STAT;
			I2C0CONSET = I2C0CONSET_AA|I2C0CONSET_STO;
			I2C0CONCLR = I2C0CONCLR_SIC;
			return -tmp;
		}
	}
}



/* Funkcja glowna main */
int main(void)
{	
	//Inicjalizacja UART
	Uart0Init(UART_BAUD(9600));
	//Inicjalizacja obslugi EEROM
	EepromInit();
	//Napis zachety
	Uart0Puts("Mikrokontrolery z rdzeniem ARM - pierwsze kroki\r\n");
	Uart0Puts("Pamiec EEPROM\r\n");
	Uart0Puts("Wpisz write=tekst lub read\r\n");

	char *wr,*rd;
	while(1)
	{
		//Pobierz jedna linie z terminala
		Uart0Gets(buf);
		//Sprawdz jaka komenda
		wr=strstr(buf,"write=");
		rd=strstr(buf,"read");
		//Zadna z powyzszych
		if(wr==NULL && rd==NULL)
		{
			Uart0Puts("Unknown command\r\n");
			continue;
		}
		//Adres w pamieci I2c
		unsigned short adr = 0;
		//Odczyt danych
		if(rd!=NULL)
		{
			//Komenda odczytu danych z I2C
			Uart0Puts("String: ");
			int c;
			//Odczytuj kolejne bajty i wyslij do UART
			do
			{
				c = EepromRead(adr++);
				if(c<0)
				{
					Uart0Puts("*EEPROM read error*");
					break;
				}
				Uart0PutChar(c);
			}
			while(c);
			Uart0Puts("\r\n");
		}
		//Komenda zapisu danych
		if(wr!=NULL)
		{
			wr+= 6;
			//Zapisuj kolejne adresy
			while(*wr)
			{
				if(EepromWrite(adr++,*wr++)<0)
				{
					Uart0Puts("\r\n*EEPROM write error*\r\n");
					break;
				}
				//Czekaj na zapisanie
				for(volatile int i=0;i<100000;i++);
			}
			//Zapisz koniec lancucha
			if(EepromWrite(adr++,0)<0)
			{
				Uart0Puts("\r\n*EEPROM write error*\r\n");
			}
			else
			{
				Uart0Puts("\r\n*EEPROM write ok\r\n");
			}
		}
	}
	return 0;
}
