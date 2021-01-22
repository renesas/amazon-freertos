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
 * File Name    : r_rza2_riic_lld.c
 * Device(s)    : RZ/A2M
 * Tool-Chain   : GCC ARM Embedded 6.3.1.20170620
 * OS           : None
 * H/W Platform : RZ/A2M Evaluation Board
 * Description  : RZ/A2M RIIC Driver - Low level function of RIIC module
 * Operation    :
 * Limitations  :
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 *****************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"           /* Low level register read/write header */

/* Driver Includes */
#include "driver.h"                 /* Device driver header */
#include "r_rza2m_riic_lld_api.h"   /* Low level Driver Header */
#include "r_rza2m_riic_lld_cfg.h"   /* Low level Driver Configuration Header */
#include "r_rza2m_riic_lld_prv.h"   /* Low level Driver Configuration Header */
#include "r_riic_drv_sc_cfg.h"
#include "r_stb_drv_api.h"

/******************************************************************************
 Macro definitions
 *****************************************************************************/

/******************************************************************************
 Typedefs
 *****************************************************************************/

/******************************************************************************
 Private global variables and functions
 *****************************************************************************/
/**
 * @section st_riic structure used for register access of RIIC channels
 */
static volatile struct st_riic *gsp_riic[RIIC_CFG_LLD_NUM_CHANNELS] =
{
    /* RIIC Channels */
    &RIIC30, &RIIC31, &RIIC32, &RIIC33 
};

static const e_stb_module_t gs_riic_module[] =
{ 
    MODULE_I2C0, MODULE_I2C1, MODULE_I2C2, MODULE_I2C3 
};

/* clock parameter base table define 
 * in case of PCLK = 66MHz, noise filter disable, rise_time = 0, fall_time = 0
 */
static const st_riic_freq_table_param_t gs_riic_clk_base_param_66m[RIIC_FREQUENCY_MAX][RIIC_DUTY_MAX] =
{
    {
        /* SCL = 100KHz */
        { RIIC_PCLK_DIV_16, 4, 34 }, /* Duty 10%, vaule exceeds RIIC_BITRATE_REG_MAX, an error occur */
        { RIIC_PCLK_DIV_16, 8, 30 }, /* Duty 20% */
        { RIIC_PCLK_DIV_16, 11, 27 }, /* Duty 30% */
        { RIIC_PCLK_DIV_16, 15, 23 }, /* Duty 40% */
        { RIIC_PCLK_DIV_16, 19, 19 }, /* Duty 50% */
        { RIIC_PCLK_DIV_16, 23, 15 }, /* Duty 60% */
        { RIIC_PCLK_DIV_16, 27, 11 }, /* Duty 70% */
        { RIIC_PCLK_DIV_16, 30, 8 }, /* Duty 80% */
        { RIIC_PCLK_DIV_16, 34, 4 } /* Duty 90% vaule exceeds RIIC_BITRATE_REG_MAX, an error occur */
    },
    {
        /* SCL = 400KHz */
        { RIIC_PCLK_DIV_4, 4, 34 }, /* Duty 10% vaule exceeds RIIC_BITRATE_REG_MAX, an error occur */
        { RIIC_PCLK_DIV_4, 8, 30 }, /* Duty 20% */
        { RIIC_PCLK_DIV_4, 11, 27 }, /* Duty 30% */
        { RIIC_PCLK_DIV_4, 15, 23 }, /* Duty 40% */
        { RIIC_PCLK_DIV_4, 19, 19 }, /* Duty 50% */
        { RIIC_PCLK_DIV_4, 23, 15 }, /* Duty 60% */
        { RIIC_PCLK_DIV_4, 27, 11 }, /* Duty 70% */
        { RIIC_PCLK_DIV_4, 30, 8 }, /* Duty 80% */
        { RIIC_PCLK_DIV_4, 34, 4 } /* Duty 90% vaule exceeds RIIC_BITRATE_REG_MAX, an error occur */
    },
    {
        /* SCL = 1MHz */
        { RIIC_PCLK_DIV_2, 3, 24 }, /* Duty 10% */
        { RIIC_PCLK_DIV_2, 5, 22 }, /* Duty 20% */
        { RIIC_PCLK_DIV_2, 8, 19 }, /* Duty 30% */
        { RIIC_PCLK_DIV_2, 10, 17 }, /* Duty 40% */
        { RIIC_PCLK_DIV_2, 13, 14 }, /* Duty 50% */
        { RIIC_PCLK_DIV_2, 17, 10 }, /* Duty 60% */
        { RIIC_PCLK_DIV_2, 19, 8 }, /* Duty 70% */
        { RIIC_PCLK_DIV_2, 22, 5 }, /* Duty 80% */
        { RIIC_PCLK_DIV_2, 24, 3 } /* Duty 90% */
    } 
};

/* clock parameter base table define 
 * in case of PCLK = 60MHz, noise filter disable, rise_time = 0, fall_time = 0
 */
static const st_riic_freq_table_param_t gs_riic_clk_base_param_60m[RIIC_FREQUENCY_MAX][RIIC_DUTY_MAX] =
{
    {
        /* SCL = 100KHz */
        { RIIC_PCLK_DIV_16, 3, 31 }, /* Duty 10% */
        { RIIC_PCLK_DIV_16, 7, 27 }, /* Duty 20% */
        { RIIC_PCLK_DIV_16, 10, 24 }, /* Duty 30% */
        { RIIC_PCLK_DIV_16, 13, 21 }, /* Duty 40% */
        { RIIC_PCLK_DIV_16, 17, 17 }, /* Duty 50% */
        { RIIC_PCLK_DIV_16, 21, 13 }, /* Duty 60% */
        { RIIC_PCLK_DIV_16, 24, 10 }, /* Duty 70% */
        { RIIC_PCLK_DIV_16, 27, 7 }, /* Duty 80% */
        { RIIC_PCLK_DIV_16, 31, 3 } /* Duty 90% */
    },
    {
        /* SCL = 400KHz */
        { RIIC_PCLK_DIV_4, 3, 31 }, /* Duty 10% */
        { RIIC_PCLK_DIV_4, 7, 27 }, /* Duty 20% */
        { RIIC_PCLK_DIV_4, 10, 24 }, /* Duty 30% */
        { RIIC_PCLK_DIV_4, 13, 21 }, /* Duty 40% */
        { RIIC_PCLK_DIV_4, 17, 17 }, /* Duty 50% */
        { RIIC_PCLK_DIV_4, 21, 13 }, /* Duty 60% */
        { RIIC_PCLK_DIV_4, 24, 10 }, /* Duty 70% */
        { RIIC_PCLK_DIV_4, 27, 7 }, /* Duty 80% */
        { RIIC_PCLK_DIV_4, 31, 3 } /* Duty 90% */
    },
    {
        /* SCL = 1MHz */
        { RIIC_PCLK_DIV_2, 2, 22 }, /* Duty 10% */
        { RIIC_PCLK_DIV_2, 5, 19 }, /* Duty 20% */
        { RIIC_PCLK_DIV_2, 7, 17 }, /* Duty 30% */
        { RIIC_PCLK_DIV_2, 10, 14 }, /* Duty 40% */
        { RIIC_PCLK_DIV_2, 12, 12 }, /* Duty 50% */
        { RIIC_PCLK_DIV_2, 14, 10 }, /* Duty 60% */
        { RIIC_PCLK_DIV_2, 17, 7 }, /* Duty 70% */
        { RIIC_PCLK_DIV_2, 19, 5 }, /* Duty 80% */
        { RIIC_PCLK_DIV_2, 22, 2 } /* Duty 90% */
    } 
};

/* clock parameter base table define 
 * in case of PCLK = 55MHz, noise filter disable, rise_time = 0, fall_time = 0
 */
