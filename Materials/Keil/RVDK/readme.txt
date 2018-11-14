/*****************************************************************************
 *   readme.txt:  Description of the LPC213x/4x Sample Software
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.02.01  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/

/*****************************************************************************/
Software that is described herein is for illustrative purposes only which 
provides customers with programming information regarding the products.
This software is supplied "AS IS" without any warranties. Philips Semiconductors 
assumes no responsibility or liability for the use of the software, conveys no 
license or title under any patent, copyright, or mask work right to the 
product. Philips Semiconductors reserves the right to make changes in the 
software without notification. Philips Semiconductors also make no representation 
or warranty that such application will be suitable for the specified use without 
further testing or modification.
/*****************************************************************************/


The Description of the sample software
===================

This example demonstrates the use of build-in peripherals on the Philips
LPC213x/214x family MCUs.

The sample software includes, common library, peripheral APIs, and test modules
for the APIs. The common library include startup file, standard definition and
header files, processor specific setup module, generic interrupt related APIs, 
timer routine, and scatter loading file. The peripheral directories include, 
GPIO, PWM, Real-time clock, timer, SPI, SSP, I2C, Watchdog timer, UART, external 
interrupt, ADC, DAC, etc.
  
The development environment is ARM's RealView Development Kits for Philips (RVDK) 
with RVI-ME ICE. The target board is Keil's, now an ARM Company, MCB214x with a
Philips's LPC214x MCU on it.

Some external components, such as I2C temperature, SPI serial EEPROM, will be 
required to add on the board in order to complete the test below.  


The directory tree and content of the sample software
===================

common
    -- inc
	-- lpc214x.h		Definition file of all the registers of LPC213x/4x
	-- irq.h		Interrupt related definitions and API prototyping
	-- type.h		Type definition
	-- target.h		Target specific definition for Keil's MCB214x board
	-- timer.h		Definition and header file for timer module
	-- mem_b.scf		Scatter loader for code running in SRAM
	-- mem_c.scf		Scatter loader for code running in flash
    -- src
	-- startup.s		Code start up entry
	-- heap.s		Heap entry definition 
	-- stack.s		Stack entry definition
	-- swi_handler.s	SWI handler 
	-- irq.c		Generic interrupt handler related APIs 
	-- target.c		Target specific modules
	-- timer.c		Timer related APIs

GPIO
    -- fio.h			GPIO and fast I/O header
    -- fio.c			GPIO and fast I/O APIs
    -- fiotest.c		GPIO and Fast I/O test module
    -- gpio.prj			RVDK project file


EXTINT
    -- extint.h			External interrupt header
    -- extint.c			External interrupt APIs
    -- einttest.c		External interrupt test module
    -- extint.prj		RVDK project file

Timer
    -- tmrtest.c		Timer test module, note: API modules
				are in the COMMON directory and 
				shared and used by some peripheral testing.
    -- timer.prj		RVDK project file

PWM
    -- pwmc.h			PWM header
    -- pwm.c			PWM APIs
    -- pwmtest.c		PWM test module
    -- pwm.prj			RVDK project file

SPI
    -- spi.h			SPI header
    -- spi.c			SPI APIs
    -- spitest.c		SPI test module
    -- spi.prj			RVDK project file

SSP
    -- ssp.h			SSP(SPI1) header
    -- ssp.c			SSP(SPI1) APIs
    -- ssptest.c		SSP(SPI1) test module
    -- ssp.prj			RVDK project file

UART
    -- uart.h			UART header
    -- uart.c			UART APIs
    -- uarttest.c		UART test module
    -- uart.prj			RVDK project file

RTC
    -- rtc.h			Real-time clock(RTC) header
    -- rtc.c			Real-time clock(RTC) APIs
    -- rtctest.c		Real-time clock(RTC) test module
    -- rtc.prj			RVDK project file


I2C
    -- i2c.h			I2C header
    -- i2c.c			I2C APIs
    -- i2ctest.c		I2C test module
    -- i2c.prj			RVDK project file

WDT
    -- wdt.h			Watchdog timer header
    -- wdt.c			Watchdog timer APIs
    -- wdttest.c		Watchdog timer test module
    -- wdt.prj			RVDK project file

ADC
    -- adc.h			ADC header
    -- adc.c			ADC APIs
    -- adctest.c		ADC test module
    -- adc.prj			RVDK project file

DAC
    -- dac.h			DAC header
    -- dac.c			DAC APIs
    -- dactest.c		DAC test module
    -- dac.prj			RVDK project file

