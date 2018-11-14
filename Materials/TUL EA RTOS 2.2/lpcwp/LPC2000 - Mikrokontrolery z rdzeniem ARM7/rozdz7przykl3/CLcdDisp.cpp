#include "CLcdDisp.h"


//Konstruktor klasy obslugi wyswietlacza LCD
CLcdDisp::CLcdDisp()
{
	//Linie E i RS jako wyjsciowe
	Delay(1000000);
	LCDCDIR |= E|RS;	
	LCDCCLR = E|RS;
	//Linia danych jako wyjsciowa
	LCDDDIR |= DMASK;
	Delay(100000);
	//Wysylaj kolejno komendy inicjalizacyjne
	PortSend(FUNC_CMD|FUNC_8b,true);
	Delay(DELAY_CLS);
	PortSend(FUNC_CMD|FUNC_8b,true);
	Delay(DELAY_CMD);
	PortSend(FUNC_CMD|FUNC_8b,true);
	Delay(DELAY_CMD);
	PortSend(FUNC_CMD|FUNC_8b|FUNC_2L|FUNC_5x7,true);
	Delay(DELAY_CMD);
	PortSend(ON_CMD|ON_DISPLAY,true);
	Delay(DELAY_CMD);
	PortSend(MODE_CMD|MODE_R,true);
	Delay(DELAY_CMD);
	PortSend(CLS_CMD,true);
	Delay(DELAY_CLS);
	PortSend(HOME_CMD,true);
	Delay(DELAY_CLS);
}


//Destruktor klasy obslugi wyswietlacza LCD
CLcdDisp::~CLcdDisp()
{
}


//Wysyla bajt do wyswietlacza LCD
void CLcdDisp::PortSend(unsigned char data,bool cmd)
{
	//E=0
	LCDCCLR = E;
	//Data = 0;
	LCDDCLR = DMASK;
	//Wyslij dane
	LCDDSET = ((unsigned int)data) << 16;
	//Skasuj lub ustaw RS
	if(cmd) LCDCCLR = RS;
	else LCDCSET = RS;
	//Ustaw Enable
	LCDCSET = E;
	Delay(DELAY_HW);
	//Skasuje enable
	LCDCCLR = E;
}

void CLcdDisp::Write(const char *str)
{
	while(*str)
	{
	  PortSend(*str++);
	  Delay(DELAY_CMD);
	}
}

void CLcdDisp::Delay(unsigned int del)
{
	asm volatile
	(
	"dloop%=:"
	"subs %[del],%[del],#1\t\n"		//1c
	"bne dloop%=\t\n"				//3c
	: :[del]"r"(del)
	);
}

void CLcdDisp::GotoXY(unsigned char x,unsigned char y)
{
		x-=1;
		if(y==2) x |= 0x40;
		PortSend(DDA_CMD|x,true);
		Delay(DELAY_CMD);
}

//Wyczysc wyswietlacz
void CLcdDisp::Clear(void)
{
	PortSend(CLS_CMD,true);
	Delay(DELAY_CLS);
}

void CLcdDisp::SetCursor(unsigned char cmd)
{
	PortSend(cmd|ON_CMD|ON_DISPLAY,true);
	Delay(DELAY_CMD);
}

void CLcdDisp::Write(char zn)
{
	 PortSend(zn);
	 Delay(DELAY_CMD);
}

void CLcdDisp::Write(unsigned int licz)
{
	char buf[10];
    unsigned int calk;
    int i=0;
    calk = licz;
    while(calk)
    {
      calk /= 10;
      i++;
    }
    if(i)
    {
        calk = licz;
        buf[i] = 0;
        for(--i;i>=0;i--)
        {
           buf[i] = calk % 10 + '0';
           calk /= 10;
        }
    }
    else
    {
        buf[0] = '0';
        buf[1] = 0;
    } 
	char *str = buf;
	while(*str)
	{
	  PortSend(*str++);
	  Delay(DELAY_CMD);
	}
}
