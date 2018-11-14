/*****************************************************************************
 *   i2cmst.c:  main C entry file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC214x.h"                        /* LPC21xx definitions */
#include "type.h"
#include "irq.h"
#include "i2c.h"

extern DWORD I2CCount;
extern BYTE I2CMasterBuffer[BUFSIZE];
extern DWORD I2CCmd, I2CMasterState;
extern DWORD I2CReadLength, I2CWriteLength;

/*******************************************************************************
**   Main Function  main()
*******************************************************************************/
int main (void)
{
    DWORD i;

    init_VIC();
    if ( I2CInit( (DWORD)I2CMASTER ) == FALSE )	/* initialize I2c */
    {
	while ( 1 );						/* Fatal error */
    }

    /* In order to start the I2CEngine, the all the parameters 
    must be set in advance, including I2CWriteLength, I2CReadLength,
    I2CCmd, and the I2cMasterBuffer which contains the stream
    command/data to the I2c slave device. */

  	//PAGE WRITE TO E2PROM
    for ( i = 0; i < BUFSIZE; i++ )		/* clear buffer */
    {
	I2CMasterBuffer[i] = 0;
    }
    I2CWriteLength = 10;
    I2CReadLength = 0;
    I2CMasterBuffer[0] = PCF_ADDR;
    I2CMasterBuffer[1] = PCF_WORD;
    I2CMasterBuffer[2] = 0x01;
	I2CMasterBuffer[3] = 0x02;
	I2CMasterBuffer[4] = 0x03;
	I2CMasterBuffer[5] = 0x04;
	I2CMasterBuffer[6] = 0x05;
	I2CMasterBuffer[7] = 0x06;
	I2CMasterBuffer[8] = 0x07;
	I2CMasterBuffer[9] = 0x08;
    I2CEngine(); 

    //CLEAR I2C BUFFER
    for ( i = 0; i < BUFSIZE; i++ ){	
		I2CMasterBuffer[i] = 0;
	}
    
	//READ FROM E2PROM
	I2CWriteLength = 1;
    I2CReadLength = 8;
    I2CMasterBuffer[0] = PCF_ADDR;
    I2CMasterBuffer[1] = PCF_WORD;
	I2CMasterBuffer[2] = PCF_ADDR | RD_BIT;
    I2CCmd = GET_DATA;
    I2CEngine(); 

    // The data resides in I2CMasterBuffer byte 3, 4, ...
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
