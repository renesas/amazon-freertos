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
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_gpio_lld_rza2m.c
 * Device(s)    : RZ/A2M
 * Tool-Chain   : e2Studio Ver 7.5.0
 *              : GNU Arm Embedded Toolchain 6-2017-q2-update
 * OS           : None
 * H/W Platform : RZ/A2M Evaluation Board
 * Description  : RZ/A2M Sample Program - Low level function of GPIO module
 * Operation    :
 * Limitations  :
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <string.h>
#include "r_typedefs.h"
#include "r_compiler_abstraction_api.h"
#include "r_os_abstraction_api.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "r_gpio_drv_api.h"
#include "rza_io_regrw.h"
#include "r_gpio_drv_sc_cfg.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/*
 * This control SHOULD be in r_gpio_drv_sc_cfg.h BUT when sc runs the contents of this file
 * is overwritten (in error), as only sections marked (This code is auto-generated. Do not edit manually)
 * should be modified. When the move to putting auto-generated table code into .c files is addressed
 * this issue shall be resolved
 */
#define GPIO_CFG_PARAM_CHECKING_ENABLE           (1)

/* DSCR pin current driving capability settings */
#define DSCR_REG_SETTING_8MA_PRV_                (3u)
#define DSCR_REG_SETTING_4MA_PRV_                (1u)

/* DSCR pin current driving capability for QSPI pins */
#define DSCR_QSPI_REG_SETTING_8MA_PRV_           (1u)
#define DSCR_QSPI_REG_SETTING_12MA_PRV_          (3u)

/* PDR register direction setting enumerations */
#define PDR_REG_SETTING_DIRECTION_INPUT_PRV_     (2u)
#define PDR_REG_SETTING_DIRECTION_OUTPUT_PRV_    (3u)
#define PDR_REG_SETTING_DIRECTION_HIZ_PRV_       (0u)

/* PCKIO register setting for drive capability */
#define PCKIO_REG_SETTING_8MA_PRV_               (1u)
#define PCKIO_REG_SETTING_12MA_PRV_              (2u)

#define PIN_NOT_ALLOCATED_PRV_                   (-1)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum
{
    GPIO_NOT_IN_USE,                /** Not configured */
    GPIO_IN_USE,                    /** Configured */
} e_r_drv_gpio_inuse_t;

typedef struct
{
    volatile void * const p_reg;    /** Register address */
    uint8_t               shift;    /** bit shift */
    uint32_t              mask;     /** bit mask */
} st_r_reg_shift_mask_t;


/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

static bool_t is_dedicated_pin(r_gpio_port_pin_t pin);
static e_r_drv_gpio_err_t deallocate_pin(r_gpio_port_pin_t port_pin);

/** Table of valid GPIO functionality */
static const uint8_t s_r_drv_gpio_support[] =
{
    0x7F, 0x1F, 0x0F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF,            /** implemented pins */
    0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x7F, 0xFF,
    0xFF, 0x7F, 0xFF, 0x3F, 0x1F, 0x03
};

static const uint8_t s_r_drv_gpio_dscr_8ma_support[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,            /** DSCR supported pins for 8mA */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFC, 0x00, 0x7F, 0x00, 0x00, 0x00
};

static const uint8_t s_r_drv_gpio_dscr_4ma_support[] =
{
    0x7F, 0x1F, 0x0F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF,            /** DSCR supported pins for 4mA */
    0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x7F, 0xFF,
    0xFF, 0x7F, 0xFF, 0x3F, 0x1F, 0x00
};

/*! Table of "In-use" pin locking */
static int16_t s_gpio_pin_allocation[GPIO_PORT_MAX * 8];       /*!< the handle that has allocated the pin */

/** LLD Version Information */
static const st_drv_info_t s_lld_info =
{
    {
        ((GPIO_LLD_VERSION_MAJOR << 16) + GPIO_LLD_VERSION_MINOR)
    },
    GPIO_LLD_BUILD_NUM,
    GPIO_RZ_LLD_DRV_NAME
};

/** flags for checking first time initialised */
static bool_t s_r_drv_gpio_initialised = false;

/** 2bit width register's (DSCR, etc) pin mask array */
static const uint32_t s_mask_2bit_register[] =
{
    0x03uL << 0,
    0x03uL << 2,
    0x03uL << 4,
    0x03uL << 6,
    0x03uL << 8,
    0x03uL << 10,
    0x03uL << 12,
    0x03uL << 14,
    0x03uL << 16,
    0x03uL << 18,
    0x03uL << 20,
    0x03uL << 22,
    0x03uL << 24,
    0x03uL << 26,
    0x03uL << 28,
    0x03uL << 30,
};

/** 2 bit width registers (DSCR, etc) pin shift array */
static const uint32_t s_shift_2bit_register[] =
{
    0,
    2,
    4,
    6,
    8,
    10,
    12,
    14,
    16,
    18,
    20,
    22,
    24,
    26,
    28,
    30,
};

/** 1 bit width registers (PMR, etc) pin mask array */
static const uint32_t s_mask_1bit_register[] =
{
    0x01uL << 0,
    0x01uL << 1,
    0x01uL << 2,
    0x01uL << 3,
    0x01uL << 4,
    0x01uL << 5,
    0x01uL << 6,
    0x01uL << 7,
    0x01uL << 8,
    0x01uL << 9,
    0x01uL << 10,
    0x01uL << 11,
    0x01uL << 12,
    0x01uL << 13,
    0x01uL << 14,
    0x01uL << 15,
};

/** 1 bit width registers (PMR) pin shift array */
static const uint32_t s_shift_1bit_register[] =
{
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
};

/** SDMMC0 group driver strength register table */
static const st_r_reg_shift_mask_t s_sd0_reg[] =
{
    {&GPIO.PSDMMC0.LONG, GPIO_PSDMMC0_SD0_CLK_DRV_SHIFT,  GPIO_PSDMMC0_SD0_CLK_DRV},  /** GPIO_PIN_SD0_CLK */
    {&GPIO.PSDMMC0.LONG, GPIO_PSDMMC0_SD0_CMD_DRV_SHIFT,  GPIO_PSDMMC0_SD0_CMD_DRV},  /** GPIO_PIN_SD0_CMD */
    {&GPIO.PSDMMC0.LONG, GPIO_PSDMMC0_SD0_DAT0_DRV_SHIFT, GPIO_PSDMMC0_SD0_DAT0_DRV}, /** GPIO_PIN_SD0_DAT0 */
    {&GPIO.PSDMMC0.LONG, GPIO_PSDMMC0_SD0_DAT1_DRV_SHIFT, GPIO_PSDMMC0_SD0_DAT1_DRV}, /** GPIO_PIN_SD0_DAT1 */
    {&GPIO.PSDMMC0.LONG, GPIO_PSDMMC0_SD0_DAT2_DRV_SHIFT, GPIO_PSDMMC0_SD0_DAT2_DRV}, /** GPIO_PIN_SD0_DAT2 */
    {&GPIO.PSDMMC0.LONG, GPIO_PSDMMC0_SD0_DAT3_DRV_SHIFT, GPIO_PSDMMC0_SD0_DAT3_DRV}, /** GPIO_PIN_SD0_DAT3 */
    {&GPIO.PSDMMC1.LONG, GPIO_PSDMMC1_SD0_DAT4_DRV_SHIFT, GPIO_PSDMMC1_SD0_DAT4_DRV}, /** GPIO_PIN_SD0_DAT4 */
    {&GPIO.PSDMMC1.LONG, GPIO_PSDMMC1_SD0_DAT5_DRV_SHIFT, GPIO_PSDMMC1_SD0_DAT5_DRV}, /** GPIO_PIN_SD0_DAT5 */
    {&GPIO.PSDMMC1.LONG, GPIO_PSDMMC1_SD0_DAT6_DRV_SHIFT, GPIO_PSDMMC1_SD0_DAT6_DRV}, /** GPIO_PIN_SD0_DAT6 */
    {&GPIO.PSDMMC1.LONG, GPIO_PSDMMC1_SD0_DAT7_DRV_SHIFT, GPIO_PSDMMC1_SD0_DAT7_DRV}, /** GPIO_PIN_SD0_DAT7 */
    {&GPIO.PSDMMC1.LONG, GPIO_PSDMMC1_SD0_RSTN_DRV_SHIFT, GPIO_PSDMMC1_SD0_RSTN_DRV}, /** GPIO_PIN_SD0_RST */
};

/** SDMMC1 group driver strength register table */
static const st_r_reg_shift_mask_t s_sd1_reg[] =
{
    {&GPIO.PSDMMC2.LONG, GPIO_PSDMMC2_SD1_CLK_DRV_SHIFT,  GPIO_PSDMMC2_SD1_CLK_DRV},  /** GPIO_PIN_SD1_CLK */
    {&GPIO.PSDMMC2.LONG, GPIO_PSDMMC2_SD1_CMD_DRV_SHIFT,  GPIO_PSDMMC2_SD1_CMD_DRV},  /** GPIO_PIN_SD1_CMD */
    {&GPIO.PSDMMC2.LONG, GPIO_PSDMMC2_SD1_DAT0_DRV_SHIFT, GPIO_PSDMMC2_SD1_DAT0_DRV}, /** GPIO_PIN_SD1_DAT0 */
    {&GPIO.PSDMMC2.LONG, GPIO_PSDMMC2_SD1_DAT1_DRV_SHIFT, GPIO_PSDMMC2_SD1_DAT1_DRV}, /** GPIO_PIN_SD1_DAT1 */
    {&GPIO.PSDMMC2.LONG, GPIO_PSDMMC2_SD1_DAT2_DRV_SHIFT, GPIO_PSDMMC2_SD1_DAT2_DRV}, /** GPIO_PIN_SD1_DAT2 */
    {&GPIO.PSDMMC2.LONG, GPIO_PSDMMC2_SD1_DAT3_DRV_SHIFT, GPIO_PSDMMC2_SD1_DAT3_DRV}, /** GPIO_PIN_SD1_DAT3 */
};

