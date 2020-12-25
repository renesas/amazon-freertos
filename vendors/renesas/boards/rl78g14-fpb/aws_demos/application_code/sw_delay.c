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
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.

**********************************************************************************************************************/
/*********************************************************************************************************************
* File Name    : sw_delay.c
* Description  : sw delay file
**********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "sw_delay.h"
//#include "r_cg_macrodriver.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define DLEAY_UINT32_MAX       (0xFFFFFFFFU)
#define DLEAY_NS_PER_SECOND    (1000000000)
#define DELAY_NS_PER_US        (1000)

void prv_software_delay_loop (uint64_t loop_cnt);

/*******************************************************************************************************************//**
 *              Delay the at least specified duration in units and return.
 * @param[in]   delay  The number of 'units' to delay.
 * @param[in]   units  The 'base' (delay_units_t) for the units specified. Valid values are:
 *              DELAY_UNITS_SECONDS, DELAY_UNITS_MILLISECONDS, DELAY_UNITS_MICROSECONDS.@n
 *              For example:@n
 *              At 1 MHz one cycle takes 1 microsecond (.000001 seconds).@n
 *              At 12 MHz one cycle takes 1/12 microsecond or 83 nanoseconds.@n
 *              Therefore one run through prv_software_delay_loop() takes:
 *              ~ (83 * DELAY_LOOP_CYCLES) or 332 ns.
 *              A delay of 2 us therefore requires 2000ns/332ns or 6 loops.
 *
 *              The 'theoretical' maximum delay that may be obtained is determined by a full 32 bit loop count and the system clock rate.
 *              @240MHz:  ((0xFFFFFFFF loops * 4 cycles /loop) / 240000000) = 71 seconds.
 *              @32MHz:  ((0xFFFFFFFF loops * 4 cycles /loop) / 32000000) = 536 seconds
 *
 *              Note that requests for very large delays will be affected by rounding in the calculations and the actual delay
 *              achieved may be slightly less. @32 MHz, for example, a request for 532 seconds will be closer to 536 seconds.
 *
 *              Note also that if the calculations result in a loop_cnt of zero, the prv_software_delay_loop() function is not called
 *              at all. In this case the requested delay is too small (nanoseconds) to be carried out by the loop itself, and the
 *              overhead associated with executing the code to just get to this point has certainly satisfied the requested delay.
 **********************************************************************************************************************/

void R_SoftwareDelay (uint32_t delay, delay_units_t units)
{
    uint32_t iclk_hz;
    uint32_t cycles_requested;
    uint32_t ns_per_cycle;
    uint32_t loops_required = 0;
    uint32_t total_us       = (delay * 1000);                        /** Convert the requested time to microseconds. */
    uint64_t ns_64bits;

    iclk_hz = 12000000U;                                        	  /** Get the system clock frequency in Hz. */
    total_us       = (delay * units);

    /* Running on the Sub-clock (32768 Hz) there are 30517 ns/cycle. This means one cycle takes 31 us. One execution
     * loop of the delay_loop takes 6 cycles which at 32768 Hz is 180 us. That does not include the overhead below prior to even getting
     * to the delay loop. Given this, at this frequency anything less then a delay request of 122 us will not even generate a single
     * pass through the delay loop.  For this reason small delays (<=~200 us) at this slow clock rate will not be possible and such a request
     * will generate a minimum delay of ~200 us.*/
    ns_per_cycle = DLEAY_NS_PER_SECOND / iclk_hz;                 /** Get the # of nanoseconds/cycle. */

    /* We want to get the time in total nanoseconds but need to be conscious of overflowing 32 bits. We also do not want to do 64 bit */
    /* division as that pulls in a division library. */
    ns_64bits = (uint64_t) total_us * (uint64_t) DELAY_NS_PER_US; // Convert to ns.

    /* Have we overflowed 32 bits? */
    if (ns_64bits <= DLEAY_UINT32_MAX)
    {
        /* No, we will not overflow. */
        cycles_requested = ((uint32_t) ns_64bits / ns_per_cycle);
        loops_required   = cycles_requested / DELAY_LOOP_CYCLES;
    }
    else
    {
        /* We did overflow. Try dividing down first. */
        total_us  = (total_us / (ns_per_cycle * DELAY_LOOP_CYCLES));
        ns_64bits = (uint64_t) total_us * (uint64_t) DELAY_NS_PER_US; // Convert to ns.

        /* Have we overflowed 32 bits? */
        if (ns_64bits <= DLEAY_UINT32_MAX)
        {
            /* No, we will not overflow. */
            loops_required = (uint32_t) ns_64bits;
        }
        else
        {
            /* We still overflowed, use the max count for cycles */
            loops_required = DLEAY_UINT32_MAX;
        }
    }

    /** Only delay if the supplied parameters constitute a delay. */
    if (loops_required > (uint32_t) 0)
    {
        prv_software_delay_loop(loops_required);
    }
}

void R_SoftwareDelayMs (uint32_t delay)
{
    uint32_t iclk_hz;
    uint32_t cycles_requested;
    uint32_t ns_per_cycle;
    uint32_t loops_required = 0;
    uint32_t total_us       = (delay * 1000);                        /** Convert the requested time to microseconds. */
    uint64_t ns_64bits;

    iclk_hz = 12000000U;                                        	  /** Get the system clock frequency in Hz. */

    /* Running on the Sub-clock (32768 Hz) there are 30517 ns/cycle. This means one cycle takes 31 us. One execution
     * loop of the delay_loop takes 6 cycles which at 32768 Hz is 180 us. That does not include the overhead below prior to even getting
     * to the delay loop. Given this, at this frequency anything less then a delay request of 122 us will not even generate a single
     * pass through the delay loop.  For this reason small delays (<=~200 us) at this slow clock rate will not be possible and such a request
     * will generate a minimum delay of ~200 us.*/
    ns_per_cycle = DLEAY_NS_PER_SECOND / iclk_hz;                 /** Get the # of nanoseconds/cycle. */

    /* We want to get the time in total nanoseconds but need to be conscious of overflowing 32 bits. We also do not want to do 64 bit */
    /* division as that pulls in a division library. */
    ns_64bits = (uint64_t) total_us * (uint64_t) DELAY_NS_PER_US; // Convert to ns.

    /* Have we overflowed 32 bits? */
    if (ns_64bits <= DLEAY_UINT32_MAX)
    {
        /* No, we will not overflow. */
        cycles_requested = ((uint32_t) ns_64bits / ns_per_cycle);
        loops_required   = cycles_requested / DELAY_LOOP_CYCLES;
    }
    else
    {
        /* We did overflow. Try dividing down first. */
        total_us  = (total_us / (ns_per_cycle * DELAY_LOOP_CYCLES));
        ns_64bits = (uint64_t) total_us * (uint64_t) DELAY_NS_PER_US; // Convert to ns.

        /* Have we overflowed 32 bits? */
        if (ns_64bits <= DLEAY_UINT32_MAX)
        {
            /* No, we will not overflow. */
            loops_required = (uint32_t) ns_64bits;
        }
        else
        {
            /* We still overflowed, use the max count for cycles */
            loops_required = DLEAY_UINT32_MAX;
        }
    }

    /** Only delay if the supplied parameters constitute a delay. */
    if (loops_required > (uint32_t) 0)
    {
        prv_software_delay_loop(loops_required);
    }
}
/** @} (end addtogroup MCU) */

/*******************************************************************************************************************//**
 *        This routine takes 42 cycles per loop.
 **********************************************************************************************************************/
void prv_software_delay_loop (uint64_t loop_cnt)
{
	while(loop_cnt--);
}
