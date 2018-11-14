/*--------------------------------------------------------------------------*/
/*
 * MP3 player DEMO - MPEG queue management functions interface file
 * Copyright (C) 2006 MXP semicontuctor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: lpc_io.c 1.0 2006/10/16 09:41:32 rob Exp $
 */
/*--------------------------------------------------------------------------*/
#ifndef  __IO_H
#define  __IO_H

#ifdef  DEBUG 
#define TOGGLE_LIVE_LED0()   ((IO1SET & 0x00010000) ? (IO1CLR=0x00010000) : (IO1SET=0x00010000))
#define TOGGLE_LIVE_LED1()   ((IO1SET & 0x00020000) ? (IO1CLR=0x00020000) : (IO1SET=0x00020000))
#define TOGGLE_LIVE_LED2()   ((IO1SET & 0x00040000) ? (IO1CLR=0x00040000) : (IO1SET=0x00040000))
#define TOGGLE_LIVE_LED3()   ((IO1SET & 0x00080000) ? (IO1CLR=0x00080000) : (IO1SET=0x00080000))
#define TOGGLE_LIVE_LED4()   ((IO1SET & 0x00100000) ? (IO1CLR=0x00100000) : (IO1SET=0x00100000))
#define SET_LIVE_LED4()      (IO1SET=0x00100000)
#define CLR_LIVE_LED4()      (IO1CLR=0x00100000) 
#else
#define TOGGLE_LIVE_LED0()   
#define TOGGLE_LIVE_LED1()   
#define TOGGLE_LIVE_LED2()   
#define TOGGLE_LIVE_LED3()   
#define TOGGLE_LIVE_LED4()   
#define SET_LIVE_LED4()      
#define CLR_LIVE_LED4()      
#endif

void init_IO(void);
void set_dac_sample_rate(unsigned int sample_rate);
inline void render_sample(short int sample);
void wait_end_of_excerpt(void);
void render_sample_block(short int *blk_ch1, short int *blk_ch2, unsigned short int len, unsigned char nch);
void disable_audio_render(void);
void enable_audio_render(void);

#endif
