/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

/*
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 *
 * Main.c also creates a task called "Check".  This only executes every three
 * seconds but has the highest priority so is guaranteed to get processor time.
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is
 * incremented each time the task successfully completes its function.  Should
 * any error occur within such a task the count is permanently halted.  The
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 * To check the operation of the memory allocator the check task also
 * dynamically creates a task before delaying, and deletes it again when it
 * wakes.  If memory cannot be allocated for the new task the call to xTaskCreate
 * will fail and an error is signalled.  The dynamically created task itself
 * allocates and frees memory just to give the allocator a bit more exercise.
 *
 */

/*
	Changes from V2.4.2

	+ The vErrorChecks() task now dynamically creates then deletes a task each
	  cycle.  This tests the operation of the memory allocator.

	Changes from V2.5.2

	+ vParTestInitialise() is called during initialisation to ensure all the
	  LED's start off.
*/


/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo application includes. */
#include "testLEDMatrix.h"
#include "TestRGB.h"
#include "testLCD.h"
#include "testMotor.h"
#include "serial.h"
#include "testUART.h"
#include "testADC.h"

/*-----------------------------------------------------------*/

/* Constants to setup I/O. */
#define UART0_TX_ENABLE	( ( unsigned portLONG ) 0x0001 )
#define UART0_RX_ENABLE	( ( unsigned portLONG ) 0x0004 )
#define IO_P0_14		( ( unsigned portLONG ) 0x4000 )
#define JTAG_PORT	    ( ( unsigned portLONG ) 0x3E0000UL )


/* Constants to setup the peripheral bus. */
//#define mainBUS_CLK_FULL	( ( unsigned portCHAR ) 0x01 )


/* Priorities for the demo application tasks. */
#define mainLED_TASK_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainRGB_TASK_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainLCD_TASK_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainMotor_TASK_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainADC_TASK_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainUART_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
//#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 0 )
//#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 4 )
//#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 0 )
//#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )

/* The rate at which the on board LED will toggle when there is/is not an
error. */
//#define mainNO_ERROR_FLASH_PERIOD	( ( portTickType ) 3000 / portTICK_RATE_MS  )
//#define mainERROR_FLASH_PERIOD		( ( portTickType ) 500 / portTICK_RATE_MS  )
//#define mainON_BOARD_LED_BIT		( ( unsigned portLONG ) 0x80 )

/* Constants used by the vMemCheckTask() task. */
//#define mainCOUNT_INITIAL_VALUE		( ( unsigned portLONG ) 0 )
//#define mainNO_TASK					( 0 )

/* The size of the memory blocks allocated by the vMemCheckTask() task. */
//#define mainMEM_CHECK_SIZE_1		( ( size_t ) 51 )
//#define mainMEM_CHECK_SIZE_2		( ( size_t ) 52 )
//#define mainMEM_CHECK_SIZE_3		( ( size_t ) 151 )

/*-----------------------------------------------------------*/


/*
 * Configure the processor for use with the Olimex demo board.  This includes
 * setup for the I/O, system clock, and access timings.
 */
static void prvSetupHardware( void );

/*-----------------------------------------------------------*/

/*
 * Starts all the other tasks, then starts the scheduler.
 */
void TestLED(void);

void TestLED(void)
{
	int 	i;
	tU32    pattern;

	PINSEL0 &= 0x0000ffff;
    IODIR0  |= 0x0000ff00;
    IOSET0   = 0x0000fb00;
    IOCLR0   = 0x00000400;

    for(i=0; i<4; i++)
    {
      	pattern = 0x00000100;
      	while(pattern <= 0x00008000)
      	{
      		IOCLR = pattern;
      		vTaskDelay(100);
      		IOSET0 = pattern;
        	IOCLR0 = 0x00000400; //P0.10 must always be low = LED on since motor control else will be enables

      		pattern <<= 1;
      	}
    }
    //flash with P0.10 (with P0.8 and P0.9 low)
    IOCLR0 = 0x00000700;
    vTaskDelay(100);
    IOSET0 = 0x00000400;
   	vTaskDelay(100);
    IOCLR0 = 0x00000400;
    vTaskDelay(100);
    IOSET0 = 0x00000400;
   	vTaskDelay(100);
    IOCLR0 = 0x00000400;
    vTaskDelay(100);
    IOSET0 = 0x00000400;

}

