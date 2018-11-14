#include "lpc214x.h"
#include "CLcdDisp.h"
/***************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki *
 * Autor: Lucjan Bryndza			   *				
 * UART NO INTERRUPT				   *
***************************************************/

//Ustawienia kontrolera VIC
#define TXD0_P00_SEL (1<<0) 
#define RXD0_P01_SEL (1<<2)

//Czetotliwosc PCLK
#define PCLK 60000000

//Wyznaczenie predkosci
#define UART_BAUD(baud) (unsigned short)(PCLK/(baud*16.0) + 0.5)

#define U0LCR_8Bit_Data 3
#define U0LCR_1Bit_Stop 0
#define U0LCR_No_Parity 0
#define U0FCR_14Char_Fifo (3<<6)
#define U0FCR_FIFO_Enable 0x1
#define U0LSR_THRE 0x20
#define U0LSR_RDR 0x1 
#define U0LCR_Divisor_Latch_Access_Bit 0x80U


/* Inicjalizacja Uart0 */
static void Uart0Init(unsigned short BaudRate)
{
	//Wybor RXD i TXD jako funkcja alternatywna
	PINSEL0 |= TXD0_P00_SEL | RXD0_P01_SEL;
	//Ustawianie predkosci transmisji
	U0LCR = U0LCR_Divisor_Latch_Access_Bit;
	//Ustaw predkosci transmisji
	U0DLL = (unsigned char)BaudRate;
	U0DLM = (unsigned char)(BaudRate>>8);
	//Ustawienie 8,n,1
	U0LCR = U0LCR_8Bit_Data | U0LCR_1Bit_Stop | U0LCR_No_Parity;
	//Wlacz fifo
	U0FCR = U0FCR_FIFO_Enable |	U0FCR_14Char_Fifo;
	//Wylacz przerwania i kasuj flagi przerwan
	U0IER = 0;
	U0IIR = 0;
	//Wszystkie znaczniki odebrania znaku
	U0LSR = 0;
}

//Nadawanie znaku
static void Uart0Puts(const char *str)
{
	//Wysylaj kolejne znaki
	while(*str)
	{
		//Czekaj az bedzie mozna zapisac do bufora nadajnika
		while(!(U0LSR & U0LSR_THRE));
		//Wyslij znak i przejdz do nast znaku
		U0THR = *str++;
	}
}


//Odbior znaku
static void Uart0Gets(char *str)
{
	char c;
	do
	{
		//Czekaj na znak
		while(!(U0LSR & U0LSR_RDR));
		//Odbierz znak
		c = U0RBR;
		//Zapisz znak do bufora
		*str++ = c;
		//Czekaj na nadajnik
		while(!(U0LSR & U0LSR_THRE));
		//Nadaj zwrotnie znak
		U0THR = c;
	}
	while(c != '\r');
	*(str-1) = 0;
}


//Obiekt wyswietlacza LCD
CLcdDisp cout;
//Bufor odebranych znakow
char buf[256];

/* Funkcja glowna main */
int main(void)
{	
	Uart0Init(UART_BAUD(9600));
	Uart0Puts("Mikrokontrolery z rdzeniem ARM - pierwsze kroki\r\n");
	Uart0Puts("Uart bez systemu przerwan\r\n");
	Uart0Puts("Wpisz cos i nacisnij Enter\r\n");
	while(1)
	{
		//Pobierz linie
		Uart0Gets(buf);
		//Czy pierwszy znak to 1
		if(buf[0] == '1')
		{
			//Tak wypisz na lcd wiersz 1
			cout << pos(1,1) << (buf+1);
		}
		//Czy pierwszy znak to 2
		else if(buf[0] == '2')
		{
			//Tak wypisz na lcd wiersz 2
			cout << pos(1,2) << (buf+1);
		}
		//ani 1 ani 2
		else
		{
			//Jezeli ani 1 ani 2 to wypisz w 1 wierszu
			cout << pos(1,1) << buf;
		}
		//Wypisz potwierdzenie
		Uart0Puts("\r\nOK\r\n");
	}
	return 0;
}
