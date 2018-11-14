////////////////////////////////////////////////////////
// Function:	PLL testing example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC-4.0.1 (www.gnuarm.com)
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>
#include "Timer.h"


//*******************************
// MAIN
//*******************************
int main(void) 
{
	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;
	
	//P1.16..23 defined as outputs
	IODIR1 = 0x00FF0000;

	//P0.22 toggled by Timer0
	PINSEL1|=(3<<12);

	//inits
	InitTimer0();

	while(1);
}
