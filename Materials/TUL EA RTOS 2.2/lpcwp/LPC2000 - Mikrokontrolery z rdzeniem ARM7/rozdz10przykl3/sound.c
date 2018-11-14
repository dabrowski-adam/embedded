/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *	
 * Przyklad obslugi przetwornika DAC              *
***************************************************/

#include "lpc214x.h"
#include "armint.h"
#include "wav.h"

#define P025_DAC_SEL (2<<18)
#define TIMER0_VIC (1<<4)
#define TIMER0_VIC_BIT 4
#define VIC_IRQSLOT_EN (1<<5)

#define T0IR_MR0 0x1
#define T0MCR_Interrupt_on_MR0 0x1
#define T0MCR_Reset_on_MR0 0x2
#define T0TCR_Counter_Reset 0x2
#define T0TCR_Counter_Enable 0x1

static int AdcPos = 0;

//Przerwanie od Timera
void IrqTimerHandler(void) __attribute__ ((interrupt("IRQ")));

void IrqTimerHandler(void)
{
	//Zapis danych do DAC
	DACR = ((unsigned int)wav_file[AdcPos++])<<8;
	if(AdcPos>= wav_length) T0TCR = 0;
	//Kasuj zrodlo przerwania
	T0IR = T0IR_MR0;
	//Informacja dla VIC - koniec procedury przerwania
	VICVectAddr = 0;
}

/* Funkcja glowna main */
int main(void)
{	
	PINSEL1 |= P025_DAC_SEL;	
	//Licznik zlicza impulsy z czestotliwoscia 15Hz
	T0PR = 0;
	//Gdy warunek spelniony zeruj Timer i zglaszaj przerwanie
	T0MCR |= T0MCR_Interrupt_on_MR0 | T0MCR_Reset_on_MR0;
	//Przeladowanie licznika co 1s
	T0MR0 = 2720;
	//Zeruj licznik i preskaler
	T0TCR = T0TCR_Counter_Reset;
	//Zalacz licznik T0
	T0TCR = T0TCR_Counter_Enable;
	//Wektor 0
	VICVectAddr0 = (unsigned int)IrqTimerHandler;
	VICVectCntl0 = TIMER0_VIC_BIT | VIC_IRQSLOT_EN;
	//Zalaczenie przerwania
	VICIntEnable = TIMER0_VIC;
	//Zalacz IRQ
	enable_irq();
	return 0;
}


