#include "lpc214x.h"
#include "armint.h"

/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *	
 * Przyklad poslugiwania sie przerwaniami FIQ     *
 **************************************************/



//Definicja LEDOW
#define LEDS (0xFF<<16)
#define LEDDIR IO1DIR
#define LEDSET IO1SET
#define LEDCLR IO1CLR 
#define LEDPIN IO1PIN

//Funkcje alternatywne pinsel
#define EINT1_SEL (2<<28)
#define P014_SEL_MASK (3<<28)
#define EINT1_VIC (1<<15)

//Definicje konfiguracji przerwan
#define EXTINT_EINT1 0x02 

//Przerwanie szybkie (FIQ)
void FiqIntHandler(void) __attribute__ ((interrupt("FIQ")));

void FiqIntHandler(void)
{
	//Zmien stan LEDOW na przeciwny
	IO1PIN ^= LEDS;
	//Kasuj zrodlo przerwania
	EXTINT = EXTINT_EINT1;
}


/* Funkcja glowna main */
int main(void)
{
	//Diody LED jako wyjscie
	LEDDIR |= LEDS;
	//Wylacz LEDY
	LEDCLR = LEDS;
	//Uruchomienie na P0.14 funkcji alternatywnej INT1
	PINSEL0 &= ~P014_SEL_MASK;
	PINSEL0 |= EINT1_SEL;
	//Przerwanie zboczem
	EXTMODE |= EXTINT_EINT1;
	//Zbocze opadajace
	EXTPOLAR &= ~EXTINT_EINT1;
	//Kasuj wystapienie przerwania (1 kasuje)
	EXTINT = EXTINT_EINT1;
	//Ustawienie INT1 jako FIQ
	VICIntSelect |= EINT1_VIC;
	//Zalaczenie przerwania
	VICIntEnable = EINT1_VIC;
	//Zalacz FIQ
	enable_fiq();
	return 0;
}
