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
@* File Name   : reset_handler.asm
@* Description : Reset handler
@*******************************************************************************/


@ Standard definitions of mode bits and interrupt (I & F) flags in PSRs
    .EQU    USR_MODE,   0x10
    .EQU    FIQ_MODE,   0x11
    .EQU    IRQ_MODE,   0x12
    .EQU    SVC_MODE,   0x13
    .EQU    ABT_MODE,   0x17
    .EQU    UND_MODE,   0x1b
    .EQU    SYS_MODE,   0x1f
    .EQU    Thum_bit,   0x20            @ CPSR/SPSR Thumb bit

@==================================================================
@ Entry point for the Reset handler
@==================================================================
    .section    RESET_HANDLER, #execinstr
    .arm

    .extern  __sys_stack_end     @ Linker symbol from scatter file
    .extern  __irq_stack_end     @ Linker symbol from scatter file
    .extern  __fiq_stack_end     @ Linker symbol from scatter file
    .extern  __svc_stack_end     @ Linker symbol from scatter file
    .extern  __abt_stack_end     @ Linker symbol from scatter file

    .weak    DTBL_F
    .weak    BTBL_F
    .weak    DTBL_F2
    .weak    BTBL_F2
    .weak    DTBL
    .weak    BTBL

    .extern  vector_table
    .extern  R_SC_HardwareSetup
    .extern  R_MMU_Init
    .extern  Enable_VFP
    .extern  INITSCT
    .extern  __libc_init_array
    .extern  main
    .extern  exit
    .extern  r_memclk_setup

    .global  reset_handler
    .global  undefined_handler
    .global  svc_handler
    .global  prefetch_handler
    .global  abort_handler
    .global  reserved_handler

@******************************************************************************
@ Function Name : reset_handler
@ Description   : This function is the assembler function executed after reset
@               : cancellation. After initial setting for the stack pointer or
@               : the MMU and reset cancellation, executes initial setting for
@               : the minimum required peripheral functions. Calls the main
@               : function to execute.
@******************************************************************************
reset_handler:

@===================================================================
@ Clock Setting
@ Set FRQCR.IFC : CPU Clock 1/8(132MHz) -> 1/2(528MHz)
@===================================================================
    @; Set standby_mode_en of REG15 Power Control Register
    LDR  r0, =0x1F003F80            @;; Address(REG15 Power Control Register)
    LDR  r1, =0x00000001            @;; Setting Value (REG15 Power Control Register)
    STR  r1, [r0]                   @;; REG15_POWER_CTRL = 0x00000001
    LDR  r1, [r0]                   @;; dummy read

    @; FRQCR = 0x0012 PLL(x88), I:B:P1:P0 = 22:5.5:2.75:1.375
    LDR  r0, =0xFCFE0010            @;; Address(FRQCR Register)
    LDR  r1, =0x0012                @;; Setting Value(FRQCR Register)
    STRH r1, [r0]                   @;; FRQCR = 0x0012
    LDRH r0, [r0]                   @;; dummy read

@===================================================================
@ Set Vector Base Address Register (VBAR) to point to initializer routine
@===================================================================
    LDR r0, =vector_table
    MCR p15, 0, r0, c12, c0, 0

@==================================================================
@ Disable cache and MMU in case it was left enabled from an earlier run
@ This does not need to be done from a cold reset
@==================================================================
    MRC  p15, 0, r0, c1, c0, 0      @;; Read CP15 System Control register (SCTLR)
    BIC  r0, r0, #(0x1 << 13)       @;; Set Low Vectors
    BIC  r0, r0, #(0x1 << 12)       @;; Clear I bit 12 to disable I Cache
    BIC  r0, r0, #(0x1 <<  2)       @;; Clear C bit  2 to disable D Cache
    BIC  r0, r0, #0x1               @;; Clear M bit  0 to disable MMU
    MCR  p15, 0, r0, c1, c0, 0      @;; Write value back to CP15 System Control register

