#include <LPC213X.H>
#include "Timer.h"

//Assembly division routine
extern unsigned int div(unsigned int, unsigned int);

//Sinus table
int Sin[1000]={
#include "Sin1000.dat"
};

//***** Signal's parameters	*****
SignalType SigType=stSinus;

//Amplitude=128*value[v] (1Volt=128), exampl. for 1.27V Amplitude=162
int Amplitude=128;

//ConstValue=value for DAC, exampl. for 1.65V ConstValue=512
int ConstValue=512;

//Frequency=frequency[Hz]/100
int Frequency=10;

//Duty: 100%=128, exampl. for 50%duty Duty=64
int Duty=64;
//***** End of signal's parameters *****

//Step for wave generation
int Step;

//Generate Interrupt 
void tc0 (void) __attribute__ ((interrupt));

//Setup the Timer Counter 0
void InitTimer0(void) 
{
    T0MR0 = 5999;                  			// 100us counts
    T0MCR = 3;                             	// Interrupt and Reset on MR0
    T0TCR = 0;                             	// Timer0 Disable
    VICVectAddr0 = (unsigned long)tc0;      // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                // use it for Timer 0 Interrupt
    VICIntEnable = 0x00000010;              // Enable Timer0 Interrupt
}

//Timer Counter 0 Interrupt
void tc0(void) 
{
	static unsigned int i=0;
	unsigned int dac_val=0;

	if(SigType==stSinus)
	{
		int tmpS=(Sin[i]*Amplitude)>>7;
		dac_val=(unsigned int)(tmpS+ConstValue);
	}
	else if(SigType==stRectangle)
	{
		int tmpA=(310*Amplitude)>>7;
		int tmpD=(1000*Duty)>>7;
		if(i<tmpD)
			dac_val=(unsigned int)(ConstValue+tmpA);
		else
			dac_val=(unsigned int)(ConstValue-tmpA);
	}
	else if(SigType==stTriangle)
	{
		int tmpA,j;
		if(i<250)
			j=i;
		else if(i<500)
			j=500-i;
		else if(i<750)
			j=i-500;
		else
			j=1000-i;
			
		tmpA=(310*Amplitude*j)/32000;

		if(i<500)
			dac_val=(unsigned int)(ConstValue+tmpA);
		else
			dac_val=(unsigned int)(ConstValue-tmpA);

	}

	DACR = (dac_val & 0x3FF) << 6;

	i+=Step;
	if(i>=999)
		i=0;

	//Clear interrupt flag & acknowledge interrupt
    T0IR = 1;					
    VICVectAddr = 0;
}

//Set signal's parameters
void SetCurrentSignalParameters(void)
{
	unsigned int i,divres=6000;
	int ThousandDividers[]={1,2,4,5,8,10,20,25,40};

	//Disable timer
	T0TCR=0;

	//Disable Timer0 Interrupt
    VICIntEnable = 0x00000000;

	//Setting time parameters: Step & T0MR0
	for(i=0;i<9;i++)
	{
		Step=ThousandDividers[i];

		//T0MR0=(Step*600)/Frequency-1;
		divres=div(Step*600,Frequency);

		if(divres>=240)
		{
			T0MR0=divres-1;
			break;
		}
	}

	//Enable Timer0 Interrupt & reset timer
	T0TC=0;
	T0IR=1;
    VICIntEnable = 0x00000010;

	//Enable timer0
	T0TCR=1;
}

//Set wave type
void SetSignalType(SignalType ST)
{
	SigType=ST;
	SetCurrentSignalParameters();
	
}

//Set amplitude
void SetAmplitude(int A)
{
	Amplitude=A;
	SetCurrentSignalParameters();	
}

//Set const value
void SetConstValue(int C)
{
	ConstValue=C;
	SetCurrentSignalParameters();	
}

//Set frequency
void SetFrequency(int F)
{
	Frequency=F;
	SetCurrentSignalParameters();	
}

//Set duty
void SetDuty(int D)
{
	if(SigType!=stRectangle)
		return;

	Duty=D;
	SetCurrentSignalParameters();	
}

SignalType GetSignalType(void)
{
	return SigType;
}

int GetAmplitude(void)
{
	return Amplitude;
}

int GetConstValue(void)
{
	return ConstValue;
}

int GetFrequency(void)
{
	return Frequency;
}

int GetDuty(void)
{
	return Duty;
}
