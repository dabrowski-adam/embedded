/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use a timer to generate
 *    interrupts and to keep a system time base.
 *
 *    Observe that at least the interrupt rountines must be compiled in
 *    ARM code, as opposed to THUMB code. The IRQ routines are compiled
 *    in a separate library (irq_code).
 *
 *    Also observe that the startup code framwork must handle the VIC and
 *    dispatch the correct interrupt ISRs. This can be done with Embedded
 *    Artists' startup framework by setting the define: IRQ_HANDLER to 1
 *    (meaning: Load vector directly from VIC, i.e., LDR PC,[PC,#-0xFF0]).
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <ea_init.h>
#include "startup/config.h"
#include "irq_code/irq.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

/******************************************************************************
 * Local variables
 *****************************************************************************/
tU32 systemTimeMs;
void (*pTimerCallback)(void);

/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);
tU32 getSystemTime(void);

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void initVIC(void (*pDefaultISR)());
static void initTimer0(void (*pISR)(), void (*pCallback)());


/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    This function initialize the Vectored Interrupt Controller (VIC) to
 *    a known state. All interrupts are vectored to a default interrupt
 *    handler.
 *
 * Params:
 *    [in] pDefaultISR - function pointer to the default interrupt ISR.
 *
 ****************************************************************************/
static void
initVIC(void (*pDefaultISR)())
{
  VICIntSelect   = 0x00000000;         //all IRQs are assigned to IRQ (not FIQ)
  VICIntEnClr    = 0xffffffff;         //disable all interrupts
  VICSoftIntClr  = 0xffffffff;         //clear all pending software interrupts
  VICProtection  = 0x00000000;         //VIC protection disabled, registers can
                                       // be accessed in User and Privileged mode
  VICVectAddr    = 0x00000000;         //clear any pending/active interrupts
  VICDefVectAddr = (tU32)pDefaultISR;  //set default ISR address for for non-vectored IRQs
  
  //reset/disable all vectored IRQ slots
  VICVectCntl0   = 0x00000000;
  VICVectCntl1   = 0x00000000;
  VICVectCntl2   = 0x00000000;
  VICVectCntl3   = 0x00000000;
  VICVectCntl4   = 0x00000000;
  VICVectCntl5   = 0x00000000;
  VICVectCntl6   = 0x00000000;
  VICVectCntl7   = 0x00000000;
  VICVectCntl8   = 0x00000000;
  VICVectCntl9   = 0x00000000;
  VICVectCntl10  = 0x00000000;
  VICVectCntl11  = 0x00000000;
  VICVectCntl12  = 0x00000000;
  VICVectCntl13  = 0x00000000;
  VICVectCntl14  = 0x00000000;
  VICVectCntl15  = 0x00000000;
}

/*****************************************************************************
 *
 * Description:
 *    Initialize Timer #0 (and VIC) for generating system tick interrupts.
 *
 * Params:
 *    [in] pISR      - function pointer to timer ISR.
 *    [in] pCallback - function pointer to timer callback function.
 *                     NULL if no callback function shall be registered.
 *
 ****************************************************************************/
static void
initTimer0(void (*pISR)(), void (*pCallback)())
{
  //store timer callback function pointer
  pTimerCallback = pCallback;
  
  //initialize VIC for Timer0 interrupts
  VICIntSelect &= ~0x10;       //Timer0 interrupt is assigned to IRQ (not FIQ)
  VICVectAddr4  = (tU32)pISR;  //register ISR address
  VICVectCntl4  = 0x24;        //enable vector interrupt for timer0
  VICIntEnable  = 0x10;        //enable timer0 interrupt
  
  //initialize and start Timer0
  T0TCR = 0x00000002;                           //disable and reset Timer0
  T0PC  = 0x00000000;                           //no prescale of clock
  T0MR0 = (CRYSTAL_FREQUENCY * PLL_FACTOR) /    //calculate no of timer ticks
          (SYSTEM_TIME_TICK * VPBDIV_FACTOR);   //for a given system tick rate
  T0IR  = 0x000000ff;                           //reset all flags before enable IRQs
  T0MCR = 0x00000003;                           //reset counter and generate IRQ on MR0 match
  T0TCR = 0x00000001;                           //start Timer0
  
  //reset system time
  systemTimeMs = 0;
}


/*****************************************************************************
 *
 * Description:
 *    A callback function that is called every time a timer interrupt occurs.
 *    Observe that the function executes in interrupt context.
 *
 ****************************************************************************/
static void
myCallback(void)
{
	static tU8 ioFlag = FALSE;
	
	//do something...
  //...and remember that this function will be executed in interrupt
  //context so keep it short... printf():s are generally not a good idea...
  
  if (ioFlag == FALSE)
  {
  	IOSET = 0x00000100;
  	ioFlag = TRUE;
  }
  else
  {
  	IOCLR = 0x00000100;
  	ioFlag = FALSE;
  }
}

/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Returns the current number of ms from system start.
 *
 * Returns:
 *    tU32 - number of milliseconds since system start.
 *
 ****************************************************************************/
tU32
getSystemTime(void)
{
  return systemTimeMs;
}


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
  //initialize the VIC and register the default ISR handler
  initVIC(dummyISR);
  
  //initialize Timer #0 and start system time counting
  initTimer0(timer0ISR, myCallback);
  
  //initialize printf()-functionality
  eaInit();

  //set IOPIN P0.8 as output (for sample application)
	IODIR = 0x00000100;

  while(1)
  {
    tU32 currentTime;
    
    //pause for 100 ms by taking a time stamp
    currentTime = getSystemTime();
    while ((getSystemTime() - currentTime) < 100)
      ;
      
    //do something...
    printf("!");
  }

  return 0;
}
