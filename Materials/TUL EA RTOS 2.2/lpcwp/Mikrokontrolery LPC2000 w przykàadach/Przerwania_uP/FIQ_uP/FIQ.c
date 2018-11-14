////////////////////////////////////////////////////////
// Function:	FIQ interrupt example - EINT2 interrupt
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

//EINT2 interrupt handler's header
void EINT2ISR(void) __attribute__ ((interrupt("FIQ")));

//*******************************
// EINT2 Init
//*******************************
void EINT2Init(void)
{
	//P0.7=EINT2
	PINSEL0|=(3<<14);
	
	//EXT4 edge trigered
	EXTMODE=(1<<2);
	
	//Falling edge
	EXTPOLAR=0;

	//Setting VIC for FIQ interrupt
	VICIntSelect = 0x00010000;
	VICIntEnable |= 0x00010000;
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

	EINT2Init();
	LCDInit();

	LCDClear();

	LCDText("FIQ example");
	LCDGotoXY(0,1);
	LCDText("Press S4");

	while(1);
}

//*******************************
// EINT2 interrupt routine
//*******************************
void EINT2ISR(void)
{
	LCDClear();
	LCDText("FIQ Interrupt!!!");
	delay(0x800000);
	LCDClear();

	//Clear EINT2 interrupt flag
	EXTINT=(1<<2);
}
