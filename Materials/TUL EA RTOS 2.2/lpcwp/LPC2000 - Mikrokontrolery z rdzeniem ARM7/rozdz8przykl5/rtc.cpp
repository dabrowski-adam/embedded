#include "lpc214x.h"
#include "CLcdDisp.h"
#include "armint.h"
/**************************************************
 * Mikrokontrolery z rdzeniem ARM - pierwsze kroki*
 * Autor: Lucjan Bryndza			  *				
 * RTC USAGE				          *
**************************************************/

//Ustawienia kontrolera VIC
#define RTC_VIC (1<<13)
#define RTC_VIC_BIT 13
#define VIC_IRQSLOT_EN (1<<5)

//Sterowanie linia audio
#define BUZZ_PIN (1<<25)
#define BUZ_OUT() IO0DIR |= BUZZ_PIN
#define BUZ_CHG() IO0PIN ^= BUZZ_PIN

//Zewnetrzny oscylator 32768
#define CCR_EXTOSC (1<<4)
//Zmiana sekundy generuje IRQ
#define CIIR_IMSEC 1

#define ILR_RTCALF 0x2
#define ILR_RTCCIF 0x1
#define CCR_CTCRST 0x2
#define CCR_CLKEN  0x1
#define AMR_AMRSEC 0x1

//Flaga aktywnosci alarmu
static volatile unsigned char AlarmOn = 0;

//Przerwanie od RTC
void RtcTimerHandler(void) __attribute__ ((interrupt("IRQ")));

void RtcTimerHandler(void)
{
	//Jezeli przerwanie od alarmu
	if(ILR & ILR_RTCALF)
	{
		//Kasuj flage przerwania od alarmu
		ILR = ILR_RTCALF;
		//Ustaw flage aktywnosci alarmu
		AlarmOn = 1;
		//Aktywuj przerwanie od zmiany sekund
		CIIR = CIIR_IMSEC;
	}
	//Jezeli przerwanie od zmiany sekund
	if(ILR & ILR_RTCCIF)
	{
		//Kasuj flage przerwania od zmiany wartosci
		ILR = ILR_RTCCIF;
		//Kasuj flage alarmu
		AlarmOn = 0;
		//Deaktywuj zglaszanie przerwania od zmiany
		CIIR = 0;
	}
	//Informacja dla VIC - koniec procedury przerwania
	VICVectAddr = 0;
}



//Obiekt wyswietlacza LCD
CLcdDisp cout;


/* Funkcja glowna main */
int main(void)
{
	//Preskaler gdy taktujemy zegar z PCLK
	//PREINT = 1830;
	//PREFRAC = 1792;
	//Resetuj Clock 
	CCR = CCR_CTCRST;
	//Zalacz zegar z zewnetrznego rezonatora kwarcowego
	CCR = CCR_CLKEN|CCR_EXTOSC;
	//Wpisujemy date i godzine
	HOUR = 23;	//Godzina
	MIN = 59;	//Minuta
	SEC = 50;	//Sekunda
	YEAR = 2006;	//Rok
	MONTH = 3;	//Miesiac
	DOM = 27;	//Dzien
	
	//Alarm gdy sekundy rowne 00
	ALSEC = 00;
	//Maska na aby minuty generowaly alarm
	AMR = ~AMR_AMRSEC;
	
	//Inicjalizacja przerwan
	//Wektor 0
	VICVectAddr0 = (unsigned int)RtcTimerHandler;
	VICVectCntl0 = RTC_VIC_BIT | VIC_IRQSLOT_EN;
	//Kasuj ewentualnie wczesniej ustawione flagi
	ILR = ILR_RTCALF | ILR_RTCCIF;	
	//Zalaczenie przerwania
	VICIntEnable = RTC_VIC;
	//Zalacz IRQ
	enable_irq();
	//Wlacz buzzer
	BUZ_OUT();
	//Ustaw tal aby wyswietlane byly 2 zera znaczace
	cout.SetPrecision(2);
	//Poprzedni stan sekund
	unsigned char psec = 255;
	//Petla glowna programu
	while(1)
	{
		//Jezeli flaga alarmu to zmianiaj stan buzzera
		if(AlarmOn) 
		{
			BUZ_CHG();
			//Petla opozniajaca
			for(volatile int i=0;i<600;i++);
		}
		//Jezeli nie bylo zmiany sekundy to nie wyswietlaj
		if(psec == SEC) continue;
		//Zapisz stan sekund
		psec = SEC;
		//Wyswietl w pierwszej linii godz,min,sek
		cout << pos(1,1) << (unsigned int)HOUR << ":";
		cout << (unsigned int)MIN << ":";
		cout << (unsigned int)SEC;
		//Wyswietl w drugiej linii rok,mies,dzien
		cout << pos(1,2) << (unsigned int)YEAR << "/";
		cout << (unsigned int)MONTH << "/";
		cout << (unsigned int)DOM;
	}
	return 0;
}
