/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : Pin.c
* Version      : 1.0.2
* Device(s)    : R5F572MNDxBD
* Description  : This file implements SMC pin code generation.
* Creation Date: 2020-05-22
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for include. Do not edit comment generated here */
#include "Pin.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Pins_Create
* Description  : This function initializes Smart Configurator pins
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Pins_Create(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set CLKOUT25M pin */
    MPC.PH7PFS.BYTE = 0x2AU;
    PORTH.PMR.BYTE |= 0x80U;

    /* Set ET0_COL pin */
    MPC.PK1PFS.BYTE = 0x11U;
    PORTK.PMR.BYTE |= 0x02U;

    /* Set ET0_CRS pin */
    MPC.P83PFS.BYTE = 0x11U;
    PORT8.PMR.BYTE |= 0x08U;

    /* Set ET0_ERXD0 pin */
    MPC.PL0PFS.BYTE = 0x11U;
    PORTL.PMR.BYTE |= 0x01U;

    /* Set ET0_ERXD1 pin */
    MPC.PL1PFS.BYTE = 0x11U;
    PORTL.PMR.BYTE |= 0x02U;

    /* Set ET0_ERXD2 pin */
    MPC.PE4PFS.BYTE = 0x11U;
    PORTE.PMR.BYTE |= 0x10U;

    /* Set ET0_ERXD3 pin */
    MPC.PE3PFS.BYTE = 0x11U;
    PORTE.PMR.BYTE |= 0x08U;

    /* Set ET0_ETXD0 pin */
    MPC.PB5PFS.BYTE = 0x11U;
    PORTB.PMR.BYTE |= 0x20U;

    /* Set ET0_ETXD1 pin */
    MPC.PL5PFS.BYTE = 0x11U;
    PORTL.PMR.BYTE |= 0x20U;

    /* Set ET0_ETXD2 pin */
    MPC.PM4PFS.BYTE = 0x11U;
    PORTM.PMR.BYTE |= 0x10U;

    /* Set ET0_ETXD3 pin */
    MPC.PC6PFS.BYTE = 0x11U;
    PORTC.PMR.BYTE |= 0x40U;

    /* Set ET0_LINKSTA pin */
    MPC.P34PFS.BYTE = 0x11U;
    PORT3.PMR.BYTE |= 0x10U;

    /* Set ET0_MDC pin */
    MPC.PA4PFS.BYTE = 0x11U;
    PORTA.PMR.BYTE |= 0x10U;

    /* Set ET0_MDIO pin */
    MPC.PA3PFS.BYTE = 0x11U;
    PORTA.PMR.BYTE |= 0x08U;

    /* Set ET0_RX_CLK pin */
    MPC.PE5PFS.BYTE = 0x11U;
    PORTE.PMR.BYTE |= 0x20U;

    /* Set ET0_RX_DV pin */
    MPC.PK2PFS.BYTE = 0x11U;
    PORTK.PMR.BYTE |= 0x04U;

    /* Set ET0_RX_ER pin */
    MPC.PB3PFS.BYTE = 0x11U;
    PORTB.PMR.BYTE |= 0x08U;

    /* Set ET0_TX_CLK pin */
    MPC.PM6PFS.BYTE = 0x11U;
    PORTM.PMR.BYTE |= 0x40U;

    /* Set ET0_TX_EN pin */
    MPC.PA0PFS.BYTE = 0x11U;
    PORTA.PMR.BYTE |= 0x01U;

    /* Set RXD6 pin */
    MPC.P01PFS.BYTE = 0x0AU;
    PORT0.PMR.BYTE |= 0x02U;

    /* Set SCK6 pin */
    MPC.P02PFS.BYTE = 0x0AU;
    PORT0.PMR.BYTE |= 0x04U;

    /* Set TXD6 pin */
    PORT0.PODR.BYTE |= 0x01U;
    MPC.P00PFS.BYTE = 0x0AU;
    PORT0.PDR.BYTE |= 0x01U;
    // PORT0.PMR.BIT.B0 = 1U; // Please set the PMR bit after TE bit is set to 1.

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}   

