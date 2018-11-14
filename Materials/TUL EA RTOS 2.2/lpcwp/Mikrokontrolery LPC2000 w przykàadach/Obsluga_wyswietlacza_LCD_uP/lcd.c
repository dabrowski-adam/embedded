#include "lcd.h"

const char dat_lcd = 0x01; 		//send data
const char cmd_lcd = 0x00; 		//send command

const unsigned int E  = 0x40000000; // P0.30
const unsigned int RS = 0x80000000; // P0.31
const char cmd_bit = 31;


void LCDDelay (void) 
{ 
  unsigned int i;
  
  for(i = 0; i < 0x100; i++);	   
}								    

char* convert32bit_dec(int val,char *str)
{
	int v;
	signed char i,j,p;

	if(val<0)
		v=-val;
	else
		v=val;
		
	for(i=12;i>=0;i--)
	{	
		str[i]=(v%10)+'0';
		v=v/10;
		if(!v)
			break;
	}
	if(val<0)
		p=1;
	else
		p=0;
		
	j=i;
	for(i=p;j<13;j++,i++)
		str[i]=str[j];  

	if(val<0)
		str[0]='-';

	str[i]=0;
	return str;
}

// ustawia kursor na wspó³rzêdnych x, y
void LCDGotoXY(char x, char y)
{
	switch (y)			//obliczenie o ile nale¿y zwiêkszyæ
 					    //x w zale¿noœci od wartoœci y
	{
		case 0:
			x += 0x80;	//x = x + 0x80
			break;
		case 1:
			x += 0xC0;	//x = x + 0xC0
			break;
		case 2:
			x += 0x94;	//x = x + 0x94
			break;
		case 3:
			x += 0xD4;	//x = x + 0xD4
			break;
	}
	LCDSendByte( x , cmd_lcd );
}


//wyœwietla tekst rozpoczynaj¹c od aktualnego po³o¿enia kursora
void LCDText(char *T)
{
	while (*T)			//pêtla dzia³a, a¿ napotka 
		LCDSendByte(*T++,dat_lcd);		//wyœwietlenie pojedynczego znaku
}

//wyœwietla tekst rozpoczynaj¹c od wsp. (X,Y)
void LCDTextXY(char x, char y, char *T)
{
	while (*T)
	{
		LCDGotoXY(x, y);
		LCDSendByte(*T,dat_lcd);		//wyœwietlenie pojedynczego znaku
		x++; 			//nastêpna pozycja x na ekranie
		T++;			//nastêpny znak napisu
		if (x > 19)		//jeœli x>19 to nastêpna linia
		{
			x = 0; 
			y++;
		}
	}
}				   

void LCDSendByte(unsigned int data_lcd,unsigned int cmd)
{
  IOCLR0 = 0xFFFFFFFF & (E | RS);
  LCDDelay();
  IOCLR1 = 0x00FF0000;
  LCDDelay();

  // RS=cmd, E=0
  IOSET0 = 0x00000000 | (cmd << cmd_bit);                  
  LCDDelay();   
 
  // Set data
  IOSET1 = ((data_lcd & 0xFF) << 16) & 0x00FF0000;    
  LCDDelay();

  // RS=cmd, E=1
  IOSET0 = (0xFF000000 & E) | (cmd << cmd_bit);			  
  LCDDelay();   	 

  // RS=cmd, E=0
  IOCLR0 = (0xFF000000 & E);

  IOCLR1 = 0x00FF0000;		                          
  LCDDelay();
} 

void LCDInit(void)
{
   LCDSendByte(0x30,cmd_lcd);	LCDDelay();  LCDDelay();  LCDDelay();
   LCDSendByte(0x30,cmd_lcd);	LCDDelay();  LCDDelay();  LCDDelay();
   LCDSendByte(0x30,cmd_lcd);   LCDDelay();  LCDDelay();  LCDDelay();

   LCDSendByte(0x38,cmd_lcd);   LCDDelay();
   LCDSendByte(0x0C,cmd_lcd);   LCDDelay();   
   LCDSendByte(0x06,cmd_lcd);   LCDDelay();   
   LCDSendByte(0x01,cmd_lcd);   LCDDelay();   
   LCDSendByte(0x02,cmd_lcd);   LCDDelay();   

}

void LCDClear(void)
{
  int i;
  LCDSendByte(0x01,cmd_lcd);
  for(i = 0; i < 0x10000; i++);
}

void LCD32bitNumberDec(int number)
{
	char buf[15];
	LCDText(convert32bit_dec(number,buf));
}

void LCDDisplayShiftLeft(void)
{
    LCDSendByte(0x18,cmd_lcd);
}

void LCDDisplayShiftRight(void)
{
    LCDSendByte(0x1C,cmd_lcd);
}
