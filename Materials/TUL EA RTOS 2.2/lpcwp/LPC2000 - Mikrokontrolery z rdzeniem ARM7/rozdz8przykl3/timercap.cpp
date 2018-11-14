#include "lpc214x.h"
#include "CLcdDisp.h"

/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *				
 * Timer0 wejscie CAP 		      	          *
 **************************************************/

#define CAP01_P04_SEL (2<<8)
#define T0TCR_Counter_Reset 0x2U
#define T0TCR_Counter_Enable 0x1U
#define T0CCR_Capture_on_CAPn_1_falling_edge 0x10U 

CLcdDisp cout;

/* Funkcja glowna main */
int main(void)
{
	//Licznik zlicza impulsy z czestotliwoscia 15Hz
	T0PR = 3999999;
	//Cature na zbocze opadajace 
	T0CCR |= T0CCR_Capture_on_CAPn_1_falling_edge;
	//Zeruj licznik i preskaler
	T0TCR = T0TCR_Counter_Reset;
	//Zalacz licznik T0
	T0TCR = T0TCR_Counter_Enable;
	//Linia P0.4 jako CAP0.1
	PINSEL0 |= CAP01_P04_SEL;
	cout << pos(1,1) << "Timer=";
	cout << pos(1,2) << "Cap=";
	while(1)
	{
		cout << pos(7,1) << (unsigned int)T0TC << "   ";
		cout << pos(7,2) << (unsigned int)T0CR1 << "   ";
	}
	return 0;
}
