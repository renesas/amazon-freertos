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
* File Name    : r_pinset.h
* Version      : 1.01
* Device(s)    : R7S921053
* Tool-Chain   : GCC
* Description  : Setting of port and mpc registers
* Creation Date: 2018-12-06
**********************************************************************************************************************/

#ifndef R_FIT_PINSET_H
#define R_FIT_PINSET_H

/**********************************************************************************************************************
Includes
**********************************************************************************************************************/

/**********************************************************************************************************************
Global variables and functions
**********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_ETHER_PinSet_ETHERC_RMII
 * @brief         This function initializes pins for r_ether_rza2 module.
 * @warning       None.
 * @param [in]    None.
 * @param [out]   None.
 * @retval        None.
 *********************************************************************************************************************/
void R_ETHER_PinSet_ETHERC_RMII(void);

#endif
