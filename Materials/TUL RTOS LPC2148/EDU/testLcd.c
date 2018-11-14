#include "testLCD.h"
#include "FreeRTOS.h"
#include "task.h"

#define LCD_DATA      0x00ff0000  //P1.16-P1.23
#define LCD_E         0x02000000  //P1.25
#define LCD_RW        0x00400000  //P0.22
#define LCD_RS        0x01000000  //P1.24
#define LCD_BACKLIGHT 0x40000000  //P0.30







void initLCD (void);
void delay37us(void);
void writeLCD(tU8 reg, tU8 data);
void lcdBacklight(tU8 onOff);
void WriteString (char *str);



void initLCD (void)
{
	IODIR1 |= (LCD_DATA | LCD_E | LCD_RS);
	IOCLR1  = (LCD_DATA | LCD_E | LCD_RS);

	IODIR0 |= LCD_RW;
	IOCLR0  = LCD_RW;

  	IODIR0 |= LCD_BACKLIGHT;
  	IOCLR0  = LCD_BACKLIGHT;
}

void delay37us(void)
{
	vTaskDelay(1);
//	volatile tU32 i;

//Temp test for 140 uS delay
//	for(i=0; i<6*2500; i++)
//    asm volatile (" nop"); //delay 15 ns x 2500 = about 37 us delay

}

void writeLCD(tU8 reg, tU8 data)
{
	volatile tU8 i;

	if (reg == 0)
	  IOCLR1 = LCD_RS;
	else
	  IOSET1 = LCD_RS;

  	IOCLR0 = LCD_RW;
	IOCLR1 = LCD_DATA;
	IOSET1 = ((tU32)data << 16) & LCD_DATA;
//printf("\nwrite: %x", data);

	IOSET1 = LCD_E;
//	osSleep(1);
	for(i=0; i<16; i++)
    	asm volatile (" nop"); //delay 15 ns x 16 = about 250 ns delay
	IOCLR1 = LCD_E;
//	osSleep(1);
	for(i=0; i<16; i++)
    asm volatile (" nop"); //delay 15 ns x 16 = about 250 ns delay
}



void lcdBacklight(tU8 onOff)
{
	if (onOff == TRUE)
	  IOSET0 = LCD_BACKLIGHT;
	else
	  IOCLR0 = LCD_BACKLIGHT;
}



void WriteString (char *str)
{
	while ( *str )
	{
	    writeLCD(1, *str++);
    	delay37us();
	}
}


char* Gorny_Wiersz = "Malgosia-Bambosia wsciekla zona jest!";
char* Dolny_Wiersz = "Potwor Basia i Dementror straszliwy Zosia";

void testLCD(void *pvParameters)
{
	char *gw = Gorny_Wiersz;
	char *dw = Dolny_Wiersz;

  	initLCD();
	lcdBacklight(FALSE);
  	vTaskDelay(10);

  	for(;;)
  	{
 		lcdBacklight(TRUE);
    	vTaskDelay(50);

    	//function set
    	writeLCD(0, 0x30);
    	vTaskDelay(1);
    	writeLCD(0, 0x30);
    	delay37us();
    	writeLCD(0, 0x30);
    	delay37us();

    	//function set
    	writeLCD(0, 0x38);
    	delay37us();

    	//display off
    	writeLCD(0, 0x08);
    	delay37us();

    	//display clear
    	writeLCD(0, 0x01);
    	vTaskDelay(1); //actually only 1.52 mS needed

    	//display control set
    	writeLCD(0, 0x06);
    	vTaskDelay(1);

    	//display control set
    	writeLCD(0, 0x0c);
    	delay37us();

    	//cursor home
    	writeLCD(0, 0x02);
		vTaskDelay(1);

		WriteString(gw);

    	//move curstor to second row
    	writeLCD(0, 0x80 | 0x40);
    	delay37us();

//		vTaskDelay(20);
		WriteString(dw);
		if ( *gw != 0)
			gw++;
		if ( *dw != 0)
			dw++;
		if ( *dw==0 && *gw==0)
		{
			gw = Gorny_Wiersz;
			dw = Dolny_Wiersz;

	    	//display clear
    		writeLCD(0, 0x01);
    		vTaskDelay(1); //actually only 1.52 mS needed

			lcdBacklight(FALSE);
			vTaskDelay(5000);
		}

    	vTaskDelay(300);
/*
		lcdBacklight(FALSE);
    	vTaskDelay(50);
		lcdBacklight(TRUE);
    	vTaskDelay(50);
		lcdBacklight(FALSE);
    	vTaskDelay(50);
		lcdBacklight(TRUE);
    	vTaskDelay(50);
		lcdBacklight(FALSE);
    	vTaskDelay(50);
		lcdBacklight(TRUE);
    	vTaskDelay(50);
		lcdBacklight(FALSE);
    	vTaskDelay(50);
*/
  }
//	lcdBacklight(FALSE);
}
