////////////////////////////////////////////////////////
// Function:	Pulse (logic 1/HIGH) width meter  ___-----___
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
	InitTimer1();

	LCDClear();

	LCDText("Pulse Duration");
	LCDGotoXY(0,1);
	LCDText("Meter");

	delay(0x800000);

	//P0.10 works as CAP1.0
	PINSEL0&=~(3<<20);
	PINSEL0|=(1<<21);

	//Enable Timer1
	T1TC=0;
	T1TCR=1;

	while(1);
}
/////////////////////////////////////////////////////////////
