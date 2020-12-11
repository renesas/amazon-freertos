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
@* File Name   : r_cache_l1_rza2.asm
@* Description : Declaration of the Cortex-A9 cache API function.
@*******************************************************************************/


    .section    .text
    .arm

@*******************************************************************************
@ Function Name: r_cache_l1_cache_init
@ Description  : Sub function of initialize the Cortex-A9 Cache.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_cache_init
    .global r_cache_l1_cache_init
    .type   r_cache_l1_cache_init, %function
    .func   r_cache_l1_cache_init
r_cache_l1_cache_init:

    @ Do nothing

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_cache_init
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_i_inv_all
@ Description  : Invalidate whole of the Cortex-A9 instruction cache.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_i_inv_all
    .global r_cache_l1_i_inv_all
    .type   r_cache_l1_i_inv_all, %function
    .func   r_cache_l1_i_inv_all
r_cache_l1_i_inv_all:

    MOV  r0, #0                 @ r0 data is ignored
    MCR  p15, 0, r0, c7, c5, 0  @ ICIALLU
    DSB
    ISB

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_i_inv_all
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_d_cache_operation
@ Description  : Operate whole of the Cortex-A9 data cache.
@ Arguments    : r0 - R_CACHE_L1_OP_D_INV_ALL(0)         : Invalidate all
@                   - R_CACHE_L1_OP_D_CLEAN_ALL(1)       : Clean all
@                   - R_CACHE_L1_OP_D_CLEAN_INV_ALL(2)   : Clean&Invalidate all
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_d_cache_operation
    .global r_cache_l1_d_cache_operation
    .type   r_cache_l1_d_cache_operation, %function
    .func   r_cache_l1_d_cache_operation
r_cache_l1_d_cache_operation:

    PUSH {r4-r11}

    MRC  p15, 1, r6, c0, c0, 1  @ Read CLIDR
    ANDS r3, r6, #0x07000000    @ Extract coherency level
    MOV  r3, r3, LSR #23        @ Total cache levels << 1
    BEQ  Finished               @ If 0, no need to clean

    MOV  r10, #0                @ R10 holds current cache level << 1
Loop1:
    ADD  r2, r10, r10, LSR #1   @ R2 holds cache "Set" position
    MOV  r1, r6, LSR r2         @ Bottom 3 bits are the Cache-type for this level
    AND  r1, r1, #7             @ Isolate those lower 3 bits
    CMP  r1, #2
    BLT  Skip                   @ No cache or only instruction cache at this level

    MCR  p15, 2, r10, c0, c0, 0 @ Write the Cache Size selection register (CSSELR)
    ISB                         @ ISB to sync the change to the CacheSizeID reg
    MRC  p15, 1, r1, c0, c0, 0  @ Reads current Cache Size ID register (CCSIDR)
    AND  r2, r1, #7             @ Extract the line length field
    ADD  r2, r2, #4             @ Add 4 for the line length offset (log2 16 bytes)
    LDR  r4, =0x3FF
    ANDS r4, r4, r1, LSR #3     @ R4 is the max number on the way size (right aligned)
    CLZ  r5, r4                 @ R5 is the bit position of the way size increment
    LDR  r7, =0x7FFF
    ANDS r7, r7, r1, LSR #13    @ R7 is the max number of the index size (right aligned)
Loop2:
    MOV  r9, r4                 @ R9 working copy of the max way size (right aligned)

Loop3:
    ORR  r11, r10, r9, LSL r5   @ Factor in the Way number and cache number into R11
    ORR  r11, r11, r7, LSL r2   @ Factor in the Set number
    CMP  r0, #0
    BNE  Dccsw
    MCR  p15, 0, r11, c7, c6, 2 @ Invalidate by Set/Way (DCISW)
    B    Count
Dccsw:
    CMP  r0, #1
    BNE  Dccisw
    MCR  p15, 0, r11, c7, c10, 2@ Clean by set/way (DCCSW)
    B    Count
Dccisw:
    MCR  p15, 0, r11, c7, c14, 2@ Clean and Invalidate by set/way (DCCISW)
Count:
    SUBS r9, r9, #1             @ Decrement the Way number
    BGE  Loop3
    SUBS r7, r7, #1             @ Decrement the Set number
    BGE  Loop2
Skip:
    ADD  r10, r10, #2           @ increment the cache number
    CMP  r3, r10
    BGT  Loop1

Finished:
    DSB
    POP {r4-r11}
    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_d_cache_operation
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_d_inv_mva
@ Description  : Apply invalidate operation to a cache line which is included
@                in the specified address.
@ Arguments    : r0 -
@                    Starting address of cache operation (virtual address).
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_d_inv_mva
    .global r_cache_l1_d_inv_mva
    .type   r_cache_l1_d_inv_mva, %function
    .func   r_cache_l1_d_inv_mva
r_cache_l1_d_inv_mva:

    MCR  p15, 0, r0, c7, c6, 1  @ DCIMVAC
    DMB

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_d_inv_mva
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_d_clean_mva
@ Description  : Apply clean operation to a cache line which is included
@                in the specified address.
@ Arguments    : r0 -
@                    Starting address of cache operation (virtual address).
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_d_clean_mva
    .global r_cache_l1_d_clean_mva
    .type   r_cache_l1_d_clean_mva, %function
    .func   r_cache_l1_d_clean_mva
r_cache_l1_d_clean_mva:

    MCR  p15, 0, r0, c7, c10, 1 @ DCCMVAC
    DMB

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_d_clean_mva
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_d_clean_inv_mva
@ Description  : Apply clean&invalidate operation to a cache line which is
@                included in the specified address.
@ Arguments    : r0 -
@                    Starting address of cache operation (virtual address).
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_d_clean_inv_mva
    .global r_cache_l1_d_clean_inv_mva
    .type   r_cache_l1_d_clean_inv_mva, %function
    .func   r_cache_l1_d_clean_inv_mva
