/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    iap.h
 *
 * Description:
 *    Code that implements the iap interface.
 *
 *****************************************************************************/
#ifndef _IAP_H_
#define _IAP_H_

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"


/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

//IAP Commands
#define IAP_SELSECTOR    50
#define IAP_RAMTOFLASH   51
#define IAP_ERASESECTOR  52
#define IAP_BLANKCHK     53
#define IAP_READPARTID   54
#define IAP_BOOTCODEID   55
#define IAP_COMPARE      56
#define IAP_REINVOKE_ISP 57

//IAP and ISP Result Status Codes
#define IAP_CMD_SUCCESS                             0
#define IAP_INVALID_COMMAND                         1
#define IAP_SRC_ADDR_ERROR                          2 
#define IAP_DST_ADDR_ERROR                          3
#define IAP_SRC_ADDR_NOT_MAPPED                     4
#define IAP_DST_ADDR_NOT_MAPPED                     5
#define IAP_COUNT_ERROR                             6
#define IAP_INVALID_SECTOR                          7
#define IAP_SECTOR_NOT_BLANK                        8
#define IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 9
#define IAP_COMPARE_ERROR                          10
#define IAP_BUSY                                   11

//ISP Result Status Codes
#define ISP_PARAM_ERROR                  12
#define ISP_ADDR_ERROR                   13
#define ISP_ADDR_NOT_MAPPED              14
#define ISP_CMD_LOCKED                   15
#define ISP_INVALID_CODE                 16
#define ISP_INVALID_BAUD_RATE            17
#define ISP_INVALID_STOP_BIT             18
#define ISP_CODE_READ_PROTECTION_ENABLED 19


/*****************************************************************************
 * Public function prototypes
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
tU32 iapSelectSector(tU8 sector1, tU8 sector2);


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
tU32 iapRamToFlash(tU32 destAddress, tU32 srcAddress, tU32 length);


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
tU32 iapEraseSector(tU8 sector1, tU8 sector2);


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
tU32 iapBlankCheck(tU8 sector1, tU8 sector2);


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Read Part ID
 *    
 * Returns:
 *    The Part ID, or 0 (if command failed)
 *
 ****************************************************************************/
tU32 iapReadPartID(void);


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Read Boot Version
 *    
 * Returns:
 *    The boot code version, or 0 (if command failed)
 *
 ****************************************************************************/
tU32 iapReadBootVer(void);


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
tU32 iapCompare(tU32 destAddress, tU32 srcAddress, tU32 length);


/*****************************************************************************
 *
 * Description:
 *    Performs the IAP command: Reinvoke ISP
 *    
 ****************************************************************************/
void iapReinvokeIsp(void);

#endif
