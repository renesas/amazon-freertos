@/*******************************************************************************
@* DISCLAIMER
@* This software is supplied by Renesas Electronics Corporation and is only
@* intended for use with Renesas products. No other uses are authorized. This
@* software is owned by Renesas Electronics Corporation and is protected under
@* all applicable laws, including copyright laws.
@* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
@* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
@* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
@* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
@* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
@* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
@* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
@* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
@* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
@* Renesas reserves the right, without notice, to make changes to this software
@* and to discontinue the availability of this software. By using this software,
@* you agree to the additional terms and conditions found by accessing the
@* following link:
@* http://www.renesas.com/disclaimer
@* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
@*******************************************************************************/
@/*******************************************************************************
@* File Name   : irq.asm
@* Description : enable/disable irq
@*******************************************************************************/


@******************************************************************************
@ Function Name : __disable_irq
@ Description   : Disable IRQ
@******************************************************************************
    .section .text.__disable_irq
    .global __disable_irq
    .type   __disable_irq,%function
    .func   __disable_irq
__disable_irq:
    MRS     r0,APSR             @ formerly CPSR
    AND     r0,r0,#0x80
    CPSID   I
    BX      LR

    .endfunc

@******************************************************************************
@ Function Name : __enable_irq
@ Description   : Enable IRQ
@******************************************************************************
    .section .text.__enable_irq
    .global __enable_irq
    .type   __enable_irq,%function
    .func   __enable_irq

__enable_irq:
    CPSIE   I
    BX      LR

    .endfunc

@******************************************************************************
@ Function Name : __disable_fiq
@ Description   : Disable FIQ
@******************************************************************************
    .section .text.__disable_fiq
    .global __disable_fiq
    .type   __disable_fiq,%function
    .func   __disable_fiq

__disable_fiq:
    CPSID   F
    BX      LR

    .endfunc

@******************************************************************************
@ Function Name : __enable_fiq
@ Description   : Enable FIQ
@******************************************************************************
    .section .text.__enable_fiq
    .global __enable_fiq
    .type   __enable_fiq,%function
    .func   __enable_fiq

__enable_fiq:
    CPSIE   F
    BX      LR


    .endfunc


    .END
