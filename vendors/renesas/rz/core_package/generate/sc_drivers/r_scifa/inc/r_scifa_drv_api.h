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
/******************************************************************************************************************//**
 * @headerfile     r_scifa_drv_api.h
 * @brief          The High Layer Driver for the SCIFA
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 15.05.2018 1.00    Release
 *               : 10.05.2019 1.01    Fixed issue with channel 0 Baud rate setting
 *               : 23.05.2019 1.02    Restructuring to ensure that other LLDs are only accessed from the SCIFA LLD
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"         /* Renesas Type Definitions */
#include "r_scifa_device_cfg.h" /* Abstracted Low Level driver configuration */
#include "r_devlink_wrapper.h"  /* Device driver interface */

#include "r_scifa_drv_link.h"

/******************************************************************************************************************//**
 * @ingroup RENESAS_DRIVER_LIBRARY
 * @defgroup R_SCIFA_HLD_API_77 RZA2M SCIFA High Level Driver API
 *
 * @section R_SCIFA_HLD_API_SUMMARY Summary
 *
 * The driver implements the Renesas STDIO interface intended to be used
 * in systems with our without an OS. The driver makes calls to the
 * Renesas OS_Abstraction interface enabling most third party OS
 * implementations to be used without modification to this driver code.
 * Where compiler specific keywords are required they have also been abstracted
 * and can be found in the relevant folder.
 *
 * The driver is accessed by the Renesas STDIO interface, or directly by
 * instantiating a device structure with the interface. Examples can be found
 * in the OS Abstraction interface documentation.
 *
 * @section R_SCIFA_HLD_API_77_FEATURES Driver Features:
 * * Thread safe and re-entrant.
 * * Supports multiple channels.
 * * Selectable Baud Rate.
 * * Selectable Stop bits.
 * * Selectable Parity.
 * * Selectable Data Bits.
 * * Read/Write functionality.
 * * Selectable Mode between Asynchronous and Clock Synchronous transmission modes.
 *
 * @section R_SCIFA_HLD_API_77_INSTANCES Known Implementations:
 * This driver is used in the RZA2M Software Package.
 *
 * @section R_SCIFA_HLD_API_77_RESTRICTIONS Known Restrictions:
 * * SSGSTARC-86 SCIFA Feature: TX/RX Break Handling is not supported.
 * * SSGSTARC-87 SCIFA Feature: Circular Buffer Overflow handling is not supported.
 * * SSGSTARC-88 SCIFA Feature: Error Reporting is not accessible by the user.
 *
 * @section R_SCIFA_HLD_API_77_SEE_ALSO See Also:
 * @see R_SCIFA_LLD_API_78 SCIFA RZA2 EBK_RZA2M Low Level Driver
 * @see RENESAS_SC_CFG Renesas Smart Configuration Settings
 * @see RENESAS_DEVLINK          Renesas Device Link interface (based on STDIO)
 * @see RENESAS_OS_ABSTRACTION   Renesas OS Abstraction interface
 * @{
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef RENESAS_DRIVERS_R_SCIFA_DRV_API_H_
#define RENESAS_DRIVERS_R_SCIFA_DRV_API_H_

#define STDIO_SCIFA_RZ_HLD_DRV_NAME ("HLD SCIFA")

/** Major Version Number of API.
 * Updated by product owner */
#define STDIO_SCIFA_RZ_HLD_VERSION_MAJOR (2)

/** Minor Version Number of API.
 * Updated by developer */
#define STDIO_SCIFA_RZ_HLD_VERSION_MINOR (1)

/** Build Number of API.
 * Generated during customer release */
#define STDIO_SCIFA_RZ_HLD_BUILD_NUM (0)

/** Unique ID. Assigned by requirements */
#define STDIO_SCIFA_RZ_HLD_UID (77)

/** List channels supported */
#define R_CFG_SCIFA_CHANNELS_SUPPORTED_OLD (5)

/** Maximum number of modules that can be granted simultaneous access to this driver  */
#define R_CFG_SCIFA_DRV_MAXIMUM_ACCESS_PRV (1)


/**
 * @typedef SCIFA_SC_IF_CONTROL Control Interface
 */
typedef enum
{
    CTL_SCIFA_SET_CONFIGURATION = (STDIO_SCIFA_RZ_HLD_UID<<16) + 0, /*!< Sets up SCIFA peripheral */
    CTL_SCIFA_GET_CONFIGURATION,                                    /*!< Gets current SCIFA configuration */
    CTL_SCIFA_DMA_READ_ABORT,                                       /*!< Abort DMA read */
    CTL_SCIFA_DMA_WRITE_ABORT,                                      /*!< Abort DMA write */
    CTL_SCIFA_WRITE_NEXT,                                           /*!< Set DMA next write */
    CTL_SCIFA_READ_NEXT,                                            /*!< Set DMA next read */
    CTL_SCIFA_FLUSH_OUTPUT,                                         /*!< Wait until all data in the TX buffer has been sent */
} e_ctrl_code_scifa_t;

/******************************************************************************
 Constant Data
 ******************************************************************************/

extern const st_r_driver_t g_scifa_driver;

#endif /* RENESAS_DRIVERS_R_SCIFA_DRV_API_H_ */

/* End of File */

