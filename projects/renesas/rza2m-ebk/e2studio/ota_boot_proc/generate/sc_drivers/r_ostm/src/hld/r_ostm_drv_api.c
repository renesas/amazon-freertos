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
 * Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/*******************************************************************************
 * File Name   : r_ostm_drv_api.c
 * Description : OS timer driver high layer driver
 *******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_ostm_drv_api.h"
#include "r_ostm_drv_link.h"
#include "r_ostm_drv_sc_cfg.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* Comment this line out to turn ON module trace in this file */
#undef _TRACE_ON_

#ifndef _TRACE_ON_
#undef DEBUG_PRV_TRACE
#define  DEBUG_PRV_TRACE(x)
#endif

/******************************************************************************
 Enumerated Types
 ******************************************************************************/

/******************************************************************************
 Typedefs
 ******************************************************************************/

/*****************************************************************************
 Function Prototypes
 ******************************************************************************/
static int_t ostm_open(st_stream_ptr_t stream_ptr);
static void ostm_close(st_stream_ptr_t stream_ptr);
static int_t ostm_read(st_stream_ptr_t stream_ptr, uint8_t *buff_ptr, uint32_t count);
static int_t ostm_control(st_stream_ptr_t stream_ptr, uint32_t ctrl_code, void *ctrl_ptr);
static int_t ostm_get_version(st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo);

/*****************************************************************************
 Constant Data
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/*! Version Information */
static const st_drv_info_t gs_hld_info =
{
    {
        ((STDIO_OSTM_HLD_VERSION_MAJOR << 16) + STDIO_OSTM_HLD_VERSION_MINOR)
    },
    STDIO_OSTM_HLD_BUILD_NUM,
    STDIO_OSTM_HLD_DRV_NAME
};

bool_t gs_r_drv_ostm_open[OSTM_CH_MAX] = {false, false, false};


/* Define the driver function table for this implementation */
/* GSCE Warning (global variable 'g_ostm_Driver' not declared static [9.3a])
 * can be safely ignored */
const st_r_driver_t g_ostm_driver =
{
    /* st_r_driver_t p_device */
    "OS Timer Driver",
    ostm_open,
    ostm_close,
    ostm_read,
    no_dev_write,
    ostm_control,
    ostm_get_version
};

/**
 * @brief          reconfigure_timer
 *                 Reconfigure opened channel
 * @param[in]      channel:     desired channel
 * @param[in/out]  p_config:    configuration for the channel
 * @retval         DRV_SUCCESS  Successful operation
 * @retval         DRV_ERROR    Failure operation
 */
static int_t reconfigure_timer(e_r_drv_ostm_channel_t channel, const st_r_drv_ostm_config_t * p_config)
{
    int_t ret = DRV_SUCCESS;

    /* 1: stop current timer */
    if (DRV_SUCCESS == ret)
    {
        ret = R_OSTM_Stop(channel);
    }

    /* 2: close the timer */
    if (DRV_SUCCESS == ret)
    {
        ret = R_OSTM_Close(channel);
    }

    /* 3: re-initialise the timer */
    if (DRV_SUCCESS == ret)
    {
        ret = R_OSTM_Init(channel, p_config);
    }

    return (ret);
}
/***********************************************************************************************************************
 End of function reconfigure_timer
 **********************************************************************************************************************/

/******************************************************************************
 Function Name: ostm_open
 Description:   Function to open the system timer device driver
 Arguments:     IN  stream_ptr - Pointer to the file stream
 Return Value:  DRV_SUCCESS for success otherwise DRV_ERROR
 ******************************************************************************/
static int_t ostm_open(st_stream_ptr_t stream_ptr)
{
    int_t ret = DRV_ERROR;
    e_r_drv_ostm_channel_t channel;

    /* obtain the OSTM channel */
    channel = OSTM_SC_TABLE[stream_ptr->sc_config_index].channel;

    /* only 1 instance of each channel is permissible */
    /* check to see if channel is available HLD */
    if (!gs_r_drv_ostm_open[channel])
    {
        ret = R_OSTM_Init(channel, &OSTM_SC_TABLE[stream_ptr->sc_config_index].config);

        if (DRV_SUCCESS == ret)
        {
            gs_r_drv_ostm_open[channel] = true;
        }
    }
    else
    {
        /* No more modules can be granted simultaneous access to this driver */
        DEBUG_PRV_TRACE(("Error - Can not open %s channel is already opened\r\n", stream_ptr->pszStreamName));
    }

    return (ret);
}
/******************************************************************************
 End of function  ostm_open
 ******************************************************************************/

