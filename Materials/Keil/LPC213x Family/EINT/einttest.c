/*****************************************************************************
 *   einttest.c:  main C entry file for Philips LPC214x Family Microprocessors
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
#include "extint.h"

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	    		
    init_VIC();

    /* initialize GPIO pins as external interrupts */        
    EINTInit();

    /****** It's an endless loop waiting for external interrupt ******/    
    /* On Keil MCB214x board, INT1 can be used to test the external interrupt */   
    while( 1 );
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
