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
* File Name    : device.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* H/W Platform : RZ/A2M Evaluation Board
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 09.27.2017 1.00    First Release
*         : 29.05.2019 1.20    Correspond to internal coding rules
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "ff.h"         /* Obtains integer types */
#include "diskio.h"     /* Declarations of disk functions */
#include "r_sd_cfg.h"
#include "r_sdif.h"
#include "device.h"
#include "r_sdhi_simplified_drv_sc_cfg.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define SD_PRV_RW_BUFF_SIZE     (1 * 1024) /* for SDIO */

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
#if defined(__GNUC__)
static uint32_t
    s_sd_work[2][SD_SIZE_OF_INIT / sizeof(uint32_t)] __attribute__ ((section ("UNCACHED_BSS"), aligned(8)));
static uint32_t
    s_sd_rw_buff[2][SD_PRV_RW_BUFF_SIZE / sizeof(uint32_t)] __attribute__ ((section ("UNCACHED_BSS"), aligned(8)));
#elif defined(__ICCARM__)
static uint32_t s_sd_work[2][SD_SIZE_OF_INIT / sizeof(uint32_t)] @ "UNCACHED_BSS";
static uint32_t s_sd_rw_buff[2][SD_PRV_RW_BUFF_SIZE / sizeof(uint32_t)] @ "UNCACHED_BSS";
#else
error!!
#endif /* __ICCARM__ */

static st_sd_info_t s_sd_info[2] =
{
    { SD_MEDIA_UNKNOWN, SD_WP_OFF },
    { SD_MEDIA_UNKNOWN, SD_WP_OFF }
};

static const uint32_t s_sd_base_addr[2] =
{
    SD_CFG_IP0_BASE,
    SD_CFG_IP1_BASE
};

