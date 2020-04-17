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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_cpg_lld_rza2m.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2Studio Ver 7.5.0
*              : GNU Arm Embedded Toolchain 6-2017-q2-update
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : RZ/A2M Sample Program - Low layer function of CPG module
* Operation    :
* Limitations  :
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "rza_io_regrw.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "r_typedefs.h"
#include "r_cpg_hld_api.h"
#include "r_cpg_drv_link.h"
#include "r_cpg_drv_sc_cfg.h"
#include "r_cpg_lld_rza2m.h"

/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
/*! Version Information */
static const st_drv_info_t gs_lld_info =
{
    {
        ((STDIO_CPG_RZ_LLD_VERSION_MAJOR << 16) + STDIO_CPG_RZ_LLD_VERSION_MINOR)
    },
    STDIO_CPG_RZ_LLD_BUILD_NUM,
    STDIO_CPG_RZ_LLD_DRV_NAME
};

/*! EXTAL frequency (khz) */
static float64_t gs_cpg_extal_frequency_khz = 24000.0;

/*! PLL frequency (khz) */
static float64_t gs_cpg_pll_frequency_khz = 1056000.0;

/*! ICLK divisor */
static uint32_t gs_cpg_iclk_divisor = 2;

/*! ICLK frequency (khz) */
static float64_t gs_cpg_iclk_frequency_khz = 528000.0;

/*! BCLK divisor */
static uint32_t gs_cpg_bclk_divisor = 8;

/*! BCLK frequency (khz) */
static float64_t gs_cpg_bclk_frequency_khz = 132000.0;

/*! P1CLK divisor */
static uint32_t gs_cpg_p1clk_divisor = 16;

/*! P1CLK frequency (khz) */
static float64_t gs_cpg_p1clk_frequency_khz = 66000.0;

/*! flag to check initialised or not */
static bool_t gs_drv_cpg_is_initialized = false;

#if R_ENABLE_LOCAL_8_BIT_REGISTER_ACCESS
/**
 * @fn         cpg_io_reg_write_8
 * @brief      Write 8 bit I/O register
 * @param[in]  ioreg: register address
 * @param[in]  write_value: value to write
 * @param[in]  shift: beginning target bit in register (LSB is 0)
 * @param[in]  mask: target bit mask in register
 *                   @note if pass IOREG_NONMASK_ACCESS to the mask,
 *                         this function does not read the previous value
 *                         and write entire bit of write_value without shifting.
 */
static void cpg_io_reg_write_8(volatile uint8_t * ioreg, uint8_t write_value, uint8_t shift, uint32_t mask)
{
    uint8_t reg_value;
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        reg_value = *ioreg;                                         /* Read from register */
        reg_value = (uint8_t)((reg_value & (~mask)) | (unsigned)(write_value << shift)); /* Modify value       */
    }
    else
    {
        reg_value = write_value;
    }
    *ioreg    = reg_value;                                      /* Write to register  */
}
/******************************************************************************
 * End of function cpg_io_reg_write_8
 ******************************************************************************/
#endif // #if R_ENABLE_LOCAL_8_BIT_REGISTER_ACCESS

/**
 * @fn         cpg_io_reg_write_16
 * @brief      Write 16 bit I/O register
 * @param[in]  ioreg: register address
 * @param[in]  write_value: value to write
 * @param[in]  shift: beginning target bit in register (LSB is 0)
 * @param[in]  mask: target bit mask in register
 *                   @note if pass IOREG_NONMASK_ACCESS to the mask,
 *                         this function does not read the previous value
 *                         and write entire bit of write_value without shifting.
 */
