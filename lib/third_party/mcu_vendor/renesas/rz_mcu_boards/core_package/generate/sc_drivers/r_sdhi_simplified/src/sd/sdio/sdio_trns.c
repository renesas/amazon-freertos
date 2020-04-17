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
* File Name    : sdio_trns.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Data transfer
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 17.09.2019 1.30     Support for SDIO
*         : 06.11.2019 1.40     Replaces the register access with iodefine
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "r_sdif.h"
#include "sd.h"
#include "iodefine.h"

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
 * Function Name: _sdio_software_trans
 * Description  : transfer data to/from card by software
 *                this operations are used multiple command data phase
 *                if dir is SD_TRANS_READ, data is from card to host
 *                if dir is SD_TRANS_WRITE, data is from host to card
 * Arguments    : st_sdhndl_t *p_hndl    : SD handle
 *                uint8_t *buff   : destination/source data buffer
 *                int32_t cnt     : number of transfer bytes
 *                int32_t dir     : transfer direction
 *                uint16_t blklen : block length
 * Return Value : p_hndl->error : SD handle error value
 *                SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : transfer finished, check CMD12 sequence refer to All end
 *****************************************************************************/
int32_t _sdio_software_trans(st_sdhndl_t *p_hndl, uint8_t *buff, int32_t cnt, int32_t dir, uint16_t blklen)
{
    int32_t j;
    int32_t (*func)(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num);

    if (SD_TRANS_READ == dir)
    {
        func = sddev_read_data;
    }
    else
    {
        func = sddev_write_data;
    }

    for (j = cnt; j > 0 ; j--)
    {
        /* ---- wait BWE/BRE interrupt ---- */
        if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
            break;
        }

        /* Cast to an appropriate type */
        if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
        {
            _sd_check_info2_err(p_hndl);
            break;
        }
        if (SD_TRANS_READ == dir)
        {
            /* Cast to an appropriate type */
            _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_RE); /* clear BRE and errors bit */
        }
        else
        {
            /* Cast to an appropriate type */
            _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_WE); /* clear BWE and errors bit */
        }

        /* write/read to/from SD_BUF by 1 sector */
        if ((*func)(p_hndl->sd_port, buff, (uint32_t)(&SDMMC.SD_BUF0.LONGLONG), (int32_t)blklen) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_CPU_IF);
            break;
        }

        /* update buffer */
        buff += blklen;

    }

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_software_trans
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_software_trans2
 * Description  : transfer data to/from card by software
 *                this operations are used multiple command data phase
 *                if dir is SD_TRANS_READ, data is from card to host
 *                if dir is SD_TRANS_WRITE, data is from host to card
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 *                uint8_t *buff : destination/source data buffer
 *                int32_t cnt   : number of transfer bytes
 *                int32_t dir   : transfer direction
 * Return Value : p_hndl->error : SD handle error value
 *                SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : transfer finished, check CMD12 sequence refer to All end
 *****************************************************************************/
int32_t _sdio_software_trans2(st_sdhndl_t *p_hndl, uint8_t *buff, int32_t cnt, int32_t dir)
{
    int32_t (*func)(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num);

    if (SD_TRANS_READ == dir)
    {
        func = sddev_read_data;
    }
    else
    {
        func = sddev_write_data;
    }

    /* ---- wait BWE/BRE interrupt ---- */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return p_hndl->error;
    }

    /* Cast to an appropriate type */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return p_hndl->error;
    }
    if (SD_TRANS_READ == dir)
    {
        /* Cast to an appropriate type */
        _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_RE); /* clear BRE and errors bit */
    }
    else
    {
        /* Cast to an appropriate type */
        _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_WE); /* clear BWE and errors bit */
    }

    /* write/read to/from SD_BUF */
    if ((*func)(p_hndl->sd_port, buff, (uint32_t)(&SDMMC.SD_BUF0.LONGLONG), cnt) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return p_hndl->error;
    }

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_software_trans2
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_dma_trans
 * Description  : transfer data to/from card by DMA
 *                this operations are multiple command data phase
 * Arguments    : st_sdhndl_t *p_hndl      : SD handle
 *                int32_t cnt       : number of transfer bytes
 *                uint16_t blocklen : block length
 * Return Value : p_hndl->error : SD handle error value
 *                SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : transfer finished, check CMD12 sequence refer to All end
 *****************************************************************************/
int32_t _sdio_dma_trans(st_sdhndl_t *p_hndl, int32_t cnt, uint16_t blocklen)
{
    /* ---- check DMA transfer end  --- */
    /* timeout value is depend on transfer size */
    if (sddev_wait_dma_end(p_hndl->sd_port, (cnt * blocklen)) != SD_OK)
    {
        /* Cast to an appropriate type */
        (void)sddev_disable_dma((int32_t)(p_hndl->sd_port));   /* disable DMAC */
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return p_hndl->error;
    }

    /* ---- disable DMAC ---- */
    if (sddev_disable_dma((int32_t)p_hndl->sd_port) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return p_hndl->error;
    }
    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_dma_trans
 *****************************************************************************/

/* End of File */
