/*****************************************************************************
 *   rtctest.c:  main C entry file for Philips LPC214x Family Microprocessors
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
#include "rtc.h"

extern DWORD alarm_on;
RTCTime local_time, alarm_time, current_time;

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	    		
    init_VIC();

    /* Initialize RTC module */
    RTCInit();

    local_time.RTC_Sec = 0;
    local_time.RTC_Min = 0;
    local_time.RTC_Hour = 11;
    local_time.RTC_Mday = 2;
    local_time.RTC_Wday = 4;
    local_time.RTC_Yday = 275;		/* current date 10/02/2008 @ 11:00:00 */
    local_time.RTC_Mon = 10;
    local_time.RTC_Year = 2008;
    RTCSetTime( local_time );		/* Set local time */

    alarm_time.RTC_Sec = 5;
    alarm_time.RTC_Min = 0;
    alarm_time.RTC_Hour = 11;
    alarm_time.RTC_Mday = 2;
    alarm_time.RTC_Wday = 4;
    alarm_time.RTC_Yday = 275;		/* alarm date 10/02/2008 @ 11:00:05 */
    alarm_time.RTC_Mon = 10;
    alarm_time.RTC_Year = 2008;
    RTCSetAlarm( alarm_time );		/* set alarm time */
   
    /* install RTC timer handler mainly for alarm control */
    if ( install_irq( RTC_INT, (void *)RTCHandler ) == FALSE )
    {
	while ( 1 );					/* something very bad happened */
    }

    /* mask off alarm mask, turn on IMYEAR in the counter increment interrupt
    register */
    RTCSetAlarmMask(~(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR));
    /* 2006/01/01/00:00:00 is the alarm on */


	// Configure as GPIO
	PINSEL1 = 0x00000000;		/* P1.16~23 GPIO output */
    SCS	= 0;					/* set GPIOx to use regular I/O */
    IODIR1 = 0x00FF0000;		/* P1.16..23 defined as Outputs */
    IOCLR1 = 0x00FF0000;		/* turn off LEDs */    


    RTCStart();

				
    while (1) 
    {							/* Loop forever */
	if ( alarm_on != 0 )
		{
	    alarm_on = 0;
	    /* Get current time when alarm is on */
		IOSET1 = 0x00FF0000;	/* turn on P1.16~19 */
	    current_time = RTCGetTime();
		}
    }
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
