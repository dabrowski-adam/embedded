/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    irq.h
 *
 * Description:
 *    Contains interface definitions for interrupt routines
 *
 *****************************************************************************/
#ifndef _IRQ_H_
#define _IRQ_H_

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define SYSTEM_TIME_TICK  40    //40Hz tick frequency = 25 ms tick rate

/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
void dummyISR(void);
void timer0ISR(void);

#endif
