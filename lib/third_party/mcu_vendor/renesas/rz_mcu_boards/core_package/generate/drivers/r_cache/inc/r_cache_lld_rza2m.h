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
* File Name    : r_cache_lld_rza2m.h
* Version      : 1.0
* Description  : Definition of the Cortex-A9 Cache API function.
*******************************************************************************/
/*********************************************************************//**
* @ingroup RENESAS_DRIVER_LIBRARY
* @defgroup R_CACHE_LLD_API CACHE RZA2 EBK_RZA2M Low Layer Driver
*
* @section R_CACHE_LLD_API_SUMMARY Summary
* The cache interface provides cache control functionality.
* The cache interface can operate the primary cache of Cortex-A9 and the
* secondary cache of PL 310.
* @{
************************************************************************/

/*******************************************************************************
Includes <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>

#include "driver.h"

#ifndef R_CACHE_LLD_RZA2M_H
#define R_CACHE_LLD_RZA2M_H

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/* Version Number of API */

#define STDIO_CACHE_RZ_LLD_DRV_NAME ("LLD EBK_RZA2M CACHE")

/** Major Version Number of API */
#define STDIO_CACHE_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API */
#define STDIO_CACHE_RZ_LLD_VERSION_MINOR      (3)
/** Minor Version Number of API */
#define STDIO_CACHE_RZ_LLD_BUILD_NUM          (0)
/** Unique ID */
#define STDIO_CACHE_RZ_LLD_UID                (111)

/******************************************************************************
Functions Prototypes
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @fn    R_CACHE_L1Init
 * @brief Initializes the Cortex-A9 cache.
 *
 *        This function initializes cache of the Cortex-A9 in the following procedure.
 *            1. Enable data cache of the Cortex-A9.
 *            2. Enable instruction cache of the Cortex-A9.
 *            3. Enable branch predicton of the Cortex-A9.
 *            4. Enable prefetching of the Cortex-A9.
 *
 * @note This function does not perform cache invalidate operation.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1Init(void);

/**
 * @fn    R_CACHE_L1InstInvalidAll
 * @brief Performs invalidate operation to
 *        all cache lines of the Cortex-A9 instruction cache.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1InstInvalidAll(void);

/**
 * @fn    R_CACHE_L1DataInvalidAll
 * @brief Performs invalidate operation to
 *        all cache lines of the Cortex-A9 data cache.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1DataInvalidAll(void);

/**
 * @fn    R_CACHE_L1DataCleanAll
 * @brief Performs clean operation to
 *        all cache lines of the Cortex-A9 data cache.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1DataCleanAll(void);

/**
 * @fn    R_CACHE_L1DataCleanInvalidAll
 * @brief Performs combination of clean
 *        and invalidate operations to all cache lines of the Cortex-A9 data cache.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1DataCleanInvalidAll(void);

/**
 * @fn    R_CACHE_L1DataInvalidLine
 * @brief Performs invalidation operations
 *        on all cache lines included in the specified address range of
 *        the Cortex-A9 data cache.
 *
 *        If this function receives an area that is not aligned with the
 *        cache line boundary, it extends the range to the entire cache
 *        line including the specified area.
 *        If the size is 0, doing anything and success is returned.
 *        If the end address exceeds 0xFFFFFFFF, an error is returned.
 *
 * @param[in]  line_addr Starting address of cache operation(virtual address).
 * @param[in]  size Byte size from line_addr.
 *
 * @retval   0 DRV_SUCCESS
 * @retval  -1 DRV_ERROR
 */
extern e_err_code_t R_CACHE_L1DataInvalidLine(void* line_addr, uint32_t size);

/**
 * @fn    R_CACHE_L1DataCleanLine
 * @brief Performs clean operations
 *        on all cache lines included in the specified address range of
 *        the Cortex-A9 data cache.
 *
 *        If this function receives an area that is not aligned with the
 *        cache line boundary, it extends the range to the entire cache
 *        line including the specified area.
 *        If the size is 0, doing anything and success is returned.
 *        If the end address exceeds 0xFFFFFFFF, an error is returned.
 *
 * @param[in]  line_addr Starting address of cache operation(virtual address).
 * @param[in]  size Byte size from line_addr.
 *
 * @retval   0 DRV_SUCCESS
 * @retval  -1 DRV_ERROR
 */
extern e_err_code_t R_CACHE_L1DataCleanLine(void* line_addr, uint32_t size);

/**
 * @fn    R_CACHE_L1DataCleanInvalidLine
 * @brief Performs combination of clean
 *        and invalidate operations on all cache lines included in the specified
 *        address range of the Cortex-A9 data cache.
 *
 *        If this function receives an area that is not aligned with the
 *        cache line boundary, it extends the range to the entire cache
 *        line including the specified area.
 *        If the size is 0, doing anything and success is returned.
 *        If the end address exceeds 0xFFFFFFFF, an error is returned.
 *
 * @param[in]  line_addr Starting address of cache operation(virtual address).
 * @param[in]  size Byte size from line_addr.
 *
 * @retval   0 DRV_SUCCESS
 * @retval  -1 DRV_ERROR
 */
extern e_err_code_t R_CACHE_L1DataCleanInvalidLine(void* line_addr, uint32_t size);

