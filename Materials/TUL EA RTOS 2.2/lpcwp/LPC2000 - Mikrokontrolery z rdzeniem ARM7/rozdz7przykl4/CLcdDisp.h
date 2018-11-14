#ifndef CLCDDISP_H
#define CLCDDISP_H

#include "lpc214x.h"

//Rejestr kierunku dla E i RS
#define LCDCDIR IO0DIR
//Rejestr ustawiania dla E i RS
#define LCDCSET IO0SET
//Rejestr kasowania dla E i RS
#define LCDCCLR IO0CLR


//Rejestr kierunku dla danych
#define LCDDDIR IO1DIR
//Rejestr ustawiania dla danych
#define LCDDSET IO1SET
//Rejestr kasowania dla danych
#define LCDDCLR IO1CLR


//Klasa wyznaczajaca pozycje
class pos
{
public:
	pos(unsigned char x,unsigned char y):mx(x),my(y) {}
	unsigned char mx,my;
};

//Klasa obslugi wyswietlacza LCD

class CLcdDisp
{
public:
	CLcdDisp();
	~CLcdDisp();
	void Write(const char *str);
	void Write(char zn);
	void Write(unsigned int licz);
	//Wyczysc wyswietlacz
	void Clear(void);
	//Zalacz wylacz kursor
	void SetCursor(unsigned char cmd);
	void GotoXY(unsigned char x,unsigned char y);
	template<class T> CLcdDisp& operator <<(const T &obj)
	{
	  Write(obj);
      return *this;
	}
	CLcdDisp& operator <<(const pos &obj)
	{
		GotoXY(obj.mx,obj.my);
		return *this;
	}
	//Komenda ON
	enum {ON_DISPLAY=0x04,ON_CURSOR=0x02,ON_BLINK=0x01};
private:
	//Funkcja opozniajaca 
	void Delay(unsigned int del);
	//Wysyla do portu
	void PortSend(unsigned char data,bool cmd=false);
	//Pin E P0.30
	static const unsigned int E = 0x40000000;
	//Pin RW P0.31
	static const unsigned int RS = 0x80000000;
	//Maska danych
	static const unsigned int DMASK = 0x00FF0000;
	//Domyslne sprzetowe
	static const unsigned int DELAY_HW = 15;
	//Opoznienie komend
	static const unsigned int DELAY_CMD = 3000;
	//Opoznienie dla CLS
	static const unsigned int DELAY_CLS = 30000;
	//Komendy wyswietlacza 
	enum {CLS_CMD=0x01,HOME_CMD=0x02,MODE_CMD=0x04,ON_CMD=0x08,
	      SHIFT_CMD=0x10,FUNC_CMD=0x20,CGA_CMD=0x40,DDA_CMD=0x80};
	//Komenda MODE
	enum {MODE_R=0x02,MODE_L=0,MODE_MOVE=0x01};
	//Komenda SHIFT
	enum {SHIFT_DISP=0x08,SHIFT_R=0x04,SHIFT_L=0};
	//Komenda FUNC
	enum {FUNC_8b=0x10,FUNC_4b=0,FUNC_2L=0x08,
			FUNC1L=0,FUNC_5x10=0x4,FUNC_5x7=0};
};



#endif // CLCDDISP_H
