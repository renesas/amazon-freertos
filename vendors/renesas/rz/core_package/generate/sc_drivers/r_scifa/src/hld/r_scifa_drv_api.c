/**********************************************************************************************************************
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_scifa_drv_api.c
 * Description  : SCIFA High Layer Device driver
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 15.05.2018 1.00    Release
 *              : 10.05.2019 1.01    Fixed issue with channel 0 Baud rate setting
 *              : 23.05.2019 1.02    Restructuring to ensure that other LLDs are only accessed from the SCIFA LLD
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_devlink_wrapper.h"      /* Devlink API header */
#include "r_scifa_drv_api.h"        /* SCIFA driver API header */
#include "r_scifa_drv_link.h"       /* Link include to Low Layer Driver */
#include "r_scifa_hld_prv.h"        /* High Layer Driver private functions */

#include "r_gpio_drv_api.h"
#include "r_gpio_drv_sc_cfg.h"

#include "r_os_abstraction_api.h"

#include "driver.h"                 /* device driver header */


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Function Prototypes
 *********************************************************************************************************************/
/* STDIO */
static int_t scifa_hld_open (st_stream_ptr_t p_stream);
static void  scifa_hld_close (st_stream_ptr_t p_stream);
static int_t scifa_hld_read (st_stream_ptr_t p_stream, uint8_t *p_buffer, uint32_t ui_count);
static int_t scifa_hld_write (st_stream_ptr_t p_stream, const uint8_t *p_buffer, uint32_t count);
static int_t scifa_hld_control (st_stream_ptr_t p_stream, uint32_t ctlCode, void *pCtlStruct);
static int_t scifa_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo);

/* file stream access */
static r_sc_index_t scifa_get_config_index (st_stream_ptr_t stream_ptr);

/**********************************************************************************************************************
 Constant Data
 *********************************************************************************************************************/

static const st_drv_info_t gs_hld_info =
{
    {
        ((STDIO_SCIFA_RZ_HLD_VERSION_MAJOR << 16) + STDIO_SCIFA_RZ_HLD_VERSION_MINOR)
    },
    STDIO_SCIFA_RZ_HLD_BUILD_NUM,
    STDIO_SCIFA_RZ_HLD_DRV_NAME
};

/* Define the driver function table for this implementation */
const st_r_driver_t g_scifa_driver =
{"SCIFA Device Driver", scifa_hld_open, scifa_hld_close, scifa_hld_read, scifa_hld_write, scifa_hld_control, scifa_hld_get_version};

/**********************************************************************************************************************
 Private global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private Functions
 *********************************************************************************************************************/

/**
 * @brief  Opens the SCIFA device driver
 * 
 * This function allows for a channel to be ready for
 * operational use. If a channel is unavailable or a
 * channel is already opened this function will return
 * a fail (-1)
 *
 * @param[in] p_stream Pointer to the file stream
 * 
 * @retval 0  DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 **/
static int_t scifa_hld_open (st_stream_ptr_t p_stream)
{
    int_t ret = DRV_ERROR;

    /* get the Smart Configuration index associated with the handle */
    r_sc_index_t sc_config_index = scifa_get_config_index(p_stream);

    /* open using SC settings for selected channel */
    ret = r_scifa_hld_prv_open(sc_config_index, p_stream->file_mode);

    /* return status */
    return (ret);
}
/**********************************************************************************************************************
 * End of Function scifa_hld_open
 *********************************************************************************************************************/

/**
 * @brief Closes the SCIFA driver
 *
 * The SCIFA driver's file stream can be closed once
 * the desired operation has been done, freeing any
 * memory or resources it uses
 * 
 * @param[in] p_stream Pointer to the file stream
 *
 * @return None
 */
