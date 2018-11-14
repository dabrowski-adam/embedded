/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use the i2c library.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <ea_init.h>
#include <framework.h>
#include "startup/config.h"
#include "i2c.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define LOCAL_EEPROM_ADDR 0x0         //if EEPROM has local address pins
#define EEPROM_ADDR       0xA0
#define I2C_EEPROM_ADDR   (EEPROM_ADDR + (LOCAL_EEPROM_ADDR << 1))


/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void delayMs(tU16 delayInMs);


/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Delay execution by a specified number of milliseconds by using
 *    timer #1. A polled implementation.
 *
 * Params:
 *    [in] delayInMs - the number of milliseconds to delay.
 *
 ****************************************************************************/
static void
delayMs(tU16 delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = delayInMs * ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (1000 * VPBDIV_FACTOR));
  TIMER1_IR  = 0xff;          //reset all interrrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer
  
  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01)
    ;
}


/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    The main-function. 
 *
 * Returns:
 *    Always 0, since return value is not used.
 *
 ****************************************************************************/
int
main(void)
{
	tU8 counter = '0';

  //initialize printf()-functionality
  eaInit();

  //print welcome message
  printf("\n*********************************************************");
  printf("\n*");
  printf("\n* (C) 2005 Embedded Artists AB");
  printf("\n*");
  printf("\n* Welcome to this program that demonstrates how to use");
  printf("\n* the i2c library to read / write in an EEPROM.");
  printf("\n*");
  printf("\n*********************************************************");

  //initialice i2c communication
  i2cInit(100000);

  //enter forever loop...
  while(1)
  {
  	tU16 length;
  	tU8  buffer[20];
  	tS8  retCode;

  	/*
  	 * read from EEPROM at address 0x00 with one address byte
  	 */
  	length = 0;
    i2cWrite(I2C_EEPROM_ADDR, I2C_EXTRA_BYTE, 0x00, buffer, length);
    retCode = i2cRepeatStart();
    if (retCode == I2C_CODE_OK)
      retCode = i2cRead(I2C_EEPROM_ADDR + 1, buffer, 15);
    printf("\n\nRead operation  [%s] status: %d (1 = OK, else failed)", buffer, retCode);
  	
  	/*
  	 * write to EEPROM at address 0x00 with one address byte
  	 */
  	buffer[0]  = 'T';
  	buffer[1]  = 'e';
  	buffer[2]  = 's';
  	buffer[3]  = 't';
  	buffer[4]  = 'i';
  	buffer[5]  = 'n';
  	buffer[6]  = 'g';
  	buffer[7]  = ' ';
  	buffer[8]  = 'i';
  	buffer[9]  = '2';
  	buffer[10] = 'c';
  	buffer[11] = ':';
  	buffer[12] = counter++;
  	buffer[13] = '!';
  	buffer[14] = 0x00;
  	length = 15;
  	
    retCode = i2cWrite(I2C_EEPROM_ADDR, I2C_EXTRA_BYTE, 0x00, buffer, length);
    i2cStop();
    printf("\nWrite operation [%s] status: %d (1 = OK, else failed) ", buffer, retCode);

    retCode = i2cPoll(I2C_EEPROM_ADDR);
    if (retCode != I2C_CODE_OK)
      printf("but EEPROM is not ready = ERROR!");

  	//wait 10000 ms
    delayMs(10000);
  }

  return 0;
}
