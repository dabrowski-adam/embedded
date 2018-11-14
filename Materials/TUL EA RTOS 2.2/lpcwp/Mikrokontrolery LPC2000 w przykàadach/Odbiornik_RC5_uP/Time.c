#include <LPC213x.H>
#include "Timer.h"

//Time counter variable
volatile unsigned int time_count;

void tc1(void) __attribute__ ((interrupt("IRQ")));

void init_bit_timer(void)
{
    T1MR0 = 132;                         // 889us/100
    T1MCR = 3;                           // Reset & int on MR0
    T1TCR = 0;                           // Timer1 Disable
    VICVectAddr0 = (unsigned long)tc1;   // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 5;             // use it for Timer 1 Interrupt
    VICIntEnable = 0x00000020;           // Enable Timer0 Interrupt
}

//Timer Counter 1 Interrupt
void tc1(void) 
{	
	time_count++;

	//Clear interrupt flag & Acknowledge Interrupt
    T1IR = 1;
    VICVectAddr = 0;
}

