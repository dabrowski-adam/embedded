////////////////////////////////////////////////////////
// Function:	Function Generator
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"
#include "delay.h"
#include "Timer.h"
#include "SignalType.h"
#include "switches_pins.h"
#include "UserSettings.h"


//*******************************
// MAIN
//*******************************
int main(void)
{
	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	//DAC pin configuration P0.25 = CA
	PINSEL1 &= ~(1<<18);
	PINSEL1 |= (1<<19);

	LCDInit();
	InitTimer0();

	LCDClear();

	LCDText("Function");
	LCDGotoXY(0,1);
	LCDText("Generator");
	delay(0x800000);

	//Sine wave A=1V, C=1V, F=1kHz
	SetSignalType(stSinus);
	SetAmplitude(128);
	SetConstValue(310);
	SetFrequency(10);

	//Enable timer
	T0TC=0;
	T0TCR=1;

	while(1)
	{
		LCDGotoXY(0,0);
		LCDText("S1:Wave S2:A    ");
		LCDGotoXY(0,1);
		LCDText("S3:C S4:F S5:D  ");

		if(SW1==0)
			ChangeSignalType();
		else if(SW2==0)
			ChangeAmplitude();
		else if(SW3==0)
			ChangeConstValue();
		else if(SW4==0)
			ChangeFrequency();
		else if(SW5==0)
			ChangeDuty();

		SW_DELAY;
	}
}
