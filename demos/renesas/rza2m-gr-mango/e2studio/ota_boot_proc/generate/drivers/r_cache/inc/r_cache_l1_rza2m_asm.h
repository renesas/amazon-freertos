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
* File Name    : r_cache_l1_rza2m_asm.h
* Version      : 1.0
* Description  : Definition of the Cortex-A9 Cache asm function.
*******************************************************************************/

/*******************************************************************************
Includes <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>

#ifndef R_CACHE_L1_RZA2M_ASM_H
#define R_CACHE_L1_RZA2M_ASM_H
/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/******************************************************************************
 * @fn            r_cache_l1_cache_init
 * @brief         Sub function of initialize the Cortex-A9 Cache
 *****************************************************************************/
extern void r_cache_l1_cache_init(void);

/******************************************************************************
 * @fn            r_cache_l1_i_inv_all
 * @brief         Invalidate whole of the Cortex-A9 instruction cache
 *****************************************************************************/
extern void r_cache_l1_i_inv_all(void);

/******************************************************************************
 * @fn            r_cache_l1_d_cache_operation
 * @brief         Operate whole of the Cortex-A9 data cache
 * @param [in]    operation
 *                    R_CACHE_L1_OP_D_INV_ALL(0)       : Invalidate
 *                    R_CACHE_L1_OP_D_CLEAN_ALL(1)     : Clean
 *                    R_CACHE_L1_OP_D_CLEAN_INV_ALL(2) : Clean and Invalidate
 *****************************************************************************/
extern void r_cache_l1_d_cache_operation(uint32_t operation);

/******************************************************************************
 * @fn            r_cache_l1_d_inv_mva
 * @brief         Apply invalidate operation to a cache line which is included
 *                in the specified address
 * @param [in]    line_addr : starting address of cache operation
 *                            (virtual address)
 *****************************************************************************/
extern void r_cache_l1_d_inv_mva(uint32_t line_addr);

/******************************************************************************
 * @fn            r_cache_l1_d_clean_mva
 * @brief         Apply clean operation to a cache line which is included in
 *                the specified address
 * @param [in]    line_addr : starting address of cache operation
 *                            (virtual address)
 *****************************************************************************/
extern void r_cache_l1_d_clean_mva(uint32_t line_addr);

/******************************************************************************
 * @fn            r_cache_l1_d_clean_inv_mva
 * @brief         Apply clean&invalidate operation to a cache line which is
 *                included in the specified address
 * @param [in]    line_addr : starting address of cache operation
 *                            (virtual address)
 *****************************************************************************/
extern void r_cache_l1_d_clean_inv_mva(uint32_t line_addr);

/******************************************************************************
 * @fn            r_cache_l1_i_enable
 * @brief         Enable the Cortex-A9 instruction cache
 *****************************************************************************/
extern void r_cache_l1_i_enable(void);

/******************************************************************************
 * @fn            r_cache_l1_i_disable
 * @brief         Disable the Cortex-A9 instruction cache
 *****************************************************************************/
extern void r_cache_l1_i_disable(void);

/******************************************************************************
 * @fn            r_cache_l1_d_enable
 * @brief         Enable the Cortex-A9 data cache
 *****************************************************************************/
extern void r_cache_l1_d_enable(void);

/******************************************************************************
 * @fn            r_cache_l1_d_disable
 * @brief         Disable the Cortex-A9 data cache
 *****************************************************************************/
extern void r_cache_l1_d_disable(void);

/******************************************************************************
 * @fn            r_cache_l1_btac_enable
 * @brief         Enable the Cortex-A9 branch prediction
 *****************************************************************************/
extern void r_cache_l1_btac_enable(void);

/******************************************************************************
 * @fn            r_cache_l1_btac_disable
 * @brief         Disable the Cortex-A9 branch prediction
 *****************************************************************************/
extern void r_cache_l1_btac_disable(void);

/******************************************************************************
 * @fn            r_cache_l1_btac_inv
 * @brief         Invalidate whole of the Cortex-A9 branch prediction table
 *****************************************************************************/
extern void r_cache_l1_btac_inv(void);

/******************************************************************************
 * @fn            r_cache_l1_prefetch_enable
 * @brief         Enable the Cortex-A9 prefetching
 *****************************************************************************/
extern void r_cache_l1_prefetch_enable(void);

/******************************************************************************
 * @fn            r_cache_l1_prefetch_disable
 * @brief         Disable the Cortex-A9 prefetching
 *****************************************************************************/
extern void r_cache_l1_prefetch_disable(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* R_CACHE_L1_RZA2M_ASM_H */

/* End of File */

