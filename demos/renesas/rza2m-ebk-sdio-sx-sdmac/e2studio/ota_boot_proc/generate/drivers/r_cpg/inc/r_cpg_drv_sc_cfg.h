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
/******************************************************************************
* File Name    : r_cpg_drv_sc_cfg.h
* Date         : 2019-11-05
* version      : 1.0.6
* Device(s)    : R7S921053
* Description  : CPG Driver setting file from Smart Configurator
******************************************************************************/
#ifndef R_CPG_DRV_SC_CFG_H_
#define R_CPG_DRV_SC_CFG_H_

/* This code is auto-generated. Do not edit manually */
static const float64_t s_sc_cpg_xtal_frequency_khz_config = 24000.0;

static const st_r_drv_cpg_set_main_t s_sc_cpg_main_clock_config =
  {CPG_CLOCK_SOURCE_PLL, 1056000.0};

static const st_r_drv_cpg_set_sub_t s_sc_cpg_sub_clock_div_config[] =
{
  {CPG_SUB_CLOCK_ICLK, 528000.0},
  {CPG_SUB_CLOCK_BCLK, 132000.0},
  {CPG_SUB_CLOCK_P1CLK, 66000.0},
};

static const st_r_drv_cpg_set_src_t s_sc_cpg_sub_clock_src_config[] =
{
  {CPG_SUB_CLOCK_CKIO, CPG_SUB_CLOCK_BCLK_IN},
  {CPG_SUB_CLOCK_OCTAMEM, CPG_SUB_CLOCK_GCLK_IN},
  {CPG_SUB_CLOCK_HYPERBUS, CPG_SUB_CLOCK_GCLK_IN},
  {CPG_SUB_CLOCK_SPICLK, CPG_SUB_CLOCK_BCLK_IN},
};

static const st_r_drv_cpg_ext_clk_t s_sc_cpg_ext_clk_config =
  {CPG_CKIO_INVALID_UNSTBLE_NORM_ON_STDBY_DEEP_HIZ};
/* End of modification */

#endif /* R_CPG_DRV_SC_CFG_H_ */
/* End of File */
