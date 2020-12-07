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
 * Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/*******************************************************************************
 * File Name   : r_gpio_drv_api.c
 * Description : GPIO high layer driver
 *******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_gpio_drv_link.h"
#include "r_gpio_drv_api.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Enumerated Types
 ******************************************************************************/

/******************************************************************************
 Typedefs
 ******************************************************************************/
#define NOT_USED(p)                     ((void)(p))

/*****************************************************************************
 Function Prototypes
 ******************************************************************************/
static int_t gpio_open (st_stream_ptr_t stream_ptr);
static void gpio_close (st_stream_ptr_t stream_ptr);
static int_t gpio_control (st_stream_ptr_t stream_ptr, uint32_t ctrl_code, void *ctrl_ptr);
static int_t gpio_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo);

/*****************************************************************************
 Constant Data
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/*! Version Information */
static const st_drv_info_t gs_hld_info =
{
    {
        ((GPIO_HLD_VERSION_MAJOR << 16) + GPIO_HLD_VERSION_MINOR)
    },
    GPIO_HLD_BUILD_NUM,
    GPIO_HLD_DRV_NAME
};

/* Define the driver function table for this implementation */
/* GSCE Warning (global variable 'g_gpio_Driver' not declared static [9.3a])
 * can be safely ignored */
const st_r_driver_t g_gpio_driver =
{
    /* st_r_driver_t p_device */
    "GPIO Driver",
    gpio_open,
    gpio_close,
    no_dev_read,
    no_dev_write,
    gpio_control,
    gpio_get_version
};

/******************************************************************************
 Function Name: gpio_open
 Description:   Function to open the GPIO device driver
 Arguments:     IN  stream_ptr - Pointer to the file stream
 Return value:  DRV_SUCCESS : Success when attempting to initialise GPIO
 *              DRV_ERROR   : Failure to initialise GPIO
 ******************************************************************************/
static int_t gpio_open (st_stream_ptr_t stream_ptr)
{
    /* initialise GPIO */
    return (R_GPIO_HWInitialise((int16_t) stream_ptr->file_number));
}
/******************************************************************************
 End of function  gpio_open
 ******************************************************************************/

/******************************************************************************
 Function Name: gpio_close
 Description:   Function to close the GPIO driver. All open devices
 connected to the led are closed.
 Arguments:     IN  stream_ptr - Pointer to the file stream
 Return value:  none
 ******************************************************************************/
static void gpio_close (st_stream_ptr_t stream_ptr)
{
    /* mark all pins allocated by this handle as not allocated */
    R_GPIO_DeallocateAllPins((int16_t) stream_ptr->file_number);
}
/******************************************************************************
 End of function  gpio_close
 ******************************************************************************/

/******************************************************************************
 Function Name: gpio_control
 Description:   Function to handle custom controls for the GPIO device
 Arguments:     IN  stream_ptr - Pointer to the file stream
 IN  ctrl_code - The custom control code
 IN  ctrl_ptr - Pointer to the custom control structure
 Return value:  0 for success -1 on error
 ******************************************************************************/
