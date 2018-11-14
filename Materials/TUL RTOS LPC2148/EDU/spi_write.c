#include "FreeRTOSConfig.h"

#define  SPI_CS   0x00008000  //<= new board, old board = 0x00800000

extern tU8 pattern[8];

void send_SPI(unsigned char indata);
void ledMatrix(void);


void send_SPI(unsigned char indata)
{
	tU32 failsafe;

    IOCLR0 = SPI_CS;  //Uruchomienie SPI

    SPI_SPDR = indata;
    failsafe = 0;
    while(((SPI_SPSR & 0x80) == 0) && (failsafe < 5000))
    	failsafe++;     //¿eby nie czekaæ nie wiadomo jak d³ugo
  	IOSET0 = SPI_CS;  //Wy³¹czenie SPI

  	if (failsafe >= 5000)
  	{
    	SPI_SPCCR = 0x08;
    	SPI_SPCR  = 0x60;
  	}
}

void ledMatrix( void ) __attribute__((interrupt ("IRQ")));
void ledMatrix(void)
{
//	static tU32 sw = 0x5555;
	static tU32 columnCounter = 0x01;
 	static tU8 index = 0;

//	GPIO_IOSET = sw;
//  	GPIO_IOCLR = ~sw;
//	sw = ~sw;

  	if (columnCounter > 0x80)
  	{
    	columnCounter = 0x01;
    	index = 0;
  	}
  	send_SPI(~pattern[index++]);
  	send_SPI(~columnCounter);
  	columnCounter <<= 1;

  	T1IR        = 0xff;        //reset all IRQ flags
  	VICVectAddr = 0x00;        //dummy write to VIC to signal end of interrupt
}

