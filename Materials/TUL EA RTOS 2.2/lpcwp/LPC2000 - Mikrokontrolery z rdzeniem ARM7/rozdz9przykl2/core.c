#include "lpc214x.h"
#include "uart.h"
#include <stdlib.h>
#include <string.h>
/***************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki *
 * Autor: Lucjan Bryndza			   *	
 * UART INTERRUPT			           *
***************************************************/



//Definicja portu LEDOW
#define LEDS (0xFF<<16)
#define LEDDIR IO1DIR
#define LEDSET IO1SET
#define LEDCLR IO1CLR
#define LEDPIN IO1PIN

//Bufor odbiornika
char buf[128];


/* Funkcja glowna main */
int main(void)
{	
	//Inicjalizacja UART
	Uart0Init(UART_BAUD(9600));
	//Napis zachety
	Uart0Puts("Mikrokontrolery z rdzeniem ARM cz VIII\r\n");
	Uart0Puts("Uart z systemem przerwan\r\n");
	Uart0Puts("Wpisz SET=n np. SET=254\r\n");
	//Ledy jako wyjscie
	LEDDIR |= LEDS;
	//Wylacz diody LED
	LEDCLR = LEDS;
	while(1)
	{
		//Pobierz jedna linie z terminala
		Uart0Gets(buf);
		//Sprawdzenie komendy set
		if(strstr(buf,"SET="))
		{
			//Zamien argument na liczbe
			int val = atoi(strchr(buf,'=')+1);
			//Czy liczba miesci sie w zakresie 0-255
			if(val>=0 && val<=255) 
			{
				//Ustaw stan ledow
				LEDPIN = val << 16;
				//Wypisz potwierdzenie ERROR
				Uart0Puts("\r\nLEDSET OK\r\n");			
			}
			else
			{
				//Blad wartosci
				Uart0Puts("\r\nVALUE ERROR\r\n");			
			}
		}
		else
		{
			//Bledny rozkaz
			Uart0Puts("\r\nCOMMAND ERROR\r\n");
		}
	}
	return 0;
}
