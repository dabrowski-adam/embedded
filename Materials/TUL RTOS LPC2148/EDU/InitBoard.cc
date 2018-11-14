#include "FreeRTOSConfig.h"
#include "InitBoard.h"

#define PLL_CONNECT		 0x0003
#define PLL_FEED_BYTE1	   0xaa
#define PLL_FEED_BYTE2	   0x55
#define PLL_LOCK		 0x0400
#define UART0_TX_ENABLE  0x0001
#define UART0_RX_ENABLE	 0x0004
#define IO_P0_14	     0x4000
#define JTAG_PORT	   0x3E0000UL


Initialize_Board board;

Initialize_Board::Initialize_Board()
	{
		/* Configure the RS2332 pins.  All other pins remain at their default of 0. */
		PINSEL0 |= UART0_TX_ENABLE;
		PINSEL0 |= UART0_RX_ENABLE;

		/* Set all GPIO to output other than the P0.14 (BSL), and the JTAG pins.
		   The JTAG pins are left as input as I'm not sure what will happen if the
		   Wiggler is connected after powerup - not that it would be a good idea to
		   do that anyway.
		 */
		IODIR = ~( IO_P0_14 + JTAG_PORT );


		//PLL factor jest na 5 m³odszych bitach, podzielnik na starszych
		PLLCFG = (configPLL_FACTOR-1) | ((configPLL_DIV-1)<<5);
		//PLLCFG = 0x24;



		/* Activate the PLL by turning it on then feeding the correct sequence of bytes. */
		PLLCON  = configUSE_PLL;
		PLLFEED = PLL_FEED_BYTE1;
		PLLFEED = PLL_FEED_BYTE2;

		/* Wait for the PLL to lock... */
		while( !( PLLSTAT & PLL_LOCK ) )
			{};

		/* ...before connecting it using the feed sequence again. */
		PLLCON  = PLL_CONNECT;
		PLLFEED = PLL_FEED_BYTE1;
		PLLFEED = PLL_FEED_BYTE2;


		/* Setup and turn on the MAM.  Three cycle access is used due to the fast
	       PLL used.  It is possible faster overall performance could be obtained by
	       tuning the MAM and PLL settings.
		 */
		MAMTIM = MAM_TIMING;
		MAMCR = MAM_MAP;

		/* Setup the peripheral bus to be the same as the PLL output. */
		VPBDIV = configVPBDIV_FACTOR;
	}




