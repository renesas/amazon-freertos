/******************************************************************************
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
 * Renesas reserves the right, without notice, to make changes to this
 * software and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * File Name    : r_rza2_riic_lld_int.c
 * Device(s)    : RZ/A2M
 * Tool-Chain   : GCC ARM Embedded 6.3.1.20170620
 * OS           : None
 * H/W Platform : RZ/A2M Evaluation Board
 * Description  : RZ/A2M RIIC Driver - Interrupt control of RIIC module
 * Operation    :
 * Limitations  :
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 *****************************************************************************/

/* System Includes */
#include "r_typedefs.h"
#include "iodefine.h"
#include "iobitmask.h"
/* Low level register read/write header */
#include "rza_io_regrw.h"

/* Driver Includes */
/* Device driver header */
#include "driver.h"
/* High level API header for callback function */
#include "r_riic_hld_api.h"
/* High level Driver header for callback funtion */
#include "r_riic_drv_api.h"
/* Low level Driver Header */
#include "r_rza2m_riic_lld_api.h"
/* Low level Driver Configuration Header */
#include "r_rza2m_riic_lld_cfg.h"
/* Low level Driver Header */
#include "r_rza2m_riic_lld_prv.h"
/* Interrupt Driver API Header */
#include "r_intc_drv_api.h"

/******************************************************************************
 Private global variables and functions
 *****************************************************************************/

/******************************************************************************
 Macro definitions
 *****************************************************************************/

/******************************************************************************
 Typedefs
 *****************************************************************************/

/******************************************************************************
 Private function definitions
 *****************************************************************************/
static void intriictei_proc_handler (int_t channel);
static void intriic0tei_handler (uint32_t int_sense);
static void intriic1tei_handler (uint32_t int_sense);
static void intriic2tei_handler (uint32_t int_sense);
static void intriic3tei_handler (uint32_t int_sense);

static void intriicri_proc_handler (int_t channel);
static void intriic0ri_handler (uint32_t int_sense);
static void intriic1ri_handler (uint32_t int_sense);
static void intriic2ri_handler (uint32_t int_sense);
static void intriic3ri_handler (uint32_t int_sense);

static void intriicti_proc_handler (int_t channel);
static void intriic0ti_handler (uint32_t int_sense);
static void intriic1ti_handler (uint32_t int_sense);
static void intriic2ti_handler (uint32_t int_sense);
static void intriic3ti_handler (uint32_t int_sense);

static void intriicsti_proc_handler (int_t channel);
static void intriic0sti_handler (uint32_t int_sense);
static void intriic1sti_handler (uint32_t int_sense);
static void intriic2sti_handler (uint32_t int_sense);
static void intriic3sti_handler (uint32_t int_sense);

static void intriicspi_proc_handler (int_t channel);
static void intriic0spi_handler (uint32_t int_sense);
static void intriic1spi_handler (uint32_t int_sense);
static void intriic2spi_handler (uint32_t int_sense);
static void intriic3spi_handler (uint32_t int_sense);

static void intriicnaki_proc_handler (int_t channel);
static void intriic0naki_handler (uint32_t int_sense);
static void intriic1naki_handler (uint32_t int_sense);
static void intriic2naki_handler (uint32_t int_sense);
static void intriic3naki_handler (uint32_t int_sense);

static void intriicali_proc_handler (int_t channel);
static void intriic0ali_handler (uint32_t int_sense);
static void intriic1ali_handler (uint32_t int_sense);
static void intriic2ali_handler (uint32_t int_sense);
static void intriic3ali_handler (uint32_t int_sense);

static void intriictmoi_proc_handler (int_t channel);
static void intriic0tmoi_handler (uint32_t int_sense);
static void intriic1tmoi_handler (uint32_t int_sense);
static void intriic2tmoi_handler (uint32_t int_sense);
static void intriic3tmoi_handler (uint32_t int_sense);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static volatile struct st_riic *gsp_riic[RIIC_CFG_LLD_NUM_CHANNELS] =
{
    /* RIIC Channels */
    &RIIC30, &RIIC31, &RIIC32, &RIIC33 
};

/* RIIC interrupt info */

