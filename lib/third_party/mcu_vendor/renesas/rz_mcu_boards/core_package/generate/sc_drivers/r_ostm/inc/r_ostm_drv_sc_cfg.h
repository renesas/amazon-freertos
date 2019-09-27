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
 *
 * Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/*******************************************************************************
 * @headerfile     r_ostm_drv_sc_cfg.h
 * @brief          Smart Configurator table define.
 * @version        1.00
 * @date           16.05.2018
 * H/W Platform    RZ/A2M
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 
 *******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_ostm_drv_api.h"

#ifndef R_OSTM_DRV_SC_CFG_H_
#define R_OSTM_DRV_SC_CFG_H_

#if defined(__cplusplus)
extern "C" {
#endif
/* This code is auto-generated. Do not edit manually */
extern void FreeRTOS_Tick_Handler(void);
/* End of modification */
#if defined(__cplusplus)
}
#endif

/**
 * @typedef st_r_drv_ostm_sc_config_t Smart Configurator Interface
 */
typedef struct
{
    int_t                    channel;     /*!< OSTM Channel*/
    st_r_drv_ostm_config_t   config;      /*!< Configuration for that channel*/
} st_r_drv_ostm_sc_config_t;

/**
 * @section SCIFA_SC_TABLE Smart configurator settings table.
 */
static const st_r_drv_ostm_sc_config_t OSTM_SC_TABLE[] =
{
    /* This code is auto-generated. Do not edit manually */
    { 0, 
        {
            OSTM_MODE_INTERVAL, 
            OSTM_TIME_MS, 
            1000, 
            OSTM_START_INTERRUPT_OFF, 
            INTC_ENABLE, 
            31, 
            FreeRTOS_Tick_Handler, 
        }
    },
    /* End of modification */
};


#endif /* R_OSTM_DRV_SC_CFG_H_ */


/*******************************************************************************************************************//**
End  Of File
 ***********************************************************************************************************************/
