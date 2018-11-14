/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use a SWI handler.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <ea_init.h>
#include <framework.h>
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
static void delayMs(tU16 delayInMs);


/*****************************************************************************
 * External function prototypes
 ****************************************************************************/
extern void _swi_handler(void);


/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    installed C handle for SWI
 *
 ****************************************************************************/
void
mySwiHandler(tU32 number, tU32 *pReg)
{
	printf("\nmySwiHandler: number = %d", number);
	
	printf("\n  R0  = %x", pReg[0]);
	printf("\n  R1  = %x", pReg[1]);
	printf("\n  R2  = %x", pReg[2]);
	printf("\n  R3  = %x", pReg[3]);
	printf("\n  R4  = %x", pReg[4]);
	printf("\n  R5  = %x", pReg[5]);
	printf("\n  R6  = %x", pReg[6]);
	printf("\n  R7  = %x", pReg[7]);
	printf("\n  R8  = %x", pReg[8]);
	printf("\n  R9  = %x", pReg[9]);
	printf("\n  R10 = %x", pReg[10]);
	printf("\n  R11 = %x", pReg[11]);
	printf("\n  R12 = %x", pReg[12]);

  printf("\nThis handler execute in Supervisor mode, so keep it short...");
  printf("\n and do not call C library functions like shown here!\n\n");
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
	tU32 counter = 0;

  //initialize printf()-functionality
  eaInit();

  //print welcome message
  printf("\n*********************************************************");
  printf("\n*");
  printf("\n* (C) 2005 Embedded Artists AB");
  printf("\n*");
  printf("\n* Welcome to this program that demonstrates how to use");
  printf("\n* a SWI handler.");
  printf("\n*");
  printf("\n*********************************************************");

  //install own SPI handler (other than default handler)
  pISR_SWI = (unsigned int)_swi_handler;

  while(1)
  {
  	//It's not necessary to pass parameters
    asm volatile ("swi #1");
  	
  	//wait 1000 ms
    delayMs(1000);

    //It is also possible to pass (constant) parameters in the registers
    asm volatile ("mov r0,#0x12  \n\t"
                  "mov r1,#0x34  \n\t"
                  "mov r2,#0x56  \n\t"
                  "mov r3,#0x78  \n\t"
                  "swi #257      \n\t");
  	
  	//wait 1000 ms
    delayMs(1000);

    //...as well as dynamic paramters (watch the R0 paramter and how it increments)
    asm volatile ("mov r0,%0  \n\t"
                  "swi #10000 \n\t"
                  :
                  : "r" (counter)
                  : "r0"
                 );
    counter++;

  	//wait 1000 ms
    delayMs(1000);
  }

  return 0;
}
