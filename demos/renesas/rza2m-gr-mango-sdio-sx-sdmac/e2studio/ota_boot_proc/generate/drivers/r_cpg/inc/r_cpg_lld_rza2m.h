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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_cpg_lld_rza2m.h
 * Description  : CPG driver API header
 *********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_devlink_wrapper.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#ifndef R_CPG_LLD_RZA2M_H_
#define R_CPG_LLD_RZA2M_H_


/* Version Number of API */

#define STDIO_CPG_RZ_LLD_DRV_NAME           ("LLD EBK_RZA2M CPG")

/** Major Version Number of API */
#define STDIO_CPG_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API */
#define STDIO_CPG_RZ_LLD_VERSION_MINOR      (5)
/** Minor Version Number of API */
#define STDIO_CPG_RZ_LLD_BUILD_NUM          (0)
/** Unique ID */
#define STDIO_CPG_RZ_LLD_UID                (64)



/*! @enum e_cpg_clock_source_t
 *  @brief For setting the main system clock
 */
typedef enum
{
    CPG_CLOCK_SOURCE_PLL,                   /*!< PLL */
} e_cpg_clock_source_t;

/*! @enum e_cpg_clk_pll_ip_t
 *  @brief For setting the main system clock
 */
typedef enum
{
    CPG_PLL_SOURCE_MAIN_OSC = 0u,           /*!< Main oscillator as source for PLL */
} e_cpg_clk_pll_ip_t;

/*! @enum e_cpg_clk_ip_div_t
 *  @brief PLL multiplier values
 */
typedef enum
{
    CPG_PLL_DIV_1_1 = 1u,                   /*!< No Division of input to PLL */
    CPG_PLL_DIV_1_2 = 2u,                   /*!< PLL input divided by 2 */
} e_cpg_clk_ip_div_t;

/*! @enum e_cpg_clk_pll_t
 *  @brief PLL multiplier value
 */
typedef enum
{
    CPG_PLL_MULT_X88 = 88u,                 /*!< PLL Multiplication x88 */
} e_cpg_clk_pll_t;

/*! @enum e_cpg_sub_clock_t
 *  @brief For setting any system sub-clock
 */
typedef enum
{
    CPG_SUB_CLOCK_ICLK = 0u,                /*!< CPU Clock */
    CPG_SUB_CLOCK_BCLK,                     /*!< Internal Bus Clock */
    CPG_SUB_CLOCK_P1CLK,                    /*!< Peripheral Clock */
} e_cpg_sub_clock_t;

/*! @enum e_cpg_clk_sub_div_t
 *  @brief Divider options for setting the source clock for system sub-clocks
 */
typedef enum
{
    CPG_SUB_CLK_DIV_1_2 = 2u,               /*!< PLL input divided by 2 */
    CPG_SUB_CLK_DIV_1_4 = 4u,               /*!< PLL input divided by 4 */
    CPG_SUB_CLK_DIV_1_8 = 8u,               /*!< PLL input divided by 8 */
    CPG_SUB_CLK_DIV_1_16 = 16u,             /*!< PLL input divided by 16 */
    CPG_SUB_CLK_DIV_1_32 = 32u,             /*!< PLL input divided by 32 */
} e_cpg_clk_sub_div_t;

/*! @enum e_cpg_clock_sel_t
 *  @brief For setting the source clock for system sub-clocks
 */
typedef enum
{
    CPG_SUB_CLOCK_CKIO = 0u,                /*!< External Clock Pin */
    CPG_SUB_CLOCK_OCTAMEM,                  /*!< Octa Memory Clock */
    CPG_SUB_CLOCK_HYPERBUS,                 /*!< Hyper Bus Clock */
    CPG_SUB_CLOCK_SPICLK,                   /*!< SPI multi IO Clock */
} e_cpg_sub_clock_sel_t;

/*! @enum e_cpg_sub_clock_set_input_t
 *  @brief For setting the source clock for system sub-clocks
 */
