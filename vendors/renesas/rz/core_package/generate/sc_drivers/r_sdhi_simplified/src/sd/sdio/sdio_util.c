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
* File Name    : sdio_util.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Function setting
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
static int32_t _sdio_set_enable_error(st_sdhndl_t *p_hndl);
static int32_t _sdio_get_ready_error(st_sdhndl_t *p_hndl);
static int32_t _sdio_set_int_error(st_sdhndl_t *p_hndl);
static int32_t _sdio_get_int_error(st_sdhndl_t *p_hndl);
static int32_t _sdio_set_blocklen_error(st_sdhndl_t *p_hndl);
static int32_t _sdio_get_blocklen_error(st_sdhndl_t *p_hndl);

/******************************************************************************
 * Function Name: sdio_reset
 * Description  : Reset SDIO Function
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sdio_reset(int32_t sd_port)
{
    st_sdhndl_t   *p_hndl;
    uint8_t  io_buff;
    uint64_t sd_option;
    uint64_t sd_clk_ctrl;

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

    /* Cast to an appropriate type */
    if ((SDMMC.SD_INFO2.LONGLONG & SD_INFO2_MASK_CBSY) == SD_INFO2_MASK_CBSY)
    {
        sddev_loc_cpu(sd_port);

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
        sddev_unl_cpu(sd_port);
    }

    io_buff = 0x08;
    if (sdio_write_direct(sd_port, &io_buff, 0, 0x06, SD_IO_VERIFY_WRITE) != SD_OK)
    {
        return p_hndl->error;
    }

    /* ---- clear mount flag ---- */
    p_hndl->mount = SD_UNMOUNT_CARD;

    /* ---- initilaize SD handle ---- */
    _sd_init_hndl(p_hndl, 0, p_hndl->voltage);
    p_hndl->error = SD_OK;

    return SD_OK;
}
/******************************************************************************
 End of function sdio_reset
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_get_ioocr
 * Description  : get io ocr register value
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *                uint32_t *ioocr : IO OCR register address
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sdio_get_ioocr(int32_t sd_port, uint32_t *ioocr)
{
    st_sdhndl_t   *p_hndl;
    uint32_t temp;

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

    /* ---- check media type ---- */
    if ((p_hndl->media_type & SD_MEDIA_IO) == 0)
    {
        _sd_set_err(p_hndl, SD_ERR_CARD_TYPE);
        return SD_ERR_CARD_TYPE;
    }

    if (ioocr)
    {
        /* Cast to an appropriate type */
        temp   = (uint32_t)(p_hndl->io_ocr[0] & 0xffff);
        temp <<= 16;

        /* Cast to an appropriate type */
        temp  |= (uint32_t)(p_hndl->io_ocr[1] & 0xffff);
        *ioocr = temp;
    }

    return SD_OK;
}
/******************************************************************************
 End of function sdio_get_ioocr
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_get_ioinfo
 * Description  : get io information(IO OCR[31:24])
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *                uint8_t *ioinfo : io information(IO OCR[31:24])
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sdio_get_ioinfo(int32_t sd_port, uint8_t *ioinfo)
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

    /* ---- check media type ---- */
    if ((p_hndl->media_type & SD_MEDIA_IO) == 0)
    {
        _sd_set_err(p_hndl, SD_ERR_CARD_TYPE);
        return SD_ERR_CARD_TYPE;
    }

    if (ioinfo)
    {
        *ioinfo = p_hndl->io_info;
    }

    return SD_OK;
}
/******************************************************************************
 End of function sdio_get_ioinfo
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_get_cia
 * Description  : get cia value
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *                uint8_t *reg      : CCCR or FBR register address
 *                uint8_t *cis      : CIS register address
 *                uint32_t func_num : function number (0 to 7, 0 means Common)
 *                int32_t cnt       : size of CIS to read
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sdio_get_cia(int32_t sd_port, uint8_t *reg, uint8_t *cis, uint32_t func_num, int32_t cnt)
{
    st_sdhndl_t   *p_hndl;
    uint32_t i;
    uint32_t adr = 0;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    /* check read register address and function number */
    if (func_num > 7)
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }
    p_hndl->error = SD_OK;

    /* Cast to an appropriate type */
    if ( sdio_read(sd_port, p_hndl->io_reg[func_num], 0, (uint32_t)(0x100 * func_num),
                    SDIO_INTERNAL_REG_SIZE, (SD_IO_INCREMENT_ADDR | SD_IO_FORCE_BYTE)) != SD_OK )
    {
        return p_hndl->error;
    }

    if (reg)    /* CCCR or FBR */
    {
        for (i = 0; i < SDIO_INTERNAL_REG_SIZE; ++i)
        {
            *reg++ = p_hndl->io_reg[func_num][i];
        }
    }

    /* Cast to an appropriate type */
    adr = (uint32_t)( ((uint32_t)p_hndl->io_reg[func_num][0x0b] << 16) |

            /* Cast to an appropriate type */
            ((uint32_t)p_hndl->io_reg[func_num][0x0a] <<  8) | p_hndl->io_reg[func_num][0x09] );
    if (0 != adr)
    {
        if (cis)
        {
            if (sdio_read(sd_port, cis, 0, adr, cnt, (SD_IO_INCREMENT_ADDR | SD_IO_FORCE_BYTE)) != SD_OK)
            {
                return p_hndl->error;
            }

            for (i = 0; i < cnt; ++i)
            {
                p_hndl->cis[func_num][i] = *cis++;
            }
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function sdio_get_cia
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_set_enable
 * Description  : set io functions access raedy state
 * Arguments    : int32_t sd_port  : channel no (0 or 1)
 *                uint8_t func_bit : specify function by bit map
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_set_enable(int32_t sd_port, uint8_t func_bit)
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

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    func_bit &= 0xfe;

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sdio_set_enable_error(p_hndl);
    }

    /* ==== set io functions (I/O enable) ==== */
    if (_sdio_direct(p_hndl, &func_bit, 0, 0x02, 1, SD_IO_VERIFY_WRITE) != SD_OK)
    {
        return _sdio_set_enable_error(p_hndl);
    }

    /* save I/O enable register */
    p_hndl->io_reg[0][0x02] = func_bit;

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function sdio_set_enable
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_set_enable_error
 * Description  : set io functions ready error
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_set_enable_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_set_enable_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_get_ready
 * Description  : inquire io function's access ready status
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *                uint8_t *func_bit : access enable function by bit map
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_get_ready(int32_t sd_port, uint8_t *func_bit)
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

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sdio_get_ready_error(p_hndl);
    }

    /* ==== get io functions (I/O ready) ==== */
    if (_sdio_direct(p_hndl, func_bit, 0, 0x03, 0, 0) != SD_OK)
    {
        return _sdio_get_ready_error(p_hndl);
    }

    /* save I/O ready register */
    p_hndl->io_reg[0][0x03] = *func_bit;

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function sdio_get_ready
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_get_ready_error
 * Description  : inquire io function's access ready status
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_get_ready_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_get_ready_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_set_int
 * Description  : enable or disable interrupt from io functions
 * Arguments    : int32_t sd_port  : channel no (0 or 1)
 *                uint8_t func_bit : specify enable function by bit map
 *                int32_t enab     : control enable or disable
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_set_int(int32_t sd_port, uint8_t func_bit, int32_t enab)
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

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    if (enab)
    {
        func_bit |= 0x01;   /* set enable */
    }
    else
    {
        func_bit &= 0xfe;   /* set disable */
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sdio_set_int_error(p_hndl);
    }

    /* ==== set io functions (Int Enable) ==== */
    if (_sdio_direct(p_hndl, &func_bit, 0, 0x04, 1, SD_IO_VERIFY_WRITE) != SD_OK)
    {
        return _sdio_set_int_error(p_hndl);
    }

    /* save Int enable register */
    p_hndl->io_reg[0][0x04] = func_bit;

    sdio_enable_int(sd_port);

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function sdio_set_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_set_int_error
 * Description  : set interrupt from io functions error
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_set_int_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_set_int_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_get_int
 * Description  : inquire io functions's interrupt status
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *                uint8_t *func_bit : ready function by bit map
 *                int32_t *enab     : control enable or disable
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_get_int(int32_t sd_port, uint8_t *func_bit, int32_t *enab)
{
    st_sdhndl_t  *p_hndl;
    uint8_t int_enab;

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

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sdio_get_int_error(p_hndl);
    }

    /* ==== get io functions (Int Enable) ==== */
    if (_sdio_direct(p_hndl, func_bit, 0, 0x04, 0, 0) != SD_OK)
    {
        return _sdio_get_int_error(p_hndl);
    }

    /* save I/O Int Enable register */
    p_hndl->io_reg[0][0x04] = *func_bit;

    int_enab = p_hndl->io_reg[0][0x04];
    if (int_enab & 0x1)
    {
        *enab = 1;
    }
    else
    {
        *enab = 0;
    }

    return SD_OK;
}
/******************************************************************************
 End of function sdio_get_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_get_int_error
 * Description  : get interrupt form io functions error
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_get_int_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_get_int_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_set_blocklen
 * Description  : set io function's block length
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *                uint16_t len      : block length
 *                uint32_t func_num : specify function by number (from 0 to 7)
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_set_blocklen(int32_t sd_port, uint16_t len, uint32_t func_num)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    /* check read register address and function number */
    if (func_num > 7)
    {
        return SD_ERR;
    }

    switch (len)
    {
        case 32:
        case 64:
        case 128:
        case 256:
        case 512:

            /* len is OK */
            break;
        default:
            return SD_ERR;
            break;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->error = SD_OK;

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sdio_set_blocklen_error(p_hndl);
    }

    /* ---- set io functions block length ---- */
    if (_sdio_set_blocklen(p_hndl, len, func_num) != SD_OK)
    {
        return _sdio_set_blocklen_error(p_hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function sdio_set_blocklen
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_set_blocklen_error
 * Description  : set io functions block length error
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_set_blocklen_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_set_blocklen_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_set_blocklen
 * Description  : set io function's block length
 * Arguments    : st_sdhndl_t *p_hndl      : SD handle
 *                uint16_t len      : block length
 *                uint32_t func_num : specify function by number (from 0 to 7)
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sdio_set_blocklen(st_sdhndl_t *p_hndl, uint16_t len, uint32_t func_num)
{
    uint32_t adr;
    uint32_t base_adr;
    uint8_t  reg_val;
    uint16_t len_t;

    if (func_num > 7)
    {
        _sd_set_err(p_hndl, SD_ERR_ILL_FUNC);
        return p_hndl->error;     /* illegal function */
    }

    if (len > 512)
    {
        _sd_set_err(p_hndl, SD_ERR_ILL_FUNC);
        return p_hndl->error;     /* illegal function */
    }

    /* ---- check is support multiple block transfer ---- */
    if ((p_hndl->io_reg[0][0x08] & 0x02) == 0)
    {
        _sd_set_err(p_hndl, SD_ERR_ILL_FUNC);
        return p_hndl->error;     /* illegal function */
    }

    len_t = len;

    /* Cast to an appropriate type */
    base_adr = (uint32_t)(0x100 * func_num);
    for (adr = 0x10; adr <= 0x11; adr++)
    {
        /* set write value by little endian form */
        reg_val = (uint8_t)len;
        len   >>= 8;

        /* ==== set io functions (FN Block Size) ==== */
        if (_sdio_direct(p_hndl, &reg_val, 0, (base_adr + adr), 1, SD_IO_VERIFY_WRITE) != SD_OK)
        {
            return p_hndl->error;
        }

        /* save FN Block Size */
        p_hndl->io_reg[func_num][adr] = reg_val;
    }

    /* save io block length */
    p_hndl->io_len[func_num] = len_t;

    return SD_OK;
}
/******************************************************************************
 End of function _sdio_set_blocklen
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_get_blocklen
 * Description  : inquire io function's block length
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *                uint16_t *len     : block length
 *                uint32_t func_num : specify function by number (from 0 to 7)
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_get_blocklen(int32_t sd_port, uint16_t *len, uint32_t func_num)
{
    st_sdhndl_t   *p_hndl;
    uint16_t len_t;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    /* check read register address and function number */
    if (func_num > 7)
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }
    p_hndl->error = SD_OK;

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* check to support block transfer */
    if ((p_hndl->io_reg[0][0x08] & 0x02) == 0)
    {
        _sd_set_err(p_hndl, SD_ERR_ILL_FUNC);
        return SD_ERR;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sdio_get_blocklen_error(p_hndl);
    }

    /* Cast to an appropriate type */
    if ( _sdio_read_byte(p_hndl, p_hndl->io_reg[func_num], 0, (uint32_t)(0x100 * func_num),
                            SDIO_INTERNAL_REG_SIZE, SD_IO_INCREMENT_ADDR) != SD_OK )
    {
        return _sdio_get_blocklen_error(p_hndl);
    }
    len_t  = p_hndl->io_reg[func_num][0x11];
    len_t <<= 8;
    len_t |= (p_hndl->io_reg[func_num][0x10] & 0x00ff);

    switch (len_t)
    {
        case 32:
        case 64:
        case 128:
        case 256:
        case 512:

            /* len is OK */
            p_hndl->io_len[func_num] = len_t;             /* already and supported */
            break;
        default:
            p_hndl->io_len[func_num] = 0xffff;            /* already but not supported */
            break;
    }

    *len = len_t;

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function sdio_get_blocklen
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_get_blocklen_error
 * Description  : get io functions block length error
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sdio_get_blocklen_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sdio_get_blocklen_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_abort
 * Description  : set flag (=SD handle member stop) stop operations compulsory
 *                if this flag is set, read, write, format operations is
 *                stopped
 *                this flag is used for card detect/removal interrupt detection
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *                uint32_t func_num : function number
 * Return Value : none
 *****************************************************************************/
void sdio_abort(int32_t sd_port, uint32_t func_num)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return;
    }

    /* check read register address and function number */
    if (func_num > 7)
    {
        return;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return; /* not initilized */
    }

    p_hndl->io_abort[func_num] = 1;
}
/******************************************************************************
 End of function sdio_abort
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_set_blkcnt
 * Description  : set maximum block count per multiple command
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *                int16_t blocks  : block count
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 * Remark       : maximam block count is constrained from 3 to 32767(0x7fff)
 *****************************************************************************/
int32_t sdio_set_blkcnt(int32_t sd_port, int16_t blocks)
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

    if (blocks < 1)
    {
        /* need more than 1 continuous transfer */
        return SD_ERR;  /* undefined value */
    }

    if (!(p_hndl->media_type & SD_MEDIA_IO))
    {
        return SD_ERR_IO_CAPAB;
    }

    p_hndl->trans_blocks = blocks;

    return SD_OK;
}
/******************************************************************************
 End of function sdio_set_blkcnt
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_get_blkcnt
 * Description  : get maximum block count per multiple command
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : not less than 0 : block count
 *                SD_ERR: end of error
 * Remark       : maximam block count are constrained from 1 to 65535
 *****************************************************************************/
int32_t sdio_get_blkcnt(int32_t sd_port)
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

    if (!(p_hndl->media_type & SD_MEDIA_IO))
    {
        return SD_ERR_IO_CAPAB;
    }

    /* Cast to an appropriate type */
    return (int32_t)p_hndl->trans_blocks;
}
/******************************************************************************
 End of function sdio_get_blkcnt
 *****************************************************************************/

/* End of File */
