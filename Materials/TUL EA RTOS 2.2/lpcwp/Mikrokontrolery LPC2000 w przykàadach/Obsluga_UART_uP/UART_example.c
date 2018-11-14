////////////////////////////////////////////////////////
// Function:	UART example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC-4.0.1 (www.gnuarm.com)
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"
#include "switches_pins.h"

//Macros
#define TEMT		(1<<6)
#define RDR			(1<<0)
#define LF			0x0A
#define CR			0x0D
#define SW_DELAY	delay(0x8000)

//Globals
const char Msg1[]="S1 pressed";
const char Msg2[]="S2 pressed";
const char Msg3[]="S3 pressed";
const char Msg4[]="S4 pressed";

//UART0 receiver interrupt handler's header
void UART0_RECISR(void) __attribute__ ((interrupt("IRQ")));


//*******************************
// UART Init
//*******************************
void UARTInit(void)
{
	//P0.0=TxD0, P0.1=RxD0
	PINSEL0=0x05;

	//8 bit, no Parity, 1 Stop bit, enable access to DLAB
  	U0LCR=0x83;

	//BAUDRATE=115200bps @ VPBfreq=60MHz
  	U0DLL=33;
	U0DLM=0;

	//Disable access to DLAB
	U0LCR = 0x03; 

	//Enable & reset FIFO's. Trigger level for Rx FIFO=1
	U0FCR=0x07;

	//Enable RDA and RLS interrupt
   	U0IER=(1<<0)|(1<<2);

	//VIC channel0 for UART0 receiver interrupt
    VICVectAddr0 = (unsigned long)UART0_RECISR;
    VICVectCntl0 = 0x20 | 6;
    VICIntEnable = 0x00000040;
}

//*******************************
// Send Byte via UART0
//*******************************
void SendByte(unsigned char ch)
{
	U0THR=ch;
	while(!(U0LSR & TEMT));
}

//*******************************
// Send String
//*******************************
void SendString(const char *pStr)
{
	while(*pStr)
		SendByte(*pStr++);

	SendByte(CR);
	SendByte(LF);
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
	//Peripherials' frequency = fclk = 60MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x01;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();

	LCDClear();

	LCDText("UART example");
	delay(0x400000);

	UARTInit();

	//Endless loop
	while(1)
	{
		if(SW1==0)
			SendString(Msg1);
		else if(SW2==0)
			SendString(Msg2);
		else if(SW3==0)
			SendString(Msg3);
		else if(SW4==0)
			SendString(Msg4);

		if(!SW1|!SW2|!SW3|!SW4)
			delay(0x80000);

		SW_DELAY;
	}
}

//*******************************
// UART0 receiver interrupt
//*******************************
void UART0_RECISR(void)
{
	//Read interrupt ID
	unsigned int iid=U0IIR;

	//if RLS interrupt - error occured
	if((iid & 0x0F)==0x6)
	{
		//reading U0LSR clears RLS interrupt
		unsigned int err=U0LSR;

		LCDClear();
		LCDText("Error: ");

		if((err & 0x02)!=0)
			LCDText("OE ");

		if((err & 0x04)!=0)
			LCDText("PE ");

		if((err & 0x08)!=0)
			LCDText("FE ");

		delay(0x800000);
		LCDClear();
	}

	//if RDA interrupt
	if((iid & 0x0F)==0x4)
	{
		unsigned int data=0;
		
		//reading U0RBR clears RDA interrupt
		//there may be more bytes than 1 in Rx FIFO
		while(U0LSR & RDR)
			data=U0RBR;

		LCDGotoXY(0,0);
		LCD32bitNumberDec(data);
		LCDText("               ");
	}

	//Acknowledge interrupt
    VICVectAddr = 0;
}
