#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "lpc214x.h"
#include "uart.h"
#include "mmc.h"
/***************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki *
 * Autor: Lucjan Bryndza			   *				
 * Przyklad obslugi karty MMC			   *
***************************************************/

//Bufor odbiornika
unsigned char buf[600];


/* Funkcja glowna main */
int main(void)
{	
	//Inicjalizacja UART
	Uart0Init(UART_BAUD(9600));
	//Napis zachety
	Uart0Puts("Mikrokontrolery z rdzeniem ARM - pierwsze kroki\r\n");
	Uart0Puts("Interfejs SPI\r\n");
	//Inicjalizacja karty
	mmcInit();
	if(mmcReset()!=0) 
	{
		Uart0Puts("MMC Init failed\r\n");
		return -1;
	}
	//Odczyt zerowego sektora
	if(mmcRead(buf,32)!=0) 
	{
		Uart0Puts("MMC read failed\r\n");
		return -1;
	}
	//Wyswietlenie zawartosci sektora ASCII
	Uart0Puts("Zawartosc sektora 32 karty MMC:\r\n");
	char c;
	for(int i=0;i<512;i++)
	{
		c = buf[i];
		if(isprint(c)) Uart0PutChar(c);
		else Uart0PutChar('.');
	}
	Uart0Puts("\r\n");
	return 0;
}

