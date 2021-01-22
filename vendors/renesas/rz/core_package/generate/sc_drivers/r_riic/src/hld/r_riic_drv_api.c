/******************************************************************************
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
 * and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * File Name    : r_riic_drv_api.c
 * Version      : See History
 * Description  : I2C high level device driver
 ******************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 06.02.2018 1.00    First Release
 *              : 08.05.2019 1.01    Fixed minor issue in riic_hld_open fn
 *              : 01.08.2019 1.03    Bump version number for V5 release
 *                                   Get clock speed from CPG driver
 *              : 30.06.2020 1.04    Fixed transfer end interrupt enable
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_riic_drv_api.h"
#include "r_riic_drv_sc_cfg.h"

/* #include "trace.h" */
#include "r_riic_hld_prv.h"
#include "r_riic_drv_link.h"    /* Link include to low level driver */
#include "driver.h"
#include "r_os_abstraction_api.h"
#include "r_cpg_drv_api.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/* Comment this line out to turn ON module trace in this file */
/* #undef _TRACE_ON_

 #ifndef _TRACE_ON_
 #undef TRACE
 #define TRACE(x)
 #endif */

/*****************************************************************************
 Function Prototypes
 ******************************************************************************/

static int_t riic_hld_open (st_stream_ptr_t pStream);
static void riic_hld_close (st_stream_ptr_t pStream);
static int_t riic_hld_control (st_stream_ptr_t pStream, uint32_t ctlCode, void *pCtlStruct);
static int_t riic_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo);
static int_t riic_get_channel (st_stream_ptr_t stream_ptr);
static void riic_reset (int_t channel);
static int_t riic_get_file_mode (st_stream_ptr_t stream_ptr);
static int_t validate_config_struct (st_riic_config_t *p_config_struct);

/*****************************************************************************
 Constant Data
 ******************************************************************************/

static const st_drv_info_t gs_hld_info =
{
    {
        ((STDIO_RIIC_HLD_VERSION_MAJOR << 16) + STDIO_RIIC_HLD_VERSION_MINOR)
    },
    STDIO_RIIC_HLD_BUILD_NUM,
    STDIO_RIIC_HLD_DRV_NAME
};

