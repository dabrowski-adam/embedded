#include <LPC213X.H>
#include "Timer.h"
#include "lcd.h"

#define T_1000MS 59999999
#define T_300MS  17999999
#define T_100MS  5999999

//Generate Interrupt 
void tc0 (void) __attribute__ ((interrupt));

//Setup the Timer Counter 0
void InitTimer0(void) 
{
    T0MR0 = T_1000MS;                  		// 1s = 60*10^6-1 counts
    T0MCR = 3;                             	// Interrupt and Reset on MR0
    T0TCR = 0;                             	// Timer0 Disable
    VICVectAddr0 = (unsigned long)tc0;      // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                // use it for Timer 0 Interrupt
    VICIntEnable = 0x00000010;              // Enable Timer0 Interrupt
}

//Setup the Timer Counter 1
void InitTimer1(void) 
{
	T1CTCR = 2;								// Counter mode, falling edge on CAP1.0
    T1TCR = 0;                             	// Timer0 Disable
}


//Timer Counter 0 Interrupt executes each 1s @ 60 MHz CPU Clock
void tc0(void) 
{
	unsigned int freq;

	freq=T1TC;
	T1TC=0;

	if(T0MR0==T_1000MS)
	{
		LCDClear();
		LCD32bitNumberDec(freq);
		LCDText(" Hz");

		T0MR0=T_300MS;   //0.3s
	}
	else
	{
		LCDGotoXY(0,1);
		LCDText("C");

		T0MR0=T_1000MS;  //1s
	}

	//Clear interrupt flag & acknowledge interrupt
    T0IR = 1;					
    VICVectAddr = 0;
}
