/**************************************************
 * Mikrokontrolery z rdzeniem ARM a-              *
 * Autor: Lucjan Bryndza			  *				
 * Bardzo prosty program pokazujacy prace z       *
 * portami IO dla zestawu ZL6ARM	          *
**************************************************/


#include <stdio.h>
#include "lpc214x.h"



#define LEDDIR FIO1DIR	//Rejestr kierunku
#define LEDSET FIO1SET	//Rejestr ustawiaj
#define LEDCLR FIO1CLR	//Rejestr kasujacy
#define LEDPIN FIO1PIN	//Rejestr portu

#define KEYDIR FIO0DIR	//Rejestr kierunku
#define KEYPIN FIO0PIN	//Rejestr portu

#define LEDY (0xFF<<16)
#define LED0 (1<<16)	//P1.16 - Dioda LED0
#define LED1 (2<<16)	//P1.17	- Dioda LED1
#define LED2 (4<<16)	//P1.18	- Dioda LED2

#define S1 0x10			//P0.4 - Klawisz S1
#define S2 0x20			//P0.5 - Klawisz S2
#define S3 0x40			//P0.6 - Klawisz S3

#define SCS_GPIO0M_BIT 0x01
#define SCS_GPIO1M_BIT 0x02

//Funkcja glowna main
int main(void)
{
	int pkey = 0;
	int key;
	//Uzyj fast GPIO
	SCS = SCS_GPIO0M_BIT | SCS_GPIO1M_BIT;
	//Kierunek dla ledow wyjscie
	LEDDIR |= LEDY;
	//Kierunek dla klawiszy wejscie
	KEYDIR &= ~(S1|S2|S3);
	//Petla glowna programu
	while(1)
	{
		if(!(KEYPIN & S1)) 
		{
			//!Jezeli wcisniety S1 to zalacz LED0 i wylacz LED1
			LEDSET = LED0;
			LEDCLR = LED1;
		}
		else if(!(KEYPIN & S2))
		{
			//!Jezeli wcisniety S2 to wylacz LED0 i zalacz LED2
			LEDSET = LED1;
			LEDCLR = LED0;
		}
		//Jezeli zbocze opadajace na S3 to zmien stan LED2
		key = KEYPIN & S3;
		if(pkey && !key)
		{
			LEDPIN ^= LED2;
		}
		pkey = key;
	}
			
}
