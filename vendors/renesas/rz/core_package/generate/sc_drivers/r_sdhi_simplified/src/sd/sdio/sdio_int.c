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
* File Name    : sdio_int.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : SDIO interrupt operations
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
 * Function Name: _sdio_set_int_mask
 * Description  : set int_io_info_mask depend on the mask bits value
 *                if mask bit is one, it is enabled
 *                if mask bit is zero, it is disabled
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 *                uint64_t mask : SDIO_INFO1_MASK bits value
 * Return Value : SD_OK : end of succeed
 * Remark       : execute sdio_enable_int preceeded, to enable interrupt
 *****************************************************************************/
int32_t _sdio_set_int_mask(st_sdhndl_t *p_hndl, uint64_t mask)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- set int_io_info_mask ---- */
    p_hndl->int_io_info_mask |= mask;

    /* ---- set hardware mask ---- */
    SDMMC.SDIO_INFO1_MASK.LONGLONG = (uint64_t)~(p_hndl->int_io_info_mask);

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sdio_set_int_mask
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_clear_int_mask
 * Description  : clear int_io_info_mask depend on the mask bits value
 *                if mask bit is one, it is disabled
 *                if mask bit is zero, it is enabled
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 *                uint64_t mask : SDIO_INFO1_MASK bits value
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t _sdio_clear_int_mask(st_sdhndl_t *p_hndl, uint64_t mask)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- clear int_io_info_mask ---- */
    p_hndl->int_io_info_mask &= (uint64_t)~mask;

    /* ---- clear hardware mask ---- */
    SDMMC.SDIO_INFO1_MASK.LONGLONG = (uint64_t)~(p_hndl->int_io_info_mask);

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sdio_clear_int_mask
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_clear_info
 * Description  : clear int_io_info depend on the clear value
 * Arguments    : st_sdhndl_t *p_hndl   : SD handle
 *                uint64_t clear : int_io_info clear bits value
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sdio_clear_info(st_sdhndl_t *p_hndl, uint64_t clear)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- clear int_io_info ---- */
    p_hndl->int_io_info &= (uint64_t)~clear;

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sdio_clear_info
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sdio_get_int
 * Description  : get SDIO_INFO1 bits
 *                examine enabled elements
 *                hearafter, clear SDIO_INFO1 bits
 *                save those bits to int_io_info
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sdio_get_int(st_sdhndl_t *p_hndl)
{
    uint64_t info;

    /* get SDIO_INFO1 bits */
    info = (uint64_t)(SDMMC.SDIO_INFO1.LONGLONG & p_hndl->int_io_info_mask);  
    
    /* save enabled elements */
    p_hndl->int_io_info = info;

    /* Cast to an appropriate type */
    if (0 == (info & SDIO_MODE_IOMOD))
    {
        /* clear SDIO_INFO1 bits */
        SDMMC.SDIO_INFO1.LONGLONG = (((uint64_t)~(info)) & ((uint64_t)~(SDIO_MODE_IOMOD)) & (uint64_t)(SDIO_INFO1_MASK_EXWT|SDIO_INFO1_MASK_EXPUB52)) | (uint64_t)0x0000000000000006;
    }
    else
    {
        /* Set Mask of SDIO_MODE_IOMODE to mask SDIO Interrupts */
        p_hndl->int_io_info_mask &= (~SDIO_MODE_IOMOD);
        
        /* Cast to an appropriate type */
        SDMMC.SDIO_INFO1_MASK.LONGLONG = (uint64_t)~(p_hndl->int_io_info_mask);
    }

    if (info)
    {
        return SD_OK;   /* any interrupt occured */
    }

    return SD_ERR;  /* no interrupt occured */
}
/******************************************************************************
 End of function _sdio_get_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_check_int
 * Description  : check SDIO_INFO1 interrupt elements
 *                if any interrupt is detected, return SD_OK
 *                if no interrupt is detected, return SD_ERR
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_check_int(int32_t sd_port)
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

    if (p_hndl->int_mode)
    {
        /* ---- hardware interrupt mode ---- */
        if (p_hndl->int_io_info)
        {
            return SD_OK;
        }
        else
        {
            return SD_ERR;
        }
    }

    /* ---- polling mode ---- */
    return _sdio_get_int(p_hndl);
}
/******************************************************************************
 End of function sdio_check_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_int_handler
 * Description  : SDIO_INFO1 interrupt handler
 *                examine the relevant elements (without masked)
 *                save those elements to int_io_info
 *                if a callback function is registered, call it
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : none
 *****************************************************************************/
void sdio_int_handler(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return ;
    }

    if (_sdio_get_int(p_hndl) == SD_OK)
    {
        if (p_hndl->int_io_callback)
        {
            (*p_hndl->int_io_callback)(sd_port);
        }
    }
    else
    {
        /* any interrupt should be occured! */
        ;
    }
}
/******************************************************************************
 End of function sdio_int_handler
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_set_intcallback
 * Description  : register SDIO_INFO1 callback function
 *                if SDIO_INFO1 interrupt are occured, call callback function
 * Arguments    : int32_t sd_port              : channel no (0 or 1)
 *                int32_t (*callback)(int32_t) : callback function
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t sdio_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t))
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

    p_hndl->int_io_callback = callback;

    return SD_OK;
}
/******************************************************************************
 End of function sdio_set_intcallback
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_enable_int
 * Description  : enable SDHI detect interrupt from SDIO (=IRQ)
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t sdio_enable_int(int32_t sd_port)
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

    sddev_loc_cpu(p_hndl->sd_port);

    /* Cast to an appropriate type */
    p_hndl->int_io_info &= (~SDIO_MODE_IOMOD);

    /* ---- clear IOIRQ ---- */
    SDMMC.SDIO_INFO1.LONGLONG = (uint64_t)~(SDIO_MODE_IOMOD);

    /* Clear Mask of SDIO_MODE_IOMODE to Set SDIO Interrupts Enable */
    SDMMC.SDIO_INFO1_MASK.LONGLONG = ((uint64_t)(SDMMC.SDIO_INFO1_MASK.LONGLONG & (~SDIO_MODE_IOMOD))) | (uint64_t)0x06;

    /* Cast to an appropriate type */
    p_hndl->int_io_info_mask |= SDIO_MODE_IOMOD;

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function sdio_enable_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: sdio_disable_int
 * Description  : disable SDHI detect interrupt from SDIO (=IRQ)
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t sdio_disable_int(int32_t sd_port)
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

    sddev_loc_cpu(p_hndl->sd_port);

    /* Set Mask of SDIO_MODE_IOMODE to mask SDIO Interrupts */
    SDMMC.SDIO_INFO1_MASK.LONGLONG = SDMMC.SDIO_INFO1_MASK.LONGLONG | SDIO_MODE_IOMOD;
    
    /* Cast to an appropriate type */
    p_hndl->int_io_info_mask &= (~SDIO_MODE_IOMOD);

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function sdio_disable_int
 *****************************************************************************/

/* End of File */
