////////////////////////////////////////////////////////
// Function:	Data swap between ISR and main example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

//Globals
/*unsigned int fInterruptOccured=0;*/
volatile unsigned int fInterruptOccured=0;

//Timer0 match interrupt handler's header
void tc0(void) __attribute__ ((interrupt("IRQ")));

//*******************************
// Timer0 Init
//*******************************
void InitTimer0(void) 
{
    T0MR0 = 7499999;                  		// 0.5s = 7.5*10^6-1 counts
    T0MCR = 3;                             	// Interrupt and Reset on MR0
    T0TCR = 1;                             	// Timer0 enable
    VICVectAddr0 = (unsigned long)tc0;      // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                // use it for Timer 0 Interrupt
    VICIntEnable = 0x00000010;              // Enable Timer0 Interrupt
}

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
	//Peripherials' frequency = fclk/4 = 15MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x00;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();
	LCDClear();
	LCDText("ISR data swap");

	InitTimer0();

	while(1)
		if(fInterruptOccured!=0)
		{
			fInterruptOccured=0;

			//Toggle LEDs 4...7
			IOPIN1^=0x00F00000;
		}
}

//*******************************
// Timer0 match interrupt routine
//*******************************
void tc0(void) 
{
	//Toggle LEDs 0...3
	IOPIN1^=0x000F0000;

	//Set interrupt occured flag
	fInterruptOccured=1;

	//Clear interrupt flag & acknowledge interrupt
    T0IR = 1;					
    VICVectAddr = 0;
}
