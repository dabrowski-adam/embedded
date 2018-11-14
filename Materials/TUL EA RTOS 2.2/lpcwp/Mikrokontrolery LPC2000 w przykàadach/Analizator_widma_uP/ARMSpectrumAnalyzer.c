////////////////////////////////////////////////////////
// Function:	FFT Spectrum Analyzer
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
                  
#include <LPC213x.H>
#include "uart.h"
#include "lcd.h"
#include "port_alias.h"
#include "fft.h"
#include "SamplingModule.h"
#include "WindowType.h"

//*******************************
// Delay function
//*******************************
void delay(unsigned int del_time) 
{ 
	unsigned int i;
	for(i=0;i<del_time;i++);	   
}								    							    

//*******************************
// Displays information about
// window used to LCD.
//*******************************
void DisplayWindowType(WindowType wt)
{
	LCDClear();
	if(wt==wtNone)
		LCDText("Window: None");
	else if(wt==wtHanning)
		LCDText("Window: Hanning");
	else if(wt==wtHamming)
		LCDText("Window: Hamming");
}

//*******************************
// Sending information about
// spectrum and window used to PC
//*******************************
void SendSpectrumInfo(fix *Spectrum, WindowType wt)
{
	unsigned long i;
	unsigned char summa;

	SendByte(0xAA);
	SendByte((unsigned char)wt);
	summa=0xAA+(unsigned char)wt;
	for(i=0;i<256;i++)
	{
		unsigned long SpBin=Spectrum[i];
		SendByte((SpBin>>24)&0xFF);
		SendByte((SpBin>>16)&0xFF);
		SendByte((SpBin>>8)&0xFF);
		SendByte(SpBin&0xFF);

		summa+=((SpBin>>24)&0xFF);
		summa+=((SpBin>>16)&0xFF);
		summa+=((SpBin>>8)&0xFF);
		summa+=(SpBin&0xFF);
	}
	SendByte(0x100-summa);
}

//*******************************
// MAIN
//*******************************
int main (void)
{
	cplx Samples[N];
	cplx FFTResult[N];
	fix Spectrum[N];
	WindowType WinType=wtNone;

	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;

	//Port direction configuration
	IODIR0 = 0xFF000000; 
	IODIR1 = 0x00FF0000;

	//ADC0 configuration
	PINSEL1 |= (1<<24);			// P0.28 = AD0.1
	AD0CR = 0x002E0D02;		// AC: 10 bit AD0.1 @ 4.3MHz	

	//Init devices
	InitSerial();
	LCDInit();
	InitFFT();
	InitSamplingModule();

	LCDClear();
	LCDText("Disconnect input");
	LCDTextXY(0,1,"Calibrating....");
	delay(0x600000);

	//First dummy conversion (big error)
	GetInitialVoltage();

	//The right conversion
	if(GetInitialVoltage()==0)
	{
		LCDClear();
		LCDText("Calibration Err");
		LCDTextXY(0,1,"Try again-RESET");
		while(1);
	}

	LCDClear();
	LCDText("Calibration OK");
	delay(0x400000);
	DisplayWindowType(WinType);

	while(1)
	{
		unsigned long i;

		//Sampling
		Sampling(Samples);

		//Multiplying by window's factors
		if(WinType==wtHanning)
			Hanning(Samples);
		else if(WinType==wtHamming)
			Hamming(Samples);

		//Computing Spectrum (FFT)
		FFT(Samples,FFTResult);

		//Computing squared modules of FFTResult (Spectrum)
		for(i=0;i<N;i++)
		{
			fix Re2,Im2;

			//Scaling by N/2=128
			FFTResult[i].Re>>=7;
			FFTResult[i].Im>>=7;

			//Squareing
			Re2=FMUL(FFTResult[i].Re,FFTResult[i].Re,q);
			Im2=FMUL(FFTResult[i].Im,FFTResult[i].Im,q);
			Spectrum[i]=Re2+Im2;
		}

		//Changing the window
		if(!SW1 | !SW2 | !SW3)
		{
			delay(0x800);
			if(!SW1)
				WinType=wtNone;
			else if(!SW2)
				WinType=wtHanning;
			else if(!SW3)
				WinType=wtHamming;

			DisplayWindowType(WinType);
		}

		//Checking request
		if((U0LSR & 0x01)==0)
			continue;

		//Sending spectrum to PC (if requested)
		SendSpectrumInfo(Spectrum,WinType);

		//Empty input buffer by dummy reading
		while(U0LSR & 0x01)
			i=U0RBR;
	}
}
