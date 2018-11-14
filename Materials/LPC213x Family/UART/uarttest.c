/*****************************************************************************
 *   uarttest.c:  main C entry file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC214x.H"                        /* LPC21xx definitions */
#include "type.h"
#include "irq.h"
#include "uart.h"
#include <stdio.h>

extern DWORD UART0Count;
extern BYTE UART0Buffer[BUFSIZE];

/*****************************************************************************
**   Main Function  main()
*****************************************************************************/
int main (void)
{
    init_VIC();
    UARTInit(115200);	/* baud rate setting */
	printf("Hello\r");

	IODIR1  = 0x00FF0000;  
	IOSET1  = 0x00FF0000;  

    while (1) 
    {				/* Loop forever */
	if ( UART0Count != 0 )
	{
	    U0IER = IER_THRE | IER_RLS;				/* Disable RBR */
	    UARTSend( UART0Buffer, UART0Count );
	    UART0Count = 0;
	    U0IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
	}
    }
    return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/
