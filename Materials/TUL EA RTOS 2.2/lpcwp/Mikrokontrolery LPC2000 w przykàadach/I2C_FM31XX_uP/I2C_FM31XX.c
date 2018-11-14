////////////////////////////////////////////////////////
// Function:	I2C example - FRAM FM31XX
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

//Constants
#define SLAVE_ADDRESS_MEM_W 0xA0
#define SLAVE_ADDRESS_MEM_R 0xA1
#define BUFFER_SIZE	256
#define SW_DELAY	delay(0x2000)

//Globals

//I2C
unsigned int I2CAddress,I2CNumberOfBytes;
unsigned char *pI2CData;
unsigned char I2CBuffer[BUFFER_SIZE];

//flag: 0 - Write, 1 - Read
unsigned int fReadWrite=0;

//fReadPart=0 -> writing W slave address, MSB and LSB of Address
//fReadPart=1 -> sending R slave address, reading byte
unsigned int fReadPart=0;

//lock=1 while I2C transmission
volatile unsigned int lock=0;

//Example data
const unsigned char ExampleData[]={5,10,15,20,25};

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

//***********************************
// I2C Send Multiple Bytes to FM31XX
//***********************************
void I2CFM31XXSendMultipleBytes(const unsigned int SlaveAddress, const unsigned int Address, const unsigned char *pData, const unsigned int NumberOfBytes)
{
	unsigned int i;
	I2CAddress=SlaveAddress;

	//Copy Address MSB and LSB to I2C buffer
	I2CBuffer[0]=(Address & 0xFF00)>>8;
	I2CBuffer[1]=Address & 0xFF;

	for(i=0;i<NumberOfBytes;i++)
		I2CBuffer[i+2]=pData[i];

	pI2CData=I2CBuffer;
	I2CNumberOfBytes=NumberOfBytes+2;

	while(lock==1);
	lock=1;

	//write
	fReadWrite=0;

	I2C0CONCLR 	= 0x000000FF;				//Clear all I2C settings
	I2C0CONSET 	= 0x00000040; 				//Enable the I2C interface
	I2C0CONSET 	= 0x00000020; 				//Start condition

	while(lock==1);
}	  

