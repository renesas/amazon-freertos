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
* File Name   : r_intc_lld_rza2m_handler.c
* Description : INTC Low layer driver interrupt handler
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_intc_drv_link.h"
#include "compiler_settings.h"
#include "iodefine.h"
#include "r_intc_lld_rza2m_handler.h"

/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
* Function Name: INTC_Handler_Interrupt
* Description  : This function is the INTC interrupt handler processing called
*              : by the irq_handler. Executes the handler processing which
*              : corresponds to the INTC interrupt source ID specified by the
*              : icciar by calling the Userdef_INTC_HandlerExe function. The IRQ
*              : multiple interrupts are enabled. The processing for unsupported
*              : interrupt ID is executed by calling intc_undefined_id function.
*              : In the interrupt handler processing, when the int_sense shows
*              : "INTC_LEVEL_SENSITIVE", clear the interrupt source because it
*              : means a level sense interrupt.
* Arguments    : uint32_t icciar : Interrupt ID (value of ICCIAR register)
* Return Value : none
******************************************************************************/
void INTC_Handler_Interrupt(uint32_t icciar)
{
    /* Stacks are restored by ASM with the top level to correspond to multiple interrupts */
    uint32_t mask;
    uint32_t int_sense;
    uint16_t int_id;
    uint32_t volatile * p_addr;

    /* Obtain interrupt ID */
    int_id = (uint16_t)(icciar & 0x000003FFuL);

    if (int_id >= INTC_GIC_ID_TOTAL)    /* In case of unsupported interrupt ID */
    {
        Userdef_INTC_UndefId(int_id);
        return;
    }

    /* ==== Interrupt handler call ==== */
    __enable_irq();         /* IRQ interrupt enabled */

    /* ICDICFRn has 16 sources in the 32 bits                                  */
    /* The n can be calculated by int_id / 16                                  */
    /* The upper 1 bit out of 2 bits for the bit field width is the target bit */
    /* The target bit can be calculated by ((int_id % 16) * 2) + 1             */
    p_addr = (volatile uint32_t *)&INTC.GICD_ICFGR0;
    mask = 1u << (((int_id % 16u) * 2u) + 1u);
    if (0 == ((*(p_addr + (int_id / 16u))) & mask))  /* In the case of level sense */
    {
        int_sense = INT_SENSE_LEVEL;
    }
    else                                        /* In the case of edge trigger */
    {
        int_sense = INT_SENSE_EDGE;
    }

    Userdef_INTC_Pre_Interrupt(int_id);         /* Call pre_interrupt handler */
    if(NULL != g_intc_func_table[int_id])
    {
        g_intc_func_table[int_id](int_sense);     /* Call interrupt handler */
    }
    else
    {
        Userdef_INTC_UnregisteredID(int_id);    /* Call default handler */
    }
    Userdef_INTC_Post_Interrupt(int_id);        /* Call post_interrupt handler */

    __disable_irq();        /* IRQ interrupt disabled */
}
/******************************************************************************
 * End of function INTC_Handler_Interrupt
 ******************************************************************************/

/******************************************************************************
* Function Name: NMI_Handler_Interrupt
* Description  : This function is the NMI interrupt handler called by
*                irqfiq_handler.asm.
* Arguments    : none
* Return Value : none
******************************************************************************/
void NMI_Handler_Interrupt(void)
{
    if(NULL != g_intc_func_table[INTC_VID_NMI])
    {
        g_intc_func_table[INTC_VID_NMI](INT_SENSE_EDGE);     /* Call interrupt handler */
    }
    else
    {
        Userdef_INTC_UnregisteredID(INTC_VID_NMI);    /* Call default handler */
    }

    /* clear NMIF register */
    INTC.ICR0.BIT.NMIF = 0;
}
/******************************************************************************
 * End of function NMI_Handler_Interrupt
 ******************************************************************************/

/* End of File */
