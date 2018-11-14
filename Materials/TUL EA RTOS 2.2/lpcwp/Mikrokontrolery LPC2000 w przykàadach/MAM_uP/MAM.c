////////////////////////////////////////////////////////
// Function:	MAM testing example
// Target:		LPC2138
// Board:		ZL6ARM
// Quartz:		12MHz
// Compiler:	ARM-GCC-4.0.1 (www.gnuarm.com)
// Author:		Arkadiusz Antoniak @ 2006, Poland
// Copyright:	(c) Arkadiusz Antoniak, 2006
////////////////////////////////////////////////////////
#include <LPC213x.H>


//*******************************
// MAIN
//*******************************
int main(void) 
{
    //Peripherials' frequency = fclk = 60MHz
    VPBDIV&=0xFFFFFFFC;
    VPBDIV|=0x01;
    
    //P1.16..23 defined as outputs
    IODIR1 = 0x00FF0000;
    
    while(1)
    {
        IOPIN1=0x00FF0000;
        IOPIN1=0x00000000;
    }
}
