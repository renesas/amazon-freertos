/*********************************************************************************************************************
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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*********************************************************************************************************************/
/*********************************************************************************************************************
* File Name    : rxdemo_i2c_api.h
* Version      : 1.00
* Description  : Using the i2c.
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 08.01.2018  1.00    First Release
*********************************************************************************************************************/

#ifndef SYNERGY_I2C_API_H_
#define SYNERGY_I2C_API_H_

#include "platform.h"
#include "r_smc_entry.h"
#include "r_riic_rx_if.h"

int8_t rxdemo_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t rxdemo_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
void rxdemo_delay_ms(uint32_t delay);

#endif /* SYNERGY_I2C_API_H_ */
