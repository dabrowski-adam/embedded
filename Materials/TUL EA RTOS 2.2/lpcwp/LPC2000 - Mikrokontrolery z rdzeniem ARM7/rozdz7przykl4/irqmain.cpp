#include "lpc214x.h"
#include "armint.h"
#include "CLcdDisp.h"
/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki *
 * Autor: Lucjan Bryndza			   *				
 * Przyklad poslugiwania sie przerwaniami IRQ      *
 **************************************************/

#define EINT1_SEL (2<<28)
#define P014_SEL_MASK (3<<28)
#define EINT1_VIC (1<<15)
#define EINT1_VIC_BIT 15
#define VIC_IRQSLOT_EN (1<<5)

#define EXTINT_EINT1	0x02

//Przerwanie wektoryzowane IRQ
void IrqInt1Handler(void) __attribute__ ((interrupt("IRQ")));

static volatile unsigned int EintCnt;

void IrqInt1Handler(void)
{
	if(VICIRQStatus & EINT1_VIC)
	{
		//Zmien stan LEDOW na przeciwny
		EintCnt++;
		//Kasuj zrodlo przerwania
		EXTINT = EXTINT_EINT1;
	}
	//Informacja dla VIC - koniec procedury przerwania
	VICVectAddr = 0;
}


CLcdDisp cout;
/* Funkcja glowna main */
int main(void)
{
	//Uruchomienie na P0.14 funkcji alternatywnej INT1
	PINSEL0 &= ~P014_SEL_MASK;
	PINSEL0 |= EINT1_SEL;
	//Przerwanie zboczem
	EXTMODE |= EXTINT_EINT1;
	//Zbocze opadajace
	EXTPOLAR &= ~EXTINT_EINT1;
	//Kasuj wystapienie przerwania (1 kasuje)
	EXTINT = EXTINT_EINT1;
	//Wektor default
	VICDefVectAddr = (unsigned int)IrqInt1Handler;
	//Zalaczenie przerwania
	VICIntEnable = EINT1_VIC;
	//Zalacz IRQ
	enable_irq();
	
	cout << "ARM - NoVect IRQ";
	cout << pos(1,2) << "IntCnt=";
	while(1)
	{
		cout << pos(8,2) << EintCnt;
	}
	return 0;
}