int main( void )
{
	prvSetupHardware();

    //immediately turn off buzzer (if connected)
	IODIR0 |= 0x00000080;
    IOSET0  = 0x00000080;
/*
	printf("\n\n\n\n\n*******************************************************\n");
    printf("*                                                     *\n");
    printf("* This is the test program for Embedded Artists'      *\n");
    printf("* LPC2148 Education Board...                          *\n");
    printf("*                                                     *\n");
    printf("* Version: 1.1 (special version without MMC test)     *\n");
    printf("* Date:    26 Nov 2005                                *\n");
    printf("* (C) Embedded Artists 2005                           *\n");
    printf("*                                                     *\n");
    printf("*******************************************************\n");
*/



	/* Start the check task - which is defined in this file. */
	xTaskCreate( testLEDMatrix,
	            ( signed portCHAR * ) "LED",
				configMINIMAL_STACK_SIZE,
				NULL,
				mainLED_TASK_PRIORITY,
				NULL);
/*
	xTaskCreate( testRGB,
	            ( signed portCHAR * ) "RGB",
				configMINIMAL_STACK_SIZE,
				NULL,
				mainRGB_TASK_PRIORITY,
				NULL);
 */
	xTaskCreate( testLCD,
	            ( signed portCHAR * ) "LCD",
				configMINIMAL_STACK_SIZE,
				NULL,
				mainLCD_TASK_PRIORITY,
				NULL);
	xTaskCreate( testMotor,
	            ( signed portCHAR * ) "Motor",
				configMINIMAL_STACK_SIZE,
				NULL,
				mainMotor_TASK_PRIORITY,
				NULL);
	xTaskCreate( testADC,
	            ( signed portCHAR * ) "ADC",
				configMINIMAL_STACK_SIZE,
				NULL,
				mainADC_TASK_PRIORITY,
				NULL);

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here! */
	return 0;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Configure the RS2332 pins.  All other pins remain at their default of 0. */
	PINSEL0 |= UART0_TX_ENABLE;
	PINSEL0 |= UART0_RX_ENABLE;

	/* Set all GPIO to output other than the P0.14 (BSL), and the JTAG pins.
	The JTAG pins are left as input as I'm not sure what will happen if the
	Wiggler is connected after powerup - not that it would be a good idea to
	do that anyway. */
	IODIR = ~( IO_P0_14 + JTAG_PORT );


	//PLL factor jest na 5 m³odszych bitach, podzielnik na starszych
	PLLCFG = (configPLL_FACTOR-1) | ((configPLL_DIV-1)<<5);
//	PLLCFG = 0x24;


#define PLL_CONNECT		 0x0003
#define PLL_FEED_BYTE1	   0xaa
#define PLL_FEED_BYTE2	   0x55
#define PLL_LOCK		 0x0400

	/* Activate the PLL by turning it on then feeding the correct sequence of
	bytes. */
	PLLCON  = configUSE_PLL;
	PLLFEED = PLL_FEED_BYTE1;
	PLLFEED = PLL_FEED_BYTE2;

	/* Wait for the PLL to lock... */
	while( !( PLLSTAT & PLL_LOCK ) );

	/* ...before connecting it using the feed sequence again. */
	PLLCON  = PLL_CONNECT;
	PLLFEED = PLL_FEED_BYTE1;
	PLLFEED = PLL_FEED_BYTE2;


	/* Setup and turn on the MAM.  Three cycle access is used due to the fast
	PLL used.  It is possible faster overall performance could be obtained by
	tuning the MAM and PLL settings. */
	MAMTIM = MAM_TIMING;
	MAMCR = MAM_MAP;

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = configVPBDIV_FACTOR;

	InitSerial0();

}
/*-----------------------------------------------------------*/




