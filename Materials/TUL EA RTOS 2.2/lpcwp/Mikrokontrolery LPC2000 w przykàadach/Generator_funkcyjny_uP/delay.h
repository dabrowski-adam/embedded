#ifndef _DELAY_H
#define _DELAY_H

extern void delay(unsigned int del_time);

//Switch delay
#define SW_DELAY	delay(0x8000)
#endif
