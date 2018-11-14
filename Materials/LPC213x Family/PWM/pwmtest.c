/*****************************************************************************
 *   pwmtest.c:  main C entry file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC214x.h"                        /* LPC21xx definitions */
#include "type.h"
#include "irq.h"
#include "timer.h"
#include "pwm.h"

extern DWORD timer_counter;
extern DWORD match_counter;

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	    
    DWORD cycle = PWM_CYCLE, offset = 0;

    init_VIC();

    if ( PWM_Init( 0 ) != TRUE )
    {
	while( 1 );			/* fatal error */
    }

    PWM_Set( cycle, offset );		 //1200, 0
    PWM_Start();

    while ( 1 )
    {
	if ( match_counter != 0 )
	{
	    match_counter = 0;
	    if( offset <= PWM_CYCLE )
		offset += PWM_OFFSET;
	    else
		offset = 0;
	    PWM_Set( cycle, offset );
	}
    }
    PWM_Stop() ;
	
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