static int_t gpio_control (st_stream_ptr_t stream_ptr, uint32_t ctrl_code, void *ctrl_ptr)
{
    int_t retval = DRV_ERROR;
    st_r_drv_gpio_config_t *p_config_rw;
    st_r_drv_gpio_pin_list_t *p_pin_list;
    st_r_drv_gpio_pin_init_table_t *p_pin_table;
    st_r_drv_gpio_pin_rw_t *p_pin_rw;
    st_r_drv_gpio_port_rw_t *p_port_rw;

    /* control code is of type e_ctrl_code_gpio_t */
    switch ((e_ctrl_code_gpio_t)ctrl_code)
    {
        case CTL_GPIO_SET_CONFIGURATION: /*!< Set pin configuration function */
        {
            /* cast pointer to configuration struct type */
            p_config_rw = (st_r_drv_gpio_config_t *) ctrl_ptr;
            p_config_rw->err = R_GPIO_PinSetConfiguration((int16_t) stream_ptr->file_number, &p_config_rw->config);
            if (GPIO_SUCCESS == p_config_rw->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_GET_CONFIGURATION: /*!< Get pin configuration function */
        {
            /* cast pointer to configuration struct type */
            p_config_rw = (st_r_drv_gpio_config_t *) ctrl_ptr;
            p_config_rw->err = R_GPIO_PinGetConfiguration(&p_config_rw->config);
            if (GPIO_SUCCESS == p_config_rw->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_INIT_BY_PIN_LIST: /*!< Set pin configuration from a pin list function */
        {
            /* cast pointer to pin list struct type */
            p_pin_list = (st_r_drv_gpio_pin_list_t *) ctrl_ptr;
            p_pin_list->err = R_GPIO_InitByPinList((int16_t) stream_ptr->file_number, p_pin_list->p_pin_list, p_pin_list->count);
            if (GPIO_SUCCESS == p_pin_list->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_CLEAR_BY_PIN_LIST: /*!< Resets (deallocates) pins from a pin list function */
        {
            /* cast pointer to pin list struct type */
            p_pin_list = (st_r_drv_gpio_pin_list_t *) ctrl_ptr;
            p_pin_list->err = R_GPIO_ClearByPinList(p_pin_list->p_pin_list, p_pin_list->count);
            if (GPIO_SUCCESS == p_pin_list->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_INIT_BY_TABLE: /*!< Set pin configuration from a pin table function */
        {
            /* cast pointer to table struct type */
            p_pin_table = (st_r_drv_gpio_pin_init_table_t *) ctrl_ptr;
            p_pin_table->err = R_GPIO_InitByTable((int16_t) stream_ptr->file_number, p_pin_table->p_config_table, p_pin_table->count);
            if (GPIO_SUCCESS == p_pin_table->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_CLEAR_BY_TABLE: /*!< Resets (deallocates) pin configuration from a pin table */
        {
            /* cast pointer to table struct type */
            p_pin_table = (st_r_drv_gpio_pin_init_table_t *) ctrl_ptr;
            p_pin_table->err = R_GPIO_ClearByTable(p_pin_table->p_config_table, p_pin_table->count);
            if (GPIO_SUCCESS == p_pin_table->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_PORT_WRITE: /*!< Write port value function */
        {
            /* cast pointer to port read/write struct type */
            p_port_rw = (st_r_drv_gpio_port_rw_t *) ctrl_ptr;
            p_port_rw->err = R_GPIO_PortWrite(p_port_rw->port, p_port_rw->value);
            if (GPIO_SUCCESS == p_port_rw->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_PORT_READ: /*!< Read port value function */
        {
            /* cast pointer to port read/write struct type */
            p_port_rw = (st_r_drv_gpio_port_rw_t *) ctrl_ptr;
            p_port_rw->err = R_GPIO_PortRead(p_port_rw->port, &p_port_rw->value);
            if (GPIO_SUCCESS == p_port_rw->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_PIN_WRITE: /*!< Write pin level function */
        {
            /* cast pointer to port read/write struct type */
            p_pin_rw = (st_r_drv_gpio_pin_rw_t *) ctrl_ptr;
            p_pin_rw->err = R_GPIO_PinWrite(p_pin_rw->pin, p_pin_rw->level);
            if (GPIO_SUCCESS == p_pin_rw->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        case CTL_GPIO_PIN_READ: /*!< Read pin level function */
        {
            /* cast pointer to port read/write struct type */
            p_pin_rw = (st_r_drv_gpio_pin_rw_t *) ctrl_ptr;
            p_pin_rw->err = R_GPIO_PinRead(p_pin_rw->pin, &p_pin_rw->level);
            if (GPIO_SUCCESS == p_pin_rw->err)
            {
                retval = DRV_SUCCESS;
            }
            break;
        }

        default:
        {
            retval = DRV_ERROR;
            break;
        }
    }

    return (retval);
}
/******************************************************************************
 End of function  gpio_control
 ******************************************************************************/

/*******************************************************************************
 * Function Name: gpio_get_version
 * Description  : Provides build information even if driver fails to open
 *                version information is updated by developer
 * Arguments    : none
 * Return Value : DRV_SUCCESS for success DRV_ERROR on error
 ******************************************************************************/
static int_t gpio_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo)
{
    /* get version */
    NOT_USED(stream_ptr);

    pVerInfo->hld.version.sub.major = gs_hld_info.version.sub.major;
    pVerInfo->hld.version.sub.minor = gs_hld_info.version.sub.minor;
    pVerInfo->hld.build = gs_hld_info.build;
    pVerInfo->hld.p_szdriver_name = gs_hld_info.p_szdriver_name;

    /* Obtain version information from Low layer Driver */
    return R_GPIO_GetVersion(pVerInfo);
}
/*******************************************************************************
 End of function gpio_get_version
 ******************************************************************************/

/* End of File */

