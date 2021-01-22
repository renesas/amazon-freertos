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
 * File Name     : r_adc_lld_rza2m.h
 * Description   : ADC low layer driver API header
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 19.02.2019 1.00
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "driver.h"

#include "r_intc_drv_api.h"

/* Multiple inclusion prevention macro */
#ifndef R_ADC_INC_R_ADC_LLD_RZA2M_H_
#define R_ADC_INC_R_ADC_LLD_RZA2M_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/* Version Number of API */

#define R_DRV_ADC_LLD_DRV_NAME            ("LLD EBK_RZA2M ADC")

/** Major Version Number of API */
#define R_DRV_ADC_LLD_VERSION_MAJOR       (1)
/** Minor Version Number of API */
#define R_DRV_ADC_LLD_VERSION_MINOR       (0)
/** Minor Version Number of API */
#define R_DRV_ADC_LLD_BUILD_NUM           (0)
/** Unique ID */
#define R_DRV_ADC_LLD_UID                 (704)

/** Number of ADC channels available */
#define ADC_CHANNELS                      (8)

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

typedef enum
{
    ADC_SUCCESS = DRV_SUCCESS,              /*!< Success */
    ADC_ERR_FAILED = -1,                    /*!< Operation failed */
    ADC_ERR_INVALID_CFG = -2,               /*!< Invalid configuration */
} e_r_drv_adc_err_t;

typedef enum
{
    ADC_CHANNEL_AN000,                      /*!< Channel 0 */
    ADC_CHANNEL_AN001,                      /*!< Channel 1 */
    ADC_CHANNEL_AN002,                      /*!< Channel 2 */
    ADC_CHANNEL_AN003,                      /*!< Channel 3 */
    ADC_CHANNEL_AN004,                      /*!< Channel 4 */
    ADC_CHANNEL_AN005,                      /*!< Channel 5 */
    ADC_CHANNEL_AN006,                      /*!< Channel 6 */
    ADC_CHANNEL_AN007                       /*!< Channel 7 */
} e_r_drv_adc_channel_t;

typedef enum
{
    ADC_TRIGGER_DISABLED,                   /*!< Channel disabled */
    ADC_TRIGGER_NORMAL,                     /*!< Normal (software trigger) */
    ADC_TRIGGER_GROUPA,                     /*!< Group A */
    ADC_TRIGGER_GROUPB,                     /*!< Group B */
    ADC_TRIGGER_GROUPC                      /*!< Group C */
} e_r_drv_adc_trigger_source_t;

typedef enum
{
    ADC_RESOLUTION_12 = 0,                  /*!< 12-bit conversion resolution */
    ADC_RESOLUTION_10,                      /*!< 10-bit conversion resolution */
    ADC_RESOLUTION_8                        /*!< 8-bit conversion resolution */
} e_r_drv_adc_resolution_t;

typedef enum
{
    ADC_ALIGN_RIGHT = 0,                    /*!< Conversion value is right-aligned in the result word */
    ADC_ALIGN_LEFT                          /*!< Conversion value is left-aligned in the result word */
} e_r_drv_adc_alignment_t;

typedef enum
{
    ADC_SCAN_MODE_SINGLE = 0,               /*!< Single scan mode */
    ADC_SCAN_MODE_GROUP = 1,                /*!< Group scan mode (not supported in the initial release) */
    ADC_SCAN_MODE_CONTINUOUS = 2            /*!< Continuous scan mode */
} e_r_drv_adc_scan_mode_t;

typedef enum
{
    ADC_TRIGGER_SOFTWARE = 0,               /*!< Software trigger */
    ADC_TRIGGER_ASYNC = 1,                  /*!< Asynchronous Trigger (i.e. I/O pin) */
    ADC_TRIGGER_SYNC_MTU = 2,               /*!< Triggered from MTU */
    ADC_TRIGGER_SYNC_GPT = 3                /*!< Triggered from GPT */
} e_r_drv_adc_trigger_mode_t;

