/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_compiler_abstraction_api.h
 * Description  : Provides a common api for frequently used compiler specifc operations
 **********************************************************************************************************************/
#include  "r_typedefs.h"

#ifndef SRC_RENESAS_COMPILER_INC_R_COMPILER_ABSTRACTION_API_H_
#define SRC_RENESAS_COMPILER_INC_R_COMPILER_ABSTRACTION_API_H_

#define R_COMPILER_INLINE static inline
#define R_COMPILER_WEAK __attribute__((weak))

extern void __enable_irq(void);
extern uint32_t __disable_irq(void);
extern void __enable_fiq(void);
extern void __disable_fiq(void);


R_COMPILER_INLINE void R_COMPILER_Nop(void) { __asm volatile ("nop"); }

R_COMPILER_INLINE void R_COMPILER_EnableInterrupts(void) { __enable_irq(); }

R_COMPILER_INLINE void R_COMPILER_DisableInterrupts(void) { __disable_irq(); }

#endif /* SRC_RENESAS_COMPILER_INC_R_COMPILER_ABSTRACTION_API_H_ */