r_cache_l1_d_clean_inv_mva:

    MCR  p15, 0, r0, c7, c14, 1 @ DCCIMVAC
    DMB

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_d_clean_inv_mva
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_i_enable
@ Description  : Enable the Cortex-A9 instruction cache.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_i_enable
    .global r_cache_l1_i_enable
    .type   r_cache_l1_i_enable, %function
    .func   r_cache_l1_i_enable
r_cache_l1_i_enable:

    MRC  p15, 0, r0, c1, c0, 0  @ Read System Control Register
    ORR  r0, r0, #(0x1 << 12)   @ Enable I Cache
    MCR  p15, 0, r0, c1, c0, 0  @ Write System Control Register

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_i_enable
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_i_disable
@ Description  : Disable the Cortex-A9 instruction cache.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_i_disable
    .global r_cache_l1_i_disable
    .type   r_cache_l1_i_disable, %function
    .func   r_cache_l1_i_disable
r_cache_l1_i_disable:

    MRC  p15, 0, r0, c1, c0, 0  @ Read System Control Register
    BIC  r0, r0, #(0x1 << 12)   @ Disable I Cache
    MCR  p15, 0, r0, c1, c0, 0  @ Write System Control Register

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_i_disable
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_d_enable
@ Description  : Enable the Cortex-A9 data cache.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_d_enable
    .global r_cache_l1_d_enable
    .type   r_cache_l1_d_enable, %function
    .func   r_cache_l1_d_enable
r_cache_l1_d_enable:

    MRC  p15, 0, r0, c1, c0, 0  @ Read System Control Register
    ORR  r0, r0, #(0x1 << 2)    @ Enable D Cache
    MCR  p15, 0, r0, c1, c0, 0  @ Write System Control Register

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_d_enable
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_d_disable
@ Description  : Disable the Cortex-A9 data cache.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_d_disable
    .global r_cache_l1_d_disable
    .type   r_cache_l1_d_disable, %function
    .func   r_cache_l1_d_disable
r_cache_l1_d_disable:

    MRC  p15, 0, r0, c1, c0, 0  @ Read System Control Register
    BIC  r0, r0, #(0x1 << 2)    @ Disable D Cache
    MCR  p15, 0, r0, c1, c0, 0  @ Write System Control Register

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_d_disable
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_btac_enable
@ Description  : Enable the Cortex-A9 branch prediction.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_btac_enable
    .global r_cache_l1_btac_enable
    .type   r_cache_l1_btac_enable, %function
    .func   r_cache_l1_btac_enable
r_cache_l1_btac_enable:

    MRC  p15, 0, r0, c1, c0, 0  @ Read System Control Register
    ORR  r0, r0, #(0x1 << 11)   @ Enable program flow prediction
    MCR  p15, 0, r0, c1, c0, 0  @ Write System Control Register

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_btac_enable
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_btac_disable
@ Description  : Disable the Cortex-A9 branch prediction.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_btac_disable
    .global r_cache_l1_btac_disable
    .type   r_cache_l1_btac_disable, %function
    .func   r_cache_l1_btac_disable
r_cache_l1_btac_disable:

    MRC  p15, 0, r0, c1, c0, 0  @ Read CP15 register 1
    BIC  r0, r0, #(0x1 << 11)   @ Disable program flow prediction
    MCR  p15, 0, r0, c1, c0, 0  @ Write CP15 register 1

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_btac_disable
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_btac_inv
@ Description  : Invalidate whole of the Cortex-A9 branch prediction table.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_btac_inv
    .global r_cache_l1_btac_inv
    .type   r_cache_l1_btac_inv, %function
    .func   r_cache_l1_btac_inv
r_cache_l1_btac_inv:

    MOV  r0, #0                 @ r0 data is ignored
    MCR  p15, 0, r0, c7, c5, 6  @ BPIALL
    DSB
    ISB

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_btac_inv
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_prefetch_enable
@ Description  : Enable the Cortex-A9 prefetching.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_prefetch_enable
    .global r_cache_l1_prefetch_enable
    .type   r_cache_l1_prefetch_enable, %function
    .func   r_cache_l1_prefetch_enable
r_cache_l1_prefetch_enable:

    MRC  p15, 0, r0, c1, c0, 1  @ Read Auxiliary Control Register
    ORR  r0, r0, #(0x1 << 2)    @ Enable Dside prefetch
    MCR  p15, 0, r0, c1, c0, 1  @ Write Auxiliary Control Register

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_prefetch_enable
@*******************************************************************************

@*******************************************************************************
@ Function Name: r_cache_l1_prefetch_disable
@ Description  : Disable the Cortex-A9 prefetching.
@ Arguments    : none
@ Return Value : none
@*******************************************************************************
    .section .text.r_cache_l1_prefetch_disable
    .global r_cache_l1_prefetch_disable
    .type   r_cache_l1_prefetch_disable, %function
    .func   r_cache_l1_prefetch_disable
r_cache_l1_prefetch_disable:

    MRC  p15, 0, r0, c1, c0, 1  @ Read Auxiliary Control Register
    BIC  r0, r0, #(0x1 << 2)    @ Disable Dside prefetch
    MCR  p15, 0, r0, c1, c0, 1  @ Write Auxiliary Control Register

    BX  lr

    .endfunc
@*******************************************************************************
@End of function r_cache_l1_prefetch_disable
@*******************************************************************************