typedef enum
{
    ADC_REASON_SCAN_COMPLETE = 0,           /*!< Scan complete */
} e_r_drv_adc_callback_reason_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
typedef struct
{
    uint32_t sample_time;                                 /*!< ADC channel sample time, in cycles */
    e_r_drv_adc_trigger_source_t trigger_source;          /*!< ADC channel trigger source */
} st_r_drv_adc_channel_config_t;

typedef struct
{
    e_r_drv_adc_resolution_t resolution;                         /*!< ADC conversion resolution */
    e_r_drv_adc_alignment_t alignment;                           /*!< ADC conversion value alignment */
    e_r_drv_adc_scan_mode_t scan_mode;                           /*!< ADC scan mode */
    void (*p_adcCallback)(e_r_drv_adc_callback_reason_t reason); /*!< ADC conversion complete callback function */
    e_r_drv_intc_priority_t interrupt_priority;                  /*!< ADC conversion complete interrupt priority */
    st_r_drv_adc_channel_config_t channels[ADC_CHANNELS];        /*!< ADC channel configuration */
} st_r_drv_adc_unit_config_t;

typedef struct
{
    st_r_drv_adc_unit_config_t config;                           /*!< ADC unit configuration data */
    e_r_drv_adc_err_t err;                                       /*!< error code (see e_r_drv_adc_err_t) */
} st_r_drv_adc_config_t;

typedef struct
{
    e_r_drv_adc_resolution_t resolution;                         /*!< ADC conversion resolution */
    e_r_drv_adc_alignment_t alignment;                           /*!< ADC conversion value alignment */
    e_r_drv_adc_scan_mode_t scan_mode;                           /*!< ADC scan mode */
    void (*p_adcCallback)(e_r_drv_adc_callback_reason_t reason); /*!< ADC conversion complete callback function */
    e_r_drv_intc_priority_t interrupt_priority;                  /*!< ADC conversion complete interrupt priority */

    e_r_drv_adc_trigger_source_t trigger_source_chan_0;          /*!< ADC channel 0 trigger source */
    uint32_t sample_time_chan_0;                                 /*!< ADC channel 0 sample time */
    e_r_drv_adc_trigger_source_t trigger_source_chan_1;          /*!< ADC channel 1 trigger source */
    uint32_t sample_time_chan_1;                                 /*!< ADC channel 1 sample time */
    e_r_drv_adc_trigger_source_t trigger_source_chan_2;          /*!< ADC channel 2 trigger source */
    uint32_t sample_time_chan_2;                                 /*!< ADC channel 2 sample time */
    e_r_drv_adc_trigger_source_t trigger_source_chan_3;          /*!< ADC channel 3 trigger source */
    uint32_t sample_time_chan_3;                                 /*!< ADC channel 3 sample time */
    e_r_drv_adc_trigger_source_t trigger_source_chan_4;          /*!< ADC channel 4 trigger source */
    uint32_t sample_time_chan_4;                                 /*!< ADC channel 4 sample time */
    e_r_drv_adc_trigger_source_t trigger_source_chan_5;          /*!< ADC channel 5 trigger source */
    uint32_t sample_time_chan_5;                                 /*!< ADC channel 5 sample time */
    e_r_drv_adc_trigger_source_t trigger_source_chan_6;          /*!< ADC channel 6 trigger source */
    uint32_t sample_time_chan_6;                                 /*!< ADC channel 6 sample time */
    e_r_drv_adc_trigger_source_t trigger_source_chan_7;          /*!< ADC channel 7 trigger source */
    uint32_t sample_time_chan_7;                                 /*!< ADC channel 7 sample time */
} st_r_drv_adc_sc_config_t;
#pragma GCC diagnostic pop

/**********************************************************************************************************************
 Function prototypes
 *********************************************************************************************************************/
/* ==== API for low layer ADC driver ==== */

/**********************************************************************************************************************
 * @fn            R_ADC_GetVersion
 * @brief         Gets the version number of this low-layer driver
 * @param [out]   pinfo: returns the driver information
 * @retval        DRV_SUCCESS always returned
 *********************************************************************************************************************/
