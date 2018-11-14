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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc2xxx.h>


/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/


#define configUSE_PREEMPTION		1
// configUSE_PREEMPTION set to 1 to use the preemptive kernel, or 0 to use the cooperative kernel.
#define configUSE_IDLE_HOOK			0
// Je¿eli =1, to znaczy, ¿e w czasie jak nic nie robi, to u¿ywa funkcji >>void vApplicationIdleHook( void );<< któr¹ trzeba zdefiniowaæ
#define configUSE_TICK_HOOK			0
// Je¿eli=1, to znaczy, ¿e co tykniêcie zegara wywo³uje w obs³udze ISR funkcjê void vApplicationTickHook( void );
#define configCRYSTAL_FREQUENCY     12000000
// External clock input frequency (must be between 10 MHz and 25 MHz
#define configUSE_PLL               1
// 0 = do not use on-chip PLL,  1 = use on-chip PLL
#define configPLL_FACTOR            5
// PLL multiplication factor (1 to 32)
#define configPLL_DIV               2
// PLL division factor (1, 2, 4, or 8)  Musi byæ dobrane tak, ¿e 156 < CPU_CLOCK_Hz *2 * DIV < 320; DIV=2 -> w sam raz
#define configVPBDIV_FACTOR         1
// Peripheral bus speed divider (1, 2, or 4)
#define configUART_BAUD_RATE         ( ( unsigned portLONG ) 38400 )
#define CONSOL_STARTUP_DELAY                       /* Short startup delay in order to remove
                                                      risk for false startbit detection,
                                                      timer #1 will be used in polled mode */
#define CONSOL_STARTUP_DELAY_LENGTH 100            /* 100 us is slightly more than one character at 115200 bps */



//#define configXTAL                  14745600
//#define configPLL                   4
//#define configCOM_BAUD_RATE         ( ( unsigned portLONG ) 115200 )
#if configUSE_PLL == 1
	#define configCPU_CLOCK_HZ		    (configCRYSTAL_FREQUENCY*configPLL_FACTOR)
#else
	#define configCPU_CLOCK_HZ			( configCRYSTAL_FREQUENCY )
#endif
/* Inicjalizacja MAM (Memory Accelerator Module) */
#if configCPU_CLOCK_HZ < 20000000
	#define MAM_TIMING   			1                /* number of CCLK to read from the FLASH */
#elif configCPU_CLOCK_HZ < 40000000
	#define MAM_TIMING   			2                /* number of CCLK to read from the FLASH */
#else
	#define MAM_TIMING   			3                /* number of CCLK to read from the FLASH */
#endif

#define configMAM_SETTING  			2
// 0=disabled, 1=partly enabled (enabled for code prefetch, but not for data), 2=fully enabled
#define IRQ_HANDLER  				0
// 0 = Jump to common IRQ handler,  1 = Load vector directly from VIC, i.e., LDR PC,[PC,#-0xFF0]
/* initialize the exception vector mapping */
#ifndef RUN_FROM_RAM
	#define MAM_MAP      1                  /* 1 = exception vectors are in FLASH at 0x0000 0000,
                                               2 = exception vectors are in SRAM at 0x4000 0000   */
#else
	#define MAM_MAP 	2                       /* When executing from RAM, MAM_MAP should always be 2 */
#endif




#define configTICK_RATE_HZ			( ( portTickType ) 1000 )
// Czêstotliwoœæ przerwañ zegarowych
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 5 )
#define configMINIMAL_STACK_SIZE	( ( unsigned portSHORT ) 128 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 24 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 16 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configQUEUE_REGISTRY_SIZE	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1



#ifndef NULL
#define NULL (0)
#endif

#define FALSE 0
#define TRUE  1

typedef unsigned char tU8;
typedef unsigned short tU16;
typedef unsigned int tU32;
typedef signed char tS8;
typedef signed short tS16;
typedef signed int tS32;
typedef unsigned char tBool;


#endif /* FREERTOS_CONFIG_H */
