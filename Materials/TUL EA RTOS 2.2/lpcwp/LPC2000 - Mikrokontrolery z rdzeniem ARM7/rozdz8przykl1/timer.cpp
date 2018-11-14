#include "lpc214x.h"
#include "armint.h"
#include "CLcdDisp.h"
/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *				
 * Timer0 jako czasomierz systemowy 	          *
**************************************************/

#define TIMER0_VIC (1<<4)
#define TIMER0_VIC_BIT 4
#define VIC_IRQSLOT_EN (1<<5)


#define T0MCR_Interrupt_on_MR0 0x1U
#define T0MCR_Reset_on_MR0 0x2U
#define T0TCR_Counter_Reset 0x2U
#define T0TCR_Counter_Enable 0x1U
#define T0IR_MR0 0x1U

//Przerwanie wektoryzowane IRQ
void IrqTimerHandler(void) __attribute__ ((interrupt("IRQ")));

static volatile unsigned int TimerCnt;

void IrqTimerHandler(void)
{
	//Zmien stan LEDOW na przeciwny
	TimerCnt++;
	//Kasuj zrodlo przerwania
	T0IR = T0IR_MR0;
	//Informacja dla VIC - koniec procedury przerwania
	VICVectAddr = 0;
}


CLcdDisp cout;
/* Funkcja glowna main */
int main(void)
{
	
	//Licznik zlicza impulsy z czestotliwoscia 15Hz
	T0PR = 3999999;
	//Gdy warunek spelniony zeruj Timer i zglaszaj przerwanie
	T0MCR |= T0MCR_Interrupt_on_MR0 | T0MCR_Reset_on_MR0;
	//Przeladowanie licznika co 1s
	T0MR0 = 15;
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
	cout << "ARM - Timer";
	cout << pos(1,2) << "Timer=";
	while(1)
	{
		cout << pos(7,2) << TimerCnt;
	}
	return 0;
}
