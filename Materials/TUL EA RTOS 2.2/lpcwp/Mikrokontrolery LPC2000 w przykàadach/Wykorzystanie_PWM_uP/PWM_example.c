////////////////////////////////////////////////////////
// Function:	PWM example 
//                - LCD backlight brightness modulation
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC-4.0.1 (www.gnuarm.com)
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"
#include "switches_pins.h"

//Macros
#define SW_DELAY	delay(0x2000)
#define PWM_PERIOD	15000
#define PWM_STEP	(PWM_PERIOD/100)

//Globals


//*******************************
// PWM Init
//*******************************
void PWMInit(void)
{
	//P0.8=PWM4
	PINSEL0&=~(3<<16);
	PINSEL0|=(1<<17);

	//Prescaler = 0, PWMTC increments every 1 VPB cycle
	PWMPR=0x00;

	//Enable PWM4 channel and set single edge mode
	PWMPCR=(1<<12); 

	//Set PWM period = 1ms @ VPBfreq = 15MHz
	PWMMR0 = PWM_PERIOD;

	//Initially R=50%
	PWMMR4 = PWM_PERIOD/2;

	//Reset PWMTC (counter reg.) on match with PWMMR0
	PWMMCR=0x02;

	//Latch PWMMR4
	PWMLER=(1<<4);

	//Reset PWMTC
	PWMTCR=0x02;

	//Counter Enable, PWM Mode Enable
	PWMTCR=0x09;
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
// Display PWM ratio
//*******************************
void DisplayRatio(void)
{
	unsigned int tmp;

	LCDGotoXY(0,0);
	LCDText("S1+  S2-        ");
	LCDGotoXY(0,1);
	LCDText("R = ");

	tmp=(1000*PWMMR4)/PWM_PERIOD;

	LCD32bitNumberDec(tmp/10);
	LCDText(".");
	LCD32bitNumberDec(tmp%10);
	LCDText("%               ");
}

//*******************************
// MAIN
//*******************************
int main(void)
{
	//Peripherials' frequency = fclk/4 = 15MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x00;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();

	LCDClear();

	LCDText("PWM example");
	LCDGotoXY(0,1);
	LCDText("S1+  S2-");
	delay(0x1000000);

	PWMInit();

	DisplayRatio();

	//Loop forever
	while(1)
	{
		if(SW1==0)
		{
			if(PWMMR4<(PWM_PERIOD-PWM_STEP))
			{
				PWMMR4+=PWM_STEP;
				PWMLER=(1<<4);
				DisplayRatio();
			}
		}
		else if(SW2==0)
		{
			if(PWMMR4>PWM_STEP)
			{
				PWMMR4-=PWM_STEP;
				PWMLER=(1<<4);
				DisplayRatio();
			}
		}

		if(!SW1|!SW2)
			delay(0x80000);

		SW_DELAY;
	}
}
