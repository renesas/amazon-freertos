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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : hyperbus_flash_rza2m.h
 * Version      : 1.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "r_typedefs.h"
#include "driver.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef RENESAS_HWSETUP_HYPERBUS_SETUP_INC_HYPERBUS_FLASH_RZA2M_H_
#define RENESAS_HWSETUP_HYPERBUS_SETUP_INC_HYPERBUS_FLASH_RZA2M_H_

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @fn             HyperFlash_WriteWord
 * @brief          Read SR of HyperFlash
 *                 
 * @param[in]      baddr        Base Address of HyperFlash
 * @param[in]      waddr        Write address
 * @retval         SR value
 */
extern uint16_t HyperFlash_WriteWord(uint32_t baddr, uint32_t waddr, uint16_t wdata);

/**
 * @fn             HyperFlash_EraseSect
 * @brief          Sector erase
 *                 
 * @param[in]      baddr        Base Address of HyperFlash
 * @param[in]      saddr        Sector address
 * @retval         DRV_SUCCESS  Successful operation
 * @retval         DRV_ERROR    Failure operation
 */
extern int_t HyperFlash_EraseSect(uint32_t baddr, uint32_t saddr);

/**
 * @fn             HyperFlash_ReadVCR
 * @brief          Read VCR of HyperFlash
 * @param[in]      addr Base Address of HyperFlash
 *                 
 * @retval         VCR value
 */
extern uint16_t HyperFlash_ReadVCR(uint32_t baddr);

/**
 * @fn             HyperFlash_WriteVCR
 * @param[in]      baddr Base Address of HyperFlash
 * @param[in]      wdata Write data to VCR
 *                 
 * @retval         VCR value
 */
extern int_t HyperFlash_WriteVCR(uint32_t baddr, uint16_t wdata);

/**
 * @fn             HyperFlash_ReadROMInfo
 * @brief          Read Device ID and CFI of HyperFlash
 *                 
 * @param[in]      baddr Base Address of HyperFlash
 * @param[in]      idbuf Read buffer
 * @retval         none
 */
extern void HyperFlash_ReadROMInfo(uint32_t baddr, uint16_t idbuf[]);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RENESAS_HWSETUP_HYPERBUS_SETUP_INC_HYPERBUS_FLASH_RZA2M_H_ */