uint32_t R_ADC_GetVersion (st_drv_info_t *pinfo);

/**********************************************************************************************************************
 * @fn            R_ADC_HWInitialise
 * @brief         Initialise the hardware
 * @retval        ADC_SUCCESS or the error code on failure
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_HWInitialise (void);

/**********************************************************************************************************************
 * @fn            R_ADC_LoadInitialConfiguration
 * @brief         Load the initial configuration from the SC style table
 * @param [out]   p_userCallback: pointer to user callback function pointer
 * @retval        ADC_SUCCESS or ADC_ERR_INVALID_CFG
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_LoadInitialConfiguration (void (**p_userCallback)(e_r_drv_adc_callback_reason_t reason));

/**********************************************************************************************************************
 * @fn            R_ADC_HWUninitialise
 * @brief         Uninitialise the hardware
 * @retval        ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_HWUninitialise (void);

/**********************************************************************************************************************
 * @fn            R_ADC_SoftwareTrigger
 * @brief         Trigger an ADC conversion scan
 * @retval        ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_SoftwareTrigger (void);

/**********************************************************************************************************************
 * @fn            R_ADC_SetConfiguration
 * @brief         Set the ADC configuration
 * @param [in]    p_config: pointer to configuration structure
 * @retval        ADC_SUCCESS or the error code on failure
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_SetConfiguration (st_r_drv_adc_unit_config_t * config);

/**********************************************************************************************************************
 * @fn            R_ADC_GetConfiguration
 * @brief         Get the ADC configuration
 * @param [out]   p_config: pointer to configuration structure
 * @retval        ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_GetConfiguration (st_r_drv_adc_unit_config_t * config);

/**********************************************************************************************************************
 * @fn            R_ADC_ReadConversion
 * @brief         Read ADC conversion value
 * @param [in]    channel: channel to get conversion value for
 * @param [out]   p_value: pointer to variable to receive conversion value
 * @retval        ADC_SUCCESS or ADC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_ReadConversion (e_r_drv_adc_channel_t channel, uint32_t *p_value);

/**********************************************************************************************************************
 * @fn            R_ADC_IsConversionComplete
 * @brief         Determines whether or not a conversion scan is ongoing
 * @retval        true if the conversion scan has completed, false if the conversion scan is ongoing
 *********************************************************************************************************************/
bool_t R_ADC_IsConversionComplete (void);

/**********************************************************************************************************************
 * @fn            R_ADC_ConfigureInterrupts
 * @brief         Configure ADC interrupts
 * @param [in]    interrupt_handler: pointer to interrupt handler function
 * @retval        ADC_SUCCESS or ASC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_ConfigureInterrupts (void (*interrupt_handler)(uint32_t sense));

/**********************************************************************************************************************
 * @fn            R_ADC_GetConversionWidth
 * @brief         Gets the ADC conversion width in bytes
 * @param [out]   p_width: pointer conversion width
 * @retval        ADC_SUCCESS always returned
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_GetConversionWidth (uint8_t * p_width);

/**********************************************************************************************************************
 * @fn            R_ADC_IsChannelEnabled
 * @brief         Determines if a channel is enabled
 * @param [in]    channel: the channel to check
 * @param [out]   p_enabled[out]: pointer to channel enabled flag - set true if the channel is enabled, false if not
 * @retval        ADC_SUCCESS or ADC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_IsChannelEnabled (e_r_drv_adc_channel_t channel, bool_t * enabled);

/**********************************************************************************************************************
 * @fn            R_ADC_ConfigureAnalogueInputs
 * @brief         Sets the pins on enabled channels to analogue input
 * @retval        ADC_SUCCESS or ADC_ERR_FAILED
 *********************************************************************************************************************/
e_r_drv_adc_err_t R_ADC_ConfigureAnalogueInputs (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* R_ADC_INC_R_ADC_LLD_RZA2M_H_ */

/* End of File */
