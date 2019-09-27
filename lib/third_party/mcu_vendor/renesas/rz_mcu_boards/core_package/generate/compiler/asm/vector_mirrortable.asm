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
@* File Name   : vector_mirrortable.asm
@* Description : Vector mirrortable
@*******************************************************************************/

#include "r_os_private_vector.h"

@==================================================================
@ Entry point for the Reset handler
@==================================================================
    .section    VECTOR_MIRROR_TABLE, #execinstr, #alloc
    .arm

    .extern  reset_handler
    .extern  undefined_handler
    .extern  svc_handler
    .extern  prefetch_handler
    .extern  abort_handler
    .extern  reserved_handler
    .extern  irq_handler
    .extern  fiq_handler

    .global  vector_table2

vector_table2:
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_RESET_HANDLER
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_UNDEFINED_HANDLER
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_SVC_HANDLER
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_PREFETCH_HANDLER
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_ABORT_HANDLER
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_RESERVED_HANDLER
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_IRQ_HANDLER
    LDR pc, =R_OS_ABSTRACTION_CFG_PRV_FIQ_HANDLER

Literals:
    .LTORG

    .END
