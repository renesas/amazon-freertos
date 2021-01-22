/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/******************************************************************************
* File Name    : device.h
* Version      : 1.20
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.27.2017 1.00    First Release
*         : 29.05.2019 1.20    Correspond to internal coding rules
******************************************************************************/

#ifndef DEVICE_H
#define DEVICE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    uint32_t type;
    int32_t  iswp;
} st_sd_info_t;

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

/* Function Name: sd_disk_status */
/**************************************************************************//**
 * @fn            DSTATUS sd_disk_status(BYTE pdrv)
 * @brief         Get device status.
 * @warning       .
 * @param [in]    BYTE pdrv   : Physical drive number.
 * @retval        STA_NODISK  : No card in the drive.
 * @retval        STA_NOINIT  : Device is not initialized.
 * @retval        STA_PROTECT : Card is write protected.
 * @retval        0           : Normal.
 *****************************************************************************/
DSTATUS sd_disk_status(BYTE pdrv);

/* Function Name: sd_disk_initialize */
/**************************************************************************//**
 * @fn            DSTATUS sd_disk_initialize(BYTE pdrv)
 * @brief         Initialize device.
 * @warning       .
 * @param [in]    BYTE pdrv   : Physical drive number.
 * @retval        STA_NODISK  : No card in the drive.
 * @retval        STA_NOINIT  : Device is not initialized.
 * @retval        STA_PROTECT : Card is write protected.
 * @retval        0           : Normal.
 *****************************************************************************/
DSTATUS sd_disk_initialize(BYTE pdrv);

/* Function Name: sd_disk_read */
/**************************************************************************//**
 * @fn            DRESULT sd_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
 * @brief         Read sector(s).
 * @warning       .
 * @param [in]    BYTE pdrv    : Physical drive number.
 * @param [out]   BYTE *buff   : Pointer to the read data buffer.
 * @param [in]    DWORD sector : Start sector number.
 * @param [in]    UINT count   : Number of sectors to read.
 * @retval        RES_OK       : Success.
 * @retval        RES_ERROR    : Error occured during the read operation.
 * @retval        RES_PARERR   : Invalid parameter.
 * @retval        RES_NOTRDY   : Device is not initialized.
 *****************************************************************************/
DRESULT sd_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);

/* Function Name: sd_disk_write */
/**************************************************************************//**
 * @fn            DRESULT sd_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
 * @brief         Write sector(s).
 * @warning       .
 * @param [in]    BYTE pdrv    : Physical drive number.
 * @param [out]   BYTE *buff   : Pointer to the data to be written.
 * @param [in]    DWORD sector : Sector number to write from.
 * @param [in]    UINT count   : Number of sectors to write.
 * @retval        RES_OK       : Success.
 * @retval        RES_ERROR    : Error occured during the read operation.
 * @retval        RES_PARERR   : Invalid parameter.
 * @retval        RES_NOTRDY   : Device is not initialized.
 *****************************************************************************/
DRESULT sd_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);

#endif /* DEVICE_H */
