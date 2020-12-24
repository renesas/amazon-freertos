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
@* File Name   : mmu_operation.asm
@* Description : TTB initialize
@*******************************************************************************/


    .section    .text.mmu
    .arm

@******************************************************************************
@ Function Name : r_mmu_get_ttbcr
@ Description   : Get TTBCR register
@******************************************************************************
    .section .text.r_mmu_get_ttbcr
    .global r_mmu_get_ttbcr
    .type   r_mmu_get_ttbcr,%function
    .func   r_mmu_get_ttbcr

r_mmu_get_ttbcr:

    MRC  p15, 0, r0, c2, c0, 2      @;; TTBCR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_set_ttbcr
@ Description   : Set TTBCR register
@******************************************************************************
    .section .text.r_mmu_set_ttbcr
    .global r_mmu_set_ttbcr
    .type   r_mmu_set_ttbcr,%function
    .func   r_mmu_set_ttbcr

r_mmu_set_ttbcr:

    MCR  p15, 0, r0, c2, c0, 2      @;; TTBCR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_get_ttbr0
@ Description   : Get TTBR0 register
@******************************************************************************
    .section .text.r_mmu_get_ttbr0
    .global r_mmu_get_ttbr0
    .type   r_mmu_get_ttbr0,%function
    .func   r_mmu_get_ttbr0

r_mmu_get_ttbr0:

    MRC  p15, 0, r0, c2, c0, 0      @;; TTBR0
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_set_ttbr0
@ Description   : Set TTBR0 register
@******************************************************************************
    .section .text.r_mmu_set_ttbr0
    .global r_mmu_set_ttbr0
    .type   r_mmu_set_ttbr0,%function
    .func   r_mmu_set_ttbr0

r_mmu_set_ttbr0:

    MCR  p15, 0, r0, c2, c0, 0      @;; TTBR0
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_get_dacr
@ Description   : Get DACR register
@******************************************************************************
    .section .text.r_mmu_get_dacr
    .global r_mmu_get_dacr
    .type   r_mmu_get_dacr,%function
    .func   r_mmu_get_dacr

r_mmu_get_dacr:

    MRC  p15, 0, r0, c3, c0, 0      @;; DACR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_set_dacr
@ Description   : Set DACR register
@******************************************************************************
    .section .text.r_mmu_set_dacr
    .global r_mmu_set_dacr
    .type   r_mmu_set_dacr,%function
    .func   r_mmu_set_dacr


r_mmu_set_dacr:

    MCR  p15, 0, r0, c3, c0, 0      @;; DACR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_get_sctlr
@ Description   : Get SCTLR register
@******************************************************************************
    .section .text.r_mmu_get_sctlr
    .global r_mmu_get_sctlr
    .type   r_mmu_get_sctlr,%function
    .func   r_mmu_get_sctlr


r_mmu_get_sctlr:

    MRC  p15, 0, r0, c1, c0, 0      @;; SCTLR
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_set_sctlr
@ Description   : Set SCTLR register
@******************************************************************************
    .section .text.r_mmu_set_sctlr
    .global r_mmu_set_sctlr
    .type   r_mmu_set_sctlr,%function
    .func   r_mmu_set_sctlr


r_mmu_set_sctlr:

    MCR  p15, 0, r0, c1, c0, 0      @;; SCTLR
    ISB
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_tlbiall
@ Description   : TLBIALL
@******************************************************************************
    .section .text.r_mmu_tlbiall
    .global r_mmu_tlbiall
    .type   r_mmu_tlbiall,%function
    .func   r_mmu_tlbiall


r_mmu_tlbiall:

    MCR  p15, 0, r0, c8, c7, 0      @;; TLBIALL
    ISB
    BX  LR

    .endfunc

@******************************************************************************
@ Function Name : r_mmu_tlbimvaa
@ Description   : TLBIMVAA
@******************************************************************************
    .section .text.r_mmu_tlbimvaa
    .global r_mmu_tlbimvaa
    .type   r_mmu_tlbimvaa,%function
    .func   r_mmu_tlbimvaa

r_mmu_tlbimvaa:

    MCR  p15, 0, r0, c8, c7, 3      @;; TLBIMVAA
    ISB
    BX  LR

    .endfunc


    .END
