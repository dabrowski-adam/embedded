#ifndef _LCD_H
#define _LCD_H
#include <LPC213x.H>

void LCDDelay(void);
void LCDText(char *T);
void LCDTextXY(char x, char y, char *T);
void LCDSendByte(unsigned int data_lcd,unsigned int cmd);
void LCDInit(void);
void LCDClear(void);
void LCD32bitNumberDec(int number);
void LCDGotoXY(char x, char y);
void LCDDisplayShiftLeft(void);
void LCDDisplayShiftRight(void);
#endif
