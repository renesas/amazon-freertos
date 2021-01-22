/*******************************************************************************
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
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name : sections.c
* Description :
******************************************************************************/
#include "r_typedefs.h"

/* List of the section of variables with initial value */
typedef struct {
    void    *rom_s;     /* Top address on ROM    */
    void    *rom_e;     /* Bottom address on ROM */
    void    *ram_s;     /* Top address on RAM    */
} dtbl_t;

/* List of the section of variables with zero clear */
typedef struct {
    void    *b_s;       /* Top address    */
    void    *b_e;       /* Bottom address */
} btbl_t;

extern  uint16_t   __data_memclk_setup_load_start;
extern  uint16_t   __data_memclk_setup_load_end;
extern  uint16_t   __data_memclk_setup_exec_start;

extern  uint16_t   __bss_memclk_setup_start;
extern  uint16_t   __bss_memclk_setup_end;

extern  uint16_t   __vect_load_start;
extern  uint16_t   __vect_load_end;
extern  uint16_t   __vect_exec_start;

extern  uint16_t   __fastcode_load_start;
extern  uint16_t   __fastcode_load_end;
extern  uint16_t   __fastcode_exec_start;

extern  uint16_t   __data_load_start;
extern  uint16_t   __data_load_end;
extern  uint16_t   __data_exec_start;

extern  uint16_t   __data2_load_start;
extern  uint16_t   __data2_load_end;
extern  uint16_t   __data2_exec_start;

extern  uint16_t   __uncached_RAM2_load_start;
extern  uint16_t   __uncached_RAM2_load_end;
extern  uint16_t   __uncached_RAM2_exec_start;

extern  uint16_t   __bss_start;
extern  uint16_t   __bss_end;

extern  uint16_t   __bss2_start;
extern  uint16_t   __bss2_end;

extern  uint16_t   __uncached_RAM_start;
extern  uint16_t   __uncached_RAM_end;

extern  uint16_t   __module2_load_start;
extern  uint16_t   __module2_load_end;
extern  uint16_t   __module2_exec_start;

extern  uint16_t   __module3_exec_start;
extern  uint16_t   __module3_exec_end;

const dtbl_t    DTBL[] =
{
/*  {  rom start address,     rom end address,     ram start address    },*/
    { &__vect_load_start, &__vect_load_end, &__vect_exec_start },
    { &__fastcode_load_start, &__fastcode_load_end, &__fastcode_exec_start },
    { &__data_load_start, &__data_load_end, &__data_exec_start },
    { &__data2_load_start, &__data2_load_end, &__data2_exec_start },
    { &__uncached_RAM2_load_start, &__uncached_RAM2_load_end, &__uncached_RAM2_exec_start },
    {   0,  0,  0   },  /* terminator */
};

const btbl_t    BTBL[] =
{
/*  {  bss start address,    bss end address     },*/
    { &__bss_start, &__bss_end },
    { &__bss2_start, &__bss2_end },
    { &__uncached_RAM_start, &__uncached_RAM_end },
    {   0,  0   },  /* terminator */
};

const dtbl_t    DTBL_F[] =
{
/*  {  rom start address,     rom end address,     ram start address    },*/
    { &__module2_load_start, &__module2_load_end, &__module2_exec_start },
    {   0,  0,  0   },  /* terminator */
};

const btbl_t    BTBL_F[] =
{
/*  {  bss start address,    bss end address     },*/
    { &__module3_exec_start, &__module3_exec_end },
    {   0,  0   },  /* terminator */
};

const dtbl_t    DTBL_F2[] =
{
/*  {  rom start address,     rom end address,     ram start address    },*/
    { &__data_memclk_setup_load_start, &__data_memclk_setup_load_end, &__data_memclk_setup_exec_start },
    {   0,  0,  0   },  /* terminator */
};

const btbl_t    BTBL_F2[] =
{
/*  {  bss start address,    bss end address     },*/
    { &__bss_memclk_setup_start, &__bss_memclk_setup_end },
    {   0,  0   },  /* terminator */
};

/* End of File */