/** SPIBSC group driver strength register table */
static const st_r_reg_shift_mask_t s_qspi_reg[] =
{
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI0_SPCLK_DRV_SHIFT, GPIO_PSPIBSC_QSPI0_SPCLK_DRV},/** GPIO_PIN_QSPI0_SPCLK */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI0_IO0_DRV_SHIFT,   GPIO_PSPIBSC_QSPI0_IO0_DRV},  /** GPIO_PIN_QSPI0_IO0 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI0_IO1_DRV_SHIFT,   GPIO_PSPIBSC_QSPI0_IO1_DRV},  /** GPIO_PIN_QSPI0_IO1 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI0_IO2_DRV_SHIFT,   GPIO_PSPIBSC_QSPI0_IO2_DRV},  /** GPIO_PIN_QSPI0_IO2 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI0_IO3_DRV_SHIFT,   GPIO_PSPIBSC_QSPI0_IO3_DRV},  /** GPIO_PIN_QSPI0_IO3 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI0_SSL_DRV_SHIFT,   GPIO_PSPIBSC_QSPI0_SSL_DRV},  /** GPIO_PIN_QSPI0_SSL */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_RPC_RESETN_DRV_SHIFT,  GPIO_PSPIBSC_RPC_RESETN_DRV}, /** GPIO_PIN_RPC_RESET */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_RPC_WPN_DRV_SHIFT,     GPIO_PSPIBSC_RPC_WPN_DRV},    /** GPIO_PIN_RPC_WP */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI1_SPCLK_DRV_SHIFT, GPIO_PSPIBSC_QSPI1_SPCLK_DRV},/** GPIO_PIN_QSPI1_SPCLK */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI1_IO0_DRV_SHIFT,   GPIO_PSPIBSC_QSPI1_IO0_DRV},  /** GPIO_PIN_QSPI1_IO0 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI1_IO1_DRV_SHIFT,   GPIO_PSPIBSC_QSPI1_IO1_DRV},  /** GPIO_PIN_QSPI1_IO1 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI1_IO2_DRV_SHIFT,   GPIO_PSPIBSC_QSPI1_IO2_DRV},  /** GPIO_PIN_QSPI1_IO2 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI1_IO3_DRV_SHIFT,   GPIO_PSPIBSC_QSPI1_IO3_DRV},  /** GPIO_PIN_QSPI1_IO3 */
    {&GPIO.PSPIBSC.LONG, GPIO_PSPIBSC_QSPI1_SSL_DRV_SHIFT,   GPIO_PSPIBSC_QSPI1_SSL_DRV},  /** GPIO_PIN_QSPI1_SSL */
};

/** gpio mutex for serialisation */
static void *sp_gpio_mutex = NULL;

/**
 * @fn         unprotect_pin_mux
 * @brief      This function un-protects PFS registers.
 */
static void unprotect_pin_mux (void)
{
    /* Set B0WI to 0 for PFSWE bit as writable */
    RZA_IO_RegWrite_8(&GPIO.PWPR.BYTE, 0, GPIO_PWPR_B0WI_SHIFT, GPIO_PWPR_B0WI);

    /* Set PFSWE to 1 for PxxPFS register as writable */
    RZA_IO_RegWrite_8(&GPIO.PWPR.BYTE, 1, GPIO_PWPR_PFSWE_SHIFT, GPIO_PWPR_PFSWE);
}
/******************************************************************************
 * End of function unprotect_pin_mux
 ******************************************************************************/

/**
 * @fn         protect_pin_mux
 * @brief      This function protects PFS registers.
 */
static void protect_pin_mux (void)
{
    /* Set PFSWE to 0 for PxxPFS register as not-writable */
    RZA_IO_RegWrite_8(&GPIO.PWPR.BYTE, 0, GPIO_PWPR_PFSWE_SHIFT, GPIO_PWPR_PFSWE);

    /* Set B0WI to 1 for PFSWE bit as not-writable */
    RZA_IO_RegWrite_8(&GPIO.PWPR.BYTE, 1, GPIO_PWPR_B0WI_SHIFT, GPIO_PWPR_B0WI);
}
/******************************************************************************
 * End of function protect_pin_mux
 ******************************************************************************/

/**
 * @fn             pin_function_check
 * @brief          Checks to see if a pin supports a certain function
 * @param[in]      check_array: Which support array to use
 * @param[in]      port_number: Which port to use
 * @param[in]      pin_number: Which pin to use
 * @retval         true     Functionality is supported on this pin
 * @retval         false    Functionality is not supported on this pin
 */
static bool_t pin_function_check (const uint8_t *check_array, uint8_t port_number, uint8_t pin_number)
{
    bool_t res = true;

    /* range check for port */
    if (port_number > GPIO_PORT_MAX)
    {
        /* out of range */
        res = false;
    }

    /* range check for pin
     * 7 : Maximum pin number in RZ/A2M
     * (if porting other device, check if this setting is suitable)
     */
    if (pin_number > 7)
    {
        /* out of range */
        res = false;
    }

#if defined(GPIO_CFG_PARAM_CHECKING_ENABLE) && (0 != GPIO_CFG_PARAM_CHECKING_ENABLE)
    if ((check_array[port_number] & (1u << pin_number)) == 0)
    {
        res = false;
    }
#endif

    return (res);
}
/******************************************************************************
 * End of function pin_function_check
 ******************************************************************************/

/**
 * @fn             make_port_num
 * @brief          Make port number from port name
 * @param[in]      port: port name
 * @param[out]     p_port_number: port number
 */
static void make_port_num (r_gpio_port_t port, uint8_t *p_port_number)
{
    /* It is okay for using higher 8bit of unsigned 16bit pin value */
    *p_port_number = (uint8_t) ((port & GPIO_PORT_MASK) >> GPIO_PORT_SHIFT);
}
/******************************************************************************
 * End of function make_port_num
 ******************************************************************************/

/**
 * @fn             make_port_pin_num
 * @brief          Make port number and pin number from pin name
 * @param[in]      pin: pin name
 * @param[out]     p_port_number: port number
 * @param[out]     p_pin_number: pin number
 */
static void make_port_pin_num (r_gpio_port_pin_t pin, uint8_t *p_port_number, uint8_t *p_pin_number)
{
    /* It is okay for using higher 8bit of unsigned 16bit pin value */
    *p_port_number = (uint8_t) ((pin & GPIO_PORT_MASK) >> GPIO_PORT_SHIFT);

    /* It is okay for using lower 8bit of unsigned 16bit pin value */
    *p_pin_number = (uint8_t) (pin & GPIO_PIN_MASK);
}
/******************************************************************************
 * End of function make_port_pin_num
 ******************************************************************************/

/**
 * @fn             is_dedicated_pin
 * @brief          Checks if the pin is dedicated
 * @param[in]      pin:    Pin number
 * @retval         true    This is a dedicated pin
 * @retval         false   This is not a dedicated pin
 */
static bool_t is_dedicated_pin (r_gpio_port_pin_t pin)
{
    /* In RZ/A2M the dedicated pin are assigned a value more than GPIO_DEDICATED_PIN_MIN(0x8000) */
    if (pin >= GPIO_DEDICATED_PIN_MIN)
    {
        return (true);
    }

    return (false);
}
/******************************************************************************
 * End of function is_dedicated_pin
 ******************************************************************************/

/**
 * @fn             allocate_pin
 * @brief          allocate pin to a particular handle
 * @param[in]      port_pin: port / pin
 * @param[in]      handle: handle that is configuring the pin
 * @retval         GPIO_SUCCESS         pin successfully allocated
 * @retval         GPIO_ERR_INVALID_PIN pin does not exist on this device
 */
static e_r_drv_gpio_err_t allocate_pin(r_gpio_port_pin_t port_pin, int16_t handle)
{
    uint8_t port;
    uint8_t pin;

    make_port_pin_num(port_pin, &port, &pin);

    /* check for valid pin */
    if (!pin_function_check(s_r_drv_gpio_support, port, pin))
    {
        return (GPIO_ERR_INVALID_PIN);
    }

    /* set the allocation of the pin to the handle */
    s_gpio_pin_allocation[(port * 8) + pin] = handle;

    return (GPIO_SUCCESS);
}
/******************************************************************************
 * End of function allocate_pin
 *****************************************************************************/

/**
 * @fn             deallocate_pin
 * @brief          deallocate a pin
 * @param[in]      port_pin: port / pin
 * @retval         GPIO_SUCCESS         pin successfully deallocated
 * @retval         GPIO_ERR_INVALID_PIN pin does not exist on this device
 */
static e_r_drv_gpio_err_t deallocate_pin(r_gpio_port_pin_t port_pin)
{
    /* set the allocation of the pin to PIN_NOT_ALLOCATED_PRV_ */
    return (allocate_pin(port_pin, PIN_NOT_ALLOCATED_PRV_));
}
/******************************************************************************
 * End of function deallocate_pin
 *****************************************************************************/

/**
 * @fn             get_pin_allocation
 * @brief          get the handle that the pin is currently allocated to
 * @param[in]      port_pin: port / pin
 * @param[out]     allocation: the handle that the pin is allocated to
 *                 or PIN_NOT_ALLOCATED_PRV_ if it isn't allocated
 * @retval         GPIO_SUCCESS         pin allocation successfully retrieved
 * @retval         GPIO_ERR_INVALID_PIN pin does not exist on this device
 */
static e_r_drv_gpio_err_t get_pin_allocation(r_gpio_port_pin_t port_pin, int16_t *allocation)
{
    uint8_t port;
    uint8_t pin;

    /* dedicated pin? */
    if ((port_pin >= GPIO_DEDICATED_PIN_MIN) && (port_pin <= GPIO_PIN_SPIBSC_ALL))
    {
        /* dedicated pins are always not allocated */
        *allocation = PIN_NOT_ALLOCATED_PRV_;
        return (GPIO_SUCCESS);
    }

    make_port_pin_num(port_pin, &port, &pin);

    /* check for valid pin */
    if (!pin_function_check(s_r_drv_gpio_support, port, pin))
    {
        return (GPIO_ERR_INVALID_PIN);
    }

    *allocation = s_gpio_pin_allocation[(port * 8) + pin];

    return (GPIO_SUCCESS);
}
/******************************************************************************
 * End of function get_pin_allocation
 *****************************************************************************/

/**
 * @fn             deallocate_all_pins
 * @brief          set all pins to not allocated
 * @retval         None
 */
static void deallocate_all_pins(void)
{
    uint16_t i;

    /* set all allocation handles to PIN_NOT_ALLOCATED_PRV_ */
    for (i = 0; i < ((sizeof(s_gpio_pin_allocation)) / (sizeof(int16_t))); i++)
    {
        s_gpio_pin_allocation[i] = PIN_NOT_ALLOCATED_PRV_;
    }
}
/******************************************************************************
 * End of function deallocate_all_pins
 *****************************************************************************/

/**
 * @fn             pin_is_available
 * @brief          Checks if the pin is available
 * @param[in]      pin: pin name
 * @retval         true    Functionality is supported on this pin
 * @retval         false   Functionality is not supported on this pin
 */
static bool_t pin_is_available (r_gpio_port_pin_t pin)
{
    bool_t res;
    uint8_t u_port;
    uint8_t u_pin;

    res = true;

    if (is_dedicated_pin(pin))
    {
        /* check if valid dedicated pins */
        R_COMPILER_Nop();
    }
    else
    {
        make_port_pin_num(pin, &u_port, &u_pin);
        res = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);
    }

    return (res);
}
/******************************************************************************
 * End of function pin_is_available
 ******************************************************************************/

/**
 * @fn             scan_sc_config
 * @brief          Obtaining the SC default configuration for the specified pin
 * @param[in]      pin: pin name
 * @return         pointer to the configuration data, or NULL if not found
 */
