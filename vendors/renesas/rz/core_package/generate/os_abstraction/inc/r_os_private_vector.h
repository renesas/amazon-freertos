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
 * File Name    : r_os_private_vector.h
 * Version      : 1.0
 * Description  : This file contains #defines ONLY, to allow the reset vectors to be updated at compile time
 *********************************************************************************************************************/

#ifndef SRC_RENESAS_CONFIGURATION_R_OS_ABSTRACTION_INC_R_OS_VECTOR_H_
#define SRC_RENESAS_CONFIGURATION_R_OS_ABSTRACTION_INC_R_OS_VECTOR_H_

/** Vector handlers */
#define R_OS_ABSTRACTION_CFG_PRV_RESET_HANDLER     (reset_handler)
#define R_OS_ABSTRACTION_CFG_PRV_UNDEFINED_HANDLER (undefined_handler)
#define R_OS_ABSTRACTION_CFG_PRV_SVC_HANDLER       (FreeRTOS_SWI_Handler)
#define R_OS_ABSTRACTION_CFG_PRV_PREFETCH_HANDLER  (prefetch_handler)
#define R_OS_ABSTRACTION_CFG_PRV_ABORT_HANDLER     (abort_handler)
#define R_OS_ABSTRACTION_CFG_PRV_RESERVED_HANDLER  (reserved_handler)
#define R_OS_ABSTRACTION_CFG_PRV_IRQ_HANDLER       (FreeRTOS_IRQ_Handler)
#define R_OS_ABSTRACTION_CFG_PRV_FIQ_HANDLER       (fiq_handler)

#endif /* SRC_RENESAS_CONFIGURATION_R_OS_ABSTRACTION_INC_R_OS_VECTOR_H_ */
