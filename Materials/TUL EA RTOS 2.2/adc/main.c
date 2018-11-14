/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to read the analogue inputs.
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

#define AIN0 0
#define AIN1 1
#define AIN2 2
#define AIN3 3
#define AIN4 4
#define AIN5 5
#define AIN6 6
#define AIN7 7

#define VREF_IN_MV 2475


/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void delayMs(tU16 delayInMs);
static tU16 getAnalogueInput(tU8 channel);


/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Start a conversion of one selected analogue input and return
 *    10-bit result.
 *
 * Params:
 *    [in] channel - analogue input channel to convert.
 *
 * Return:
 *    10-bit conversion result
 *
 ****************************************************************************/
static tU16
getAnalogueInput(tU8 channel)
{
	//start conversion now (for selected channel)
	ADCR = (ADCR & 0xFFFFFF00) | (1 << channel) | (1 << 24);
	
	//wait til done
	while((ADDR & 0x80000000) == 0)
	  ;

	//get result and adjust to 10-bit integer
	return (ADDR>>6) & 0x3FF;
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
    ;
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
	tU32 integerResult;
	tU32 mvResult;

  //initialize printf()-functionality
  eaInit();
  
  //print welcome message
  printf("\n\n\n\n");
  printf("\n*********************************************************");
  printf("\n*");
  printf("\n* (C) 2005 Embedded Artists AB");
  printf("\n*");
  printf("\n* Welcome to this program that demonstrates how to read");
  printf("\n* two analogue inputs (AIN1 and AIN2).");
  printf("\n*");
  printf("\n*********************************************************");
  
  //set AIN1 = P0.28 and AIN2 = P0.29
  PINSEL1 = 0x05000000;
  
  //initialize ADC
  ADCR = (1 << 0)                             |  //SEL = 1, dummy channel #1
         ((CRYSTAL_FREQUENCY *
           PLL_FACTOR /
           VPBDIV_FACTOR) / 4500000 - 1) << 8 |  //set clock division factor, so ADC clock is 4.5MHz
         (0 << 16)                            |  //BURST = 0, conversions are SW controlled
         (0 << 17)                            |  //CLKS  = 0, 11 clocks = 10-bit result
         (1 << 21)                            |  //PDN   = 1, ADC is active
         (1 << 24)                            |  //START = 1, start a conversion now
         (0 << 27);							                 //EDGE  = 0, not relevant when start=1

  //short delay and dummy read
  delayMs(1);
  integerResult = ADDR;
   
  //enter forever loop... until the watchdog trig...
  while(1)
  {
  	//get AIN1 (P0.28)
  	integerResult = getAnalogueInput(AIN1);
  	
  	//calculate result in mV
  	mvResult  = integerResult * VREF_IN_MV;
    mvResult /= 1024;
  	
    printf("\nResult from AIN1: %d /1024*Vref = %d mV", integerResult, mvResult);
    
  	//get AIN2 (P0.29)
  	integerResult = getAnalogueInput(AIN2);
  	
  	//calculate result in mV
  	mvResult  = integerResult * VREF_IN_MV;
    mvResult /= 1024;
  	
    printf(",   AIN2: %d /1024*Vref = %d mV", integerResult, mvResult);
    
    //wait 500 ms
    delayMs(500);
  }

  return 0;
}
