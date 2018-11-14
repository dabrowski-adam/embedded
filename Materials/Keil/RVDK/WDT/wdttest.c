/*****************************************************************************
 *   wdttest.c:  main C entry file for Philips LPC214x Family Microprocessors
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
#include "wdt.h"

extern DWORD timer_counter;
DWORD feed_counter = 0;

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	    		
    /*** The main Function is an endless loop ****/	
    init_VIC();
    init_timer();
    
    WDTInit();
    enable_timer( 0 );
       
    while( 1 ) 
    {
	if ( timer_counter > 0 )
	{
	    /* Feed the watchdog timer */
	    feed_counter++;
	    WDTFeed();
	    timer_counter = 0;
	}
    }
    return 0;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
