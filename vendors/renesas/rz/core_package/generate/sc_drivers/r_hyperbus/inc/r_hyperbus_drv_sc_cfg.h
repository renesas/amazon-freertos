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
 * File Name    : r_hyperbus_drv_sc_cfg.h
 * Version      : 1.0
 * Description  : Initialize HYPER function header
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_hyperbus_lld_rza2m_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef SC_DRIVERS_R_HYPERBUS_INC_R_HYPERBUS_DRV_SC_CFG_H_
#define SC_DRIVERS_R_HYPERBUS_INC_R_HYPERBUS_DRV_SC_CFG_H_

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

static const st_hyperbus_cfg_t HYPERBUS_SC_TABLE[] =
{
    /* This code is auto-generated. Do not edit manually */
    {
        HYPERBUS_NO_INIT, 
        HYPERBUS_MAXEN_OFF, 
        0, 
        HYPERBUS_CSHI_1P5_CYCLE, 
        HYPERBUS_CSHI_1P5_CYCLE, 
        HYPERBUS_CSS_1_CYCLE, 
        HYPERBUS_CSS_1_CYCLE, 
        HYPERBUS_CSH_1_CYCLE, 
        HYPERBUS_CSH_1_CYCLE, 
        HYPERBUS_LTCY_16_CYCLE, 
#if (0)
        HYPERBUS_INIT_AT_LOADER,
#else
        HYPERBUS_INIT_AT_APP,
#endif
        HYPERBUS_MAXEN_OFF,
        0, 
        HYPERBUS_CSHI_1P5_CYCLE, 
        HYPERBUS_CSHI_1P5_CYCLE, 
        HYPERBUS_CSS_1_CYCLE, 
        HYPERBUS_CSS_1_CYCLE, 
        HYPERBUS_CSH_1_CYCLE, 
        HYPERBUS_CSH_1_CYCLE, 
        HYPERBUS_LTCY_6_CYCLE, 
    },
    /* End of modification */
};

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* SC_DRIVERS_R_HYPERBUS_INC_R_HYPERBUS_DRV_SC_CFG_H_ */
