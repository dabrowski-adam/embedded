////////////////////////////////////////////////////////
// Function:	Division procedure
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

extern unsigned int div(unsigned int, unsigned int);

//*******************************
// Delay function
//*******************************
void delay (unsigned int del_time) 
{ 
	unsigned int i;
	for(i = 0; i < del_time; i++);	   
}

//*******************************
// MAIN
//*******************************
int main (void)
{
	unsigned int a,b,result;

	//Peripherials' frequency = fclk/4 = 15MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x00;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();

	LCDClear();

	LCDText("Division");
	delay(0x800000);

	a=1412;
	b=45;

	result=div(a,b);

	LCDClear();
	LCDText("a=");
	LCD32bitNumberDec(a);
	LCDText(" b=");
	LCD32bitNumberDec(b);
	LCDGotoXY(0,1);
	LCDText("a/b=");
	LCD32bitNumberDec(result);

	while(1);
}
/////////////////////////////////////////////////////////////
