/******************************************************************************
 *
 * Pawe³ Tarasiuk, 15121
 *
 * Based on "testLedMatrix.c" from the test program copyrighted by
 * Embedded Atrists AB.
 *
 *****************************************************************************/


#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <lpc2xxx.h>
#include <consol.h>
#include <config.h>

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define  SPI_CS   0x00008000  //<= new board, old board = 0x00800000


tU8 pattern[8] = {0,0,0,0,0,0,0,0};

void ledMatrix(void);

tU32 getRandBits(int count); /* from main.c */

extern tU8 request; /* from main.c */
extern tU32 requestTime; /* from main.c */
extern tU32 ticks; /* from main.c */
extern tU8 inGame; /* from main.c */

char rightArrow[] = \
  "   **   " \
  "    **  " \
  "     ** " \
  "********" \
  "********" \
  "     ** " \
  "    **  " \
  "   **   ";
char upArrow[65];
char leftArrow[65];
char downArrow[65];

char center[] = \
  "  ****  " \
  " *    * " \
  "* *  * *" \
  "*  **  *" \
  "*  **  *" \
  "* *  * *" \
  " *    * " \
  "  ****  ";

/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/
static void
startTimer1(tU16 delayInMs)
{
  //initialize VIC for Timer1 interrupts
  VICIntSelect &= ~0x20;           //Timer1 interrupt is assigned to IRQ (not FIQ)
  VICVectAddr5  = (tU32)ledMatrix; //register ISR address
  VICVectCntl5  = 0x25;            //enable vector interrupt for timer1
  VICIntEnable  = 0x20;            //enable timer1 interrupt
  
  //initialize and start Timer #0
  T1TCR = 0x00000002;                           //disable and reset Timer1
  T1PC  = 0x00000000;                           //no prescale of clock
  T1MR0 = delayInMs *                           //calculate no of timer ticks
         ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (1000 * VPBDIV_FACTOR));
  T1IR  = 0x000000ff;                           //reset all flags before enable IRQs
  T1MCR = 0x00000003;                           //reset counter and generate IRQ on MR0 match
  T1TCR = 0x00000001;                           //start Timer1
}

/*****************************************************************************
 *
 * Description:
 *    Fills the LED matrix data with random bits.
 *
 ****************************************************************************/
void
fillRandom(void)
{
  tU8 i;
  for(i = 0; i < 8; i++)
  {
    pattern[i] = getRandBits(8);
  }
  
  i = getRandBits(2);
  if(i == 0) pattern[0] |= 0x01;
  else if(i == 1) pattern[0] |= 0x80;
  else if(i == 2) pattern[7] |= 0x01;
  else if(i == 3) pattern[7] |= 0x80;
}

/*****************************************************************************
 *
 * Description:
 *    Prepares arrow pictures.
 *
 ****************************************************************************/
void
prepareArrows(void)
{
  tU8 i, j;
  for(i = 0; i < 8; i++)
  {
    for(j = 0; j < 8; j++) {
      leftArrow[(i << 3) | (j ^ 7)] = rightArrow[(i << 3) | j];
      upArrow[((j ^ 7) << 3) | i] = rightArrow[(i << 3) | j];
      downArrow[(j << 3) | i] = rightArrow[(i << 3) | j];
    }
  }
}

/*****************************************************************************
 *
 * Description:
 *    Draws the picture to the LED matrix data.
 *
 ****************************************************************************/
void
fillPicture(char const *data)
{
  tU8 i, j;
  for(i = 0; i < 8; i++)
  {
    pattern[i] = 0;
    for(j = 0; j < 8; j++) {
      pattern[i] |= ((1 << j) * (data[(j << 3) | i] == '*'));
    }
  }
}

/*****************************************************************************
 *
 * Description:
 *    A process entry function 
 *
 ****************************************************************************/
void
testLedMatrix(void)
{
  static tU8 arrowsPrepared;
  static tU8 tmp;
  
  if(arrowsPrepared == 0) {
    prepareArrows();
    arrowsPrepared = 1;
  }
  
  PINSEL0 |= 0x00001500 ;  //Initiering av SPI
  SPI_SPCCR = 0x08;    
  SPI_SPCR  = 0x60;
  IODIR0 |= SPI_CS;
  
  startTimer1(2);
  
  while(inGame)
  {
    if(request == 0 || request == 8)
    {
      tmp = getRandBits(2);
      if(request == 0 && tmp == 3)
      {
        request = (getRandBits(8) % 5) + 1;
        
        if(request == 1) fillPicture(upArrow);
        else if(request == 2) fillPicture(rightArrow);
        else if(request == 3) fillPicture(downArrow);
        else if(request == 4) fillPicture(leftArrow);
        else if(request == 5) fillPicture(center);
        
        requestTime = ticks;
      }
      else
      {
        fillRandom();
      }
      if(request == 8)
      {
	    osSleep(getRandBits(7) + 64);
	    request = 0;
      }
      else
      {
	    osSleep(getRandBits(5) + 16);
      }
    }
  }
  
  fillRandom();
}
