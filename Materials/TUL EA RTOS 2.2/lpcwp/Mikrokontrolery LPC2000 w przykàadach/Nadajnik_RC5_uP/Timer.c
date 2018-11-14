#include <LPC213X.H>
#include "Timer.h"

//Setup the Timer/Counter0
void InitTimer0(void) 
{
    T0MR0 = 832;                  			// 13,88us = 833-1 counts (36kHz)
    T0MCR = 2;                             	// Reset on MR0
    T0TCR = 0;                             	// Timer0 Disable
	T0EMR = (1<<0)|(3<<4);					// Toggle MAT0.0 on MR0
}

//Setup the Timer/Counter1
void InitTimer1(void) 
{
    T1MR0 = 106680;                  		// 2*889us
    T1MCR = 4;                             	// Stop on MR0
    T1TCR = 0;                             	// Timer0 Disable
}