/******************************************************************************
 Function Name: ostm_close
 Description:   Function to close the standard IN driver. All open devices
 connected to the led are closed.
 Arguments:     IN  stream_ptr - Pointer to the file stream
 Return value:  none
 ******************************************************************************/
static void ostm_close(st_stream_ptr_t stream_ptr)
{
    e_r_drv_ostm_channel_t channel;

    /* obtain the OSTM channel */
    channel = OSTM_SC_TABLE[stream_ptr->sc_config_index].channel;

    if (gs_r_drv_ostm_open[channel])
    {
        /* reset the state of this driver so that the next time it is opened the driver is reinitialised */
        gs_r_drv_ostm_open[channel] = false;

        R_OSTM_Close(channel);
    }
}
/******************************************************************************
 End of function  ostm_close
 ******************************************************************************/

/******************************************************************************
 Function Name: ostm_read
 Description:   Function to read data from the OSTM device
 Arguments:     IN  stream_ptr - Pointer to the file stream
                IN  buffer - Pointer to the destination memory
                IN  count - The number of bytes to read
 Return value:  data count for success or Error Code on error
 ******************************************************************************/
static int_t ostm_read(st_stream_ptr_t stream_ptr, uint8_t *buff_ptr, uint32_t count)
{
    int_t err_code = DRV_ERROR;
    e_r_drv_ostm_channel_t channel;

    /* the counter is a uint32_t, so the buffer needs to be at least that big */
    if (count < sizeof(uint32_t))
    {
        return (DRV_ERROR);
    }

    /* obtain the OSTM channel */
    channel = OSTM_SC_TABLE[stream_ptr->sc_config_index].channel;

    /* obtain the current counter
     * (safe for change to uint32_t pointer due to count restricts 4 byte or more)
     */
    err_code = R_OSTM_GetCount(channel, (uint32_t *) buff_ptr);

    return (err_code);
}
/******************************************************************************
 End of function ostm_read
 ******************************************************************************/

/******************************************************************************
 Function Name: ostm_control
 Description:   Function to handle custom controls for the standard IN device
 Arguments:     IN  stream_ptr - Pointer to the file stream
 IN  ctrl_code - The custom control code
 IN  ctrl_ptr - Pointer to the custom control structure
 Return value:  0 for success -1 on error
 ******************************************************************************/
static int_t ostm_control(st_stream_ptr_t stream_ptr, uint32_t ctrl_code, void *ctrl_ptr)
{
    int_t retval = DRV_ERROR;
    e_r_drv_ostm_channel_t channel;
    st_r_drv_ostm_config_t * p_config;

    /* obtain the OSTM channel */
    channel = OSTM_SC_TABLE[stream_ptr->sc_config_index].channel;

    switch (ctrl_code)
    {
        case CTRL_OSTM_START_TIMER:
        {
            /* start timer */
            retval = R_OSTM_Start(channel);
            break;
        }

        case CTRL_OSTM_STOP_TIMER:
        {
            /* stop timer */
            retval = R_OSTM_Stop(channel);
            break;
        }

        case CTRL_OSTM_RECONFIGURE:
        {
            if (NULL != ctrl_ptr)
            {
                /* convert parameter pointer to the function specific */
                p_config = (st_r_drv_ostm_config_t *) ctrl_ptr;

                retval = reconfigure_timer(channel, p_config);
            }

            break;
        }

        case CTRL_OSTM_READ_COUNTER:
        {
            if (NULL != ctrl_ptr)
            {
                /* read the timer */
                retval = R_OSTM_GetCount(channel, (uint32_t *) ctrl_ptr);
            }

            break;
        }

        default:
        {
            DEBUG_PRV_TRACE(("drvOSTM: Unknown control code\r\n"));
            break;
        }
    }

    return (retval);
}
/******************************************************************************
 End of function  ostm_control
 ******************************************************************************/

/*******************************************************************************
 * Function Name: ostm_get_version
 * Description  : Provides build information even if driver fails to open
 *                version information is updated by developer
 * Arguments    : none
 * Return Value : 0 (never fails)
 ******************************************************************************/
static int_t ostm_get_version(st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo)
{
    /* stream_ptr not used */
    UNUSED_PARAM (stream_ptr);

    pVerInfo->hld.version.sub.major = gs_hld_info.version.sub.major;
    pVerInfo->hld.version.sub.minor = gs_hld_info.version.sub.minor;
    pVerInfo->hld.build = gs_hld_info.build;
    pVerInfo->hld.p_szdriver_name = gs_hld_info.p_szdriver_name;

    /* Obtain version information from Low Layer Driver */
    return R_OSTM_GetVersion (pVerInfo);
}
/*******************************************************************************
 End of function ostm_get_version
 ******************************************************************************/

/* End of File */
