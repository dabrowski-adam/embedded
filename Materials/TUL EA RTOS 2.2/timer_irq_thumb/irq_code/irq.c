/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    irq.c
 *
 * Description:
 *    Sample irq code, that must be compiled in ARM code.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
#include <lpc2xxx.h>
#include "irq.h"

/******************************************************************************
 * External variables
 *****************************************************************************/
extern tU32 systemTimeMs;
extern void (*pTimerCallback)(void);


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
void dummyISR(void) __attribute__ ((interrupt));
void timer0ISR(void) __attribute__ ((interrupt));


/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Dummy default interrupt handler (ISR).
 *
 ****************************************************************************/
void
dummyISR(void)
{
  //do something... for example print some serious error message

}


/*****************************************************************************
 *
 * Description:
 *    Actual timer ISR that is called whenever timer 0 generated an interrupt.
 *
 ****************************************************************************/
void
timer0ISR(void)
{
  //update system time (in ms)
  systemTimeMs += (1000 / SYSTEM_TIME_TICK);
  
  //call timer callback, if registered
  if (pTimerCallback != NULL)
    (*pTimerCallback)();

  T0IR        = 0xff;        //reset all IRQ flags
  VICVectAddr = 0x00;        //dummy write to VIC to signal end of interrupt
}
