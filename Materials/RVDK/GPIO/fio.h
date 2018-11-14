/*****************************************************************************
 *   fio.h:  Header file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __FIO_H 
#define __FIO_H

#define GPIO0M		0x00000001
#define GPIO1M		0x00000002

#define FAST_PORT	0x01
#define REGULAR_PORT	0x02

extern void GPIOInit( DWORD PortType );

#endif /* end __FIO_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
