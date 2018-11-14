#include "lpc214x.h"
/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *				
 * WDT Example			      	          *
 **************************************************/

#define LEDY (0xff<<16)
#define LEDDIR IO1DIR
#define LEDSET IO1SET
#define LEDCLR IO1CLR
#define KEYPIN IO0PIN
#define KEY_S1 (1<<4)

#define WDMOD_WDEN 0x1U 
#define WDMOD_WDRESET 0x2U

/* Funkcja glowna main */
int main(void)
{
	//LED0..LED7 - Wyjscie
	LEDDIR |= LEDY;
	//Ustaw preskaler ok 7s
	WDTC = 80000000;
	//Wlacz Watchog oraz reset
	WDMOD = WDMOD_WDEN | WDMOD_WDRESET;
	//Sekwencja WDTFEED zapisuje WDT
	WDFEED = 0xAA;
	WDFEED = 0x55;
	while(1)
	{
		//Czekaj
		for(volatile int i=0;i<500000;i++);
		//Wlacz D0..D7
		LEDSET = LEDY;
		//Czekaj
		for(volatile int i=0;i<500000;i++);
		//Wylacz D0..D7
		LEDCLR = LEDY;
		//Symulacja zawieszenia CPU
		if(!(KEYPIN & KEY_S1)) while(1);
		//Reset Watchdog Timer
		WDFEED = 0xAA;
		WDFEED = 0x55;
	}
	return 0;
}