static void scifa_hld_close (st_stream_ptr_t p_stream)
{
    /* get the Smart Configuration index associated with the handle */
    r_sc_index_t sc_config_index = scifa_get_config_index(p_stream);

    /* close the channel */
    r_scifa_hld_prv_close(sc_config_index);
}
/**********************************************************************************************************************
 * End of Function scifa_hld_close
 *********************************************************************************************************************/

/**
 * @brief Translates the read operation to the private read function.
 *
 * The private read function requires to get the channel instead of the
 * file stream and therefore some translation is required.
 *
 * @param[in]  p_stream The file stream that contains the desired SCIFA channel
 * @param[out] p_buffer The buffer to be populated with the data read
 * @param[in]  count    The size of p_buffer
 *
 * @retval  N Where N is the number of bytes read
 * @retval -1 DRV_ERROR 
 */
static int_t scifa_hld_read (st_stream_ptr_t p_stream, uint8_t *p_buffer, uint32_t count)
{
    /* get the Smart Configuration index associated with the handle */
    r_sc_index_t sc_config_index = scifa_get_config_index(p_stream);

    /* read from channel */
    return (r_scifa_hld_prv_read(sc_config_index, p_buffer, (int32_t) count));
}
/**********************************************************************************************************************
 * End of Function scifa_hld_read
 *********************************************************************************************************************/

/**
 * @brief Translates the write operation to the private write function
 *
 * The private write function requires to get the channel instead of the
 * file stream and therefore some translation is required
 *
 * @param[in] p_stream The file stream that contains the desired SCIFA channel
 * @param[in] p_buffer The intended buffer of data intended to be written
 * @param[in] count    The amount of data in the p_buffer
 *
 * @retval >0 The number of bytes written
 * @retval -1 DRV_ERROR 
 */
static int_t scifa_hld_write (st_stream_ptr_t p_stream, const uint8_t *p_buffer, uint32_t count)
{
    /* get the Smart Configuration index associated with the handle */
    r_sc_index_t sc_config_index = scifa_get_config_index(p_stream);

    /* write to channel */
    return (r_scifa_hld_prv_write(sc_config_index, p_buffer, (int32_t) count));
}
/**********************************************************************************************************************
 * End of Function scifa_hld_write
 *********************************************************************************************************************/

/**
 * @brief scifa_hld_control handles custom controls for the SCIFA device driver
 * 
 * Depending on the ctl_code given the control function will perform specific
 * tasks. For the SCIFA this includes getting and setting the configuration.
 * If a invalid ctl_code is given the return value will be -1 and the trace will
 * print a message accordingly.
 *
 * @param[in] p_stream          Pointer to the file stream
 * @param[in] ctl_code          The custom control code
 * @param[in,out] p_ctl_struct  Pointer to the custom control structure
 *
 * @retval 0  DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
static int_t scifa_hld_control (st_stream_ptr_t p_stream, uint32_t ctl_code, void *p_ctl_struct)
{
    int_t ret = DRV_ERROR;

    /* get the Smart Configuration index associated with the handle */
    r_sc_index_t sc_config_index = scifa_get_config_index(p_stream);

    /* if the configuration index is valid */
    if (DRV_ERROR != sc_config_index)
    {
        switch (ctl_code)
        {
            case CTL_SCIFA_SET_CONFIGURATION:
            {
                /* check argument is not NULL */
                if (NULL != p_ctl_struct)
                {
                    ret = r_scifa_hld_prv_set_config(sc_config_index, p_ctl_struct);
                }
                break;
            }

            case CTL_SCIFA_GET_CONFIGURATION:
            {
                /* check argument is not NULL */
                if (NULL != p_ctl_struct)
                {
                    ret = r_scifa_hld_prv_get_config(sc_config_index, p_ctl_struct);
                }
                break;
            }

            case CTL_SCIFA_DMA_READ_ABORT:
            {
                ret = r_scifa_hld_dma_read_abort(sc_config_index);
                break;
            }

            case CTL_SCIFA_DMA_WRITE_ABORT:
            {
                ret = r_scifa_hld_dma_write_abort(sc_config_index);
                break;
            }

            case CTL_SCIFA_READ_NEXT:
            {
                /* check argument is not NULL */
                if (NULL != p_ctl_struct)
                {
                    /* Set next DMA read */
                    ret = r_scifa_hld_dma_read_next(sc_config_index, (st_scifa_dma_rw_t *) p_ctl_struct);
                }
                break;
            }

            case CTL_SCIFA_WRITE_NEXT:
            {
                /* check argument is not NULL */
                if (NULL != p_ctl_struct)
                {
                    /* Set next DMA write */
                    ret = r_scifa_hld_dma_write_next(sc_config_index, (st_scifa_dma_rw_t *) p_ctl_struct);
                }
                break;
            }

            case CTL_SCIFA_FLUSH_OUTPUT:
            {
                ret = r_scifa_hld_tx_flush(sc_config_index);
                break;
            }

            default:
            {
                printf("SCIFA Driver: Unknown control code %ld\r\n", ctl_code);
                break;
            }
        }
    }

    return (ret);
}
/**********************************************************************************************************************
 * End of Function scifa_hld_control()
 *********************************************************************************************************************/

