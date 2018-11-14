////////////////////////////////////////////////////////
// Function:	Switch example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
                  
#include <LPC213x.H>
#include "switches_pins.h"

#define SW_DELAY	delay(0x8000)

//*******************************
// Delay function
//*******************************
void delay(unsigned int del_time) 
{ 
	unsigned int i;
	for(i=0;i<del_time;i++);	   
}								    							    

//*******************************
// Wait for switch SW1 pressed
//*******************************
void WaitForSwitchDown(void)
{
	while(1)
	{
		while(SW1!=0)
			SW_DELAY;

		SW_DELAY;
		if(SW1==0)
			return;
	}
}

//*******************************
// Wait for switch SW1 released
//*******************************
void WaitForSwitchUp(void)
{
	while(1)
	{
		while(SW1==0)
			SW_DELAY;

		SW_DELAY;
		if(SW1!=0)
			return;
	}
}

//*******************************
// MAIN
//*******************************
int main (void)
{
	//switch press counter
	int cou;

	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;

	//Port direction configuration
	IODIR0 = 0xFF000000; 
	IODIR1 = 0x00FF0000;

	cou=0;
	IOCLR1=0x00FF0000;
	while(1)
	{
		WaitForSwitchDown();
		IOPIN1&=~0x00FF0000;
		IOPIN1|=(++cou)<<16;
		WaitForSwitchUp();
	}
}