static const st_r_drv_gpio_pin_config_t *scan_sc_config (r_gpio_port_pin_t pin)
{
    int_t index;
    int_t count;

    count = (sizeof GPIO_SC_TABLE_INIT) / (sizeof *GPIO_SC_TABLE_INIT);

    for (index = 0; index < count; index++)
    {
        if (pin == GPIO_SC_TABLE_INIT[index].pin)
        {
            return &GPIO_SC_TABLE_INIT[index].configuration;
        }
    }

    count = (sizeof GPIO_SC_TABLE_MANUAL) / (sizeof *GPIO_SC_TABLE_MANUAL);

    for (index = 0; index < count; index++)
    {
        if (pin == GPIO_SC_TABLE_MANUAL[index].pin)
        {
            return &GPIO_SC_TABLE_MANUAL[index].configuration;
        }
    }

    return (NULL);
}
/******************************************************************************
 * End of function scan_sc_config
 ******************************************************************************/

/**
 * @fn             set_output_current_pin
 * @brief          Set output current
 * @param[in]      pin: which pin to set
 * @param[in]      current: set current to
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t set_output_current_pin (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t current)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;
    volatile uint16_t *p_pdr;

    uint16_t dscr;
    uint16_t mask_16;
    uint16_t shift;

    err = GPIO_SUCCESS;
    dscr = 0;
    avail = true;

    /* get SC default setting */
    if (GPIO_CURRENT_SC_DEFAULT == current)
    {
        /* override from default pin level if specified as SC_DEFAULT */
        const st_r_drv_gpio_pin_config_t *p_config = scan_sc_config(pin);
        if (NULL == p_config)
        {
            /* configuration error if not exist in SC table */
            err = GPIO_ERR_INVALID_CFG;
        }
        else
        {
            current = p_config->current;
        }
    }

    if (GPIO_SUCCESS == err)
    {
        /* check if valid general purpose I/O pin for selected current */
        /* and set DSCR value
         *   GPIO_CURRENT_4mA:     4mA
         *   GPIO_CURRENT_8mA:     8mA
         *   other: invalid
         */
        make_port_pin_num(pin, &u_port, &u_pin);

        if (GPIO_CURRENT_8mA == current)
        {
            avail = pin_function_check(s_r_drv_gpio_dscr_8ma_support, u_port, u_pin);
            dscr = 3;
        }
        else if (GPIO_CURRENT_4mA == current)
        {
            avail = pin_function_check(s_r_drv_gpio_dscr_4ma_support, u_port, u_pin);
            dscr = 1;
        }
        else
        {
            /* not supported current */
            err = GPIO_ERR_INVALID_CFG;
        }
    }

    if ((GPIO_SUCCESS == err) && (!avail))
    {
        /* not avail on device */
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        /* each DSCR bit data have 2bit width */
        shift = (uint16_t) s_shift_2bit_register[u_pin];
        mask_16 = (uint16_t) s_mask_2bit_register[u_pin];

        p_pdr = &PORT0.DSCR.WORD;
        RZA_IO_RegWrite_16(&p_pdr[u_port], dscr, shift, mask_16);
    }

    return (err);
}
/******************************************************************************
 * End of function set_output_current_pin
 ******************************************************************************/

/**
 * @fn             get_output_current_pin
 * @brief          Obtaining output current
 * @param[in]      pin: which pin to set
 * @param[out]     p_current: obtained current
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t get_output_current_pin (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t *p_current)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;
    volatile uint16_t *p_pdr;

    uint16_t dscr;
    uint16_t mask_16;
    uint16_t shift;

    err = GPIO_SUCCESS;

    /* check if valid 8mA DSCR I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_dscr_8ma_support, u_port, u_pin);

    if (!avail)
    {
        /* not avail 8mA setting on device : check 4mA availability */
        avail = pin_function_check(s_r_drv_gpio_dscr_4ma_support, u_port, u_pin);
    }

    if (!avail)
    {
        /* both 4mA and 8mA is not avail */
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        /* each DSCR bit data have 2bit width */
        shift = (uint16_t) s_shift_2bit_register[u_pin];
        mask_16 = (uint16_t) s_mask_2bit_register[u_pin];

        p_pdr = &PORT0.DSCR.WORD;
        dscr = RZA_IO_RegRead_16(&p_pdr[u_port], shift, mask_16);

        switch (dscr)
        {
            /* DSCR value */
            /*   DSCR_REG_SETTING_4MA_PRV_:     4mA */
            /*   DSCR_REG_SETTING_8MA_PRV_:     8mA */
            /*   other: invalid */
            case DSCR_REG_SETTING_8MA_PRV_: /* 8mA */
            {
                *p_current = GPIO_CURRENT_8mA;
                break;
            }

            case DSCR_REG_SETTING_4MA_PRV_: /* 4mA */
            {
                *p_current = GPIO_CURRENT_4mA;
                break;
            }

            default:
            {
                err = GPIO_ERR_INVALID_CFG;
                break;
            }
        }
    }

    return (err);
}
/******************************************************************************
 * End of function get_output_current_pin
 ******************************************************************************/

/**
 * @fn             set_output_current_ckio
 * @brief          Set PCKIO register
 * @param[in]      pckio: value to set
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t set_output_current_ckio (e_r_drv_gpio_current_t current)
{
    e_r_drv_gpio_err_t err = GPIO_SUCCESS;

    switch (current)
    {
        case GPIO_CURRENT_8mA: /* 8mA */
        {
            RZA_IO_RegWrite_8(&GPIO.PCKIO.BYTE, PCKIO_REG_SETTING_8MA_PRV_,
                IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
            break;
        }

        case GPIO_CURRENT_12mA: /* 12mA */
        {
            RZA_IO_RegWrite_8(&GPIO.PCKIO.BYTE, PCKIO_REG_SETTING_12MA_PRV_,
                IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
            break;
        }

        default:
        {
            /* invalid value for the CKIO pin */
            err = GPIO_ERR_INVALID_CFG;
            break;
        }
    }

    /* return status of operation */
    return (err);
}
/******************************************************************************
 * End of function set_output_current_ckio
 ******************************************************************************/

/**
 * @fn             get_output_current_ckio
 * @brief          Get PCKIO register
 * @param[out]     p_current: value to get
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t get_output_current_ckio (e_r_drv_gpio_current_t *p_current)
{
    e_r_drv_gpio_err_t err = GPIO_SUCCESS;
    uint8_t byte_read = RZA_IO_RegRead_8(&GPIO.PCKIO.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    switch (byte_read)
    {
        case PCKIO_REG_SETTING_8MA_PRV_: /* 8mA */
        {
            *p_current = GPIO_CURRENT_8mA;
            break;
        }

        case PCKIO_REG_SETTING_12MA_PRV_: /* 12mA */
        {
            *p_current = GPIO_CURRENT_12mA;
            break;
        }

        default:
        {
            err = GPIO_ERR_INVALID_CFG;
            *p_current = GPIO_CURRENT_RESERVED;
            break;
        }
    }

    /* return status of operation */
    return (err);
}
/******************************************************************************
 * End of function get_output_current_ckio
 ******************************************************************************/

/**
 * @fn             set_output_current_sd0
 * @brief          Set output current for dedicated SD0 port
 * @param[in]      pin: which pin to set
 * @param[in]      current: driver strength
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t set_output_current_sd0 (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t current)
{
    e_r_drv_gpio_err_t err;
    uint32_t poc2;
    volatile void *p_reg;
    uint32_t value;
    uint32_t mask;
    uint32_t shift;

    err = GPIO_SUCCESS;

    /* boundary check and error if over */
    if ((pin < GPIO_PIN_SD0_CLK) || (GPIO_PIN_SD0_RST < pin))
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        if (GPIO_PIN_SD0_CLK == pin)
        {
            /* SD0_CLK pin driver strength is only support as 12mA */
            if (GPIO_CURRENT_12mA != current)
            {
                err = GPIO_ERR_INVALID_CFG;
            }
        }
        else
        {
            /* except SD0_CLK pin driver strength is only support as 6mA */
            if (GPIO_CURRENT_6mA != current)
            {
                err = GPIO_ERR_INVALID_CFG;
            }
        }
    }

    if (GPIO_SUCCESS == err)
    {
        /* SD0 drive strength value is affected by poc2 voltage setting. */
        poc2 = RZA_IO_RegRead_32(&GPIO.PPOC.LONG, GPIO_PPOC_POC2_SHIFT, GPIO_PPOC_POC2);
        if (1 == poc2)
        {
            /* if 3.3v, the value must be 2 */
            value = 2;
        }
        else
        {
            /* if 1.8v, the value must be 3 */
            value = 3;
        }

        /* using distance from GPIO_PIN_SD0_CLK as index of gs_sd0_reg */
        p_reg = s_sd0_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_SD0_CLK].p_reg;
        shift = s_sd0_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_SD0_CLK].shift;
        mask = s_sd0_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_SD0_CLK].mask;

        /* write target register */
        RZA_IO_RegWrite_32((volatile uint32_t *) p_reg, value, shift, mask);
    }

    return (err);
}
/******************************************************************************
 * End of function set_output_current_sd0
 ******************************************************************************/

/**
 * @fn             get_output_current_sd0
 * @brief          Get output current for dedicated SD0 port
 * @param[in]      pin: which pin to get
 * @param[out]     p_current: driver strength
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t get_output_current_sd0 (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t *p_current)
{
    e_r_drv_gpio_err_t err;

    err = GPIO_SUCCESS;

    /* boundary check and error if over */
    if ((pin < GPIO_PIN_SD0_CLK) || (GPIO_PIN_SD0_RST < pin))
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        if (GPIO_PIN_SD0_CLK == pin)
        {
            /* SD0_CLK pin driver strength is always 12mA */
            *p_current = GPIO_CURRENT_12mA;
        }
        else
        {
            /* except SD0_CLK pin driver strength is always 6mA */
            *p_current = GPIO_CURRENT_6mA;
        }
    }

    return (err);
}
/******************************************************************************
 * End of function get_output_current_sd0
 ******************************************************************************/

/**
 * @fn             set_output_current_sd1
 * @brief          Set output current for dedicated SD1 port
 * @param[in]      pin: which pin to set
 * @param[in]      current: driver strength
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t set_output_current_sd1 (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t current)
{
    e_r_drv_gpio_err_t err;
    uint32_t poc3;
    volatile void *p_reg;
    uint32_t value;
    uint32_t mask;
    uint32_t shift;

    err = GPIO_SUCCESS;

    /* boundary check and error if over */
    if ((pin < GPIO_PIN_SD1_CLK) || (GPIO_PIN_SD1_DAT3 < pin))
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        if (GPIO_PIN_SD1_CLK == pin)
        {
            /* SD1_CLK pin driver strength is only support as 12mA */
            if (GPIO_CURRENT_12mA != current)
            {
                err = GPIO_ERR_INVALID_CFG;
            }
        }
        else
        {
            /* except SD1_CLK pin driver strength is only support as 6mA */
            if (GPIO_CURRENT_6mA != current)
            {
                err = GPIO_ERR_INVALID_CFG;
            }
        }
    }

    if (GPIO_SUCCESS == err)
    {
        /* SD1 drive strength value is affected by poc3 voltage setting. */
        poc3 = RZA_IO_RegRead_32(&GPIO.PPOC.LONG, GPIO_PPOC_POC3_SHIFT, GPIO_PPOC_POC3);
        if (1 == poc3)
        {
            /* if 3.3v, the value must be 2 */
            value = 2;
        }
        else
        {
            /* if 1.8v, the value must be 3 */
            value = 3;
        }

        /* using distance from GPIO_PIN_SD1_CLK as index of gs_sd1_reg */
        p_reg = s_sd1_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_SD1_CLK].p_reg;
        shift = s_sd1_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_SD1_CLK].shift;
        mask = s_sd1_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_SD1_CLK].mask;

        /* write target register */
        RZA_IO_RegWrite_32((volatile uint32_t *) p_reg, value, shift, mask);
    }

    return (err);
}
/******************************************************************************
 * End of function set_output_current_sd1
 ******************************************************************************/

