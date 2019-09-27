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
* File Name   : cryptogram_sample.c
* Description : This is the RZ/A1H CRYPTOGRAM sample main code
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include "r_os_abstraction_api.h"
#include "r_devlink_wrapper.h"

#include "r_typedefs.h"
#include "r_adc_drv_api.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void R_Prnd_Init_Genrand(const uint32_t seed);
extern uint32_t R_Prnd_Genrand_int32(void);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint32_t get_init_value(void);

/******************************************************************************
* Function Name: cryptogram_sample
* Description  : It is a sample program using AES and PRND.
* Arguments    : argc, argv -
*                Arguments all Unused
* Return Value : R_PROCESS_COMPLETE    : Success
*              : R_CRYPT_ERROR_POINTER : Error
*              : R_CRYPT_ERROR_LENGTH  : Error
******************************************************************************/
int32_t cryptogram_sample(int32_t seed_in)
{
    uint32_t rnd_dat;

    uint16_t block=1;
    int_t i, ret;
    volatile uint32_t seed;

    rnd_dat = seed_in;

    seed = get_init_value(); /* for seed data */
    rnd_dat += seed;

    /* init random */
    R_Prnd_Init_Genrand((uint32_t)rnd_dat); /* Initialize the basic random number generator. */

    rnd_dat = R_Prnd_Genrand_int32();

    return rnd_dat;
}

/******************************************************************************
* Function Name: get_init_value
* Description  : The initial value used as a seed value is generated.
* Arguments    : none
* Return Value : Analog input channel data
******************************************************************************/
static uint32_t get_init_value(void)
{
    uint32_t dummy32_buf;
    static st_r_drv_adc_config_t adc_config;
    e_r_drv_adc_channel_t channel;
    int_t ret_val;
    static uint8_t buffer[2];
    int_t count;
    uint16_t channel_conversion = 0;
    static int_t s_adc_handle = -1;

    dummy32_buf = 0;

    s_adc_handle = open(DEVICE_INDENTIFIER "adc0", O_RDWR);

    /* on error */
    if (s_adc_handle < 0)
    {
        /* error */
        return (uint32_t)dummy32_buf;
    }

    /* set up an ADC configuration */
    adc_config.config.alignment = ADC_ALIGN_RIGHT;
    /* initalize interrupt priority */
    adc_config.config.interrupt_priority = INTC_PRIORITY_20;
    adc_config.config.resolution = ADC_RESOLUTION_12;
    adc_config.config.scan_mode = ADC_SCAN_MODE_SINGLE;
    /* initalize call back */
    adc_config.config.p_adcCallback = NULL;

    /* set the per-channel configuration to enable channel 6 (AN006) only */
    for (channel = ADC_CHANNEL_AN000; channel < ADC_CHANNELS; channel++)
    {
        adc_config.config.channels[channel].sample_time = 0;
        adc_config.config.channels[channel].trigger_source = ADC_TRIGGER_DISABLED;
    }

    adc_config.config.channels[ADC_CHANNEL_AN007].sample_time = 50;
    adc_config.config.channels[ADC_CHANNEL_AN007].trigger_source = ADC_TRIGGER_NORMAL;

    /* call control */
    ret_val = control(s_adc_handle, CTL_ADC_SET_CONFIGURATION, &adc_config);

    /* on error */
    if (DRV_SUCCESS != ret_val)
    {
        /* error */
        return (uint32_t)dummy32_buf;
    }

    /* call control to trigger the ADC conversion */
    /* no callback is set, so this will block until the conversion has been completed (a microsecond or so) */
    control(s_adc_handle, CTL_ADC_SOFTWARE_TRIGGER, NULL);

    count = read(s_adc_handle, buffer, 2);

    /* if we successfully read the 2 bytes of conversion data for AN006 */
    if (2 == count)
    {
        /* data is read LSB first */
        channel_conversion = buffer[1];
        channel_conversion <<= 8u;
        channel_conversion |= buffer[0];
    }

    /* shift the random number */
    dummy32_buf = (uint32_t) channel_conversion << 16;

    /* call control to trigger the ADC conversion */
    /* no callback is set, so this will block until the conversion has been completed (a microsecond or so) */
    control(s_adc_handle, CTL_ADC_SOFTWARE_TRIGGER, NULL);

    count = read(s_adc_handle, buffer, 2);

    /* if we successfully read the 2 bytes of conversion data for AN006 */
    if (2 == count)
    {
        /* data is read LSB first */
        channel_conversion = buffer[1];
        channel_conversion <<= 8u;
        channel_conversion |= buffer[0];
    }

    /* add AD data to random number */
    dummy32_buf |= ((uint32_t) channel_conversion);

    close(s_adc_handle);

    /* return the created number */
    return (uint32_t)dummy32_buf;
}

/* End of File */
