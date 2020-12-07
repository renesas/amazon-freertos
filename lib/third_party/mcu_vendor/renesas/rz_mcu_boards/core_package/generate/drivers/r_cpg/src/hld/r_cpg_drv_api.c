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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name   : r_cpg_drv_api.c
 * Description : Clock Pulse Generator high layer driver
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <string.h>
#include "r_cpg_drv_link.h"
#include "r_cpg_hld_api.h"
#include "r_cpg_drv_api.h"
#include "r_cpg_drv_sc_cfg.h"

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* List channels supported */
#define R_CFG_CPG_CHANNELS_SUPPORTED         (0)

/* Maximum number of modules that can be granted simultaneous write access to this driver  */
#define  R_CFG_CPG_DRV_MAXIMUM_WRITE_ACCESS_PRV    (1)

/**********************************************************************************************************************
 Enumerated Types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Typedefs
 *********************************************************************************************************************/
typedef struct r_drvcpg
{
    uint32_t access_handles_available;
    uint8_t  drv_free;
} st_r_drv_res_map_t;

typedef struct
{
    uint16_t gs_channel_open;
} st_internal_configuration_t;

/**********************************************************************************************************************
 Function Prototypes
 *********************************************************************************************************************/
static int_t cpg_open (st_stream_ptr_t stream_ptr);
static void cpg_close (st_stream_ptr_t stream_ptr);
static int_t cpg_control (st_stream_ptr_t stream_ptr, uint32_t ctrl_code, void *ctrl_ptr);
static int_t cpg_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo);

/**********************************************************************************************************************
 Constant Data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/
/*! Version Information */
static const st_drv_info_t gs_hld_info =
{
    {
        ((STDIO_CPG_RZ_HLD_VERSION_MAJOR << 16) + STDIO_CPG_RZ_HLD_VERSION_MINOR)
    },
    STDIO_CPG_RZ_HLD_BUILD_NUM,
    STDIO_CPG_RZ_HLD_DRV_NAME
};

static bool_t gs_drv_cpg_is_initialized = false;
static st_internal_configuration_t s_drv_config =
{
    0 /* R_CFG_CPG_DRV_MAXIMUM_ACCESS_PRV */
};

/* Define the driver function table for this implementation */
/* GSCE Warning (global variable 'g_cpg_Driver' not declared static [9.3a])
 * can be safely ignored */
const st_r_driver_t g_cpg_driver =
{
    /* st_r_driver_t p_device */
    "CPG Device Driver", cpg_open, cpg_close, no_dev_read, no_dev_write, cpg_control, cpg_get_version
};

/******************************************************************************
 Function Name: cpg_open
 Description:   Function to open the CPG device driver
 Arguments:     IN  stream_ptr - Pointer to the file stream
 Return value:  DRV_SUCCESS : Success when attempting to initialise CPG
 *              DRV_ERROR   : Failure to initialise CPG
 ******************************************************************************/
static int_t cpg_open (st_stream_ptr_t stream_ptr)
{
    int_t ret = DRV_SUCCESS;

    if (!gs_drv_cpg_is_initialized)
    {
        gs_drv_cpg_is_initialized = true;
        R_CPG_InitialiseHwIf();
    }

    if ((stream_ptr->file_mode & __SWR) > 0)
    {
        /* write access requested */

        if (s_drv_config.gs_channel_open < R_CFG_CPG_DRV_MAXIMUM_WRITE_ACCESS_PRV)
        {
            /* driver already opened, just store the reference count */
            s_drv_config.gs_channel_open++;
        }
        else
        {
            /* return DRV_ERROR when too many write references to the driver are attempted */
            ret = DRV_ERROR;
        }
    }

    return (ret);
}
/******************************************************************************
 End of function  cpg_open
 ******************************************************************************/

/******************************************************************************
 Function Name: cpg_close
 Description:   Function to close the standard IN driver. All open devices
 connected to the led are closed.
 Arguments:     IN  stream_ptr - Pointer to the file stream
 Return value:  none
 ******************************************************************************/
static void cpg_close (st_stream_ptr_t stream_ptr)
{
    if ( (0 != s_drv_config.gs_channel_open)
      && ( (stream_ptr->file_mode & __SWR) > 0 ) )
    {
        s_drv_config.gs_channel_open--;
    }
}
/******************************************************************************
 End of function  cpg_close
 ******************************************************************************/

