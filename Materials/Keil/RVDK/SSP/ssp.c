/*****************************************************************************
 *   ssp.c:  SSP C file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC214x.h"			/* LPC21XX Peripheral Registers	*/
#include "type.h"
#include "irq.h"
#include "ssp.h"

extern BYTE SPIWRData[BUFSIZE];
extern BYTE SPIRDData[BUFSIZE];
extern DWORD CurrentTxIndex;
extern DWORD CurrentRxIndex;

/* statistics of all the interrupts */
DWORD interruptTxStat = 0;
DWORD interruptRxStat = 0;
DWORD interruptOverRunStat = 0;
DWORD interruptRxTimeoutStat = 0;

/*****************************************************************************
** Function name:		SPI1Handler
**
** Descriptions:		SSP port is used for SPI communication.
**				SPI1(SSP) interrupt handler
**				The algorithm is, if RXFIFO is at least half full, 
**				start receive until it's empty; if TXFIFO is at least
**				half empty, start transmit until it's full.
**				This will maximize the use of both FIFOs and performance.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SPI1Handler (void) __irq 
{
    DWORD regValue;
  
    IENABLE;				/* handles nested interrupt */

    regValue = SSPMIS;
    if ( regValue & SSPMIS_RORMIS )	/* Receive overrun interrupt */
    {
	interruptOverRunStat++;
	SSPICR = SSPICR_RORIC;		/* clear interrupt */
    }
    if ( regValue & SSPMIS_RTMIS )	/* Receive timeout interrupt */
    {
	interruptRxTimeoutStat++;
	SSPICR = SSPICR_RTIC;		/* clear interrupt */
    }

    /* please be aware that, in main and ISR, CurrentRxIndex and CurrentTxIndex
    are shared as global variables. It may create some race condition that main
    and ISR manipulate these variables at the same time. SSPSR_BSY checking (polling)
    in both main and ISR could prevent this kind of race condition */
    if ( regValue & SSPMIS_RXMIS )	/* Rx at least half full */
    {
	interruptRxStat++;		/* receive until it's empty */
	while ( SSPSR & SSPSR_RNE )	
	{
	    SPIRDData[CurrentRxIndex++] = SSPDR;
	    /* Wait until the Busy bit is cleared */
	    while ( !(SSPSR & SSPSR_BSY) );
	}				/* interrupt will be cleared when */
					/* data register is read or written */	
    }
    if ( regValue & SSPMIS_TXMIS )	/* Tx at least half empty */
    {
	interruptTxStat++;		/* transmit until it's full */
	while ( SSPSR & SSPSR_TNF )
	{
	    SSPDR = SPIWRData[CurrentTxIndex++];
	    /* Wait until the Busy bit is cleared */
	    while ( !(SSPSR & SSPSR_BSY) );
	}				/* interrupt will be cleared when */
					/* data register is read or written */	
    }

    IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		SPI1Init
**
** Descriptions:		SPI1(SSP) port initialization routine
**				
** parameters:			None
** Returned value:		true or false, if the interrupt handler
**				can't be installed correctly, return false.
** 
*****************************************************************************/
DWORD SPI1Init( void )
{
    BYTE i, Dummy;

    /* Configure PIN connect block */
    /* bit 32, 54, 76 are 0x10, bit 98 are 0x00 */
    /* port 0 bits 17, 18, 19, 20 are SSP port SCK1, MISO1, MOSI1, and SSEL1 */
    /* set SSEL to GPIO pin that you will have the totoal freedom to set/reset 
    the SPI chip-select pin */
   
    SSPCR1 = SSPCR1_LBM; /* SSP master in loopback mode */

#if USE_CS
    PINSEL1 |= 0x02A8;
#else
    PINSEL1 |= 0x00A8;
    IODIR0 = SPI1_SEL;	/* SSEL is output */
    IOSET0 = SPI1_SEL;	/* set SSEL to high */
#endif
		
    /* Set PCLK to the same as CCLK */
    VPBDIV = 0x02;

    /* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */
    SSPCR0 = 0x0707;

    /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
    SSPCPSR = 0x2;

    for ( i = 0; i < FIFOSIZE; i++ )
    {
	Dummy = SSPDR;		/* clear the RxFIFO */
    }
       
    if ( install_irq( SPI1_INT, (void *)SPI1Handler ) == FALSE )
    {
	return (FALSE);
    }
    /* Device select as master, SSP Enabled, loopback operational mode */
    SSPCR1 = SSPCR1_LBM | SSPCR1_SSE;
    /* Set SSPINMS registers to enable interrupts */
    /* enable all interrupts, Rx overrun, Rx timeout, RX FIFO half full int, 
    TX FIFO half empty int */
    SSPIMSC = SSPIMSC_RORIM | SSPIMSC_RTIM | SSPIMSC_RXIM | SSPIMSC_TXIM;
    return( TRUE );
}

/*****************************************************************************
** Function name:		SPI1Send
**
** Descriptions:		Send a block of data to the SPI1(SSP) port, the 
**				first parameter is the buffer pointer, the 2nd 
**				parameter is the block length.
**
** parameters:			buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/
void SPI1Send( BYTE *buf, DWORD Length )
{
    DWORD i;
    for ( i = 0; i < Length; i++ )
    {
	/* as long as TNF bit is set (TxFIFO is not full), I can always transmit */
	while ( !(SSPSR & SSPSR_TNF) );
	SSPDR = *buf;
	buf++;
	CurrentTxIndex++;
	/* Wait until the Busy bit is cleared */
	while ( !(SSPSR & SSPSR_BSY) );
    }
    return; 
}

/*****************************************************************************
** Function name:		SPI1Receive
** Descriptions:		the module will receive a block of data from 
**				the SPI1(SSP), the 2nd parameter is the block 
**				length.
** parameters:			buffer pointer, and block length
** Returned value:		None
** 
*****************************************************************************/
void SPI1Receive( BYTE *buf, DWORD Length )
{
    DWORD i;
 
    for ( i = 0; i < Length; i++ )
    {
	/* As long as Receive FIFO is not empty, I can always receive. */
	/* since it's a loopback test, clock is shared for both TX and RX,
	no need to write dummy byte to get clock to get the data */
	/* if it's a peer-to-peer communication, SSPDR needs to be written
	before a read can take place. */
//	SSPDR = 0xFF;
	while ( !(SSPSR & SSPSR_RNE) );
	*buf = SSPDR;
	buf++;
	CurrentRxIndex++;
    }
    return; 
}

/******************************************************************************
**                            End Of File
******************************************************************************/

