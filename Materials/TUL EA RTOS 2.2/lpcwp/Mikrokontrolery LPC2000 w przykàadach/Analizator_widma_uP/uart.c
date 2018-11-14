#include "uart.h"

void InitSerial(void)
{
	PINSEL0=0x05;		//UART0
  	U0LCR = 0x83;		//8 bit, no Parity, 1 Stop bit
  	U0DLL = 33;			//BAUDRATE=115200bps @ VPBfreq=60MHz
  	U0LCR = 0x03; 
}

void SendByte(unsigned char ch)
{
	U0THR=ch;
	while(!(U0LSR & 0x20));
}

/*int GetByte(void)
{
  while (!(U0LSR & 0x01));

  return (U0RBR);
}*/
