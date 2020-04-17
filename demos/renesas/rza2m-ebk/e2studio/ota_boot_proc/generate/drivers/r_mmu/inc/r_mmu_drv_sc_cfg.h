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
* File Name    : r_mmu_drv_sc_cfg.h
* Date         : 2019-11-05
* version      : 1.0.5
* Description  : MMU configuration table updated by Smart Configurator
******************************************************************************/

#ifndef R_SC_RZA_MMU_H_
#define R_SC_RZA_MMU_H_

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    uint32_t virtual_address;
    uint32_t physical_address;
    uint16_t page_count;
    uint32_t page_attribute;
} st_r_mmu_sc_config_t;

/******************************************************************************
Variable Externs
******************************************************************************/
static const st_r_mmu_sc_config_t MMU_SC_TABLE[] =
{
    /* This code is auto-generated. Do not edit manually */
    {0x00000000uL, 0x00000000uL, 4096, MMU_ATTR_UNUSED | MMU_ATTR_DOMAIN(15) },             /* default */
    {0x00000000uL, 0x00000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x04000000uL, 0x04000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x08000000uL, 0x08000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x0C000000uL, 0x0C000000uL, 64, MMU_ATTR_NORMAL_L1L2CACHE | MMU_ATTR_DOMAIN(15)},
    {0x10000000uL, 0x10000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x14000000uL, 0x14000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x18000000uL, 0x18000000uL, 112, MMU_ATTR_RESERVED | MMU_ATTR_DOMAIN(15)},
    {0x1F000000uL, 0x1F000000uL, 16, MMU_ATTR_STRONGLY | MMU_ATTR_DOMAIN(15)},
    {0x20000000uL, 0x20000000uL, 256, MMU_ATTR_NORMAL_L1L2CACHE | MMU_ATTR_DOMAIN(15)},
    {0x30000000uL, 0x30000000uL, 256, MMU_ATTR_NORMAL_L1L2CACHE | MMU_ATTR_DOMAIN(15)},
    {0x40000000uL, 0x40000000uL, 256, MMU_ATTR_NORMAL_L1L2CACHE | MMU_ATTR_DOMAIN(15)},
    {0x50000000uL, 0x50000000uL, 256, MMU_ATTR_NORMAL_L1L2CACHE | MMU_ATTR_DOMAIN(15)},
    {0x60000000uL, 0x60000000uL, 256, MMU_ATTR_NORMAL_L1L2CACHE | MMU_ATTR_DOMAIN(15)},
    {0x70000000uL, 0x20000000uL, 256, MMU_ATTR_STRONGLY_NS_EXECUTABLE | MMU_ATTR_DOMAIN(15)},
    {0x80000000uL, 0x80000000uL, 4, MMU_ATTR_NORMAL_L1CACHE | MMU_ATTR_DOMAIN(15)},
    {0x80400000uL, 0x80400000uL, 28, MMU_ATTR_RESERVED | MMU_ATTR_DOMAIN(15)},
    {0x82000000uL, 0x80000000uL, 4, MMU_ATTR_NORMAL | MMU_ATTR_DOMAIN(15)},
    {0x82400000uL, 0x82400000uL, 92, MMU_ATTR_RESERVED | MMU_ATTR_DOMAIN(15)},
    {0x88000000uL, 0x00000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x8C000000uL, 0x04000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x90000000uL, 0x08000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x94000000uL, 0x0C000000uL, 64, MMU_ATTR_NORMAL | MMU_ATTR_DOMAIN(15)},
    {0x98000000uL, 0x10000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0x9C000000uL, 0x14000000uL, 64, MMU_ATTR_STRONGLY_NS | MMU_ATTR_DOMAIN(15)},
    {0xA0000000uL, 0x30000000uL, 256, MMU_ATTR_STRONGLY_NS_EXECUTABLE | MMU_ATTR_DOMAIN(15)},
    {0xB0000000uL, 0x40000000uL, 256, MMU_ATTR_NORMAL | MMU_ATTR_DOMAIN(15)},
    {0xC0000000uL, 0x50000000uL, 256, MMU_ATTR_STRONGLY_NS_EXECUTABLE | MMU_ATTR_DOMAIN(15)},
    {0xD0000000uL, 0x60000000uL, 256, MMU_ATTR_NORMAL | MMU_ATTR_DOMAIN(15)},
    {0xE0000000uL, 0xE0000000uL, 128, MMU_ATTR_RESERVED | MMU_ATTR_DOMAIN(15)},
    {0xE8000000uL, 0xE8000000uL, 384, MMU_ATTR_STRONGLY | MMU_ATTR_DOMAIN(15)},
    /* End of modification */
    
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
};

#endif /* R_SC_RZA_MMU_H_ */

/* End of File */

