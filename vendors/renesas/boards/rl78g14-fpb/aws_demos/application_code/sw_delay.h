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
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.

**********************************************************************************************************************/
/*********************************************************************************************************************
* File Name    : sw_delay.h
* Description  : head file for delay
**********************************************************************************************************************/
#ifndef SW_DELAY_H
#define SW_DELAY_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "r_cg_macrodriver_sensor.h"
/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */


/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* The number of cycles required per software delay loop. */
#ifndef DELAY_LOOP_CYCLES
 #define DELAY_LOOP_CYCLES    (42)
#endif

/* Calculates the number of delay loops to pass to bsp_prv_software_delay_loop to achieve at least the requested cycle
 * count delay. This is 1 loop longer than optimal if cycles is a multiple of BSP_DELAY_LOOP_CYCLES, but it ensures
 * the requested number of loops is at least 1 since bsp_prv_software_delay_loop cannot be called with a loop count
 * of 0. */
#define DELAY_LOOPS_CALCULATE(cycles)    (((cycles) / DELAY_LOOP_CYCLES) + 1U)

typedef unsigned long long	uint64_t;

/** Available delay units for R_BSP_SoftwareDelay(). These are ultimately used to calculate a total # of microseconds */
/*typedef enum
{
    DELAY_UNITS_SECONDS      = 1000000, ///< Requested delay amount is in seconds
    DELAY_UNITS_MILLISECONDS = 1000,    ///< Requested delay amount is in milliseconds
    DELAY_UNITS_MICROSECONDS = 1        ///< Requested delay amount is in microseconds
} delay_units_t;*/
typedef uint64_t delay_units_t;
#define DELAY_UNITS_SECONDS			(1000000)	///< Requested delay amount is in seconds
#define DELAY_UNITS_MILLISECONDS	(1000)		///< Requested delay amount is in milliseconds
#define DELAY_UNITS_MICROSECONDS	(1)			///< Requested delay amount is in microseconds
/** @} (end addtogroup BSP_MCU) */

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
void R_SoftwareDelay (uint32_t delay, delay_units_t units);
void R_SoftwareDelayMs (uint32_t delay);

#endif
