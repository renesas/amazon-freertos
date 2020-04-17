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
@* File Name   : cpu.asm
@* Description : cpu c15 sccess, etc
@*******************************************************************************/


    .arm

@******************************************************************************
@ Function Name : __set_sctlr
@ Description   : Set SCTLR register
@******************************************************************************
    .section .text.__set_sctlr
    .global __set_sctlr
    .type   __set_sctlr,%function
    .func   __set_sctlr
__set_sctlr:
    MCR  p15, 0, r0, c1, c0, 0      @;; SCTLR
    BX  LR

    .endfunc


@******************************************************************************
@ Function Name : __get_sctlr
@ Description   : Get SCTLR register
@******************************************************************************
    .section .text.__get_sctlr
    .global __get_sctlr
    .type   __get_sctlr,%function
    .func   __get_sctlr
__get_sctlr:
    MRC  p15, 0, r0, c1, c0, 0      @;; SCTLR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __set_cpsr
@ Description   : Set CPSR register
@******************************************************************************
    .section .text.__set_cpsr
    .global __set_cpsr
    .type   __set_cpsr,%function
    .func   __set_cpsr
__set_cpsr:
    MSR  cpsr, r0
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __get_cpsr
@ Description   : Get CPSR register
@******************************************************************************
    .section .text.__get_cpsr
    .global __get_cpsr
    .type   __get_cpsr,%function
    .func   __get_cpsr
__get_cpsr:
    MRS  r0, cpsr
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __set_vbar
@ Description   : Set VBAR register
@******************************************************************************
    .section .text.__set_vbar
    .global __set_vbar
    .type   __set_vbar,%function
    .func   __set_vbar
__set_vbar:
    MCR  p15, 0, r0, c12, c0, 0      @;; VBAR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __get_vbar
@ Description   : Get VBAR register
@******************************************************************************
    .section .text.__get_vbar
    .global __get_vbar
    .type   __get_vbar,%function
    .func   __get_vbar

__get_vbar:
    MRC  p15, 0, r0, c12, c0, 0      @;; VBAR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __wfi
@ Description   : WFI
@******************************************************************************
    .section .text.__wfi
    .global __wfi
    .type   __wfi,%function
    .func   __wfi

__wfi:
    WFI
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __isb
@ Description   : ISB
@******************************************************************************
    .section .text.__isb
    .global __isb
    .type   __isb,%function
    .func   __isb

__isb:
    ISB
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __dsb
@ Description   : DSB
@******************************************************************************
    .section .text.__dsb
    .global __dsb
    .type   __dsb,%function
    .func   __dsb

__dsb:
    DSB
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : __dmb
@ Description   : DMB
@******************************************************************************
    .section .text.__dmb
    .global __dmb
    .type   __dmb,%function
    .func   __dmb

__dmb:
    DMB
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : R_SoftwareDelay
@ Description   : delay loop
@******************************************************************************
    .section .text.R_SoftwareDelay
    .global R_SoftwareDelay
    .type   R_SoftwareDelay,%function
R_SoftwareDelay:
    .fnstart
    CMP r0, #0
1:  BXEQ  LR
    SUBS  r0, r0, #1
    B     1b
    .size   R_SoftwareDelay, .-R_SoftwareDelay
    .cantunwind
    .fnend

    .end
