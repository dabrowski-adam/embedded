#include "UserSettings.h"
#include "Timer.h"
#include "delay.h"
#include "lcd.h"

//Change Signal Type
void ChangeSignalType(void)
{
	LCDClear();
	LCDText("S1:Sine S2:Rect");
	LCDGotoXY(0,1);
	LCDText("S3:Tria");

	while(!SW1|!SW2|!SW3|!SW4|!SW5)
		SW_DELAY;

	while(1)
	{
		if(SW1==0)
			SetSignalType(stSinus);
		else if(SW2==0)
			SetSignalType(stRectangle);
		else if(SW3==0)
			SetSignalType(stTriangle);

		if(!SW1|!SW2|!SW3)
		{
			while(!SW1|!SW2|!SW3|!SW4|!SW5)
				SW_DELAY;
			
			LCDClear();
			LCDText("OK");
			delay(0x80000);
			return;
		}

		SW_DELAY;
	}
}

//Display current setting of many value types
void DisplayValue(int val, int ValueType)
{
	int tmp;

	if(ValueType==1)
	{
		tmp=(val*100)/128;
		LCDText("A = ");
		LCD32bitNumberDec(tmp/100);
		LCDText(".");
		LCD32bitNumberDec((tmp/10)%10);
		LCD32bitNumberDec(tmp%10);
		LCDText("V");
		LCDText("                ");
	}
	else if(ValueType==2)
	{
		tmp=(val*330)/1023;
		LCDText("C = ");
		LCD32bitNumberDec(tmp/100);
		LCDText(".");
		LCD32bitNumberDec((tmp/10)%10);
		LCD32bitNumberDec(tmp%10);
		LCDText("V");
		LCDText("                ");
	}
	else if(ValueType==3)
	{
		LCDText("F = ");
		LCD32bitNumberDec(val/10);
		LCDText(".");
		LCD32bitNumberDec(val%10);
		LCDText("kHz");
		LCDText("                ");
	}
	else if(ValueType==4)
	{
		tmp=(val*100)/128;
		LCDText("D = ");
		LCD32bitNumberDec(tmp/10);
		LCD32bitNumberDec(tmp%10);
		LCDText("%");
		LCDText("                ");
	}
}

//Change current setting
// ValueType:
// 1 - Amplitude
// 2 - ConstValue
// 3 - Frequency
// 4 - Duty
int ChangeSwValue(int Init, int Max, int Min, int ValueType, int Del)
{
	int val=Init;

	//Disable timer
	T0TCR=0;

	LCDClear();
	LCDText("S1+ S2- S3ok S4c");
	LCDGotoXY(0,1);
	DisplayValue(val,ValueType);

	while(!SW1|!SW2|!SW3|!SW4|!SW5)
		SW_DELAY;

	while(1)
	{
		LCDGotoXY(0,1);
		DisplayValue(val,ValueType);

		if(SW1==0)
		{
			if(val++>=Max)
				val=Max;

			delay(Del);
		}
		else if(SW2==0)
		{
			if(val--<=Min)
				val=Min;

			delay(Del);
		}
		else if(SW3==0)
		{
			while(!SW3)
				SW_DELAY;
			
			LCDClear();
			LCDText("OK");
			delay(0x80000);

			//Enable timer
			T0TCR=1;
			return val;
		}
		else if(SW4==0)
		{
			while(!SW4)
				SW_DELAY;

			//Enable timer
			T0TCR=1;
			return -1;
		}

		SW_DELAY;		
	}
}

void ChangeAmplitude(void)
{
	int r=ChangeSwValue(GetAmplitude(),210,1,1,0x40000);

	if(r>0)
		SetAmplitude(r);
}

void ChangeConstValue(void)
{
	int r=ChangeSwValue(GetConstValue(),1023,1,2,0x30000);

	if(r>0)
		SetConstValue(r);
}

void ChangeFrequency(void)
{
	int r=ChangeSwValue(GetFrequency(),100,1,3,0x100000);

	if(r>0)
		SetFrequency(r);
}

void ChangeDuty(void)
{
	int r=ChangeSwValue(GetDuty(),128,1,4,0x80000);

	if(r>0)
		SetDuty(r);
}