static bool_t gs_driver_open = false;
static void *gsp_riic_control_mutex[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile int_t gs_channel_count[RIIC_CFG_LLD_NUM_CHANNELS];

/* Define the driver function table for this implementation */
const st_r_driver_t g_riic_driver =
{ "I2C Device Driver", riic_hld_open, riic_hld_close, no_dev_read, no_dev_write, riic_hld_control, riic_hld_get_version };

/******************************************************************************
 Private global variables
 ******************************************************************************/

/*****************************************************************************
 Private Functions
 ******************************************************************************/

/**
 * @fn             get_p1clk
 * @brief          Interrogate the CPG driver to get the P1 clock frequency
 * @param[out]     p1clk - the P1 clock frequency in KHz
 * @retval         DRV_ERROR or DRV_SUCCESS
 */
static int_t get_p1clk(float64_t *p1clk)
{
    int_t cpg_handle;
    int_t retval;
    st_r_drv_cpg_get_clk_t cpg_get_clock_t;

    /* open the CPG driver */
    cpg_handle = direct_open("cpg", O_RDWR);
    if (cpg_handle < 0)
    {
        return (DRV_ERROR);
    }

    /* interrogate the CPG driver for the P1 clock frequency */
    cpg_get_clock_t.freq_src = CPG_FREQ_P1CLK;
    retval = control(cpg_handle, CTL_CPG_GET_CLK, &cpg_get_clock_t);

    *p1clk = cpg_get_clock_t.clk_frequency_khz;

    /* close the CPG driver */
    direct_close(cpg_handle);

    return (retval);
}
/******************************************************************************
 * End of function get_p1clk
 ******************************************************************************/

/******************************************************************************
 Function Name: riic_hld_open
 Description:   Function to open the I2C device driver
 Arguments:     IN pStream - Pointer to the file stream
 Return value:  0 for success otherwise -1
 ******************************************************************************/
static int_t riic_hld_open (st_stream_ptr_t p_stream)
{
    float64_t p1clk;
    int_t ret_value = DRV_ERROR;

    /* determine hardware channel number */
    int_t channel = riic_get_channel(p_stream);

    if (DRV_ERROR != channel)
    {
        /* see if channel is supported */
        ret_value = validate_channel(channel);

        if (DRV_ERROR != ret_value)
        {
            /* initialise if first entry */
            if (!gs_driver_open)
            {
                /* initialise channel status */
                uint32_t loop = 0u;
                for (loop = 0; loop < RIIC_CFG_LLD_NUM_CHANNELS; loop++)
                {
                    gs_channel_count[loop] = 0;
                    gsp_riic_control_mutex[loop] = NULL;
                }

                gs_driver_open = true;
            }

            if (0 == gs_channel_count[channel])
            {
                /* Channel is not open. Implement SC configuration and open */

                /* setup access mutex */
                R_OS_EnterCritical();
                gsp_riic_control_mutex[channel] = R_OS_MutexCreate();
                if (NULL == gsp_riic_control_mutex[channel])
                {
                    R_OS_ExitCritical();
                    ret_value = DRV_ERROR;
                }
                else
                {
                    R_OS_MutexAcquire(gsp_riic_control_mutex[channel]);
                    R_OS_ExitCritical();

                    if (DRV_SUCCESS == ret_value)
                    {
                        ret_value = get_p1clk(&p1clk);
                    }

                    if (DRV_SUCCESS == ret_value)
                    {
                        /* apply SC configuration on open of channel */
                        ret_value = R_RIIC_ScInitChannel(p_stream->sc_config_index,
                                (uint32_t) (p1clk * 1000));
                    }

                    if (DRV_SUCCESS == ret_value)
                    {
                        /* open channel */
                        ret_value = open_channel(channel);
                    }

                    if (DRV_SUCCESS == ret_value)
                    {
                        /* increment channel count */
                        gs_channel_count[channel]++;
                    }

                    /* Release driver mutex*/
                    R_OS_MutexRelease(gsp_riic_control_mutex[channel]);
                }
            }
            else
            {
                /* Already open. Increment channel open count */
                gs_channel_count[channel]++;
            }
        }
    }

    /* return status */
    return (ret_value);
}
/******************************************************************************
 End of function riic_hld_open
 ******************************************************************************/

/******************************************************************************
 Function Name: riic_hld_close
 Description:   Function to close the I2C driver
 Arguments:     IN pStream - Pointer to the file stream
 Return value:  none
 ******************************************************************************/
static void riic_hld_close (st_stream_ptr_t p_stream)
{
    /* determine hardware channel number */
    int_t channel = riic_get_channel(p_stream);

    if ((DRV_ERROR != channel) && (NULL != gsp_riic_control_mutex[channel]))
    {
        if (validate_channel(channel) == DRV_SUCCESS)
        {
            /* decrease channel open count */
            gs_channel_count[channel]--;

            /* valid channel. Check it is open */
            if (0 == gs_channel_count[channel])
            {
                /* Ensure single task access to driver operations */
                R_OS_MutexAcquire(gsp_riic_control_mutex[channel]);

                /* Close channel in low layer */
                R_RIIC_CloseChannel(channel);

                /* As there are no more drivers open on this channel, close the mutex */
                R_OS_EnterCritical();
                R_OS_MutexDelete( &gsp_riic_control_mutex[channel]);
                gsp_riic_control_mutex[channel] = NULL;
                R_OS_ExitCritical();
            }
        }
    }
}
/******************************************************************************
 End of function riic_hld_close
 ******************************************************************************/

/******************************************************************************
 Function Name: riic_hld_control
 Description:   Function to handle custom controls for the I2C device driver
 Arguments:     IN  p_stream - Pointer to the file stream
 IN  ctl_code - The custom control code
 IN  p_ctl_struct - Pointer to the custom control structure
 Return value:  DRV_SUCCESS for success DRV_ERROR on error
 ******************************************************************************/
static int_t riic_hld_control (st_stream_ptr_t p_str, uint32_t ctl_code, void *p_ctl_struct)
{
    int_t ret_value = DRV_ERROR;
    int_t file_mode;

    /* get channel number */
    int_t channel = riic_get_channel(p_str);

    /* fail if pStream has wrong name */
    if ((DRV_ERROR) == channel)
    {
        return (ret_value);
    }

    /* get R/W setting */
    file_mode = riic_get_file_mode(p_str);

    /* fail if the channel is not open */
    if (0 >= gs_channel_count[channel])
    {
        return (ret_value);
    }

    if (p_ctl_struct)
    {
        /* Ensure single task access to driver operations */
        R_OS_MutexAcquire(gsp_riic_control_mutex[channel]);

        ret_value = DRV_ERROR;

        switch (ctl_code)
        {
            case CTL_RIIC_SET_CONFIG:
            {
                /*!< Set channel configuration */

                /* make sure that the parameters are valid */
                ret_value = validate_config_struct((st_riic_config_t *) p_ctl_struct);

                if (DRV_SUCCESS == ret_value)
                {
                    /* cast to configuration structure */
                    ret_value = R_RIIC_SetConfig(channel, (st_riic_config_t *) p_ctl_struct);
                }
            }
            break;

            case CTL_RIIC_GET_CONFIG:
            {
                /*!< Get channel configuration */
                ret_value = R_RIIC_GetConfig(channel, (st_riic_config_t *) p_ctl_struct);
            }
            break;

            case CTL_RIIC_READ:
            {
                /*!< Read from RIIC device */

                /* Ensure correct access mode */
                if ((file_mode & __SRD) > 0)
                {
                    /* cast control pointer to control struct */
                    ret_value = read_data(channel, (st_r_drv_riic_transfer_t *) p_ctl_struct);

                    if (DRV_SUCCESS != ret_value)
                    {
                        riic_reset(channel);
                    }
                }
            }
            break;

            case CTL_RIIC_WRITE:
            {
                /*!< Write to IIC device */

                /* Ensure correct access mode */
                if ((file_mode & __SWR) > 0)
                {
                    /* cast control pointer to control struct */
                    ret_value = write_data(channel, (st_r_drv_riic_transfer_t *) p_ctl_struct);

                    if (DRV_SUCCESS != ret_value)
                    {
                        riic_reset(channel);
                    }
                }
            }
            break;

            default:
            {
                ret_value = DRV_ERROR;
                break;
            }
        }

        /* Release driver */
        R_OS_MutexRelease(gsp_riic_control_mutex[channel]);
    }

    return (ret_value);
}
/******************************************************************************
 End of function  riic_hld_control
 ******************************************************************************/

/*******************************************************************************
 * Function Name: riic_hld_get_version
 * Description  : Provides build information even if driver fails to open
 *                version information is updated by developer
 * Arguments    : none
 * Return Value : DRV_SUCCESS (never fails)
 ******************************************************************************/
static int_t riic_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *p_ver_info)
{
    /* Unused argument */
    (void) stream_ptr;

    p_ver_info->hld.version.sub.major = gs_hld_info.version.sub.major;
    p_ver_info->hld.version.sub.minor = gs_hld_info.version.sub.minor;
    p_ver_info->hld.build = gs_hld_info.build;
    p_ver_info->hld.p_szdriver_name = gs_hld_info.p_szdriver_name;

    /* Obtain version information from Low layer Driver */
    R_RIIC_GetVersion(p_ver_info);

    return (DRV_SUCCESS);
}
/*******************************************************************************
 End of function riic_hld_get_version
 ******************************************************************************/

