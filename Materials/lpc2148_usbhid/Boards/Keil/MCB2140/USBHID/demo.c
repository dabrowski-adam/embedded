/*----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Purpose: USB HID Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on Philips LPC2xxx microcontroller devices only. Nothing else gives
 *      you the right to use this software.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

#include <LPC214X.H>                        /* LPC214x definitions */

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"

#include "demo.h"


BYTE InReport;                              /* HID Input Report */
                                            /*   Bit0:    Button */
                                            /*   Bit1..7: Reserved */

BYTE OutReport;                             /* HID Out Report */
                                            /*   Bit0..7: LEDs */


/*
 *  Get HID Input Report -> InReport
 */

void GetInReport (void) {

  if ((IOPIN0 & PBINT) == 0) {              /* Check if PBINT is pressed */
    InReport = 0x01;
  } else {
    InReport = 0x00;
  }
}


/*
 *  Set HID Output Report <- OutReport
 */

void SetOutReport (void) {

  IOPIN1 = (IOPIN1 & ~LEDMSK) | (OutReport << 16);
}


/* Main Program */

int main (void) {

  IODIR1 = LEDMSK;                          /* LED's defined as Outputs */

  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1);                                /* Loop forever */
}
