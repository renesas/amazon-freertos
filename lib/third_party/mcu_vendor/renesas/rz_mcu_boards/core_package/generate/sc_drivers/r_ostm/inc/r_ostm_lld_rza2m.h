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
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/******************************************************************************
 * File Name    : r_ostm_lld_rza2m.h
 * Description  : OS timer driver API header
 ******************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup RZA2M_EBK_OSTM EBK+RZA2M OSTM low layer driver configuration
 * @brief This interface allows control of the OSTM interface on the EBK+RZ/A2.
 *
 * @section RZA2M_EBK_OSTM_SUMMARY Summary
 * @brief The EBK+RZA2 low layer driver has direct access to the OSTM peripheral. It is configured by smart configurator.
 *
 * @section RZA2M_EBK_OSTM_API_LIMITATIONS Known Limitations
 * NONE
 *
 * @section RZA2M_EBK_OSTM_API_INSTANCES Known Implementations
 * @ref NONE_YET
 *
 * @section RZA2M_EBK_OSTM_API_RELATED Related modules
 * @ingroup RZA2M_EBK_OSTM_DRIVER
 * See also:
 * @ref RZA2M_EBK_OSTM_DRIVER
 * @ref OSTM_SC_IF
 * @ref RZA2M_EBK_OSTM_LINK
 * @ref RZA2M_EBK_OSTM_PRV
 *
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "iodefine.h"
#include "r_typedefs.h"
#include "driver.h"
#include "r_intc_drv_api.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#ifndef SRC_RENESAS_DRIVERS_R_OSTM_INC_R_OSTM_LLD_RZA2M_H_
#define SRC_RENESAS_DRIVERS_R_OSTM_INC_R_OSTM_LLD_RZA2M_H_

/* Version Number of API */

#define STDIO_OSTM_RZ_LLD_DRV_NAME           ("LLD EBK_RZA2M OSTM")

/** Major Version Number of API */
#define STDIO_OSTM_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API */
#define STDIO_OSTM_RZ_LLD_VERSION_MINOR      (3)
/** Minor Version Number of API */
#define STDIO_OSTM_RZ_LLD_BUILD_NUM          (0)
/** Unique ID */
#define STDIO_OSTM_RZ_LLD_UID                (36)


/******************************************************************************
 Typedef definitions
 ******************************************************************************/
typedef enum
{
    OSTM_MODE_INTERVAL = 0,
    OSTM_MODE_FREERUN,
} e_r_drv_ostm_mode_t;

typedef enum
{
    OSTM_TIME_MS,
    OSTM_TIME_COUNT,
} e_r_drv_ostm_counter_format_t;

typedef enum
{
    OSTM_START_INTERRUPT_OFF,
    OSTM_START_INTERRUPT_ON,
} e_r_drv_start_interrupt_t;

typedef struct
{
    e_r_drv_ostm_mode_t           mode;
    e_r_drv_ostm_counter_format_t counter_format;
    uint32_t                      time;
    e_r_drv_start_interrupt_t     start_generates_interruption;
    e_r_drv_intc_enable_t         int_enable;
    e_r_drv_intc_priority_t       int_priority;
    void *                        int_function;
} st_r_drv_ostm_config_t;

typedef enum
{
    OSTM_CH0 = 0,
    OSTM_CH1 = 1,
    OSTM_CH2 = 2,
    OSTM_CH_MAX
} e_r_drv_ostm_channel_t;


/******************************************************************************
 Constant Data
 ******************************************************************************/


/******************************************************************************
 Interface
 ******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @fn         R_OSTM_Init
 *
 * @brief      Initialises the OSTM channel.
 *
 * @param[in]  channel:  desired channel
 * @param[in]  p_config: parameters of configuration
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_OSTM_Init(e_r_drv_ostm_channel_t channel, const st_r_drv_ostm_config_t *p_config);

/**
 * @fn         R_OSTM_Close
 *
 * @brief      Releases the ostm channel allowing the channel to be reused in
 *             another module of the application
 *
 * @param[in]  channel:  desired channel
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_OSTM_Close(e_r_drv_ostm_channel_t channel);

/**
 * @fn         R_OSTM_GetVersion
 *
 * @brief      Obtains driver specific version information which is used for
 *             reporting
 *
 * @param[out] pVerInfo:  driver version information is populated into this
 *                        structure.
 *                        @note the structure resources are created
 *                              by the application not this function
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_OSTM_GetVersion(st_ver_info_t *p_ver_info);

/**
 * @fn         R_OSTM_Start
 *
 * @brief      Starts timer for specified channel
 *
 * @param[in]  channel:  desired channel
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_OSTM_Start(e_r_drv_ostm_channel_t channel);

/**
 * @fn         R_OSTM_Stop
 *
 * @brief      Stops timer for specified channel
 *
 * @param[in]  channel:  desired channel
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_OSTM_Stop(e_r_drv_ostm_channel_t channel);

/**
 * @fn         R_OSTM_GetCount
 *
 * @brief      Gets the count value for the specified OSTM channel
 * @note       Does not stop the channel from running
 *
 * @param[in]  channel:  desired channel
 * @param[out] p_data:   count value
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_OSTM_GetCount(e_r_drv_ostm_channel_t channel, uint32_t *p_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* SRC_RENESAS_DRIVERS_R_OSTM_INC_R_OSTM_LLD_RZA2M_H_ */

/*******************************************************************************************************************//**
 * @} (end addtogroup RZA2M_EBK_OSTM)
 ***********************************************************************************************************************/

/* End of File */
