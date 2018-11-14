#include "testLEDMatrix.h"
#include "FreeRTOS.h"
#include "task.h"


#define  SPI_CS   0x00008000  //<= new board, old board = 0x00800000


const tU8 eaText[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,
  0x7f,0x49,0x49,0x36,0x00,      //B
  0x7e,0x09,0x09,0x7e,0x00,      //A
  0x48,0x54,0x55,0x24,0x00,      //�
//  0x26,0x49,0x49,0x32,0x00,      //S
  0x7f,0x08,0x14,0x22,0x41,0x00, //K
  0x7e,0x09,0x09,0x7e,0x00,      //A
  0x00,0x00,0x00,0x00,0x00,
  0x01,0x01,0x7f,0x01,0x01,0x00, //T
  0x3E,0x41,0x41,0x3e,0x00,      //O
  0x00,0x00,0x00,0x00,0x00,
  0x7f,0x20,0x18,0x20,0x7f,0x00, //W
  0x26,0x49,0x49,0x32,0x00,      //S
  0x01,0x01,0x7f,0x01,0x01,0x00, //T
  0x7f,0x19,0x29,0x46,0x00,      //R
  0x7f,0x49,0xC9,0x41,0x00,      //�
  0x01,0x01,0x7f,0x01,0x01,0x00, //T
  0x7F,0x02,0x1C,0x20,0x7F,0x00, //N
  0x7e,0x09,0x09,0x7e,0x00,      //A
  0x00,0x00,0x00,0x00,0x00,
  0x7f,0x08,0x14,0x22,0x41,0x00, //K
  0x7f,0x48,0x44,0x40,0x00,      //�
  0x7e,0x09,0x09,0x7e,0x00,      //A
  0x7f,0x02,0x04,0x02,0x7f,0x00, //M
  0x3E,0x41,0x41,0x22,0x00,      //C
  0x61,0x59,0x45,0x43,0x00,      //Z
  0x3f,0x40,0x40,0x3F,0x00,      //U
  0x3E,0x41,0x41,0x22,0x00,      //C
  0x7F,0x08,0x08,0x7F,0x00,      //H
  0x7e,0x09,0x09,0x7e,0x00,      //A
  0x00,0x00,0x00,0x00,0x00,
  0x7e,0x09,0x09,0x7e,0x00,      //A
  0x00,0x00,0x00,0x00,0x00,
  0x7f,0x41,0x41,0x3e,0x00,      //D
  0x7f,0x49,0x49,0x41,0x00,      //E
  0x7f,0x02,0x04,0x02,0x7f,0x00, //M
  0x3f,0x40,0x40,0x3F,0x00,      //U
  0x48,0x54,0x55,0x24,0x00,      //�
//  0x26,0x49,0x49,0x32,0x00,      //S
  0x00,0x00,0x00,0x00,0x00,
  0x20,0x40,0x41,0x3F,0x00,      //J
  0x7f,0x49,0x49,0x41,0x00,      //E
  0x26,0x49,0x49,0x32,0x00,      //S
  0x61,0x59,0x45,0x43,0x00,      //Z
  0x3E,0x41,0x41,0x22,0x00,      //C
  0x61,0x59,0x45,0x43,0x00,      //Z
  0x7f,0x49,0x49,0x41,0x00,      //E
  0x00,0x00,0x00,0x00,0x00,
  0x3E,0x41,0x51,0x32,0x00,      //G
  0x3E,0x41,0x41,0x3e,0x00,      //O
  0x7f,0x19,0x29,0x46,0x00,      //R
  0x26,0x49,0x49,0x32,0x00,      //S
  0x61,0x59,0x45,0x43,0x00,      //Z
  0x03,0x04,0x78,0x04,0x03, 0x00,//Y
//  0x7f,0x49,0x49,0x41,0x00,      //E
//  0x7f,0x02,0x04,0x02,0x7f,0x00, //M
//  0x7f,0x49,0x49,0x36,0x00,      //B
//  0x7f,0x49,0x49,0x41,0x00,      //E
//  0x7f,0x41,0x41,0x3e,0x00,      //D
//  0x7f,0x41,0x41,0x3e,0x00,      //D
//  0x7f,0x49,0x49,0x41,0x00,      //E
//  0x7f,0x41,0x41,0x3e,0x00,      //D
//  0x00,0x00,0x00,0x00,0x00,
//  0x7e,0x09,0x09,0x7e,0x00,      //A
//  0x7f,0x19,0x29,0x46,0x00,      //R
//  0x01,0x01,0x7f,0x01,0x01,0x00, //T
//  0x7f,0x00,                     //I
//  0x26,0x49,0x49,0x32,0x00,      //S
//  0x01,0x01,0x7f,0x01,0x01,0x00, //T
//  0x26,0x49,0x49,0x32,0x00,      //S
//	0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00};

tU8 pattern[8] = {1,3,7,15,31,63,127,255};

void ledMatrix(void);


/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/
static void
startTimer1(tU32 delayInMs)
{
  //initialize VIC for Timer1 interrupts
  VICIntSelect &= ~0x20;           //Timer1 interrupt is assigned to IRQ (not FIQ)
  VICVectAddr5  = (tU32)ledMatrix; //register ISR address
  VICVectCntl5  = 0x25;            //enable vector interrupt for timer1
  VICIntEnable  = 0x20;            //enable timer1 interrupt

  //initialize and start Timer #1
  T1TCR = 0x00000002;                           //disable and reset Timer1
  T1PC  = 0x00000000;                           //no prescale of clock
  T1MR0 = delayInMs *                           //calculate no of timer ticks
         (configCPU_CLOCK_HZ / (1000 * configVPBDIV_FACTOR));
  T1IR  = 0x000000ff;                           //reset all flags before enable IRQs
  T1MCR = 0x00000003;                           //reset counter and generate IRQ on MR0 match
  T1TCR = 0x00000001;                           //start Timer1
}


/*****************************************************************************
 *
 * Description:
 *    A process entry function
 *
 ****************************************************************************/
void testLEDMatrix(void *pvParameters)
{
  tU16 cntA = 0;
  tU8 i;
//  tU32 sw = 0x55555555;

  PINSEL0 |= 0x00001500 ;  //Inicjalizacja  SPI
  SPI_SPCCR = 0x08;
  SPI_SPCR  = 0x60;
  IODIR0 |= SPI_CS;


  startTimer1(2);

  for(;;)
  {
  	vTaskDelay(200);

//	GPIO_IOSET = sw;
//  	GPIO_IOCLR = ~sw;
//	sw = ~sw;

	cntA++;
//	cntA %= sizeof(eaText)-8;
    if (cntA > sizeof(eaText)-8)
      cntA = 0;

	for ( i=0; i < 8; i++)
	{
		pattern[i] = eaText[cntA+i];
	}
  }
}