/**
 * @fn             get_output_current_sd1
 * @brief          Get output current for dedicated SD1 port
 * @param[in]      pin: which pin to get
 * @param[out]     p_current: driver strength
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t get_output_current_sd1 (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t *p_current)
{
    e_r_drv_gpio_err_t err;

    err = GPIO_SUCCESS;

    /* boundary check and error if over */
    if ((pin < GPIO_PIN_SD1_CLK) || (GPIO_PIN_SD1_DAT3 < pin))
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        if (GPIO_PIN_SD1_CLK == pin)
        {
            /* SD1_CLK pin driver strength is always 12mA */
            *p_current = GPIO_CURRENT_12mA;
        }
        else
        {
            /* except SD1_CLK pin driver strength is always 6mA */
            *p_current = GPIO_CURRENT_6mA;
        }
    }

    return (err);
}
/******************************************************************************
 * End of function get_output_current_sd1
 ******************************************************************************/

/**
 * @fn             set_output_current_qspi
 * @brief          Set output current for dedicated QSPI(SPIBSC) port
 * @param[in]      pin: which pin to set
 * @param[in]      current: driver strength
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t set_output_current_qspi (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t current)
{
    e_r_drv_gpio_err_t err;
    uint32_t ppoc;
    uint32_t value;
    uint32_t mask;
    uint32_t shift;
    volatile void *p_reg;

    err = GPIO_SUCCESS;
    value = 0;

    /* boundary check and error if over */
    if ((pin < GPIO_PIN_QSPI0_SPCLK) || (GPIO_PIN_QSPI1_SSL < pin))
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        /* output strength must specific value depends on POC settings */
        ppoc = RZA_IO_RegRead_32(&GPIO.PPOC.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

        /* if both GPIO_PPOC_POCSEL0|GPIO_PPOC_POC0 is set, meaning their electrical characteristics fits as 3.3v */
        ppoc &= (GPIO_PPOC_POCSEL0 | GPIO_PPOC_POC0);
        if ((GPIO_PPOC_POCSEL0 | GPIO_PPOC_POC0) == ppoc)
        {
            /* if 3.3v, the value must be 1 */
            if (GPIO_CURRENT_8mA == current)
            {
                value = 1;
            }
            else
            {
                err = GPIO_ERR_INVALID_CFG;
            }
        }
        else
        {
            /* if 1.8v, the value must be 3 */
            if (GPIO_CURRENT_12mA == current)
            {
                value = 3;
            }
            else
            {
                err = GPIO_ERR_INVALID_CFG;
            }
        }
    }

    if (GPIO_SUCCESS == err)
    {
        /* using distance from GPIO_PIN_SD1_CLK as index of gs_sd1_reg */
        p_reg = s_qspi_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_QSPI0_SPCLK].p_reg;
        shift = s_qspi_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_QSPI0_SPCLK].shift;
        mask = s_qspi_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_QSPI0_SPCLK].mask;

        /* write target register */
        RZA_IO_RegWrite_32((volatile uint32_t *) p_reg, value, shift, mask);
    }

    return (err);
}
/******************************************************************************
 * End of function set_output_current_qspi
 ******************************************************************************/

/**
 * @fn             get_output_current_qspi
 * @brief          Get output current for dedicated QSPI(SPIBSC) port
 * @param[in]      pin: which pin to get
 * @param[out]     p_current: driver strength
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t get_output_current_qspi (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t *p_current)
{
    e_r_drv_gpio_err_t err;
    uint32_t value;
    uint32_t mask;
    uint32_t shift;
    volatile void *p_reg;

    err = GPIO_SUCCESS;

    /* boundary check and error if over */
    if ((pin < GPIO_PIN_QSPI0_SPCLK) || (GPIO_PIN_QSPI1_SSL < pin))
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        /* using distance from GPIO_PIN_SD1_CLK as index of gs_sd1_reg */
        p_reg = s_qspi_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_QSPI0_SPCLK].p_reg;
        shift = s_qspi_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_QSPI0_SPCLK].shift;
        mask = s_qspi_reg[(uint32_t) pin - (uint32_t) GPIO_PIN_QSPI0_SPCLK].mask;

        /* read target register */
        value = RZA_IO_RegRead_32(p_reg, shift, mask);

        switch (value)
        {
            /* value table */
            /* DSCR_QSPI_REG_SETTING_8MA_PRV_ : 8mA */
            /* DSCR_QSPI_REG_SETTING_12MA_PRV_ : 12mA */
            /* All others are invalid */

            case DSCR_QSPI_REG_SETTING_8MA_PRV_:
            {
                *p_current = GPIO_CURRENT_8mA;
                break;
            }
            case DSCR_QSPI_REG_SETTING_12MA_PRV_:
            {
                *p_current = GPIO_CURRENT_12mA;
                break;
            }
            default:
            {
                err = GPIO_ERR_INVALID_CFG;
                break;
            }
        }
    }

    return (err);
}
/******************************************************************************
 * End of function get_output_current_qspi
 ******************************************************************************/

/**
 * @fn             pin_configuration
 * @brief          Get output current for dedicated QSPI(SPIBSC) port
 * @param[in]      pin: pin name
 * @param[in]      config: configuration parameter for specified pin
 * @retval         GPIO_SUCCESS      Successful operation
 * @retval         Not GPIO_SUCCESS  Failure code
 */
static e_r_drv_gpio_err_t pin_configuration (r_gpio_port_pin_t port_pin,
    const st_r_drv_gpio_pin_config_t *config)
{
    uint8_t port;
    uint8_t pin;
    e_r_drv_gpio_err_t err;

    /* check if the pin is a valid general purpose I/O pin */
    make_port_pin_num(port_pin, &port, &pin);

    if (!pin_function_check(s_r_drv_gpio_support, port, pin))
    {
        return (GPIO_ERR_INVALID_PIN);
    }

    /* set DSCR first as it can fail if the pin does not support the current specified */
    err = R_GPIO_PinDscrSet(port_pin, config->current);

    if (GPIO_SUCCESS == err)
    {
        err = R_GPIO_PinSetFunction(port_pin, config->function);
    }

    if (GPIO_SUCCESS == err)
    {
        err = R_GPIO_PinTintSet(port_pin, config->tint);
    }

    return (err);
}
/******************************************************************************
 * End of function pin_configuration
 ******************************************************************************/

/**
 * @fn             R_GPIO_HWInitialise
 * @brief          Prepare the GPIO driver for operation
 * @param[in]      handle: driver instance handle
 * @retval         GPIO_SUCCESS  Successful operation always returned
 */
int_t R_GPIO_HWInitialise (int16_t handle)
{
    uint32_t count;

    /* create mutex if not created yet */
    R_OS_EnterCritical();
    if (NULL == sp_gpio_mutex)
    {
        sp_gpio_mutex = R_OS_MutexCreate();
    }
    R_OS_ExitCritical();

    if (!s_r_drv_gpio_initialised)
    {
        /* mark all pins as not allocated */
        deallocate_all_pins();

        count = (sizeof GPIO_SC_TABLE_INIT) / (sizeof *GPIO_SC_TABLE_INIT);
        R_GPIO_InitByTable(handle, GPIO_SC_TABLE_INIT, count);

        /* set flag as "initialised" */
        s_r_drv_gpio_initialised = true;
    }

    return (GPIO_SUCCESS);
}
/******************************************************************************
 * End of function R_GPIO_HWInitialise
 ******************************************************************************/

/**
 * @fn      R_GPIO_HWUninitialise
 * @brief   Uninitialise the hardware
 * @retval  GPIO_SUCCESS    Successful operation (always)
 */