static const st_riic_freq_table_param_t gs_riic_clk_base_param_55m[RIIC_FREQUENCY_MAX][RIIC_DUTY_MAX] =
{
    {
        /* SCL = 100KHz */
        { RIIC_PCLK_DIV_16, 3, 28 }, /* Duty 10% */
        { RIIC_PCLK_DIV_16, 6, 25 }, /* Duty 20% */
        { RIIC_PCLK_DIV_16, 9, 22 }, /* Duty 30% */
        { RIIC_PCLK_DIV_16, 12, 19 }, /* Duty 40% */
        { RIIC_PCLK_DIV_16, 15, 16 }, /* Duty 50% */
        { RIIC_PCLK_DIV_16, 19, 12 }, /* Duty 60% */
        { RIIC_PCLK_DIV_16, 22, 9 }, /* Duty 70% */
        { RIIC_PCLK_DIV_16, 25, 6 }, /* Duty 80% */
        { RIIC_PCLK_DIV_16, 28, 3 } /* Duty 90% */
    },
    {
        /* SCL = 400KHz */
        { RIIC_PCLK_DIV_4, 3, 28 }, /* Duty 10% */
        { RIIC_PCLK_DIV_4, 6, 25 }, /* Duty 20% */
        { RIIC_PCLK_DIV_4, 9, 22 }, /* Duty 30% */
        { RIIC_PCLK_DIV_4, 12, 19 }, /* Duty 40% */
        { RIIC_PCLK_DIV_4, 15, 16 }, /* Duty 50% */
        { RIIC_PCLK_DIV_4, 19, 12 }, /* Duty 60% */
        { RIIC_PCLK_DIV_4, 22, 9 }, /* Duty 70% */
        { RIIC_PCLK_DIV_4, 25, 6 }, /* Duty 80% */
        { RIIC_PCLK_DIV_4, 28, 3 } /* Duty 90% */
    },
    {
        /* SCL = 1MHz */
        { RIIC_PCLK_DIV_2, 2, 20 }, /* Duty 10% */
        { RIIC_PCLK_DIV_2, 4, 18 }, /* Duty 20% */
        { RIIC_PCLK_DIV_2, 7, 15 }, /* Duty 30% */
        { RIIC_PCLK_DIV_2, 9, 13 }, /* Duty 40% */
        { RIIC_PCLK_DIV_2, 11, 11 }, /* Duty 50% */
        { RIIC_PCLK_DIV_2, 13, 9 }, /* Duty 60% */
        { RIIC_PCLK_DIV_2, 15, 7 }, /* Duty 70% */
        { RIIC_PCLK_DIV_2, 18, 4 }, /* Duty 80% */
        { RIIC_PCLK_DIV_2, 20, 2 } /* Duty 90% */
    } 
};

static st_riic_config_t gs_riic_config[RIIC_CFG_LLD_NUM_CHANNELS];
static uint32_t gs_board_pclk = 0;

/* Prototypes */
static int_t riic_adjust_scl_clk (st_riic_clk_check_param_t *p_cfg);
static int_t riic_set_reset (int_t channel);
static int_t riic_release_reset (int_t channel);
static int_t riic_register_slave_addrss (int_t channel, st_riic_config_t *p_cfg);
static int_t riic_set_scl_clk (int_t channel, st_riic_config_t *p_cfg, uint32_t pclk);
static int_t riic_set_noise_filter (int_t channel, st_riic_config_t *p_cfg);
static int_t riic_set_timeout (int_t channel, st_riic_config_t *p_cfg);
static int_t riic_set_bus_format (int_t channel, st_riic_config_t *p_cfg);
static int_t riic_hw_init (int_t channel);

/* noise filter table define */
static uint32_t gs_noise_filter_nf_table[RIIC_FILTER_STAGE_MAX] =
{
    RIIC_MR3_REG_NF_STAGE1, /* RIIC_FILTER_NOT_USED */
    RIIC_MR3_REG_NF_STAGE1, /* RIIC_FILTER_STAGE_1 */
    RIIC_MR3_REG_NF_STAGE2, /* RIIC_FILTER_STAGE_2 */
    RIIC_MR3_REG_NF_STAGE3, /* RIIC_FILTER_STAGE_3 */
    RIIC_MR3_REG_NF_STAGE4 /* RIIC_FILTER_STAGE_4 */
};

