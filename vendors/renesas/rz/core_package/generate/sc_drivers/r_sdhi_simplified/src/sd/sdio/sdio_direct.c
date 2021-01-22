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
/*******************************************************************************
* System Name  : SDHI Driver
* File Name    : sdio_direct.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : io direct read and write
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 17.09.2019 1.30     Support for SDIO
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "r_sdif.h"
#include "sd.h"

#ifdef __CC_ARM
#pragma arm section code = "CODE_SDHI"
#pragma arm section rodata = "CONST_SDHI"
#pragma arm section rwdata = "DATA_SDHI"
#pragma arm section zidata = "BSS_SDHI"
#endif

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
 * Function Name: sdio_read_direct
 * Description  : direct read io register from specified address (=adr)
 *                using CMD52
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *                uint8_t *buff   : data buffer
 *                uint32_t func   : access function number
 *                uint32_t adr    : access register address
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_read_direct(int32_t sd_port, uint8_t *buff, uint32_t func, uint32_t adr)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->error = SD_OK;

    /* ---- check card is mounted ---- */
    if (SD_MOUNT_UNLOCKED_CARD != p_hndl->mount)
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error; /* not mounted yet */
    }

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return p_hndl->error;
    }

    /* ==== direct read and write io register ==== */
    _sdio_direct(p_hndl, buff, func, adr, 0, 0);

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function sdio_read_direct
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_write_direct
 * Description  : direct write io register from specified address (=adr)
 *                using CMD52
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *                uint8_t *buff     : data buffer
 *                uint32_t func     : access function number
 *                uint32_t adr      : access register address
 *                uint32_t raw_flag : write mode
 *                  SD_IO_SIMPLE_WRITE : just write
 *                  SD_IO_VERIFY_WRITE : read after write
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_write_direct(int32_t sd_port, uint8_t *buff, uint32_t func, uint32_t adr,
                            uint32_t raw_flag)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->error = SD_OK;

    /* ---- check card is mounted ---- */
    if (SD_MOUNT_UNLOCKED_CARD != p_hndl->mount)
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error; /* not mounted yet */
    }

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return p_hndl->error;
    }

    /* ==== direct read and write io register ==== */
    _sdio_direct(p_hndl, buff, func, adr, 1, raw_flag);

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function sdio_write_direct
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_direct
 * Description  : direct read or write io register from specified address
 *                (=adr) using CMD52
 * Arguments    : st_sdhndl_t *p_hndl      : SD handle
 *                uint8_t *buff     : data buffer
 *                uint32_t func     : access function number
 *                uint32_t adr      : access register address
 *                uint32_t rw_flag  : read (= 0) or write (= 1)
 *                uint32_t raw_flag : write mode
 *                  SD_IO_SIMPLE_WRITE : just write
 *                  SD_IO_VERIFY_WRITE : read after write
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sdio_direct(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t func,
                        uint32_t adr, uint32_t rw_flag, uint32_t raw_flag)
{
    uint32_t arg = 0;
    uint16_t cmd;

    /* ---- check media type ---- */
    if ((p_hndl->media_type & SD_MEDIA_IO) == 0)
    {
        _sd_set_err(p_hndl, SD_ERR_CARD_TYPE);
        return SD_ERR_CARD_TYPE;
    }

    /* check read register address and function number */
    if ((func > 7) || (adr > 0x1ffff))
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error;
    }

    /* Cast to an appropriate type */
    arg = ((rw_flag << 31) | (func << 28) | (raw_flag << 27) | (adr << 9) | (uint32_t) * buff);

    if (1 == rw_flag)
    {
        cmd = CMD52_W;      /* write */
    }
    else
    {
        cmd = CMD52_R;      /* read */
    }

    /* issue CMD */
    if (_sd_send_iocmd(p_hndl, cmd, arg) != SD_OK)
    {
        return p_hndl->error;
    }

    /* Set Read data from R5 Respose */
    *buff = (uint8_t)(p_hndl->resp_status & 0x00ff);

    /* enable resp end and illegal access interrupts */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_direct
 *****************************************************************************/

/* End of File */
