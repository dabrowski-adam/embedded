////////////////////////////////////////////////////////
// Function:	Periphelial devices clock choosing
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    


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
	//Peripherials' frequency pclk = fclk/4 = 15MHz
	//VPBDIV&=0xFFFFFFFC;
	//VPBDIV|=0x00;

	//Peripherials' frequency pclk = fclk = 60MHz
	//VPBDIV&=0xFFFFFFFC;
	//VPBDIV|=0x01;

	//Peripherials' frequency pclk = fclk/2 = 30MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x02;


	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	while(1)
	{		
		IOSET1=0x00FF0000;
		IOCLR1=0x00FF0000;
	}
}