/******************************************************************************
* Function Name: R_RIIC_ScInitChannel
* Description  : This function configures device registers in preparation for
*                configuration.
* Arguments    : sc_config_index - Index number of RIIC Initialization channel.
*                pclk_frequency - PCLK frequency for RIIC.
* Return Value : DRV_SUCCESS - Successful operation.
*                DRV_ERROR - Combination of frequency setting does not conform 
*                            to the standard.
******************************************************************************/
int_t R_RIIC_ScInitChannel (int_t sc_config_index, uint32_t pclk_frequency)
{
    int_t retval = DRV_SUCCESS;
    uint32_t slave_addr_num;
    int_t channel;
    int_t sc_table_size = (sizeof(RIIC_SC_TABLE)) / (sizeof(st_r_drv_riic_sc_config_t));

    /* check index number of SC */
    if ((sc_config_index >= 0) && (sc_config_index < sc_table_size))
    {
        channel = RIIC_SC_TABLE[sc_config_index].channel;
            
        /* check channel registered in SC */
        if ((channel >= RIIC_CFG_LLD_NUM_CHANNELS) || (channel < 0))
        {
            retval = DRV_ERROR;
        }
    }
    else
    {
        retval = DRV_ERROR;
    }

    if (DRV_SUCCESS == retval)
    {
        gs_riic_config[channel].riic_mode = RIIC_SC_TABLE[sc_config_index].config.riic_mode;
        for (slave_addr_num = 0; slave_addr_num < RIIC_SLAVE_ADDR_NUM_MAX; slave_addr_num++)
        {
            gs_riic_config[channel].slave_address_enable[slave_addr_num] =
                    RIIC_SC_TABLE[sc_config_index].config.slave_address_enable[slave_addr_num];
            gs_riic_config[channel].slave_address[slave_addr_num] =
                    RIIC_SC_TABLE[sc_config_index].config.slave_address[slave_addr_num];
            gs_riic_config[channel].slave_address_length[slave_addr_num] =
                    RIIC_SC_TABLE[sc_config_index].config.slave_address_length[slave_addr_num];
        }
        gs_riic_config[channel].frequency = RIIC_SC_TABLE[sc_config_index].config.frequency;
        gs_riic_config[channel].duty = RIIC_SC_TABLE[sc_config_index].config.duty;
        gs_riic_config[channel].rise_time = RIIC_SC_TABLE[sc_config_index].config.rise_time;
        gs_riic_config[channel].fall_time = RIIC_SC_TABLE[sc_config_index].config.fall_time;
        gs_riic_config[channel].noise_filter_stage = RIIC_SC_TABLE[sc_config_index].config.noise_filter_stage;
        gs_riic_config[channel].timeout = RIIC_SC_TABLE[sc_config_index].config.timeout;
        gs_riic_config[channel].format = RIIC_SC_TABLE[sc_config_index].config.format;
        gs_riic_config[channel].host_address_enabled = RIIC_SC_TABLE[sc_config_index].config.host_address_enabled;
        gs_riic_config[channel].tei_priority = RIIC_SC_TABLE[sc_config_index].config.tei_priority;
        gs_riic_config[channel].ri_priority = RIIC_SC_TABLE[sc_config_index].config.ri_priority;
        gs_riic_config[channel].ti_priority = RIIC_SC_TABLE[sc_config_index].config.ti_priority;
        gs_riic_config[channel].spi_priority = RIIC_SC_TABLE[sc_config_index].config.spi_priority;
        gs_riic_config[channel].sti_priority = RIIC_SC_TABLE[sc_config_index].config.sti_priority;
        gs_riic_config[channel].naki_priority = RIIC_SC_TABLE[sc_config_index].config.naki_priority;
        gs_riic_config[channel].ali_priority = RIIC_SC_TABLE[sc_config_index].config.ali_priority;
        gs_riic_config[channel].tmoi_priority = RIIC_SC_TABLE[sc_config_index].config.tmoi_priority;
        gs_board_pclk = pclk_frequency;
    }

    /* Start RIIC module */
    if (DRV_SUCCESS == retval)
    {
        retval = R_STB_StartModule(gs_riic_module[channel]);
        if (STB_SUCCESS != retval)
        {
            retval = DRV_ERROR;
        }
    }

    if (DRV_SUCCESS == retval)
    {
        riic_set_reset(channel);

        riic_register_slave_addrss(channel, &gs_riic_config[channel]);

        retval = riic_set_scl_clk(channel, &gs_riic_config[channel], pclk_frequency);

        if (DRV_SUCCESS == retval)
        {
            riic_set_bus_format(channel, &gs_riic_config[channel]);

            riic_set_noise_filter(channel, &gs_riic_config[channel]);

            riic_set_timeout(channel, &gs_riic_config[channel]);

            interrupt_init(channel, &gs_riic_config[channel]);
        }
        else
        {
            riic_hw_init(channel);
        }

        riic_release_reset(channel);
    }

    return retval;
}
/******************************************************************************
 * End of Function R_RIIC_ScInitChannel
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_CloseChannel
* Description  : This function clear device registers and shutdown.
* Arguments    : channel - Channel to close.
* Return Value : None.
******************************************************************************/
void R_RIIC_CloseChannel (int_t channel)
{
    interrupt_uninit(channel);

    riic_set_reset(channel);

    riic_hw_init(channel);

    riic_release_reset(channel);

    R_STB_StopModule(gs_riic_module[channel]);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_CloseChannel
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_SetConfig
* Description  : This function configures device registers in preparation for
*                configuration.
* Arguments    : channel - Channel to configure.
* Return Value : DRV_SUCCESS - Successful operation.
*                DRV_ERROR - Specify a illegal channel number.
******************************************************************************/
int_t R_RIIC_SetConfig (int_t channel, st_riic_config_t *p_cfg)
{
    int_t retval = DRV_SUCCESS;
    uint32_t slave_addr_num;
    st_riic_clk_check_param_t clk_param;

    /* check arguments */
    if ((channel >= RIIC_CFG_LLD_NUM_CHANNELS) || (channel < 0))
    {
        retval = DRV_ERROR;
    }

    if (DRV_SUCCESS == retval)
    {
        clk_param.pclk = gs_board_pclk;
        clk_param.frequency = p_cfg->frequency;
        clk_param.duty = p_cfg->duty;
        clk_param.rise_time = p_cfg->rise_time;
        clk_param.fall_time = p_cfg->fall_time;
        clk_param.noise_filter_stage = p_cfg->noise_filter_stage;
        clk_param.format = p_cfg->format;

        /* check clock parameter */
        retval = riic_adjust_scl_clk( &clk_param);
    }

    if (DRV_SUCCESS == retval)
    {
        riic_set_reset(channel);

        riic_register_slave_addrss(channel, p_cfg);

        /* clock has already been checked, so no error check */
        riic_set_scl_clk(channel, p_cfg, gs_board_pclk);

        riic_set_bus_format(channel, p_cfg);

        riic_set_noise_filter(channel, p_cfg);

        riic_set_timeout(channel, p_cfg);

        interrupt_uninit(channel);

        interrupt_init(channel, p_cfg);

        gs_riic_config[channel].riic_mode = p_cfg->riic_mode;
        for (slave_addr_num = 0; slave_addr_num < RIIC_SLAVE_ADDR_NUM_MAX; slave_addr_num++)
        {
            gs_riic_config[channel].slave_address_enable[slave_addr_num] 
                = p_cfg->slave_address_enable[slave_addr_num];
            gs_riic_config[channel].slave_address[slave_addr_num] 
                = p_cfg->slave_address[slave_addr_num];
            gs_riic_config[channel].slave_address_length[slave_addr_num] 
                = p_cfg->slave_address_length[slave_addr_num];
        }
        gs_riic_config[channel].frequency = p_cfg->frequency;
        gs_riic_config[channel].duty = p_cfg->duty;
        gs_riic_config[channel].rise_time = p_cfg->rise_time;
        gs_riic_config[channel].fall_time = p_cfg->fall_time;
        gs_riic_config[channel].noise_filter_stage = p_cfg->noise_filter_stage;
        gs_riic_config[channel].timeout = p_cfg->timeout;
        gs_riic_config[channel].format = p_cfg->format;
        gs_riic_config[channel].host_address_enabled = p_cfg->host_address_enabled;
        gs_riic_config[channel].tei_priority = p_cfg->tei_priority;
        gs_riic_config[channel].ri_priority = p_cfg->ri_priority;
        gs_riic_config[channel].ti_priority = p_cfg->ti_priority;
        gs_riic_config[channel].spi_priority = p_cfg->spi_priority;
        gs_riic_config[channel].sti_priority = p_cfg->sti_priority;
        gs_riic_config[channel].naki_priority = p_cfg->naki_priority;
        gs_riic_config[channel].ali_priority = p_cfg->ali_priority;
        gs_riic_config[channel].tmoi_priority = p_cfg->tmoi_priority;

        riic_release_reset(channel);
    }

    return retval;
}
/******************************************************************************
 * End of Function R_RIIC_SetConfig
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_GetConfig
* Description  : This function gets configuration parameters for each channel.
* Arguments    : channel - Channel to getting configuration parameters.
*              : *p_cfg - RIIC configure parameter
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
int_t R_RIIC_GetConfig (int_t channel, st_riic_config_t *p_cfg)
{
    int_t retval = DRV_SUCCESS;
    uint32_t slave_addr_num;

    p_cfg->riic_mode = gs_riic_config[channel].riic_mode;
    for (slave_addr_num = 0; slave_addr_num < RIIC_SLAVE_ADDR_NUM_MAX; slave_addr_num++)
    {
        p_cfg->slave_address_enable[slave_addr_num] = gs_riic_config[channel].slave_address_enable[slave_addr_num];
        p_cfg->slave_address[slave_addr_num] = gs_riic_config[channel].slave_address[slave_addr_num];
        p_cfg->slave_address_length[slave_addr_num] = gs_riic_config[channel].slave_address_length[slave_addr_num];
    }
    p_cfg->frequency = gs_riic_config[channel].frequency;
    p_cfg->duty = gs_riic_config[channel].duty;
    p_cfg->rise_time = gs_riic_config[channel].rise_time;
    p_cfg->fall_time = gs_riic_config[channel].fall_time;
    p_cfg->noise_filter_stage = gs_riic_config[channel].noise_filter_stage;
    p_cfg->timeout = gs_riic_config[channel].timeout;
    p_cfg->format = gs_riic_config[channel].format;
    p_cfg->host_address_enabled = gs_riic_config[channel].host_address_enabled;
    p_cfg->tei_priority = gs_riic_config[channel].tei_priority;
    p_cfg->ri_priority = gs_riic_config[channel].ri_priority;
    p_cfg->ti_priority = gs_riic_config[channel].ti_priority;
    p_cfg->spi_priority = gs_riic_config[channel].spi_priority;
    p_cfg->sti_priority = gs_riic_config[channel].sti_priority;
    p_cfg->naki_priority = gs_riic_config[channel].naki_priority;
    p_cfg->ali_priority = gs_riic_config[channel].ali_priority;
    p_cfg->tmoi_priority = gs_riic_config[channel].tmoi_priority;

    return retval;
}
/******************************************************************************
 * End of Function R_RIIC_GetConfig
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_TransmitStop
* Description  : This function is called by HLD, and issues STOP condition.
* Arguments    : channel - Channel that issues STOP condition.
* Return Value : None.
******************************************************************************/
void R_RIIC_TransmitStop (int_t channel)
{
    /* clear stop bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR2.LONG, (uint32_t) 0, RIIC_ICCR2_SP_SHIFT,
    RIIC_ICCR2_SP);

    /* enable detect stop condition interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_SPIE_SHIFT,
    RIIC_ICIER_SPIE);

    /* set stop bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR2.LONG, (uint32_t) 1, RIIC_ICCR2_SP_SHIFT,
    RIIC_ICCR2_SP);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_TransmitStop
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_ClearStop
* Description  : This function is called by HLD, and clear STOP condition bit.
* Arguments    : channel - Channel that clear STOP condition bit.
* Return Value : None.
******************************************************************************/
void R_RIIC_ClearStop (int_t channel)
{
    volatile uint32_t dummy;

    /* read stop bit */
    dummy = RZA_IO_RegRead_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_STOP_SHIFT,
    RIIC_ICSR2_STOP);

    /* clear stop bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, (uint32_t) 0, RIIC_ICSR2_STOP_SHIFT,
    RIIC_ICSR2_STOP);

    /* disable start interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 0, RIIC_ICIER_SPIE_SHIFT,
    RIIC_ICIER_SPIE);

    /* disable transmit interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 0, RIIC_ICIER_TIE_SHIFT,
    RIIC_ICIER_TIE);

    /* disable receive interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 0, RIIC_ICIER_RIE_SHIFT,
    RIIC_ICIER_RIE);

    /* disable nak interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 0, RIIC_ICIER_NAKIE_SHIFT,
    RIIC_ICIER_NAKIE);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_ClearStop 
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_TransmitStart
* Description  : This function is called by HLD,and issues START condition.
* Arguments    : channel - Channel that issues START condition.
* Return Value : None.
******************************************************************************/
void R_RIIC_TransmitStart (int_t channel)
{
    /* enable transmit interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_TIE_SHIFT,
    RIIC_ICIER_TIE);

    /* enable receive interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_RIE_SHIFT,
    RIIC_ICIER_RIE);

    /* enable nak interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_NAKIE_SHIFT,
    RIIC_ICIER_NAKIE);

    /* transmit start bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR2.LONG, (uint32_t) 1, RIIC_ICCR2_ST_SHIFT,
    RIIC_ICCR2_ST);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_TransmitStart
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_TransmitRestart
* Description  : This function is called by HLD, and issues RESTART condition.
* Arguments    : channel - Channel that issues RESTART condition.
* Return Value : None.
******************************************************************************/
void R_RIIC_TransmitRestart (int_t channel)
{
    /* enable transmit interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_TIE_SHIFT,
    RIIC_ICIER_TIE);

    /* enable receive interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_RIE_SHIFT,
    RIIC_ICIER_RIE);

    /* enable nak interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_NAKIE_SHIFT,
    RIIC_ICIER_NAKIE);

    /* set restart */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR2.LONG, (uint32_t) 1, RIIC_ICCR2_RS_SHIFT,
    RIIC_ICCR2_RS);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_TransmitRestart
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_ClearNack
* Description  : This function is called by HLD, and clear receiving NACK bit.
* Arguments    : channel - Channel that clear receiving NACK bit.
* Return Value : None.
******************************************************************************/
void R_RIIC_ClearNack (int_t channel)
{
    /* clear stop bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSR2.LONG, (uint32_t) 0, RIIC_ICSR2_NACKF_SHIFT,
    RIIC_ICSR2_NACKF);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_ClearNack 
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_GetAckStatus
* Description  : This function is called by HLD, and get the ACK status.
* Arguments    : channel - Channel to get the ack status.
* Return Value : 0 - ACK received.
*              : 1 - NACK received.
******************************************************************************/
uint8_t R_RIIC_GetAckStatus (int_t channel)
{

    uint32_t ret_ack;

    ret_ack = RZA_IO_RegRead_32( &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_NACKF_SHIFT, RIIC_ICSR2_NACKF);

    /* cast result back to uint8_t */
    return (uint8_t) ret_ack;
}
/******************************************************************************
 * End of Function  R_RIIC_GetAckStatus
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_TransmitAck
* Description  : This function is called by HLD, and transfer ACK to 
*                write device.
* Arguments    : channel - Channel that transfer ACK.
* Return Value : None.
******************************************************************************/
void R_RIIC_TransmitAck (int_t channel)
{
    /* get permission of writing to ACK bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 1, RIIC_ICMR3_ACKWP_SHIFT,
    RIIC_ICMR3_ACKWP);

    /* clear ACK bit to ACK transfer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 0, RIIC_ICMR3_ACKBT_SHIFT,
    RIIC_ICMR3_ACKBT);

    /* release permission of writing to ACK bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 0, RIIC_ICMR3_ACKWP_SHIFT,
    RIIC_ICMR3_ACKWP);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_TransmitAck
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_TransmitNack
* Description  : This function is called by HLD, and transfer NACK to 
*                write device.
* Arguments    : channel - Channel that transfer NACK.
* Return Value : None.
******************************************************************************/
void R_RIIC_TransmitNack (int_t channel)
{
    /* get permission of writing to NACK bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 1, RIIC_ICMR3_ACKWP_SHIFT,
    RIIC_ICMR3_ACKWP);

    /* set ACK bit to NACK transfer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 1, RIIC_ICMR3_ACKBT_SHIFT,
    RIIC_ICMR3_ACKBT);

    /* release permission of writing to NACK bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 0, RIIC_ICMR3_ACKWP_SHIFT,
    RIIC_ICMR3_ACKWP);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_TransmitNack
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_AssertLowHold
* Description  : This function is called by HLD, and hold the SCL in 
*                the low level.
* Arguments    : channel - Channel that hold the SCL.
* Return Value : None.
******************************************************************************/
void R_RIIC_AssertLowHold (int_t channel)
{
    /* set wait bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 1, RIIC_ICMR3_WAIT_SHIFT,
    RIIC_ICMR3_WAIT);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_AssertLowHold 
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_ReleaseLowHold
* Description  : This function is called by HLD, and release the low hold of 
*                the SCL.
* Arguments    : channel - Channel that low hold of SCL.
* Return Value : None.
******************************************************************************/
void R_RIIC_ReleaseLowHold (int_t channel)
{
    /* set wait bit */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 0, RIIC_ICMR3_WAIT_SHIFT,
    RIIC_ICMR3_WAIT);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_ReleaseLowHold  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_EnableTransEndIntr
* Description  : This function is called by HLD, and enable transfer end 
*                interrupt.
* Arguments    : channel - Channel that enable transfer end.
* Return Value : None.
******************************************************************************/
void R_RIIC_EnableTransEndIntr (int_t channel)
{
    /* enable transmit end interrupt*/
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_TEIE_SHIFT,
    RIIC_ICIER_TEIE);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_EnableTransEndIntr  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_DisableTransEndIntr
* Description  : This function is called by HLD, and disable transfer end 
*                interrupt.
* Arguments    : channel - Channel that disable transfer end.
* Return Value : None.
******************************************************************************/
void R_RIIC_DisableTransEndIntr (int_t channel)
{
    /* enable transmit end interrupt*/
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 0, RIIC_ICIER_TEIE_SHIFT,
    RIIC_ICIER_TEIE);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_EnableTransEndIntr  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_WriteByte
* Description  : This function is called by HLD, and write to the 
*                data register.
* Arguments    : channel - Channel that write to the data register.
*                byte - transfer data.
* Return Value : None.
******************************************************************************/
void R_RIIC_WriteByte (int_t channel, uint8_t byte)
{
    /* write data */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICDRT.LONG, (uint32_t) byte, RIIC_ICDRT_DRT_SHIFT,
    RIIC_ICDRT_DRT);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_WriteByte  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_ReadByte
* Description  : This function is called by HLD, and read transfer data 
*                from register.
* Arguments    : channel - Channel that read transfer data from register.
* Return Value : 0x00 to 0xFF - receive data.
******************************************************************************/
uint8_t R_RIIC_ReadByte (int_t channel)
{
    uint32_t read_data;

    /* read data */
    read_data = RZA_IO_RegRead_32( &gsp_riic[channel]->ICDRR.LONG, RIIC_ICDRR_DRR_SHIFT, RIIC_ICDRR_DRR);

    /* data is in LSB of uint32_t result */
    return (uint8_t) read_data;
}
/******************************************************************************
 * End of Function R_RIIC_ReadByte  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_IsBusBusy
* Description  : This function is called by HLD, and get bus busy status.
* Arguments    : channel - Channel that get bus busy status.
* Return Value : 0 - BUS free.
*                1 - BUS busy.
******************************************************************************/
uint8_t R_RIIC_IsBusBusy (int_t channel)
{
    uint32_t busy_stat;

    /* get bus busy status */
    busy_stat = RZA_IO_RegRead_32( &gsp_riic[channel]->ICCR2.LONG, RIIC_ICCR2_BBSY_SHIFT, RIIC_ICCR2_BBSY);

    /* cast to uint8_t */
    return (uint8_t) busy_stat;
}
/******************************************************************************
 * End of Function R_RIIC_IsBusBusy  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_IsStopAsserted
* Description  : This function is called by HLD, and get detection status of 
*                STOP condition.
* Arguments    : channel - Channel that get detection status of 
*                STOP condition.
* Return Value : 0 - STOP condition not detected.
*                1 - STOP condition detected.
******************************************************************************/
uint8_t R_RIIC_IsStopAsserted (int_t channel)
{
    uint32_t stop_stat;

    /* get bus busy status */
    stop_stat = RZA_IO_RegRead_32( &gsp_riic[channel]->ICSR2.LONG, RIIC_ICSR2_STOP_SHIFT, RIIC_ICSR2_STOP);

    /* cast to uint8_t */
    return (uint8_t) stop_stat;
}
/******************************************************************************
 * End of Function R_RIIC_IsStopAsserted  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_WaitSlaveAddr
* Description  : This function is called by HLD, and start waiting slave 
*                address from master device.
*                Also, after calling this function, it is always in the 
*                stop condition standby state.
* Arguments    : channel - RIIC channel of waiting slave address.
* Return Value : DRV_SUCCESS - Waiting slave address is suucess.
*                DRV_ERROR - Waiting slave address in master mode.
******************************************************************************/
int_t R_RIIC_WaitSlaveAddr (int_t channel)
{
    if (RIIC_MODE_MASTER == gs_riic_config[channel].riic_mode)
    {
        return (DRV_ERROR);
    }

    /* enable receive full interrupt and start waiting slave address */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_RIE_SHIFT,
    RIIC_ICIER_RIE);

    /* enable transmit interrupt*/
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_TIE_SHIFT,
    RIIC_ICIER_TIE);

    /* enable nak interrupt*/
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_NAKIE_SHIFT,
    RIIC_ICIER_NAKIE);

    /* enable start detect interrupt*/
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_SPIE_SHIFT,
    RIIC_ICIER_SPIE);

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function R_RIIC_WaitSlaveAddr  
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_DetectTimeoutStart
* Description  : This function is called by HLD, and start timeout counter.
* Arguments    : channel - Channel that start timeout counter.
* Return Value : None.
******************************************************************************/
void R_RIIC_DetectTimeoutStart (int_t channel)
{
    /* start timer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 1, RIIC_ICFER_TMOE_SHIFT,
    RIIC_ICFER_TMOE);

    /* enable timer interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_TMOIE_SHIFT,
    RIIC_ICIER_TMOIE);

    return;
}
/*****************************************************************************
 * End of Function R_RIIC_DetectTimeoutStart
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_DetectTimeoutStop
* Description  : This function is called by HLD, and stop timeout counter.
* Arguments    : channel - Channel that stop timeout counter.
* Return Value : None.
******************************************************************************/
void R_RIIC_DetectTimeoutStop (int_t channel)
{
    /* disable timer interrupt */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 0, RIIC_ICIER_TMOIE_SHIFT,
    RIIC_ICIER_TMOIE);

    /* stop timer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_TMOE_SHIFT,
    RIIC_ICFER_TMOE);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_DetectTimeoutStop
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_DetectArbitrationStart
* Description  : This function is called by HLD, and start detecting of 
*                arbitration.
* Arguments    : channel - Channel that start detecting arbitration.
* Return Value : None.
******************************************************************************/
void R_RIIC_DetectArbitrationStart (int_t channel)
{
    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 1, RIIC_ICFER_MALE_SHIFT,
    RIIC_ICFER_MALE);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 1, RIIC_ICFER_NALE_SHIFT,
    RIIC_ICFER_NALE);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 1, RIIC_ICFER_SALE_SHIFT,
    RIIC_ICFER_SALE);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 1, RIIC_ICIER_ALIE_SHIFT,
    RIIC_ICIER_ALIE);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_DetectArbitrationStart
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_DetectArbitrationStop
* Description  : This function is called by HLD, and stop detecting of 
*                arbitration.
* Arguments    : channel - Channel that stop detecting arbitration.
* Return Value : None.
******************************************************************************/
void R_RIIC_DetectArbitrationStop (int_t channel)
{
    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICIER.LONG, (uint32_t) 0, RIIC_ICIER_ALIE_SHIFT,
    RIIC_ICIER_ALIE);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_MALE_SHIFT,
    RIIC_ICFER_MALE);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_NALE_SHIFT,
    RIIC_ICFER_NALE);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_SALE_SHIFT,
    RIIC_ICFER_SALE);

    return;
}
/******************************************************************************
 * End of Function R_RIIC_DetectArbitrationStop
 *****************************************************************************/

