/******************************************************************************/
/* BLINKY.C: LED Flasher                                                      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
                  
#include <stdio.h>                         /* standard I/O .h-file */
#include <LPC213x.H>                       /* LPC213x definitions  */
#include <math.h>

extern void init_serial (void);            /* Initialize Serial Interface     */

int volatile timeval;


/*
 * ----- Parameters and Variables for Sine Wave Generator ----- 
 */
 
unsigned long OutFreq = 480l;       /* Output Frequency (Range  1Hz - 4000Hz) */
#define OutAmpl 600                 /* Output Amplitute (Range  0 - 0.99)     */
#define PI  3.1415926
#define T0_RlFreq  8000             /* Timer 0 Reload Frequency               */

struct tone  {                      /* struct for Sine Wave Generator Signal  */
  int  cos;                         /* cosine factor                          */
  long y1;                          /* y[-1] value                            */
  int  y2;                          /* y[-2] value                            */
};                                                                            

int  NewFreq;                       /* New Frequency Setup indication         */

short tval;
signed char cval;
struct tone Tone;


/*
 * Generate Sine Wave Tone
 */
static void Generate_Sine (struct tone *t)  {
  int y;
  y     = (t->cos * (t->y1 >> 14)) - t->y2;
  t->y2 = t->y1;
  t->y1 = y;
  tval  = t->y1 >> 16;
  cval  = tval;
}


/*
 * Initialize the Sine Wave Tone Generator
 */
__inline void Init_Sine (int OutFreq) {
  Tone.cos = (cos (2*PI*((float)OutFreq/T0_RlFreq))) * 32768;
  Tone.y1  = 0;
  Tone.y2  = (sin (2*PI*((float)OutFreq/T0_RlFreq)) * OutAmpl) * 32768; 
}


/* Timer Counter 0 Interrupt executes each 10ms @ 60 MHz CPU Clock */
__irq void tc0 (void) {
  ++timeval;

  if (NewFreq)  {   /* The frequency changes when a new ADC value is measured */
    Init_Sine (NewFreq);
    NewFreq = 0;
  }

  Generate_Sine (&Tone);
  DACR = ((Tone.y1 >> 10) & 0xFFC0) + 0x8000;   
  T0IR        = 1;                             /* Clear interrupt flag        */
  VICVectAddr = 0;                             /* Acknowledge Interrupt       */  
}


/* Setup the Timer Counter 0 Interrupt */
void init_timer (void) {
  T0MR0 = 1874;                                /* 0.125uSec = 1875-1 counts   */
  T0MCR = 3;                                   /* Interrupt and Reset on MR0  */
  T0TCR = 1;                                   /* Timer0 Enable               */
  VICVectAddr0 = (unsigned long)tc0;           /* set interrupt vector in 0   */
  VICVectCntl0 = 0x20 | 4;                     /* use it for Timer0 Interrupt */
  VICIntEnable = 0x00000010;                   /* Enable Timer0 Interrupt     */
}


void delay (void) {                         /* Delay function */
  unsigned int cnt;
  unsigned int val;
  static unsigned int oldval;  
  int diff;

  AD0CR |= 0x01200000;                      /* Start A/D Conversion */
  do {
    val = AD0DR;                            /* Read A/D Data Register */
  } while ((val & 0x80000000) == 0);        /* Wait for end of A/D Conversion */
  AD0CR &= ~0x01000000;                     /* Stop A/D Conversion */
  val = (val >> 6) & 0x03FF;                /* Extract AIN0 Value */

  diff = val - oldval;
  if (diff > 10 || diff < -10)  {
    oldval = val;
    NewFreq = 1200 - val;
  }
 
  printf ("\nAIN1 Result = 0x%03X", val);   /* Output A/D Conversion Result */

  val = (val >> 2) << 12;                   /* Adjust Delay Value */
  for (cnt = 0; cnt < val; cnt++);          /* Delay */
}


int main (void) {
  unsigned int n;

  IODIR1  = 0x00FF0000;                     /* P1.16..23 defined as Outputs  */
  AD0CR   = 0x00200402;                     /* Setup A/D: 10-bit AIN0 @ 3MHz */
  PINSEL1 = 0x01080000;                     /* enable DAC */

  init_serial();                            /* Initialize Serial Interface   */
  init_timer ();
  
  while (1) {                               /* Loop forever */
    for (n = 0x00010000; n <= 0x00800000; n <<= 1) {
      /* Blink LED 0, 1, 2, 3, 4, 5, 6, 7 */
      IOSET1 = n;                           /* Turn on LED */
      delay();                              /* Delay */
      IOCLR1 = 0x00FF0000;                  /* Turn off LEDs */
    }
  }
}
