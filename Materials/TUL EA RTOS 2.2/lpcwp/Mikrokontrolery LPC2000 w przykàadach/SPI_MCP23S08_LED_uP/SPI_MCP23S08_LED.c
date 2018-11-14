////////////////////////////////////////////////////////
// Function:	SPI example - LED driver via MCP23S08
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

//Constants
#define SLAVE_ADDRESS_MCP23S08_W 0x46
#define SLAVE_ADDRESS_MCP23S08_R 0x47
#define CS_SET IOSET0=(1<<27)
#define CS_CLR IOCLR0=(1<<27)
#define ADDR_MCP23S08_GPIO 0x09

//Globals
const unsigned char MCP23S08_settings[]={
	0x0F,	//IODIR (0x00)
	0x00,	//IPOL (0x01)
	0x00,	//GPINTEN
	0x00,	//DEFVAL
	0x00,	//INTCON
	0x04,	//IOCON
	0xFF	//GPPU
};


//*******************************
// SPI Init
//*******************************
void SPIInit(void)
{
	//P0.4=SCK0, P0.5=MISO0, P0.6=MOSI0
	PINSEL0&=~(0x3F<<8);
	PINSEL0|=(0x15<<8);

	//Initially CS set
	CS_SET;

	//Set bit rate 15MHz/S0SPCCR = cca. 1.8M
	S0SPCCR = 0x8;

	//Control register: Master mode
	S0SPCR|=(1<<5);
}

//*************************************
// Write SPI byte
//*************************************
unsigned char WriteSPIByte(unsigned char byte)
{
	unsigned char spi_status,dummy;

	//Write byte to SPI Data Register
	S0SPDR = byte;

	//Wait for end of byte transmission
	while((S0SPSR & (1<<7))==0);

	//read SPI status register
	spi_status=S0SPSR;

	//dummy read of SPI data register to clear SPIF (S0SPSR.7) bit
	dummy=S0SPDR;	

	return (spi_status & 0x78);
}

//*************************************
// SPI Send multiple bytes to MCP23S08
//*************************************
unsigned char SPIMCP23S08SendMultipleBytes(const unsigned int SlaveAddress, const unsigned char Address, const unsigned char *pData, const unsigned int NumberOfBytes)
{
	unsigned int count;
	unsigned char err;

	//Start
	CS_CLR;

	//Write SlaveAddress to SPI
	if((err=WriteSPIByte(SlaveAddress & 0xFF))!=0)
	{
		CS_SET;
		return err;
	}

	//Write Address to SPI
	if((err=WriteSPIByte(Address))!=0)
	{
		CS_SET;
		return err;
	}


	//Transmit all bytes
	for(count=0;count<NumberOfBytes;count++)
	{
		//Write byte
		if((err=WriteSPIByte(pData[count]))!=0)
		{
			CS_SET;
			return err;
		}
	}

	//End
	CS_SET;

	return 0;
}	  

//*******************************
// SPI Send Byte to MCP23S08
//*******************************
unsigned char SPIMCP23S08SendByte(const unsigned int SlaveAddress, const unsigned char Address, unsigned char DataByte)
{
	return SPIMCP23S08SendMultipleBytes(SlaveAddress, Address, &DataByte, 1);
}	  

//*******************************
// MCP23S08 Initialization
//*******************************
unsigned char MCP23S08_Init(void)
{
	return SPIMCP23S08SendMultipleBytes(SLAVE_ADDRESS_MCP23S08_W, 0, MCP23S08_settings, sizeof(MCP23S08_settings));
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
	unsigned char Error=0;

	//Peripherials' frequency = fclk/4 = 15MHz
	VPBDIV&=0xFFFFFFFC;
	VPBDIV|=0x00;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();

	LCDClear();

	LCDText("SPI MCP23S08");
	LCDGotoXY(0,1);
	LCDText("LED Driver");
	delay(0x400000);

	SPIInit();
	if(MCP23S08_Init()!=0)
	{
		LCDClear();
		LCDText("Error while");
		LCDGotoXY(0,1);
		LCDText("MCP23S08 config");
		while(1);
	}

	//LEDs blinking
	while(1)
	{
		unsigned int i;
		for(i=4;i<8;i++)
		{
			Error=SPIMCP23S08SendByte(SLAVE_ADDRESS_MCP23S08_W, ADDR_MCP23S08_GPIO, ~(1<<i));
			if(Error!=0)
			{
				LCDClear();
				delay(0x400000);
				LCDText("Error: ");
				if(Error&(1<<3))
					LCDText("A ");
				if(Error&(1<<4))
					LCDText("M ");
				if(Error&(1<<5))
					LCDText("R ");
				if(Error&(1<<6))
					LCDText("W ");

				delay(0x800000);
			}
			delay(0x200000);
		}
		for(i=6;i>4;i--)
		{
			Error=SPIMCP23S08SendByte(SLAVE_ADDRESS_MCP23S08_W, ADDR_MCP23S08_GPIO, ~(1<<i));
			if(Error!=0)
			{
				LCDClear();
				delay(0x400000);
				LCDText("Error: ");
				if(Error&(1<<3))
					LCDText("A ");
				if(Error&(1<<4))
					LCDText("M ");
				if(Error&(1<<5))
					LCDText("R ");
				if(Error&(1<<6))
					LCDText("W ");

				delay(0x800000);
			}
			delay(0x200000);
		}
	}
}
