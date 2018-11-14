////////////////////////////////////////////////////////
// Function:	ADC example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

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
	//Peripherials' frequency = fclk/4 = 15MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x00;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	//ADC0 configuration
	PINSEL1 |= (1<<24);			// P0.28 = AD0.1
	AD0CR = 0x002E0D02;			// AC: 10 bit AD0.1 @ 1MHz	

	LCDInit();

	LCDClear();

	LCDText("ADC example");

	delay(0x800000);

	while(1)
	{
		unsigned int adc_val;

		AD0CR |= 0x01000000;					//start A/D conversion
		do
		{
			adc_val = AD0DR;					//read result
		}while((adc_val & 0x80000000)==0);		//wait until conversion finished
		AD0CR &= ~0x01000000;					//finish A/D conversion
		adc_val=(adc_val>>6)&0x03FF;

		//0,0 -> Go Home
		LCDGotoXY(0,0);
		LCDText("N = ");
		LCD32bitNumberDec(adc_val);
		adc_val=(adc_val*330)/1023;
		LCDText("           ");

		LCDGotoXY(0,1);
		LCDText("V = ");
		LCD32bitNumberDec(adc_val/100);
		LCDText(".");
		LCD32bitNumberDec((adc_val/10)%10);
		LCD32bitNumberDec(adc_val%10);
		LCDText("V");
		LCDText("       ");

		//Some delay
		delay(0x200000);
	}
}
/////////////////////////////////////////////////////////////