static void cpg_io_reg_write_16(volatile uint16_t * ioreg, uint16_t write_value, uint16_t shift, uint32_t mask)
{
    uint16_t reg_value;

    if ( IOREG_NONMASK_ACCESS != mask )
    {
        reg_value = *ioreg;                                         /* Read from register */
        reg_value = (uint16_t)((reg_value & (~mask)) | (unsigned)(write_value << shift)); /* Modify value       */
    }
    else
    {
        reg_value = write_value;
    }
    *ioreg    = reg_value;                                      /* Write to register  */
}
/******************************************************************************
 * End of function cpg_io_reg_write_16
 ******************************************************************************/

/**
 * @fn         cpg_io_reg_write_32
 * @brief      Write 32 bit I/O register
 * @param[in]  ioreg: register address
 * @param[in]  write_value: value to write
 * @param[in]  shift: beginning target bit in register (LSB is 0)
 * @param[in]  mask: target bit mask in register
 *                   @note if pass IOREG_NONMASK_ACCESS to the mask,
 *                         this function does not read the previous value
 *                         and write entire bit of write_value without shifting.
 */
static void cpg_io_reg_write_32(volatile uint32_t * ioreg, uint32_t write_value, uint32_t shift, uint32_t mask)
{
    uint32_t reg_value;
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        /* Read from register */
        reg_value = *ioreg;

        /* Modify value       */
        reg_value = (reg_value & (~mask)) | (unsigned)(write_value << shift);
    }
    else
    {
        reg_value = write_value;
    }
    *ioreg    = reg_value;                                      /* Write to register  */
}
/******************************************************************************
 * End of function cpg_io_reg_write_32
 ******************************************************************************/

#if R_ENABLE_LOCAL_8_BIT_REGISTER_ACCESS
/**
 * @fn         cpg_io_reg_read_8
 * @brief      Read 8 bit I/O register
 * @param[in]  ioreg: register address
 * @param[in]  shift: beginning target bit in register (LSB is 0)
 * @param[in]  mask: target bit mask in register
 * @return     read value
 */
static uint8_t cpg_io_reg_read_8(volatile uint8_t * ioreg, uint8_t shift, uint32_t mask)
{
    uint8_t reg_value;

    /* Read from register */
    reg_value = *ioreg;
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        /* Clear other bit and Bit shift */
        reg_value = (uint8_t)((reg_value & mask) >> shift);
    }

    return (reg_value);
}
/******************************************************************************
 * End of function cpg_io_reg_read_8
 ******************************************************************************/
#endif // #if R_ENABLE_LOCAL_8_BIT_REGISTER_ACCESS

/**
 * @fn         cpg_io_reg_read_16
 * @brief      Read 16 bit I/O register
 * @param[in]  ioreg: register address
 * @param[in]  shift: beginning target bit in register (LSB is 0)
 * @param[in]  mask: target bit mask in register
 * @return     read value
 */
static uint16_t cpg_io_reg_read_16(volatile uint16_t * ioreg, uint16_t shift, uint32_t mask)
{
    uint16_t reg_value;

    /* Read from register */
    reg_value = *ioreg;
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        /* Clear other bit and Bit shift */
        reg_value = (uint16_t)((reg_value & mask) >> shift);
    }

    return (reg_value);
}
/******************************************************************************
 * End of function cpg_io_reg_read_16
 ******************************************************************************/

/**
 * @fn         cpg_io_reg_read_32
 * @brief      Read 32 bit I/O register
 * @param[in]  ioreg: register address
 * @param[in]  shift: beginning target bit in register (LSB is 0)
 * @param[in]  mask: target bit mask in register
 * @return     read value
 */
static uint32_t cpg_io_reg_read_32(volatile uint32_t * ioreg, uint32_t shift, uint32_t mask)
{
    uint32_t reg_value;

    /* Read from register */
    reg_value = *ioreg;
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        /* Clear other bit and Bit shift */
        reg_value = (reg_value & mask) >> shift;
    }

    return (reg_value);
}
/******************************************************************************
 * End of function cpg_io_reg_read_32
 ******************************************************************************/

