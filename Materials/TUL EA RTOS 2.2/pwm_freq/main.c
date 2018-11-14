/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to generate a PWM signal
 *    with a given frequency.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include "startup/config.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD


/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void initPwm(tU32 initialFreqValue);
static void setPwmFrequency(tU32 freqValue);
static void delayMs(tU16 delayInMs);

/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Initialize the PWM unit to generate a variable frequency signal on
 *    PWM2. Connect signal PWM2 to pin P0.7.
 *
 * Params:
 *    [in] initialFreqValue - the initial frequency value. Value calculated as:
 *
 *                     (crystal frequency * PLL multiplication factor)
 *                     -----------------------------------------------
 *                           (VPBDIV factor * desired frequency)
 *
 ****************************************************************************/
static void
initPwm(tU32 initialFreqValue)
{
  /*
   * initialize PWM
   */
  PWM_PR  = 0x00000000;             //set prescale to 0
  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
  PWM_MR2 = initialFreqValue / 2;   //MR2 = duty cycle control
  PWM_LER = 0x05;                   //latch new values for MR0 and MR2
  PWM_PCR = 0x0400;                 //enable PWM2 in single edge control mode
  PWM_TCR = 0x09;                   //enable PWM and Counter

  /*
   * connect signal PWM2 to pin P0.7
   */
  PINSEL0 &= ~0x0000c000;  //clear bits related to P0.7
  PINSEL0 |=  0x00008000;  //connect signal PWM2 to P0.7 (second alternative function)
}

/*****************************************************************************
 *
 * Description:
 *    Update the value of the PWM signal. Always sets the duty cycle to 50%.
 *
 * Params:
 *    [in] freqValue - the new frequency value. Value calculated as:
 *                     (crystal frequency * PLL multiplication factor)
 *                     -----------------------------------------------
 *                           (VPBDIV factor * desired frequency)
 *
 ****************************************************************************/
static void
setPwmFrequency(tU32 freqValue)
{
  PWM_MR0 = freqValue;        //set frequency
  PWM_MR2 = freqValue / 2;    //set 50% duty cycle
  PWM_LER = 0x05;             //latch new values for MR0 and MR2
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
  volatile tU32 freq = 15000000;

  //initialize PWM unit
  initPwm(freq);

  //vary frequency
  while(1)
  {
    //set frequency value
    setPwmFrequency(freq);
    
    //wait 2 second
    delayMs(2000);
    
    //update frequency value (100..100000, in steps of 100)
    freq -= 1000000;
    if (freq <= 2000000)
      freq = 15000000;
  }

  return 0;
}
