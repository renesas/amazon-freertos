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
 * @file         r_adc_drv_api.c
 * @version      1.0
 * @description  ADC high layer device driver
 **********************************************************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 19.02.2019 1.00    First Release
 *              : 20.06.2019 1.01    Update in response to change to cbuffer interface
 *********************************************************************************************************************/


/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "compiler_settings.h"
#include "driver.h"
#include "r_adc_drv_api.h"
#include "r_adc_drv_link.h"    /* Link include to Low Layer Driver */

#include "r_cbuffer.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define R_ADC_PRV_BUFF_SIZE             (8192)
#define NOT_USED_PRV_(p)                     ((void)(p))

/* Comment this line out to turn ON module trace in this file */
#undef _TRACE_ON_

#ifndef _TRACE_ON_
    #undef TRACE
    #define TRACE(x)
#endif

/******************************************************************************
 Typedefs
 ******************************************************************************/

typedef struct
{
    bool_t driver_open;
    st_pcbuff_t p_conversion_cbuff;
    void (*p_userCallback)(e_r_drv_adc_callback_reason_t reason);
    uint_t conversion_data_size;
} st_adc_internal_configuration_t;

/**********************************************************************************************************************
 Function Prototypes
 *********************************************************************************************************************/

static int_t adc_hld_open (st_stream_ptr_t pStream);
static void adc_hld_close (st_stream_ptr_t pStream);
static int_t adc_hld_read (st_stream_ptr_t p_stream, uint8_t *p_buffer, uint32_t count);
static int_t adc_hld_control (st_stream_ptr_t pStream, uint32_t ctlCode, void *pCtlStruct);
static int_t adc_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo);

static void adc_hld_interrupt_handler (uint32_t sense);
static void set_conversion_data_size (void);

/**********************************************************************************************************************
 Constant Data
 *********************************************************************************************************************/

static const st_drv_info_t s_hld_info =
{
    {
        ((R_DRV_ADC_HLD_VERSION_MAJOR << 16) + R_DRV_ADC_HLD_VERSION_MINOR)
    },
    R_DRV_ADC_HLD_BUILD_NUM,
    R_DRV_ADC_HLD_DRV_NAME
};

/**********************************************************************************************************************
 Private variables
 *********************************************************************************************************************/

static st_adc_internal_configuration_t s_adc_drv_config =
{
    false,          // driver not open
    NULL,           // circular buffer
    NULL,           // user callback
    0u              // conversion data size
};

/**********************************************************************************************************************
 Global variables
 *********************************************************************************************************************/

/* Define the driver function table for this implementation */
const st_r_driver_t g_adc_driver =
{
    "ADC Device Driver",
    adc_hld_open,
    adc_hld_close,
    adc_hld_read,
    no_dev_write,
    adc_hld_control,
    adc_hld_get_version
};

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private Functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_hld_open
 * Description  : Open the ADC device driver
 * Arguments    : p_stream[in] - pointer to the file stream
 * Return Value : DRV_SUCCESS or DRV_ERROR
 *********************************************************************************************************************/
