#include "lpc214x.h"
#include "CLcdDisp.h"
/**************************************************
 * Mikrokontrolery z rdzeniem ARM - kurs czesc VII*
 * Autor: Lucjan Bryndza						  *				
 * SINGLE EDGE PWM							      *
**************************************************/

//Klawisz zwiekszajacy
#define KEYADD (1<<4)
//Klawisz zmniejszajacy
#define KEYSUB (1<<5)
//Port klawiatury
#define KEYPIN IO0PIN
//Port IO jako PWM5
#define PWM5_P021_SEL (1<<10)
//Zalacz wyjscie 5 PWM
#define PWMPCR_PWMENA5 0x2000U
//Counter reset
#define PWMTCR_Counter_Reset 0x2U
//PWM enable
#define PWMTCR_PWM_Enable 0x8U
//Counter enable
#define PWMTCR_Counter_Enable 0x1U
//Reset on MR0
#define PWMMCR_Reset_on_PWMMR0 0x1U

//Obiekt wyswietlacza LCD
CLcdDisp cout;


/* Funkcja glowna main */
int main(void)
{
	//Preskaler CCLK/10 = 6MHz
	PWMPR = 9;
	//Zgodne zbocze kasuje timer
	PWMMCR |= PWMMCR_Reset_on_PWMMR0;
	//Ustaw PWM5 jako wyjscie
	PWMPCR |= PWMPCR_PWMENA5; 
	//Ustaw rejestr zakres 100 
	PWMMR0 = 100;
	//PWM= 0%
	PWMMR5 = 0;
	//Wykonaj zapis MR0 i MR5
	PWMLER = (1<<5) | (1<<0);
	//Wlacz alternatywny pin PWM5
	PINSEL1 |= PWM5_P021_SEL;
	//Zeruj licznik i preskaler
	PWMTCR = PWMTCR_Counter_Reset;
	//Wlacz licznik PWM
	PWMTCR = PWMTCR_PWM_Enable | PWMTCR_Counter_Enable;
	int Light = 0;
	cout << "Pwm=";
	while(1)
	{
		//Wcisnieto klawisz +
		if(!(KEYPIN&KEYADD))
		{
			if(++Light>100) Light = 100;
			for(volatile int i=0;i<400000;i++);
		}
		//Wcisnieto klawisz -
		if(!(KEYPIN&KEYSUB))
		{
			if(--Light<0) Light = 0;
			for(volatile int i=0;i<400000;i++);
		}
		//Aktualizuj wspolczynnik PWM
		PWMMR5 = Light;
		PWMLER = 1<<5;
		cout << pos(7,1) << (unsigned int)Light << "%   ";
	}
	return 0;
}
