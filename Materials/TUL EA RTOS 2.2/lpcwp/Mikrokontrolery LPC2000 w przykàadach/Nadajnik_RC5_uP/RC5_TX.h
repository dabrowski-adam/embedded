//************************************************************
//  File: RC5_TX.h
//  RC5 transmitter header
//  for ARM based microcontrollers.
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#ifndef _RC5_TX_H
#define _RC5_TX_H

extern void SendRC5(unsigned int Address, unsigned int Command);
extern void InitRC5_TX(void);
#endif
