////////////////////////////////////////////////////////
// Function:	RC5 receiver
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC-4.0.1 (www.gnuarm.com)
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>
#include "Timer.h"
#include "lcd.h"

extern volatile unsigned int time_count;
	 
#define IRED_PIN		(IOPIN0 & (1<<2))
#define BIT_TIMER_EN    T1TCR
#define FRAME_TIMEOUT   6000
#define MIN_BIT_TIME    150
#define MAX_BIT_TIME    250

//Error codes
#define ERR_BIT_TIME		1
#define ERR_SAMPLING		2
#define ERR_FRAME_TIMEOUT	3


//*******************************
// RC5 frame receiver
// waits for leading edge of the RC5 frame
// returns:
// 0 - OK
// 1 - bit time error
// 2 - sampling error
// 3 - frame timeout
//*******************************
unsigned int RC5_RX(unsigned int *ptoggle, unsigned int *paddress, unsigned int *pcommand)
{
	unsigned int bit_time, tmp_time, i, tmp_ired;

	//wait for start of transmission
	while(IRED_PIN!=0);

	//measure bit pulse time
	time_count=0;
	BIT_TIMER_EN=1;
	while(IRED_PIN==0)
		if(time_count>FRAME_TIMEOUT)
			return ERR_FRAME_TIMEOUT;

	while(IRED_PIN!=0)
		if(time_count>FRAME_TIMEOUT)
			return ERR_FRAME_TIMEOUT;

	bit_time=time_count;

	//error?
	if((bit_time<MIN_BIT_TIME) || (bit_time>MAX_BIT_TIME))
		return ERR_BIT_TIME;

	//waiting 0.75*bit time
	tmp_time=time_count;
	while((time_count-tmp_time)<=(3*bit_time)/4);


	//main RC5 loop
	for(*pcommand=0,*paddress=0,*ptoggle=0,i=0;i<12;i++)
	{
		//waiting for the edge
		tmp_ired=IRED_PIN;
		while(IRED_PIN==tmp_ired)
			if(time_count>FRAME_TIMEOUT)
				return ERR_FRAME_TIMEOUT;


		//waiting 0.25*bit time
		tmp_time=time_count;
		while((time_count-tmp_time)<=bit_time/4);

		//sampling two times
		tmp_ired=IRED_PIN;

		//wait for cca. 0.10 of half bit time
		tmp_time=time_count;
		while((time_count-tmp_time)<=10);

		if(IRED_PIN!=tmp_ired)
			return ERR_SAMPLING;

		if(i==0)
		{
			//toggle bit
			if(tmp_ired==0)
				*ptoggle|=1;
		}
		else if(i<6)
		{
			//address
			*paddress<<=1;
			if(tmp_ired==0)
				*paddress|=1;
		}
		else
		{
			//command
			*pcommand<<=1;
			if(tmp_ired==0)
				*pcommand|=1;
		}

		//wait half bit time
		while((time_count-tmp_time)<=bit_time/2);
	}
	BIT_TIMER_EN=0;

	return 0;
}

//*******************************
// Waits for RC5 remote key release
// returns, when no signal received
// during at least MinimumNoSignalTime miliseconds
//*******************************
void WaitForKeyRelease(unsigned int MinimumNoSignalTime)
{
	const unsigned int MinNoSignalTicks=115*MinimumNoSignalTime;

	time_count=0;
	BIT_TIMER_EN=1;
	while(time_count<MinNoSignalTicks)
		if(IRED_PIN==0)
			time_count=0;		
	BIT_TIMER_EN=0;
}

//*******************************
// MAIN
//*******************************
int main(void) 
{
	//Peripherials' frequency = fclk/4 = 15MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x00;
	
	//P1.16..23 defined as outputs
	IODIR1 = 0x00FF0000;

	//P0.2 defined as input
	IODIR0 = 0xFF000000;

	//inits
	LCDInit();
	init_bit_timer();

	LCDClear();
	LCDText("RC5 Receiver");
	LCDGotoXY(0,1);
	LCDText("Use your remote");

	while(1)
	{
		unsigned int toggle,address,command,err;

		//receive RC5 frame
		err=RC5_RX(&toggle,&address,&command);

		LCDClear();
		if(err==0)
		{
			LCDText("Command: ");
			LCD32bitNumberDec(command);
			LCDGotoXY(0,1);
			LCDText("Address: ");
			LCD32bitNumberDec(address);
			LCDText(", T:");
			LCD32bitNumberDec(toggle);
		}
		else if(err==ERR_BIT_TIME)
			LCDText("ERR_BIT_TIME");
		else if(err==ERR_SAMPLING)
			LCDText("ERR_SAMPLING");
		else if(err==ERR_FRAME_TIMEOUT)
			LCDText("ERR_FRAME_TIMEOUT");

		//wait for key release (cca. 100ms without signal)
		WaitForKeyRelease(100);
	}
}
