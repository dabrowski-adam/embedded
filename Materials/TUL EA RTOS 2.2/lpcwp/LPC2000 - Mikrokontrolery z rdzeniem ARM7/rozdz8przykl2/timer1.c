#include "lpc214x.h"

/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *				
 * Timer0 fala prostokatna		          *
**************************************************/

#define MAT00_P022_SEL (3<<12)
#define T0EM0 1
#define T0EMC0_TOGGLE (3<<4)

#define T0TCR_Counter_Reset 0x2U 
#define T0TCR_Counter_Enable 0x1U
#define T0MCR_Reset_on_MR0 0x2U 


/* Funkcja glowna main */
int main(void)
{
	
	//Licznik zlicza impulsy z czestotliwoscia 15Hz
	T0PR = 3999999;
	//Gdy warunek spelniony zeruj Timer
	T0MCR |= T0MCR_Reset_on_MR0;
	//Przeladowanie licznika co 1s
	T0MR0 = 15;
	//Zeruj licznik i preskaler
	T0TCR = T0TCR_Counter_Reset;
	//Zalacz licznik T0
	T0TCR = T0TCR_Counter_Enable;
	//Wybierz funkcje MAT0.0 dla P0.22
	PINSEL1 |= MAT00_P022_SEL;
	//Ustaw zmiane stanu przy zdarzeniu MATCH
	T0EMR |= T0EM0 | T0EMC0_TOGGLE;
	return 0;
}
