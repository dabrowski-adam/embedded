////////////////////////////////////////////////////////
// Function:    No init
// Target:      LPC2138
// Board:       ZL6ARM
// Quartz:      12MHz
// Compiler:    ARM-GCC
// Author:      Arkadiusz Antoniak @ 2006, Poland
// Copyright:   (c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>	    
#include "lcd.h"
#include "switches_pins.h"

//Globals
volatile unsigned int global_var __attribute__ ((section(".noinit")));

//*******************************
// Delay function
//*******************************
void delay(unsigned int del_time) 
{ 
    unsigned int i;
    for(i=0;i<del_time;i++);	   
}

//*******************************
// MAIN
//*******************************
int main (void)
{
    //Peripherials' frequency = fclk/4 = 15MHz
    VPBDIV&=0xFFFFFFFC;
    VPBDIV|=0x00;
    
    //Port direction configuration
    IODIR0 = 0xFF000000;
    IODIR1 = 0x00FF0000;
    
    LCDInit();
    
    while(1)
    {
        //Increment and display global variable
        LCDClear();
        LCDText("var=");
        LCD32bitNumberDec(global_var++);
        delay(0x800000);
        
        //Zeroing global variable
        if(SW1==0)
            global_var=0;
    }
}