/**
 * @brief Provides build information even if driver fails to open\n
 *        version information is updated by developer
 * 
 * Acquires the version information for both the high layer and
 * low layer driver. This allows for control of releases and debugging
 * code.
 *
 * @param stream_ptr  Pointer to the file stream
 * @param p_ver_info  Structure holding the version information
 *
 * @retval 0  DRV_SUCCESS (Never Fails)
 */
static int_t scifa_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *p_ver_info)
{
    (void) stream_ptr;

    p_ver_info->hld.version.sub.major = gs_hld_info.version.sub.major;
    p_ver_info->hld.version.sub.minor = gs_hld_info.version.sub.minor;
    p_ver_info->hld.build = gs_hld_info.build;
    p_ver_info->hld.p_szdriver_name = gs_hld_info.p_szdriver_name;

    /* Obtain version information from Low Layer Driver */
    R_SCIFA_GetVersion(p_ver_info);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function scifa_hld_get_version
 *********************************************************************************************************************/

/**
 * @brief Acquires and validates the smart configuration index from a given stream_ptr
 *
 * This function safely returns a configuration index if it is
 * supported by the driver, otherwise it returns -1
 *
 * @param stream_ptr Pointer to the file stream
 *
 * @retval  N Where N is the configuration index
 * @retval -1 DRV_ERROR
 */
static r_sc_index_t scifa_get_config_index (st_stream_ptr_t stream_ptr)
{
    r_channel_t channel_id;
    r_sc_index_t sc_config_index;

    /* return error if not valid stream pointer */
    if (NULL == stream_ptr)
    {
        return (DRV_ERROR);
    }

    /* get channel associated with configuration */
    sc_config_index = (int_t) (stream_ptr->sc_config_index);
    channel_id = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel_id)
    {
        return (DRV_ERROR);
    }

    /* verify that the channel is supported */
    /* look for channel in the map of available channels from low level driver */
    /* (e_channel_id_t). This is a bit field where the power of 2 is the available channel */
    if (((1 << channel_id) <= R_CH15))
    {
        /* set channel as bit field where value is 2 << channel */
        uint16_t channel_bit_value = (uint16_t) (1 << channel_id);

        /* get bit field of supported channels */
        uint16_t supported_channels = SCIFA_CFG_LLD_SUPPORTED_CHANNELS;

        if (0 != (channel_bit_value & supported_channels))
        {
            /* channel is supported */
            return (sc_config_index);
        }
    }

    return (DRV_ERROR);
}
/**********************************************************************************************************************
 * End of Function scifa_get_config_index
 *********************************************************************************************************************/

/**********************************************************************************************************************
 End Of File
 *********************************************************************************************************************/