/* interrupt information table define */
static const st_riic_int_param_t gs_riic_int_info[RIIC_CFG_LLD_NUM_CHANNELS][RIIC_INT_MAX] =
{
    { 
        /* ch0 */
        { &intriic0tei_handler, INTC_ID_IIC_INTRIICTEI0, INT_SENSE_LEVEL }, /* TEI */
        { &intriic0ri_handler, INTC_ID_IIC_INTRIICRI0, INT_SENSE_EDGE }, /* RI */
        { &intriic0ti_handler, INTC_ID_IIC_INTRIICTI0, INT_SENSE_EDGE }, /* TI */
        { &intriic0spi_handler, INTC_ID_IIC_INTRIICSPI0, INT_SENSE_LEVEL }, /* SPI */
        { &intriic0sti_handler, INTC_ID_IIC_INTRIICSTI0, INT_SENSE_LEVEL }, /* STI */
        { &intriic0naki_handler, INTC_ID_IIC_INTRIICNAKI0, INT_SENSE_LEVEL }, /* NAKI */
        { &intriic0ali_handler, INTC_ID_IIC_INTRIICALI0, INT_SENSE_LEVEL }, /* ALI */
        { &intriic0tmoi_handler, INTC_ID_IIC_INTRIICTMOI0, INT_SENSE_LEVEL } /* TMOI */
    },
    { 
        /* ch1 */
        { &intriic1tei_handler, INTC_ID_IIC_INTRIICTEI1, INT_SENSE_LEVEL }, /* TEI */
        { &intriic1ri_handler, INTC_ID_IIC_INTRIICRI1, INT_SENSE_EDGE }, /* RI */
        { &intriic1ti_handler, INTC_ID_IIC_INTRIICTI1, INT_SENSE_EDGE }, /* TI */
        { &intriic1spi_handler, INTC_ID_IIC_INTRIICSPI1, INT_SENSE_LEVEL }, /* SPI */
        { &intriic1sti_handler, INTC_ID_IIC_INTRIICSTI1, INT_SENSE_LEVEL }, /* STI */
        { &intriic1naki_handler, INTC_ID_IIC_INTRIICNAKI1, INT_SENSE_LEVEL }, /* NAKI */
        { &intriic1ali_handler, INTC_ID_IIC_INTRIICALI1, INT_SENSE_LEVEL }, /* ALI */
        { &intriic1tmoi_handler, INTC_ID_IIC_INTRIICTMOI1, INT_SENSE_LEVEL } /* TMOI */
    },
    { 
        /* ch2 */
        { &intriic2tei_handler, INTC_ID_IIC_INTRIICTEI2, INT_SENSE_LEVEL }, /* TEI */
        { &intriic2ri_handler, INTC_ID_IIC_INTRIICRI2, INT_SENSE_EDGE }, /* RI */
        { &intriic2ti_handler, INTC_ID_IIC_INTRIICTI2, INT_SENSE_EDGE }, /* TI */
        { &intriic2spi_handler, INTC_ID_IIC_INTRIICSPI2, INT_SENSE_LEVEL }, /* SPI */
        { &intriic2sti_handler, INTC_ID_IIC_INTRIICSTI2, INT_SENSE_LEVEL }, /* STI */
        { &intriic2naki_handler, INTC_ID_IIC_INTRIICNAKI2, INT_SENSE_LEVEL }, /* NAKI */
        { &intriic2ali_handler, INTC_ID_IIC_INTRIICALI2, INT_SENSE_LEVEL }, /* ALI */
        { &intriic2tmoi_handler, INTC_ID_IIC_INTRIICTMOI2, INT_SENSE_LEVEL } /* TMOI */
    },
    { 
        /* ch3 */
        { &intriic3tei_handler, INTC_ID_IIC_INTRIICTEI3, INT_SENSE_LEVEL }, /* TEI */
        { &intriic3ri_handler, INTC_ID_IIC_INTRIICRI3, INT_SENSE_EDGE }, /* RI */
        { &intriic3ti_handler, INTC_ID_IIC_INTRIICTI3, INT_SENSE_EDGE }, /* TI */
        { &intriic3spi_handler, INTC_ID_IIC_INTRIICSPI3, INT_SENSE_LEVEL }, /* SPI */
        { &intriic3sti_handler, INTC_ID_IIC_INTRIICSTI3, INT_SENSE_LEVEL }, /* STI */
        { &intriic3naki_handler, INTC_ID_IIC_INTRIICNAKI3, INT_SENSE_LEVEL }, /* NAKI */
        { &intriic3ali_handler, INTC_ID_IIC_INTRIICALI3, INT_SENSE_LEVEL }, /* ALI */
        { &intriic3tmoi_handler, INTC_ID_IIC_INTRIICTMOI3, INT_SENSE_LEVEL } /* TMOI */
    } 
};

