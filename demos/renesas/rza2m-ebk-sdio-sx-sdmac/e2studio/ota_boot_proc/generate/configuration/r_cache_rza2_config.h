/******************************************************************************
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
 * Renesas reserves the right, without notice, to make changes to this
 * software and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * File Name    : r_cache_rza2_config.h
 * Description  : User modifiable CACHE configuration table
 *                (This file currently not updated by Smart Configurator)
 *****************************************************************************/

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#ifndef R_CACHE_RZA2_CONFIG_H_
#define R_CACHE_RZA2_CONFIG_H_

/* select whether PL310 double line fill enable or disable
 * 0 : Disable PL310 double line fill
 * 1 : Enable PL310 double line fill
 */
#define PL310_CFG_DOUBLE_LINE_FILL      (0)

/* select whether PL310 instruction prefetch enable or disable
 * 0 : Disable PL310 instruction prefetch
 * 1 : Enable PL310 instruction prefetch
 */
#define PL310_CFG_INSTRUCTION_PREFETCH  (0)

/* select whether PL310 data prefetch enable or disable
 * 0 : Disable PL310 data prefetch
 * 1 : Enable PL310 data prefetch
 */
#define PL310_CFG_DATA_PREFETCH         (0)

#endif /* R_CACHE_RZA2_CONFIG_H_ */
/* END of File */
