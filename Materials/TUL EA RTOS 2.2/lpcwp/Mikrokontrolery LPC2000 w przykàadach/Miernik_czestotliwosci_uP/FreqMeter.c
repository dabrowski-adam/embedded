////////////////////////////////////////////////////////
// Function:	Frequency meter
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"
#include "Timer.h"

//*******************************
// MAIN
//*******************************
int main (void)
{
	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();
	InitTimer0();
	InitTimer1();

	LCDClear();

	LCDText("Frequency");
	LCDGotoXY(0,1);
	LCDText("Meter");

	//P0.10 works as CAP1.0
	PINSEL0&=~(3<<20);
	PINSEL0|=(1<<21);

	//Enable timers
	T1TC=0;
	T0TC=0;
	T0TCR=1;
	T1TCR=1;

	while(1);
}
