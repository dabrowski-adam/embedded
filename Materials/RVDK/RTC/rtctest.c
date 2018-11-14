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
    local_time.RTC_Hour = 0;
    local_time.RTC_Mday = 8;
    local_time.RTC_Wday = 3;
    local_time.RTC_Yday = 8;		/* current date 12/08/2005 */
    local_time.RTC_Mon = 12;
    local_time.RTC_Year = 2005;
    RTCSetTime( local_time );		/* Set local time */

    alarm_time.RTC_Sec = 0;
    alarm_time.RTC_Min = 0;
    alarm_time.RTC_Hour = 0;
    alarm_time.RTC_Mday = 1;
    alarm_time.RTC_Wday = 0;
    alarm_time.RTC_Yday = 1;		/* alarm date 01/01/2006 */
    alarm_time.RTC_Mon = 1;
    alarm_time.RTC_Year = 2006;
    RTCSetAlarm( alarm_time );		/* set alarm time */
   
    /* install RTC timer handler mainly for alarm control */
    if ( install_irq( RTC_INT, (void *)RTCHandler ) == FALSE )
    {
	while ( 1 );		/* very bad happened */
    }

    /* mask off alarm mask, turn on IMYEAR in the counter increment interrupt
    register */
    RTCSetAlarmMask(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);
    CIIR = IMMIN | IMYEAR;
    /* 2006/01/01/00:00:00 is the alarm on */
    
    RTCStart();

    while (1) 
    {					/* Loop forever */
	if ( alarm_on != 0 )
	{
	    alarm_on = 0;
	    /* Get current time when alarm is on */
	    current_time = RTCGetTime();	
	}
    }
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
