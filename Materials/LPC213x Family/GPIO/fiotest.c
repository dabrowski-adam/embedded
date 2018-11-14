/*****************************************************************************
 *   fiotest.c:  main C entry file for Philips LPC214x Family Microprocessors
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

extern DWORD timer_counter;

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
    DWORD counter = 0;
    DWORD flip_port = REGULAR_PORT;
	    		
    /********* The main Function is an endless loop ***********/	
    init_VIC();
    init_timer();

    /* Initialize GPIO pins, default is REGULAR_PORT  */
    GPIOInit( REGULAR_PORT );

    /* Initialize timer for GPIO toggling timing */
    enable_timer( 0 );

    /* From the blink speed of the LED, it won't tell the difference between
    the regular port and fast port unless a scope is applied. This demo is
    show how the regular and fast port should be initialized and the level
    of the port can be changed. */        
    while( 1 ) 
    {
	if ( flip_port == REGULAR_PORT )
	{
	    if ( counter == 0 )
	    {
		GPIOInit( REGULAR_PORT );	
		/* only need to do once when port is switched. */	
	    }
	    if ( timer_counter >= 0x20 * counter )
	    {
		IOSET1 = 1 << (16 + counter);
		counter++;
		if ( counter > 8 )
		{
		    counter = 0;
		    timer_counter = 0;
		    IOCLR1 = 0x00FF0000;
		    flip_port = FAST_PORT;	/* switch port from regular to fast */
		}
	    }
	}
	if ( flip_port == FAST_PORT )
	{
	    if ( counter == 0 )
	    {
		GPIOInit( FAST_PORT );		
		/* only need to do once when port is switched. */
	    }
	    if ( timer_counter >= 0x20 * counter )
	    {
		FIO1SET = 1 << (16 + counter);
		counter++;
		if ( counter > 8 )
		{
		    counter = 0;	
		    timer_counter = 0;
		    FIO1CLR = 0x00FF0000;
		    flip_port = REGULAR_PORT;	/* switch port from fast to regular */
		}
	    }
	}
    }
    return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

