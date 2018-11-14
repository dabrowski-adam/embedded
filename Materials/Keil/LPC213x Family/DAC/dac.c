/*****************************************************************************
 *   dac.c:  ADC module file for Philips LPC214x Family Microprocessors
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
** Function name:		DACInit
**
** Descriptions:		initialize DAC channel
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void DACInit( void )
{
    /* setup the related pin to DAC output */
    PINSEL1 = 0x00080000;	/* set p0.25 to DAC output */   
    return;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
