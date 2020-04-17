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
 * Renesas reserves the right, without notice, to make changes to this
 * software and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * File Name    : hwsetup.h
 * Description  : Initialisation process done before section initialise
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 *****************************************************************************/
#include "r_typedefs.h"
#include "r_compiler_abstraction_api.h"

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#ifndef SYSTEM_INC_HWSETUP_H_
#define SYSTEM_INC_HWSETUP_H_

/* Parameter for SPIBSC driver */
#define HWSETUP_SPIBSC_USE_DDR (1)
#define HWSETUP_SPIBSC_USE_SDR (0)

/******************************************************************************
 Exported global functions
 *****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @fn         R_SC_HardwareSetup
 * @brief      System initialisation routine before section initialise
 */
extern void     R_SC_HardwareSetup(void);

/**
 * @fn         Userdef_PreHardwareSetup
 * @brief      User defined hardware setup called earlier of R_SC_HardwareSetup
 */
extern void Userdef_PreHardwareSetup(void) R_COMPILER_WEAK;

/**
 * @fn         Userdef_PostHardwareSetup
 * @brief      User defined hardware setup called later of R_SC_HardwareSetup
 */
extern void Userdef_PostHardwareSetup(void) R_COMPILER_WEAK;

/**
 * @fn         R_SPIBSC_Setup
 * @brief      SPIBSC controller and device initialisation routine
 *             wrapper interface
 * @param[in]  ddrsdr: HWSETUP_SPIBSC_USE_DDR or HWSETUP_SPIBSC_USE_SDR
 */
extern void R_SPIBSC_Setup(uint32_t ddrsdr) R_COMPILER_WEAK;

/**
 * @fn         R_HYPERBUS_Setup
 * @brief      HyperBus(TM) controller and device initialisation routine
 *             wrapper interface
 */
extern void R_HYPERBUS_Setup(void) R_COMPILER_WEAK;

/**
 * @fn         R_OCTABUS_Setup
 * @brief      OctaBus(TM) controller and device initialisation routine
 *             wrapper interface
 */
extern void R_OCTABUS_Setup(void) R_COMPILER_WEAK;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* SYSTEM_INC_HWSETUP_H_ */

/* End of File */
