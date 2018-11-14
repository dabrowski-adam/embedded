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
#include "LPC214x.H"                        /* LPC21xx definitions */
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
	while ( 1 );				/* Fatal error */
    }

    /* the example used to test the I2C interface is 
    a Philips's SE95 temp sensor. LPC214x is used a I2C
    master, the temp sensor is a I2C slave.
    
    The sequence to get the temp reading is: 
	get device ID register,
	set configuration register,
	get temp reading
    */

    /* In order to start the I2CEngine, the all the parameters 
    must be set in advance, including I2CWriteLength, I2CReadLength,
    I2CCmd, and the I2cMasterBuffer which contains the stream
    command/data to the I2c slave device. */

    /* Get device ID register */
    for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
    {
	I2CMasterBuffer[i] = 0;
    }
    I2CWriteLength = 1;
    I2CReadLength = 1;
    I2CMasterBuffer[0] = SE95_ADDR;
    I2CMasterBuffer[1] = SE95_ID;
    I2CMasterBuffer[2] = SE95_ADDR | RD_BIT;
    I2CCmd = GET_DEVICE_ID;
    I2CEngine(); 

    /* Configure temp register before reading */
    for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
    {
	I2CMasterBuffer[i] = 0;
    }
    I2CWriteLength = 2;
    I2CReadLength = 0;
    I2CMasterBuffer[0] = SE95_ADDR;
    I2CMasterBuffer[1] = SE95_CONFIG;
    I2CMasterBuffer[2] = 0x00;		/* configuration value, no change from 
					default */
    I2CCmd = SET_CONFIGURATION;
    I2CEngine(); 

    /* Get temp reading */
    for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
    {
	I2CMasterBuffer[i] = 0;
    }
    I2CWriteLength = 1;
    I2CReadLength = 2;
    I2CMasterBuffer[0] = SE95_ADDR;
    I2CMasterBuffer[1] = SE95_TEMP;
    I2CMasterBuffer[2] = SE95_ADDR | RD_BIT;
    I2CCmd = GET_TEMPERATURE;
    I2CEngine();

    /* The temp reading value should reside in I2CMasterBuffer byte 3, 4, ... */ 
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
