////////////////////////////////////////////////////////
// Function:	Nested interrupts example.
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

// Macro for enabling interrupts, moving to System mode and relevant stack operations
#define IENABLE /* Nested Interrupts Entry */ \
asm ( "MRS LR, SPSR" ); /* Copy SPSR_irq to LR */ \
asm ( "STMFD SP!, {LR}" ); /* Save SPSR_irq */ \
asm ( "MSR CPSR_c, #0x1F" ); /* Enable IRQ (Sys Mode) */ \
asm ( "STMFD SP!, {LR}" ); /* Save LR */ \

// Macro for disabling interrupts, switching back to IRQ and relevant stack operations
#define IDISABLE /* Nested Interrupts Exit */ \
asm ( "LDMFD SP!, {LR}" ); /* Restore LR */ \
asm ( "MSR CPSR_c, #0x92" ); /* Disable IRQ (IRQ Mode) */ \
asm ( "LDMFD SP!, {LR}" ); /* Restore SPSR_irq to LR */ \
asm ( "MSR SPSR_cxsf, LR" ); /* Copy LR to SPSR_irq */ \


//Timer0 interrupt handler's header
void tc0 (void) __attribute__ ((interrupt("IRQ")));

//EINT2 interrupt handler's header
void EINT2ISR(void) __attribute__ ((interrupt("IRQ")));

//*******************************
// EINT2 Init
//*******************************
void EINT2Init(void)
{
	//P0.7=EINT2
	PINSEL0|=(3<<14);
	
	//EXT4 edge trigered
	EXTMODE=(1<<2);
	
	//Falling edge
	EXTPOLAR=0;

	//VIC Slot 1 used for handling EINT2 IRQ
	VICVectCntl1 = 0x00000030;
	VICVectAddr1 = (unsigned int)EINT2ISR;
	VICIntEnable |= 0x00010000;
}

//*******************************
// Setup the Timer Counter 0
//*******************************
void InitTimer0(void) 
{
    T0MR0 = 2999999;                  		// 200ms counts
    T0MCR = 3;                             	// Interrupt and Reset on MR0
    T0TCR = 0;                             	// Timer0 Disable
    VICVectAddr0 = (unsigned long)tc0;      // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                // use it for Timer 0 Interrupt
    VICIntEnable |= 0x00000010;             // Enable Timer0 Interrupt
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

	InitTimer0();
	EINT2Init();
	LCDInit();

	LCDClear();

	LCDText("Nested Interr.");
	LCDGotoXY(0,1);
	LCDText("falling -> EINT2");
	delay(0x200000);

	while(1);
}

//*******************************
// EINT2 interrupt routine
//*******************************
void EINT2ISR(void)
{
	//Clear EINT2 interrupt flag
	EXTINT=(1<<2);

	IENABLE;

	//Enable timer
	T0TC=0;
	T0TCR=1;

	//Here LED is blinking in T0 Interrupt
	delay(0x800000);

	//Disable timer
	T0TCR=0;

	IDISABLE;

	//Clear EINT2 interrupt flag & ack interrupt
	EXTINT=(1<<2);
	VICVectAddr=0x00000000;
}

//*******************************
// Timer Counter 0 Interrupt
//*******************************
void tc0(void) 
{
    IOPIN1^=0x00FF0000;

    //Clear interrupt flag & acknowledge interrupt
    T0IR = 1;					
    VICVectAddr = 0;
}