/******************************************************************************
* Function Name: R_RIIC_GetSlaveAddrNum
* Description  : This function is called by HLD, and get slave address number 
*                of detected slave address.
* Arguments    : channel - Channel that gets slave address number of detected 
*                          slave address.
* Return Value : 0 - Slave address registered in slave address0 was detected.
*                1 - Slave address registered in slave address1 was detected
*                2 - Slave address registered in slave address2 was detected
*                DRV_ERROR - Slave address wasn't detected.
******************************************************************************/
e_riic_slave_addr_num_t R_RIIC_GetSlaveAddrNum (int_t channel)
{
    uint32_t detect_stat = 0;
    e_riic_slave_addr_num_t detect_num = 0;

    /* get bus busy status */
    while (detect_num < RIIC_SLAVE_ADDR_NUM_MAX)
    {
        detect_stat = RZA_IO_RegRead_32( &gsp_riic[channel]->ICSR1.LONG, 0, (RIIC_ICSR1_AAS0 << detect_num));
        if (0 != detect_stat)
        {
            break;
        }
        detect_num++;
    }

    if (RIIC_SLAVE_ADDR_NUM_MAX == detect_num)
    {
        return DRV_ERROR;
    }

    return detect_num;
}
/******************************************************************************
 * End of Function R_RIIC_GetSlaveAddrNum  
 *****************************************************************************/

