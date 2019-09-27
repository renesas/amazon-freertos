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
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name     : r_adc_lld_rza2m.c
 * Device(s)     : RZ/A2M
 * Tool-Chain    : GCC Arm Embedded 6.3.1
 * H/W Platform  : RZ/A2M EBK board
 * Description   : ADC driver
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 19.02.2019 1.00
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <unistd.h>

#include "rza_io_regrw.h"
#include "iodefine.h"
#include "iobitmask.h"

#include "r_stb_drv_api.h"

#include "r_adc_drv_link.h"

#include "r_gpio_drv_api.h"
#include "r_compiler_abstraction_api.h"
#include "r_adc_drv_sc_cfg.h"

#include "driver.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define R_ADC_PRV_ADCER_ADC_RESOLUTION_12           (0u)
#define R_ADC_PRV_ADCER_ADC_RESOLUTION_10           (1u)
#define R_ADC_PRV_ADCER_ADC_RESOLUTION_8            (2u)

#define R_ADC_PRV_ADCER_ADC_DATA_FORMAT_RIGHT_ALIGN (0u)
#define R_ADC_PRV_ADCER_ADC_DATA_FORMAT_LEFT_ALIGN  (1u)

#define R_ADC_PRV_ADCSR_ADC_SCAN_MODE_SINGLE        (0u)
#define R_ADC_PRV_ADCSR_ADC_SCAN_MODE_GROUP         (1u)
#define R_ADC_PRV_ADCSR_ADC_SCAN_MODE_CONTINUOUS    (2u)

#define R_ADC_PRV_ADC_MINIMUM_SAMPLE_TIME           (5u)
#define R_ADC_PRV_ADC_MAXIMUM_SAMPLE_TIME           (255u)

#define R_ADC_PRV_DEFAULT_INTERRUPT_PRIORITY        (INTC_PRIORITY_20)

/**********************************************************************************************************************
 Enumerated Types
 *********************************************************************************************************************/


/**********************************************************************************************************************
 Structures
 *********************************************************************************************************************/


/**********************************************************************************************************************
 Private variables
 *********************************************************************************************************************/

static const st_drv_info_t s_lld_info =
{
    {
        ((R_DRV_ADC_LLD_VERSION_MAJOR << 16) + R_DRV_ADC_LLD_VERSION_MINOR)
    },
    R_DRV_ADC_LLD_BUILD_NUM,
    R_DRV_ADC_LLD_DRV_NAME
};

/**********************************************************************************************************************
 Private functions
 *********************************************************************************************************************/

static void adc_set_resolution (e_r_drv_adc_resolution_t resolution);
static void adc_set_data_alignment (e_r_drv_adc_alignment_t alignment);
static void adc_set_scan_mode (e_r_drv_adc_scan_mode_t scan_mode);
static bool_t is_valid_configuration (st_r_drv_adc_unit_config_t * p_config);
static e_r_drv_adc_resolution_t adc_get_resolution (void);
static e_r_drv_adc_alignment_t adc_get_data_alignment (void);
static e_r_drv_adc_scan_mode_t adc_get_scan_mode (void);

/**********************************************************************************************************************
 Global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_GetVersion
 * Description  : Gets the version number of this Low Layer Driver
 * Arguments    : pinfo[out] - returns the driver information
 * Return Value : DRV_SUCCESS always returned
 *********************************************************************************************************************/
