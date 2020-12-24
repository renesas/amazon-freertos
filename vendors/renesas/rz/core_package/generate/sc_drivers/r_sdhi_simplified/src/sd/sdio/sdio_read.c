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
* File Name    : sdio_read.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : io read
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 17.09.2019 1.30     Support for SDIO
*         : 06.11.2019 1.40     Replaces the register access with iodefine
*         : 22.11.2019 1.41     Support of variable-length
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
static int32_t _sdio_read_error(st_sdhndl_t *p_hndl, int32_t mode);
static int32_t _sdio_read_byte_error(st_sdhndl_t *p_hndl, int32_t mode);

/******************************************************************************
 * Function Name: sdio_read
 * Description  : read io register from specified address (=adr) by the
 *                number of bytes or blocks (=cnt)
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : int32_t sd_port  : channel no (0 or 1)
 *                uint8_t *buff    : read data buffer
 *                uint32_t func    : access function number
 *                uint32_t adr     : read register address
 *                int32_t cnt      : number of read registers (byte)
 *                uint32_t op_code : operation code
 *                  SD_IO_FIXED_ADDR     : R/W fixed address
 *                  SD_IO_INCREMENT_ADDR : R/W increment address
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_read(int32_t sd_port, uint8_t *buff, uint32_t func, uint32_t adr,
                    int32_t cnt, uint32_t op_code)
{
    st_sdhndl_t   *p_hndl;
    uint16_t blocklen;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    /* check read register address and function number */
    if ((func > 7) || (adr > 0x1ffff))
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

    /* ---- is io_abort compulsory? ---- */
    if (p_hndl->io_abort[func])
    {
        p_hndl->io_abort[func] = 0;
        _sd_set_err(p_hndl, SD_ERR_STOP);
        return p_hndl->error; /* write protect error */
    }

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* ==== read io register space ==== */
    if ((op_code & SD_IO_FORCE_BYTE ) == 0)
    {
        if (0 == p_hndl->io_len[func])              /* not yet */
        {
            if (sdio_get_blocklen(sd_port, &blocklen, func) != SD_OK)
            {
                return p_hndl->error;
            }
        }
        blocklen = p_hndl->io_len[func];              /* judge by io_len */
        if ( (0 == blocklen) || (0xffff == blocklen) )
        {
            _sd_set_err(p_hndl, SD_ERR_ILL_FUNC);
            return p_hndl->error;
        }

        /* ---- supply clock (data-transfer ratio) ---- */
        if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
        {
            return p_hndl->error;
        }

        _sdio_read(p_hndl, buff, func, adr, cnt, op_code, blocklen);
    }
    else
    {
        /* ---- supply clock (data-transfer ratio) ---- */
        if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
        {
            return p_hndl->error;
        }

        _sdio_read_byte(p_hndl, buff, func, adr, cnt, (op_code & SD_IO_INCREMENT_ADDR));
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function sdio_read
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_read
 * Description  : read io register from specified address (=adr) by the
 *                number of bytes or blocks (=cnt)
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : st_sdhndl_t *p_hndl       : SD handle
 *                uint8_t *buff      : read data buffer
 *                uint32_t func      : access function number
 *                uint32_t adr       : read register address
 *                int32_t cnt        : number of read registers (byte)
 *                uint32_t op_code   : operation code
 *                  SD_IO_FIXED_ADDR     : R/W fixed address
 *                  SD_IO_INCREMENT_ADDR : R/W increment address
 *                uint16_t blocklen  : block length
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sdio_read(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t func,
                    uint32_t adr, int32_t cnt, uint32_t op_code, uint16_t blocklen)
{
    int32_t  i;
    int32_t  sec;
    int32_t  rem;
    int32_t  mode = 0;
    uint32_t arg = 0;
    uint64_t reg;
    uint16_t info1_back;
    int32_t  ret;
    uint16_t cmd = 0;

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

    if (cnt <= 0)
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error;
    }

    if (0 == blocklen)
    {
        _sd_set_err(p_hndl, SD_ERR_ILL_FUNC);
        return p_hndl->error;
    }

    /* clear io_abort */
    reg  = SDMMC.SDIO_MODE.LONGLONG;
    
    /* Cast to an appropriate type */
    reg &= (~SDIO_MODE_IOABT);

    /* Cast to an appropriate type */
    SDMMC.SDIO_MODE.LONGLONG = reg;

    sec = (cnt / blocklen);     /* set sector count */
    rem = (cnt % blocklen);     /* set remaining bytes */

    if (sec)    /* more than io block size (=blocklen bytes) */
    {
        /* ==== multiple transfer by io block length ==== */
        /* ---- applied to CMD53 (IO_READ_EXTENDED_BLOCK) ---- */

        /* if DMA transfer, buffer boundary is octlet unit */
        if ((p_hndl->trans_mode & SD_MODE_DMA) && (((uint32_t)buff & 0x07u) == 0))
        {
            mode = SD_MODE_DMA; /* set DMA mode */
        }

        /* transfer size is blocklen bytes */
        SDMMC.SD_SIZE.LONGLONG = (uint64_t)blocklen;
        
        /* loop during remaining bytes are more than io block length */
        for (i = sec; i > 0; i -= TRANS_BLOCKS)
        {

            /* ---- is card existed? ---- */
            if (_sd_check_media(p_hndl) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
                return _sdio_read_error(p_hndl, mode);
            }

            /* enable SD_SECCNT */
            SDMMC.SD_STOP.LONGLONG = (uint64_t)0x0100;
            
            /* set transfer sector numbers to SD_SECCNT */
            sec = i - TRANS_BLOCKS;
            if (sec < 0)    /* remaining sectors are less than TRANS_BLOCKS */
            {
                sec = i;
            }
            else
            {
                sec = TRANS_BLOCKS;
            }

            /* Cast to an appropriate type */
            SDMMC.SD_SECCNT.LONGLONG = (uint64_t)sec;
            
            /* set argument */
            arg = ((func << 28) | 0x08000000u | (op_code << 26) | (adr << 9) | (uint32_t)sec);

            if ( (p_hndl->io_reg[0][0x08] & 0x10) == 0 )
            {
                /* Not Support Block Gap Interrupt */
                /* Disable SDIO interrupt */
                reg  = SDMMC.SDIO_MODE.LONGLONG;
                
                /* Cast to an appropriate type */
                reg &= (~SDIO_MODE_IOMOD);

                /* Cast to an appropriate type */
                SDMMC.SDIO_MODE.LONGLONG = reg;
            }

            /* ---- enable RespEnd and ILA ---- */
            _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, 0);

            if (sec > 1)
            {
                cmd = CMD53_R_BLOCK;
            }
            else
            {
                cmd = CMD53_R_BYTE;
            }
            /* issue CMD53 (IO_READ_EXTENDED_BLOCK) */
            if (_sd_send_iocmd(p_hndl, cmd, arg) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR);
                return _sdio_read_error(p_hndl, mode);
            }

            /* ---- disable RespEnd and ILA ---- */
            _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

            if (0 == mode)  /* ==== PIO ==== */
            {
                /* enable All end, BRE and errors */
                _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

                /* software data transfer */
                ret = _sdio_software_trans(p_hndl, buff, sec, SD_TRANS_READ, blocklen);
            }
            else    /* ==== DMA ==== */
            {
                /* disable card ins&rem interrupt for FIFO */
                info1_back = (uint16_t)(p_hndl->int_info1_mask & SD_INFO1_MASK_DET_CD);

                /* Cast to an appropriate type */
                _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DET_CD, 0);

                /* enable All end and errors */
                _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR);

                /* ---- initialize DMAC ---- */
                if (sddev_init_dma(p_hndl->sd_port, (uint32_t)buff, SD_TRANS_READ) != SD_OK)
                {
                    _sd_set_err(p_hndl, SD_ERR_CPU_IF);
                    return _sdio_read_error(p_hndl, mode);
                }

                /* DMA data transfer */
                ret = _sdio_dma_trans(p_hndl, sec, blocklen);

                _sd_set_int_mask(p_hndl, info1_back, 0);
            }

            if (SD_OK != ret)
            {
                return _sdio_read_error(p_hndl, mode);
            }

            /* ---- wait All end interrupt ---- */
            if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
                return _sdio_read_error(p_hndl, mode);
            }

            /* ---- check errors ---- */
            if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
            {
                _sd_check_info2_err(p_hndl);
                return _sdio_read_error(p_hndl, mode);
            }

            /* clear All end bit */
            _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);

            /* disable All end, BRE and errors */
            _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

            if (SD_IO_INCREMENT_ADDR == op_code)
            {
                adr += (sec * blocklen);
            }
            buff += (sec * blocklen);

            if ( (p_hndl->io_reg[0][0x08] & 0x10) == 0 )
            {
                /* Not Support Block Gap Interrupt */
                /* Enable SDIO interrupt */
                reg  = SDMMC.SDIO_MODE.LONGLONG;
                
                /* Cast to an appropriate type */
                reg |= SDIO_MODE_IOMOD;

                /* Cast to an appropriate type */
                SDMMC.SDIO_MODE.LONGLONG = reg;
            }

            /* ---- is io_abort compulsory? ---- */
            if (p_hndl->io_abort[func])
            {
                /* data transfer stop (issue CMD52) */
                reg  = SDMMC.SDIO_MODE.LONGLONG;
                
                /* Cast to an appropriate type */
                reg |= SDIO_MODE_IOABT;

                /* Cast to an appropriate type */
                SDMMC.SDIO_MODE.LONGLONG = reg;
                i = 0;  /* set zero to break loop */
            }
        }
    }

    /* ---- is io_abort compulsory? ---- */
    if (p_hndl->io_abort[func])
    {
        p_hndl->io_abort[func] = 0;
        _sd_set_err(p_hndl, SD_ERR_STOP);

        return p_hndl->error;
    }

    if (rem)    /* remaining bytes are less than io block size or not support multi block transfer */
    {
        /* ==== applied to CMD53 (IO_READ_EXTENDED_BYTE) ==== */
        mode = 0;   /* just support PIO */

        /* ---- disable SD_SECCNT ---- */
        SDMMC.SD_STOP.LONGLONG = 0x0000;
        
        /* ---- set transfer bytes ---- */
        SDMMC.SD_SIZE.LONGLONG = (uint64_t)rem;
        
        /* set argument */
        arg = ((func << 28) | (op_code << 26) | (adr << 9) | (uint32_t)rem);

        /* ---- enable RespEnd and ILA ---- */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, 0);

        /* issue CMD53 (IO_READ_EXTENDED_BYTE) */
        if (_sd_send_iocmd(p_hndl, CMD53_R_BYTE, arg) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR);
            return _sdio_read_error(p_hndl, mode);
        }

        /* ---- disable RespEnd and ILA ---- */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

        /* enable All end, BRE and errors */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

        /* software data transfer */
        ret = _sdio_software_trans2(p_hndl, buff, rem, SD_TRANS_READ);
        if (SD_OK != ret)
        {
            return _sdio_read_error(p_hndl, mode);
        }

        /* wait All end interrupt */
        if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
            return _sdio_read_error(p_hndl, mode);
        }

        /* ---- check errors ---- */
        if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
        {
            _sd_check_info2_err(p_hndl);
            return _sdio_read_error(p_hndl, mode);
        }

        /* Cast to an appropriate type */
        _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);  /* clear All end bit */

        /* disable all interrupts */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);
    }

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_read
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_read_error
 * Description  : read io register space error
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 *                int32_t mode  : data transfer mode
 *                  SD_MODE_SW  : software
 *                  SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_read_error(st_sdhndl_t *p_hndl, int32_t mode)
{
    uint64_t reg;
    int32_t  tmp_error;
    uint64_t sd_option;
    uint64_t sd_clk_ctrl;

    if (SD_MODE_DMA == mode)
    {
        /* Cast to an appropriate type */
        (void)sddev_disable_dma((int32_t)(p_hndl->sd_port));   /* disable DMA */

        /* reset DMAC */
        tmp_error = sddev_reset_dma((int32_t)(p_hndl->sd_port));
        if (SD_OK != tmp_error)
        {
            ;   /* do nothing */
        }
    }

    if ( (p_hndl->io_reg[0][0x08] & 0x10) == 0 )
    {
        /* Not Support Block Gap Interrupt */
        /* Enable SDIO interrupt */
        reg  = SDMMC.SDIO_MODE.LONGLONG;
        
        /* Cast to an appropriate type */
        reg |= SDIO_MODE_IOMOD;

        /* Cast to an appropriate type */
        SDMMC.SDIO_MODE.LONGLONG =reg;
    }

    tmp_error = p_hndl->error;

    /* ---- clear error bits ---- */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- disable all interrupts ---- */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* Cast to an appropriate type */
    if ((SDMMC.SD_INFO2.LONGLONG & SD_INFO2_MASK_CBSY) == SD_INFO2_MASK_CBSY)
    {
        sddev_loc_cpu(p_hndl->sd_port);

        /* Cast to an appropriate type */
        sd_option   = SDMMC.SD_OPTION.LONGLONG;
        
        /* Cast to an appropriate type */
        sd_clk_ctrl = SDMMC.SD_CLK_CTRL.LONGLONG;
        
        /* Cast to an appropriate type */
        SDMMC.SOFT_RST.LONGLONG = SOFT_RST_SDRST_RESET;
        
        /* Cast to an appropriate type */
        SDMMC.SOFT_RST.LONGLONG = SOFT_RST_SDRST_RELEASED;

        /* Cast to an appropriate type */
        SDMMC.SD_OPTION.LONGLONG = sd_option;

        /* Cast to an appropriate type */
        SDMMC.SD_CLK_CTRL.LONGLONG = sd_clk_ctrl;
        sddev_unl_cpu(p_hndl->sd_port);
    }

    p_hndl->error = tmp_error;

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_read_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_read_byte
 * Description  : read io register from specified address (=adr) by the
 *                number of bytes or blocks (=cnt)
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : st_sdhndl_t *p_hndl       : SD handle
 *                uint8_t *buff      : read data buffer
 *                uint32_t func      : access function number
 *                uint32_t adr       : read register address
 *                int32_t cnt        : number of read registers (byte)
 *                uint32_t op_code   : operation code
 *                  SD_IO_FIXED_ADDR     : R/W fixed address
 *                  SD_IO_INCREMENT_ADDR : R/W increment address
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sdio_read_byte(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t func,
                        uint32_t adr, int32_t cnt, uint32_t op_code)
{
    int32_t  i;
    int32_t  sec;
    int32_t  rem;
    int32_t  mode = 0;
    uint32_t arg = 0;
    uint64_t reg;
    uint16_t info1_back;
    int32_t  ret;

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

    if (cnt <= 0)
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error;
    }

    /* clear io_abort */
    reg  = SDMMC.SDIO_MODE.LONGLONG;
    
    /* Cast to an appropriate type */
    reg &= (~SDIO_MODE_IOABT);

    /* Cast to an appropriate type */
    SDMMC.SDIO_MODE.LONGLONG = reg;

    sec = cnt / 512; /* set sector count */
    rem = cnt % 512; /* set remaining bytes */
    
    if (sec)    /* more than io block size (=512 bytes) */
    {
        /* ==== multiple transfer by io block length ==== */
        /* ---- applied to CMD53 (IO_READ_EXTENDED_BLOCK) ---- */

        /* if DMA transfer, buffer boundary is octlet unit */
        if ((p_hndl->trans_mode & SD_MODE_DMA) && (((uint32_t)buff & 0x07u) == 0))
        {
            mode = SD_MODE_DMA; /* set DMA mode */
        }

        /* transfer size is fixed (512 bytes) */
        SDMMC.SD_SIZE.LONGLONG = (uint64_t)512;
        
        /* loop during remaining bytes are more than io block length */
        for (i = sec; i > 0; i--)
        {

            /* ---- is card existed? ---- */
            if (_sd_check_media(p_hndl) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
                return _sdio_read_byte_error(p_hndl, mode);
            }

            /* enable SD_SECCNT */
            SDMMC.SD_STOP.LONGLONG = 0x0000;
            
            /* set argument */
            arg = ((func << 28) | 0x00000000u | (op_code << 26) | (adr << 9) | (uint32_t)0);

            /* ---- enable RespEnd and ILA ---- */
            _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, 0);

            /* issue CMD53 (IO_READ_EXTENDED_BLOCK) */
            if (_sd_send_iocmd(p_hndl, CMD53_R_BYTE, arg) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR);
                return _sdio_read_byte_error(p_hndl, mode);
            }

            /* ---- disable RespEnd and ILA ---- */
            _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

            if (0 == mode)  /* ==== PIO ==== */
            {
                /* enable All end, BRE and errors */
                _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

                /* software data transfer */
                ret = _sdio_software_trans(p_hndl, buff, 1, SD_TRANS_READ, 512);
            }
            else    /* ==== DMA ==== */
            {
                /* disable card ins&rem interrupt for FIFO */
                info1_back = (uint16_t)(p_hndl->int_info1_mask & SD_INFO1_MASK_DET_CD);

                /* enable All end and errors */
                _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR);

                /* ---- initialize DMAC ---- */
                if (sddev_init_dma(p_hndl->sd_port, (uint32_t)buff, SD_TRANS_READ) != SD_OK)
                {
                    _sd_set_err(p_hndl, SD_ERR_CPU_IF);
                    return _sdio_read_byte_error(p_hndl, mode);
                }

                /* DMA data transfer */
                ret = _sdio_dma_trans(p_hndl, 1, 512);

                _sd_set_int_mask(p_hndl, info1_back, 0);
            }

            if (SD_OK != ret)
            {
                return _sdio_read_byte_error(p_hndl, mode);
            }

            /* ---- wait All end interrupt ---- */
            if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
                return _sdio_read_byte_error(p_hndl, mode);
            }

            /* ---- check errors ---- */
            if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
            {
                _sd_check_info2_err(p_hndl);
                return _sdio_read_byte_error(p_hndl, mode);
            }

            /* clear All end bit */
            _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);

            /* disable All end, BRE and errors */
            _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

            if (SD_IO_INCREMENT_ADDR == op_code)
            {
                adr += 512;
            }
            buff += 512;

            /* ---- is io_abort compulsory? ---- */
            if (p_hndl->io_abort[func])
            {
                i = 0;  /* set zero to break loop */
            }
        }
    }

    /* ---- is io_abort compulsory? ---- */
    if (p_hndl->io_abort[func])
    {
        p_hndl->io_abort[func] = 0;
        _sd_set_err(p_hndl, SD_ERR_STOP);

        return p_hndl->error;
    }

    if (rem)    /* remaining bytes are less than io block size or not support multi block transfer */
    {
        /* ==== applied to CMD53 (IO_READ_EXTENDED_BYTE) ==== */
        mode = 0;   /* just support PIO */

        /* ---- disable SD_SECCNT ---- */
        SDMMC.SD_STOP.LONGLONG = 0x0000;
        
        /* ---- set transfer bytes ---- */
        SDMMC.SD_SIZE.LONGLONG = (uint64_t)rem;

        /* set argument */
        arg = ((func << 28) | (op_code << 26) | (adr << 9) | (uint32_t)rem);

        /* enable resp end and illegal access interrupts */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, 0);

        /* issue CMD53 (IO_READ_EXTENDED_BYTE) */
        if (_sd_send_iocmd(p_hndl, CMD53_R_BYTE, arg) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR);
            return _sdio_read_byte_error(p_hndl, mode);
        }

        /* ---- disable RespEnd and ILA ---- */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

        /* enable All end, BRE and errors */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

        /* software data transfer */
        ret = _sdio_software_trans2(p_hndl, buff, rem, SD_TRANS_READ);
        if (SD_OK != ret)
        {
            return _sdio_read_byte_error(p_hndl, mode);
        }

        /* wait All end interrupt */
        if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
            return _sdio_read_byte_error(p_hndl, mode);
        }

        /* ---- check errors ---- */
        if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
        {
            _sd_check_info2_err(p_hndl);
            return _sdio_read_byte_error(p_hndl, mode);
        }

        /* Cast to an appropriate type */
        _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);  /* clear All end bit */

        /* disable all interrupts */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);
    }

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_read_byte
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_read_byte_error
 * Description  : read io register space error
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 *                int32_t mode  : data transfer mode
 *                  SD_MODE_SW  : software
 *                  SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_read_byte_error(st_sdhndl_t *p_hndl, int32_t mode)
{
    int32_t  tmp_error;
    uint64_t sd_option;
    uint64_t sd_clk_ctrl;

    if (SD_MODE_DMA == mode)
    {
        /* Cast to an appropriate type */
        (void)sddev_disable_dma((int32_t)(p_hndl->sd_port));   /* disable DMA */

        /* reset DMAC */
        tmp_error = sddev_reset_dma((int32_t)(p_hndl->sd_port));
        if (SD_OK != tmp_error)
        {
            ;   /* do nothing */
        }
    }
    tmp_error = p_hndl->error;

    /* ---- clear error bits ---- */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- disable all interrupts ---- */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* Cast to an appropriate type */
    if ((SDMMC.SD_INFO2.LONGLONG & SD_INFO2_MASK_CBSY) == SD_INFO2_MASK_CBSY)
    {
        sddev_loc_cpu(p_hndl->sd_port);

        /* Cast to an appropriate type */
        sd_option   = SDMMC.SD_OPTION.LONGLONG;
        
        /* Cast to an appropriate type */
        sd_clk_ctrl = SDMMC.SD_CLK_CTRL.LONGLONG;
        
        /* Cast to an appropriate type */
        SDMMC.SOFT_RST.LONGLONG = SOFT_RST_SDRST_RESET;

        /* Cast to an appropriate type */
        SDMMC.SOFT_RST.LONGLONG = SOFT_RST_SDRST_RELEASED;

        /* Cast to an appropriate type */
        SDMMC.SD_OPTION.LONGLONG = sd_option;

        /* Cast to an appropriate type */
        SDMMC.SD_CLK_CTRL.LONGLONG = sd_clk_ctrl;
        sddev_unl_cpu(p_hndl->sd_port);
    }

    p_hndl->error = tmp_error;

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_read_byte_error
 *****************************************************************************/


/* End of File */
