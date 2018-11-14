/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *	
 * Przyklad obslugi przetwornika A/C              *
***************************************************/

#include "lpc214x.h"
#include "uart.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define P028_ADC01_SEL (1<<24)
#define P029_ADC02_SEL (1<<26)

#define ADC0_VIC (1<<18)
#define ADC0_VIC_BIT 18
#define VIC_IRQSLOT_EN (1<<5)


#define AD0CR_CLKDIV_BIT 8
#define AD0CR_PDN_BIT 21
#define AD0CR_BURST 0x10000

/* Initialize ADC */
inline static void initADC(void)
{
	AD0CR = 60<<AD0CR_CLKDIV_BIT | 1<<AD0CR_PDN_BIT |2|4;
	PINSEL1 |= P028_ADC01_SEL | P029_ADC02_SEL;
	AD0CR |= AD0CR_BURST;
}

static int AdcKty,AdcPot;

/* Przerwanie od przetwornika AC */
static void AdcIrq(void) __attribute__ ((interrupt("IRQ")));
void AdcIrq(void)
{
	unsigned int r,ch;
	r = AD0GDR;
	ch = (r>>24) & 0x07;
	if(ch==1) AdcPot = (r>>6) & 0x03FF;
	else if(ch==2) AdcKty = (r>>6) & 0x03FF;
	VICVectAddr = 0;
}

/* Funkcja glowna main */
int main(void)
{	
	//Inicjalizacja UART
	Uart0Init(UART_BAUD(9600));
	//Napis zachety
	printf("Mikrokontrolery z rdzeniem ARM cz IX\r\n");
	printf("Przetwornik A/C z systemem przerwan\r\n");
	//Inicjalizacja kontrolera VIC
	VICVectAddr0 = (unsigned int)AdcIrq;
	VICVectCntl0 = ADC0_VIC_BIT | VIC_IRQSLOT_EN;
	VICIntEnable = ADC0_VIC;
	initADC();
	const float Vref = 3.3;
	while(1)
	{
		printf("Vpot = %.2f V\r\n",(AdcPot/1023.0)*Vref);
		float Ukty = (AdcKty/1023.0)*Vref;
		printf("Rkty = %.2f kOhm\r\n",Ukty/(Vref-Ukty));
		for(volatile int i=0;i<1000000;i++);
	}
	return 0;
}


