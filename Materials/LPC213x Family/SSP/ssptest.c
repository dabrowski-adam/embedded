/*****************************************************************************
 *   ssptest.c:  main C entry file for Philips LPC214x Family Microprocessors
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
#include "ssp.h"

BYTE SPIWRData[BUFSIZE];
BYTE SPIRDData[BUFSIZE];
DWORD CurrentTxIndex = 0;
DWORD CurrentRxIndex = 0;

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{

    DWORD i;

    init_VIC();

    for ( i = 0; i < BUFSIZE; i++ )
    {
		SPIWRData[i] = i;	/* set pattern in TX */
		SPIRDData[i] = 0;	/* clear RX */
    }
    SPI1Init();		/* initialize SPI1(SSP) port, SPI0 is the SPI port */
 
    /* No need to consider CurrentTxIndex because it will always transmit 
    as long as CurrentRxIndex is less than BUFSIZE. Once CurrentRxIndex reaches 
    BUFSIZE, it has received the complete block of data, then we bail out on 
    transmit. */

	//This portion performs a write enable; checks the EEPROMs status register
	//Sends the WRITE command with the 16-bit write register


    IOCLR0 = SPI1_SEL;
    while ( CurrentRxIndex <= BUFSIZE )
    {
	/* to check the RXIM and TXIM interrupt, I send a block data at one time 
	based on the FIFOSIZE(8). */
	SPI1Send( (BYTE *)&SPIWRData[CurrentTxIndex], FIFOSIZE );
	SPI1Receive( (BYTE *)&SPIRDData[CurrentRxIndex], FIFOSIZE );
    }
    IOSET0 = SPI1_SEL;
   
    /* verifying write and read data buffer. */
    for ( i = 0; i < BUFSIZE; i++ )
    {
	if ( SPIWRData[i] != SPIRDData[i] )
	{
	    while( 1 );			/* Verification failed */
	}
    }

    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

