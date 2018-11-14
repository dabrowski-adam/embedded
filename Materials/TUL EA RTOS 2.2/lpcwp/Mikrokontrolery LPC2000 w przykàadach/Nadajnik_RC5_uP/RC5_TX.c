//************************************************************
//  File: RC5_TX.c
//  RC5 transmitter implementation
//  for ARM based microcontrollers.
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#include <LPC213x.H>
#include "Timer.h"

#define T_889US  53340
#define T_1778US 106679

//*******************************
// Init RC5 transmitter
//*******************************
void InitRC5_TX(void)
{
	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;

	//Port direction configuration
	IODIR0 = 0xFFF00000; 
	IODIR1 = 0x00FF0000;

	//P0.22 GPIO + LOW -> P0.22
	PINSEL1&=~(3<<12);
	IOPIN0&=~0x00400000;

	//Timer0 - 36kHz
	InitTimer0();

	//Timer1 - bit timings
	InitTimer1();
}

//*******************************
// 0 = 36kHz + space
//*******************************
void RC5_Zero(void)
{
	//Enable bit timing
	T1TC=0;
	T1TCR=1;

	//*** 36kHz 889us ***
	//P0.22 toggled by Timer0
	PINSEL1|=(3<<12);
	T0TCR=1;
	while(T1TC<T_889US);

	//*** space 889us ***
	T0TCR=0;
	//P0.22 GPIO + LOW -> P0.22
	PINSEL1&=~(3<<12);
	IOPIN0&=~0x00400000;
	while(T1TC<T_1778US);
}

//*******************************
// 1 = space + 36kHz
//*******************************
void RC5_One(void)
{
	//Enable bit timing
	T1TC=0;
	T1TCR=1;

	//*** space 889us ***
	T0TCR=0;
	//P0.22 GPIO + LOW -> P0.22
	PINSEL1&=~(3<<12);
	IOPIN0&=~0x00400000;
	while(T1TC<T_889US);

	//*** 36kHz 889us ***
	//P0.22 toggled by Timer0
	PINSEL1|=(3<<12);
	T0TCR=1;
	while(T1TC<T_1778US);

	//off 36kHz
	T0TCR=0;
	//P0.22 GPIO + LOW -> P0.22
	PINSEL1&=~(3<<12);
	IOPIN0&=~0x00400000;
}

//*******************************
// Send RC5 14-bit frame
//*******************************
void SendRC5(unsigned int Address, unsigned int Command)
{
	static unsigned char ToggleBit=0;
	unsigned int i;

	//Header
	RC5_One();
	RC5_One();

	//Toggle bit
	(ToggleBit==0)?RC5_Zero():RC5_One();
	ToggleBit^=1;

	//Address
	for(i=0;i<5;i++)
	{
		if((Address&(1<<(4-i)))==0)
			RC5_Zero();
		else
			RC5_One();
	}

	//Command
	for(i=0;i<6;i++)
	{
		if((Command&(1<<(5-i)))==0)
			RC5_Zero();
		else
			RC5_One();
	}
}
