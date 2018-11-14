/**************************************************
 * Mikrokontrolery z rdzeniem ARM - kurs czesc IV *
 * Autor: Lucjan Bryndza						  *				
 * Bardzo prosty program pokazujacy prace z       *
 * portami IO dla zestawu ZL6ARM			      *
**************************************************/


#include <stdio.h>
#include "lpc214x.h"
#include "clcddisp.h"


CLcdDisp cout;


//Funkcja glowna main
int main(void)
{
	cout << "ARM w BTC!";
	cout << pos(1,2) << "IO0PIN=";
	unsigned int sk; 
	while(1)
	{
		sk = (~IO0PIN >> 4) & 0x0f;
		cout << pos(8,2)<< sk << " ";
	}	
}
