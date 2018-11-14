////////////////////////////////////////////////////////
// Function:	I2C example - LED driver via PCF8574
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
// I2C Send Byte to PCF8574
//*******************************
void I2CPCF8574SendByte(unsigned int SlaveAddress, unsigned int DataByte)
{
	I2CAddress=SlaveAddress;
	I2CData=DataByte & 0xFF;

	while(lock==1);
	lock=1;

	I2C0CONCLR 	= 0x000000FF;				//Clear all I2C settings
	I2C0CONSET 	= 0x00000040; 				//Enable the I2C interface
	I2C0CONSET 	= 0x00000020; 				//Start condition

	while(lock==1);
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
	LCDText("LED Driver");
	delay(0x400000);

	//LEDs blinking
	while(1)
	{
		unsigned int i;
		for(i=0;i<8;i++)
		{
			I2CPCF8574SendByte(SLAVE_ADDRESS_PCF8574_W,~(1<<i));
			delay(0x100000);
		}
		for(i=6;i>0;i--)
		{
			I2CPCF8574SendByte(SLAVE_ADDRESS_PCF8574_W,~(1<<i));
			delay(0x100000);
		}
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
		//Start and Send byte conditions
		case ( 0x08):						//Start bit
			I2C0CONCLR=0x20;					//Clear start bit
			I2C0DAT=I2CAddress; 				//Send address and write bit
			break;

		case (0x18):						//Slave address+W, ACK
			I2C0DAT=I2CData;					//Write data to tx register
			break;

		case (0x20):						//Salve address +W, Not ACK
			I2C0DAT=I2CAddress; 				//Resend address and write bit
			break;

		case (0x28):
			I2C0CONSET=0x10;					//Stop condition
			lock=0;                       	//Signal end of I2C activity
			break;

		case (0x30)	:						//Data sent, NOT Ack
			I2C0DAT=I2CData;					//Write data to tx register
			break;

		default :
			break;
	}

	I2C0CONCLR 	= 0x08;						//Clear I2C interrupt flag
	VICVectAddr = 0x00000000;				//Acknowlage interrupt
}
