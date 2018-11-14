////////////////////////////////////////////////////////
// Function:	Real Time Clock example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC
// Author:		Arkadiusz Antoniak @ 2005, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2005
////////////////////////////////////////////////////////
#include <LPC213x.H>
#include "lcd.h"
#include "switches_pins.h"

//Switch delay
#define SW_DELAY	delay(0x8000)

//Mask values used while reading from Consolidated Time Registers
#define MASKSEC		0x3F
#define MASKMIN		0x3F00
#define MASKHR		0x1F0000
#define MASKYEAR	0xFFF0000
#define MASKMONTH	0xF00
#define MASKDOM		0x1F
#define MASKDOW		0x7000000

//Interrupt handler
void read_rtc(void) __attribute__ ((interrupt));

//Names of days of week
const char* DaysOfWeek[7]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

//*******************************
// Delay function
//*******************************
void delay (unsigned int del_time) 
{ 
	unsigned int i;
	for(i = 0; i < del_time; i++);	   
}

//*******************************
// Setting number by switches
//*******************************
unsigned int GetSwNumber(int Init, int Max, int Min,int fDayOfWeek)
{
	int val=Init;

	while(SW3==0)
		SW_DELAY;

	LCDGotoXY(0,1);
	if(fDayOfWeek==0)
		LCD32bitNumberDec(val);
	else
		LCDText((char*)DaysOfWeek[val]);
	LCDText("                ");

	while(1)
	{
		LCDGotoXY(0,1);
		if(SW1==0)
		{
			if(val++>=Max)
				val=Max;

			if(fDayOfWeek==0)
				LCD32bitNumberDec(val);
			else
				LCDText((char*)DaysOfWeek[val]);
			LCDText("                ");
			delay(0x200000);
		}
		else if(SW2==0)
		{
			if(val--<=Min)
				val=Min;

			if(fDayOfWeek==0)
				LCD32bitNumberDec(val);
			else
				LCDText((char*)DaysOfWeek[val]);
			LCDText("                ");
			delay(0x200000);
		}
		else if (SW3==0)
			return (unsigned int)val;

		SW_DELAY;
	}
}

//*******************************
// Set time & date using switches
//*******************************
void SetTimeAndDate(void)
{
	LCDClear();
	LCDText("Y: S1+ S2- S3 OK");
	YEAR=GetSwNumber(2005,3000,1000,0);

	LCDClear();
	LCDText("M: S1+ S2- S3 OK");
	MONTH=GetSwNumber(1,12,1,0);

	LCDClear();
	LCDText("D: S1+ S2- S3 OK");
	DOM=GetSwNumber(1,31,1,0);

	LCDClear();
	LCDText("d: S1+ S2- S3 OK");
	DOW=GetSwNumber(0,6,0,1);

	LCDClear();
	LCDText("h: S1+ S2- S3 OK");
	HOUR=GetSwNumber(0,23,0,0);

	LCDClear();
	LCDText("m: S1+ S2- S3 OK");
	MIN=GetSwNumber(0,59,0,0);

	SEC=0;

	LCDClear();
	LCDText("****** OK ******");
}

//*******************************
// RTC Initialization
//*******************************
void InitRTC(void)
{
	//Reset the clock
	CCR=0x2;

	//Clear the Interrupt Location Register
	ILR=0x3;

	//RTC Interrupt on increment of Second value
	CIIR=0x01;

	//Set prescaler values (pclk=15MHz)
	PREINT=0x1C8;
	PREFRAC=0x61C0;

	//Set time & date
	SetTimeAndDate();

	//Initialize Vectored Interrupt Controller (VIC)
	VICIntSelect=0x000;
	VICVectCntl0=0x2d;
	VICVectAddr0=(unsigned long )read_rtc;
	VICIntEnable =0x2000;
}

//*******************************
// RTC Interrupt Service Routine
//*******************************
void read_rtc(void)
{
	int hour,min,sec;
	int year,month,dom,dow;

	//Reading Consolidated Time & Date registers
	hour =(CTIME0 & MASKHR)>>16;
	min = (CTIME0 & MASKMIN)>>8;
	sec = CTIME0 & MASKSEC;
	year =(CTIME1 & MASKYEAR)>>16;
	month = (CTIME1 & MASKMONTH)>>8;
	dom = CTIME1 & MASKDOM;
	dow = (CTIME0 & MASKDOW)>>24;

	LCDClear();

	//Display time
	LCD32bitNumberDec(hour);
	LCDText(":");
	LCD32bitNumberDec(min);
	LCDText(":");
	LCD32bitNumberDec(sec);

	LCDGotoXY(0,1);

	//Display day of week
	LCDText((char*)DaysOfWeek[dow]);
	LCDText(", ");

	//Display Date
	LCD32bitNumberDec(dom);
	LCDText(".");
	LCD32bitNumberDec(month);
	LCDText(".");
	LCD32bitNumberDec(year);

	//Clearing Interrupt & Updating VIC
	ILR=0x1;
	VICVectAddr=0xff;
}

//*******************************
// MAIN
//*******************************
int main(void)
{
	//pclk=fclk/4
	VPBDIV=0x0;

	//Port direction configuration
	IODIR0 = 0xFF000000;
	IODIR1 = 0x00FF0000;

	LCDInit();
	LCDClear();
	LCDText("RTC example");
	delay(0x800000);

	//Initialize RTC
	InitRTC();

	//Start RTC
	CCR=0x01;

	while (1);
}
/////////////////////////////////////////////////////////////

