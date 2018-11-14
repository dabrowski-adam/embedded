/*****************************************************************************
 *   dactest.c:  main C entry file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC214x.h"                        /* LPC21xx definitions */
#include "type.h"
#include "dac.h"

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
    DWORD i = 0;

    /* Initialize DAC  */
    DACInit();

    while ( 1 )
    {
	DACR = (i << 6) | DAC_BIAS;
	i++;
	if ( i == 1024 )
	{
	    i = 0;
	}
    }
   
    return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

