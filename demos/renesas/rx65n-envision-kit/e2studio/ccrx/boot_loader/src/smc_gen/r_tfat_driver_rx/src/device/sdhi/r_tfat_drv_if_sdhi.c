/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_tfat_drv_if_sdhi.c
* Description  : TFAT driver Interface for SDHI.
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*              : 01.12.2014 1.00     First Release
*              : 22.06.2015 1.02     Added support MCU RX231.
*              : 01.04.2016 1.03     Updated the xml file.
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <string.h>
#include "r_tfat_drv_if_dev.h"
#include "r_tfat_driver_rx_config.h"

#if (TFAT_SDHI_DRIVE_NUM > 0)
#include "r_tfat_lib.h"              /* TFAT define */

#include "r_sdhi_rx_if.h"
#include "r_sdhi_rx_config.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
*******************************************************************************/

/******************************************************************************
* Function Name : R_tfat_sdhi_disk_initialize
* Description   : This function initializes the memory medium
*               :    for file operations
* Arguments     :  uint8_t  drive        : Physical drive number
* Return value  : Status of the memory medium
******************************************************************************/
DSTATUS R_tfat_sdhi_disk_initialize(uint8_t drive)
{
    return TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_sdhi_disk_read
* Description   : This function reads data from the specified location
*               :    of the memory medium
* Arguments     : uint8_t  drive        : Physical drive number
*               : uint8_t* buffer       : Pointer to the read data buffer
*               : uint32_t sector_number : uint32_t SectorNumber
*               : uint8_t sector_count   : Number of sectors to read
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_sdhi_disk_read (
    uint8_t drive,               /* Physical drive number            */
    uint8_t* buffer,             /* Pointer to the read data buffer  */
    uint32_t sector_number,      /* Start sector number              */
    uint8_t sector_count         /* Number of sectors to read        */
)
{
    sdhi_access_t   sdhi_Access;
    sdhi_status_t   res = SDHI_SUCCESS;
//    uint32_t        reg_buff;

    /* parameter check */
    if ( ( NULL == buffer       )
            || ( 0       == sector_count )
       )
    {
        return TFAT_RES_ERROR;
    }

    sdhi_Access.p_buff = (uint8_t *)buffer;
    sdhi_Access.lbn    = sector_number;
    sdhi_Access.cnt    = sector_count;

    /* TRANS mode : Software */
    res = R_SDHI_Read_Memory_Software_Trans(drive, &sdhi_Access);
    if ( SDHI_SUCCESS != res )
    {
        return TFAT_RES_ERROR;
    }

    return TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_sdhi_disk_write
* Description   : This function writes data to a specified location
*               :    of the memory medium
* Arguments     : uint8_t Drive : Physical drive number
*               : const uint8_t* buffer       : Pointer to the write data
*               : uint32_t       sector_number : Sector number to write
*               : uint8_t        sector_count  : Number of sectors to write
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_sdhi_disk_write (
    uint8_t drive,                /* Physical drive number           */
    const uint8_t* buffer,        /* Pointer to the write data       */
    uint32_t sector_number,       /* Sector number to write          */
    uint8_t sector_count          /* Number of sectors to write      */
)
{

    sdhi_access_t   sdhi_Access;
    sdhi_status_t   res;
//    uint32_t        reg_buff;

    /* parameter check */
    if ( ( NULL == buffer       )
            || ( 0       == sector_count )
       )
    {
        return TFAT_RES_ERROR;
    }

    sdhi_Access.p_buff     = (uint8_t *)buffer;
    sdhi_Access.lbn        = sector_number;
    sdhi_Access.cnt        = sector_count;
    sdhi_Access.write_mode = SDHI_WRITE_WITH_PREERASE;

    /* TRANS mode : Software */
    res = R_SDHI_Write_Memory_Software_Trans(drive, &sdhi_Access);
    if ( SDHI_SUCCESS != res )
    {
        return TFAT_RES_ERROR;
    }

    return TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_sdhi_disk_ioctl
* Description   : This function is used to execute memory operations
*               :    other than read\write
* Arguments     : uint8_t drive   : Drive number
*               : uint8_t command : Control command code
*               : void*   buffer  : Data transfer buffer
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_sdhi_disk_ioctl (
    uint8_t drive,                /* Drive number             */
    uint8_t command,              /* Control command code     */
    void* buffer                  /* Data transfer buffer     */
)
{

    /*  Please put the code for R_tfat_disk_ioctl driver interface
         function over here.  */
    /*  Please refer the application note for details.  */
    return TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_sdhi_disk_status
* Description   : This function is used to retrieve the current status
*               :    of the disk
* Arguments     : uint8_t drive : Physical drive number
* Return value  : Status of the disk
******************************************************************************/
DSTATUS R_tfat_sdhi_disk_status (
    uint8_t drive                  /* Physical drive number    */
)
{

    /*  Please put the code for R_tfat_disk_status driver interface
         function over here.  */
    /*  Please refer the application note for details.  */
    return TFAT_RES_OK;
}
#endif /* (TFAT_SDHI_DRIVE_NUM > 0) */
/******************************************************************************
End  Of File
******************************************************************************/