/* Prototypes */

/******************************************************************************
 Private function definitions
 *****************************************************************************/

/******************************************************************************
* Function Name: interrupt_init
* Description  : Initialize function of interrupt.
* Arguments    : channel - The RIIC channel on initialising interrupt.
                 *p_cfg - The RIIC channel configration information.
* Return Value : None.
******************************************************************************/
void interrupt_init (const int_t channel, const st_riic_config_t *p_cfg)
{
    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_TEI].int_num,
            gs_riic_int_info[channel][RIIC_INT_TEI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_TEI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_TEI].int_num, p_cfg->tei_priority);

    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_RI].int_num,
            gs_riic_int_info[channel][RIIC_INT_RI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_RI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_RI].int_num, p_cfg->ri_priority);

    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_TI].int_num,
            gs_riic_int_info[channel][RIIC_INT_TI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_TI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_TI].int_num, p_cfg->ti_priority);

    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_STI].int_num,
            gs_riic_int_info[channel][RIIC_INT_STI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_STI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_STI].int_num, p_cfg->sti_priority);

    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_SPI].int_num,
            gs_riic_int_info[channel][RIIC_INT_SPI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_SPI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_SPI].int_num, p_cfg->spi_priority);

    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_NAKI].int_num,
            gs_riic_int_info[channel][RIIC_INT_NAKI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_NAKI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_NAKI].int_num, p_cfg->naki_priority);

    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_ALI].int_num,
            gs_riic_int_info[channel][RIIC_INT_ALI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_ALI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_ALI].int_num, p_cfg->ali_priority);

    R_INTC_RegistIntFunc(gs_riic_int_info[channel][RIIC_INT_TMOI].int_num,
            gs_riic_int_info[channel][RIIC_INT_TMOI].int_handler);
    R_INTC_Enable(gs_riic_int_info[channel][RIIC_INT_TMOI].int_num);
    R_INTC_SetPriority(gs_riic_int_info[channel][RIIC_INT_TMOI].int_num, p_cfg->tmoi_priority);
}
/******************************************************************************
 * End of Function interrupt_init
 *****************************************************************************/

/******************************************************************************
* Function Name: interrupt_uninit
* Description  : Uninitialize function of interrupt.
* Arguments    : channel -The RIIC channel on initialising interrupt.
* Return Value : None.
******************************************************************************/
void interrupt_uninit (const int_t channel)
{
    e_riic_int_type_t int_type;

    for (int_type = RIIC_INT_TEI; int_type < RIIC_INT_MAX; int_type++)
    {
        R_INTC_Disable(gs_riic_int_info[channel][int_type].int_num);
        
        /* Casting the pointer to NULL is valid because the interrupt handler
           function is set to NULL on driver termination process */
        R_INTC_RegistIntFunc(gs_riic_int_info[channel][int_type].int_num, NULL);
    }
}
/******************************************************************************
 * End of Function interrupt_uninit
 *****************************************************************************/

