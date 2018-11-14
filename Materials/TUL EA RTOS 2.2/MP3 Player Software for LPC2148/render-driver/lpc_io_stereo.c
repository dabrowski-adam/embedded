/*--------------------------------------------------------------------------*/
/* 
 * MP3 player DEMO - MPEG queue management functions
 * Copyright (C) 2006 MXP semicontuctor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY and WUTHOUT ANY SUPPORT; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: lpc_io.c 1.0 2006/10/16 09:41:32 rob Exp $
 */
/*--------------------------------------------------------------------------*/
#include  <targets/LPC2148.H>
#include  <string.h>
#include  "lpc_io.h"
#include  "PLL_calc.h"

#define  bool unsigned char

#define  STREAM_DECODED_SIZE   1024
#define  ENABLED                  1
#define  DISABLED                 0

typedef struct {
  short int raw[STREAM_DECODED_SIZE];	/* 16 bit PCM output samples [ch][sample] */
  volatile  unsigned short wr_idx;
  volatile  unsigned short rd_idx;
}decoded_stream_t;

static decoded_stream_t    DecodedBuff;
static bool                RenderStatus; /* Audio render status  */
                                          /* TRUE = render enabled , FALSE =render disabled */

#define DFIFO_DELETE()     (DecodedBuff.wr_idx = DecodedBuff.rd_idx = 0)
#define IS_DFIFO_FULL()    ((DecodedBuff.wr_idx+1)%STREAM_DECODED_SIZE == DecodedBuff.rd_idx)
#define IS_DFIFO_EMPTY()   (DecodedBuff.wr_idx == DecodedBuff.rd_idx)

#define DFIFO_WRITE(S)     do {                                                                   \
                             DecodedBuff.raw[DecodedBuff.wr_idx] = S;                             \
                             DecodedBuff.wr_idx = ( (DecodedBuff.wr_idx+1)%STREAM_DECODED_SIZE ); \
                           }while(0)

#define DFIFO_READ(S)      do {                                                                   \
                             S = DecodedBuff.raw[DecodedBuff.rd_idx];                             \
                             DecodedBuff.rd_idx = ( (DecodedBuff.rd_idx+1)%STREAM_DECODED_SIZE ); \
                           }while(0)

#define FIFO_LEN()        ( DecodedBuff.wr_idx >= DecodedBuff.rd_idx ? \
                            DecodedBuff.wr_idx - DecodedBuff.rd_idx  : \
                            DecodedBuff.wr_idx + (STREAM_DECODED_SIZE - DecodedBuff.rd_idx) )

void tc0 (void) __attribute__ ((interrupt));    // Manage timer Interrupt

void enable_audio_render(void)
{
    if (RenderStatus == DISABLED)
    {
      T0TCR = 1;                                /* Timer0 Enable */
      VICIntEnable = 0x00000010;                /* Enable Timer0 Interrupt */    
      RenderStatus = ENABLED;
    }
}

void disable_audio_render(void)
{
    if (RenderStatus == ENABLED)
    {
      T0TCR = 0;                                  /* Timer0 Disable */
      VICIntEnable = 0x00000000;                  /* Disable Timer0 Interrupt */    
      RenderStatus = DISABLED;
    }
}

void wait_end_of_excerpt(void)
{
    while (!IS_DFIFO_EMPTY());
    disable_audio_render();
}

inline void render_sample(short int sample)
{
    while (IS_DFIFO_FULL());
    // if (!IS_DFIFO_FULL())
      DFIFO_WRITE(sample);
    enable_audio_render();
}

void render_sample_block(short int *blk_ch1, short int *blk_ch2, unsigned short int len, unsigned char nch)
{
  unsigned short i;
  signed short sample;

  if (nch == 2)

    for (i=0; i<len; i++)
    {
      while (IS_DFIFO_FULL());
      //if (!IS_DFIFO_FULL())
      sample = ((signed int)*(blk_ch1+i) + (signed int)*(blk_ch2+i)) >> 1;
      DFIFO_WRITE(sample);      
    }

  else

    for (i=0; i<len; i++)
    {
      while (IS_DFIFO_FULL());
      //if (!IS_DFIFO_FULL())
      sample = *(blk_ch1+i);
      DFIFO_WRITE(sample);      
    }

    enable_audio_render();
}

/* Setup the Timer Counter 0 Interrupt */
void init_timer ()
{
    VICVectAddr0 = (unsigned long)tc0;            // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                      // use it for Timer 0 Interrupt
    VICIntEnable = 0x00000010;                    // Enable Timer0 Interrupt

    T0MCR = 3;                                    // Interrupt and Reset on MR0
}


/* Timer Counter 0 Interrupt executes sample time @ 60 MHz CPU Clock */
void tc0 (void)
{
  short int sample;
  unsigned short dacr;
      
  // TOGGLE_LIVE_LED3();
        
    if (!IS_DFIFO_EMPTY())
       DFIFO_READ(sample);
    else
    {
      TOGGLE_LIVE_LED2();
      sample = 0;  
    }
    
    dacr = (unsigned short)sample + (unsigned short)0x8000;
    DACR = dacr & 0xFFC0;				        // send a mono 10 bit sample to the DAC

    T0IR = 1;                                   // Clear interrupt flag
    VICVectAddr = 0;                            // Acknowledge Interrupt
}

void set_dac_sample_rate(unsigned int sample_rate)
{
  unsigned int _T0MCR;

    _T0MCR = ((unsigned int)cclk/sample_rate) - 1;
    T0MR0 = _T0MCR;
}

void init_IO(void)
{

#ifdef DEBUG
    IO1DIR  = 0x00FF0000;      /* P1.16..23 defined as Outputs  */
#endif
    
    DFIFO_DELETE();
    RenderStatus = DISABLED;

    init_timer();
    __ARMLIB_enableIRQ();     /* Enable global interrupt */
}