typedef enum
{
    CPG_SUB_CLOCK_BCLK_IN,                  /*!< Internal Bus Clock */
    CPG_SUB_CLOCK_P1CLK_IN,                 /*!< Peripheral Clock 1 */
    CPG_SUB_CLOCK_GCLK_IN,                  /*!< Image Processor Clock */
    CPG_SUB_CLOCK_P0CLK_IN,                 /*!< Peripheral Clock 0 */
} e_cpg_sub_clock_set_input_t;

/*! @enum e_cpg_ckio_pin_t
 *  @brief For setting the external clock pin behaviour
 */
typedef enum
{
    CPG_CKIO_INVALID_LOW_NORM_ON_STDBY_DEEP_HIZ = 0,
    CPG_CKIO_INVALID_LOW_NORM_ON_STDBY_DEEP_LOW,
    CPG_CKIO_INVALID_LOW_NORM_ON_STDBY_OP_DEEP_LOWHIGH,
    CPG_CKIO_OFF_HIZ,
    CPG_CKIO_INVALID_UNSTBLE_NORM_ON_STDBY_DEEP_HIZ,
    CPG_CKIO_INVALID_UNSTBLE_NORM_ON_STDBY_DEEP_LOW,
    CPG_CKIO_INVALID_UNSTBLE_NORM_ON_STDBY_OP_DEEP_LOWHIGH,
    CPG_CKIO_INVALID_UNSTBLE_OFF_HIZ,
} e_cpg_ckio_pin_t;

typedef struct
{
    e_cpg_clock_source_t clk_src;           /*!< CPG Main system clock source */
    float64_t main_clk_frequency_khz;       /*!< CPG Required main clock frequency */
} st_r_drv_cpg_set_main_t;

typedef struct
{
    e_cpg_sub_clock_t clk_sub_src;          /*!< CPG sub-clock to configure */
    float64_t sub_clk_frequency_khz;        /*!< CPG Required sub clock frequency */
} st_r_drv_cpg_set_sub_t;

typedef struct
{
    st_r_drv_cpg_set_sub_t * p_sub_clk_settings;     /*!< Pointer to array of CPG sub-clock settings to configure */
    uint32_t number_of_settings;                     /*!< Number of sub-clock settings in array at above location */
} st_r_drv_cpg_set_sub_multi_t;

typedef struct
{
    e_cpg_sub_clock_sel_t clk_sub_selection;     /*!< CPG clock to configure */
    e_cpg_sub_clock_set_input_t clk_src_option;  /*!< CPG clock input selection */
} st_r_drv_cpg_set_src_t;

typedef struct
{
    e_cpg_ckio_pin_t clk_ext; /*!< CPG external clock behaviour configuration */
} st_r_drv_cpg_ext_clk_t;

/*! @enum e_cpg_get_freq_t
 *  @brief For retrieve clock frequency
 */
typedef enum
{
    CPG_FREQ_EXTAL = 0,
    CPG_FREQ_ICLK,
    CPG_FREQ_GCLK,
    CPG_FREQ_BCLK,
    CPG_FREQ_P1CLK,
    CPG_FREQ_P0CLK,
    CPG_FREQ_RTCEXTAL,
} e_r_drv_cpg_get_freq_src_t;

typedef struct
{
    e_r_drv_cpg_get_freq_src_t freq_src;    /*!< CPG frequency source */
    float64_t clk_frequency_khz;            /*!< CPG clock frequency */
} st_r_drv_cpg_get_clk_t;

/******************************************************************************
 Constant Data
 ******************************************************************************/
/** Exposed Driver configuration data structure */




/******************************************************************************
 Interface
 ******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/**
 * @fn         R_CPG_InitialiseHwIf
 * @brief      Prepare the CPG driver for operation
 * @retval     DRV_SUCCESS Successful operate
 */
int_t R_CPG_InitialiseHwIf(void);

