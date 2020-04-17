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
* File Name   : linker_script.ld
* Description : Linker Script File
*******************************************************************************/

MEMORY {
    /* Debugging or without OTA update */
    ROM               (rx)  : ORIGIN = 0x20010000, LENGTH = 0x04000000
    /* OTA update */
/*
    ROM               (rx)  : ORIGIN = 0x20200300, LENGTH = 0x04000000
*/
    CACHED_HYPER_RAM  (rwx) : ORIGIN = 0x40000000, LENGTH = 0x00800000
    MMU_PAGE_TABLE    (rw)  : ORIGIN = 0x80020000, LENGTH = 0x00004000
    HARDWARE_INIT_RAM (rwx) : ORIGIN = 0x80024000, LENGTH = 0x00004000
    STACK             (rw)  : ORIGIN = 0x80028000, LENGTH = 0x00010000
    CACHED_RAM        (rwx) : ORIGIN = 0x80038000, LENGTH = 0x00148000
    UNCACHED_RAM      (rwx) : ORIGIN = 0x82180000, LENGTH = 0x00280000
}

SECTIONS
{
    LOAD_MODULE1 : {
        KEEP(* (VECTOR_TABLE))      /*asm*/
    } > ROM

    __vector_table_base = ADDR(LOAD_MODULE1);


    .data.memclk_setup : ALIGN( 0x20 ) {
        */r_spibsc_setup.o (.text .text.* .rodata .rodata.* .data .data.*)
        */r_memclk_setup.o (.text .text.* .rodata .rodata.* .data .data.*)
        */r_*_memclk_setup.o (.text .text.* .rodata .rodata.* .data .data.*)
        . = ALIGN( 0x4 );
    } > HARDWARE_INIT_RAM AT > ROM

    __data_memclk_setup_load_start  = LOADADDR(.data.memclk_setup);
    __data_memclk_setup_exec_start  = ADDR(.data.memclk_setup);
    __data_memclk_setup_load_end    = LOADADDR(.data.memclk_setup) + SIZEOF(.data.memclk_setup);

    .bss.memclk_setup (NOLOAD) : ALIGN( 0x20 ) {
        */r_spibsc_setup.o (.bss .bss.* COMMON)
        */r_memclk_setup.o (.bss .bss.* COMMON)
        */r_*_memclk_setup.o (.bss .bss.* COMMON)
        . = ALIGN( 0x4 );
    } > HARDWARE_INIT_RAM

    __bss_memclk_setup_start  = ADDR(.bss.memclk_setup);
    __bss_memclk_setup_end    = ADDR(.bss.memclk_setup) + SIZEOF(.bss.memclk_setup);

    LOAD_MODULE2 : ALIGN( 0x200 ) {
        /* This section used by "hardware setup" and must to be assigned to the Internal RAM. */
        "*/r_cpg/*.o"       (.text .text.* .rodata .rodata.* .data .data.*)
        "*/rza_io_regrw.o" (.text .text.* .rodata .rodata.* .data .data.*)
        "*/hwsetup*.o"     (.text .text.* .rodata .rodata.* .data .data.*)

        . = ALIGN( 0x4 );
    } > HARDWARE_INIT_RAM AT > ROM

    __module2_load_start  = LOADADDR(LOAD_MODULE2);
    __module2_exec_start  = ADDR(LOAD_MODULE2);
    __module2_load_end    = LOADADDR(LOAD_MODULE2) + SIZEOF(LOAD_MODULE2);


    LOAD_MODULE3 : ALIGN( 0x20 ) {
        /* This section used by "hardware setup" and must to be assigned to the Internal RAM. */
        __module3_exec_start = .;
        "*/r_cpg/*.o"       (.bss* COMMON*)
        "*/rza_io_regrw.o" (.bss .bss.* COMMON)
        . = ALIGN( 0x4 );
        __module3_exec_end = .;
    } > HARDWARE_INIT_RAM AT > ROM


    LOAD_MODULE4 : ALIGN( 0x4 ) {
        KEEP(* (RESET_HANDLER))     /*asm*/
        . = ALIGN( 0x80 );
        * (INIT_SECTION)            /*asm*/
        "*/sections.o"       (.rodata .rodata.*)
    } > ROM

    /*
        The vector mirror table and the irq handler must include in the .data section.
        Because the addresses on ROM are not calculated if the two put in each sections.
    */
    .data : ALIGN( 0x20 ) {
        __vect_exec_start = .;
        __vect_load_start = LOADADDR(.data) + ( . - ADDR(.data) );

        __vector_mirror_table_base = .;
        KEEP(* (VECTOR_MIRROR_TABLE))   /*asm*/

        . = ALIGN( 0x4 );
        __vect_load_end =  LOADADDR(.data) + ( . - ADDR(.data) );

        /********************************************************************/

        . = ALIGN(0x00000100);
        __fastcode_exec_start = .;
        __fastcode_load_start = LOADADDR(.data) + ( . - ADDR(.data) );

        "*/r_intc_*.o"  (.text .text.* .rodata .rodata.*)

        KEEP(* (IRQ_FIQ_HANDLER))       /*asm*/


        . = ALIGN( 0x4 );
        __fastcode_load_end = LOADADDR(.data) + ( . - ADDR(.data) );

        /********************************************************************/

        __data_exec_start = .;
        __data_load_start = LOADADDR(.data) + ( . - ADDR(.data) );
        /*tmp*/
        *(RAM_CODE)
        *(RAM_CONST)
        *(.text.r_mmu_tlbiall)
        /*tmpend*/
        . = ALIGN( 0x4 );
        __data_load_end = LOADADDR(.data) + ( . - ADDR(.data) );

    } > CACHED_RAM AT > ROM

    .bss (NOLOAD) : ALIGN( 0x20 ) {
        __bss_start = .;
        . = ALIGN( 0x4 );
        __bss_end = .;

    } > CACHED_RAM

    VRAM (NOLOAD) : ALIGN( 0x40 ) {
        * (Video_RAM)
        * (Graphics_RAM)
        . = ALIGN( 0x80 );
        * (Capture_RAM)

    } > UNCACHED_RAM

    .uncached_RAM (NOLOAD) : ALIGN( 0x20 ) {
        __uncached_RAM_start = .;

        "*/r_cache_*.o" (.bss .bss.* COMMON)
        * (UNCACHED_BSS)
        . = ALIGN( 0x4 );
        PROVIDE(__sectop__ETHERNET_BUFFERS = .);
        *(_ETHERNET_BUFFERS)
        __uncached_RAM_end = .;
    } > UNCACHED_RAM

    .uncached_RAM2 : ALIGN( 0x20 ) {
        __uncached_RAM2_exec_start = .;
        __uncached_RAM2_load_start = LOADADDR(.uncached_RAM2) + ( . - ADDR(.uncached_RAM2) );

        "*/r_cache_*.o" (.text .text.* .rodata .rodata.* .data .data.*)
        * (UNCACHED_DATA)

        . = ALIGN( 0x4 );
        __uncached_RAM2_load_end = LOADADDR(.uncached_RAM2) + ( . - ADDR(.uncached_RAM2) );
    } > UNCACHED_RAM AT > ROM

    .mmu_page_table (NOLOAD) : ALIGN( 0x4000 ) {
        __mmu_page_table_base = .;
        . += 0x00004000;
        __mmu_page_table_end  = .;
    } > MMU_PAGE_TABLE


    .stack (NOLOAD) : ALIGN( 0x10 ) {
        __sys_stack_start = .;
        . += 0x00008000;
        . = ALIGN( 0x10 );
        __sys_stack_end = .;

        __irq_stack_start = .;
        . += 0x00002000;
        . = ALIGN( 0x10 );
        __irq_stack_end = .;

        __fiq_stack_start = .;
        . += 0x00002000;
        . = ALIGN( 0x10 );
        __fiq_stack_end = .;

        __svc_stack_start = .;
        . += 0x00002000;
        . = ALIGN( 0x10 );
        __svc_stack_end = .;

        __abt_stack_start = .;
        . += 0x00002000;
        . = ALIGN( 0x10 );
        __abt_stack_end = .;

    } > STACK

    .text2 : ALIGN( 0x4 ) {
        * (.text .text.*)
        * (.rodata .rodata.*)

        . = ALIGN( 0x4 );

        __preinit_array_start = .;
        KEEP(* (.preinit_array*))
        __preinit_array_end = .;
        __init_array_start = .;
        KEEP(* (.init_array*))
        __init_array_end = .;
        __fini_array_start = .;
        KEEP(* (.fini_array*))
        __fini_array_end = .;
    } > ROM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ROM

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > ROM

    .data2 : ALIGN( 0x20 ) {
        __data2_exec_start = .;
        __data2_load_start = LOADADDR(.data2) + ( . - ADDR(.data2) );

        * (.data .data.*)

        . = ALIGN( 0x4 );
        __data2_load_end = LOADADDR(.data2) + ( . - ADDR(.data2) );
    } > CACHED_RAM AT > ROM

    .bss2 (NOLOAD) : ALIGN( 0x20 ) {
        __bss2_start = .;

        * (.bss .bss.*)
        * (COMMON)

        . = ALIGN( 0x4 );
        __bss2_end = .;
    } > CACHED_RAM

    .heap (NOLOAD) : ALIGN( 0x10 ) {
        end = .;
        __heap_start = .;
        . = . + 0x00008000;
        __heap_end = .;
        /* heap end */

    } > CACHED_RAM

    .freertos_heap (NOLOAD) : ALIGN( 0x10 ) {
        * (HYPER_RAM)
        PROVIDE(__freertos_heap_start = .);
        *(.freertos_heap)
        PROVIDE(__freertos_heap_end = .);
        /* heap end */

    } > CACHED_HYPER_RAM
}
/*EOF*/
