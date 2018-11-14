/*****************************************************************************
 *   adctest.c:  main C entry file for Philips LPC214x Family Microprocessors
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
#include "target.h"
#include "irq.h"
#include "adc.h"

extern DWORD ADC0Value[ADC_NUM], ADC1Value[ADC_NUM];
extern DWORD ADC0IntDone, ADC1IntDone;

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
    DWORD i;

    init_VIC();
    					/* Initialize ADC  */
    ADCInit( ADC_CLK );

	i = AD0STAT;
	i = AD1STAT;

#if ADC_INTERRUPT_FLAG			/* Interrupt driven */
    for ( i = 0; i < ADC_NUM; i++ ){
		ADC0Read( i );
		while ( !ADC0IntDone );
		ADC0IntDone = 0;
    }
    for ( i = 0; i < ADC_NUM; i++ ){
		ADC1Read( i );
		while ( !ADC1IntDone );
		ADC1IntDone = 0;
    }
#else
  	
					/* Polling */
	
	for ( i = 0; i < ADC_NUM; i++ )
    {
			ADC0Value[i] = ADC0Read( i );
    }
    for ( i = 0; i < ADC_NUM; i++ )
    {
			ADC1Value[i] = ADC1Read( i );
    }
#endif
    return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

