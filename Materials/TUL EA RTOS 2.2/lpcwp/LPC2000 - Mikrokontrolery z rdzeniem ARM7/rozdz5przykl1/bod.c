#include "lpc214x.h"

//Definicja LEDOW
#define LEDS (0xFF<<16)
#define LEDDIR IO1DIR
#define LEDPIN IO1PIN





//Funkcja glowna main
int main(void)
{
	LEDDIR = LEDS;			//Ledy jako wyjscie
	unsigned char r=0;
	while(1)
	{
		if(RSID & 0x01) RSID |= 0x0F;
		r = (RSID & 0x0f)<<4;		
		if(VICRawIntr & (1<<20) ) r |= 1;
		else r |= 2;
		LEDPIN = ((unsigned int)r)<<16;
	}
}