/******************************************************************************
* Function Name: sd_disk_status
* Description  : Get device status
* Arguments    : BYTE pdrv   : Physical drive number
* Return Value : STA_NODISK  : No card in the drive
*              : STA_NOINIT  : Device is not initialized
*              : STA_PROTECT : Card is write protected
*              : 0           : Normal
******************************************************************************/
DSTATUS sd_disk_status(BYTE pdrv)
{
    DSTATUS ret;
    int32_t chk;

    ret = 0;

    /* Check if physical drive number is valid. */
    if (pdrv > 1)
    {
        ret = (STA_NODISK | STA_NOINIT);
    }
    else
    {
        /* Check if the card is inserted. *//* Cast to an appropriate type */
        chk = sd_check_media((int32_t)pdrv);

        if (SD_OK != chk)
        {
            ret = (STA_NODISK | STA_NOINIT);
        }
        /* Check if the card type information is already retrived. */
        else if (SD_MEDIA_UNKNOWN == s_sd_info[pdrv].type)
        {
            ret = STA_NOINIT;
        }
        /* Check if the card is write protected. */
        else if (SD_WP_OFF != s_sd_info[pdrv].iswp)
        {
            ret = STA_PROTECT;
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}
/*******************************************************************************
 End of function sd_disk_status
 ******************************************************************************/

/******************************************************************************
* Function Name: sd_disk_initialize
* Description  : Initialize device
* Arguments    : BYTE pdrv   : Physical drive number
* Return Value : STA_NODISK  : No card in the drive
*              : STA_NOINIT  : Device is not initialized
*              : STA_PROTECT : Card is write protected
*              : 0           : Normal
******************************************************************************/
DSTATUS sd_disk_initialize(BYTE pdrv)
{
    DSTATUS     ret;
    int32_t     chk;
    uint16_t    type;
    int32_t     sc_port;

    ret = (STA_NODISK | STA_NOINIT);

    /* Check if physical drive number is valid. */
    if (pdrv > 1)
    {
        return ret;
    }

    /* Initialize card information. */
    s_sd_info[pdrv].type = SD_MEDIA_UNKNOWN;
    s_sd_info[pdrv].iswp = SD_WP_OFF;

    /* Initialize SD driver. *//* Cast to an appropriate type */
    chk = sd_init((int32_t)pdrv, s_sd_base_addr[pdrv], &s_sd_work[pdrv][0], SD_CD_SOCKET);
    if (SD_OK != chk)
    {
        return ret;
    }

    chk = sddev_get_sc_table_config_ch(pdrv, &sc_port);
    if (SD_OK == chk)
    {
        if (SD_CB_USED == SDHI_SC_TABLE[sc_port].config.cb)
        {
            /* Set the card detect interrupt. *//* Cast to an appropriate type */
            chk = sd_cd_int((int32_t)pdrv, SD_CD_INT_ENABLE, SDHI_SC_TABLE[sc_port].config.cb_func);
        }
        else
        {
            /* Set the card detect interrupt. *//* Cast to an appropriate type */
            chk = sd_cd_int((int32_t)pdrv, SD_CD_INT_ENABLE, NULL);
        }
    }
    if (SD_OK != chk)
    {
        return ret;
    }

    /* Check if the card is inserted. *//* Cast to an appropriate type */
    chk = sd_check_media((int32_t)pdrv);
    if (SD_OK != chk)
    {
        return ret;
    }
    else
    {
        /* The card is inserted. *//* Cast to an appropriate type */
        ret &= (DSTATUS)(~STA_NODISK);

        /* Register callback function. *//* Cast to an appropriate type */
        chk = sd_set_intcallback((int32_t)pdrv, &SD_status_callback_function);
        if (SD_OK != chk)
        {
            return ret;
        }

        /* Cast to an appropriate type *//* Cast to an appropriate type */
        chk = sd_set_dma_intcallback((int32_t)pdrv, &SD_dma_end_callback_function);
        if (SD_OK != chk)
        {
            return ret;
        }

        /* Initialize SD driver work buffer. *//* Cast to an appropriate type */
        chk = sd_set_buffer((int32_t)pdrv, &s_sd_rw_buff[pdrv][0], SD_PRV_RW_BUFF_SIZE);
        if (SD_OK != chk)
        {
            return ret;
        }

        /* Mount SD card. *//* Cast to an appropriate type */
        chk = sd_mount((int32_t)pdrv, SD_CFG_DRIVER_MODE, SD_VOLT_3_3);
        if (SD_OK != chk)
        {
            return ret;
        }

        /* Get the card type. *//* Cast to an appropriate type */
        chk = sd_get_type((int32_t)pdrv, &type, NULL, NULL);
        if (SD_OK != chk)
        {
            return ret;
        }
        s_sd_info[pdrv].type = type;

        /* Get write protected information. *//* Cast to an appropriate type */
        chk = sd_iswp((int32_t)pdrv);
        if (SD_ERR == chk)
        {
            return ret;
        }
        s_sd_info[pdrv].iswp = chk;

        /* Initialization finish. *//* Cast to an appropriate type */
        ret &= (DSTATUS)(~STA_NOINIT);

        /* Check if the card is write protected. */
        if (SD_WP_OFF != s_sd_info[pdrv].iswp)
        {
            ret |= STA_PROTECT;
        }
    }
    return ret;
}
/*******************************************************************************
 End of function sd_disk_initialize
 ******************************************************************************/

/******************************************************************************
* Function Name: sd_disk_read
* Description  : Read sector(s)
* Arguments    : BYTE pdrv    : Physical drive number
*              : BYTE *buff   : Pointer to the read data buffer
*              : DWORD sector : Start sector number
*              : UINT count   : Number of sectors to read
* Return Value : RES_OK       : Success
*              : RES_ERROR    : Error occured during the read operation
*              : RES_PARERR   : Invalid parameter
*              : RES_NOTRDY   : Device is not initialized
******************************************************************************/
DRESULT sd_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    DRESULT ret;
    int32_t chk;

    ret = RES_PARERR;

    /* Check if physical drive number is valid. */
    if (pdrv > 1)
    {
        return ret;
    }

    /* Read sector data from the card. *//* Cast to an appropriate type */
    chk = sd_read_sect(pdrv, buff, sector, (int32_t)count);

    switch (chk)
    {
        case SD_OK:
            ret = RES_OK;
            break;

        default:
            ret = RES_ERROR;
            break;
    }
    return ret;
}
/*******************************************************************************
 End of function sd_disk_read
 ******************************************************************************/

/******************************************************************************
* Function Name: sd_disk_write
* Description  : Write sector(s)
* Arguments    : BYTE pdrv    : Physical drive number
*              : BYTE *buff   : Pointer to the data to be written
*              : DWORD sector : Sector number to write from
*              : UINT count   : Number of sectors to write
* Return Value : RES_OK       : Success
*              : RES_ERROR    : Error occured during the read operation
*              : RES_PARERR   : Invalid parameter
*              : RES_NOTRDY   : Device is not initialized
******************************************************************************/
DRESULT sd_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT ret;
    int32_t chk;

    ret = RES_PARERR;

    /* Check if physical drive number is valid. */
    if (pdrv > 1)
    {
        return ret;
    }

    /* Write sector data to the card. *//* Cast to an appropriate type */
    chk = sd_write_sect(pdrv, (uint8_t *)buff, sector, (int32_t)count, (int32_t)SD_WRITE_OVERWRITE);

    switch (chk)
    {
        case SD_OK:
            ret = RES_OK;
            break;

        default:
            ret = RES_ERROR;
            break;
    }
    return ret;
}
/*******************************************************************************
 End of function sd_disk_write
 ******************************************************************************/

/******************************************************************************
* Function Name: get_fattime
* Description  : Get current time
* Arguments    : none
* Return Value : 0x00000000 : Invalid
******************************************************************************/
DWORD get_fattime(void)
{
    return 0x00000000;
}
/*******************************************************************************
 End of function get_fattime
 ******************************************************************************/

/* End of File */
