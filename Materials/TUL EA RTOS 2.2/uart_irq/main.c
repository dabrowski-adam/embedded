/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use UART #0 in interrupt mode.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include "uart.h"
#include "startup/config.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

/******************************************************************************
 * Local variables
 *****************************************************************************/

/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void printNumber(tU8  base, tU8  noDigits, tU8  sign, tU8  pad, tU32 number);

/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Routine for printing integer numbers in various formats. The number is 
 *    printed in the specified 'base' using exactly 'noDigits', using +/- if 
 *    signed flag 'sign' is TRUE, and using the character specified in 'pad' 
 *    to pad extra characters. 
 *
 * Params:
 *    [in] base     - Base to print number in (2-16) 
 *    [in] noDigits - Number of digits to print (max 32) 
 *    [in] sign     - Flag if sign is to be used (TRUE), or not (FALSE) 
 *    [in] pad      - Character to pad any unused positions 
 *    [in] number   - Signed number to print 
 *
 ****************************************************************************/
static void
printNumber(tU8  base,
            tU8  noDigits,
            tU8  sign,
            tU8  pad,
            tU32 number)
{
  static tU8  hexChars[16] = "0123456789ABCDEF";
  tU8        *pBuf;
  tU8         buf[32];
  tU32        numberAbs;
  tU32        count;

  // prepare negative number
  if(sign && (number < 0))
    numberAbs = -number;
  else
    numberAbs = number;

  // setup little string buffer
  count = (noDigits - 1) - (sign ? 1 : 0);
  pBuf = buf + sizeof(buf);
  *--pBuf = '\0';

  // force calculation of first digit
  // (to prevent zero from not printing at all!!!)
  *--pBuf = hexChars[(numberAbs % base)];
  numberAbs /= base;

  // calculate remaining digits
  while(count--)
  {
    if(numberAbs != 0)
    {
      //calculate next digit
      *--pBuf = hexChars[(numberAbs % base)];
      numberAbs /= base;
    }
    else
      // no more digits left, pad out to desired length
      *--pBuf = pad;
  }

  // apply signed notation if requested
  if(sign)
  {
    if(number < 0)
      *--pBuf = '-';
    else if(number > 0)
       *--pBuf = '+';
    else
       *--pBuf = ' ';
  }

  // print the string right-justified
  uart0SendString(pBuf);
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
  //initialize uart #0: 115.2 kbps, 8N1, no FIFO
  initUart0(B115200((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR), UART_8N1, UART_FIFO_OFF);

  //print welcome message
  uart0SendString("\n*********************************************************");
  uart0SendString("\n*");
  uart0SendString("\n* (C) 2005 Embedded Artists AB");
  uart0SendString("\n*");
  uart0SendString("\n* Welcome to this program that demonstrates how the UART");
  uart0SendString("\n* can be used in interrupt mode.");
  uart0SendString("\n* It's a simple echo application...");
  uart0SendString("\n*");
  uart0SendString("\n*********************************************************");
  
  while(1)
  {
    static tU32 rxCharCnt = 0;
    tU8         rxChar;
    
    //check if any character has been received
    if (uart0GetChar(&rxChar) == TRUE)
    {
      uart0SendString("\nThis is the ");
      printNumber(10, 3, FALSE, ' ', ++rxCharCnt);
      uart0SendString(" received char: ");
      uart0SendCh(rxChar);
    }
  }

  return 0;
}
