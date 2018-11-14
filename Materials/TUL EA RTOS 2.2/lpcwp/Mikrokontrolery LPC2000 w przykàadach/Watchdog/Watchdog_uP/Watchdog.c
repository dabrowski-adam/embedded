////////////////////////////////////////////////////////
// Function:	Watchdog example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"


//*******************************
// Delay function
//*******************************
void delay(unsigned int del_time) 
{ 
	unsigned int i;
	for(i=0;i<del_time;i++);	   
}

//*******************************
// MAIN
//*******************************
int main (void)
{
	unsigned int count=0;

	//Peripherials' frequency = fclk/4 = 15MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x00;

	//Clear WDTOF flag
	WDMOD&=~(1<<2);

	//Configure watchdog
	//cca. 2s watchdog timeout
	WDTC=0x800000;

	//Operate with watchdog enabled
	WDMOD|=0x03;

	//Start watchdog
	WDFEED=0xAA;
	WDFEED=0x55;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();
	LCDClear();
	LCDText("Reset");
	delay(0x800000);
	LCDClear();
	LCDText("Watchdog");

	while(1)
	{		
		delay(0x200000);
		IOPIN1&=~0x00FF0000;
		IOPIN1|=(++count)<<16;

		//Feed watchdog
		//WDFEED=0xAA;
		//WDFEED=0x55;
	}
}

