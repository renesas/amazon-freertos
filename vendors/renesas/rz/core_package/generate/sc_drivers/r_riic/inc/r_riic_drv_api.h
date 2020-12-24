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
 * Renesas reserves the right, without notice, to make changes to this software
 * and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/


/**************************************************************************//**
 * @headerfile     r_riic_drv_api.h
 * @brief          This is the SSGSTARC-56 High Level Driver for RIIC
 *****************************************************************************/
/*****************************************************************************
 * @version        See history
 *****************************************************************************/
/**********************************************************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 15.05.2018 1.00    Release
 *               : 01.08.2019 1.03    Bump version number for V5 release
 *********************************************************************************************************************/

/**************************************************************************//**
 * @ingroup SSGSTARC_56_RIIC_API
 * @defgroup SSGSTARC_56_RIIC_HLD_API RIIC High Level Driver
 * @brief This interface provides access to RIIC Peripheral.
 * @anchor SSGSTARC_56_RIIC_HLD_API_SUMMARY
 * @par Summary
 * @brief The driver implements High Level access to the RIIC Peripheral.
 * @brief Before accessing the driver, call the open function to configure the
 * selected hardware interface.
 *
 * When finished with the driver close it using the close function.
 *
 * This driver does not allocate any heap memory.
 *
 * This driver uses locking primitives to control access to the I2C
 * communications bus.<BR>
 *
 * Driver Instance : @ref g_riic_driver </b><BR>
 * @copydoc g_riic_driver
 *
 * <b>Generic example of driver usage</b> ::st_def_stdio_device_t
 *
 * @anchor SSGSTARC_56_RIIC_DRIVER_HLD_API_LIMITATIONS
 * @par Known Limitations
 * @ref NONE_YET
 *
 * @anchor SSGSTARC_56_RIIC_API_INSTANCES
 * @par Known Implementations:
 * @ref RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @ref RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 *
 * @ref R_SW_PKG_93_EEPROM_API
 * @{
 *****************************************************************************/
/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_devlink_wrapper.h"
#include "r_riic_drv_link.h"

/* Multiple inclusion prevention macro */
#ifndef RENESAS_DRIVERS_R_RIIC_INC_R_RIIC_DRV_API_H_
    #define RENESAS_DRIVERS_R_RIIC_INC_R_RIIC_DRV_API_H_

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/** Driver Name */
    #define STDIO_RIIC_HLD_DRV_NAME             ("HLD RIIC")

/** Major Version Number of API.
 * Updated by product owner */
    #define STDIO_RIIC_HLD_VERSION_MAJOR        (1)

/** Minor Version Number of API.
 * Updated by developer */
    #define STDIO_RIIC_HLD_VERSION_MINOR        (4)

/** Build Number of API.
 * Generated during customer release */
    #define STDIO_RIIC_HLD_BUILD_NUM            (0)

/** Unique ID. Assigned by requirements */
    #define STDIO_RIIC_HLD_UID                  (56)

/* Maximum number of modules that can be granted
 * simultaneous access to this driver  */
    #define  R_CFG_RIIC_DRV_MAXIMUM_ACCESS_PRV      (1)

/**
 * @typedef RIIC_SC_IF_CONTROL Control Interface
 */
typedef enum
{
    CTL_RIIC_SET_CONFIG = 0, /*!< Set channel configuration */
    CTL_RIIC_GET_CONFIG = 1, /*!< Get channel configuration */
    CTL_RIIC_SET_MODE = 2, /*!< Set channel mode (Master, slave or SMBus) */
    CTL_RIIC_READ = 3, /*!< Read from IIC device */
    CTL_RIIC_WRITE = 4 /*!< Write to IIC device */
} e_riic_ctrl_code_t;

/******************************************************************************
 Constant Data
 ******************************************************************************/

extern const st_r_driver_t g_riic_driver;

/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
#endif /* RENESAS_DRIVERS_R_RIIC_INC_R_RIIC_DRV_API_H_ */

