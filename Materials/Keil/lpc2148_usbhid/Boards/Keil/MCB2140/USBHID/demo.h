/*----------------------------------------------------------------------------
 *      Name:    DEMO.H
 *      Purpose: USB HID Demo Definitions
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

/* Push Button Definitions */
#define PBINT  0x00004000  /* P0.14 */

/* LED Definitions */
#define LEDMSK 0x00FF0000  /* P1.16..23 */

/* HID Demo Variables */
extern BYTE InReport;
extern BYTE OutReport;

/* HID Demo Functions */
extern void GetInReport  (void);
extern void SetOutReport (void);