//***********************************
// I2C Read Byte from FM31XX
//***********************************
unsigned int I2CFM31XXReadByte(const unsigned int SlaveAddress, const unsigned int Address)
{
	I2CAddress=SlaveAddress;

	//Copy Address MSB and LSB to I2C buffer
	I2CBuffer[0]=(Address & 0xFF00)>>8;
	I2CBuffer[1]=Address & 0xFF;

	pI2CData=I2CBuffer;
	I2CNumberOfBytes=2;

	while(lock==1);
	lock=1;

	//read
	fReadWrite=1;
	fReadPart=0;

	I2C0CONCLR 	= 0x000000FF;				//Clear all I2C settings
	I2C0CONSET 	= 0x00000040; 				//Enable the I2C interface
	I2C0CONSET 	= 0x00000020; 				//Start condition

	while(lock==1);
	return I2CBuffer[2];
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
// Example writing
//*******************************
void WriteExampleData(void)
{
	LCDClear();
	LCDText("Writing... ");

	I2CFM31XXSendMultipleBytes(SLAVE_ADDRESS_MEM_W,0,ExampleData,sizeof(ExampleData));

	LCDText("OK");
	delay(0x800000);
}

//*******************************
// Example reading
//*******************************
void ReadExampleData(void)
{
	unsigned int ReadResult,i;

	LCDClear();
	LCDText("Reading...");
	delay(0x800000);

	for(i=0;i<sizeof(ExampleData);i++)
	{
		ReadResult=I2CFM31XXReadByte(SLAVE_ADDRESS_MEM_R,i);
		LCDClear();
		LCD32bitNumberDec(i);
		LCDText(": ");
		LCD32bitNumberDec(ReadResult);
		delay(0x800000);
	}
}

//*******************************
// Menu
//*******************************
void DisplayMenu(void)
{
	LCDClear();
	LCDText("S1 - Write");
	LCDGotoXY(0,1);
	LCDText("S2 - Read");
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

	LCDText("I2C FM31XX");
	LCDGotoXY(0,1);
	LCDText("FRAM test");
	delay(0x800000);

	DisplayMenu();

	//Main loop
	while(1)
	{
		if(SW1==0)
		{
			WriteExampleData();
			while(SW1==0)
				SW_DELAY;

			DisplayMenu();
		}
		else if(SW2==0)
		{
			ReadExampleData();
			while(SW2==0)
				SW_DELAY;

			DisplayMenu();
		}

		SW_DELAY;
	}
}

//*******************************
// I2C interrupt routine
//*******************************
void I2CISR(void)
{
	//Simple state machine

	if(fReadWrite==0)
	{
		//Write to FRAM
		switch (I2C0STAT)
		{
			//Start and Send byte conditions
			case ( 0x08):                      //Start bit
				I2C0CONCLR=0x20;               //Clear start bit
				I2C0DAT=I2CAddress;            //Send address and write bit
				break;
	
			case (0x18):                       //Slave address+W, ACK
				I2C0DAT=*pI2CData++;           //Write data to tx register
				break;
	
			case (0x20):                       //Salve address +W, Not ACK
				I2C0DAT=I2CAddress;            //Resend address and write bit
				break;
	
			case (0x28):                       //Data sent, Ack
				if(--I2CNumberOfBytes==0)
				{
					I2C0CONSET=0x10;           //Stop condition
					lock=0;                    //Signal end of I2C activity
				}
				else
				{
					I2C0DAT=*pI2CData++;       //Write data to tx register
				}
				break;
	
			case (0x30):                       //Data sent, NOT Ack
				I2C0DAT=*(pI2CData-1);         //Write data to tx register
				break;
	
			default :
				break;
		}
	}
	else
	{
		//Read from FRAM
		switch (I2C0STAT)
		{
			case (0x08):                       //Start bit
				if(fReadPart==0)
				{
					//writing slave W address
					I2C0CONCLR=0x20;           //Clear start bit
					I2C0DAT=I2CAddress & 0xFE; //Send W slave address
				}
				else
				{
					//writing slave R address
					I2C0CONCLR=0x20;           //Clear start bit
					I2C0DAT=I2CAddress;        //Send address
				}
				break;
	
			//writing MSB and LSB of Address
			case (0x18):                       //Slave address+W, ACK
				I2C0DAT=*pI2CData++;           //Write data to tx register
				break;
	
			case (0x20):                       //Salve address +W, Not ACK
				I2C0DAT=I2CAddress & 0xFE;     //Resend w slave address
				break;
	
			case (0x28):                       //Data sent, Ack
				if(--I2CNumberOfBytes==0)
				{
					//MSB and LSB of Address have been sent
					fReadPart=1;
					I2C0CONCLR 	= 0x000000FF;  //Clear all I2C settings
					I2C0CONSET 	= 0x00000040;  //Enable the I2C interface
					I2C0CONSET 	= 0x00000020;  //Start condition
				}
				else
				{
					I2C0DAT=*pI2CData++;       //Write data to tx register
				}
				break;
	
			case (0x30):                       //Data sent, NOT Ack
				I2C0DAT=*(pI2CData-1);         //Write data to tx register
				break;


			//reading data byte

			case (0x40):                       //Slave Address +R, ACK
				I2C0CONSET 	= 0x04;            //Enable ACK for data byte
				break;
	
			case (0x48):                       //Slave Address +R, Not Ack
				I2C0CONSET 	= 0x20;            //Resend Start condition
				break;
	
			case (0x50):                       //Data Received, ACK 
				I2CBuffer[2]= I2C0DAT;
				I2C0CONSET 	= 0x10;            //Stop condition
				lock 		= 0;               //Signal end of I2C activity							
				break;
	
			case (0x58):                       //Data Received, Not Ack
				I2C0CONSET 	= 0x20;            //Resend Start condition
				break;
	
			default :
				break;
		}
	}

	I2C0CONCLR 	= 0x08;                        //Clear I2C interrupt flag
	VICVectAddr = 0x00000000;                  //Acknowlage interrupt
}
