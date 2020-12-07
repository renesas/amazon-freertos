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
 * File Name    : hyperbus_ram_rza2m.h
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

#ifndef RENESAS_HWSETUP_HYPERBUS_SETUP_INC_HYPERBUS_RAM_RZA2M_H_
#define RENESAS_HWSETUP_HYPERBUS_SETUP_INC_HYPERBUS_RAM_RZA2M_H_

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
 * @fn             HyperRAM_ReadID0
 * @brief          Read ID0 register of HyperRAM
 *                 
 * @param[in]      baddr Base address of HyperRAM
 * @retval         ID0 value
 */
extern uint16_t HyperRAM_ReadID0(uint32_t baddr);

/**
 * @fn             HyperRAM_ReadID1
 * @brief          Read ID1 register of HyperRAM
 *                 
 * @param[in]      baddr Base address of HyperRAM
 * @retval         ID1 value
 */
extern uint16_t HyperRAM_ReadID1(uint32_t baddr);

/**
 * @fn             HyperRAM_ReadCR0
 * @brief          Read CR0 register of HyperRAM
 *                 
 * @param[in]      baddr Base address of HyperRAM
 * @retval         CR0 value
 */
extern uint16_t HyperRAM_ReadCR0(uint32_t baddr);

/**
 * @fn             HyperRAM_WriteCR0
 * @brief          Write CR0 register of HyperRAM
 *                 
 * @param[in]      baddr Base address of HyperRAM
 * @param[in]      wdata Write data to CR0
 * @retval         CR0 value
 */
extern int_t HyperRAM_WriteCR0(uint32_t baddr, uint16_t wdata);

/**
 * @fn             HyperRAM_ReadCR1
 * @brief          Read CR1 register of HyperRAM
 *                 
 * @param[in]      baddr Base address of HyperRAM
 * @retval         CR1 value
 */
extern uint16_t HyperRAM_ReadCR1(uint32_t baddr);

/**
 * @fn             HyperRAM_WriteCR1
 * @brief          Write CR1 register of HyperRAM
 *                 
 * @param[in]      baddr Base address of HyperRAM
 * @param[in]      wdata Write data to CR1
 * @retval         CR0 value
 */
extern int_t HyperRAM_WriteCR1(uint32_t baddr, uint16_t wdata);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RENESAS_HWSETUP_HYPERBUS_SETUP_INC_HYPERBUS_RAM_RZA2M_H_ */