static int_t adc_hld_open(st_stream_ptr_t p_stream)
{
    /* Stream not used */
    NOT_USED_PRV_(p_stream);

    /* can only have one instance of this driver open */
    if (s_adc_drv_config.driver_open)
    {
        return DRV_ERROR;
    }

    /* initialise the ADC peripheral */
    if (R_ADC_HWInitialise() != ADC_SUCCESS)
    {
        return DRV_ERROR;
    }

    /* load the initial ADC configuration and get the configured user callback function */
    if (R_ADC_LoadInitialConfiguration(&s_adc_drv_config.p_userCallback) != ADC_SUCCESS)
    {
        /* set the size of the conversion data packet */
        set_conversion_data_size();
        return DRV_ERROR;
    }

    /* set up interrupts and the conversion complete interrupt handler */
    if (R_ADC_ConfigureInterrupts(adc_hld_interrupt_handler) != ADC_SUCCESS)
    {
        return DRV_ERROR;
    }

    /* allocate circular buffer for conversion data */
    s_adc_drv_config.p_conversion_cbuff = cbCreate(R_ADC_PRV_BUFF_SIZE);

    if (NULL == s_adc_drv_config.p_conversion_cbuff)
    {
        return DRV_ERROR;
    }

    /* driver successfully opened */
    s_adc_drv_config.driver_open = true;

    /* return status */
    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 End of function adc_hld_open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_hld_close
 * Description  : close the ADC device driver
 * Arguments    : p_stream[in] - pointer to the file stream
 * Return Value : None
 *********************************************************************************************************************/
static void adc_hld_close(st_stream_ptr_t p_stream)
{
    /* stream not used */
    NOT_USED_PRV_(p_stream);

    if (s_adc_drv_config.driver_open)
    {
        /* un-initialise the hardware */
        R_ADC_HWUninitialise();

        s_adc_drv_config.driver_open = false;

        /* destroy the conversion buffer */
        cbDestroy(s_adc_drv_config.p_conversion_cbuff);
        s_adc_drv_config.p_conversion_cbuff = NULL;

        /* unregister the interrupt handler */
        R_INTC_Disable(INTC_ID_ADC_S12ADI0);
        R_INTC_RegistIntFunc(INTC_ID_ADC_S12ADI0, NULL);
    }
}
/**********************************************************************************************************************
 End of function adc_hld_close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_hld_read
 * Description  : Reads ADC conversion data
 * Arguments    : p_stream[in] - the file stream that contains the desired ADC unit
 *                p_buffer[out] - the buffer to be populated with the data read
 *                count[in] - the size of p_buffer
 * Return Value : >= 0 (the number of bytes read) or DRV_ERROR on failure
 *********************************************************************************************************************/
static int_t adc_hld_read(st_stream_ptr_t p_stream, uint8_t *p_buffer, uint32_t count)
{
    /* stream not used */
    NOT_USED_PRV_(p_stream);
    uint32_t i;
    int_t bytes_read = 0;

    /* ensure that count is a multiple of the packet size in order to prevent */
    /* misalignment issues with the conversion data packets */
    if ((count % (uint32_t) s_adc_drv_config.conversion_data_size) != 0)
    {
        return DRV_ERROR;
    }

    /* don't allow the receive interrupt routine to change the buffer while we're accessing it */
    R_COMPILER_DisableInterrupts();

    /* copy data from the circular buffer to the supplied buffer */
    for (i = 0; i < count; i++)
    {
        if (cbGet(s_adc_drv_config.p_conversion_cbuff, &p_buffer[bytes_read]))
        {
            bytes_read++;
        }
        else
        {
            /* break out of loop */
            i = count;
        }
    }

    /* we've finished with the buffer */
    R_COMPILER_EnableInterrupts();

    return bytes_read;
}
/**********************************************************************************************************************
 End of function adc_hld_read
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_hld_control
 * Description  : handles custom controls for the ADC device driver
 * Arguments    : p_stream[in] - pointer to the file stream
 *                ctl_code[in] - the custom control code
 * Return Value : DRV_SUCCESS or DRV_ERROR
 *********************************************************************************************************************/
static int_t adc_hld_control(st_stream_ptr_t p_stream, uint32_t ctl_code, void *p_ctl_struct)
{
    /* Stream not used */
    NOT_USED_PRV_(p_stream);
    st_r_drv_adc_config_t *p_configuration;
    e_r_drv_adc_err_t adc_err;
    int_t ret_value = DRV_ERROR;

    /* cast to e_r_drv_ctrl_code_adc_t */
    switch ((e_r_drv_ctrl_code_adc_t) ctl_code)
    {
        /* ADC conversion software trigger */
        case CTL_ADC_SOFTWARE_TRIGGER:
        {
            /* trigger the conversion */
            R_ADC_SoftwareTrigger();

            /* if a callback wasn't specified, then we need to block here until the scan is complete */
            if (NULL == s_adc_drv_config.p_userCallback)
            {
                while (!R_ADC_IsConversionComplete())
                {
                    R_COMPILER_Nop();
                }
            }

            ret_value = DRV_SUCCESS;
            break;
        }

        /* Set ADC configuration */
        case CTL_ADC_SET_CONFIGURATION:
        {
            if (NULL != p_ctl_struct)
            {
                /* assign new pointer for readability */
                p_configuration = (st_r_drv_adc_config_t *) p_ctl_struct;

                adc_err = R_ADC_SetConfiguration(&p_configuration->config);
                p_configuration->err = adc_err;

                /* set pins to analogue inputs */
                if (ADC_SUCCESS == adc_err)
                {
                    adc_err = R_ADC_ConfigureAnalogueInputs();
                }

                if (ADC_SUCCESS == adc_err)
                {
                    s_adc_drv_config.p_userCallback = p_configuration->config.p_adcCallback;

                    /* clear the circular buffer */
                    R_COMPILER_DisableInterrupts();
                    cbClear(s_adc_drv_config.p_conversion_cbuff);
                    R_COMPILER_EnableInterrupts();

                    /* update the size of the conversion data packet */
                    set_conversion_data_size();

                    ret_value = DRV_SUCCESS;
                }
                else
                {
                    ret_value = DRV_ERROR;
                }
            }
            break;
        }

        /* Get ADC configuration */
        case CTL_ADC_GET_CONFIGURATION:
        {
            if (NULL != p_ctl_struct)
            {
                /* assign new pointer for readability */
                p_configuration = (st_r_drv_adc_config_t *) p_ctl_struct;

                adc_err = R_ADC_GetConfiguration(&p_configuration->config);
                p_configuration->config.p_adcCallback = s_adc_drv_config.p_userCallback;
                p_configuration->err = adc_err;

                ret_value = (ADC_SUCCESS == adc_err) ? DRV_SUCCESS : DRV_ERROR;
            }
            break;
        }

        default:
        {
            TRACE(("ADC Driver: Unknown control code\r\n"));
            break;
        }
    }

    return (ret_value);
}
/**********************************************************************************************************************
 End of function adc_hld_control
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_hld_get_version
 * Description  : provides build information even if driver fails to open
 * Arguments    : stream_ptr[in] - pointer to the file stream
 * Return Value : always returns DRV_SUCCESS
 *********************************************************************************************************************/
static int_t adc_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *p_ver_info)
{
    /* stream not used */
    NOT_USED_PRV_(stream_ptr);

    p_ver_info->hld.version.sub.major = s_hld_info.version.sub.major;
    p_ver_info->hld.version.sub.minor = s_hld_info.version.sub.minor;
    p_ver_info->hld.build = s_hld_info.build;
    p_ver_info->hld.p_szdriver_name = s_hld_info.p_szdriver_name;

    /* Obtain version information from Low Layer Driver */
    R_ADC_GetVersion(&p_ver_info->lld);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 End of function adc_hld_get_version
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_conversion_data_size
 * Description  : Sets the size of the conversion data packet that will be added
 *                to the conversions buffer following every conversion scan
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
static void set_conversion_data_size(void)
{
    uint_t channels_enabled;
    e_r_drv_adc_channel_t channel;
    bool_t enabled;
    uint8_t conversion_width;

    /* get the conversion width from the Low Layer Driver */
    R_ADC_GetConversionWidth(&conversion_width);

    channels_enabled = 0;
    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        R_ADC_IsChannelEnabled(channel, &enabled);

        if (enabled)
        {
            channels_enabled++;
        }
    }

    /* calculate the number of bytes that are needed to store the conversion data */
    s_adc_drv_config.conversion_data_size = channels_enabled * conversion_width;
}
/**********************************************************************************************************************
 End of function set_conversion_data_size
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_hld_interrupt_handler
 * Description  : conversion complete interrupt handler
 * Arguments    : sense[in] - not used
 * Return Value : none
 *********************************************************************************************************************/
