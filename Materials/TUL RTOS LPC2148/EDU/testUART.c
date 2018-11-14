#include "testUART.h"
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"



xComPortHandle uart0;

void UARTWrStr (char*str)
{
	char c;

	while ( *str != 0)
	{
		c=*str++;
		if (c == '\n')
			c = '\r';
		xSerialPutChar( uart0, c, 1000 );
	}
}

void InitSerial0(void)
{
	uart0 = xSerialPortInitMinimal( configUART_BAUD_RATE, 16 );
}


/*
void testUART(void *pvParameters)
{


	for ( ;; )
	{
		vTaskDelay (100000);
//		UARTWrStr ("Aplikacja przyk³adowa\n");
	}
}
 */

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
void  SendNumber(unsigned char base,
                 unsigned char noDigits,
                 unsigned char sign,
                 char          pad,
                 int           number)
{
  static char    hexChars[16] = "0123456789ABCDEF";
  char          *pBuf;
  char           buf[32];
  unsigned int   numberAbs;
  unsigned char  count;

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
  UARTWrStr(pBuf);
}

 /*
int GetIntNum(void)
{
  char  abStr[30];
  char *pString = abStr;
  int   wBase=10;
  int   wMinus=0;
  int   wLastIndex;
  int   wResult=0;
  int   wI;

  consolGetString(pString);

  if(pString[0] == '-')
  {
    wMinus = 1;
    pString++;
  }

  if(pString[0] == '0' && (pString[1] == 'x' || pString[1] == 'X'))
  {
    wBase = 16;
    pString += 2;
  }

  wLastIndex = strlen(pString) - 1;
  if(pString[wLastIndex] == 'h' || pString[wLastIndex] == 'H')
  {
    wBase = 16;
    pString[wLastIndex] = 0;
    wLastIndex--;
  }

  if(wBase == 10)
  {
    wResult = atoi(pString);
    wResult = wMinus ? (-1*wResult):wResult;
  }
  else
  {
    for(wI=0; wI<=wLastIndex; wI++)
    {
      if(__isalpha(pString[wI]))
      {
        if(__isupper(pString[wI]))
          wResult = (wResult<<4) + pString[wI] - 'A' + 10;
        else
          wResult = (wResult<<4) + pString[wI] - 'a' + 10;
      }
      else
      {
        wResult = (wResult<<4) + pString[wI] - '0';
      }
    }
    wResult = wMinus ? (-1*wResult):wResult;
  }
  return wResult;
}
*/



