/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    iap.c
 *
 * Description:
 *    Code that implements the iap interface.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
#include <lpc2xxx.h>
#include "../startup/config.h"
#include "iap.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC

#define m_IAP_ENTRY(a, b) ((void (*)(tU32[], tU32[]))(0x7ffffff1))(a, b)


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void iapCommand(tU32 *pCommand, tU32 *pResult);


/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Performs the actual IAP command, based on input command stored in
 *    'pCommand'. The PLL, MAM, and VIC are disabled during the command
 *    and restored afterwards.
 *    This code must run in THUMB mode, since the IAP interface assumes this!
 *    
 * Params:
 *    [in]    pCommand - pointer to command vector (up to five tU32 words)
 *    [inout] pResult  - pointer to where the result can be stored
 *
 ****************************************************************************/
static void
iapCommand(tU32 *pCommand, tU32 *pResult)
{
	tU32 pllValue;
	tU32 mamValue;
	tU32 vicValue;

  //disable MAM
  mamValue = MAMCR;
  MAMCR    = 0;

	//disable PLL
	pllValue = PLLCFG;
  PLLCON  = 0x00;
  PLLFEED = 0xaa;
  PLLFEED = 0x55;
	
	//disable interrupts
	vicValue    = VICIntEnable;
	VICIntEnClr = 0xffffffff;
	
	//perform IAP command
	m_IAP_ENTRY(pCommand, pResult);

	//enable interrupts again
	VICIntEnClr = vicValue;

	/*
	 * restore PLL
	 */
  PLLCFG = pllValue;

  // enable PLL
  PLLCON  = 0x01;
  PLLFEED = 0xaa;
  PLLFEED = 0x55;

  // wait for PLL lock
  while ((PLLSTAT & (1 << 10)) == 0)
    ;

  // enable & connect PLL
  PLLCON  = 0x03;
  PLLFEED = 0xaa;
  PLLFEED = 0x55;
	
  //restore MAM
  MAMCR = mamValue;
}

/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Select Sector, which prepares the sectors for
 *    write operations.
 *    
 * Params:
 *    [in]    sector1 - start sector
 *    [in]    sector2 - end sector (must be equal or larger to sector1)
 *
 * Returns:
 *    IAP status code (see list in iap.h)
 *
 ****************************************************************************/
tU32
iapSelectSector(tU8 sector1, tU8 sector2)
{
	tU32 parameter[3];
	tU32 result[1];
	
	parameter[0] = IAP_SELSECTOR;
	parameter[1] = sector1;
	parameter[2] = sector2;
	iapCommand(parameter, result);
	
	return result[0];
}


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Copy RAM to FLASH
 *    
 * Params:
 *    [in] destAddress - desitnation address within the FLASH memory
 *                       Must be on 256 byte boundary.
 *    [in] srcAddress  - address to RAM data block (word boundary needed).
 *    [in] length      - length of data block (256, 512, 1024 or 4096)
 *
 * Returns:
 *    IAP status code (see list in iap.h)
 *
 ****************************************************************************/
tU32
iapRamToFlash(tU32 destAddress, tU32 srcAddress, tU32 length)
{
	tU32 parameter[5];
	tU32 result[1];
	
	parameter[0] = IAP_RAMTOFLASH;
	parameter[1] = destAddress;
	parameter[2] = srcAddress;
	parameter[3] = length;
	parameter[4] = CRYSTAL_FREQUENCY / 1000;
	iapCommand(parameter, result);
	
	return result[0];
}


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Erase Sector
 *    
 * Params:
 *    [in] sector1 - start sector
 *    [in] sector2 - end sector (must be equal or larger to sector1)
 *
 * Returns:
 *    IAP status code (see list in iap.h)
 *
 ****************************************************************************/
tU32
iapEraseSector(tU8 sector1, tU8 sector2)
{
	tU32 parameter[4];
	tU32 result[1];
	
	parameter[0] = IAP_ERASESECTOR;
	parameter[1] = sector1;
	parameter[2] = sector2;
	parameter[3] = CRYSTAL_FREQUENCY / 1000;
	iapCommand(parameter, result);
	
	return result[0];
}


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Blank Check Sectors
 *    
 * Params:
 *    [in] sector1 - start sector
 *    [in] sector2 - end sector (must be equal or larger to sector1)
 *
 * Returns:
 *    IAP status code (see list in iap.h)
 *
 ****************************************************************************/
tU32
iapBlankCheck(tU8 sector1, tU8 sector2)
{
	tU32 parameter[3];
	tU32 result[1];
	
	parameter[0] = IAP_BLANKCHK;
	parameter[1] = sector1;
	parameter[2] = sector2;
	iapCommand(parameter, result);
	
	return result[0];
}


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Read Part ID
 *    
 * Returns:
 *    The Part ID, or 0 (if command failed)
 *
 ****************************************************************************/
tU32
iapReadPartID(void)
{
	tU32 parameter[1];
	tU32 result[2];
	
	parameter[0] = IAP_READPARTID;
	iapCommand(parameter, result);
	
	if (result[0] == IAP_CMD_SUCCESS)
	  return result[1];
	else
	  return 0;
}


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Read Boot Version
 *    
 * Returns:
 *    The boot code version, or 0 (if command failed)
 *
 ****************************************************************************/
tU32
iapReadBootVer(void)
{
	tU32 parameter[1];
	tU32 result[2];
	
	parameter[0] = IAP_BOOTCODEID;
	iapCommand(parameter, result);
	
	if (result[0] == IAP_CMD_SUCCESS)
	  return result[1];
	else
	  return 0;
}


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Compare
 *    
 * Params:
 *    [in] destAddress - desitnation address within the FLASH memory
 *                       Must be on word boundary.
 *    [in] srcAddress  - address to RAM data block (word boundary needed).
 *    [in] length      - length of data block (must be a multiple of 4)
 *
 * Returns:
 *    IAP status code (see list in iap.h)
 *
 ****************************************************************************/
tU32
iapCompare(tU32 destAddress, tU32 srcAddress, tU32 length)
{
	tU32 parameter[4];
	tU32 result[1];
	
	parameter[0] = IAP_COMPARE;
	parameter[1] = destAddress;
	parameter[2] = srcAddress;
	parameter[3] = length;
	iapCommand(parameter, result);
	
	return result[0];
}


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Reinvoke ISP
 *    
 ****************************************************************************/
void
iapReinvokeIsp(void)
{
	tU32 parameter[1];
	tU32 result[1];
	
	parameter[0] = IAP_REINVOKE_ISP;
	iapCommand(parameter, result);
}