uint32_t R_ADC_GetVersion(st_drv_info_t *pinfo)
{
    pinfo->version.sub.major = s_lld_info.version.sub.major;
    pinfo->version.sub.minor = s_lld_info.version.sub.minor;
    pinfo->build = s_lld_info.build;
    pinfo->p_szdriver_name = s_lld_info.p_szdriver_name;

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of function R_ADC_GetVersion
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_HWInitialise
 * Description  : Initialise the hardware
 * Arguments    : None
 * Return Value : ADC_ERR_FAILED or ADC_SUCCESS
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_HWInitialise(void)
{
    e_r_drv_adc_channel_t channel;
    int_t delay;

    e_stb_err_t stb_return;

    /* enable the ADC module */
    stb_return = R_STB_StartModule(MODULE_ADC);

    if (STB_SUCCESS != stb_return)
    {
        return ADC_ERR_FAILED;
    }

    /* a delay to allow the converter to settle */
    for (delay = 0; delay < 10000; delay++)
    {
        R_COMPILER_Nop();
    }

    /* write zero to all registers */
    RZA_IO_RegWrite_16(&ADC.ADCSR.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADANSA0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADADS0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_8(&ADC.ADADC.BYTE, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADCER.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADSTRGR.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADANSB0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADDBLDR.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADRD.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_8(&ADC.ADDISCR.BYTE, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADGSPCR.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADDBLDRA.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADDBLDRB.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADCMPCR.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADCMPANSR0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADCMPLR0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADCMPDR0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADCMPDR1.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADCMPSR0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_8(&ADC.ADCMPBNSR.BYTE, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADWINLLB.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADWINULB.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_16(&ADC.ADANSC0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegWrite_8(&ADC.ADGCTRGR.BYTE, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* A/D Sampling State Register n */
    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        RZA_IO_RegWrite_8((&ADC.ADSSTR0.BYTE) + channel, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_HWInitialise
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_LoadInitialConfiguration
 * Description  : Load the initial configuration from the SC style table
 * Arguments    : p_userCallback[out] - pointer to user callback function pointer
 * Return Value : ADC_SUCCESS or ADC_ERR_INVALID_CFG
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_LoadInitialConfiguration(void (**p_userCallback)(e_r_drv_adc_callback_reason_t reason))
{
    st_r_drv_adc_unit_config_t adc_config;

    /* Smart Configuration style initialisation */
    adc_config.resolution = ADC_SC_TABLE.resolution;
    adc_config.alignment = ADC_SC_TABLE.alignment;
    adc_config.scan_mode = ADC_SC_TABLE.scan_mode;
    adc_config.p_adcCallback = ADC_SC_TABLE.p_adcCallback;
    adc_config.interrupt_priority = ADC_SC_TABLE.interrupt_priority;
    adc_config.channels[ADC_CHANNEL_AN000].sample_time = ADC_SC_TABLE.sample_time_chan_0;
    adc_config.channels[ADC_CHANNEL_AN000].trigger_source = ADC_SC_TABLE.trigger_source_chan_0;
    adc_config.channels[ADC_CHANNEL_AN001].sample_time = ADC_SC_TABLE.sample_time_chan_1;
    adc_config.channels[ADC_CHANNEL_AN001].trigger_source = ADC_SC_TABLE.trigger_source_chan_1;
    adc_config.channels[ADC_CHANNEL_AN002].sample_time = ADC_SC_TABLE.sample_time_chan_2;
    adc_config.channels[ADC_CHANNEL_AN002].trigger_source = ADC_SC_TABLE.trigger_source_chan_2;
    adc_config.channels[ADC_CHANNEL_AN003].sample_time = ADC_SC_TABLE.sample_time_chan_3;
    adc_config.channels[ADC_CHANNEL_AN003].trigger_source = ADC_SC_TABLE.trigger_source_chan_3;
    adc_config.channels[ADC_CHANNEL_AN004].sample_time = ADC_SC_TABLE.sample_time_chan_4;
    adc_config.channels[ADC_CHANNEL_AN004].trigger_source = ADC_SC_TABLE.trigger_source_chan_4;
    adc_config.channels[ADC_CHANNEL_AN005].sample_time = ADC_SC_TABLE.sample_time_chan_5;
    adc_config.channels[ADC_CHANNEL_AN005].trigger_source = ADC_SC_TABLE.trigger_source_chan_5;
    adc_config.channels[ADC_CHANNEL_AN006].sample_time = ADC_SC_TABLE.sample_time_chan_6;
    adc_config.channels[ADC_CHANNEL_AN006].trigger_source = ADC_SC_TABLE.trigger_source_chan_6;
    adc_config.channels[ADC_CHANNEL_AN007].sample_time = ADC_SC_TABLE.sample_time_chan_7;
    adc_config.channels[ADC_CHANNEL_AN007].trigger_source = ADC_SC_TABLE.trigger_source_chan_7;

    if (R_ADC_SetConfiguration(&adc_config) != ADC_SUCCESS)
    {
        return ADC_ERR_INVALID_CFG;
    }

    /* pass the address of the user callback function back to the HLD */
    *p_userCallback = ADC_SC_TABLE.p_adcCallback;

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_LoadInitialConfiguration
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_HWUninitialise
 * Description  : Uninitialise the hardware
 * Arguments    : None
 * Return Value : ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_HWUninitialise(void)
{
    int_t i;

    /* write a 0 to ADST to ensure that the converter is stopped */
    RZA_IO_RegWrite_16(&ADC.ADCSR.WORD, 0, ADC_ADCSR_ADST_SHIFT, ADC_ADCSR_ADST);

    /* wait for the converter to stop */
    for (i = 0; i < 10000; i++)
    {
        R_COMPILER_Nop();
    }

    /* power the module down */
    R_STB_StopModule(MODULE_ADC);

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_HWUninitialise
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_SoftwareTrigger
 * Description  : Trigger an ADC conversion scan
 * Arguments    : None
 * Return Value : ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_SoftwareTrigger(void)
{
    /* write a 0 to the ADCSR register TRGE bit to disable synchronous and asynchronous triggering */
    RZA_IO_RegWrite_16(&ADC.ADCSR.WORD, 0, ADC_ADCSR_TRGE_SHIFT, ADC_ADCSR_TRGE);

    /* write a 1 to the ADCSR register ADST bit to trigger a conversion */
    RZA_IO_RegWrite_16(&ADC.ADCSR.WORD, 1, ADC_ADCSR_ADST_SHIFT, ADC_ADCSR_ADST);

    return ADC_SUCCESS;
}
/******************************************************************************
 * End of function R_ADC_SoftwareTrigger
 ******************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_set_resolution
 * Description  : Sets the register ADC resolution value
 * Arguments    : resolution: The ADC resolution (8, 10 or 12 bits)
 * Return Value : None
 *********************************************************************************************************************/
static void adc_set_resolution(e_r_drv_adc_resolution_t resolution)
{
    uint16_t adprc;

    switch (resolution)
    {
        default:
        case ADC_RESOLUTION_12:
        {
            adprc = R_ADC_PRV_ADCER_ADC_RESOLUTION_12;
            break;
        }

        case ADC_RESOLUTION_10:
        {
            adprc = R_ADC_PRV_ADCER_ADC_RESOLUTION_10;
            break;
        }

        case ADC_RESOLUTION_8:
        {
            adprc = R_ADC_PRV_ADCER_ADC_RESOLUTION_8;
            break;
        }
    }

    RZA_IO_RegWrite_16(&ADC.ADCER.WORD, adprc, ADC_ADCER_ADPRC_SHIFT, ADC_ADCER_ADPRC);
}
/**********************************************************************************************************************
 * End of function adc_set_resolution
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_set_data_alignment
 * Description  : Sets the register ADC data alignment value
 * Arguments    : alignment - The ADC data alignment (left or right)
 * Return Value : None
 *********************************************************************************************************************/
static void adc_set_data_alignment(e_r_drv_adc_alignment_t alignment)
{
    uint16_t adcer;

    switch (alignment)
    {
        case ADC_ALIGN_LEFT:
        {
            adcer = R_ADC_PRV_ADCER_ADC_DATA_FORMAT_LEFT_ALIGN;
            break;
        }

        default:
        case ADC_ALIGN_RIGHT:
        {
            adcer = R_ADC_PRV_ADCER_ADC_DATA_FORMAT_RIGHT_ALIGN;
            break;
        }
    }

    RZA_IO_RegWrite_16(&ADC.ADCER.WORD, adcer, ADC_ADCER_ADRFMT_SHIFT, ADC_ADCER_ADRFMT);
}
/******************************************************************************
 * End of function adc_set_data_alignment
 ******************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_set_scan_mode
 * Description  : Sets the register ADC scan mode value
 * Arguments    : scan_mode - The ADC scan mode (single, group or continuous)
 * Return Value : None
 *********************************************************************************************************************/
static void adc_set_scan_mode(e_r_drv_adc_scan_mode_t scan_mode)
{
    uint16_t adcsr;

    switch (scan_mode)
    {
        default:
        case ADC_SCAN_MODE_SINGLE:
        {
            adcsr = R_ADC_PRV_ADCSR_ADC_SCAN_MODE_SINGLE;
            break;
        }

        case ADC_SCAN_MODE_GROUP:
        {
            adcsr = R_ADC_PRV_ADCSR_ADC_SCAN_MODE_GROUP;
            break;
        }

        case ADC_SCAN_MODE_CONTINUOUS:
        {
            adcsr = R_ADC_PRV_ADCSR_ADC_SCAN_MODE_CONTINUOUS;
            break;
        }
    }

    RZA_IO_RegWrite_16(&ADC.ADCSR.WORD, adcsr, ADC_ADCSR_ADCS_SHIFT, ADC_ADCSR_ADCS);
}
/**********************************************************************************************************************
 * End of function adc_set_scan_mode
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: is_valid_configuration
 * Description  : Validates the configuration settings
 * Arguments    : p_config[in] - pointer to configuration structure
 * Return Value : true if the settings are valid, false otherwise
 *********************************************************************************************************************/
static bool_t is_valid_configuration(st_r_drv_adc_unit_config_t * p_config)
{
    e_r_drv_adc_channel_t channel;

    if ((ADC_RESOLUTION_12 != p_config->resolution) && (ADC_RESOLUTION_10 != p_config->resolution)
            && (ADC_RESOLUTION_8 != p_config->resolution))
    {
        return false;
    }

    if ((ADC_ALIGN_RIGHT != p_config->alignment) && (ADC_ALIGN_LEFT != p_config->alignment))
    {
        return false;
    }

    if ((ADC_SCAN_MODE_SINGLE != p_config->scan_mode) && (ADC_SCAN_MODE_GROUP != p_config->scan_mode)
            && (ADC_SCAN_MODE_CONTINUOUS != p_config->scan_mode))
    {
        return false;
    }

    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        if ((ADC_TRIGGER_DISABLED != p_config->channels[channel].trigger_source) &&
                ((p_config->channels[channel].sample_time < R_ADC_PRV_ADC_MINIMUM_SAMPLE_TIME) ||
                (p_config->channels[channel].sample_time > R_ADC_PRV_ADC_MAXIMUM_SAMPLE_TIME)))
        {
            return false;
        }

        if (p_config->channels[channel].trigger_source > ADC_TRIGGER_GROUPC)
        {
            return false;
        }
    }

    return true;
}
/**********************************************************************************************************************
 * End of function is_valid_configuration
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_SetConfiguration
 * Description  : Set the ADC configuration
 * Arguments    : p_config[in] - pointer to configuration structure
 * Return Value : ADC_SUCCESS or the error code on failure
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_SetConfiguration(st_r_drv_adc_unit_config_t * p_config)
{
    e_r_drv_adc_channel_t channel;
    uint16_t mask;
    uint16_t ansa0;

    if (!is_valid_configuration(p_config))
    {
        return ADC_ERR_INVALID_CFG;
    }

    /* configure settings that apply to all channels */
    adc_set_resolution(p_config->resolution);
    adc_set_data_alignment(p_config->alignment);
    adc_set_scan_mode(p_config->scan_mode);

    R_INTC_SetPriority(INTC_ID_ADC_S12ADI0, p_config->interrupt_priority);

    ansa0 = 0;
    mask = 1;

    /* configure the input channels */
    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        if (ADC_TRIGGER_DISABLED != p_config->channels[channel].trigger_source)
        {
            /* set the channel sample time */
            RZA_IO_RegWrite_8((&ADC.ADSSTR0.BYTE) + channel, (uint8_t) p_config->channels[channel].sample_time,
                    IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
        }

        if (ADC_TRIGGER_NORMAL == p_config->channels[channel].trigger_source)
        {
            ansa0 |= mask;
        }

        /* the compiler promotes the variable to signed integer for the shift, so cast it back */
        mask = (uint16_t) (mask << 1u);
    }

    RZA_IO_RegWrite_16(&ADC.ADANSA0.WORD, ansa0, ADC_ADANSA0_ANSA0_SHIFT, ADC_ADANSA0_ANSA0);

    return ADC_SUCCESS;
}
/*********************************************************************************************************************
 * End of function R_ADC_SetConfiguration
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_get_resolution
 * Description  : Gets the ADC resolution setting
 * Arguments    : None
 * Return Value : The ADC resolution (8, 10 or 12 bits)
 *********************************************************************************************************************/
static e_r_drv_adc_resolution_t adc_get_resolution(void)
{
    uint16_t adprc;
    e_r_drv_adc_resolution_t resolution;

    /* read the ADPRC value from the ADCER register */
    adprc = RZA_IO_RegRead_16(&ADC.ADCER.WORD, ADC_ADCER_ADPRC_SHIFT, ADC_ADCER_ADPRC);

    /* map to our enumerated type */
    switch (adprc)
    {
        default:
        case R_ADC_PRV_ADCER_ADC_RESOLUTION_12:
        {
            resolution = ADC_RESOLUTION_12;
            break;
        }

        case R_ADC_PRV_ADCER_ADC_RESOLUTION_10:
        {
            resolution = ADC_RESOLUTION_10;
            break;
        }

        case R_ADC_PRV_ADCER_ADC_RESOLUTION_8:
        {
            resolution = ADC_RESOLUTION_8;
            break;
        }
    }

    return resolution;
}
/*********************************************************************************************************************
 * End of function adc_get_resolution
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_get_data_alignment
 * Description  : Gets the ADC data alignment setting
 * Arguments    : None
 * Return Value : The ADC data alignment (left or right)
 *********************************************************************************************************************/
static e_r_drv_adc_alignment_t adc_get_data_alignment(void)
{
    uint16_t adrfmt;
    e_r_drv_adc_alignment_t alignment;

    /* read the ADRFMT value from the ADCER register */
    adrfmt = RZA_IO_RegRead_16(&ADC.ADCER.WORD, ADC_ADCER_ADRFMT_SHIFT, ADC_ADCER_ADRFMT);

    /* map to our enumerated type */
    switch (adrfmt)
    {
        case R_ADC_PRV_ADCER_ADC_DATA_FORMAT_LEFT_ALIGN:
        {
            alignment = ADC_ALIGN_LEFT;
            break;
        }

        default:
        case R_ADC_PRV_ADCER_ADC_DATA_FORMAT_RIGHT_ALIGN:
        {
            alignment = ADC_ALIGN_RIGHT;
            break;
        }
    }

    return alignment;
}
/**********************************************************************************************************************
 * End of function adc_get_data_alignment
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_get_scan_mode
 * Description  : Gets the ADC scan mode setting
 * Arguments    : None
 * Return Value : The ADC scan mode (single, group or continuous)
 *********************************************************************************************************************/
static e_r_drv_adc_scan_mode_t adc_get_scan_mode(void)
{
    uint16_t adcs;
    e_r_drv_adc_scan_mode_t scan_mode;

    /* read the ADCS value from the ADCSR register */
    adcs = RZA_IO_RegRead_16(&ADC.ADCSR.WORD, ADC_ADCSR_ADCS_SHIFT, ADC_ADCSR_ADCS);

    switch (adcs)
    {
        default:
        case R_ADC_PRV_ADCSR_ADC_SCAN_MODE_SINGLE:
        {
            scan_mode = ADC_SCAN_MODE_SINGLE;
            break;
        }

        case R_ADC_PRV_ADCSR_ADC_SCAN_MODE_GROUP:
        {
            scan_mode = ADC_SCAN_MODE_GROUP;
            break;
        }

        case R_ADC_PRV_ADCSR_ADC_SCAN_MODE_CONTINUOUS:
        {
            scan_mode = ADC_SCAN_MODE_CONTINUOUS;
            break;
        }
    }

    return scan_mode;
}
/**********************************************************************************************************************
 * End of function adc_get_scan_mode
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_GetConfiguration
 * Description  : Get the ADC configuration
 * Arguments    : p_config[in] - pointer to configuration structure
 * Return Value : ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_GetConfiguration(st_r_drv_adc_unit_config_t * p_config)
{
    uint16_t ansa0;
    uint16_t mask;
    e_r_drv_adc_channel_t channel;

    p_config->resolution = adc_get_resolution();
    p_config->alignment = adc_get_data_alignment();
    p_config->scan_mode = adc_get_scan_mode();
    R_INTC_GetPriority(INTC_ID_ADC_S12ADI0, &p_config->interrupt_priority);

    /* read the ANSA0 field value from the ADANSA0 register */
    ansa0 = RZA_IO_RegRead_16(&ADC.ADANSA0.WORD, ADC_ADANSA0_ANSA0_SHIFT, ADC_ADANSA0_ANSA0);

    mask = 1;

    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        /* determine whether or not the channel is enabled */
        p_config->channels[channel].trigger_source = ((ansa0 & mask) == 0) ? ADC_TRIGGER_DISABLED : ADC_TRIGGER_NORMAL;

        /* the compiler promotes the variable to signed integer for the shift, so cast it back */
        mask = (uint16_t) (mask << 1u);

        /* read the sample time setting */
        p_config->channels[channel].sample_time = RZA_IO_RegRead_8((&ADC.ADSSTR0.BYTE) + channel,
                IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_GetConfiguration
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_ReadConversion
 * Description  : Read ADC conversion value
 * Arguments    : channel[in] - channel to get conversion value for
 *                p_value[out] - pointer to variable to receive conversion value
 * Return Value : ADC_SUCCESS or ADC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_ReadConversion(e_r_drv_adc_channel_t channel, uint32_t *p_value)
{
    if (channel > ADC_CHANNEL_AN007)
    {
        return ADC_ERR_FAILED;
    }

    /* retrieve the ADC conversion value */
    *p_value = RZA_IO_RegRead_16((&ADC.ADDR0.WORD) + channel, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_ReadConversion
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_IsConversionComplete
 * Description  : Returns true if the conversion scan has completed
 * Arguments    : None
 * Return Value : true if the conversion scan has completed, false if the conversion scan is ongoing
 *********************************************************************************************************************/
bool_t R_ADC_IsConversionComplete(void)
{
    /* read the value of the ADCSR register ADST bit - it will be 1 if a scan is in progress */
    if (RZA_IO_RegRead_16(&ADC.ADCSR.WORD, ADC_ADCSR_ADST_SHIFT, ADC_ADCSR_ADST) == 0)
    {
        return true;
    }

    return false;
}
/**********************************************************************************************************************
 * End of function R_ADC_IsConversionComplete
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_ConfigureInterrupts
 * Description  : Configure ADC interrupts
 * Arguments    : p_interrupt_handler[in] - pointer to interrupt handler function
 * Return Value : ADC_SUCCESS or ASC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_ConfigureInterrupts(void (*p_interrupt_handler)(uint32_t sense))
{
    e_r_drv_intc_err_t err;

    err = R_INTC_RegistIntFunc(INTC_ID_ADC_S12ADI0, p_interrupt_handler);

    if (INTC_SUCCESS != err)
    {
        return ADC_ERR_FAILED;
    }

    err = R_INTC_Enable(INTC_ID_ADC_S12ADI0);

    if (INTC_SUCCESS != err)
    {
        return ADC_ERR_FAILED;
    }

    err = R_INTC_SetPriority(INTC_ID_ADC_S12ADI0, R_ADC_PRV_DEFAULT_INTERRUPT_PRIORITY);

    if (INTC_SUCCESS != err)
    {
        return ADC_ERR_FAILED;
    }

    /* enable conversion complete interrupt */
    RZA_IO_RegWrite_16(&ADC.ADCSR.WORD, 1, ADC_ADCSR_ADIE_SHIFT, ADC_ADCSR_ADIE);

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_ConfigureInterrupts
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_GetConversionWidth
 * Description  : Gets the ADC conversion width in bytes
 * Arguments    : p_width[out] - pointer to the conversion width
 * Return Value : ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_GetConversionWidth(uint8_t * p_width)
{
    uint16_t adprc;

    /* read the ADPRC value from the ADCER register */
    adprc = RZA_IO_RegRead_16(&ADC.ADCER.WORD, ADC_ADCER_ADPRC_SHIFT, ADC_ADCER_ADPRC);

    /* map to width in bytes */
    switch (adprc)
    {
        default:
        case R_ADC_PRV_ADCER_ADC_RESOLUTION_12:
        {
            *p_width = 2;
            break;
        }

        case R_ADC_PRV_ADCER_ADC_RESOLUTION_10:
        {
            *p_width = 2;
            break;
        }

        case R_ADC_PRV_ADCER_ADC_RESOLUTION_8:
        {
            *p_width = 1;
            break;
        }
    }

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_GetConversionWidth
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_IsChannelEnabled
 * Description  : Determines if a channel is enabled
 * Arguments    : channel[in] - the channel to check
 *                p_enabled[out] - pointer to channel enabled flag
 *                                 true:  channel is enabled
 *                                 false: channel is disabled
 * Return Value : ADC_SUCCESS or ADC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_IsChannelEnabled(e_r_drv_adc_channel_t channel, bool_t * p_enabled)
{
    uint16_t ansa0;

    if (channel > ADC_CHANNEL_AN007)
    {
        return ADC_ERR_FAILED;
    }

    /* read the mask of enabled channels */
    ansa0 = RZA_IO_RegRead_16(&ADC.ADANSA0.WORD, ADC_ADANSA0_ANSA0_SHIFT, ADC_ADANSA0_ANSA0);

    if ((ansa0 & (1u << channel)) == 0)
    {
        *p_enabled = false;
    }
    else
    {
        *p_enabled = true;
    }

    return ADC_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_ADC_IsChannelEnabled
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_ADC_ConfigureAnalogueInputs
 * Description  : Sets the pins on enabled channels to analogue input
 * Arguments    : None
 * Return Value : ADC_SUCCESS or ADC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_ConfigureAnalogueInputs(void)
{
    int_t handle;
    st_r_drv_gpio_config_t pin_configuration;
    int_t result;
    e_r_drv_adc_channel_t channel;
    bool_t enabled;
    bool_t success = true;

    /* open the GPIO driver */
    handle = open("\\\\.\\gpio", O_RDWR);

    if (handle < 0)
    {
        return ADC_ERR_FAILED;
    }

    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        R_ADC_IsChannelEnabled(channel, &enabled);

        if (enabled)
        {
            /* set port to analogue input */
            /* all analogue input pins are on port 5 */
            pin_configuration.config.pin = (r_gpio_port_pin_t) (GPIO_PORT_5_PIN_0 + channel);
            pin_configuration.config.configuration.function = GPIO_FUNC_PERIPHERAL1;
            pin_configuration.config.configuration.current = GPIO_CURRENT_RESERVED;
            pin_configuration.config.configuration.tint = GPIO_TINT_DISABLE;

            /* cast configuration structure to void pointer */
            result = control(handle, CTL_GPIO_SET_CONFIGURATION, (void *) &pin_configuration);

            if (DRV_SUCCESS != result)
            {
                success = false;
            }
        }
    }

    close(handle);

    return success ? ADC_SUCCESS : ADC_ERR_FAILED;
}
/**********************************************************************************************************************
 * End of function R_ADC_ConfigureAnalogueInputs
 *********************************************************************************************************************/

/* End of File */
