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
***********************************************************************************************************************
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************
 * File Name    : r_adc_drv_api.h
 * Device(s)    : RZ/A2M
 * Description  : Header file for ADC Driver
***********************************************************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 19.02.2019 1.00 First Release
**********************************************************************************************************************/


#include "r_typedefs.h"
#include "r_devlink_wrapper.h"

#include "r_adc_drv_link.h"

#ifndef R_ADC_INC_R_ADC_DRV_API_H_
#define R_ADC_INC_R_ADC_DRV_API_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define R_DRV_ADC_HLD_DRV_NAME           ("HLD ADC")

/** Major Version Number of API.
 * Updated by product owner */
#define R_DRV_ADC_HLD_VERSION_MAJOR      (1)

/** Minor Version Number of API.
 * Updated by developer */
#define R_DRV_ADC_HLD_VERSION_MINOR      (0)

/** Build Number of API.
 * Generated during customer release */
#define R_DRV_ADC_HLD_BUILD_NUM          (0)

/** Unique ID. Assigned by requirements */
#define R_DRV_ADC_HLD_UID                (0)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

typedef enum
{
    CTL_ADC_SOFTWARE_TRIGGER = 0u,         /*!< ADC conversion software trigger */
    CTL_ADC_SET_CONFIGURATION,             /*!< Set ADC configuration */
    CTL_ADC_GET_CONFIGURATION              /*!< Get ADC configuration */
} e_r_drv_ctrl_code_adc_t;


/**********************************************************************************************************************
 Constant Data
 *********************************************************************************************************************/

extern const st_r_driver_t g_adc_driver;

#endif /* R_ADC_INC_R_ADC_DRV_API_H_ */