/*******************************************************************************
 * Function Name: riic_get_channel
 * Description  : Provides channel number written in stream_ptr
 * Arguments    : IN pStream - Pointer to the file stream
 * Return value:  0 to 3 for channel number -1 on error
 ******************************************************************************/
static int_t riic_get_channel (st_stream_ptr_t stream_ptr)
{
    int_t ret_value = DRV_ERROR;
    int_t channel_id;

    if (NULL != stream_ptr)
    {
        /* cast channel id to an int_t */
        channel_id = (int_t) (RIIC_SC_TABLE[stream_ptr->sc_config_index].channel);

        /* verify that the channel is supported */

        /* look for channel in the conglomeration  of available channels from low level driver */
        /* (e_channel_id_t). This is a bitfield where the power of 2 is the available channel */
        if (((1 << channel_id) <= R_CH15))
        {
            /* set channel as bitfield where value is 2 << channel */
            int_t test1 = (1 << channel_id);

            /* get bitfield of supported channels */
            int_t test2 = RIIC_CFG_LLD_SUPPORTED_CHANNELS;
            if (0 != (test1 & test2))
            {
                /* Channel is supported */
                ret_value = (int_t) channel_id;
            }
            else
            {
                /* Channel is supported */
                ret_value = DRV_ERROR;
            }
        }
    }

    /* return status as error or channel number  */
    return (ret_value);
}
/*******************************************************************************
 End of function riic_get_channel
 ******************************************************************************/

/**
 * @brief Reset Driver Hardware configuration
 * @param[in] channel : the device specific channel number (in RIIC_CFG_LLD_SUPPORTED_CHANNELS)
 **/