int_t R_GPIO_HWUninitialise (void)
{
    s_r_drv_gpio_initialised = false;

    return (GPIO_SUCCESS);
}
/******************************************************************************
 * End of function R_GPIO_HWUninitialise
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinSetConfiguration
 * @brief      Set single pin configuration
 * @param[in]  handle: driver instance handle
 * @param[in]  p_config: configuration data
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinSetConfiguration (int16_t handle, const st_r_drv_gpio_sc_config_t *p_config)
{
    e_r_drv_gpio_err_t err;
    int16_t allocation;

    /* serialise with multiple threads */
    R_OS_MutexAcquire(sp_gpio_mutex);

    err = get_pin_allocation(p_config->pin, &allocation);

    if (GPIO_SUCCESS == err)
    {
        /* we can only configure the pin if it is not configured, or already configured by us */
        if ((PIN_NOT_ALLOCATED_PRV_ != allocation) && (allocation != handle))
        {
            err = GPIO_ERR_PIN_ALREADY_IN_USE;
        }
        else
        {
            /* configure the pin */
            err = pin_configuration(p_config->pin, &p_config->configuration);

            /* if we were successful, then mark the pin as in use */
            if (GPIO_SUCCESS == err)
            {
                err = allocate_pin(p_config->pin, handle);
            }
        }
    }

    /* serialise with multiple threads */
    R_OS_MutexRelease(sp_gpio_mutex);

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinSetConfiguration
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinGetConfiguration
 * @brief      Get current pin setting
 * @param[in]  p_config->pin: the pin to get the current setting
 * @param[out] p_config->configuration: the setting
 * @retval     GPIO_SUCCESS  Successful operation.
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinGetConfiguration (st_r_drv_gpio_sc_config_t *p_config)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;

    err = GPIO_SUCCESS;

    /* reset hardware defaults */
    p_config->configuration.function = GPIO_FUNC_HIZ;
    p_config->configuration.current = GPIO_CURRENT_RESERVED;
    p_config->configuration.tint = GPIO_TINT_RESERVED;

    /* check whether multiplexed pin or not */
    make_port_pin_num(p_config->pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (avail)
    {
        /* function and tint are supported by pins except dedicated */
        err = R_GPIO_PinGetFunction(p_config->pin, &p_config->configuration.function);

        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinTintGet(p_config->pin, &p_config->configuration.tint);
        }
    }
    else
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    if (GPIO_SUCCESS == err)
    {
        /* output driver strength is supported both GPIO and few dedicated pins */
        /* (ignore this error code) */
        R_GPIO_PinDscrGet(p_config->pin, &p_config->configuration.current);
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinGetConfiguration
 ******************************************************************************/

/**
 * @fn         R_GPIO_InitByPinList
 * @brief      Set pin configuration by a pin list and locks the pins
 *             Pin configuration is retrieved from the Smart Configurator table
 * @param[in]  handle: driver instance handle
 * @param[in]  p_pin_list: the pin list to initialise
 * @param[in]  count: count of the pins in the list
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_InitByPinList (int16_t handle, const r_gpio_port_pin_t *p_pin_list, uint32_t count)
{
    e_r_drv_gpio_err_t err;
    uint32_t sc_index;
    uint32_t sc_count;
    uint32_t list_index;
    r_gpio_port_pin_t pin_number;
    bool_t pin_in_use;
    bool_t invalid_pin;
    int16_t allocation;

    /* serialise with multiple threads */
    R_OS_MutexAcquire(sp_gpio_mutex);

    err = GPIO_SUCCESS;
    pin_in_use = false;
    invalid_pin = false;

    /* check if listed pin is valid and not in use */
    for (list_index = 0; list_index < count; list_index++)
    {
        pin_number = p_pin_list[list_index];

        err = get_pin_allocation(pin_number, &allocation);

        if (GPIO_ERR_INVALID_PIN == err)
        {
            invalid_pin = true;
        }

        /* we can only configure the pin if it is not configured, or already configured by us */
        if ((PIN_NOT_ALLOCATED_PRV_ != allocation) && (allocation != handle))
        {
            pin_in_use = true;
        }

        /* check that the pin is in the SC table */
        if (scan_sc_config(pin_number) == NULL)
        {
            invalid_pin = true;
        }
    }

    /* invalid pin error has priority over pin in use error */
    if (invalid_pin)
    {
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        if (pin_in_use)
        {
            err = GPIO_ERR_PIN_ALREADY_IN_USE;
        }
    }

    if (GPIO_SUCCESS == err)
    {
        sc_count = (sizeof GPIO_SC_TABLE_INIT) / (sizeof *GPIO_SC_TABLE_INIT);

        for (sc_index = 0; sc_index < sc_count; sc_index++)
        {
            const st_r_drv_gpio_pin_config_t *p_config = &GPIO_SC_TABLE_INIT[sc_index].configuration;

            for (list_index = 0; list_index < count; list_index++)
            {
                pin_number = p_pin_list[list_index];

                if (GPIO_SC_TABLE_INIT[sc_index].pin == pin_number)
                {
                    /* find matched pin in the SC table */
                    pin_configuration(pin_number, p_config);

                    /* mark the pin as allocated */
                    allocate_pin(pin_number, handle);
                }
            }
        }

        sc_count = (sizeof GPIO_SC_TABLE_MANUAL) / (sizeof *GPIO_SC_TABLE_MANUAL);

        for (sc_index = 0; sc_index < sc_count; sc_index++)
        {
            const st_r_drv_gpio_pin_config_t *p_config = &GPIO_SC_TABLE_MANUAL[sc_index].configuration;

            for (list_index = 0; list_index < count; list_index++)
            {
                pin_number = p_pin_list[list_index];

                if (GPIO_SC_TABLE_MANUAL[sc_index].pin == pin_number)
                {
                    /* find matched pin in the SC table */
                    pin_configuration(pin_number, p_config);

                    /* mark the pin as allocated */
                    allocate_pin(pin_number, handle);
                }
            }
        }
    }

    /* serialise with multiple threads */
    R_OS_MutexRelease(sp_gpio_mutex);

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_InitByPinList
 ******************************************************************************/

/**
 * @fn         R_GPIO_ClearByPinList
 * @brief      Release pins by a pin list and unlocks the pins.
 * @param[in]  p_pin_list: the pin list to clear
 * @param[in]  count: count of the pins in the list
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_ClearByPinList (const r_gpio_port_pin_t *p_pin_list, uint32_t count)
{
    e_r_drv_gpio_err_t ret;
    uint32_t list_index;
    r_gpio_port_pin_t pin_number;
    st_r_drv_gpio_sc_config_t config;
    int16_t allocation;

    /* serialise with multiple threads */
    R_OS_MutexAcquire(sp_gpio_mutex);

    ret = GPIO_SUCCESS;

    /* check if listed pins are valid */
    for (list_index = 0; list_index < count; list_index++)
    {
        pin_number = p_pin_list[list_index];

        /* check that the pin is valid */
        if (get_pin_allocation(pin_number, &allocation) == GPIO_ERR_INVALID_PIN)
        {
            ret = GPIO_ERR_INVALID_PIN;
            break;
        }

        /* check that the pin is in the SC table */
        if (scan_sc_config(pin_number) == NULL)
        {
            ret = GPIO_ERR_INVALID_PIN;
            break;
        }
    }

    if (GPIO_SUCCESS == ret)
    {
        /* clear pins */
        for (list_index = 0; list_index < count; list_index++)
        {
            uint8_t u_port;
            uint8_t u_pin;

            /* get current pin number */
            pin_number = p_pin_list[list_index];

            /* get port pins */
            make_port_pin_num(pin_number, &u_port, &u_pin);

            /* set pin configuration to Hi-Z input protection */
            config.pin = pin_number;
            config.configuration.function = GPIO_FUNC_HIZ;
            config.configuration.tint = GPIO_TINT_DISABLE;

            /* Note that port pins supporting 8mA drive are set to 8mA upon device reset
             * so this check is done first when setting the current level for the pin
             */
            if (pin_function_check(s_r_drv_gpio_dscr_8ma_support, u_port, u_pin))
            {
                /* 8mA default drive current */
                config.configuration.current = GPIO_CURRENT_8mA;
            }
            else if (pin_function_check(s_r_drv_gpio_dscr_4ma_support, u_port, u_pin))
            {
                /* 4mA default drive current */
                config.configuration.current = GPIO_CURRENT_4mA;
            }
            else
            {
                /* other configuration */
                config.configuration.current = GPIO_CURRENT_RESERVED;
            }

            /* update pin configuration */
            ret = pin_configuration(config.pin, &config.configuration);

            if (GPIO_SUCCESS != ret)
            {
                break;
            }

            /* assign pin to GPIO */
            ret = R_GPIO_PinAssignmentSet(pin_number, GPIO_ASSIGN_TO_GPIO);

            /* list pin as available for use */
            deallocate_pin(pin_number);
        }
    }

    /* serialise with multiple threads */
    R_OS_MutexRelease(sp_gpio_mutex);

    return (ret);
}
/******************************************************************************
 * End of function R_GPIO_ClearByPinList
 ******************************************************************************/

/**
 * @fn         R_GPIO_InitByTable
 * @brief      Optional Set a group of pins using an array of pin configurations.
 *             This function locks the pins.
 * @param[in]  handle: driver instance handle
 * @param[in]  p_table: the configuration list to initialise
 * @param[in]  count: count of the pins in the table
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_InitByTable (int16_t handle, const st_r_drv_gpio_sc_config_t *p_table, uint32_t count)
{
    e_r_drv_gpio_err_t err;
    uint32_t index;
    r_gpio_port_pin_t pin_number;
    bool_t pin_in_use;
    bool_t invalid_pin;
    int16_t allocation;

    /* serialise with multiple threads */
    R_OS_MutexAcquire(sp_gpio_mutex);

    err = GPIO_SUCCESS;
    pin_in_use = false;
    invalid_pin = false;

    /* check if listed pin is valid and not in use */
    for (index = 0; index < count; index++)
    {
        pin_number = p_table[index].pin;

        err = get_pin_allocation(pin_number, &allocation);

        if (GPIO_ERR_INVALID_PIN == err)
        {
            invalid_pin = true;
        }

        /* we can only configure the pin if it is not configured, or already configured by us */
        if ((PIN_NOT_ALLOCATED_PRV_ != allocation) && (allocation != handle))
        {
            pin_in_use = true;
        }
    }

    /* invalid pin error has priority over pin in use error */
    if (invalid_pin)
    {
        err = GPIO_ERR_INVALID_PIN;
    }
    else if (pin_in_use)
    {
        err = GPIO_ERR_PIN_ALREADY_IN_USE;
    }
    else
    {
        /* do nothing */
        R_COMPILER_Nop();
    }

    /* configure pins if all pins are not in use */
    for (index = 0; (index < count) && (GPIO_SUCCESS == err); index++)
    {
        pin_configuration(p_table[index].pin, &p_table[index].configuration);

        /* mark the pin as allocated */
        allocate_pin(p_table[index].pin, handle);
    }

    /* serialise with multiple threads */
    R_OS_MutexRelease(sp_gpio_mutex);

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_InitByTable
 ******************************************************************************/

/**
 * @fn         R_GPIO_ClearByTable
 * @brief      Optional release a group pins using an array of pin configurations.
 *             This function unlocks the pins
 * @param[in]  p_table: the configuration list to clear (only using the pin number)
 * @param[in]  count: counts of the pins
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_ClearByTable (const st_r_drv_gpio_sc_config_t *p_table, uint32_t count)
{
    e_r_drv_gpio_err_t err;
    uint32_t index;
    bool_t avail;
    st_r_drv_gpio_sc_config_t config;

    /* serialise with multiple threads */
    R_OS_MutexAcquire(sp_gpio_mutex);

    err = GPIO_SUCCESS;

    for (index = 0; index < count; index++)
    {
        avail = pin_is_available(p_table[index].pin);

        if (avail)
        {
            uint8_t u_port;
            uint8_t u_pin;

            /* get port pins */
            make_port_pin_num(p_table[index].pin, &u_port, &u_pin);

            /* set pin configuration to Hi-Z input protection */
            config.pin = p_table[index].pin;
            config.configuration.function = GPIO_FUNC_HIZ;
            config.configuration.tint = GPIO_TINT_DISABLE;

            /* Note that port pins supporting 8mA drive are set to 8mA upon device reset
             * so this check is done first when setting the current level for the pin
             */
            if (pin_function_check(s_r_drv_gpio_dscr_8ma_support, u_port, u_pin))
            {
                /* 8mA default drive current */
                config.configuration.current = GPIO_CURRENT_8mA;
            }
            else if (pin_function_check(s_r_drv_gpio_dscr_4ma_support, u_port, u_pin))
            {
                /* 4mA default drive current */
                config.configuration.current = GPIO_CURRENT_4mA;
            }
            else
            {
                /* other configuration */
                config.configuration.current = GPIO_CURRENT_RESERVED;
            }

            /* update pin configuration */
            err = pin_configuration(config.pin, &config.configuration);
        }

        if ((GPIO_SUCCESS == err) && (avail))
        {
            err = R_GPIO_PinAssignmentSet(p_table[index].pin, GPIO_ASSIGN_TO_GPIO);
        }

        if ((GPIO_SUCCESS == err) && (avail))
        {
            /* mark the pin as not allocated */
            deallocate_pin(p_table[index].pin);
        }
    }

    /* serialise with multiple threads */
    R_OS_MutexRelease(sp_gpio_mutex);

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_ClearByTable
 ******************************************************************************/

