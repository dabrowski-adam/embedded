#ifndef _UART_H
#define _UART_H
#include <LPC213x.H>

extern void InitSerial(void);
extern void SendByte(unsigned char);
//extern int GetByte(void);
#endif
