/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    uart.c
 *
 * Description:
 *    Implementation of interrupt driven UART.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include "uart.h"
#include "irq_code/irqUart.h"


/*****************************************************************************
 * Local variables
 ****************************************************************************/
tU8 uart0TxBuf[TX_BUFFER_SIZE];
volatile tU32 uart0TxHead = 0;
volatile tU32 uart0TxTail = 0;
volatile tU8  uart0TxRunning = FALSE;

tU8 uart0RxBuf[RX_BUFFER_SIZE];
volatile tU32 uart0RxHead = 0;
volatile tU32 uart0RxTail = 0;


/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Initialize UART #0 in polled mode, i.e., interrupts are not used.
 *
 * Parameters:
 *    [in] div_factor - UART clock division factor to get desired bit rate.
 *                      Use definitions in uart.h to calculate correct value.
 *    [in] mode       - transmission format settings. Use constants in uart.h
 *    [in] fifo_mode  - FIFO control settings. Use constants in uart.h
 *
 ****************************************************************************/
void
initUart0(tU16 div_factor, tU8 mode, tU8 fifo_mode)
{
  volatile tU32 dummy;
  
  //enable uart #0 pins in GPIO (P0.0 = TxD0, P0.1 = RxD0)
  PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x00000005;

  U0IER = 0x00;                        //disable all uart interrupts
  dummy = U0IIR;                       //clear all pending interrupts
  dummy = U0RBR;                       //clear receive register
  dummy = U0LSR;                       //clear line status register

  //set the bit rate = set uart clock (pclk) divisionfactor
  U0LCR = 0x80;                        //enable divisor latches (DLAB bit set, bit 7)
  U0DLL = (tU8)div_factor;             //write division factor LSB
  U0DLM = (tU8)(div_factor >> 8);      //write division factor MSB

  //set transmissiion and fifo mode
  U0LCR = (mode & ~0x80);              //DLAB bit (bit 7) must be reset
  U0FCR = fifo_mode;

  //initialize the interrupt vector
  VICIntSelect &= ~0x00000040;      // UART0 selected as IRQ
  VICVectCntl6  =  0x00000026;
  VICVectAddr6  =  (tU32)uart0ISR;  // address of the ISR
  VICIntEnable |=  0x00000040;      // UART0 interrupt enabled

  //initialize the transmit data queue
  uart0TxHead    = 0;
  uart0TxTail    = 0;
  uart0TxRunning = FALSE;

  //initialize the receive data queue
  uart0RxHead   = 0;
  uart0RxTail   = 0;

  //enable receiver interrupts
  U0IER = 0x01;
}

/*****************************************************************************
 *
 * Description:
 *    Blocking output routine, i.e., the routine waits until the uart 
 *    buffer is free and then sends the character. 
 *
 * Params:
 *    [in] charToSend - The character to print (to uart #0) 
 *
 ****************************************************************************/
void
uart0SendChar(tU8 charToSend)
{
  volatile tU32 cpsrReg;
  tU32 tmpHead;

  //calculate head index
  tmpHead = (uart0TxHead + 1) & TX_BUFFER_MASK; 

  //wait for free space in buffer
  while(tmpHead == uart0TxTail)
    ;

  //disable IRQ
  cpsrReg = disIrq();

  U0IER &= ~0x02;   //disable TX IRQ

  //enable IRQ
  restoreIrq(cpsrReg);

  if(uart0TxRunning == TRUE)
  {
    uart0TxBuf[tmpHead] = charToSend; 
    uart0TxHead         = tmpHead;
  }
  else
  {
    uart0TxRunning = TRUE;
    U0THR          = charToSend;
  }

  //disable IRQ
  cpsrReg = disIrq();

  U0IER |= 0x02;   //enable TX IRQ

  //enable IRQ
  restoreIrq(cpsrReg);
}


/*****************************************************************************
 *
 * Description:
 *    Output routine that adds extra line feeds at line breaks. 
 *
 * Params:
 *    [in] charToSend - The character to print (to uart #0) 
 *
 ****************************************************************************/
void
uart0SendCh(tU8 charToSend)
{
  if(charToSend == '\n')
    uart0SendChar('\r');

  uart0SendChar(charToSend);
}


/*****************************************************************************
 *
 * Description:
 *    Print NULL-terminated string to uart #0. 
 *
 * Params:
 *    [in] pString - Pointer to NULL-terminated string to be printed 
 *
 ****************************************************************************/
void
uart0SendString(tU8 *pString)
{
  while(*pString)
    uart0SendCh(*pString++);
}

/*****************************************************************************
 *
 * Description:
 *    Print a fixed number of bytes (as opposed to NULL-terminated string).
 *
 * Params:
 *    [in] pBuff - The character to print (to uart #0) 
 *    [in] count - Number of characters to print
 *
 ****************************************************************************/
void
uart0SendChars(char *pBuff, tU16 count)
{
  while (count--)
    uart0SendChar(*pBuff++);
}

/*****************************************************************************
 *
 * Description:
 *    Blocking function that waits for a received character. 
 *
 * Return:
 *    The received character. 
 *
 ****************************************************************************/
tU8
uart0GetCh(void)
{
	tU8 rxChar;

  //wait for a character to be available
  while(uart0GetChar(&rxChar) == FALSE)
    ;
  return rxChar;
}

/*****************************************************************************
 *
 * Description:
 *    Non-blocking receive function.
 *
 * Params:
 *    [in] pRxChar - Pointer to buffer where the received character shall
 *                   be placed.
 *
 * Return:
 *    TRUE if character was received, else FALSE.
 *
 ****************************************************************************/
tU8
uart0GetChar(tU8 *pRxChar)
{
  tU32 tmpTail;

  /* buffer is empty */
  if(uart0RxHead == uart0RxTail)
    return FALSE;

  tmpTail     = (uart0RxTail + 1) & RX_BUFFER_MASK;
  uart0RxTail = tmpTail; 

  *pRxChar = uart0RxBuf[tmpTail];
  return TRUE;
}
