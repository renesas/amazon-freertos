/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_cpg_drv_api.h
 * Description  : CPG driver API header
 **********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_devlink_wrapper.h"
#include "r_cpg_drv_link.h"

#ifndef SRC_RENESAS_DRIVERS_R_CPG_INC_R_CPG_DRV_API_H_
#define SRC_RENESAS_DRIVERS_R_CPG_INC_R_CPG_DRV_API_H_

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
typedef enum
{
    CTL_CPG_SET_SC_CFG = 0u,   /*!< Initialize CPG parameters as defined in Smart Configurator table */
    CTL_CPG_SET_XTAL_KHZ,      /*!< Set the system oscillator XTAL frequency */
    CTL_CPG_SET_MAIN_CLK,      /*!< Configure main clock function */
    CTL_CPG_SET_SUB_CLK,       /*!< Configure sub clock CPG control function */
    CTL_CPG_SET_CLK_SRC,       /*!< Configure sub clock input source */
    CTL_CPG_SET_EXT_CLK,       /*!< Configure external clock pin characteristics */
    CTL_CPG_GET_CLK,           /*!< Get clock function */
    CTL_CPG_SET_SUB_CLK_MULTI, /*!< Configure multiple sub clocks simultaneously */
} e_ctrl_code_cpg_t;


/******************************************************************************
 Macro definitions
 ******************************************************************************/

#define STDIO_CPG_RZ_HLD_DRV_NAME        ("HLD CPG")

/** Major Version Number of API.
 * Updated by product owner */
#define STDIO_CPG_RZ_HLD_VERSION_MAJOR   (1)

/** Minor Version Number of API.
 * Updated by developer */
#define STDIO_CPG_RZ_HLD_VERSION_MINOR   (5)

/** Build Number of API.
 * Generated during customer release */
#define STDIO_CPG_RZ_HLD_BUILD_NUM       (0)

/** Unique ID. Assigned by requirements */
#define STDIO_CPG_RZ_HLD_UID             (63)


/******************************************************************************
 Variable Externs
 ******************************************************************************/

/* Define the driver function table for this implementation */
extern const st_r_driver_t g_cpg_driver;

/******************************************************************************
 Functions Prototypes
 ******************************************************************************/
/* ==== API functions ==== */

#endif  /* SRC_RENESAS_DRIVERS_R_CPG_INC_R_CPG_DRV_API_H_ */

/*******************************************************************************************************************//**
  End  Of File
 ***********************************************************************************************************************/