/******************************************************************************
 * Function Name: R_RIIC_GetVersion
 * Description  : Get version information
 * Arguments    : pVerInfo - version info
 * Return Value : DRV_SUCCESS - Success(always).
 *****************************************************************************/
int_t R_RIIC_GetVersion (st_ver_info_t *pVerInfo)
{
    pVerInfo->lld.p_szdriver_name = STDIO_RIIC_RZ_LLD_DRV_NAME;
    pVerInfo->lld.version.sub.major = STDIO_RIIC_RZ_LLD_VERSION_MAJOR;
    pVerInfo->lld.version.sub.minor = STDIO_RIIC_RZ_LLD_VERSION_MINOR;
    pVerInfo->lld.build = STDIO_RIIC_RZ_LLD_BUILD_NUM;

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function R_RIIC_GetVersion  
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_adjust_scl_clk
* Description  : This function adjusts the high time and the low time of SCL 
*                from the values of rise time and fall time.
* Arguments    : *p_cfg - RIIC frequency parameter.
* Return Value : DRV_SUCCESS - Operation is success.
*                DRV_ERROR - Illegal parameters are set.
******************************************************************************/
static int_t riic_adjust_scl_clk (st_riic_clk_check_param_t *p_cfg)
{
    int_t retval = DRV_SUCCESS;
    uint32_t width_delay;
    uint32_t unit_time = 0;

    /* Check combinations of settings that are not allowed */
    if ((RIIC_FORMAT_SMBUS == p_cfg->format) && (RIIC_FREQUENCY_400KHZ <= p_cfg->frequency))
    {
        retval = DRV_ERROR;
    }

    if ((RIIC_FREQUENCY_1MHZ == p_cfg->frequency) && (RIIC_FILTER_NOT_USED != p_cfg->noise_filter_stage))
    {
        retval = DRV_ERROR;
    }

    if (((RIIC_DUTY_10 == p_cfg->duty) || (RIIC_DUTY_90 == p_cfg->duty))
            && (RIIC_FILTER_NOT_USED != p_cfg->noise_filter_stage))
    {
        retval = DRV_ERROR;
    }

    if (DRV_SUCCESS == retval)
    {
        /* Get clock parameter table data */
        if ((RIIC_PCLK_66MHZ == p_cfg->pclk) || ((RIIC_PCLK_66MHZ / 2) == p_cfg->pclk))
        {
            p_cfg->high_width = gs_riic_clk_base_param_66m[p_cfg->frequency][p_cfg->duty].high_width;
            p_cfg->low_width = gs_riic_clk_base_param_66m[p_cfg->frequency][p_cfg->duty].low_width;
            p_cfg->pclk_div = gs_riic_clk_base_param_66m[p_cfg->frequency][p_cfg->duty].pclk;
            if (RIIC_PCLK_66MHZ != p_cfg->pclk)
            {
                /* 33MHz, div rate 1/2 setting */
                p_cfg->pclk_div -= 1;
            }
        }
        else if ((RIIC_PCLK_60MHZ == p_cfg->pclk) || ((RIIC_PCLK_60MHZ / 2) == p_cfg->pclk))
        {
            p_cfg->high_width = gs_riic_clk_base_param_60m[p_cfg->frequency][p_cfg->duty].high_width;
            p_cfg->low_width = gs_riic_clk_base_param_60m[p_cfg->frequency][p_cfg->duty].low_width;
            p_cfg->pclk_div = gs_riic_clk_base_param_60m[p_cfg->frequency][p_cfg->duty].pclk;
            if (RIIC_PCLK_60MHZ != p_cfg->pclk)
            {
                /* 30MHz, div rate 1/2 setting */
                p_cfg->pclk_div -= 1;
            }
        }
        else if ((RIIC_PCLK_55MHZ == p_cfg->pclk) || ((RIIC_PCLK_55MHZ / 2) == p_cfg->pclk))
        {
            p_cfg->high_width = gs_riic_clk_base_param_55m[p_cfg->frequency][p_cfg->duty].high_width;
            p_cfg->low_width = gs_riic_clk_base_param_55m[p_cfg->frequency][p_cfg->duty].low_width;
            p_cfg->pclk_div = gs_riic_clk_base_param_55m[p_cfg->frequency][p_cfg->duty].pclk;
            if (RIIC_PCLK_55MHZ != p_cfg->pclk)
            {
                /* 27.5MHz, div rate 1/2 setting */
                p_cfg->pclk_div -= 1;
            }
        }
        else
        {
            retval = DRV_ERROR;
        }
    }

    /* Calculate the time per clock of SCL */
    if (DRV_SUCCESS == retval)
    {
        unit_time = ((RIIC_UNIT_TIME_SCL * RIIC_UNIT_PCLK_HZ) / p_cfg->pclk) * (1 << (p_cfg->pclk_div));

        /* Calculate how much rise_time corresponds to unit of SCL width */
        width_delay = (p_cfg->rise_time / unit_time);

        if ((p_cfg->high_width >= (width_delay + p_cfg->noise_filter_stage))
                && (RIIC_BITRATE_REG_MAX >= (p_cfg->high_width - (width_delay + p_cfg->noise_filter_stage))))
        {
            /* As the width widens by rise_time and filter steps, 
               subtract them */
            p_cfg->high_width -= ((uint32_t) width_delay + p_cfg->noise_filter_stage);
        }
        else
        {
            retval = DRV_ERROR;
        }
    }

    if (DRV_SUCCESS == retval)
    {
        /* Calculate how much fall_time corresponds to unit of SCL width */
        width_delay = (p_cfg->fall_time / unit_time);

        if ((p_cfg->low_width >= (width_delay + p_cfg->noise_filter_stage))
                && (RIIC_BITRATE_REG_MAX >= (p_cfg->low_width - (width_delay + p_cfg->noise_filter_stage))))
        {
            /* As the width widens by rise_time and filter steps, subtract them */
            p_cfg->low_width -= ((uint32_t) width_delay + p_cfg->noise_filter_stage);
        }
        else
        {
            retval = DRV_ERROR;
        }
    }

    return retval;
}
/******************************************************************************
 * End of Function riic_adjust_scl_clk
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_set_reset
* Description  : Resets an RIIC channel HW.
* Arguments    : channel - RIIC channel to be reset.
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
static int_t riic_set_reset (int_t channel)
{

    /* get lock */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR1.LONG, (uint32_t) 0, RIIC_ICCR1_ICE_SHIFT,
    RIIC_ICCR1_ICE);

    /* Reset */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR1.LONG, (uint32_t) 1, RIIC_ICCR1_IICRST_SHIFT,
    RIIC_ICCR1_IICRST);

    /* release lock*/
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR1.LONG, (uint32_t) 1, RIIC_ICCR1_ICE_SHIFT,
    RIIC_ICCR1_ICE);

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function riic_set_reset
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_release_reset
* Description  : Release resets an RIIC channel HW.
* Arguments    : channel - RIIC channel to release reset.
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
static int_t riic_release_reset (int_t channel)
{

    /* Release Reset */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICCR1.LONG, (uint32_t) 0, RIIC_ICCR1_IICRST_SHIFT,
    RIIC_ICCR1_IICRST);

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function riic_release_reset
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_register_slave_addrss
* Description  : Register slave address for the RIIC channel.
*                This function is called only when operating in slave mode.
*                The 3registered addresses are detected in parallel.
* Arguments    : channel - RIIC channel to resigter slave address.
*                *p_cfg - RIIC configure parameter.
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
static int_t riic_register_slave_addrss (int_t channel, st_riic_config_t *p_cfg)
{
#if (RIIC_MODE_SLAVE_IMPLEMENTED == 1)
    uint32_t addr_num;

    if (RIIC_MODE_SLAVE == p_cfg->riic_mode)
    {
        for (addr_num = 0; addr_num < RIIC_SLAVE_ADDR_NUM_MAX; addr_num++)
        {
            if (false == p_cfg->slave_address_enable[addr_num])
            {
                /* clear slave address */
                switch (addr_num)
                {
                    case RIIC_SLAVE_ADDR_NUM_0:
                    {
                        /* clear slave address */
                        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, 
                                            RIIC_ICSER_SAR0_SHIFT, RIIC_ICSER_SAR0);

                        /* cast to volatile uint32_t pointer */
                        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR0.LONG, (uint32_t) 0, 
                                            IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
                    }
                    break;

                    case RIIC_SLAVE_ADDR_NUM_1:
                    {
                        /* clear slave address */
                        RZA_IO_RegWrite_32((volatile uint32t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, 
                                            RIIC_ICSER_SAR1_SHIFT, RIIC_ICSER_SAR1);

                        /* cast to volatile uint32_t pointer */
                        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR1.LONG, (uint32_t) 0, 
                                            IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
                    }
                    break;

                    case RIIC_SLAVE_ADDR_NUM_2:
                    {
                        /* clear slave address */
                        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, 
                                            RIIC_ICSER_SAR2_SHIFT, RIIC_ICSER_SAR2);

                        /* cast to volatile uint32_t pointer */
                        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR2.LONG, (uint32_t) 0, 
                                            IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
                    }
                    break;

                    default :
                    /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                    /* <-IPA R3.5.2 */
                    /* do nothing */
                    break;
                }
            }
            else
            {
                if ((RIIC_DEVICE_ADDR_7 == p_cfg->slave_address_length[addr_num]))
                {
                    switch (addr_num)
                    {
                        /* register 7bit slave address */
                        case RIIC_SLAVE_ADDR_NUM_0:
                        {
                            /* clear slave address */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 1, 
                                                RIIC_ICSER_SAR0_SHIFT, RIIC_ICSER_SAR0);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR0.LONG, (uint32_t) 1, 
                                                RIIC_ICSAR0_FS0_SHIFT, RIIC_ICSAR0_FS0);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR0.LONG, 
                                                (uint32_t) p_cfg->slave_address[addr_num], 
                                                RIIC_ICSAR0_SVA_SHIFT,
                                                RIIC_ICSAR0_SVA);
                        }
                        break;

                        case RIIC_SLAVE_ADDR_NUM_1:
                        {
                            /* clear slave address */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 1, 
                                                RIIC_ICSER_SAR1_SHIFT, RIIC_ICSER_SAR1);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR1.LONG, (uint32_t) 1, 
                                                RIIC_ICSAR1_FS1_SHIFT, RIIC_ICSAR1_FS1);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR1.LONG, 
                                                (uint32_t) p_cfg->slave_address[addr_num], 
                                                RIIC_ICSAR1_SVA_SHIFT, 
                                                RIIC_ICSAR1_SVA);
                        }
                        break;

                        case RIIC_SLAVE_ADDR_NUM_2:
                        {
                            /* clear slave address */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 1, 
                                                RIIC_ICSER_SAR2_SHIFT, RIIC_ICSER_SAR2);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR2.LONG, (uint32_t) 1, 
                                                RIIC_ICSAR2_FS2_SHIFT, RIIC_ICSAR2_FS2);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR2.LONG, 
                                                (uint32_t) p_cfg->slave_address[addr_num], 
                                                RIIC_ICSAR2_SVA_SHIFT, 
                                                RIIC_ICSAR2_SVA);
                        }
                        break;

                        default :
                        /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                        /* <-IPA R3.5.2 */
                        /* do nothing */
                        break;
                    }
                }
                else
                {
                    switch (addr_num)
                    {
                        /* register 10bit slave address */
                        case RIIC_SLAVE_ADDR_NUM_0:
                        {
                            /* clear slave address */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 1, 
                                                RIIC_ICSER_SAR0_SHIFT, RIIC_ICSER_SAR0);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR0.LONG, (uint32_t) 1, 
                                                RIIC_ICSAR0_FS0_SHIFT, RIIC_ICSAR0_FS0);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR0.LONG, 
                                                (uint32_t) p_cfg->slave_address[addr_num], 
                                                RIIC_ICSAR0_SVA0_SHIFT, 
                                                RIIC_ICSAR0_SVA0);
                        }
                        break;

                        case RIIC_SLAVE_ADDR_NUM_1:
                        {
                            /* clear slave address */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 1, 
                                                RIIC_ICSER_SAR1_SHIFT, RIIC_ICSER_SAR1);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR1.LONG, (uint32_t) 1, 
                                                RIIC_ICSAR1_FS1_SHIFT, RIIC_ICSAR1_FS1);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR1.LONG, 
                                                (uint32_t) p_cfg->slave_address[addr_num], 
                                                RIIC_ICSAR1_SVA0_SHIFT, 
                                                RIIC_ICSAR1_SVA0);
                        }
                        break;

                        case RIIC_SLAVE_ADDR_NUM_2:
                        {
                            /* clear slave address */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 1, 
                                                RIIC_ICSER_SAR2_SHIFT, RIIC_ICSER_SAR2);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR2.LONG, (uint32_t) 1, 
                                                RIIC_ICSAR2_FS2_SHIFT, RIIC_ICSAR2_FS2);

                            /* cast to volatile uint32_t pointer */
                            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR2.LONG, 
                                                (uint32_t) p_cfg->slave_address[addr_num], 
                                                RIIC_ICSAR2_SVA0_SHIFT, 
                                                RIIC_ICSAR2_SVA0);
                        }
                        break;

                        default :
                        /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                        /* <-IPA R3.5.2 */
                        /* do nothing */
                        break;
                    }
                }
            }
        }
    }
    else
