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
/*******************************************************************************
* File Name : r_stb_lld_rza2m.h
* Description :
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "iodefine.h"
#include "r_typedefs.h"
#include "driver.h"

/******************************************************************************
Macro definitions
******************************************************************************/

#ifndef R_STB_REG_RZA2M_H_
#define R_STB_REG_RZA2M_H_

/* Version Number of API */

#define STDIO_STB_RZ_LLD_DRV_NAME ("LLD EBK_RZA2M STB")

/** Major Version Number of API */
#define STDIO_STB_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API */
#define STDIO_STB_RZ_LLD_VERSION_MINOR      (3)
/** Minor Version Number of API */
#define STDIO_STB_RZ_LLD_BUILD_NUM          (0)
/** Unique ID */
#define STDIO_STB_RZ_LLD_UID                (68)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum stb_err{
    STB_SUCCESS = 0,            /*!< Successful operate */
    STB_INVALID_MODULE=(-1),    /*!< Invalid module */
    STB_AGAIN=(-2),             /*!< Operation not complete */
    STB_INVALID_STATE=(-3),     /*!< Invalid state for operation */
} e_stb_err_t;

typedef enum stb_module
{
    MODULE_CORESIGHT=1,
    MODULE_OSTM0,
    MODULE_OSTM1,
    MODULE_OSTM2,
    MODULE_MTU3,
    MODULE_CANFD,
    MODULE_ADC,
    MODULE_GPT,
    MODULE_SCIFA0,
    MODULE_SCIFA1,
    MODULE_SCIFA2,
    MODULE_SCIFA3,
    MODULE_SCIFA4,
    MODULE_SCI0,
    MODULE_SCI1,
    MODULE_IrDA,
    MODULE_CEU,
    MODULE_RTC0,
    MODULE_RTC1,
    MODULE_JCU,
    MODULE_VIN,
    MODULE_ETHER,
    MODULE_USB0,
    MODULE_USB1,
    MODULE_IMR2,
    MODULE_DRW,
    MODULE_MIPI,
    MODULE_SSIF0,
    MODULE_SSIF1,
    MODULE_SSIF2,
    MODULE_SSIF3,
    MODULE_I2C0,
    MODULE_I2C1,
    MODULE_I2C2,
    MODULE_I2C3,
    MODULE_SPIBSC,
    MODULE_VDC6,
    MODULE_RSPI0,
    MODULE_RSPI1,
    MODULE_RSPI2,
    MODULE_HYPERBUS,
    MODULE_OCTAMEM,
    MODULE_RSPDIF,
    MODULE_DRP,
    MODULE_TSIP,
    MODULE_NAND,
    MODULE_SDMMC0,
    MODULE_SDMMC1,
} e_stb_module_t;

typedef enum
{
    MODULE_RESET_STATE,
    MODULE_NOT_RESET_STATE,
} e_stb_reset_state_t;



/******************************************************************************
Variable Externs
******************************************************************************/


/******************************************************************************
Functions Prototypes
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @fn         R_STB_RequestModuleStop
 * @brief      Prepare to stop the module
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
extern e_stb_err_t R_STB_RequestModuleStop(e_stb_module_t module);

/**
 * @fn         R_STB_RequestModuleStart
 * @brief      Cleanup to start the module after supplying clock
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
extern e_stb_err_t R_STB_RequestModuleStart(e_stb_module_t module);

/**
 * @fn         R_STB_StopModule
 * @brief      Stops the module
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
extern e_stb_err_t R_STB_StopModule(e_stb_module_t module);

/**
 * @fn         R_STB_StartModule
 * @brief      Starts the module
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
extern e_stb_err_t R_STB_StartModule(e_stb_module_t module);

/**
 * @fn         R_STB_SetModuleResetState
 * @brief      Set module reset state
 * @note       this API also run the process described in the section 52.3.6
 *             of User's Manual.
 * @param[in]  module: module number
 * @param[in]  state: reset state
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
extern e_stb_err_t R_STB_SetModuleResetState(e_stb_module_t module, e_stb_reset_state_t state);

/**
 * @fn         R_STB_GetModuleResetState
 * @brief      Get module reset state
 * @param[in]  module: module number
 * @param[out] p_state: reset state
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
extern e_stb_err_t R_STB_GetModuleResetState(e_stb_module_t module, e_stb_reset_state_t * p_state);

/**
 * @fn         R_STB_GetVersion
 * @brief      Get the Low Layer Driver specific version
 * @param[in]  p_ver_info: detailed driver information structure
 * @retval     STB_SUCCESS           Successful operation (always).
 */
extern int_t R_STB_GetVersion(st_ver_info_t *p_ver_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* R_STB_REG_RZA2M_H_ */

/* End of File */
