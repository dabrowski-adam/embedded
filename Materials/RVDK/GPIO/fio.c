/*****************************************************************************
 *   fio.c:  Fast I/O module file for Philips LPC214x Family Microprocessors
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
#include "timer.h"
#include "fio.h"

/*****************************************************************************
** Function name:		GPIOInit
**
** Descriptions:		initialize GPIO port
**
** parameters:			port type, Fast I/O or regular GPIO
** Returned value:		None
** 
*****************************************************************************/
void GPIOInit( DWORD PortType )
{
    if ( PortType == FAST_PORT )
    {
	SCS |= (GPIO0M | GPIO1M);	/* set GPIOx to use Fast I/O */
	FIO1DIR = 0x00FF0000;
	FIO1CLR	= 0x00FF0000;
    }
    else
    {
	SCS = 0;			/* set GPIOx to use regular I/O */
	IODIR1 = 0x00FF0000;		/* P1.16..23 defined as Outputs */
	IOCLR1 = 0x00FF0000;		/* turn off LEDs */
    }
    return;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
