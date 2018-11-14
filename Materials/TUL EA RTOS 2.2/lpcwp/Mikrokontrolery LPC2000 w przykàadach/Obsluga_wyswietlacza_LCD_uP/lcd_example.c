////////////////////////////////////////////////////////
// Function:	LCD example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC-4.0.1 (www.gnuarm.com)
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"

#define del_l 0x400000
#define del_s 0x10000
#define SHIFT_TIME 0x200000
				  
//*******************************
// Delay
//*******************************
void delay (unsigned int del_time) 
{ 
    unsigned int i;
    for(i = 0; i < del_time; i++);	   
}								    							    

//*******************************
// MAIN
//*******************************
int main (void)
{
    unsigned int i;
    
    IODIR0 = 0xFF000000;
    IODIR1 = 0x00FF0000;
    
    LCDInit();
    
    LCDClear();
    
    //Display shift demo
    LCDText("   LCD example");
    LCDTextXY(0,1,"       ARM");
    
    for(i=0;i<3;i++)
    {
        delay(SHIFT_TIME);
        LCDDisplayShiftLeft();
    }
    
    for(i=0;i<5;i++)
    {
        delay(SHIFT_TIME);
        LCDDisplayShiftRight();
    }
    
    for(i=0;i<2;i++)
    {
        delay(SHIFT_TIME);
        LCDDisplayShiftLeft();
    }
    
    delay(0x1000000);
    
    //Display MAX and MIN signed integer value
    LCDClear();
    
    //-(2^31-1)
    LCDText("-: ");
    LCD32bitNumberDec(-((1<<30)+((1<<30)-1)));
    
    LCDGotoXY(0,1);
    
    //2^31-1
    LCDText("+: ");
    LCD32bitNumberDec(((1<<30)+((1<<30)-1)));
    
    while(1);
}
