/*****************************************************************************
 *   adc.h:  Header file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __ADC_H 
#define __ADC_H

#define ADC_INTERRUPT_FLAG	0	/* 1 is interrupt driven, 0 is polling */
/* NOTE: The ADC Interrupt will work only with Rev /01 as outlined in the usermanual */


/*
This flag indicates revision value
	1: Rev /01
	0: Rev /-
Rev /01 is backwards compatible with Rev /-
Rev /01 has dedicated dataregisters whereas Rev /- does not.
*/
#define ADC_REV_DASH_FLAG 0

#define ADC_OFFSET	0x10
#define ADC_INDEX	4

#define ADC_DONE	0x80000000
#define ADC_OVERRUN	0x40000000
#define ADC_ADINT	0x00010000

#define ADC_NUM		8			/* for LPC2146/8 */
#define ADC_CLK		1000000		/* set to 1Mhz */

extern void ADC0Handler( void ) __irq;
extern void ADC1Handler( void ) __irq;
extern DWORD ADCInit( DWORD ADC_Clk );
extern DWORD ADC0Read( BYTE channelNum );
extern DWORD ADC1Read( BYTE channelNum );

#endif /* end __ADC_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
