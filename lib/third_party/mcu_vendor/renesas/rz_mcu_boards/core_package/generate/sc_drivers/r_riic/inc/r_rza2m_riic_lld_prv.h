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
 * File Name    : r_rza2m_riic_lld_prv.h
 * Description  : RIIC low level driver interface header
 *****************************************************************************/

/*********************************************************************//**
 * @ingroup RENESAS_DRIVER_LIBRARY
 * @defgroup SSGSTARC_55_RIIC_LLD_API RIIC Low Level Driver
 *
 * @section SSGSTARC_55_RIIC_LLD_API_SUMMARY Summary
 * 
 * This RIIC Low Level Driver is developed for the RZA2M.
 * 
 * The driver depends upon the common SSGSTARC-56 High Level Driver for RIIC.
 *
 * This is a device and board specific component of an RIIC driver, 
 * configured for the RZA2M board. This interface is not intended to be
 * used directly without the associated High Level Driver.
 * 
 * @section R_RIIC_LLD_API_55_INSTANCES Known Implementations:
 * This driver is used in the RZA2M Software Package.
 * @ref RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @section R_RIIC_LLD_API_55_SEE_ALSO See Also:
 * @see RENESAS_SC_CFG Renesas Smart Configuration Settings
 * @see R_RIIC_HLD_API_56 RZA2M RIIC High Level Driver API
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 ************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/
#include "driver.h"
#include "r_riic_drv_api.h"
#include "r_riic_drv_sc_cfg.h"
#include "r_intc_drv_api.h"

#ifndef R_RIIC_INC_R_RZA2M_RIIC_LLD_PRV_H_
    #define R_RIIC_INC_R_RZA2M_RIIC_LLD_PRV_H_

/******************************************************************************
 Macro definitions
 *****************************************************************************/

    #define RIIC_FREQ_VALUE_1MHZ                  (1000000uL)
    #define RIIC_FREQ_VALUE_400KHZ                (400000uL)
    #define RIIC_FREQ_VALUE_100KHZ                (100000uL)

    /* 1/66MHz = 15.2nsec */
    #define RIIC_UNIT_TIME_SCL                    (15uL)
    /* 66MHz */
    #define RIIC_UNIT_PCLK_HZ                     (66000000uL)

    /* 66MHz */
    #define RIIC_PCLK_66MHZ                       (66000000uL)
    /* 60MHz */
    #define RIIC_PCLK_60MHZ                       (60000000uL)
    /* 55MHz */
    #define RIIC_PCLK_55MHZ                       (55000000uL)

    #define RIIC_BITRATE_REG_MAX                  (0x1FuL)

    #define RIIC_MR3_REG_NF_STAGE1                (0x00uL)
    #define RIIC_MR3_REG_NF_STAGE2                (0x01uL)
    #define RIIC_MR3_REG_NF_STAGE3                (0x02uL)
    #define RIIC_MR3_REG_NF_STAGE4                (0x03uL)

/******************************************************************************
 Typedef definitions
 *****************************************************************************/

/* To make casting to the ISR prototype expected by the Renesas GIC drivers. */
typedef void (*ISR_FUNCTION)(uint32_t sense);

typedef enum e_riic_ch_t      // RIIC channel numbers
{
    RIIC_CH0 = 0, RIIC_CH1 = 1, RIIC_CH2 = 2, RIIC_CH3 = 3, RIIC_NUM_CH = 4
} e_riic_ch_t;