#else /* (RIIC_MODE_SLAVE_IMPLEMENTED == 1) */
    
#endif
    {
        /* clear slave address */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, RIIC_ICSER_SAR0_SHIFT,
        RIIC_ICSER_SAR0);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR0.LONG, (uint32_t) 0, IOREG_NONSHIFT_ACCESS,
        IOREG_NONMASK_ACCESS);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, RIIC_ICSER_SAR1_SHIFT,
        RIIC_ICSER_SAR1);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR1.LONG, (uint32_t) 0, IOREG_NONSHIFT_ACCESS,
        IOREG_NONMASK_ACCESS);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, RIIC_ICSER_SAR2_SHIFT,
        RIIC_ICSER_SAR2);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR2.LONG, (uint32_t) 0, IOREG_NONSHIFT_ACCESS,
        IOREG_NONMASK_ACCESS);
    }

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function riic_register_slave_addrss
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_set_scl_clk
* Description  : Sets RIIC SCL parameter.
* Arguments    : channel - RIIC channel to set clock parameter.
*                *p_cfg - RIIC configration parameter.
*                pclk - PCLK frequency for RIIC.
* Return Value : DRV_SUCCESS - Operation is success.
*                DRV_ERROR - Combination of frequency setting is illegal 
*                            parameter.
******************************************************************************/
static int_t riic_set_scl_clk (int_t channel, st_riic_config_t *p_cfg, uint32_t pclk)
{
    int_t retval = DRV_SUCCESS;
    st_riic_clk_check_param_t clk_param;

    if (RIIC_MODE_MASTER == p_cfg->riic_mode)
    {
        clk_param.pclk = pclk;
        clk_param.frequency = p_cfg->frequency;
        clk_param.duty = p_cfg->duty;
        clk_param.rise_time = p_cfg->rise_time;
        clk_param.fall_time = p_cfg->fall_time;
        clk_param.noise_filter_stage = p_cfg->noise_filter_stage;
        clk_param.format = p_cfg->format;

        retval = riic_adjust_scl_clk( &clk_param);

        if (DRV_SUCCESS == retval)
        {
            if (RIIC_FREQUENCY_1MHZ == p_cfg->frequency)
            {
                /* set fast mode plus */
                RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 1,
                RIIC_ICFER_FMPE_SHIFT, RIIC_ICFER_FMPE);
            }
            else
            {
                /* clear fast mode plus */
                RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0,
                RIIC_ICFER_FMPE_SHIFT, RIIC_ICFER_FMPE);
            }

            /* cast to volatile uint32_t pointer */
            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR1.LONG, (uint32_t) clk_param.pclk_div,
            RIIC_ICMR1_CKS_SHIFT, RIIC_ICMR1_CKS);

            /* cast to volatile uint32_t pointer */
            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICBRL.LONG, (uint32_t) clk_param.low_width,
            RIIC_ICBRL_BRL_SHIFT, RIIC_ICBRL_BRL);

            /* cast to volatile uint32_t pointer */
            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICBRH.LONG, (uint32_t) clk_param.high_width,
            RIIC_ICBRH_BRH_SHIFT, RIIC_ICBRH_BRH);
        }
    }
    else
    {
        /* clear fast mode plus */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_FMPE_SHIFT,
        RIIC_ICFER_FMPE);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR1.LONG, (uint32_t) 0, RIIC_ICMR1_CKS_SHIFT,
        RIIC_ICMR1_CKS);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICBRL.LONG, (uint32_t) 0, RIIC_ICBRL_BRL_SHIFT,
        RIIC_ICBRL_BRL);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICBRH.LONG, (uint32_t) 0, RIIC_ICBRH_BRH_SHIFT,
        RIIC_ICBRH_BRH);

    }

    return retval;
}
/******************************************************************************
 * End of Function riic_set_scl_clk
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_set_noise_filter
* Description  : Sets RIIC noise filter.
* Arguments    : channel - RIIC channel to set clock parameter.
*                *p_cfg - RIIC frequency parameter.
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
static int_t riic_set_noise_filter (int_t channel, st_riic_config_t *p_cfg)
{
    /* set noise filter stages */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG,
                        gs_noise_filter_nf_table[p_cfg->noise_filter_stage], RIIC_ICMR3_NF_SHIFT, RIIC_ICMR3_NF);

    if (RIIC_FILTER_NOT_USED != p_cfg->noise_filter_stage)
    {
        /* noise filter enable */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 1, RIIC_ICFER_NFE_SHIFT,
        RIIC_ICFER_NFE);
    }
    else
    {
        /* noise filter disable */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_NFE_SHIFT,
        RIIC_ICFER_NFE);
    }

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function riic_set_noise_filter
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_set_timeout
* Description  : Sets RIIC timeout parameter.
*                The count-up permission setting is performed at the start of 
*                RIIC transfer.
* Arguments    : channel - RIIC channel channel to set timeout.
*                *p_cfg - RIIC timeout parameter.
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
static int_t riic_set_timeout (int_t channel, st_riic_config_t *p_cfg)
{
    if (RIIC_TIMEOUT_NOT_USED != p_cfg->timeout)
    {
        /* timeout enabling setting is not done here */
        /* when RIIC transfer start, timeout is enabled */
        if (RIIC_TIMEOUT_SHORT == p_cfg->timeout)
        {
            /* set short mode to timeout counter */
            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 1,
            RIIC_ICMR2_TMOS_SHIFT, RIIC_ICMR2_TMOS);
        }
        else if (RIIC_TIMEOUT_LONG == p_cfg->timeout)
        {
            /* set long mode to timeout counter */
            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 0,
            RIIC_ICMR2_TMOS_SHIFT, RIIC_ICMR2_TMOS);
        }
        else
        {
            /* disable timeout */
            /* Nothing done */
            ;
        }

        /* register the state to be monitored (high & low state) */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 1, RIIC_ICMR2_TMOL_SHIFT,
        RIIC_ICMR2_TMOL);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 1, RIIC_ICMR2_TMOH_SHIFT,
        RIIC_ICMR2_TMOH);
    }
    else
    {
        /* register the state to be monitored (high & low state) */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 0, RIIC_ICMR2_TMOL_SHIFT,
        RIIC_ICMR2_TMOL);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 0, RIIC_ICMR2_TMOH_SHIFT,
        RIIC_ICMR2_TMOH);
    }

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function riic_set_timeout
 *****************************************************************************/