/******************************************************************************
* Function Name: intriictei_proc_handler
* Description  : ISR process function of TEI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
******************************************************************************/
static void intriictei_proc_handler (int_t channel)
{
    uint32_t dummy;

    r_riic_hld_set_tx_end(channel);

    /* clear interrupt source */
    dummy = RZA_IO_RegRead_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_TEND_SHIFT,
            RIIC_ICSR2_TEND);

    /* cast to volatile uint32_t * */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, 
                        0, RIIC_ICSR2_TEND_SHIFT, RIIC_ICSR2_TEND);

}
/******************************************************************************
 * End of Function intriictei_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0tei_handler
* Description  : The function accepts INTRIIC0TEI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0tei_handler (uint32_t int_sense)
{

    intriictei_proc_handler(RIIC_CH0);

}
/******************************************************************************
 * End of Function intriic0tei_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1tei_handler
* Description  : The function accepts INTRIIC1TEI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1tei_handler (uint32_t int_sense)
{
    intriictei_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1tei_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2tei_handler
* Description  : The function accepts INTRIIC2TEI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2tei_handler (uint32_t int_sense)
{
    intriictei_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2tei_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3tei_handler
* Description  : The function accepts INTRIIC3TEI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3tei_handler (uint32_t int_sense)
{
    intriictei_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3tei_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriicri_proc_handler
* Description  : ISR process function of RI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
******************************************************************************/
static void intriicri_proc_handler (int_t channel)
{
    /* cast channel to int_t */
    r_riic_hld_set_rx_full(channel);
}
/******************************************************************************
 * End of Function intriicri_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0ri_handler
* Description  : The function accepts INTRIIC0RI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0ri_handler (uint32_t int_sense)
{
    intriicri_proc_handler(RIIC_CH0);

}
/******************************************************************************
 * End of Function intriic0ri_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1ri_handler
* Description  : The function accepts INTRIIC1RI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1ri_handler (uint32_t int_sense)
{
    intriicri_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1ri_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2ri_handler
* Description  : The function accepts INTRIIC2RI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2ri_handler (uint32_t int_sense)
{
    intriicri_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2ri_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3ri_handler
* Description  : The function accepts INTRIIC3RI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3ri_handler (uint32_t int_sense)
{
    intriicri_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3ri_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriicti_proc_handler
* Description  : ISR process function of TI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
******************************************************************************/
static void intriicti_proc_handler (int_t channel)
{
    /* cast to int_t */
    r_riic_hld_set_tx_empty(channel);

}
/******************************************************************************
 * End of Function intriicti_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0ti_handler
* Description  : The function accepts INTRIIC0TI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0ti_handler (uint32_t int_sense)
{
    intriicti_proc_handler(RIIC_CH0);

}
/******************************************************************************
 * End of Function intriic0ti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1ti_handler
* Description  : The function accepts INTRIIC1TI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1ti_handler (uint32_t int_sense)
{
    intriicti_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1ti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2ti_handler
* Description  : The function accepts INTRIIC2TI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2ti_handler (uint32_t int_sense)
{
    intriicti_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2ti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3ti_handler
* Description  : The function accepts INTRIIC3TI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3ti_handler (uint32_t int_sense)
{
    intriicti_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3ti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriicsti_proc_handler
* Description  : ISR process function of STI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
******************************************************************************/
static void intriicsti_proc_handler (int_t channel)
{
    uint32_t dummy;

    /* cast to int_t */
    r_riic_hld_set_start_asserted(channel);

    /* clear interrupt source */
    dummy = RZA_IO_RegRead_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_START_SHIFT,
            RIIC_ICSR2_START);

    /* cast to volatile uint32_t * */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, 0, RIIC_ICSR2_START_SHIFT,
            RIIC_ICSR2_START);

}
/******************************************************************************
 * End of Function intriicsti_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0sti_handler
* Description  : The function accepts INTRIIC0STI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0sti_handler (uint32_t int_sense)
{
    intriicsti_proc_handler(RIIC_CH0);

}
/*****************************************************************************
 * End of Function intriic0sti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1sti_handler
* Description  : The function accepts INTRIIC1STI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1sti_handler (uint32_t int_sense)
{
    intriicsti_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1sti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2sti_handler
* Description  : The function accepts INTRIIC2STI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2sti_handler (uint32_t int_sense)
{
    intriicsti_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2sti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3sti_handler
* Description  : The function accepts INTRIIC3STI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3sti_handler (uint32_t int_sense)
{
    intriicsti_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3sti_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriicspi_proc_handler
* Description  : ISR process function of SPI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
******************************************************************************/
static void intriicspi_proc_handler (int_t channel)
{
    uint32_t dummy;

    /* cast to int_t */
    r_riic_hld_set_stop_asserted(channel);

    /* clear interrupt source */
    dummy = RZA_IO_RegRead_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_STOP_SHIFT,
            RIIC_ICSR2_STOP);

    /* cast to volatile uint32_t * */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, 
                        0, RIIC_ICSR2_STOP_SHIFT, RIIC_ICSR2_STOP);

}
/******************************************************************************
 * End of Function intriicspi_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0spi_handler
* Description  : The function accepts INTRIIC0SPI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0spi_handler (uint32_t int_sense)
{
    intriicspi_proc_handler(RIIC_CH0);
}
/******************************************************************************
 * End of Function intriic0spi_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1spi_handler
* Description  : The function accepts INTRIIC1SPI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1spi_handler (uint32_t int_sense)
{
    intriicspi_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1spi_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2spi_handler
* Description  : The function accepts INTRIIC2SPI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2spi_handler (uint32_t int_sense)
{
    intriicspi_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2spi_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3spi_handler
* Description  : The function accepts INTRIIC3SPI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3spi_handler (uint32_t int_sense)
{
    intriicspi_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3spi_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriicali_proc_handler
* Description  : ISR process function of ALI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
*******************************************************************************/
static void intriicali_proc_handler (int_t channel)
{
    uint32_t dummy;

    /* cast to int_t */
    r_riic_hld_set_arbitrate_assert(channel);

    /* clear interrupt source */
    dummy = RZA_IO_RegRead_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, 
                                RIIC_ICSR2_AL_SHIFT, RIIC_ICSR2_AL);

    /* cast to volatile uint32_t * */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, 0, RIIC_ICSR2_AL_SHIFT, RIIC_ICSR2_AL);

}
/******************************************************************************
 * End of Function intriicali_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0ali_handler
* Description  : The function accepts INTRIIC0ALI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0ali_handler (uint32_t int_sense)
{
    intriicali_proc_handler(RIIC_CH0);
}
/******************************************************************************
 * End of Function intriic0ali_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1ali_handler
* Description  : The function accepts INTRIIC1ALI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1ali_handler (uint32_t int_sense)
{
    intriicali_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1ali_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2ali_handler
* Description  : The function accepts INTRIIC2ALI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2ali_handler (uint32_t int_sense)
{
    intriicali_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2ali_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3ali_handler
* Description  : The function accepts INTRIIC3ALI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3ali_handler (uint32_t int_sense)
{
    intriicali_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3ali_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriicnaki_proc_handler
* Description  : ISR process function of NAKI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
******************************************************************************/
static void intriicnaki_proc_handler (int_t channel)
{
    uint32_t dummy;

    /* cast to int_t*/
    r_riic_hld_set_nack_asserted(channel);

    /* clear interrupt source */
    dummy = RZA_IO_RegRead_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_NACKF_SHIFT,
            RIIC_ICSR2_NACKF);

    /* cast to volatile uint32_t * */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, 0, RIIC_ICSR2_NACKF_SHIFT,
            RIIC_ICSR2_NACKF);

}
/******************************************************************************
 * End of Function intriicnaki_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0naki_handler
* Description  : The function accepts INTRIIC0NAKI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0naki_handler (uint32_t int_sense)
{
    intriicnaki_proc_handler(RIIC_CH0);
}
/******************************************************************************
 * End of Function intriic0naki_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1naki_handler
* Description  : The function accepts INTRIIC1NAKI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1naki_handler (uint32_t int_sense)
{
    intriicnaki_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1naki_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2naki_handler
* Description  : The function accepts INTRIIC2NAKI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2naki_handler (uint32_t int_sense)
{
    intriicnaki_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2naki_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3naki_handler
* Description  : The function accepts INTRIIC3NAKI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3naki_handler (uint32_t int_sense)
{
    intriicnaki_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3naki_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriictmoi_proc_handler
* Description  : ISR process function of TMOI interrupt.
* Arguments    : channel - The channel number where interrupt occurred.
* Return Value : None.
******************************************************************************/
static void intriictmoi_proc_handler (int_t channel)
{
    uint32_t dummy;

    /* cast to int_t */
    r_riic_hld_set_timeout_asserted(channel);

    /* clear interrupt source */
    dummy = RZA_IO_RegRead_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_TMOF_SHIFT,
            RIIC_ICSR2_TMOF);

    /* cast to volatile uint32_t * */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, 
                        0, RIIC_ICSR2_TMOF_SHIFT, RIIC_ICSR2_TMOF);

}
/******************************************************************************
 * End of Function intriictmoi_proc_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic0tmoi_handler
* Description  : The function accepts INTRIIC0TMOI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic0tmoi_handler (uint32_t int_sense)
{
    intriictmoi_proc_handler(RIIC_CH0);

}
/******************************************************************************
 * End of Function intriic0tmoi_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic1tmoi_handler
* Description  : The function accepts INTRIIC1TMOI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic1tmoi_handler (uint32_t int_sense)
{
    intriictmoi_proc_handler(RIIC_CH1);

}
/******************************************************************************
 * End of Function intriic1tmoi_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic2tmoi_handler
* Description  : The function accepts INTRIIC2TMOI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic2tmoi_handler (uint32_t int_sense)
{
    intriictmoi_proc_handler(RIIC_CH2);

}
/******************************************************************************
 * End of Function intriic2tmoi_handler
 *****************************************************************************/

/******************************************************************************
* Function Name: intriic3tmoi_handler
* Description  : The function accepts INTRIIC3TMOI interrupt.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
static void intriic3tmoi_handler (uint32_t int_sense)
{
    intriictmoi_proc_handler(RIIC_CH3);

}
/******************************************************************************
 * End of Function intriic3tmoi_handler
 *****************************************************************************/

/* End of File */
