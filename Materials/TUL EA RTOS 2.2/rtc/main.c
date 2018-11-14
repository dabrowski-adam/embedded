/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use the real-time clock.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <ea_init.h>
#include <consol.h>
#include "startup/config.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define LED_TO_FLASH      0x00000100


/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void initRtc(tU8 use32khzCrystal, tU16 year, tU8 month, tU8 day, tU8 hour, tU8 minute, tU8 second);


/*****************************************************************************
 * Local variables
 ****************************************************************************/
static tU16 rtcYear;
static tU8  rtcMonth;
static tU8  rtcDay;
static tU8  rtcHour;
static tU8  rtcMinute;
static tU8  rtcSecond;
static tU8  use32khzCrystal;


/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Initialize the real-time clock to the specified time.
 *
 * Params:
 *    [in] year   - the current year
 *    [in] month  - the current month
 *    [in] day    - the current day
 *    [in] hour   - the current hour
 *    [in] minute - the current minute
 *    [in] second - the current second
 *
 ****************************************************************************/
static void
initRtc(tU8 use32khzCrystal, tU16 year, tU8 month, tU8 day, tU8 hour, tU8 minute, tU8 second)
{
  RTC_CCR  = 0x00000002;   //reset clock
  RTC_ILR  = 0x00000000;   //clear interrupt flags
  RTC_CIIR = 0x00000001;   //set interrupt flag once each second
  RTC_AMR  = 0x000000ff;   //do not compare any alarm

  if (use32khzCrystal == 0)
  {
    RTC_PREINT  = ((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / 32768 - 1;
    RTC_PREFRAC = ((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) - 
                  (((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / 32768) * 32768;
  }
  
  RTC_YEAR  = year;
  RTC_MONTH = month;
  RTC_DOM   = day;
  RTC_HOUR  = hour;
  RTC_MIN   = minute;
  RTC_SEC   = second;
  
  if (use32khzCrystal == 0)
    RTC_CCR  = 0x00000001;   //start real-time clock, use core clock
  else
    RTC_CCR  = 0x00000011;   //start real-time clock, use 32kHz crystal
}

/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    The main-function. 
 *
 * Returns:
 *    Always 0, since return value is not used.
 *
 ****************************************************************************/
int
main(void)
{
  //initialize printf()-functionality
  eaInit();
  
  //set one IO-pin as an output, and set the pin high (= turn off LED)
  IODIR = LED_TO_FLASH;
  IOSET = LED_TO_FLASH;
  
  //print welcome message
  printf("\n\n\n\n");
  printf("\n*********************************************************");
  printf("\n*");
  printf("\n* (C) 2005 Embedded Artists AB");
  printf("\n*");
  printf("\n* Welcome to this program that demonstrates usage of");
  printf("\n* the real-time clock.");
  printf("\n*");
  printf("\n*********************************************************");
  
  //initialize the real-time clock
  printf("\n\nWhat year is it?   : ");
  rtcYear = consolGetIntNum();
  printf("\nWhat month is it?  : ");
  rtcMonth = consolGetIntNum();
  printf("\nWhat day is it?    : ");
  rtcDay = consolGetIntNum();
  printf("\nWhat hour is it?   : ");
  rtcHour = consolGetIntNum();
  printf("\nWhat minute is it? : ");
  rtcMinute = consolGetIntNum();
  printf("\nWhat second is it? : ");
  rtcSecond = consolGetIntNum();

  printf("\n\nType 0 to use processor core crystal or 1 to use 32kHz crystal: ");
  use32khzCrystal = consolGetIntNum();
 
  initRtc(use32khzCrystal, rtcYear, rtcMonth, rtcDay, rtcHour, rtcMinute, rtcSecond);

  //enter forever loop...
  while(1)
  {
  	//wait for 1 second to pass
  	while(0 == (RTC_ILR & 0x01))
  	  ;
  	RTC_ILR = 0x01;      //reset interrupt flag
  	
  	printf("\nCurrent time is [%d-%d-%d, %d:%d:%d]", RTC_YEAR, RTC_MONTH, RTC_DOM, RTC_HOUR, RTC_MIN, RTC_SEC); 
    
    //toggle io pin
    if (IOPIN & LED_TO_FLASH)
      //set LED pin low (turn on LED)
      IOCLR = LED_TO_FLASH;
    else
      //set LED pin high (turn off LED)
      IOSET = LED_TO_FLASH;
  }

  return 0;
}
