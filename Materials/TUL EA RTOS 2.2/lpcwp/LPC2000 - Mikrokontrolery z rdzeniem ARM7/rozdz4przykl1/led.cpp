#include "lpc214x.h"

//Definicja LEDOW
#define LEDS (0xFF<<16)
#define LEDDIR IO1DIR
#define LEDSET IO1SET
#define LEDCLR IO1CLR

class CLedShift
{
public:
	//Konstruktor klasy
	CLedShift()
	{
		//Piny jako wyjscia
		LEDDIR |= LEDS;
	}
	//Metoda Run
	void Run()
	{
		//Pierwsza dioda
		mShift = 1;
		for(int i=0;i<8;i++)
		{
			//Zapal wybrana diode
			LEDSET = mShift << 16;
			//Zgas pozostale diody
			LEDCLR = ~mShift << 16;
			//Petla opozniajaca
			for(volatile int d=0;d<500000;d++);
			//Przesun bit o 1
			mShift <<= 1;
		}
	}
private:
	//Zmienna przechowujaca biezacy LED
	unsigned char mShift;
};



//Funkcja glowna main
int main(void)
{
	//Klasa diod swiecacych
	CLedShift led1;
	//Petla glowna
	while(1)
	{
		//Wywolanie cyklicznie metody RUN
		led1.Run();
	}
}
