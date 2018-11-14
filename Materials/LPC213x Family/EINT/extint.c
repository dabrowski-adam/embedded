/*****************************************************************************
 *   extint.c:  Target C file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC214x.H"                        /* LPC21xx definitions */
#include "type.h"
#include "irq.h"
#include "extint.h"

DWORD eint1_counter;

/*****************************************************************************
** Function name:		EINT1_Handler
**
** Descriptions:		external INT handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void EINT1_Handler (void) __irq 
{
    EXTINT = EINT1;		/* clear interrupt */
		
    IENABLE;			/* handles nested interrupt */
    eint1_counter++;
    if ( eint1_counter & 0x01 )	/* alternate the LED display */
    {
	IOSET1 = 0x000F0000;	/* turn off P1.20~23 */	
	IOCLR1 = 0x00F00000;	/* turn on P1.16~19 */
    }
    else
    {
	IOSET1 = 0x00F00000;	/* turn on P1.20~23 */	
	IOCLR1 = 0x000F0000;	/* turn off P1.16~19 */
    }
    IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		EINTInit
**
** Descriptions:		Initialize external interrupt pin and
**				install interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the interrupt
**				handler can't be installed to the VIC table.
** 
*****************************************************************************/
DWORD EINTInit( void )
{
    PINSEL0 = 0x20000000;		/* set P0.14 as EINT1 */
    PINSEL1 = 0x00000000;		/* P1.16~23 GPIO output */
    SCS	= 0;				/* set GPIOx to use regular I/O */
    IODIR1 = 0x00FF0000;		/* P1.16..23 defined as Outputs */
    IOCLR1 = 0x00FF0000;		/* turn off LEDs */

    EXTMODE = EINT1_EDGE;		/* INT1 edge trigger */
    EXTPOLAR = 0;			/* INT1 is falling edge by default */
     
    if ( install_irq( EINT1_INT, (void *)EINT1_Handler ) == FALSE )
    {
	return (FALSE);
    }
        
    return( TRUE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