@==================================================================
@ Setting up Stack Area (SVC/IRQ/FIQ/ABT/SYS)
@==================================================================
    CPS  #SVC_MODE                  @;; SVC Mode
    LDR  sp, =__svc_stack_end

    CPS  #IRQ_MODE                  @;; IRQ Mode
    LDR  sp, =__irq_stack_end

    CPS  #FIQ_MODE                  @;; FIQ Mode
    LDR  sp, =__fiq_stack_end

    CPS  #ABT_MODE                  @;; ABT Mode
    LDR  sp, =__abt_stack_end

    CPS  #SYS_MODE                  @;; SYS Mode
    LDR  sp, =__sys_stack_end

#if defined(__ARM_FP) || defined(__ARM_NEON_FP)
@===================================================================
@ Enable the vfp
@===================================================================
    LDR  r12,=Enable_VFP            @;; Save this in register for possible long jump
    BLX  r12                        @;; Call to enable the vfp
#endif

@==================================================================
@ Section initialize for r_memclk_setup
@==================================================================
    LDR  r0, =DTBL_F2                @;; ROM to RAM table for r_memclk_setup
    LDR  r1, =BTBL_F2                @;; RAM zero fill table for r_memclk_setup
    LDR  r12,=INITSCT                @;; Function for Initialize sections
    BLX  r12                         @;; Call

@==================================================================
@ setting memory clock before R_SC_HardwareSetup function
@==================================================================
    LDR  r12,=r_memclk_setup       @;; Save this in register for possible long jump
    BLX  r12                       @;; Hardware Initialize

@==================================================================
@ Section initialize for R_SC_HardwareSetup
@==================================================================
    LDR  r0, =DTBL_F                @;; ROM to RAM table for R_SC_HardwareSetup
    LDR  r1, =BTBL_F                @;; RAM zero fill table for R_SC_HardwareSetup
    LDR  r12,=INITSCT               @;; Function for Initialize sections
    BLX  r12                        @;; Call

@==================================================================
@ Hardware initialized earlier than section-init
@==================================================================
    LDR  r12,=R_SC_HardwareSetup    @;; Save this in register for possible long jump
    BLX  r12                        @;; Hardware Initialize

@==================================================================
@ Initialize MMU translation table
@==================================================================
    LDR  r12,=R_MMU_Init            @;; Save this in register for possible long jump
    BLX  r12                        @;; Initialize MMU

@==================================================================
@ TLB maintenance, Invalidate Data and Instruction TLBs
@==================================================================
    MOV  r0,#0
    MCR  p15, 0, r0, c8, c7, 0      @;; Cortex-A9 I-TLB and D-TLB invalidation (TLBIALL)

@===================================================================
@ Invalidate instruction cache, also flushes BTAC
@===================================================================
    MOV  r0, #0                     @;; SBZ
    MCR  p15, 0, r0, c7, c5, 0      @;; ICIALLU - Invalidate entire I Cache, and flushes branch target cache

@==================================================================
@ Cache Invalidation code for Cortex-A9
@==================================================================
    @;; Invalidate L1 Instruction Cache
    MRC  p15, 1, r0, c0, c0, 1      @;; Read Cache Level ID Register (CLIDR)
    TST  r0, #0x3                   @;; Harvard Cache?
    MOV  r0, #0
    MCRNE   p15, 0, r0, c7, c5, 0   @;; Invalidate Instruction Cache

    @;; Invalidate Data/Unified Caches
    MRC  p15, 1, r0, c0, c0, 1      @;; Read CLIDR
    ANDS r3, r0, #0x07000000        @;; Extract coherency level
    MOV  r3, r3, LSR #23            @;; Total cache levels << 1
    BEQ  Finished                   @;; If 0, no need to clean

    MOV  r10, #0                    @;; R10 holds current cache level << 1
