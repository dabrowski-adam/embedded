////////////////////////////////////////////////////////
// Function:	LED example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
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
// Beep cca. 1kHz
// msDuration - duration [miliseconds]
//*******************************
void Beep(unsigned int msDuration)
{
	unsigned int i;
	for(i=0;i<msDuration;i++)
	{
		IOSET0=0x04000000;
		delay(0xAAA);
		IOCLR0=0x04000000;
		delay(0xAAA);
	}
}

//*******************************
// MAIN
//*******************************
int main (void)
{
	//counter
	unsigned int i;

	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;

	//Port direction configuration
	IODIR0 = 0xFF000000; 
	IODIR1 = 0x00FF0000;

	IOCLR1=0x00FF0000;
	delay(0x400000);
	while(1)
	{
		for(i=0;i<3;i++)
		{
			IOSET1=0x00FF0000;
			delay(0x400000);

			IOCLR1=0x00FF0000;
			delay(0x400000);
		}
		for(i=0;i<3;i++)
		{
			IOPIN1=0x00AA0000;
			delay(0x400000);

			IOPIN1=0x00550000;
			delay(0x400000);
		}
		IOCLR1=0x00FF0000;
		delay(0x400000);

		//Make some noise & blinking light
		IOPIN1=0x00180000;
		Beep(1000);
		IOCLR1=0x00FF0000;
		delay(0x100000);

		IOPIN1=0x003C0000;
		Beep(300);
		IOCLR1=0x00FF0000;
		delay(0x100000);

		IOPIN1=0x007E0000;
		Beep(300);
		IOCLR1=0x00FF0000;
		delay(0x200000);
	}
}

