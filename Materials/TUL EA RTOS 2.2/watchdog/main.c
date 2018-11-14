/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to start and trig the watchdog.
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

#define WD_TIMOUT_PERIOD_MS  15
#define LED_TO_FLASH         0x00000100

/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void delayMs(tU16 delayInMs);
static void wdtFeed(void);

/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Resets the watchdog.
 *
 ****************************************************************************/
static void
wdtFeed(void)
{
	WDFEED = 0xaa;
	WDFEED = 0x55;
}

/*****************************************************************************
 *
 * Description:
 *    Delay execution by a specified number of milliseconds by using
 *    timer #1. A polled implementation.
 *
 * Params:
 *    [in] delayInMs - the number of milliseconds to delay.
 *
 ****************************************************************************/
static void
delayMs(tU16 delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = delayInMs * ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (1000 * VPBDIV_FACTOR));
  TIMER1_IR  = 0xff;          //reset all interrrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer
  
  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01)
    wdtFeed();                //reset the watchdog
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
  printf("\n* Welcome to this program that demonstrates how to start");
  printf("\n* and trig the watchdog.");
  printf("\n*");
  printf("\n*********************************************************");
  
  //check reset cause (if bit WDTOF is set = wathdog caused reset)
  if ((WDMOD & 0x04) == 0x04)
    printf("\n\nReset of system was caused by a previous watchdog trip!!!\n");
  else
    printf("\n\nReset of system was not caused by the watchdog.\n");

  //set watchdog timeout period
  WDTC  = (tU32)((tU32)CRYSTAL_FREQUENCY * (tU32)PLL_FACTOR * (tU32)WD_TIMOUT_PERIOD_MS) / (tU32)((tU32)4 * (tU32)VPBDIV_FACTOR * (tU32)1000);
  printf("\n\nWatchdog timeout persiod set to %d ms.\n", WD_TIMOUT_PERIOD_MS);
  printf("This is equivalent to setting register WDTC = %d.\n", WDTC);

  //start the watchdog
  WDMOD = 0x00000003;    //Watchdog reset mode
  wdtFeed();             //start the watchdog
  
  printf("\n\nPress any key to stop the watchdog from being constantly reset...");
  //enter forever loop... until the watchdog trig...
  while(1)
  {
  	tU8 dummyRxChar;
  	
    //wait 20 ms
    delayMs(20);
    
    //set LED pin low (turn on LED)
    IOCLR = LED_TO_FLASH;
    
    //wait 20 ms
    delayMs(20);

    //set LED pin high (turn off LED)
    IOSET = LED_TO_FLASH;
    
    //check if any key has been pressed
    if (consolGetChar(&dummyRxChar) == TRUE)
    {
    	//wait until watchdog trips... and then reset
    	while(1)
    	  ;
    }
  }

  return 0;
}