/******************************************************************************
* Function Name: riic_set_bus_format
* Description  : Sets RIIC timeout parameter.
*                The count-up permission setting is performed at the start of 
*                RIIC transfer.
* Arguments    : channel - RIIC channel channel to set timeout.
*                *p_cfg - RIIC timeout parameter.
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
static int_t riic_set_bus_format (int_t channel, st_riic_config_t *p_cfg)
{
    if (RIIC_FORMAT_I2C == p_cfg->format)
    {
        /* set I2C format */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 0, RIIC_ICMR3_SMBE_SHIFT,
        RIIC_ICMR3_SMBE);

        /* cast to volatile uint32_t pointer */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, RIIC_ICSER_HOAE_SHIFT,
        RIIC_ICSER_HOAE);
    }
    else
    {
        /* set SMBUS format */
        RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 1, RIIC_ICMR3_SMBE_SHIFT,
        RIIC_ICMR3_SMBE);
        if (false != p_cfg->host_address_enabled)
        {
            /* Host address enable */
            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 1,
            RIIC_ICSER_HOAE_SHIFT, RIIC_ICSER_HOAE);
        }
        else
        {
            /* Host address disable */
            RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0,
            RIIC_ICSER_HOAE_SHIFT, RIIC_ICSER_HOAE);
        }
    }

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function riic_set_bus_format
 *****************************************************************************/