/******************************************************************************
 Function Name: cpg_control
 Description:   Function to handle custom controls for the standard IN device
 Arguments:     IN  stream_ptr - Pointer to the file stream
 IN  ctrl_code - The custom control code
 IN  ctrl_ptr - Pointer to the custom control structure
 Return value:  0 for success -1 on error
 ******************************************************************************/
static int_t cpg_control (st_stream_ptr_t stream_ptr, uint32_t ctrl_code, void *ctrl_ptr)
{
    int_t retval = DRV_ERROR;

    /* fail if the ctrl_ptr parameter is NULL */
    if (NULL == ctrl_ptr)
    {
        return (DRV_ERROR);
    }

    /* check if handle has write access */
    if (0 == (stream_ptr->file_mode & __SWR))
    {
        /* no write access - fail as handle trying to call a write function */
        if (CTL_CPG_GET_CLK != ctrl_code)
        {
            return (DRV_ERROR);
        }
    }
    else
    {
        /* This is a writeable stream */
        /* fail if the channel is not open */
        if (0 == s_drv_config.gs_channel_open)
        {
            return (DRV_ERROR);
        }
    }

    /* check if handle has read access */
    if (0 == (stream_ptr->file_mode & __SRD))
    {
        /* no read access - fail as handle trying to call a read function */
        if (CTL_CPG_GET_CLK == ctrl_code)
        {
            return (DRV_ERROR);
        }
    }

    switch (ctrl_code)
    {
        case CTL_CPG_SET_XTAL_KHZ:
        {
            retval = R_CPG_SetXtalClock(*(float64_t *) ctrl_ptr);
            break;
        }

        case CTL_CPG_SET_MAIN_CLK:
        {
            /* set input clock */
            retval = R_CPG_SetMainClock((st_r_drv_cpg_set_main_t *) ctrl_ptr);
            break;
        }

        case CTL_CPG_SET_SUB_CLK:
        {
            retval = R_CPG_SetSubClockDividers((st_r_drv_cpg_set_sub_t *) ctrl_ptr, 1);
            break;
        }

        case CTL_CPG_SET_CLK_SRC:
        {
            retval = R_CPG_SetSubClockSource((st_r_drv_cpg_set_src_t *) ctrl_ptr, 1);
            break;
        }

        case CTL_CPG_SET_EXT_CLK:
        {
            retval = R_CPG_ConfigExtClockPin((st_r_drv_cpg_ext_clk_t *) ctrl_ptr);
            break;
        }

        case CTL_CPG_GET_CLK:
        {
            st_r_drv_cpg_get_clk_t * p_cpg_get_clock_t = (st_r_drv_cpg_get_clk_t *) ctrl_ptr;

            retval = R_CPG_GetClock(p_cpg_get_clock_t->freq_src, &p_cpg_get_clock_t->clk_frequency_khz);
            break;
        }

        case CTL_CPG_SET_SUB_CLK_MULTI:
        {
            /* get array of settings and length integer */
            st_r_drv_cpg_set_sub_multi_t * p_sub_set_multi = (st_r_drv_cpg_set_sub_multi_t *) ctrl_ptr;

            /* set sub clocks */
            retval = R_CPG_SetSubClockDividers(p_sub_set_multi->p_sub_clk_settings,
                    p_sub_set_multi->number_of_settings);
            break;
        }

        default:
        {
            break;
        }
    }

    return (retval);
}
/******************************************************************************
 End of function  cpg_control
 ******************************************************************************/

/*******************************************************************************
 * Function Name: cpg_get_version
 * Description  : Provides build information even if driver fails to open
 *                version information is updated by developer
 * Arguments    : none
 * Return Value : 0 (never fails)
 ******************************************************************************/
static int_t cpg_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo)
{
    /* unused argument, kept to maintain defined API */
    (void) stream_ptr;

    pVerInfo->hld.version.sub.major = gs_hld_info.version.sub.major;
    pVerInfo->hld.version.sub.minor = gs_hld_info.version.sub.minor;
    pVerInfo->hld.build = gs_hld_info.build;
    pVerInfo->hld.p_szdriver_name = gs_hld_info.p_szdriver_name;

    /* Obtain version information from Low layer Driver */
    return (R_CPG_GetVersion(pVerInfo));
}
/*******************************************************************************
 End of function cpg_get_version
 ******************************************************************************/


/* End of File */