typedef enum
{
    RIIC_PCLK_DIV_1 = 0, /*!< Internal base clock = P1/1*/
    RIIC_PCLK_DIV_2 = 1, /*!< Internal base clock = P1/2*/
    RIIC_PCLK_DIV_4 = 2, /*!< Internal base clock = P1/4*/
    RIIC_PCLK_DIV_8 = 3, /*!< Internal base clock = P1/8*/
    RIIC_PCLK_DIV_16 = 4, /*!< Internal base clock = P1/16*/
    RIIC_PCLK_DIV_32 = 5, /*!< Internal base clock = P1/32*/
    RIIC_PCLK_DIV_64 = 6, /*!< Internal base clock = P1/64*/
    RIIC_PCLK_DIV_128 = 7, /*!< Internal base clock = P1/128*/
    RIIC_PCLK_DIV_MAX = 8
} e_riic_base_pclk_t;

typedef enum
{
    RIIC_INT_TEI = 0, /*!< Transfer end interrupt*/
    RIIC_INT_RI = 1, /*!< Receive data full interrupt*/
    RIIC_INT_TI = 2, /*!< Transfer empty interrupt*/
    RIIC_INT_SPI = 3, /*!< Detection of Stop Condition interrupt*/
    RIIC_INT_STI = 4, /*!< Detection of Start Condition interrupt*/
    RIIC_INT_NAKI = 5, /*!< Receive NACK interrupt*/
    RIIC_INT_ALI = 6, /*!< Arbitration lost interrupt*/
    RIIC_INT_TMOI = 7, /*!< SCL timeout interrupt*/
    RIIC_INT_MAX = 8
} e_riic_int_type_t;

/**
 * @typedef 
 */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpadded"
typedef struct
{
        /*!< Interrupt handler function address*/
        ISR_FUNCTION          int_handler;
        /*!< Number of interrupt */
        e_r_drv_intc_intid_t  int_num;
        /*!< Set Edge interrupt or Level nterrupt */
        e_r_drv_tint_sense_t  int_level;
} st_riic_int_param_t;
    #pragma GCC diagnostic pop

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpadded"
typedef struct
{
        /*!< Internal base clock */
        e_riic_base_pclk_t pclk;
        /*!< SCL high width */
        uint32_t           high_width;
        /*!< SCL low width */
        uint32_t           low_width;
} st_riic_freq_table_param_t;
    #pragma GCC diagnostic pop

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpadded"
typedef struct
{
        /*!< Internal base clock */
        uint32_t               pclk;
        /*!< PCLK divide rate */
        e_riic_base_pclk_t     pclk_div;
        /*!< frequency */
        e_riic_clk_frequency_t frequency;
        /*!< Duty cycle */
        e_riic_clk_duty_t      duty;
        /*!< Delay time of SCL rise time */
        uint32_t               rise_time;
        /*!< Delay time of SCL fall time */
        uint32_t               fall_time;
        /*!< Noise filter stage */
        e_riic_filter_t        noise_filter_stage;
        /*!< I2C or SMBUS format */
        e_riic_format_t        format;
        /*!< Adjusted value of high width */
        uint32_t               high_width;
        /*!< Adjusted value of low width */
        uint32_t               low_width;
} st_riic_clk_check_param_t;
    #pragma GCC diagnostic pop

/******************************************************************************
 Functions Prototypes
 *****************************************************************************/
/* ==== Functional Methods ==== */

/* Only low level functions accessed */

/******************************************************************************
 * Function Name: interrupt_init
 * Description  : Initialize interrupt for RIIC.
 * Arguments    : channel - the device specific channel number 
 *                          (under RIIC_CFG_LLD_NUM_CHANNELS)
 * Return Value : None
 *****************************************************************************/
void interrupt_init (const int_t channel, const st_riic_config_t *p_cfg);

/******************************************************************************
 * Function Name: interrupt_uninit
 * Description  : Uninitiaize interrupt for RIIC.
 * Arguments    : channel - the device specific channel number 
 *                          (under RIIC_CFG_LLD_NUM_CHANNELS)
 * Return Value : None
 *****************************************************************************/
void interrupt_uninit (const int_t channel);

#endif /* SRC_RENESAS_AUTOMATED_TEST_R_RIIC_INC_R_RZA2M_RIIC_LLD_PRV_H_ */