/**
 * @fn         is_nearly_equal
 * @brief      Check two value is nearly equal or not
 * @param[in]  value1: 1st value
 * @param[in]  value2: 2nd value
 * @retval     true nearly equal
 * @retval     false not nearly equal
 */
static bool_t is_nearly_equal(float64_t value1, float64_t value2)
{
    bool_t result;
    static const float64_t margin = 0.125;      /* margin */
    float64_t low_water = value1 - margin;
    float64_t high_water = value1 + margin;
    if ( (low_water < value2) && (value2 < high_water) )
    {
        result = true;
    }
    else
    {
        result = false;
    }
    return (result);
}
/******************************************************************************
 * End of function is_nearly_equal
 ******************************************************************************/

/**
 * @fn         R_CPG_InitialiseHwIf
 * @brief      Prepare the CPG driver for operation
 * @retval     DRV_SUCCESS Successful operate
 */
int_t R_CPG_InitialiseHwIf( void )
{
    int_t retval;

    retval = DRV_SUCCESS;

    /* only initialises at first time */
    if ( !gs_drv_cpg_is_initialized )
    {
        /* standby_mode_en bit of Power Control Register setting */
        cpg_io_reg_write_32(&pl310.REG15_POWER_CTRL.LONG, 1, PL310_REG15_POWER_CTRL_standby_mode_en_SHIFT, PL310_REG15_POWER_CTRL_standby_mode_en);
        cpg_io_reg_read_32(&pl310.REG15_POWER_CTRL.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
        gs_drv_cpg_is_initialized = true;

        /* Initialise clock setting configured on SC by user */
        retval = R_CPG_SetXtalClock(s_sc_cpg_xtal_frequency_khz_config);
        if ( DRV_SUCCESS == retval )
        {
            retval = R_CPG_SetMainClock(&s_sc_cpg_main_clock_config);
        }
        if ( DRV_SUCCESS == retval )
        {
            retval = R_CPG_SetSubClockDividers(&s_sc_cpg_sub_clock_div_config[0], (sizeof s_sc_cpg_sub_clock_div_config)/(sizeof(st_r_drv_cpg_set_sub_t)));
        }
        if ( DRV_SUCCESS == retval )
        {
            retval = R_CPG_SetSubClockSource(&s_sc_cpg_sub_clock_src_config[0], (sizeof s_sc_cpg_sub_clock_src_config)/(sizeof(st_r_drv_cpg_set_src_t)));
        }
        if ( DRV_SUCCESS == retval )
        {
            retval = R_CPG_ConfigExtClockPin(&s_sc_cpg_ext_clk_config);
        }

    }

    return (retval);
}
/******************************************************************************
 * End of function R_CPG_InitialiseHwIf
 ******************************************************************************/

/**
 * @fn         R_CPG_UninitialiseHwIf
 * @brief      Finalise the CPG driver for operation
 * @retval     DRV_SUCCESS Successful operate
 */
int_t R_CPG_UninitialiseHwIf( void )
{

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_CPG_UninitialiseHwIf
 ******************************************************************************/

/**
 * @fn         R_CPG_SetXtalClock
 * @brief      Change XTAL clock
 *             @note It is only store in local variable and does not
 *                   re-configure each driver's parameter.
 * @param[in]  frequency_khz: XTAL clock by KHz
 * @retval     DRV_SUCCESS     Successful operate
 * @retval     DRV_ERROR Failure operate
 */
int_t R_CPG_SetXtalClock(float64_t frequency_khz)
{
    int_t ret;
    if ( (frequency_khz >= 10000.0) && (frequency_khz <= 12000.0) )
    {
        gs_cpg_pll_frequency_khz = frequency_khz * 88.0;
        ret = DRV_SUCCESS;
    }
    else if ( (frequency_khz >= 20000.0) && (frequency_khz <= 24000.0) )
    {
        gs_cpg_pll_frequency_khz = frequency_khz * 44.0;
        ret = DRV_SUCCESS;
    }
    else
    {
        ret = DRV_ERROR;
    }

    if ( DRV_SUCCESS == ret )
    {
        /* calculate updated frequency */
        gs_cpg_iclk_frequency_khz = gs_cpg_pll_frequency_khz / gs_cpg_iclk_divisor;
        gs_cpg_bclk_frequency_khz = gs_cpg_pll_frequency_khz / gs_cpg_bclk_divisor;
        gs_cpg_p1clk_frequency_khz = gs_cpg_pll_frequency_khz / gs_cpg_p1clk_divisor;
        gs_cpg_extal_frequency_khz = frequency_khz;
    }

    return (ret);
}
/******************************************************************************
 * End of function R_CPG_SetXtalClock
 ******************************************************************************/

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
int_t R_CPG_SetMainClock(const st_r_drv_cpg_set_main_t * p_main_clk_settings)
{
    /* Can't modify main clock on RZA2M.
     * This function just checks if value is reasonable.
     */

    if ( CPG_CLOCK_SOURCE_PLL != p_main_clk_settings->clk_src )
    {
        /* RZA2M main clock only support from PLL */
        return (DRV_ERROR);
    }

    if ( !is_nearly_equal(p_main_clk_settings->main_clk_frequency_khz, gs_cpg_pll_frequency_khz) )
    {
        return (DRV_ERROR);
    }

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_CPG_SetMainClock
 ******************************************************************************/

/**
 * @fn             cpg_modify_frqcr
 * @brief          Make frqcr value for setting clock ratio
 * @param[in]      p_sub_clk_setting: sub clock frequency setting
 * @param[in/out]  p_frqcr: receive previous value and return modified
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
static int_t cpg_modify_frqcr(const st_r_drv_cpg_set_sub_t * p_sub_clk_setting, uint16_t * p_frqcr)
{
    float64_t div_d;
    uint16_t fc;

    /* here the comparison floating value with zero is OK for avoiding divide by zero */
    if ( is_nearly_equal( 0, p_sub_clk_setting->sub_clk_frequency_khz ) )
    {
        /* ZERO value gets error */
        return (DRV_ERROR);
    }
    div_d = gs_cpg_pll_frequency_khz / p_sub_clk_setting->sub_clk_frequency_khz;

    if ( CPG_SUB_CLOCK_ICLK == p_sub_clk_setting->clk_sub_src )
    {
        if ( is_nearly_equal( 2 , div_d ) )
        {
            fc = 0;
        }
        else if ( is_nearly_equal( 4 , div_d ) )
        {
            fc = 1;
        }
        else if ( is_nearly_equal( 8 , div_d ) )
        {
            fc = 2;
        }
        else if ( is_nearly_equal( 16 , div_d ) )
        {
            fc = 3;
        }
        else
        {
            return (DRV_ERROR);
        }

        /* clear IFC bit */
        (*p_frqcr) &= (uint16_t)(~CPG_FRQCR_IFC);

        /* modify IFC bit */
        (*p_frqcr) |= (uint16_t)(fc << CPG_FRQCR_IFC_SHIFT);
    }
    else if ( CPG_SUB_CLOCK_BCLK == p_sub_clk_setting->clk_sub_src )
    {
        if ( is_nearly_equal( 8 , div_d ) )
        {
            fc = 1;
        }
        else if ( is_nearly_equal( 16 , div_d ) )
        {
            fc = 2;
        }
        else if ( is_nearly_equal( 32 , div_d ) )
        {
            fc = 3;
        }
        else
        {
            return (DRV_ERROR);
        }

        /* clear BFC bit */
        (*p_frqcr) &= (uint16_t)(~CPG_FRQCR_BFC);

        /* modify BFC bit */
        (*p_frqcr) |= (uint16_t)(fc << CPG_FRQCR_BFC_SHIFT);
    }
    else if ( CPG_SUB_CLOCK_P1CLK == p_sub_clk_setting->clk_sub_src )
    {
        if ( is_nearly_equal( 16 , div_d ) )
        {
            fc = 2;
        }
        else if ( is_nearly_equal( 32 , div_d ) )
        {
            fc = 3;
        }
        else
        {
            return (DRV_ERROR);
        }

        /* clear PFC bit */
        (*p_frqcr) &= (uint16_t)(~CPG_FRQCR_PFC);

        /* modify PFC bit */
        (*p_frqcr) |= (uint16_t)(fc << CPG_FRQCR_PFC_SHIFT);
    }
    else
    {
        return (DRV_ERROR);
    }

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function cpg_modify_frqcr
 ******************************************************************************/

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
int_t R_CPG_SetSubClockDividers(const st_r_drv_cpg_set_sub_t * p_sub_clk_settings, uint32_t count)
{
    uint16_t frqcr;
    uint16_t check_frqcr;
    int_t ret;

    /* read previous settings */
    frqcr = cpg_io_reg_read_16(&CPG.FRQCR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* overwrite parameter settings */
    while (0 != count)
    {
        count--;

        ret = cpg_modify_frqcr(&p_sub_clk_settings[count], &frqcr);
        if ( DRV_SUCCESS != ret )
        {
            return (ret);
        }
    }

    /* check unless valid combination */
    check_frqcr = frqcr & (CPG_FRQCR_IFC | CPG_FRQCR_BFC | CPG_FRQCR_PFC);
    switch ( check_frqcr )
    {
        case 0x012: /* "VALID":do nothing, fall through */
        case 0x112: /* "VALID":do nothing, fall through */
        case 0x212: /* "VALID":do nothing, fall through */
        case 0x322: /* "VALID":do nothing, fall through */
        case 0x333: /* "VALID":do nothing, fall through */
        {
            break;
        }
        default:
        {
            return (DRV_ERROR);
        }
    }

    /* update local divisor variables */
    switch((frqcr & CPG_FRQCR_IFC)>>CPG_FRQCR_IFC_SHIFT)
    {
        case 0:
        {
            gs_cpg_iclk_divisor = 2;
            break;
        }
        case 1:
        {
            gs_cpg_iclk_divisor = 4;
            break;
        }
        case 2:
        {
            gs_cpg_iclk_divisor = 8;
            break;
        }
        case 3:
        {
            gs_cpg_iclk_divisor = 16;
            break;
        }
        default:
        {
            /* never execute */
            break;
        }
    }
    switch((frqcr & CPG_FRQCR_BFC)>>CPG_FRQCR_BFC_SHIFT)
    {
        case 1:
        {
            gs_cpg_bclk_divisor = 8;
            break;
        }
        case 2:
        {
            gs_cpg_bclk_divisor = 16;
            break;
        }
        case 3:
        {
            gs_cpg_bclk_divisor = 32;
            break;
        }
        default:
        {
            /* never execute */
            break;
        }
    }
    switch((frqcr & CPG_FRQCR_PFC)>>CPG_FRQCR_PFC_SHIFT)
    {
        case 2:
        {
            gs_cpg_p1clk_divisor = 16;
            break;
        }
        case 3:
        {
            gs_cpg_p1clk_divisor = 32;
            break;
        }
        default:
        {
            /* never execute */
            break;
        }
    }

    /* calculate updated frequency */
    gs_cpg_iclk_frequency_khz = gs_cpg_pll_frequency_khz / gs_cpg_iclk_divisor;
    gs_cpg_bclk_frequency_khz = gs_cpg_pll_frequency_khz / gs_cpg_bclk_divisor;
    gs_cpg_p1clk_frequency_khz = gs_cpg_pll_frequency_khz / gs_cpg_p1clk_divisor;

    /* write new settings */
    cpg_io_reg_write_16(&CPG.FRQCR.WORD, frqcr, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    cpg_io_reg_read_16(&CPG.FRQCR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_CPG_SetSubClockDividers
 ******************************************************************************/

/**
 * @fn             set_sub_clock_source
 * @brief          Make CKIOSEL and SCLKSEL value for setting external clock
 * @param[in]      p_sub_clk_setting: external clock selector setting
 * @param[out]     p_ckiosel: clock selector value for CKIO pin
 * @param[out]     p_sclksel: clock selector value for serial flash
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
static int_t set_sub_clock_source(const st_r_drv_cpg_set_src_t * p_sub_clk_setting, uint16_t *p_ckiosel, uint16_t *p_sclksel)
{
    if ( CPG_SUB_CLOCK_CKIO == p_sub_clk_setting->clk_sub_selection )
    {
        if ( CPG_SUB_CLOCK_BCLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear CKIOSEL bits */
            (*p_ckiosel) &= (uint16_t)(~CPG_CKIOSEL_CKIOSEL);

            /* modify CKIOSEL bits to 0 to select P1CLK for CKIO clock source */
            (*p_ckiosel) |= (uint16_t)(0 << CPG_CKIOSEL_CKIOSEL_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_P1CLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear CKIOSEL bits */
            (*p_ckiosel) &= (uint16_t)(~CPG_CKIOSEL_CKIOSEL);

            /* modify CKIOSEL bits to 1 to select P1CLK for CKIO clock source */
            (*p_ckiosel) |= (uint16_t)(1 << CPG_CKIOSEL_CKIOSEL_SHIFT);
        }
        else
        {
            return (DRV_ERROR);
        }
    }
    else if ( CPG_SUB_CLOCK_OCTAMEM == p_sub_clk_setting->clk_sub_selection )
    {
        if ( CPG_SUB_CLOCK_P0CLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear OCTCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_OCTCR);

            /* modify OCTCR bits to 0 to select P0CLK for OCTA clock source */
            (*p_sclksel) |= (uint16_t)(0 << CPG_SCLKSEL_OCTCR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_P1CLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear OCTCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_OCTCR);

            /* modify OCTCR bits to 1 to select P1CLK for OCTA clock source */
            (*p_sclksel) |= (uint16_t)(1 << CPG_SCLKSEL_OCTCR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_BCLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear OCTCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_OCTCR);

            /* modify OCTCR bits to 2 to select BCLK for OCTA clock source */
            (*p_sclksel) |= (uint16_t)(2 << CPG_SCLKSEL_OCTCR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_GCLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear OCTCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_OCTCR);

            /* modify OCTCR bits to 3 to select GCLK for OCTA clock source */
            (*p_sclksel) |= (uint16_t)(3 << CPG_SCLKSEL_OCTCR_SHIFT);
        }
        else
        {
            return (DRV_ERROR);
        }
    }
    else if ( CPG_SUB_CLOCK_HYPERBUS == p_sub_clk_setting->clk_sub_selection )
    {
        if ( CPG_SUB_CLOCK_P0CLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear HYMCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_HYMCR);

            /* modify HYMCR bits to 0 to select P0CLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(0 << CPG_SCLKSEL_HYMCR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_P1CLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear HYMCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_HYMCR);

            /* modify HYMCR bits to 1 to select P1CLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(1 << CPG_SCLKSEL_HYMCR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_BCLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear HYMCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_HYMCR);

            /* modify HYMCR bits to 2 to select BCLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(2 << CPG_SCLKSEL_HYMCR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_GCLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear HYMCR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_HYMCR);

            /* modify HYMCR bits to 3 to select GCLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(3 << CPG_SCLKSEL_HYMCR_SHIFT);
        }
        else
        {
            return (DRV_ERROR);
        }
    }
    else if ( CPG_SUB_CLOCK_SPICLK == p_sub_clk_setting->clk_sub_selection )
    {
        if ( CPG_SUB_CLOCK_P0CLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear SPICR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_SPICR);

            /* modify SPICR bits to 0 to select P0CLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(0 << CPG_SCLKSEL_SPICR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_P1CLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear SPICR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_SPICR);

            /* modify SPICR bits to 1 to select P1CLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(1 << CPG_SCLKSEL_SPICR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_BCLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear SPICR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_SPICR);

            /* modify SPICR bits to 2 to select BCLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(2 << CPG_SCLKSEL_SPICR_SHIFT);
        }
        else if ( CPG_SUB_CLOCK_GCLK_IN == p_sub_clk_setting->clk_src_option )
        {
            /* clear SPICR bits */
            (*p_sclksel) &= (uint16_t)(~CPG_SCLKSEL_SPICR);

            /* modify SPICR bits to 3 to select GCLK for HYPER clock source */
            (*p_sclksel) |= (uint16_t)(3 << CPG_SCLKSEL_SPICR_SHIFT);
        }
        else
        {
            return (DRV_ERROR);
        }
    }
    else
    {
        return (DRV_ERROR);
    }
    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function set_sub_clock_source
 ******************************************************************************/

/**
 * @fn             R_CPG_SetSubClockSource
 * @brief          Set clock selector for external clock
 * @param[in]      p_sub_clk_settings: external clock selector setting
 * @param[in]      count: count of sub clock source
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
int_t R_CPG_SetSubClockSource(const st_r_drv_cpg_set_src_t * p_sub_clk_settings, uint32_t count)
{
    int_t ret;
    uint16_t ckiosel;
    uint16_t sclksel;

    /* read previous settings */
    ckiosel = cpg_io_reg_read_16(&CPG.CKIOSEL.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    sclksel = cpg_io_reg_read_16(&CPG.SCLKSEL.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* overwrite parameter settings */
    while (0 != count)
    {
        count--;

        ret = set_sub_clock_source(&p_sub_clk_settings[count], &ckiosel, &sclksel);
        if ( DRV_SUCCESS != ret )
        {
            return (ret);
        }
    }

    /* write new settings */
    cpg_io_reg_write_16(&CPG.CKIOSEL.WORD, ckiosel, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    cpg_io_reg_write_16(&CPG.SCLKSEL.WORD, sclksel, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    cpg_io_reg_read_16(&CPG.CKIOSEL.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    cpg_io_reg_read_16(&CPG.SCLKSEL.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_CPG_SetSubClockSource
 ******************************************************************************/

/**
 * @fn             R_CPG_ConfigExtClockPin
 * @brief          Set external CKIO clock behaviour that controls supplying
 *                 external clock output while in software standby state and
 *                 in deep standby state.
 * @param[in]      p_ext_pin_settings: external clock behaviour setting
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
int_t R_CPG_ConfigExtClockPin(const st_r_drv_cpg_ext_clk_t * p_ext_pin_settings)
{
    uint16_t frqcr;

    /* read previous settings */
    frqcr = cpg_io_reg_read_16(&CPG.FRQCR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* keep bit except CPG_FRQCR_CKOEN and CPG_FRQCR_CKOEN2 */
    frqcr &= (uint16_t)(~(CPG_FRQCR_CKOEN|CPG_FRQCR_CKOEN2));

    /* overwrite parameter settings */
    switch ( p_ext_pin_settings->clk_ext )
    {
        case CPG_CKIO_INVALID_LOW_NORM_ON_STDBY_DEEP_HIZ:
        {
            frqcr |= ((1<<CPG_FRQCR_CKOEN2_SHIFT)|(0<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        case CPG_CKIO_INVALID_LOW_NORM_ON_STDBY_DEEP_LOW:
        {
            frqcr |= ((1<<CPG_FRQCR_CKOEN2_SHIFT)|(1<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        case CPG_CKIO_INVALID_LOW_NORM_ON_STDBY_OP_DEEP_LOWHIGH:
        {
            frqcr |= ((1<<CPG_FRQCR_CKOEN2_SHIFT)|(2<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        case CPG_CKIO_OFF_HIZ:
        {
            frqcr |= ((1<<CPG_FRQCR_CKOEN2_SHIFT)|(3<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        case CPG_CKIO_INVALID_UNSTBLE_NORM_ON_STDBY_DEEP_HIZ:
        {
            frqcr |= ((0<<CPG_FRQCR_CKOEN2_SHIFT)|(0<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        case CPG_CKIO_INVALID_UNSTBLE_NORM_ON_STDBY_DEEP_LOW:
        {
            frqcr |= ((0<<CPG_FRQCR_CKOEN2_SHIFT)|(1<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        case CPG_CKIO_INVALID_UNSTBLE_NORM_ON_STDBY_OP_DEEP_LOWHIGH:
        {
            frqcr |= ((0<<CPG_FRQCR_CKOEN2_SHIFT)|(2<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        case CPG_CKIO_INVALID_UNSTBLE_OFF_HIZ:
        {
            frqcr |= ((0<<CPG_FRQCR_CKOEN2_SHIFT)|(3<<CPG_FRQCR_CKOEN_SHIFT));
            break;
        }
        default:
        {
            return DRV_ERROR;
        }
    }

    /* write new settings */
    cpg_io_reg_write_16(&CPG.FRQCR.WORD, frqcr, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    cpg_io_reg_read_16(&CPG.FRQCR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_CPG_ConfigExtPinClock
 ******************************************************************************/

/**
 * @fn             R_CPG_GetClock
 * @brief          Get current clock frequency
 * @param[in]      src: desired clock source
 * @param[out]     p_freq: obtained frequency
 * @retval         DRV_SUCCESS     Successful operate
 * @retval         DRV_ERROR Failure operate
 */
int_t R_CPG_GetClock(e_r_drv_cpg_get_freq_src_t src, float64_t * p_freq)
{
    if ( NULL == p_freq )
    {
        return (DRV_ERROR);
    }

    switch ( src )
    {
        case CPG_FREQ_EXTAL:
        {
            *p_freq = gs_cpg_extal_frequency_khz;
            break;
        }
        case CPG_FREQ_ICLK:
        {
            *p_freq = gs_cpg_iclk_frequency_khz;
            break;
        }
        case CPG_FREQ_GCLK:
        {
            *p_freq = (gs_cpg_pll_frequency_khz * 2.0) / gs_cpg_bclk_divisor;
            break;
        }
        case CPG_FREQ_BCLK:
        {
            *p_freq = gs_cpg_bclk_frequency_khz;
            break;
        }
        case CPG_FREQ_P1CLK:
        {
            *p_freq = gs_cpg_p1clk_frequency_khz;
            break;
        }
        case CPG_FREQ_P0CLK:
        {
            *p_freq = gs_cpg_pll_frequency_khz / 32.0;
            break;
        }
        case CPG_FREQ_RTCEXTAL:
        {
            *p_freq = gs_cpg_extal_frequency_khz;
            break;
        }
        default:
        {
            return (DRV_ERROR);
        }
    }

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_CPG_GetClock
 ******************************************************************************/

/**
 * @fn          R_CPG_GetVersion
 * @brief       Gets the version number of this low-level driver
 * @param[out]  p_ver_info: returns the driver information
 * @retval      DRV_SUCCESS Always returned
 */
int_t R_CPG_GetVersion(st_ver_info_t *p_ver_info)
{
    p_ver_info->lld.p_szdriver_name = gs_lld_info.p_szdriver_name;
    p_ver_info->lld.version.sub.major = gs_lld_info.version.sub.major;
    p_ver_info->lld.version.sub.minor = gs_lld_info.version.sub.minor;
    p_ver_info->lld.build = gs_lld_info.build;

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_CPG_GetVersion
 ******************************************************************************/

/* End of File */