Loop1:
    ADD  r2, r10, r10, LSR #1       @;; R2 holds cache "Set" position
    MOV  r1, r0, LSR r2             @;; Bottom 3 bits are the Cache-type for this level
    AND  r1, r1, #7                 @;; Isolate those lower 3 bits
    CMP  r1, #2
    BLT  Skip                       @;; No cache or only instruction cache at this level

    MCR  p15, 2, r10, c0, c0, 0     @;; Write the Cache Size selection register (CSSELR)
    ISB                             @;; ISB to sync the change to the CacheSizeID reg
    MRC  p15, 1, r1, c0, c0, 0      @;; Reads current Cache Size ID register (CCSIDR)
    AND  r2, r1, #7                 @;; Extract the line length field
    ADD  r2, r2, #4                 @;; Add 4 for the line length offset (log2 16 bytes)
    LDR  r4, =0x3FF
    ANDS r4, r4, r1, LSR #3         @;; R4 is the max number on the way size (right aligned)
    CLZ  r5, r4                     @;; R5 is the bit position of the way size increment
    LDR  r7, =0x7FFF
    ANDS r7, r7, r1, LSR #13        @;; R7 is the max number of the index size (right aligned)
Loop2:
    MOV  r9, r4                     @;; R9 working copy of the max way size (right aligned)

Loop3:
    ORR  r11, r10, r9, LSL r5       @;; Factor in the Way number and cache number into R11
    ORR  r11, r11, r7, LSL r2       @;; Factor in the Set number
    MCR  p15, 0, r11, c7, c6, 2     @;; Invalidate by Set/Way (DCISW)
    SUBS r9, r9, #1                 @;; Decrement the Way number
    BGE  Loop3
    SUBS r7, r7, #1                 @;; Decrement the Set number
    BGE  Loop2
Skip:
    ADD  r10, r10, #2               @;; increment the cache number
    CMP  r3, r10
    BGT  Loop1

Finished:


@===================================================================
@ Enable MMU
@ Leaving the caches disabled until (main).
@===================================================================
    MRC  p15, 0, r0, c1, c0, 0      @;; Read CP15 System Control register (SCTLR)
    BIC  r0, r0, #(0x1 << 12)       @;; Clear I bit 12 to disable I Cache
    BIC  r0, r0, #(0x1 <<  2)       @;; Clear C bit  2 to disable D Cache
    BIC  r0, r0, #0x2               @;; Clear A bit  1 to disable strict alignment fault checking
    ORR  r0, r0, #0x1               @;; Set M bit 1 to enable MMU before section initialization processing
    MCR  p15, 0, r0, c1, c0, 0      @;; Write CP15 System Control register

@===================================================================
@ copy and clear the sections
@===================================================================
    LDR  r0, =DTBL                  @;; ROM to RAM table
    LDR  r1, =BTBL                  @;; RAM zero fill table
    LDR  r12,=INITSCT               @;; Save this in register for possible long jump
    BLX  r12                        @;; Call to copy and to clear the sections

@===================================================================
@ Call global constructors
@===================================================================
    LDR  r12,=__libc_init_array     @;; Save this in register for possible long jump
    BLX  r12                        @;; Call to global objects initializer

@ =========================================================================
@ Branch to C library entry point
@ =========================================================================
    LDR  r12,=resetprg
    BX   r12                              /* Branch to C library entry point */


Literals2:
    .LTORG

r_rza2m_startup_verison :
    .align 2
    .ascii "1.02"


@==================================================================
@ Other Handler
@==================================================================
undefined_handler:
    B   undefined_handler           @;; Looping in this address

svc_handler:
    B   svc_handler                 @;; Looping in this address

prefetch_handler:
    B   prefetch_handler            @;; Looping in this address

abort_handler:
    B   abort_handler               @;; Looping in this address

reserved_handler:
    B   reserved_handler            @;; Looping in this address


    .END
