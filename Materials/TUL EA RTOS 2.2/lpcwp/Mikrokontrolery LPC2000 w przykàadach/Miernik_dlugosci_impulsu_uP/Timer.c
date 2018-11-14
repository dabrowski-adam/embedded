#include <LPC213X.H>
#include "Timer.h"
#include "lcd.h"

//Generate Interrupt 
void tc1 (void) __attribute__ ((interrupt));

//Setup the Timer Counter 1
void InitTimer1(void) 
{
	T1CCR = 7;								// Capture & interrupt on rising and falling edge on CAP1.0
    T1TCR = 0;                             	// Timer1 Disable
    VICVectAddr0 = (unsigned long)tc1;      // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 5;                // use it for Timer1 Interrupt
    VICIntEnable = 0x00000020;              // Enable Timer1 Interrupt
}

//Simple delay
void delay (unsigned int del_time) 
{ 
	unsigned int i;
	for(i = 0; i < del_time; i++);	   
}

//Timer Counter 0 Interrupt executes on each edge (rising/fallnig) on CAP1.0
void tc1(void) 
{
	unsigned int val;

	//Get time value
	val=T1CR0;

	//Checking which edge (rising/falling) caused interrupt
	//P0.10 works as GPIO
	PINSEL0&=~(3<<20);

	if(IOPIN0&(1<<10))
	{
		unsigned int tmp=T1TC;

		//Checking if overflow occured during LOW logic state
		if(tmp>val)
			T1TC=tmp-val;
		else
			T1TC=tmp+(0xFFFFFFFF-val);
	}
	else
	{
		LCDClear();
		val/=60;
		LCD32bitNumberDec(val);
		LCDText(" us");
		delay(0x100000);
		while(IOPIN0&(1<<10));
		T1TC=0;
	}

	//P0.10 works as CAP1.0
	PINSEL0&=~(3<<20);
	PINSEL0|=(1<<21);

	//Clear interrupt flag (CR0=4) & acknowledge interrupt
    T1IR = 0x10;					
    VICVectAddr = 0;
}
