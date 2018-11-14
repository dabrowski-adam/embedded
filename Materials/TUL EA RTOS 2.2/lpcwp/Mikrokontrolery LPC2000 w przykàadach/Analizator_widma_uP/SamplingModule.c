#include <LPC213X.H>
#include "SamplingModule.h"

cplx *Samples;

//Timer/Counter0 interrupts counter
unsigned int IntCounter;

//Initial input voltage (fixed point numner mantisa)
int InitialVoltage=0;

//Generate Interrupt 
void tc0 (void) __attribute__ ((interrupt));

//Setup the Timer Counter 0 Interrupt
void InitSamplingModule(void) 
{
    T0MR0 = 1499;                  			// 25us = 1500-1 counts
    T0MCR = 3;                             	// Interrupt and Reset on MR0
    T0TCR = 0;                             	// Timer0 Disable
    VICVectAddr0 = (unsigned long)tc0;      // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                // use it for Timer 0 Interrupt
    VICIntEnable = 0x00000010;              // Enable Timer0 Interrupt
}

//Returns voltage [V] as fixed point number (q=11)
int GetADCVoltage(void)
{
	unsigned int adc_val;

	AD0CR |= 0x01000000;					//start A/D conversion
	do
	{
		adc_val = AD0DR;					//read result
	}while((adc_val & 0x80000000)==0);		//wait until conversion finished
	AD0CR &= ~0x01000000;					//finish A/D conversion
	adc_val=(adc_val>>6)&0x03FF;

	//3.3V = 0x1A66
	return (((int)adc_val)*0x1A66)>>10;
}

//Timer Counter 0 Interrupt executes each 25us @ 60 MHz CPU Clock
void tc0(void) 
{
	int fix_voltage;

	//Getting one sample
	fix_voltage=GetADCVoltage();

	//Substract InitialVoltage
	fix_voltage-=InitialVoltage;

	//Store sample
	Samples[IntCounter].Re=fix_voltage;
	Samples[IntCounter].Im=0;
	
	//Stop sampling
	if(IntCounter++>=255)
		T0TCR = 0;
		
	//Clear interrupt flag & acknowledge interrupt
    T0IR = 1;					
    VICVectAddr = 0;
}

//Begin sampling
void Sampling(cplx *s)
{
	Samples=s;
	IntCounter=0;
    T0TCR = 1;

	while(T0TCR==1);
}

//Getting initial voltage (cca. 3.3V/2)
//Returns 1 when OK
//Returns 0 when big noise
int GetInitialVoltage(void)
{
	unsigned int i;

	//0xCCC = 1.6V
	//0xD99 = 1.7V
	for(i=0;i<5;i++)
	{
		unsigned int j;

		InitialVoltage=GetADCVoltage();
		if((InitialVoltage<0xCCC) || (InitialVoltage>0xD99))
			return 0;

		//Short delay
		for(j=0;j<0x400;j++);
	}

	return 1;
}
