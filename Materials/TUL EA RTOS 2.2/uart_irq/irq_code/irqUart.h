/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    irqUart.h
 *
 * Description:
 *    Contains interface definitions for interrupt routines
 *
 *****************************************************************************/
#ifndef _IRQUART_H_
#define _IRQUART_H_

/*****************************************************************************
 * External variables
 ****************************************************************************/
extern tU8 uart0TxBuf[];
extern volatile tU32 uart0TxHead;
extern volatile tU32 uart0TxTail;
extern volatile tU8  uart0TxRunning;

extern tU8 uart0RxBuf[];
extern volatile tU32 uart0RxHead;
extern volatile tU32 uart0RxTail;

/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
void uart0ISR(void);

tU32 disIrq(void);
void restoreIrq(tU32 restoreValue);


#endif