static void riic_reset (int_t channel)
{
    st_riic_config_t temp_cfg;

    /* Get current configuration. Ignore return status */
    R_RIIC_GetConfig(channel, &temp_cfg);
    R_RIIC_SetConfig(channel, &temp_cfg);
}
/*******************************************************************************
 End of function riic_reset
 ******************************************************************************/

/**
 * @brief Determine if the specified RIIC channel is valid
 * @param[in] channel : the device specific channel number (in RIIC_CFG_LLD_SUPPORTED_CHANNELS)
 * @return    DRV_SUCCESS       : Success of RIIC operation
 *            DRV_ERROR         : Failure of RIIC operation
 **/
int_t validate_channel (int_t channel)
{
    int_t ret_value = DRV_ERROR;

    /* test if the channel is present */
    if (0 != ((1 << channel) & RIIC_CFG_LLD_SUPPORTED_CHANNELS))
    {
        ret_value = DRV_SUCCESS;
    }

    /* return status */
    return (ret_value);
}
/*******************************************************************************
 End of function validate_channel
 ******************************************************************************/

/**
 * @brief Acquires the file_mode from the given st_stream_ptr_t
 *
 * This function is particularly used in the open function,
 * where we need to only read and/or write to configure the RIIC
 * for reception and/or transmission.
 *
 * @param stream_ptr Pointer to the file stream structure.

 * @retval  N Where N is the current file mode.
 * @retval -1 DRV_ERROR
 */
static int_t riic_get_file_mode (st_stream_ptr_t stream_ptr)
{
    int_t ret_value = DRV_ERROR;
    int_t file_mode;

    /* If valid stream pointer */
    if (NULL != stream_ptr)
    {
        /* Get file mode */
        file_mode = stream_ptr->file_mode;

        /* Check access permissions */
        if ((file_mode & (__SRD | __SWR)) > 0)
        {
            ret_value = file_mode;
        }
    }

    return (ret_value);
}
/******************************************************************************
 * End of Function riic_get_file_mode
 ******************************************************************************/

/**
 * @brief validates configuration structure parameters
 *
 * This function validates the parameters in a configuration structure
 *
 * @param p_config_struct Pointer to the configuration structure.

 * @retval DRV_SUCCESS - valid
 * @retval DRV_ERROR - not valid
 */
static int_t validate_config_struct (st_riic_config_t *p_config_struct)
{
    int_t error = DRV_SUCCESS;
    uint32_t loop = 0u;

    if ((RIIC_MODE_MASTER > p_config_struct->riic_mode) || (RIIC_MODE_MAX <= p_config_struct->riic_mode))
    {
        error = DRV_ERROR;
    }

    for (loop = 0u; loop < RIIC_SLAVE_ADDR_NUM_MAX; loop++)
    {
        if ((false != p_config_struct->slave_address_enable[loop])
                && (true != p_config_struct->slave_address_enable[loop]))
        {
            error = DRV_ERROR;
        }

        if ((RIIC_DEVICE_ADDR_7 > p_config_struct->slave_address_length[loop])
                || (RIIC_DEVICE_ADDR_MAX <= p_config_struct->slave_address_length[loop]))
        {
            error = DRV_ERROR;
        }
    }

    if ((RIIC_FREQUENCY_100KHZ > p_config_struct->frequency) || (RIIC_FREQUENCY_MAX <= p_config_struct->frequency))
    {
        error = DRV_ERROR;
    }

    if ((RIIC_DUTY_10 > p_config_struct->duty) || (RIIC_DUTY_MAX <= p_config_struct->duty))
    {
        error = DRV_ERROR;
    }

    if ((RIIC_FILTER_NOT_USED > p_config_struct->noise_filter_stage)
            || (RIIC_FILTER_STAGE_MAX <= p_config_struct->noise_filter_stage))
    {
        error = DRV_ERROR;
    }

    if ((RIIC_TIMEOUT_NOT_USED > p_config_struct->timeout) || (RIIC_TIMEOUT_MAX <= p_config_struct->timeout))
    {
        error = DRV_ERROR;
    }

    if ((RIIC_FORMAT_I2C > p_config_struct->format) || (RIIC_FORMAT_MAX <= p_config_struct->format))
    {
        error = DRV_ERROR;
    }

    if ((false != p_config_struct->host_address_enabled) && (true != p_config_struct->host_address_enabled))
    {
        error = DRV_ERROR;
    }

    return (error);
}
/*******************************************************************************
 End of function validate_config_struct
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/
