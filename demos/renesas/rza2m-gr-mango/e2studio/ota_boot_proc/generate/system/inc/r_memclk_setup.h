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
/**********************************************************************************************************************
 * File Name    : r_memclk_setup.h
 * Version      : 1.0
 * Description  : header file for r_memclk_setup.c
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 24.10.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "r_compiler_abstraction_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef SYSTEM_INC_R_MEMCLK_SETUP_H_
#define SYSTEM_INC_R_MEMCLK_SETUP_H_

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**********************************************************************************************************************
 * @fn            r_memclk_setup
 * @brief         setting memory clock before starting R_SC_HardwareSetup function
 *********************************************************************************************************************/
extern void r_memclk_setup(void);

/**********************************************************************************************************************
 * @fn            r_spibsc_memclk_setup
 * @brief         setting SPIBSC clock before starting R_SC_HardwareSetup function
 *                implemented in SPIBSC driver
 *********************************************************************************************************************/
extern void r_spibsc_memclk_setup(void) R_COMPILER_WEAK;

/**********************************************************************************************************************
 * @fn            r_hyperbus_memclk_setup
 * @brief         setting HYPERBUS clock before starting R_SC_HardwareSetup function
 *                implemented in HYPERBUS driver
 *********************************************************************************************************************/
extern void r_hyperbus_memclk_setup(void) R_COMPILER_WEAK;

/**********************************************************************************************************************
 * @fn            r_octabus_memclk_setup
 * @brief         setting OCTABUS clock before starting R_SC_HardwareSetup function
 *                implemented in OCTABUS driver
 *********************************************************************************************************************/
extern void r_octabus_memclk_setup(void) R_COMPILER_WEAK;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYSTEM_INC_R_MEMCLK_SETUP_H_ */
