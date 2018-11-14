/******************************************************************************/
/* WATCHDOG.C: Watchdog timer Example for LPC2106                             */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
                  
#include <LPC210x.H>                       /* LPC210x definitions */

extern void init_serial (void);            /* Initialize Serial Interface     */
extern int  sendchar (int ch);             /* Write character to Serial Port  */
extern int  getkey (void);                 /* Read character from Serial Port */

void feed_watchdog (void) {				   /* Reload the watchdog timer       */
  WDFEED = 0xAA;						   
  WDFEED = 0x55;  
}

void sendhex (int hex) {                   /* Write Hex Digit to Serial Port  */
  if (hex > 9) sendchar('A' + (hex - 10));
  else         sendchar('0' +  hex);
}

void sendstr (char *p) {                   /* Write string */
  while (*p) {
    sendchar (*p++);
  }
}

/* just waste time here	for demonstration */
void do_job (void)  {
  int i;
  for (i = 0; i < 10000; i++);
}

int main (void) {
  unsigned int i;

  init_serial();                           /* Initialize Serial Interface   */

  if( WDMOD & 0x04 ) {					   /* Check for watchdog time out   */
    sendstr("Watchdog Reset Occurred\n");
	WDMOD &= ~0x04;						   /* Clear time out flag           */
  }

  WDTC  = 0x2000;						   /* Set watchdog time out value     */
  WDMOD = 0x03;                            /* Enable watchdog timer and reset */   

  for(i = 0; i < 50; i++) {
    do_job ();     	                       /* the actual job of the CPU */
    feed_watchdog();					   /* restart watchdog timer */
  }

  while (1) {                              /* Loop forever */
    do_job ();                             /* the actual job of the CPU */
			                               /* no watchdog restart,
										      watchdog reset will occur! */
  }
}