/**
 * @brief riic_hw_init Initialize register of RIIC
 *
 * riic_hw_init initialize RIIC HW.
 * This function is called when HW initialization fails and when it is closed.
 *
 * @param[in] Channel RIIC channel to set timeout.
 *
 * @retval  none
 */
/******************************************************************************
* Function Name: riic_hw_init
* Description  : Initialize RIIC HW.
*                This function is called when HW initialization fails and when 
*                it is closed.
* Arguments    : channel - RIIC channel channel to set timeout.
* Return Value : DRV_SUCCESS - Success(always).
******************************************************************************/
static int_t riic_hw_init (int_t channel)
{
    /* clear slave address */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, RIIC_ICSER_SAR0_SHIFT,
    RIIC_ICSER_SAR0);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR0.LONG, (uint32_t) 0, IOREG_NONSHIFT_ACCESS,
    IOREG_NONMASK_ACCESS);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, RIIC_ICSER_SAR1_SHIFT,
    RIIC_ICSER_SAR1);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR1.LONG, (uint32_t) 0, IOREG_NONSHIFT_ACCESS,
    IOREG_NONMASK_ACCESS);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSER.LONG, (uint32_t) 0, RIIC_ICSER_SAR2_SHIFT,
    RIIC_ICSER_SAR2);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICSAR2.LONG, (uint32_t) 0, IOREG_NONSHIFT_ACCESS,
    IOREG_NONMASK_ACCESS);

    /* clear fast mode plus */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_FMPE_SHIFT,
    RIIC_ICFER_FMPE);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR1.LONG, (uint32_t) 0, RIIC_ICMR1_CKS_SHIFT,
    RIIC_ICMR1_CKS);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICBRL.LONG, (uint32_t) 0, RIIC_ICBRL_BRL_SHIFT,
    RIIC_ICBRL_BRL);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICBRH.LONG, (uint32_t) 0, RIIC_ICBRH_BRH_SHIFT,
    RIIC_ICBRH_BRH);

    /* clear noise filter stages */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR3.LONG, (uint32_t) 0, RIIC_ICMR3_NF_SHIFT,
    RIIC_ICMR3_NF);

    /* noise filter disable */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICFER.LONG, (uint32_t) 0, RIIC_ICFER_NFE_SHIFT,
    RIIC_ICFER_NFE);

    /* register the state to be monitored (high & low state) */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 0, RIIC_ICMR2_TMOL_SHIFT,
    RIIC_ICMR2_TMOL);

    /* cast to volatile uint32_t pointer */
    RZA_IO_RegWrite_32((volatile uint32_t *) &gsp_riic[channel]->ICMR2.LONG, (uint32_t) 0, RIIC_ICMR2_TMOH_SHIFT,
    RIIC_ICMR2_TMOH);

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of Function riic_hw_init
 *****************************************************************************/

/* End of File */
