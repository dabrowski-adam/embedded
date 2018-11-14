#include <LPC213x.H>
#include "Timer.h"

//Setup the Timer/Counter0
void InitTimer0(void) 
{
    T0MR0 = 49;	                // fout=pclk/100 (pclk=fclk)
    T0MCR = 2;	                // Reset on MR0
    T0TCR = 1;	                // Timer0 Enable
    T0EMR = (1<<0)|(3<<4);      // Toggle MAT0.0 on MR0
}
