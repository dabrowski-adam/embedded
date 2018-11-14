/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use the IAP functionality.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <ea_init.h>
#include <consol.h>
#include "startup/config.h"
#include "iap_code/iap.h"

/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void printfStatusMessage(tU32 status);


/*****************************************************************************
 * Local variables
 ****************************************************************************/
static tU8 dataArea[512] = "This is a test buffer area that is 256 bytes long...\n"
                           "256 bytes is the smallest block size that can "
                           "be used to program the flash memory and the largest "
                           "is 4096 bytes.\nAfter having copied the data it is "
                           "typically a good idea to also compare/verify "
                           "that the data has been 'burnt' correctly in the FLASH memory!";

/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Write the IAP status result code in readable form.
 *    
 * Params:
 *    [in] status - the IAP status code to print
 *
 ****************************************************************************/
static void
printfStatusMessage(tU32 status)
{
	switch(status)
	{
		case IAP_CMD_SUCCESS:
		printf("\nIAP status: Success!");
		break;
		case IAP_INVALID_COMMAND:
		printf("\nIAP status: Invalid command error");
		break;
		case IAP_SRC_ADDR_ERROR:
		printf("\nIAP status: Source address error");
		break;
		case IAP_DST_ADDR_ERROR:
		printf("\nIAP status: Destination address error");
		break;
		case IAP_SRC_ADDR_NOT_MAPPED:
		printf("\nIAP status: Source address not mapped error");
		break;
		case IAP_DST_ADDR_NOT_MAPPED:
		printf("\nIAP status: Destination address not mapped error");
		break;
		case IAP_COUNT_ERROR:
		printf("\nIAP status: Count error");
		break;
		case IAP_INVALID_SECTOR:
		printf("\nIAP status: Invalid sector error");
		break;
		case IAP_SECTOR_NOT_BLANK:
		printf("\nIAP status: Sector not blank error");
		break;
		case IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION:
		printf("\nIAP status: Sector not prepared for write operation error");
		break;
		case IAP_COMPARE_ERROR:
		printf("\nIAP status: Compare error");
		break;
		case IAP_BUSY:
		printf("\nIAP status: Busy error");
		break;
		default:
		printf("\nIAP unknown status: %d", status);
		break;
	}
}


/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    The main-function. 
 *
 * Returns:
 *    Always 0, since return value is not used.
 *
 ****************************************************************************/
int
main(void)
{
	tU32 bootVersion;
	
  //initialize printf()-functionality
  eaInit();
  
  //print welcome message
  printf("\n\n\n\n");
  printf("\n*********************************************************");
  printf("\n*");
  printf("\n* (C) 2005 Embedded Artists AB");
  printf("\n*");
  printf("\n* Welcome to this program that demonstrates usage of");
  printf("\n* the IAP functionality.");
  printf("\n*");
  printf("\n*********************************************************\n");
  
  //get part ID
  printf("\nDetected device: ");
  switch(iapReadPartID())
  {
 	case 0xFFF0FF12: printf("LPC2104"); break;
 	case 0xFFF0FF22: printf("LPC2105"); break;
 	case 0xFFF0FF32: printf("LPC2106"); break;
 	case 0x0101FF12: printf("LPC2114"); break;
 	case 0x0201FF12: printf("LPC2119"); break;
 	case 0x0101FF13: printf("LPC2124"); break;
 	case 0x0201FF13: printf("LPC2129"); break;
 	case 0x0002FF01: printf("LPC2131"); break;
 	case 0x0002FF11: printf("LPC2132"); break;
 	case 0x0002FF12: printf("LPC2134"); break;
 	case 0x0002FF23: printf("LPC2136"); break;
 	case 0x0002FF25: printf("LPC2138"); break;
 	case 0x0402FF01: printf("LPC2141"); break;
 	case 0x0402FF11: printf("LPC2142"); break;
 	case 0x0402FF12: printf("LPC2144"); break;
 	case 0x0402FF23: printf("LPC2146"); break;
 	case 0x0402FF25: printf("LPC2148"); break;
 	case 0x0301FF13: printf("LPC2194"); break;
 	case 0x0301FF12: printf("LPC2210 or LPC2290"); break;
 	case 0x0401FF12: printf("LPC2212"); break;
 	case 0x0601FF13: printf("LPC2214"); break;
 	case 0x0401FF13: printf("LPC2292"); break;
 	case 0x0501FF13: printf("LPC2294"); break;
 	default:         printf("Unknown device!"); break;
  }
  
  //get boot code version
  bootVersion = iapReadBootVer();
  printf("\nBoot code version: %d.%d", bootVersion >> 8, bootVersion & 0xff);

  /*
   * A short sequence of commands to program sector 3...
   * Sector 3: 0x00003000 - 0x00003fff on LPC213x  <= code written for, change if needed
   *           0x00006000 - 0x00017fff on LPC210x
   */
  printf("\n\nSelect sector 3...");
  printfStatusMessage(iapSelectSector(3,3));
  
  printf("\n\nErase sector 3...");
  printfStatusMessage(iapEraseSector(3,3));

  printf("\n\nBlank check sector 3...");
  printfStatusMessage(iapBlankCheck(3,3));

  printf("\n\nSelect sector 3...");
  printfStatusMessage(iapSelectSector(3,3));

  printf("\n\nCopy 512 bytes large buffer to FLASH...");
  printfStatusMessage(iapRamToFlash((tU32)0x00003000, (tU32)dataArea, 512));

  printf("\n\nBlank check sector 3 again (should not be blank any more)...");
  printfStatusMessage(iapBlankCheck(3,3));

  printf("\n\nCompare FLASH memory with original buffer...");
  printfStatusMessage(iapCompare((tU32)0x00003000, (tU32)dataArea, 512));

  printf("\n\nPrint string stored in sector 3:\n%s", (tU8*)0x00003000);

  //enter forever loop...
  while(1)
    ;

  return 0;
}
