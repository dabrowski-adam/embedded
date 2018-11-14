/*****************************************************************************
 *   tmrtest.c:  main C entry file for Philips LPC214x Family Microprocessors
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

extern DWORD timer_counter;

/*****************************************************************************
**   Main Function  main()
*****************************************************************************/
int main (void)
{	    		
    /************ The main Function is an endless loop ************/
    /* The timer routine is tested on the Keil MCB214x board */
    IODIR1 = 0x00FF0000;		/* P1.16..23 defined as Outputs */
    IOCLR1 = 0x00FF0000;		/* turn off all the LEDs */
    init_VIC();
    init_timer();

    enable_timer( 0 );

    while (1) 
    {					/* Loop forever */
	if ( timer_counter > 0x100 )
	{
  	    IOSET1 = 0x000F0000;	/* turn off P1.20~23 */	
	    IOCLR1 = 0x00F00000;	/* turn on P1.16~19 */
	    if ( timer_counter > 0x200 )
	    {
		timer_counter = 0;
	    }
	}
	else
	{
	    IOSET1 = 0x00F00000;	/* turn on P1.20~23 */
	    IOCLR1 = 0x000F0000;	/* turn off P1.16~19 */
	}
    }
    return 0;
}

/*****************************************************************************
**                            End Of File
******************************************************************************/