/**
 * @fn    R_CACHE_L1InstEnable
 * @brief Enables instruction cache of the Cortex-A9.
 *
 *        This function sets the following bit of the SCTLR register of the Cortex-A9.
 *            - bit12 : I bit
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1InstEnable(void);

/**
 * @fn    R_CACHE_L1InstDisable
 * @brief Disables instruction cache of the Cortex-A9.
 *
 *        This function clears the following bit of the SCTLR register of the Cortex-A9.
 *            - bit12 : I bit
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1InstDisable(void);

/**
 * @fn    R_CACHE_L1DataEnable
 * @brief Enables data cache of the Cortex-A9.
 *
 *        This function sets the following bit of the SCTLR register of the Cortex-A9.
 *            - bit2 : C bit
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1DataEnable(void);

/**
 * @fn    R_CACHE_L1DataDisable
 * @brief Disables data cache of the Cortex-A9.
 *
 *        This function clears the following bit of the SCTLR register of the Cortex-A9.
 *            - bit2 : C bit
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1DataDisable(void);

/**
 * @fn    R_CACHE_L1BtacEnable
 * @brief Enables branch prediction of the Cortex-A9.
 *
 *        This function sets the following bit of the SCTLR register of the Cortex-A9.
 *            - bit11 : Z bit
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1BtacEnable(void);

/**
 * @fn    R_CACHE_L1BtacDisable
 * @brief Disables branch prediction of the Cortex-A9.
 *
 *        This function clears the following bit of the SCTLR register of the Cortex-A9.
 *            - bit11 : Z bit
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1BtacDisable(void);

/**
 * @fn    R_CACHE_L1BtacInvalidate
 * @brief Performs invalidation operations on the
 *       all entries of the Cortex-A9 branch predictor.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1BtacInvalidate(void);

/**
 * @fn    R_CACHE_L1PrefetchEnable
 * @brief Enables prefetching capability of the Cortex-A9.
 *
 *        This function sets the following bit of the ACTLR register of the Cortex-A9.
 *            - bit2 : L1 Prefetch enable
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1PrefetchEnable(void);

/**
 * @fn    R_CACHE_L1PrefetchDisable
 * @brief Disables prefetching capability of the Cortex-A9.
 *
 *        This function clears the following bit of the ACTLR register of the Cortex-A9.
 *            - bit2 : L1 Prefetch enable
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L1PrefetchDisable(void);

/**
 * @fn    R_CACHE_L2Init
 * @brief Initializes the PL310 cache.
 *
 *        This function initializes the PL310 in the following procedure.
 *            1. Disable cache of the PL310.
 *            2. Enable prefetching capability of the PL310.
 *            3. Invalidate all cache entries of the PL310.
 *            4. Enable cache of the PL310.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2Init(void);

/**
 * @fn    R_CACHE_L2CacheEnable
 * @brief Enables cache of the PL310.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2CacheEnable(void);

/**
 * @fn    R_CACHE_L2CacheDisable
 * @brief Disables cache of the PL310.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2CacheDisable(void);

/**
 * @fn    R_CACHE_L2PrefetchEnable
 * @brief Enables prefetching capability of the PL310.
 *
 *        This function sets the following bits of the REG15_PREFETCH_CTRL
 *        register of PL310.
 *            - bit28 : Data prefetch enable (If PL310_CFG_DATA_PREFETCH==1)
 *            - bit29 : Instruction prefetch enable (If PL310_CFG_INSTRUCTION_PREFETCH==1)
 *            - bit30 : Double linefill enable (If PL310_CFG_DOUBLE_LINE_FILL==1)
 *
 *         Which bit to set depends on the macro setting in the configuration header
 *         file r_cache_rza2_config.h.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2PrefetchEnable(void);

/**
 * @fn    R_CACHE_L2PrefetchDisable
 * @brief Disables prefetching capability of the PL310.
 *
 *        This function clears the following bits of the REG15_PREFETCH_CTRL
 *        register of PL310.
 *            - bit28 : Data prefetch enable
 *            - bit29 : Instruction prefetch enable
 *            - bit30 : Double linefill enable
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2PrefetchDisable(void);

/**
 * @fn    R_CACHE_L2InvalidAll
 * @brief Performs invalidate operation to
 * all cache lines of the PL310 cache.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2InvalidAll(void);

/**
 * @fn    R_CACHE_L2CleanAll
 * @brief Performs clean operation to
 *        all cache lines of the PL310 cache.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2CleanAll(void);

/**
 * @fn    R_CACHE_L2CleanInvalidAll
 * @brief Performs combination of clean
 *        and invalidate operations to all cache lines of the PL310 cache.
 *
 * @param   none
 *
 * @retval  none
 */
extern void R_CACHE_L2CleanInvalidAll(void);

/**
 * @fn          R_CACHE_GetVersion
 * @brief       Gets the version number of this low-level driver
 * @param[out]  p_ver_info: returns the driver information
 * @retval      DRV_SUCCESS Always returned
 */
extern int_t R_CACHE_GetVersion(st_ver_info_t *p_ver_info);


#ifdef __cplusplus
}
#endif /* __cplusplus */

/*********************************************************************//**
* @}
************************************************************************/
#endif /* R_CACHE_LLD_RZA2M_H */

/* End of File */

