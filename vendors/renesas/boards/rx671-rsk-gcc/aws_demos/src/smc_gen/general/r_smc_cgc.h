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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_smc_cgc.h
* Version      : 1.6.102
* Device(s)    : R5F5671EHxFB
* Description  : CGC setting header file.
* Creation Date: 2021-04-28
***********************************************************************************************************************/

#ifndef SMC_CGC_H
#define SMC_CGC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    High-Speed On-Chip Oscillator Control Register 2 (HOCOCR2)
*/
/* HOCO Frequency Setting (HCFRQ[1:0]) */
#define _00_CGC_HOCO_CLK_16                 (0x00U) /* 16 MHz */
#define _01_CGC_HOCO_CLK_18                 (0x01U) /* 18 MHz */
#define _02_CGC_HOCO_CLK_20                 (0x02U) /* 20 MHz */

/*
    Main Clock Oscillator Forced Oscillation Control Register (MOFCR)
*/
/* Main Oscillator Drive Capability 2 Switching (MODRV2[1:0]) */
#define _00_CGC_MAINOSC_UNDER24M            (0x00U) /* 20.1 to 24 MHz */
#define _10_CGC_MAINOSC_UNDER20M            (0x10U) /* 16.1 to 20 MHz */
#define _20_CGC_MAINOSC_UNDER16M            (0x20U) /* 8.1 to 16 MHz */
#define _30_CGC_MAINOSC_EQUATE8M            (0x30U) /* 8 MHz */
/* Main Clock Oscillator Switch (MOSEL) */
#define _00_CGC_MAINOSC_RESONATOR           (0x00U) /* Resonator */
#define _40_CGC_MAINOSC_EXTERNAL            (0x40U) /* External oscillator input */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_CGC_Create(void);
void R_CGC_Create_UserInit(void);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
