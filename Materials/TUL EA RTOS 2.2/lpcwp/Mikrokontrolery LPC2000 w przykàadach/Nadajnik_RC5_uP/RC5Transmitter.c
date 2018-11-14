////////////////////////////////////////////////////////
// Function:	RC5 transmitter
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
                  
#include <LPC213x.H>
#include "RC5_TX.h"
#include "switches_pins.h"

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
	//Init RC5 transmitter
	InitRC5_TX();

	while(1)
	{
		if(SW1==0)
			SendRC5(0,0);
		else if(SW2==0)
			SendRC5(0,1);
		else if(SW3==0)
			SendRC5(0,2);
		else if(SW4==0)
			SendRC5(0,3);

		while(!SW1 | !SW2 |!SW3 |!SW4)
			delay(0x4000);

		delay(0x4000);
	}
}
