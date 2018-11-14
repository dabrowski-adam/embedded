/*****************************************************************************
 *   spitest.c:  main C entry file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC214x.H"                        /* LPC21xx definitions */
#include "type.h"
#include "irq.h"
#include "spi.h"

BYTE SPICmd[BUFSIZE];
BYTE SPIWRData[BUFSIZE];
BYTE SPIRDData[BUFSIZE];

/*****************************************************************************
** Function name:		SPI0_Test
**
** Descriptions:		Test SPI0 as master interfacing with a Atmel 
**				SPI based Serial EEPROM 25020
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SPI0_Test( void )
{
    DWORD i, timeout;

    IOCLR0 = SPI0_SEL;
    /* The tested SPI device is a Atmel 25020 */
    SPICmd[0] = WREN;			/* set write enable latch */
    SPISend( SPICmd, 1 );
    IOSET0 = SPI0_SEL;

    for ( i = 0; i < DELAY_COUNT; i++ );	/* delay minimum 250ns */

    IOCLR0 = SPI0_SEL;
    SPICmd[0] = RDSR;	/* check status to see if write enabled is latched */
    SPISend( SPICmd, 1 );
    SPIReceive( SPIRDData, 1 );
    IOSET0 = SPI0_SEL;
    if ( SPIRDData[0] & (RDSR_WEN|RDSR_RDY) != RDSR_WEN ) 
			/* bit 0 to 0 is ready, bit 1 to 1 is write enable */
    {					
	while ( 1 );	/* Write enable can't be latched, permanent error */
    }

    for ( i = 0; i < BUFSIZE; i++ )	/* Init RD and WR buffer */    
    {
	SPIWRData[i+2] = i;	/* leave two bytes for cmd and addr */
				/* byte 0 and 1 are used for commands and offset */
	SPIRDData[i] = 0;
    }

    /* please note the first two bytes of WR and RD buffer is used for
    commands and offset, so only 2 through BUFSIZE is used for data read,
    write, and comparison. */
    IOCLR0 = SPI0_SEL;
    SPIWRData[0] = WRITE;	/* Write command is 0x02, low 256 bytes only */
    SPIWRData[1] = 0x00;	/* write address offset is 0x00 */
    SPISend( SPIWRData, BUFSIZE );
    IOSET0 = SPI0_SEL;

    for ( i = 0; i < DELAY_COUNT; i++ );	/* delay, minimum 250ns */
    timeout = 0;
    while ( timeout < MAX_TIMEOUT )
    {
	IOCLR0 = SPI0_SEL;
	SPICmd[0] = RDSR;	/* check status to see if write cycle is done or not */
	SPISend( SPICmd, 1);
	SPIReceive( SPIRDData, 1 );
	IOSET0 = SPI0_SEL;

	if ( (SPIRDData[0] & RDSR_RDY) == 0x00 )	/* bit 0 to 0 is ready */
	{
	    break;
	}
	timeout++;
    }
    if ( timeout == MAX_TIMEOUT )
    {
	while ( 1 );
    }

    for ( i = 0; i < DELAY_COUNT; i++ );	/* delay, minimum 250ns */
    IOCLR0 = SPI0_SEL;
    SPICmd[0] = READ;		/* Read command is 0x03, low 256 bytes only */
    SPICmd[1] = 0x00;		/* Read address offset is 0x00 */
    SPISend( SPICmd, 2 ); 
    SPIReceive( SPIRDData+2, BUFSIZE-2 );
    IOSET0 = SPI0_SEL;

    /* verifying, ignore the difference in the first two bytes */
    for ( i = 2; i < BUFSIZE; i++ )
    {
	if ( SPIWRData[i] != SPIRDData[i] )
	{
	    while( 1 );			/* Verification failed */
	}
    }
    return;
}

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	    	
    init_VIC();

    SPIInit();		/* initialize SPI 0 */
    SPI0_Test();
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

