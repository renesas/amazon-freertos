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
 * File Name    : r_gpio_drv_api.h
 * Description  : GPIO driver API header
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup RZA2M_XXX_GPIO_DRIVER GPIO API Interface
 * @brief This interface allows control of the GPIO module
 *
 * @section GPIO_DRIVER_SUMMARY Summary
 * @brief The driver supports STDIO or direct implementation for controlling the GPIO Module (GPIO).
 * This High Layer driver (HLD) requires an associated low layer implementation specific to the platform.
 * The HLD exposes a global for the GPIO driver, this must only be accessed directly if used in an OS Free implementation.
 * @brief Before accessing the driver, call the open function to configure the selected hardware interface, this must be
 * implemented in the Lower Driver layer. When open the control function can be used to control the operation of the GPIO.
 *
 * When finished with the driver close it using the close function.
 *
 * This driver does not allocate any heap memory.
 *
 * This driver does not have any locking primitives.
 *
 * @par Implemented as
   <pre>
   const st_r_driver_t g_stdio_scifa_driver =
   {
     Device Driver Name             "GPIO Driver",
     Device Driver Open             gpio_open,
     Device Driver Close            gpio_close,
     Device Driver Read Function    no_dev_read,
     Device Driver Write Function   no_dev_write,
     Device Driver Control Function gpio_control
     Device Driver Version Function gpio_get_version
   };
   </pre>
 * See : ::st_def_stdio_device_t
 *
 * @par Control interface e_ctrl_code_gpio_t;
 * See : ::e_ctrl_code_gpio_t
 *
 * @section RZA2M_GPIO_DRIVER_API_LIMITATIONS Known Limitations
 * NONE_YET
 *
 * @section RZA2M_GPIO_DRIVER_API_INSTANCES Known Implementations
 * @ref NONE_YET
 *
 * @section RZA2M_GPIO_DRIVER_API_RELATED Related modules
 * See also:
 * @ref DS_BOARD_SUPPORT,
 * @ref XXXXXXXXXX,
 * @ref OS_ABSTRACT
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_devlink_wrapper.h"
#include "r_gpio_drv_link.h"

#ifndef R_GPIO_INC_R_GPIO_DRV_API_H_
#define R_GPIO_INC_R_GPIO_DRV_API_H_

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#define GPIO_HLD_DRV_NAME           ("HLD GPIO")

/** Major Version Number of API.
 * Updated by product owner */
#define GPIO_HLD_VERSION_MAJOR      (1)

/** Minor Version Number of API.
 * Updated by developer */
#define GPIO_HLD_VERSION_MINOR      (4)

/** Build Number of API.
 * Generated during customer release */
#define GPIO_HLD_BUILD_NUM          (0)

/** Unique ID. Assigned by requirements */
#define GPIO_HLD_UID                (59)

/******************************************************************************
 Constant Data
 ******************************************************************************/

/* Define the driver function table for this implementation */
/* GSCE Warning (global variable 'g_gpio_driver' not declared static [9.3a])
 * can be safely ignored */
extern const st_r_driver_t g_gpio_driver;

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/* High Layer Structure and Enumeration Interface */

typedef enum
{
    CTL_GPIO_SET_CONFIGURATION = 0u,        /*!< Set pin configuration GPIO control function */
    CTL_GPIO_GET_CONFIGURATION,             /*!< Get pin configuration GPIO control function */
    CTL_GPIO_INIT_BY_PIN_LIST,              /*!< Configure specified pins by list control function */
    CTL_GPIO_CLEAR_BY_PIN_LIST,             /*!< Clear (deallocate) specified pins by list control function */
    CTL_GPIO_INIT_BY_TABLE,                 /*!< Configure pins by table control function */
    CTL_GPIO_CLEAR_BY_TABLE,                /*!< Clear (deallocate) pins by table control function */
    CTL_GPIO_PORT_WRITE,                    /*!< Write port data control function */
    CTL_GPIO_PORT_READ,                     /*!< Read port data control function */
    CTL_GPIO_PIN_WRITE,                     /*!< Write pin data control function */
    CTL_GPIO_PIN_READ,                      /*!< Read pin data control function */
} e_ctrl_code_gpio_t;

/* Control interface pin configuration */

typedef struct
{
    st_r_drv_gpio_sc_config_t config;   /*!< pin and configuration data */
    e_r_drv_gpio_err_t        err;      /*!< error code */
} st_r_drv_gpio_config_t;

typedef struct
{
    const st_r_drv_gpio_sc_config_t * p_config_table;   /*!< pin configuration table */
    uint32_t count;                                     /*!< configuration count */
    e_r_drv_gpio_err_t err;                             /*!< error code (see e_r_drv_gpio_err_t) */
} st_r_drv_gpio_pin_init_table_t;


/* Control interface port / pin read and write */

typedef struct
{
    r_gpio_port_pin_t    pin;           /*!< port / pin */
    e_r_drv_gpio_level_t level;         /*!< logic level */
    e_r_drv_gpio_err_t   err;           /*!< error code */
} st_r_drv_gpio_pin_rw_t;

typedef struct
{
    r_gpio_port_t         port;         /*!< port name */
    uint8_t               value;        /*!< logic levels of all pins on the port */
    e_r_drv_gpio_err_t    err;          /*!< error code */
} st_r_drv_gpio_port_rw_t;


/* Pin list structures */

typedef struct
{
    const r_gpio_port_pin_t *p_pin_list;    /*!< list of pins */
    uint32_t count;                         /*!< number of pins in the list */
    e_r_drv_gpio_err_t err;                 /*!< error code */
} st_r_drv_gpio_pin_list_t;


#endif  /* R_GPIO_INC_R_GPIO_DRV_API_H_ */

/*******************************************************************************************************************//**
 * @} (end addtogroup RZA2M_XXX_GPIO_DRIVER)
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
  End  Of File
 ***********************************************************************************************************************/