/**
 * @fn         R_CPG_UninitialiseHwIf
 * @brief      Finalise the CPG driver for operation
 * @retval     DRV_SUCCESS Successful operate
 */
int_t R_CPG_UninitialiseHwIf(void);

/**
 * @fn         R_CPG_SetXtalClock
 * @brief      Change XTAL clock
 *             @note It is only store in local variable and does not
 *                   re-configure each driver's parameter.
 * @param[in]  frequency_khz: XTAL clock by KHz
 * @retval     DRV_SUCCESS     Successful operate
 * @retval     DRV_ERROR Failure operate
 */
extern int_t R_CPG_SetXtalClock(float64_t frequency_khz);

/**
 * @fn         R_CPG_SetMainClock
 * @brief      Change Main clock frequency
 *             @note EBK_RZA2M can not adjust main clock ratio.
 *                   If dedicated frequency is not fit in the ratio, this
 *                   function fails.
 * @param[in]  p_main_clk_settings: main clock frequency setting
 * @retval     DRV_SUCCESS     Successful operate
 * @retval     DRV_ERROR Failure operate
 */
extern int_t R_CPG_SetMainClock(const st_r_drv_cpg_set_main_t * p_main_clk_settings);

/**
 * @fn    R_CPG_ConfigExtPinClock
 * @brief Set External Clock Pin Function
 *  @return Error code
 */
extern int_t R_CPG_ConfigExtPinClock(const st_r_drv_cpg_ext_clk_t * p_ext_pin_settings);

/**
 * @fn             R_CPG_SetSubClockDividers
 * @brief          Set clock divider for suitable the specified sub clock parameter
 * @param[in]      p_sub_clk_settings: sub clock frequency setting
 * @param[in]      count: count of sub clock settings
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 * @note RZ/A2M restricts the combination of clock ratio in listed below:
 *       FRQCR  | Instruction | Bus  | Peripheral1
 *       ------ | ----------- | ---- | -----------
 *       0x012  | 1/2         | 1/8  | 1/16
 *       0x112  | 1/4         | 1/8  | 1/16
 *       0x212  | 1/8         | 1/8  | 1/16
 *       0x322  | 1/16        | 1/16 | 1/16
 *       0x333  | 1/16        | 1/32 | 1/32
 *
 *       Settings except listed above may causes unpredictable behaviour and
 *       this function does not allow.
 */
extern int_t R_CPG_SetSubClockDividers(const st_r_drv_cpg_set_sub_t * p_sub_clk_settings, uint32_t count);

/**
 * @fn             R_CPG_SetSubClockSource
 * @brief          Set clock selector for external clock
 * @param[in]      p_sub_clk_settings: external clock selector setting
 * @param[in]      count: count of sub clock source
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
extern int_t R_CPG_SetSubClockSource(const st_r_drv_cpg_set_src_t * p_sub_clk_settings, uint32_t count);

/**
 * @fn             R_CPG_ConfigExtClockPin
 * @brief          Set external CKIO clock behaviour that controls supplying
 *                 external clock output while in software standby state and
 *                 in deep standby state.
 * @param[in]      p_ext_pin_settings: external clock behaviour setting
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
extern int_t R_CPG_ConfigExtClockPin(const st_r_drv_cpg_ext_clk_t * p_ext_pin_settings);

/**
 * @fn             R_CPG_GetClock
 * @brief          Get current clock frequency
 * @param[in]      src: desired clock source
 * @param[out]     p_freq: obtained frequency
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
extern int_t R_CPG_GetClock(e_r_drv_cpg_get_freq_src_t src, float64_t * p_freq);

/**
 * @fn          R_CPG_GetVersion
 * @brief       Gets the version number of this low-level driver
 * @param[out]  p_ver_info: returns the driver information
 * @retval      DRV_SUCCESS Always returned
 */
extern int_t R_CPG_GetVersion(st_ver_info_t *p_ver_info);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* R_CPG_LLD_RZA2M_H_*/

/* End of File */
