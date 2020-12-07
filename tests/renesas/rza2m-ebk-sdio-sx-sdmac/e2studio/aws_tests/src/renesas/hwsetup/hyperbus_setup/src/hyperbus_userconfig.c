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
 * File Name    : hyperbus_userconfig.c
 * Version      : 1.0
 * Description  : User defined startup function example for RZ/A2M EVB board
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "r_typedefs.h"

#include "r_startup_config.h"
#include "r_hyperbus_lld_rza2m_api.h"
#include "hyperbus_flash_rza2m.h"
#include "hyperbus_ram_rza2m.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define HYPERFLASH_VCR_USER_SETTING_VALUE (0x8e8b)
#define HYPERRAM_CR0_USER_SETTING_VALUE (0x8f0f)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperBus_UserConfig
 * Description  : User defined device initialisation routine for HyperBus
 * Arguments    : p_cfg       : Pointer to the configuration table
 * Return Value : DRV_SUCCESS   Successful operation.
 *              : DRV_ERROR     Error condition occurs.
 *********************************************************************************************************************/
int_t HyperBus_UserConfig(const st_hyperbus_cfg_t *p_cfg)
{
    uint16_t wdata;

    if (((0 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_LOADER == p_cfg->init_flag0))
    ||  ((1 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_APP == p_cfg->init_flag0)))
    {
        /* Set HYPERFLASH_VCR_USER_SETTING_VALUE to wdata without latency field */
        wdata = HYPERFLASH_VCR_USER_SETTING_VALUE & 0xff0f;

        /* Modify latency field by configuration table */
        wdata |= (uint16_t) ((p_cfg->operate_ltcy0) << 4);

        /* Write modified value to VCR */
        HyperFlash_WriteVCR(0x30000000, wdata);
    }

    if (((0 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_LOADER == p_cfg->init_flag1))
    ||  ((1 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_APP == p_cfg->init_flag1)))
    {
        /* Set HYPERRAM_CR0_USER_SETTING_VALUE to wdata without latency field */
        wdata = HYPERRAM_CR0_USER_SETTING_VALUE & 0xff0f;

        /* Modify latency field by configuration table */
        wdata |= (uint16_t) ((p_cfg->operate_ltcy1) << 4);

        /* Write modified value to HypeRAM CR0 */
        HyperRAM_WriteCR0(0x40000000, wdata);
    }
    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function HyperBus_UserConfig
 *********************************************************************************************************************/
