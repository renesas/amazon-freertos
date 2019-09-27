/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "Task.h"

/* Renesas driver includes. */
#include <stdint.h>
#include "iodefine.h"

#include "r_intc_lld_rza2m.h"

#define runtimeCLOCK_SCALE_SHIFT    ( 9UL )
#define runtimeOVERFLOW_BIT         ( 1UL << ( 32UL - runtimeCLOCK_SCALE_SHIFT ) )

/* --- P1 clock (Uint:kHz) for RZ/A2 --- */
#define P1_CLOCK_FREQUENCY_kHz  (66 * 1000)  /* Assume 66MHz */

/* ---- Timer cycle specification(Argument cycle)(Unit : ms) maximum value ---- */
    /* The maximum value is H'FFFF_FFFF because CMP is the register which has 32 bits */
    /* 65,075msec * 66MHz = 4,294,950,000 (H'FFFF_BC70)   : No Error                  */
    /* 65,076msec * 66MHz = 4,295,016,000 (H'1_0000_BE40) : Error                     */
    /* Timer cycle specification > Error when the cycle is 65,075ms                   */
#define MAX_CYCLE_msec          (0xFFFFFFFFuL / P1_CLOCK_FREQUENCY_kHz)

#define OSTM_INT_DISABLE    (0xFE)  /* OSTM interrupt disable */
#define OSTM_INT_ENABLE     (0x01)  /* OSTM interrupt enable */
#define OSTM_MODE_INTERVAL  (0xFD)  /* OSTM operating mode : Interval timer mode       */
#define OSTM_MODE_COMPARE   (0x02)  /* OSTM operating mode : Free running compare mode */

/* To make casting to the ISR prototype expected by the Renesas GIC drivers. */
typedef void (*ISR_FUNCTION)( uint32_t );

/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.  This file contains a function
 * that is suitable for use on the Renesas RZ MPU.
 */
void vConfigureTickInterrupt( void )
{
    /* Stop the counter. */
    OSTM0.OSTMnTT.BYTE = 1;

    /* Work in interval mode. */
    OSTM0.OSTMnCTL.BYTE |= OSTM_MODE_INTERVAL;

    /* Use interrupts after counting starts. */
    OSTM0.OSTMnCTL.BYTE |= OSTM_INT_ENABLE;

    /* Start value for down counter.
     * (configPERIPHERAL_CLOCK0_HZ / configTICK_RATE_HZ) + 14 ?? */
    OSTM0.OSTMnCMP.LONG =  (uint32_t)(( P1_CLOCK_FREQUENCY_kHz )  - 1 ) ;

    /* Configure the interrupt controller. */
    R_INTC_RegistIntFunc( INTC_ID_OSTM_OSTMI0, ( ISR_FUNCTION ) FreeRTOS_Tick_Handler );

    /* Tick must be assigned the lowest interrupt priority. */
    R_INTC_SetPriority( INTC_ID_OSTM_OSTMI0, portLOWEST_USABLE_INTERRUPT_PRIORITY );

    INTC.GICC_BPR.BIT.Binarypoint = 0;
    R_INTC_Enable( INTC_ID_OSTM_OSTMI0 );

    OSTM0.OSTMnTS.BYTE = 0x01;  /* Start counting */
}
/*-----------------------------------------------------------*/

/*
 * Crude implementation of a run time counter used to measure how much time
 * each task spends in the Running state.
 */
unsigned long ulGetRunTimeCounterValue( void )
{
    static unsigned long last_counter_value = 0UL;
    static unsigned long overflows = 0;
    unsigned long value_now;

    value_now = OSTM1.OSTMnCNT.LONG;

    /* Has the value overflowed since it was last read. */
    if( value_now < last_counter_value )
    {
        overflows++;
    }
    last_counter_value = value_now;

    /* There is no prescale on the counter, so simulate in software. */
    value_now >>= runtimeCLOCK_SCALE_SHIFT;
    value_now += ( runtimeOVERFLOW_BIT * overflows );

    return value_now;
}
/*-----------------------------------------------------------*/

void vInitialiseRunTimeStats( void )
{
    volatile uint8_t dummy_buf;

    /* OSTM1 is used as the run time stats counter. */
    CPG.STBCR3.BYTE = 0x9fu;      /* [1], OSTM0, OSTM1, OSTM3, MTU3, CAN-FD, ADC, GPT */
    dummy_buf = CPG.STBCR3.BYTE;  /* (Dummy read)                                         */

    /* Stop the counter. */
    OSTM1.OSTMnTT.BYTE = 1;

    /* Work in compare mode mode. */
    OSTM1.OSTMnCTL.BYTE |= OSTM_MODE_COMPARE;

    /* Don't use interrupts. */
    OSTM1.OSTMnCTL.BYTE &= OSTM_INT_DISABLE;

    /* Compare is just set to 0. */
    OSTM1.OSTMnCMP.LONG = 0;

    OSTM1.OSTMnTS.BYTE = 0x01;  /* Start counting */
}