static void adc_hld_interrupt_handler(uint32_t sense)
{
    /* Sense not used */
    NOT_USED_PRV_(sense);
    e_r_drv_adc_channel_t channel;
    uint32_t conversion;
    bool_t enabled;
    uint8_t conversion_width;
    uint_t i;
    uint8_t byte;

    int32_t cb_packet = cbFree(s_adc_drv_config.p_conversion_cbuff);
    if (cb_packet < 0)
    {
    	/* Internal error witn cbFree() so abort operation */
    	return;
    }

    /* get the conversion width from the Low Layer Driver */
    R_ADC_GetConversionWidth(&conversion_width);

    /* check for space in the circular buffer */
    /* if there is not enough space for a conversion packet, then */
    /* delete a conversion packet from the start of the buffer */
    if ((uint32_t)cb_packet < s_adc_drv_config.conversion_data_size)
    {
        /* discard one packet from the buffer */
        for (i = 0; i < s_adc_drv_config.conversion_data_size; i++)
        {
            cbGet(s_adc_drv_config.p_conversion_cbuff, &byte);
        }
    }

    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        R_ADC_IsChannelEnabled(channel, &enabled);

        if (enabled)
        {
            /* get the conversion value from the Low Layer Driver */
            R_ADC_ReadConversion(channel, &conversion);

            /* copy the conversion value to the circular buffer */
            for (i = 0; i < conversion_width; i++)
            {
                /* cast to uint8_t */
                cbPut(s_adc_drv_config.p_conversion_cbuff, (uint8_t) (conversion));
                conversion >>= 8;
            }
        }
    }

    /* if a user callback has been configured, then call it here */
    if (NULL != s_adc_drv_config.p_userCallback)
    {
        (*s_adc_drv_config.p_userCallback)(ADC_REASON_SCAN_COMPLETE);
    }
}
/**********************************************************************************************************************
 End of function adc_hld_interrupt_handler
 *********************************************************************************************************************/

/**********************************************************************************************************************
 End Of File
 *********************************************************************************************************************/
