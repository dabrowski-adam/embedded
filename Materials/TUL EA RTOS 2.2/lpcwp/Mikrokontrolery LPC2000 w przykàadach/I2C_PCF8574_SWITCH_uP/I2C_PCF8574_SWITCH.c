////////////////////////////////////////////////////////
// Function:	I2C example - dipswitch reading via PCF8574
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

//Constants
#define SLAVE_ADDRESS_PCF8574_W 0x4E
#define SLAVE_ADDRESS_PCF8574_R 0x4F

//Globals
unsigned int I2CData,I2CAddress;
volatile unsigned int lock=0;

//I2C interrupt handler's header
void I2CISR(void) __attribute__ ((interrupt));

//*******************************
// I2C Init
//*******************************
void I2CInit(void)
{
	//P0.2=SCL0, P0.3=SDA0
	PINSEL0&=~(15<<4);
	PINSEL0|=(5<<4);

	//VIC Slot 0 used for handling I2C IRQ
	VICVectCntl0 = 0x00000029;
	VICVectAddr0 = (unsigned)I2CISR;
	VICIntEnable = 0x00000200;

	//Set bit rate 15MHz/(I2SCLH+I2SCLL)=cca. 57.6k
	I2C0SCLH 		 = 0x82;
	I2C0SCLL 		 = 0x82;
}

//*******************************
// I2C Receive Byte from PCF8574
//*******************************
unsigned int I2CPCF8574ReceiveByte(unsigned int SlaveAddress)
{
	I2CAddress=SlaveAddress;

	while(lock==1);
	lock=1;

	I2C0CONCLR 	= 0x000000FF;				//Clear all I2C settings
	I2C0CONSET 	= 0x00000040; 				//Enable the I2C interface
	I2C0CONSET 	= 0x00000020; 				//Start condition

	while(lock==1);
	return I2CData;
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

	I2CInit();
	LCDInit();

	LCDClear();

	LCDText("I2C PCF8574");
	LCDGotoXY(0,1);
	LCDText("SWITCH Reader");
	delay(0x400000);

	while(1)
	{
		unsigned int i;
		unsigned int val=I2CPCF8574ReceiveByte(SLAVE_ADDRESS_PCF8574_R);
		LCDGotoXY(0,0);
		LCDText("Inp=");
		LCD32bitNumberDec(val);
		LCDText(" dec        ");

		LCDGotoXY(0,1);
		LCDText("Inp=");
		for(i=8;i>0;i--)
		{
			unsigned int tmp=val&(1<<(i-1));
			LCD32bitNumberDec(tmp>>(i-1));
		}
		LCDText(" bin");

		delay(0x50000);
	}
}

//*******************************
// I2C interrupt routine
//*******************************
void I2CISR(void)
{
	//Simple state machine
	switch (I2C0STAT)
	{
		//Receive byte conditions
		case (0x08):						//Start bit
			I2C0CONCLR=0x20;					//Clear start bit
			I2C0DAT=I2CAddress; 				//Send address
			break;

		case (0x40) :						//Slave Address +R, ACK
			I2C0CONSET 	= 0x04;				//Enable ACK for data byte
			break;

		case (0x48) :						//Slave Address +R, Not Ack
			I2C0CONSET 	= 0x20;				//Resend Start condition
			break;

		case (0x50) :						//Data Received, ACK 
			I2CData 	= I2C0DAT;
			I2C0CONSET 	= 0x10;				//Stop condition
			lock 		= 0;            	//Signal end of I2C activity							
			break;

		case (0x58):						//Data Received, Not Ack
			I2C0CONSET 	= 0x20;				//Resend Start condition
			break;

		default :
			break;
	}

	I2C0CONCLR 	= 0x08;						//Clear I2C interrupt flag
	VICVectAddr = 0x00000000;				//Acknowlage interrupt
}