/**
 * @fn         R_GPIO_PortWrite
 * @brief      Sets the logic levels on the specified port
 * @param[in]  port: the port to set
 * @param[in]  value: the value to write
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PortWrite (r_gpio_port_t port, uint8_t value)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    volatile uint8_t *p_uaddr;

    err = GPIO_SUCCESS;

    make_port_num(port, &u_port);

    if (u_port > GPIO_PORT_MAX)
    {
        /* out of port range */
        err = GPIO_ERR_INVALID_PORT;
    }
    else
    {
        /* write value to port */
        p_uaddr = &PORT0.PODR.BYTE;
        RZA_IO_RegWrite_8(&p_uaddr[u_port], value, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PortWrite
 ******************************************************************************/

/**
 * @fn         R_GPIO_PortRead
 * @brief      Reads the logic levels on the specified port
 * @param[in]  port: the port to read
 * @param[out] p_value: the value read from the port
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PortRead (r_gpio_port_t port, uint8_t *p_value)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    volatile uint8_t *p_uaddr;

    err = GPIO_SUCCESS;

    make_port_num(port, &u_port);

    if (u_port > GPIO_PORT_MAX)
    {
        /* out of port range */
        err = GPIO_ERR_INVALID_PORT;
    }
    else
    {
        /* read port value */
        p_uaddr = &PORT0.PIDR.BYTE;
        *p_value = RZA_IO_RegRead_8(&p_uaddr[u_port], IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PortRead
 ******************************************************************************/

/**
 * @fn         R_GPIO_PortDirectionSet
 * @brief      Sets the specified port pins to input, output or Hi-Z
 *             This function assumes that a port has exactly eight pins. Do not call on ports with more or less
 *             than eight pins.
 * @param[in]  port: the port to set
 * @param[in]  dir: input, output or Hi-Z
 * @param[in]  mask: which bits are to be affected
 * @retval     GPIO_SUCCESS  Successful operation.
 * @retval     GPIO_ERR_INVALID_PORT  Specified Port is not valid
 * @retval     GPIO_ERR_INVALID_PIN   A pin on the specified Port is not valid - Port was not changed.
 */
e_r_drv_gpio_err_t R_GPIO_PortDirectionSet (r_gpio_port_t port, e_r_drv_gpio_dir_t dir, uint8_t mask)
{
    e_r_drv_gpio_err_t err;

    uint8_t pin_count; /*!< target pin counter */
    uint8_t port_number = 0;
    bool_t avail;
    uint8_t mask_store;

    err = GPIO_SUCCESS;
    mask_store = mask;

    make_port_num(port, &port_number);

    if (port_number > GPIO_PORT_MAX)
    {
        /* out of port range */
        err = GPIO_ERR_INVALID_PORT;
    }
    else
    {
        /* Check that all the pins we are about to change are valid GPIO pins */
        /* a port has 8 pins */
        for (pin_count = 0; (GPIO_SUCCESS == err) && (pin_count < 8); pin_count++)
        {
            /* check if changing the pin */
            if (mask & 1)
            {
                /* check if valid general purpose I/O pin */
                avail = pin_function_check(s_r_drv_gpio_support, port_number, pin_count);

                if (!avail)
                {
                    /* not available on device */
                    err = GPIO_ERR_INVALID_PIN;
                }
            }

            /* shift for next pin */
            mask = mask >> 1;
        }

        mask = mask_store;

        /* Perform the pin settings */
        /* a port has 8 pins */
        for (pin_count = 0; (GPIO_SUCCESS == err) && (pin_count < 8); pin_count++)
        {
            /* check if changing the pin */
            if (mask & 1)
            {
                /* call API for changing pin direction */
                err = R_GPIO_PinDirectionSet((r_gpio_port_pin_t) (port | pin_count), dir);
            }

            /* shift for next pin */
            mask = mask >> 1;
        }
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PortDirectionSet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinWrite
 * @brief      Sets the logic level on the specified pin
 * @param[in]  pin: the pin to set
 * @param[in]  level: high or low
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinWrite (r_gpio_port_pin_t pin, e_r_drv_gpio_level_t level)
{
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;
    bool_t dont_touch;
    volatile uint8_t *p_podr;
    uint8_t mask;
    e_r_drv_gpio_err_t gpio_err;
    uint8_t value;

    gpio_err = GPIO_SUCCESS;
    dont_touch = false;
    value = 0;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        gpio_err = GPIO_ERR_INVALID_PIN;
        dont_touch = true;
    }

    if ((!dont_touch) && (GPIO_LEVEL_SC_DEFAULT == level))
    {
        /* override from default pin level if specified as SC_DEFAULT */
        const st_r_drv_gpio_pin_config_t *p_config = scan_sc_config(pin);

        if (NULL == p_config)
        {
            /* error if not exist in SC table */
            gpio_err = GPIO_ERR_INVALID_CFG;
            dont_touch = true;
        }
        else
        {
            /* using function member from SC for determining output level */
            if (GPIO_FUNC_OUT_HIGH == p_config->function)
            {
                level = GPIO_LEVEL_HIGH;
            }
            else if (GPIO_FUNC_OUT_LOW == p_config->function)
            {
                level = GPIO_LEVEL_LOW;
            }
            else
            {
                /* do not touch if the pin is not configured as general output */
                dont_touch = true;
            }
        }
    }

    /* make bit value from level */
    if (!dont_touch)
    {
        if (GPIO_LEVEL_LOW == level)
        {
            value = 0;
        }
        else if (GPIO_LEVEL_HIGH == level)
        {
            value = 1;
        }
        else if (GPIO_LEVEL_SC_DEFAULT == level)
        {
            /* do not touch if the pin is not configured as general output */
            dont_touch = true;
        }
        else
        {
            /* parameter error */
            gpio_err = GPIO_ERR_INVALID_CFG;
        }
    }

    /* write PODR from value if success and level is not RESERVED */
    if (!dont_touch)
    {
        p_podr = &(PORT0.PODR.BYTE);
        mask = (uint8_t) s_mask_1bit_register[u_pin];
        RZA_IO_RegWrite_8(&p_podr[u_port], value, u_pin, mask);
    }

    return (gpio_err);
}
/******************************************************************************
 * End of function R_GPIO_PinWrite
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinRead
 * @brief      Reads the logic level on the specified pin
 * @param[in]  pin: the pin to read
 * @param[out] p_level: the logic level read from the pin
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinRead (r_gpio_port_pin_t pin, e_r_drv_gpio_level_t *p_level)
{
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;

    volatile uint8_t *p_pidr;
    uint8_t mask;
    e_r_drv_gpio_err_t gpio_err;
    uint8_t value;

    gpio_err = GPIO_SUCCESS;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        gpio_err = GPIO_ERR_INVALID_PIN;

    }
    else
    {
        /* read PIDR */
        p_pidr = &(PORT0.PIDR.BYTE);
        mask = (uint8_t) s_mask_1bit_register[u_pin];
        value = RZA_IO_RegRead_8(&p_pidr[u_port], u_pin, mask);

        if (0 != value)
        {
            *p_level = GPIO_LEVEL_HIGH;
        }
        else
        {
            *p_level = GPIO_LEVEL_LOW;
        }
    }

    return (gpio_err);
}
/******************************************************************************
 * End of function R_GPIO_PinRead
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinDirectionSet
 * @brief      Sets the specified pin to input, output or Hi-Z
 * @param[in]  pin: the pin to set
 * @param[in]  dir: input, output or Hi-Z
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinDirectionSet (r_gpio_port_pin_t pin, e_r_drv_gpio_dir_t dir)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;
    volatile uint16_t *p_pdr;

    uint16_t mask_16;
    uint16_t shift;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        /* not avail on device */
        return (GPIO_ERR_INVALID_PIN);
    }

    err = GPIO_SUCCESS;

    if (GPIO_DIRECTION_SC_DEFAULT == dir)
    {
        /* override from default pin level if specified as SC_DEFAULT */
        const st_r_drv_gpio_pin_config_t *p_config = scan_sc_config(pin);

        if (NULL == p_config)
        {
            /* configuration error if not exist in SC table */
            err = GPIO_ERR_INVALID_CFG;
        }
        else
        {
            /* using function member from SC for determining direction */
            if ((GPIO_FUNC_OUT_HIGH == p_config->function) ||
                (GPIO_FUNC_OUT_LOW  == p_config->function) ||
                (GPIO_FUNC_OUT      == p_config->function))
            {
                dir = GPIO_DIRECTION_OUTPUT;
            }
            else if (GPIO_FUNC_IN == p_config->function)
            {
                dir = GPIO_DIRECTION_INPUT;
            }
            else
            {
                /* configuration error if the pin is not configured as general I/O */
                err = GPIO_ERR_INVALID_CFG;
            }
        }
    }

    if (GPIO_SUCCESS == err)
    {
        /* each PDR pin data have 2bit width */
        shift = (uint16_t) s_shift_2bit_register[u_pin];

        /* make mask bit (0b11 << shift) */
        mask_16 = (uint16_t) s_mask_2bit_register[u_pin];

        /* modify PDR */
        p_pdr = &PORT0.PDR.WORD;
        p_pdr = &(p_pdr[u_port]);

        switch (dir)
        {
            case GPIO_DIRECTION_INPUT:
            {
                /* write direction */
                /* 2 : as input */
                RZA_IO_RegWrite_16(p_pdr, 2, shift, mask_16);
                break;
            }
            case GPIO_DIRECTION_OUTPUT:
            {
                /* write direction */
                /* 3 : as output */
                RZA_IO_RegWrite_16(p_pdr, 3, shift, mask_16);
                break;
            }
            case GPIO_DIRECTION_HIZ:
            {
                /* write direction */
                /* 0 : as hi-z protection */
                RZA_IO_RegWrite_16(p_pdr, 0, shift, mask_16);
                break;
            }
            default:
            {
                err = GPIO_ERR_INVALID_CMD;
                break;
            }
        }
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinDirectionSet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinDirectionGet
 * @brief      Gets the specified pin direction setting (input, output or Hi-Z)
 * @param[in]  pin: the pin to interrogate
 * @param[out] p_dir: the direction read for the pin: input, output, or Hi-Z
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinDirectionGet (r_gpio_port_pin_t pin, e_r_drv_gpio_dir_t *p_dir)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;
    volatile uint16_t *p_pdr;
    uint16_t pdr_reg;
    uint16_t mask_16;
    uint16_t shift;

    err = GPIO_SUCCESS;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        /* not avail on device */
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        /* each PDR pin data have 2-bit width */
        shift = (uint16_t) s_shift_2bit_register[u_pin];

        /* make mask bit (0b11 << shift) */
        mask_16 = (uint16_t) s_mask_2bit_register[u_pin];

        /* get PDR */
        p_pdr = &PORT0.PDR.WORD;
        pdr_reg = RZA_IO_RegRead_16(&p_pdr[u_port], shift, mask_16);

        switch (pdr_reg)
        {
            case PDR_REG_SETTING_DIRECTION_INPUT_PRV_:      /* as input */
            {
                *p_dir = GPIO_DIRECTION_INPUT;
                break;
            }

            case PDR_REG_SETTING_DIRECTION_OUTPUT_PRV_:     /* as output */
            {
                *p_dir = GPIO_DIRECTION_OUTPUT;
                break;
            }

            case PDR_REG_SETTING_DIRECTION_HIZ_PRV_:        /* as hi-z protection */
            {
                *p_dir = GPIO_DIRECTION_HIZ;
                break;
            }

            default:
            {
                err = GPIO_ERR_INVALID_CFG;
                break;
            }
        }
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinDirectionGet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinAssignmentSet
 * @brief      Assigns a pin to GPIO or to a peripheral
 * @param[in]  pin: the pin to assign
 * @param[in]  assignment: how to assign the pin:
 *                 GPIO_ASSIGN_TO_PERIPHERAL or GPIO_ASSIGN_TO_GPIO
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinAssignmentSet (r_gpio_port_pin_t pin, e_r_drv_gpio_assign_t assignment)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    uint8_t u_pin;
    uint8_t mask_8;
    bool_t avail;
    volatile uint8_t *p_pmr;
    uint8_t shift;
    uint8_t value;

    err = GPIO_SUCCESS;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        /* not avail on device */
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        if (GPIO_ASSIGN_TO_GPIO == assignment)
        {
            value = 0;
        }
        else if (GPIO_ASSIGN_TO_PERIPHERAL == assignment)
        {
            value = 1;
        }
        else
        {
            err = GPIO_ERR_INVALID_CFG;
        }
    }

    if (GPIO_SUCCESS == err)
    {
        /* each PMR pin data have 1bit width */
        shift = (uint8_t) s_shift_1bit_register[u_pin];

        /* make mask bit (0b1 << shift) */
        mask_8 = (uint8_t) s_mask_1bit_register[u_pin];

        p_pmr = &PORT0.PMR.BYTE;
        RZA_IO_RegWrite_8(&p_pmr[u_port], value, shift, mask_8);
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinAssignmentSet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinAssignmentGet
 * @brief      Get pin assignment: GPIO or peripheral
 * @param[in]  pin: the pin to get the current assignment setting
 * @param[out] p_assignment: set to either:
 *                 GPIO_ASSIGN_TO_PERIPHERAL or GPIO_ASSIGN_TO_GPIO
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinAssignmentGet (r_gpio_port_pin_t pin, e_r_drv_gpio_assign_t *p_assignment)
{
    e_r_drv_gpio_err_t err;
    uint8_t u_port;
    uint8_t u_pin;
    uint8_t pmr_reg;
    uint8_t mask_8;
    bool_t avail;
    volatile uint8_t *p_pmr;
    uint8_t shift;

    err = GPIO_SUCCESS;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        /* not avail on device */
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        /* each PMR pin data have 1bit width */
        shift = (uint8_t) s_shift_1bit_register[u_pin];

        /* make mask bit (0b1 << shift) */
        mask_8 = (uint8_t) s_mask_1bit_register[u_pin];

        p_pmr = &PORT0.PMR.BYTE;
        pmr_reg = RZA_IO_RegRead_8(&p_pmr[u_port], shift, mask_8);

        if (1 == pmr_reg)
        {
            *p_assignment = GPIO_ASSIGN_TO_PERIPHERAL;
        }
        else
        {
            *p_assignment = GPIO_ASSIGN_TO_GPIO;
        }
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinAssignmentGet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinTintSet
 * @brief      Turns interrupts on or off for a pin
 * @param[in]  pin: the pin to set the interrupts for
 * @param[in]  tint: whether to enable or disable interrupts:
 *                 GPIO_TINT_ENABLE or GPIO_TINT_DISABLE
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinTintSet (r_gpio_port_pin_t pin, e_r_drv_gpio_tint_t tint)
{
    e_r_drv_gpio_err_t err;
    volatile uint8_t *p_uaddr;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;
    bool_t dont_touch;

    err = GPIO_SUCCESS;
    dont_touch = false;

    if (GPIO_TINT_RESERVED == tint)
    {
        dont_touch = true;
    }

    /* get default configuration if GPIO_CURRENT_SC_DEFAULT is specified */
    if ((!dont_touch) && (GPIO_TINT_SC_DEFAULT == tint))
    {
        const st_r_drv_gpio_pin_config_t *p_config = scan_sc_config(pin);
        if (NULL == p_config)
        {
            err = GPIO_ERR_INVALID_CFG;
        }
        else
        {
            tint = p_config->tint;
        }
    }

    if (!dont_touch)
    {
        /* check if valid general purpose I/O pin */
        make_port_pin_num(pin, &u_port, &u_pin);
        avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

        if (!avail)
        {
            /* not avail on device */
            err = GPIO_ERR_INVALID_PIN;
        }
        else
        {
            p_uaddr = &GPIO.P00PFS.BYTE;
            p_uaddr = &p_uaddr[(u_port * 8) + u_pin];

            switch (tint)
            {
                case GPIO_TINT_ENABLE:
                {
                    /* set ISEL bit to 1 if tint enable */
                    unprotect_pin_mux();
                    RZA_IO_RegWrite_8(p_uaddr, 1, GPIO_P00PFS_ISEL_SHIFT, GPIO_P00PFS_ISEL);
                    protect_pin_mux();
                    break;
                }

                case GPIO_TINT_DISABLE:
                {
                    /* set ISEL bit to 0 if tint disable */
                    unprotect_pin_mux();
                    RZA_IO_RegWrite_8(p_uaddr, 0, GPIO_P00PFS_ISEL_SHIFT, GPIO_P00PFS_ISEL);
                    protect_pin_mux();
                    break;
                }

                default:
                {
                    err = GPIO_ERR_INVALID_CFG;
                    break;
                }
            }
        }
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinTintSet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinTintGet
 * @brief      Reads interrupt on / off setting for a pin
 * @param[in]  pin: the pin to get the interrupts for
 * @param[out] p_tint: the interrupt setting
 *                 GPIO_TINT_ENABLE or GPIO_TINT_DISABLE
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinTintGet (r_gpio_port_pin_t pin, e_r_drv_gpio_tint_t *p_tint)
{
    e_r_drv_gpio_err_t err;
    uint8_t isel;
    volatile uint8_t *p_uaddr;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;

    err = GPIO_SUCCESS;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        /* not avail on device */
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        p_uaddr = &GPIO.P00PFS.BYTE;
        isel = RZA_IO_RegRead_8(&p_uaddr[(u_port * 8) + u_pin], GPIO_P00PFS_ISEL_SHIFT, GPIO_P00PFS_ISEL);

        if (0 == isel)
        {
            *p_tint = GPIO_TINT_DISABLE;
        }
        else
        {
            *p_tint = GPIO_TINT_ENABLE;
        }
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinTintGet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinDscrSet
 * @brief      Sets pin DSCR current
 * @param[in]  pin: the pin to set the DSCR for
 * @param[in]  current: the current to set:
 *                 GPIO_CURRENT_4mA or GPIO_CURRENT_8mA
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinDscrSet (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t current)
{
    e_r_drv_gpio_err_t err;

    /* get default configuration if GPIO_CURRENT_SC_DEFAULT is specified */
    err = GPIO_SUCCESS;
    if (GPIO_CURRENT_SC_DEFAULT == current)
    {
        const st_r_drv_gpio_pin_config_t *p_config = scan_sc_config(pin);
        if (NULL == p_config)
        {
            err = GPIO_ERR_INVALID_CFG;
        }
        else
        {
            current = p_config->current;
        }
    }

    if ((GPIO_SUCCESS == err) && (GPIO_CURRENT_RESERVED != current))
    {
        if (pin <= GPIO_PORT_PIN_MAX)
        {
            /* general purpose io pins */
            err = set_output_current_pin(pin, current);
        }
        else if (GPIO_PIN_CKIO == pin)
        {
            /* CKIO pin */
            err = set_output_current_ckio(current);
        }
        else if ((GPIO_PIN_SD0_CLK <= pin) && (pin <= GPIO_PIN_SD0_RST))
        {
            /* SD0 */
            err = set_output_current_sd0(pin, current);
        }
        else if ((GPIO_PIN_SD1_CLK <= pin) && (pin <= GPIO_PIN_SD1_DAT3))
        {
            /* SD1 */
            err = set_output_current_sd1(pin, current);
        }
        else if ((GPIO_PIN_QSPI0_SPCLK <= pin) && (pin <= GPIO_PIN_QSPI1_SSL))
        {
            /* SPIBSC */
            err = set_output_current_qspi(pin, current);
        }
        else
        {
            err = GPIO_ERR_INVALID_PIN;
        }
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinDscrSet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinDscrGet
 * @brief      Gets pin DSCR current setting (4mA or 8mA)
 * @param[in]  pin: the pin to get the DSCR for
 * @param[out] p_current: the current current setting:
 *                 GPIO_CURRENT_4mA or GPIO_CURRENT_8mA
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinDscrGet (r_gpio_port_pin_t pin, e_r_drv_gpio_current_t *p_current)
{
    e_r_drv_gpio_err_t err;

    err = GPIO_SUCCESS;

    if (pin <= GPIO_PORT_PIN_MAX)
    {
        /* general purpose I/O pins */
        err = get_output_current_pin(pin, p_current);
    }
    else if (GPIO_PIN_CKIO == pin)
    {
        /* CKIO pin */
        err = get_output_current_ckio(p_current);
    }
    else if ((GPIO_PIN_SD0_CLK <= pin) && (pin <= GPIO_PIN_SD0_RST))
    {
        /* SD0 */
        err = get_output_current_sd0(pin, p_current);
    }
    else if ((GPIO_PIN_SD1_CLK <= pin) && (pin <= GPIO_PIN_SD1_DAT3))
    {
        /* SD1 */
        err = get_output_current_sd1(pin, p_current);
    }
    else if ((GPIO_PIN_QSPI0_SPCLK <= pin) && (pin <= GPIO_PIN_QSPI1_SSL))
    {
        /* SPIBSC */
        err = get_output_current_qspi(pin, p_current);
    }
    else
    {
        err = GPIO_ERR_INVALID_PIN;
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinDscrGet
 ******************************************************************************/

/**
 * @fn         R_GPIO_PeripheralFunctionWrite
 * @brief      Sets the peripheral function for a pin
 * @param[in]  pin: the pin to set
 * @param[in]  psel: the code for the required peripheral function
 * @retval     GPIO_SUCCESS  Successful operation.
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PeripheralFunctionWrite (r_gpio_port_pin_t pin, uint8_t psel)
{
    e_r_drv_gpio_err_t err;
    volatile uint8_t *p_uaddr;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;

    err = GPIO_SUCCESS;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        /* not available on device */
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        p_uaddr = &GPIO.P00PFS.BYTE;
        unprotect_pin_mux();
        RZA_IO_RegWrite_8(&p_uaddr[(u_port * 8) + u_pin], psel, GPIO_P00PFS_PSEL_SHIFT, GPIO_P00PFS_PSEL);
        protect_pin_mux();
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PeripheralFunctionWrite
 ******************************************************************************/

/**
 * @fn         R_GPIO_PeripheralFunctionRead
 * @brief      Gets the current peripheral function setting for a pin
 * @param[in]  pin: the pin to interrogate for the peripheral function setting
 * @param[out] p_psel: set to the code of the current peripheral function
 * @retval     GPIO_SUCCESS  Successful operation.
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PeripheralFunctionRead (r_gpio_port_pin_t pin, uint8_t *p_psel)
{
    e_r_drv_gpio_err_t err;
    volatile uint8_t *p_uaddr;
    uint8_t u_port;
    uint8_t u_pin;
    bool_t avail;

    err = GPIO_SUCCESS;

    /* check if valid general purpose I/O pin */
    make_port_pin_num(pin, &u_port, &u_pin);
    avail = pin_function_check(s_r_drv_gpio_support, u_port, u_pin);

    if (!avail)
    {
        /* not available on device */
        err = GPIO_ERR_INVALID_PIN;
    }
    else
    {
        p_uaddr = &GPIO.P00PFS.BYTE;
        *p_psel = RZA_IO_RegRead_8(&p_uaddr[(u_port * 8) + u_pin], GPIO_P00PFS_PSEL_SHIFT, GPIO_P00PFS_PSEL);
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PeripheralFunctionRead
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinSetFunction
 * @brief      Set the function of the specified pin
 * @param[in]  pin: the pin to set
 * @param[in]  function: which function to configure
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinSetFunction (r_gpio_port_pin_t pin, e_r_drv_gpio_function_t function)
{
    e_r_drv_gpio_err_t err;

    err = GPIO_SUCCESS;

    /* get default function if GPIO_FUNC_SC_DEFAULT */
    if (GPIO_FUNC_SC_DEFAULT == function)
    {
        const st_r_drv_gpio_pin_config_t *p_config = scan_sc_config(pin);
        if (NULL == p_config)
        {
            return GPIO_ERR_INVALID_CFG;
        }

        function = p_config->function;
    }

    if (GPIO_FUNC_HIZ == function)
    {
        /* Hi-Z protection */
        err = R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_HIZ);

        /* Hi-Z protection on peripheral function control register PxnPFS */
        if (GPIO_SUCCESS == err)
        {
            /* Reset back to the value after reset (000b) */
            err = R_GPIO_PeripheralFunctionWrite(pin, (uint8_t) (GPIO_FUNC_PERIPHERAL0 - GPIO_FUNC_PERIPHERAL0));
        }

        /* and assign to GPIO */
        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinAssignmentSet(pin, GPIO_ASSIGN_TO_GPIO);
        }
    }
    else if (GPIO_FUNC_IN == function)
    {
        /* input function (procedure is described in UM) */
        err = R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_HIZ);

        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinAssignmentSet(pin, GPIO_ASSIGN_TO_GPIO);
        }

        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_INPUT);
        }
    }
    else if ((GPIO_FUNC_OUT_HIGH == function) || (GPIO_FUNC_OUT_LOW == function) || (GPIO_FUNC_OUT == function))
    {
        /* output functions (procedure is described in UM) */

        err = R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_HIZ);
        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinAssignmentSet(pin, GPIO_ASSIGN_TO_GPIO);
        }

        if (GPIO_SUCCESS == err)
        {
            switch (function)
            {
                case GPIO_FUNC_OUT_HIGH:
                {
                    err = R_GPIO_PinWrite(pin, GPIO_LEVEL_HIGH);
                    break;
                }

                case GPIO_FUNC_OUT_LOW:
                {
                    err = R_GPIO_PinWrite(pin, GPIO_LEVEL_LOW);
                    break;
                }

                default:
                {
                    /* keep previous output state if level is not specified */
                    R_COMPILER_Nop();
                    break;
                }
            }
        }

        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_OUTPUT);
        }
    }
    else if ((GPIO_FUNC_PERIPHERAL0 <= function) && (function <= GPIO_FUNC_PERIPHERAL7))
    {
        /* internal peripheral functions (procedure is described in UM) */
        err = R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_HIZ);
        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinAssignmentSet(pin, GPIO_ASSIGN_TO_GPIO);
        }

        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PeripheralFunctionWrite(pin, (uint8_t) (function - GPIO_FUNC_PERIPHERAL0));
        }

        if (GPIO_SUCCESS == err)
        {
            err = R_GPIO_PinAssignmentSet(pin, GPIO_ASSIGN_TO_PERIPHERAL);
        }
    }
    else
    {
        err = GPIO_ERR_INVALID_CFG;
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinSetFunction
 ******************************************************************************/

/**
 * @fn         R_GPIO_PinGetFunction
 * @brief      Get the function of the specified pin
 * @param[in]  pin: the pin to get
 * @param[out] p_function: set to the current pin function
 * @retval     GPIO_SUCCESS  Successful operation
 * @retval     error_code    The error code on failure
 */
e_r_drv_gpio_err_t R_GPIO_PinGetFunction (r_gpio_port_pin_t pin, e_r_drv_gpio_function_t *p_function)
{
    e_r_drv_gpio_err_t err;
    e_r_drv_gpio_assign_t assignment;
    e_r_drv_gpio_dir_t dir;
    uint8_t psel;

    /* set default */
    *p_function = GPIO_FUNC_SC_DEFAULT;

    err = R_GPIO_PinAssignmentGet(pin, &assignment);
    if (GPIO_SUCCESS == err)
    {
        if (GPIO_ASSIGN_TO_GPIO == assignment)
        {
            err = R_GPIO_PinDirectionGet(pin, &dir);
        }
        else
        {
            err = R_GPIO_PeripheralFunctionRead(pin, &psel);
        }
    }

    if ((GPIO_SUCCESS == err) && (GPIO_ASSIGN_TO_GPIO == assignment))
    {
        /* check GPIO Hi-Z/Input/Output */
        switch (dir)
        {
            case GPIO_DIRECTION_HIZ:
            {
                *p_function = GPIO_FUNC_HIZ;
                break;
            }
            case GPIO_DIRECTION_INPUT:
            {
                *p_function = GPIO_FUNC_IN;
                break;
            }
            case GPIO_DIRECTION_OUTPUT:
            {
                e_r_drv_gpio_level_t p_level;

                /* Get pin level */
                err = R_GPIO_PinRead(pin, &p_level);
                if (GPIO_SUCCESS == err)
                {
                    *p_function = GPIO_FUNC_OUT;
                }
                break;
            }
            default:
            {
                err = GPIO_ERR_INVALID_CFG;
                break;
            }
        }
    }

    if ((GPIO_SUCCESS == err) && (GPIO_ASSIGN_TO_PERIPHERAL == assignment)
            && (psel <= (GPIO_FUNC_PERIPHERAL7 - GPIO_FUNC_PERIPHERAL0)))
    {
        /* cast combined return value of psel value + peripheral to e_r_drv_gpio_function_t */
        *p_function = (e_r_drv_gpio_function_t) (GPIO_FUNC_PERIPHERAL0 + psel);
    }

    return (err);
}
/******************************************************************************
 * End of function R_GPIO_PinGetFunction
 *****************************************************************************/

/**
 * @fn             R_GPIO_DeallocateAllPins
 * @brief          Mark all pins allocated to handle as not allocated
 * @param[in]      handle: the handle to deallocate pins for
 * @retval         None
 */
void R_GPIO_DeallocateAllPins(int16_t handle)
{
    uint16_t i;

    /* set all pins allocated to this handle to PIN_NOT_ALLOCATED_PRV_ */
    for (i = 0; i < ((sizeof(s_gpio_pin_allocation)) / (sizeof(int16_t))); i++)
    {
        if (s_gpio_pin_allocation[i] == handle)
        {
            s_gpio_pin_allocation[i] = PIN_NOT_ALLOCATED_PRV_;
        }
    }
}
/******************************************************************************
 * End of function R_GPIO_DeallocateAllPins
 ******************************************************************************/

/**
 * @fn          R_GPIO_GetVersion
 * @brief       Gets the version number of this Low Layer Driver
 * @param[out]  p_ver_info: returns the driver information
 * @retval      DRV_SUCCESS Always returned
 */
int_t R_GPIO_GetVersion (st_ver_info_t *p_ver_info)
{
    p_ver_info->lld.p_szdriver_name = s_lld_info.p_szdriver_name;
    p_ver_info->lld.version.sub.major = s_lld_info.version.sub.major;
    p_ver_info->lld.version.sub.minor = s_lld_info.version.sub.minor;
    p_ver_info->lld.build = s_lld_info.build;

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_GPIO_GetVersion
 ******************************************************************************/

#ifdef TEST_ACCESS_PRV_
e_r_drv_gpio_err_t test_get_pin_allocation(r_gpio_port_pin_t port_pin, int16_t *allocation);

/**
 * @fn             test_get_pin_allocation
 * @brief          Called by test code to determine which handle a pin is allocated to
 * @param[in]      port_pin: port / pin
 * @param[out]     allocation: the handle that the pin is allocated to
 *                 or PIN_NOT_ALLOCATED_PRV_ if it isn't allocated
 * @retval         GPIO_SUCCESS         pin allocation successfully retrieved
 * @retval         GPIO_ERR_INVALID_PIN pin does not exist on this device
 */
e_r_drv_gpio_err_t test_get_pin_allocation(r_gpio_port_pin_t port_pin, int16_t *allocation)
{
    return (get_pin_allocation(port_pin, allocation));
}
/******************************************************************************
 * End of function test_get_pin_allocation
 ******************************************************************************/

e_r_drv_gpio_err_t test_allocate_pin(r_gpio_port_pin_t port_pin, int16_t handle);

/**
 * @fn             test_allocate_pin
 * @brief          allocate pin to a particular handle
 * @param[in]      port_pin: port / pin
 * @param[in]      handle: handle that is configuring the pin
 * @retval         GPIO_SUCCESS         pin successfully allocated
 * @retval         GPIO_ERR_INVALID_PIN pin does not exist on this device
 */
e_r_drv_gpio_err_t test_allocate_pin(r_gpio_port_pin_t port_pin, int16_t handle)
{
     return (allocate_pin(port_pin, handle));
}
/******************************************************************************
 * End of function test_allocate_pin
 *****************************************************************************/

e_r_drv_gpio_err_t test_deallocate_pin(r_gpio_port_pin_t port_pin);

/**
 * @fn             test_deallocate_pin
 * @brief          deallocate a pin
 * @param[in]      port_pin: port / pin
 * @retval         GPIO_SUCCESS         pin successfully deallocated
 * @retval         GPIO_ERR_INVALID_PIN pin does not exist on this device
 */
e_r_drv_gpio_err_t test_deallocate_pin(r_gpio_port_pin_t port_pin)
{
    /* set the allocation of the pin to PIN_NOT_ALLOCATED_PRV_ */
    return (deallocate_pin(port_pin));
}
/******************************************************************************
 * End of function test_deallocate_pin
 *****************************************************************************/

#endif

/* End of File */
