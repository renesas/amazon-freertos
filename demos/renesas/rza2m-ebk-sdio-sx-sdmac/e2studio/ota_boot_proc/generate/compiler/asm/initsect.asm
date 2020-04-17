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
@* File Name   : initsect.asm
@* Description : copy and fill the variables
@*******************************************************************************/


        .section    INIT_SECTION, "ax"
        .global     INITSCT
        .type       INITSCT, %function

        @@ This program need the stack area (44bytes)
INITSCT:
        PUSH    {r4-r12,lr}
        MOV     r4, r0          @ 1st parameter : copy table
        PUSH    {r1}
        CMP     r4, #0
        BLNE    _COPYVAR        @ copy the variables
        POP     {r4}            @ 2nd parameter : zero table
        CMP     r4, #0
        BLNE    _ZEROVAR        @ fill the variables with zero
        POP     {r4-r12,pc}     @ return from subroutine

@@@
@@@ Copy the vaiables
@@@ This program gets from the table of coping area "DTBL[]"
_COPYVAR:

_CVAR01:
        LDR     r0, [r4], #4        @ r0 = top of source block
        LDR     r1, [r4], #4        @ r1 = end of source block
        SUB     r2, r1, r0          @ r2 = bytes to copy
        LDR     r1, [r4], #4        @ r1 = top of destination block

        CMP     r0, #0              @ check terminator
        BXEQ    lr                  @ return

        MOVS    r3,r2, LSR #(3+2)   @ Number of eight word multiples
        BEQ     _CVAR03             @ Fewer than eight words to move?

        B       _CVAR02
        .align 7                    @ alignment for loop optimization
                                    @ 7 : 2^7 = 128bytes boundary
_CVAR02:
        LDM     r0!, {r5-r12}       @ Load 8 words from the source
        STM     r1!, {r5-r12}       @ and put them at the destination
        SUBS    r3, r3, #1          @ Decrement the counter
        BNE     _CVAR02             @ ... copy more

_CVAR03:
        ANDS    r2, r2, #0x1f       @ Number of bytes to copy
        BEQ     _CVAR05             @ No byte left to copy?
        B       _CVAR04
        .align 7                    @ alignment for loop optimization
                                    @ 7 : 2^7 = 128bytes boundary
_CVAR04:
        LDRB    r3, [r0], #1        @ Load a word from the source and
        STRB    r3, [r1], #1        @ store it to the destination
        SUBS    r2, r2, #1          @ Decrement the counter
        BNE     _CVAR04             @ ... copy more

_CVAR05:
        B       _CVAR01             @ copy the next block


@@@
@@@ Fill the vaiables
@@@ This program gets from the table of coping area "BTBL[]"
_ZEROVAR:

_ZVAR01:
        LDR     r1, [r4], #4        @ r1 = top of block
        LDR     r2, [r4], #4        @ r2 = end of block
        SUB     r2, r2, r1          @ r2 = number of bytes to fill zero

        CMP     r1, #0              @ check terminator
        BXEQ    lr                  @ return

        MOV     r0, #0              @ fill value

        MOVS    r3,r2, LSR #(3+2)   @ Number of eight word multiples
        BEQ     _ZVAR03             @ Fewer than eight words to clean?

        MOV     r5, #0              @
        MOV     r6, #0              @
        MOV     r7, #0              @
        MOV     r8, #0              @
        MOV     r9, #0              @
        MOV     r10, #0             @
        MOV     r11, #0             @

        B       _ZVAR02
        .align 7                    @ alignment for loop optimization
                                    @ 7 : 2^7 = 128bytes boundary
_ZVAR02:
        STM     r1!, {r0,r5-r11}    @ fill the block
        SUBS    r3, r3, #1          @ Decrement the counter
        BNE     _ZVAR02             @ ... fill more

_ZVAR03:
        ANDS    r2, r2, #0x1f       @ Number of bytes to copy
        BEQ     _ZVAR05             @ No words left to copy?

        B       _ZVAR04
        .align 7                    @ alignment for loop optimization
                                    @ 7 : 2^7 = 128bytes boundary
_ZVAR04:
        STRB    r0, [r1], #1        @ Fill the block
        SUBS    r2, r2, #1          @ Decrement the counter
        BNE     _ZVAR04             @ ... fill more

_ZVAR05:
        B       _ZVAR01             @ fill the next block


        .END
