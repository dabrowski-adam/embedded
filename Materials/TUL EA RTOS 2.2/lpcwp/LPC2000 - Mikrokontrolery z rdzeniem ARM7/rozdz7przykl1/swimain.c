#include "lpc214x.h"

//Definicja LEDOW
#define LEDS (0xFF<<16)
#define LEDDIR IO1DIR
#define LEDSET IO1SET
#define LEDCLR IO1CLR 

//Deklaracja funkcji przerwania SWI
void SwiIntHandler(void) __attribute__ ((interrupt("SWI")));
//Funkcja przerwania SWI
void SwiIntHandler(void) 
{
	//Rejestr R14-1 zawiera adres instrukcji SWI
	register unsigned int* link_ptr asm ("r14");
	//Rejestr R0 zawiera parametr przekazany do SWI
	register unsigned int param asm ("r0");
	param &= 0xff;
	param <<= 16;
	switch(*(link_ptr-1) & 0x00FFFFFF)
	{
	//Zalacz Ledy
	case 0x01:
		LEDSET = param;
		break;
	//Wylacz Ledy
	case 0x02:
	    LEDCLR = param;
		break;
	}
}

//Funkcja wlaczajaca LEDY poprzez SWI
static inline void SwiLedOn(unsigned int LedSt)
{
	asm volatile 
	(
		"mov r0,%[ledon]\n"
		"swi #1\n"
		::[ledon]"r"(LedSt):"r0"
	);
}

//Funkcja wylaczajaca LEDY poprzez SWI
static inline void SwiLedOff(unsigned int LedSt)
{
	asm volatile 
	(
		"mov r0,%[ledon]\n"
		"swi #2\n"
		::[ledon]"r"(LedSt):"r0"
	);
}


/* Funkcja glowna main */
int main(void)
{
	//Ledy jako wyjscie
	LEDDIR |= LEDS;
	//Petla nieskonczona
	while(1)
	{
		//Wlacz D7,D4,D1,D0
		SwiLedOn(0x93);
		//Czekaj
		for(volatile int i=0;i<2000000;i++);
		//Wylacz D7,D4,D1,D0
		SwiLedOff(0x93);
		//Czekaj
		for(volatile int i=0;i<2000000;i++);
	}
	return 0;
}
