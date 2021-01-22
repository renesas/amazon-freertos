/**********************************************************************************************************************
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
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * @file          r_rza2_scifa_lld.c
 * Device(s)      RZ/A2
 * Tool-Chain     GCC ARM Embedded 5.4 2016q3
 * H/W Platform   RZ/A2
 * @brief         SCIFA Low Layer Driver
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 15.05.2018 1.00    Release
 *              : 23.05.2019 1.01    Restructuring to ensure that other LLDs are only accessed from the SCIFA LLD
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <string.h>

#include "r_typedefs.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"              /* Low level register read/write header */

/* Driver Includes */
#include "driver.h"                    /* Device driver header */
#include "r_rza2_scifa_lld.h"          /* Low Layer Driver header */
#include "r_rza2_scifa_lld_cfg.h"      /* Low Layer Driver configuration header */
#include "r_scifa_drv_sc_cfg.h"
#include "r_stb_drv_link.h"            /* STB driver */
#include "r_cpg_drv_api.h"             /* CPG driver */

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define R_PRV_TX_FIFO_LENGTH        (16)
#define R_PRV_RX_FIFO_EMPTY         (0)

#define R_PRV_TX_FIFO_MAX_TRG       (0x0Fu)
#define R_PRV_TX_FIFO_MIN_TRG       (0x00u)
#define R_PRV_RX_FIFO_MAX_TRG       (0x10u)
#define R_PRV_RX_FIFO_MIN_TRG       (0x01u)

#define R_PRV_SCIFA_CHANNEL_0       (0)
#define R_PRV_SCIFA_CHANNEL_1       (1)
#define R_PRV_SCIFA_CHANNEL_2       (2)
#define R_PRV_SCIFA_CHANNEL_3       (3)
#define R_PRV_SCIFA_CHANNEL_4       (4)


/**********************************************************************************************************************
 Typedefs
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/
/**
 * @section st_scifa structure used for register access of SCIFA channels
 */
static volatile struct st_scifa *gsp_scifa[SCIFA_CFG_LLD_NUM_CHANNELS] =
{
    /* SCIFA channels */
    &SCIFA0, &SCIFA1, &SCIFA2, &SCIFA3, &SCIFA4
};

/**********************************************************************************************************************
 Prototypes
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Public Function Definitions
 *********************************************************************************************************************/

/**
 * @brief Starts the SCIFA channel allowing reading/writing
 *
 * R_SCIFA_Start starts an SCIFA channel according to the
 * read_write variable, this determines whether Transmit
 * Enable and Receive Enable are turned on. Or just one,
 * for read-only or write-only access
 *
 * @param[in] channel: SCIFA channel to be started
 * @param[in] read_write: read/write permission setting
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_Start (r_channel_t channel, int_t read_write)
{
    /* check arguments */
    if ((channel >= SCIFA_CFG_LLD_NUM_CHANNELS) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    if ((read_write & __SWR) > 0)
    {
        /* Enable transmit */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 1, SCIFA_SCR_TE_SHIFT, SCIFA_SCR_TE);

        /* Disable transmit interrupt */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);
    }

    if ((read_write & __SRD) > 0)
    {
        /* Enable receive */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 1, SCIFA_SCR_RE_SHIFT, SCIFA_SCR_RE);

        /* Enable receive interrupt */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 1, SCIFA_SCR_RIE_SHIFT, SCIFA_SCR_RIE);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_Start
  *********************************************************************************************************************/

/**
 * @brief Stops SCIFA channel operation
 *
 * R_SCIFA_Stop will turn off the Transmit/Receive enable for a given
 * channel, it also prevents the device from triggering TXI and RXI
 *
 * @param[in] Channel: the channel to stop operation for
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_Stop (r_channel_t channel)
{
    /* check arguments */
    if ((channel >= SCIFA_CFG_LLD_NUM_CHANNELS) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Disable transmit interrupt */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);

    /* Disable transmit */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_TE_SHIFT, SCIFA_SCR_TE);

    /* Disable receive interrupt */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_RIE_SHIFT, SCIFA_SCR_RIE);

    /* Disable receive */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_RE_SHIFT, SCIFA_SCR_RE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_Stop
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetChannel Converts SC table index into SCIFA channel
 *
 * This function interrogates SC table to determine which
 * channel is described by sc_config
 *
 * @param[in] sc_config: configuration ID
 *
 * @retval  =>0: corresponding SCIFA channel
 * @retval  -1 DRV_ERROR
 */
r_channel_t R_SCIFA_GetChannel (r_sc_index_t sc_config_index)
{
    r_channel_t ret = DRV_ERROR;
    r_sc_index_t sc_table_size = (sizeof(SCIFA_SC_TABLE)) / sizeof(st_r_drv_scifa_sc_config_t);

    /* Perform range check of sc_config to determine if index is available in st_r_drv_scifa_sc_config_t */
    if ((sc_config_index >= 0) && (sc_config_index < sc_table_size))
    {
        ret = SCIFA_SC_TABLE[sc_config_index].channel;
    }

    return (ret);
}
/**********************************************************************************************************************
 End of function R_SCIFA_GetChannel
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_Init initialises a channel for set-up
 * This function configures device registers in preparation for
 * configuration
 *
 * @param[in] channel Channel to initialise.
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_Init (r_channel_t channel)
{
    /* check arguments */
    if ((channel >= SCIFA_CFG_LLD_NUM_CHANNELS) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Set SCR.TE and SCR.RE -> 0 */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Reset TFRST */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FCR.WORD, (uint16_t) 1, SCIFA_FCR_TFRST_SHIFT, SCIFA_FCR_TFRST);

    /* Reset FRDR */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FCR.WORD, (uint16_t) 1, SCIFA_FCR_RFRST_SHIFT, SCIFA_FCR_RFRST);

    /* Read error flag first*/
    RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Clear all error flags of FSR */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, (uint16_t) 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Read error flag first */
    RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->LSR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* clear error flag of LSR */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->LSR.WORD, (uint16_t) 0, SCIFA_LSR_ORER_SHIFT, SCIFA_LSR_ORER);

    /* set TXD output high */
    R_SCIFA_SetTXDOutput(channel, SCIFA_SPTR_INIT_HIGH);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_Init
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_SetParity Sets the channel parity
 * This function configures the channel parity to a setting
 * specified in e_scifa_parity_t
 *
 * @param[in] channel: channel to configure parity for
 * @param[in] desired_parity: parity to configure channel with
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetParity (r_channel_t channel, scifa_parity_t desired_parity)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    switch (desired_parity)
    {
        default:
        {
            /* invalid desired_parity value */
            return (DRV_ERROR);
        }

        case SCIFA_NONE_PARITY:
        {
            /* Parity enable bit */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 0, SCIFA_SMR_PE_SHIFT, SCIFA_SMR_PE);
            break;
        }

        case SCIFA_ODD_PARITY:
        {
            /* Parity enable bit */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 1, SCIFA_SMR_PE_SHIFT, SCIFA_SMR_PE);

            /* Parity mode bit */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 1, SCIFA_SMR_PM_SHIFT, SCIFA_SMR_PM);
            break;
        }

        case SCIFA_EVEN_PARITY:
        {
            /* Parity enable bit */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 1, SCIFA_SMR_PE_SHIFT, SCIFA_SMR_PE);

            /* Parity mode bit */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 0, SCIFA_SMR_PM_SHIFT, SCIFA_SMR_PM);
            break;
        }
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetParity
 *********************************************************************************************************************/

/**
 * @brief R_SICFA_SetTransmissionMode sets the transmission mode for a channel
 * This function configures the channel parity to a setting
 * specified in e_scifa_mode_t
 *
 * @param[in] channel: channel to set transmission mode for
 * @param[in] desired_mode: the desired transmission mode for the channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SICFA_SetTransmissionMode (r_channel_t channel, scifa_sync_mode_t desired_mode)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    switch (desired_mode)
    {
        default:
        {
            /* invalid desired_mode value */
            return (DRV_ERROR);
        }

        case SCIFA_MODE_ASYNC:
        {
            /* Communication mode : Asynchronous */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 0, SCIFA_SMR_CM_SHIFT, SCIFA_SMR_CM);
            break;
        }

        case SCIFA_MODE_SYNC:
        {
            /* Communication mode : Synchronous */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 1, SCIFA_SMR_CM_SHIFT, SCIFA_SMR_CM);
            break;
        }
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SICFA_SetTransmissionMode
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_SetStopBits Sets the number of stop bits for a channel
 * This function configures the channel stop bits to a setting
 * specified in e_scifa_stop_bits_t
 *
 * @param[in] channel: channel to set stop bits for
 * @param[in] desired_stop_bits: number of stop bits to set for channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetStopBits (r_channel_t channel, scifa_stop_t desired_stop_bits)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    switch (desired_stop_bits)
    {
        default:
        {
            /* invalid desired_stop_bits value */
            return (DRV_ERROR);
        }

        case SCIFA_STOPBITS_1:
        {
            /* Stop bit length : 1 stop bit */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 0, SCIFA_SMR_STOP_SHIFT, SCIFA_SMR_STOP);
            break;
        }

        case SCIFA_STOPBITS_2:
        {
            /* Stop bit length : 2 stop bits */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 1, SCIFA_SMR_STOP_SHIFT, SCIFA_SMR_STOP);
            break;
        }
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetStopBits
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_SetDataBits sets the number of data bits for a channel
 * This function configures the channel data bits to a value defined in
 * e_scifa_data_bits_t
 *
 * @param[in] channel: channel to set data bits for
 * @param[in] desired_data_bits: number of data bits to set for channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_SetDataBits (r_channel_t channel, scifa_size_t desired_data_bits)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    switch (desired_data_bits)
    {
        default:
        {
            /* invalid desired_data_bits value */
            return (DRV_ERROR);
        }

        case SCIFA_DATA_7BIT:
        {
            /* Character length: 7-bit data */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 1, SCIFA_SMR_CHR_SHIFT, SCIFA_SMR_CHR);
            break;
        }

        case SCIFA_DATA_8BIT:
        {
            /* Character length: 8-bit data */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SMR.WORD, (uint16_t) 0, SCIFA_SMR_CHR_SHIFT, SCIFA_SMR_CHR);
            break;
        }
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetDataBits
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_SetLoopBack enables loop back mode on a channel
 * This function enables loop back mode on a channel,
 * loop back, connects TX to RX for a given channel,
 * this can be useful for testing whether a channel is 
 * sending/receiving correctly
 *
 * @param[in] channel: channel to enable loop back for
 * @param[in] loopback: setting for channel loop back
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetLoopBack (r_channel_t channel, scifa_loop_t loopback)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    switch (loopback)
    {
        default:
        {
            /* invalid loopback value */
            return (DRV_ERROR);
        }

        case SCIFA_LOOPBACK_DISABLE:
        {
            /* Loop back disabled */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FCR.WORD, 0u, SCIFA_FCR_LOOP_SHIFT, SCIFA_FCR_LOOP);
            break;
        }

        case SCIFA_LOOPBACK_ENABLE:
        {
            /* Loop back enabled */
            RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FCR.WORD, 1u, SCIFA_FCR_LOOP_SHIFT, SCIFA_FCR_LOOP);
            break;
        }
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetLoopBack
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_SetDataOrder Sets the data order for a given channel
 * This function sets the data order for a given channel,
 * this is either MSB or LSB first as defined in e_scifa_data_order_t
 *
 * @param[in] channel: channel to set data order for
 * @param[in] desired_data_order: data order setting for channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetDataOrder (r_channel_t channel, scifa_dir_t desired_data_order)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    switch (desired_data_order)
    {
        default:
        {
            /* invalid desired_data_order value */
            return (DRV_ERROR);
        }

        case SCIFA_LSB_FIRST:
        {
            /* LSB first */
            RZA_IO_RegWrite_8((volatile uint8_t *) &gsp_scifa[channel]->SEMR.BYTE, 0u, SCIFA_SEMR_DIR_SHIFT, SCIFA_SEMR_DIR);
            break;
        }

        case SCIFA_MSB_FIRST:
        {
            /* MSB first */
            RZA_IO_RegWrite_8((volatile uint8_t *) &gsp_scifa[channel]->SEMR.BYTE, 1u, SCIFA_SEMR_DIR_SHIFT, SCIFA_SEMR_DIR);
            break;
        }
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetDataOrder
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_Open is to be called when opening a channel
 *
 * @param[in] sc_config_index: smart Configuration index to open
 * @param[out] p_config_set: configuration settings that will be updated
 *                            with the values from the SC table
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_Open (r_sc_index_t sc_config_index, scifa_config_t *p_config_set)
{
    const scifa_config_t * p_config;
    r_channel_t channel;
    int_t result;
    uint32_t achieved_baud_rate;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* get the configuration settings from the Smart Configuration table */
    p_config = &SCIFA_SC_TABLE[sc_config_index].config;

    /* configure the channel */
    result = R_SCIFA_ChannelConfigure(channel, p_config, &achieved_baud_rate);

    /* configure GPIO */
    if (DRV_SUCCESS == result)
    {
        int_t gpio_handle;
        gpio_handle = direct_open("gpio", O_RDONLY);
        
        if (gpio_handle > 0)
        {
            /* direct_control as direct_control assumes parameter 3 is i/o, but using the parameter in input only mode is acceptable */
            direct_control(gpio_handle, CTL_GPIO_INIT_BY_TABLE, (void *) &SCIFA_SC_TABLE[sc_config_index].pin);
            direct_close(gpio_handle);
        }
        else
        {
            result = DRV_ERROR;
        }
    }

    /* if we were successful, then update the stored configuration data */
    if (DRV_SUCCESS == result)
    {
        memcpy(p_config_set, p_config, sizeof(scifa_config_t));
        p_config_set->baud_rate = achieved_baud_rate;
    }

    return (result);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_Open
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_ChannelConfigure configures a channel
 *
 * @param[in] channel: channel to configure
 * @param[in] p_configuration: configuration data
 * @param[out] p_achieved_baud_rate: achieved baud rate
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_ChannelConfigure (r_channel_t channel, const scifa_config_t *p_configuration, uint32_t * p_achieved_baud_rate)
{
    int_t result;

    /* stop the device */
    result = R_SCIFA_Stop(channel);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* start the SCIFA module */
    result = R_SCIFA_StartModule(channel, true);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* initialise the device for setup */
    result = R_SCIFA_Init(channel);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    if (SCIFA_PARITY_ON == p_configuration->parity_en)
    {
        result = R_SCIFA_SetParity(channel, p_configuration->parity_type);
    }
    else
    {
        /* set parity off as SCIFA_PARITY_NONE no longer supported */
        result = R_SCIFA_SetParity(channel, SCIFA_NONE_PARITY);
    }

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = R_SCIFA_SetStopBits(channel, p_configuration->stop_bits);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = R_SICFA_SetTransmissionMode(channel, p_configuration->sync_mode);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = R_SCIFA_SetDataBits(channel, p_configuration->data_size);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    int_t cpg_handle;
    st_r_drv_cpg_get_clk_t cpg_get_clock_t;
    uint32_t current_clk_hz;

    /* open the CPG driver */
    cpg_handle = direct_open("cpg", O_RDONLY);

    if (cpg_handle < 0)
    {
        return (DRV_ERROR);
    }

    /* interrogate the CPG driver for the P1 clock frequency */
    cpg_get_clock_t.freq_src = CPG_FREQ_P1CLK;
    if (control(cpg_handle, CTL_CPG_GET_CLK, &cpg_get_clock_t) != DRV_SUCCESS)
    {
        return (DRV_ERROR);
    }

    /* close the CPG driver */
    direct_close(cpg_handle);

    /* convert data from CPG (KHz) to SCIFA (Hz) requirements */
    current_clk_hz = ((uint32_t) (cpg_get_clock_t.clk_frequency_khz)) * 1000;

    result = R_SCIFA_SetBaud(channel, p_configuration->baud_rate, current_clk_hz, p_configuration->sync_mode, p_achieved_baud_rate);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = R_SCIFA_SetLoopBack(channel, p_configuration->loopback);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = R_SCIFA_SetDataOrder(channel, p_configuration->data_direction);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = R_SCIFA_SetTXDOutput(channel, p_configuration->txd_init_value);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = R_SCIFA_SetSCKOutput(channel, p_configuration->sck_init_value);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* RTS not available on channel 3 or channel 4 */
    if ((R_PRV_SCIFA_CHANNEL_3 != channel) && (R_PRV_SCIFA_CHANNEL_4 != channel))
    {
        result = R_SCIFA_SetRTSOutput(channel, p_configuration->rts_init_value);
    }

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* CTS not available on channel 3 or channel 4 */
    if ((R_PRV_SCIFA_CHANNEL_3 != channel) && (R_PRV_SCIFA_CHANNEL_4 != channel))
    {
        result = R_SCIFA_SetCTSOutput(channel, p_configuration->cts_init_value);
    }

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* set extended features */
    result = R_SCIFA_SetExtendedCfg(channel, p_configuration);

    return (result);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_ChannelConfigure
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_Close is to be called when closing a channel
 * This function is currently stubbed, in case a developer may wish
 * to implement it in a future driver update
 *
 * @param[in] channel: channel to close
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_Close (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_Close
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StopTransmit stops transmission at the end of a DMA transmit
 *
 * @param[in] channel Channel to stop
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopTransmit (r_channel_t channel)
{
    /* check arguments */
    if ((channel > SCIFA_CFG_LLD_NUM_CHANNELS) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Disable transmit interrupt */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);

    /* Disable transmit */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_TE_SHIFT, SCIFA_SCR_TE);

    /* disable transmit end interrupt enable */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 0, SCIFA_SCR_TEIE_SHIFT, SCIFA_SCR_TEIE);

    /* clear the TEND bit */
     RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, 0, SCIFA_FSR_TEND_SHIFT, SCIFA_FSR_TEND);

    /* flush the transmit FIFO */
    gsp_scifa[channel]->FCR.BIT.TFRST = 1;
    gsp_scifa[channel]->FCR.BIT.TFRST = 0;

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StopTransmit
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetTXEIState Determines the current TXEI state for a channel
 *
 * This function will determine whether a channel has recently triggered
 * a TXEI interrupt event.
 *
 * @param[in] channel  Channel to check TXEI status for
 *
 * @retval  SCIFA_INTERRUPTING or SCIFA_NOT_INTERRUPTING
 * @retval -1 DRV_ERROR
 */
scifa_interrupting_state_t R_SCIFA_GetTXEIState (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    uint16_t txend_interrupt_state;

    /* Get transmit end interrupt state */
    txend_interrupt_state = gsp_scifa[channel]->FSR.BIT.TEND;

    return (txend_interrupt_state ? SCIFA_INTERRUPTING : SCIFA_NOT_INTERRUPTING);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_GetTXEIState
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetTXIState Determines the current TXI state for a channel.
 * This function will determine whether a channel has recently triggered
 * a TXI interrupt event
 *
 * @param[in] channel: channel to check TXI status for
 *
 * @retval  SCIFA_INTERRUPTING or SCIFA_NOT_INTERRUPTING
 * @retval -1 DRV_ERROR
 */
scifa_interrupting_state_t R_SCIFA_GetTXIState (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    uint16_t tx_interrupt_state;
    uint16_t tx_interrupt_enable_state;

    /* Get FIFO empty status */
    tx_interrupt_state = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_TDFE_SHIFT, SCIFA_FSR_TDFE);

    /* Get interrupt enable status */
    tx_interrupt_enable_state = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);

    return ((tx_interrupt_state && tx_interrupt_enable_state) ? SCIFA_INTERRUPTING : SCIFA_NOT_INTERRUPTING);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_GetTXIState
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_ClearTXIFlags removes TXI flags on a channel
 * This function is called at the end of an TXI handler routine
 * it will clear necessary TXI flag to mark TXI as handled.
 *
 * @param[in] channel: channel to clear TXI flags for
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_ClearTXIFlags (r_channel_t channel)
{
    uint16_t dummy;

    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Dummy read */
    dummy = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_TDFE_SHIFT, SCIFA_FSR_TDFE);

    /* The TDFE flag can only be reset after a 1 is read from it */
    if (1 == dummy)
    {
        /* Reset the TDFE flag */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, 0, SCIFA_FSR_TDFE_SHIFT, SCIFA_FSR_TDFE);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_ClearTXIFlags
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_CheckRXIError Determines error status of channel receive
 * This function will determine whether a channel has recently experienced
 * any overflow/parity/framing errors when receiving data
 * It returns this information in a error data structure
 *
 * @param[in] channel: channel to check RXI error status for
 * @param[out] p_rx_err: error info struct to populate with any error details
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_CheckRXIError(r_channel_t channel, st_scifa_rx_error_t *p_rx_err)
{
    int_t ret = DRV_SUCCESS;
    uint32_t framing_errors;
    uint32_t parity_errors;
    bool_t framing_errors_present;
    bool_t parity_errors_present;

    /* read the ER flag; this tells us if there were any framing or parity errors */
    if (1 == RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_ER_SHIFT, SCIFA_FSR_ER))
    {
        /* read the FER flag for framing errors */
        framing_errors_present = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_FER_SHIFT, SCIFA_FSR_FER);

        /* read the PER flag for parity errors */
        parity_errors_present = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_PER_SHIFT, SCIFA_FSR_PER);

        /* if there were any framing errors, then get the framing error count and update our total */
        if (framing_errors_present)
        {
            /* count framing errors */
            framing_errors = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->LSR.WORD, SCIFA_LSR_FER_SHIFT, SCIFA_LSR_FER);

            /* 0 actually means that there were 16 framing errors */
            if (0 == framing_errors)
            {
                framing_errors = 16;
            }

            p_rx_err->cnt_framing += framing_errors;
        }

        /* if there were any parity errors, then get the parity error count and update our total */
        if (parity_errors_present)
        {
            /* count parity errors */
            parity_errors = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->LSR.WORD, SCIFA_LSR_PER_SHIFT, SCIFA_LSR_PER);

            /* 0 actually means that there were 16 parity errors */
            if (0 == parity_errors)
            {
                parity_errors = 16;
            }

            p_rx_err->cnt_parity += parity_errors;
        }

        /* clear the ER flag */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, 0, SCIFA_FSR_ER_SHIFT, SCIFA_FSR_ER);
    }

    /* Count break errors */
    if (1 == RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_BRK_SHIFT, SCIFA_FSR_BRK))
    {
        p_rx_err->cnt_break++;
    }

    /* Count overrun errors */
    if (1 == RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->LSR.WORD, SCIFA_LSR_ORER_SHIFT, SCIFA_LSR_ORER))
    {
        p_rx_err->cnt_overrun++;

        /* clear the overrun flag */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->LSR.WORD, 0, SCIFA_LSR_ORER_SHIFT, SCIFA_LSR_ORER);
    }

    /* Is data ready? */
    if (1 == RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_DR_SHIFT, SCIFA_FSR_DR))
    {
        ret = DRV_ERROR;
    }

    return (ret);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_CheckRXIError
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetRXIState Determines the current RXI state for a channel
 * This function will determine whether a channel has recently triggered
 * a RXI interrupt event
 *
 * @param[in] channel: channel to check RXI state for
 *
 * @retval  SCIFA_INTERRUPTING or SCIFA_NOT_INTERRUPTING
 * @retval -1 DRV_ERROR
 */
scifa_interrupting_state_t R_SCIFA_GetRXIState (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    uint16_t rx_interrupt_state;              // RDF
    uint16_t rx_enable_state;                 // RE
    uint16_t rx_interrupt_enable_state;       // RIE

    /* Get FIFO full status */
    rx_interrupt_state = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_RDF_SHIFT, SCIFA_FSR_RDF);

    /* Get receive enable status */
    rx_enable_state = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, SCIFA_SCR_RE_SHIFT, SCIFA_SCR_RE);

    /* Get interrupt enable status */
    rx_interrupt_enable_state = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, SCIFA_SCR_RIE_SHIFT, SCIFA_SCR_RIE);

    return ((rx_interrupt_state && rx_enable_state && rx_interrupt_enable_state) ? SCIFA_INTERRUPTING : SCIFA_NOT_INTERRUPTING);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_GetRXIState
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_ClearRXIFlags removes RXI flags on a channel
 * This function is called at the end of an RXI handler routine
 * it will clear necessary RXI flag to mark RXI as handled
 *
 * @param[in] channel: channel to clear RXI flags for
 *
 * @retval  0  DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_ClearRXIFlags (r_channel_t channel)
{
    uint16_t dummy;

    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Clear receive data ready */
    dummy = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, SCIFA_FSR_DR_SHIFT, SCIFA_FSR_DR);


    if (1 == dummy)
    {
        /* Clear DR only if 1 read */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FSR.WORD, 0, SCIFA_FSR_DR_SHIFT, SCIFA_FSR_DR);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_ClearRXIFlags
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_IsRxFifoEmpty checks if the RX FIFO is empty
 * This function checks the current amount of bytes in the RX FIFO
 * it will then return false if data is in the FIFO, or true if it's empty
 *
 * @param[in] channel: channel to check FIFO
 * @param[out] p_fifo_empty: false if data in the FIFO, true if it's empty
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_IsRxFifoEmpty(r_channel_t channel, bool_t *p_fifo_empty)
{
    uint16_t fifo_bytes;

    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* get the number of bytes in the FIFO */
    fifo_bytes = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FDR.WORD, SCIFA_FDR_R_SHIFT, SCIFA_FDR_R);

    if (R_PRV_RX_FIFO_EMPTY == fifo_bytes)
    {
        *p_fifo_empty = true;
    }
    else
    {
        *p_fifo_empty = false;
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_IsRxFifoEmpty
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StopTXI stops TXI for a given channel
 *
 * @param[in] channel: channel to disable TXI for
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopTXI (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* disable transmit interrupt enable */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 0, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);

    /* disable transmit end interrupt enable */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 0, SCIFA_SCR_TEIE_SHIFT, SCIFA_SCR_TEIE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StopTXI
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StartRX enable receive and receive interrupts
 *
 * @param[in] channel to start receive
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartRX (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* receive enable */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 1, SCIFA_SCR_RE_SHIFT, SCIFA_SCR_RE);

    /* enable interrupts to trigger DMA */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 1, SCIFA_SCR_RIE_SHIFT, SCIFA_SCR_RIE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StartRX
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StopRX disable receive
 *
 * @param[in] channel to stop receive
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopRX (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 0, SCIFA_SCR_RE_SHIFT, SCIFA_SCR_RE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StopRX
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetReadDataRegisterAddress return the read data register address
 *
 * @param[in] channel to get the FRDR register address for
 *
 * @retval the address of the FRDR register
 * @retval NULL error
 */
void * R_SCIFA_GetReadDataRegisterAddress (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (NULL);
    }

    return (void *) &gsp_scifa[channel]->FRDR.BYTE;
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_GetReadDataRegisterAddress
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StartTX enable transmit and transmit interrupts
 *
 * @param[in] channel to start transmit
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartTX (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 1, SCIFA_SCR_TE_SHIFT, SCIFA_SCR_TE);

    /* enable interrupts to trigger DMA */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 1, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);

    /* enable transmit end interrupt */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 1, SCIFA_SCR_TEIE_SHIFT, SCIFA_SCR_TEIE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StartTX
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StopTX disable transmit
 *
 * @param[in] channel to stop transmit
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopTX (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 0, SCIFA_SCR_TE_SHIFT, SCIFA_SCR_TE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StopTX
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetWriteDataRegisterAddress return the read data register address
 *
 * @param[in] channel to get the FTDR register address for
 *
 * @retval the address of the FTDR register
 * @retval NULL error
 */
void * R_SCIFA_GetWriteDataRegisterAddress (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (NULL);
    }

    return (void *) &gsp_scifa[channel]->FTDR.BYTE;
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_GetWriteDataRegisterAddress
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StartTXI Starts TXI interrupt for a given channel
 *
 * @param[in] channel: channel to start TXI for
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartTXI (r_channel_t channel)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* enable transmit interrupt enable */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, 1, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StartTXI
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_PutByte puts a byte in a channel's FIFO
 * This function will put a single byte into the FIFO of the
 * channel given
 *
 * @param[in] channel: channel to put byte in FIFO
 * @param[in] data: byte to place in FIFO
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_PutByte (r_channel_t channel, uint8_t data)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Put byte in FIFO */
    RZA_IO_RegWrite_8((volatile uint8_t *) &gsp_scifa[channel]->FTDR.BYTE, data, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_PutByte
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetByte gets 1 byte from a channel's FIFO
 * This function retrieves a single byte from a channel's
 * transmit FIFO
 *
 * @param[in] channel: channel to retrieve byte from FIFO
 * @param[out] p_data: location to place retrieved byte
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_GetByte (r_channel_t channel, uint8_t *p_data)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* get byte from FIFO */
    *p_data = RZA_IO_RegRead_8((volatile uint8_t *) &gsp_scifa[channel]->FRDR.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_GetByte
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_IsTxFifoFull Checks for space in the TX FIFO
 * This function checks the current amount of bytes in the TX FIFO
 * it will then return false if space in the FIFO, true if it's full
 *
 * @param[in] channel: channel to check TX FIFO for
 * @param[out] p_fifo_full: false if space in the FIFO, true if it's full
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_IsTxFifoFull(r_channel_t channel, bool_t *p_fifo_full)
{
    uint16_t fifo_bytes;

    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* get the number of bytes in the FIFO */
    fifo_bytes = RZA_IO_RegRead_16((volatile uint16_t *) &gsp_scifa[channel]->FDR.WORD, SCIFA_FDR_T_SHIFT, SCIFA_FDR_T);

    if (R_PRV_TX_FIFO_LENGTH == fifo_bytes)
    {
        *p_fifo_full = true;
    }
    else
    {
        *p_fifo_full = false;
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_IsTxFifoFull
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_SetExtendedCfg sets extended device settings
 * This function sets the extended device settings,
 * these settings are usually device
 * specific or not considered commonly used
 *
 * @param[in] channel: channel to set extended settings for
 * @param[in] p_ext_cfg: extended Settings to be set
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetExtendedCfg(r_channel_t channel, const scifa_config_t *p_ext_cfg)
{
    int_t ret = DRV_SUCCESS;

    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        ret = DRV_ERROR;
    }

    /* set clock */
    if (DRV_SUCCESS == ret)
    {
        ret = R_SCIFA_SetClock(channel, p_ext_cfg->clk_enable );
    }

    if (DRV_SUCCESS == ret)
    {
        /* set noise cancellation */
        ret = R_SCIFA_SetNoiseCanceller(channel, p_ext_cfg->noise_canceller);
    }

    /* set modem */
    if (DRV_SUCCESS == ret)
    {
        ret = R_SCIFA_SetModemControl(channel, p_ext_cfg->modem_control);
    }

    /* set FIFO */
    if (DRV_SUCCESS == ret)
    {
        ret = R_SCIFA_SetFIFOThreshold(channel, p_ext_cfg->tftc, p_ext_cfg->rftc);
    }

    return (ret);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetExtendedCfg
 *********************************************************************************************************************/

/**
 * @brief Sets up modem settings for a channel
 *
 * @param[in] channel: channel to configure modem settings for
 * @param[in] modem: modem setting for the channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_SetModemControl(r_channel_t channel, scifa_mce_t modem)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Modem Control (RTS/CTS) enable/disable */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FCR.WORD, modem, SCIFA_FCR_MCE_SHIFT, SCIFA_FCR_MCE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetModemControl
 *********************************************************************************************************************/

/**
 * @brief Sets noise cancellation
 *
 * @param[in] channel: channel to set the noise cancellation for
 * @param[in] noise_cancellation: noise cancellation setting
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_SetNoiseCanceller(r_channel_t channel, scifa_nfen_t noise_cancellation)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Noise canceller enable/disable */
    RZA_IO_RegWrite_8((volatile uint8_t *) &gsp_scifa[channel]->SEMR.BYTE, noise_cancellation, SCIFA_SEMR_NFEN_SHIFT, SCIFA_SEMR_NFEN);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetNoiseCanceller
 *********************************************************************************************************************/

/**
 * @brief Sets the clock source for a channel
 *
 * @param[in] channel: channel to set clock source for
 * @param[in] desired_clock: clock setting for channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_SetClock(r_channel_t channel, scifa_clk_enable_t desired_clock)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Clock enable mode */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, desired_clock, SCIFA_SCR_CKE_SHIFT, SCIFA_SCR_CKE);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetClock
 *********************************************************************************************************************/

/**
 * @brief sets up FIFO control
 *
 * @param[in] channel: channel to set FIFO for
 * @param[in] tx_trg: TX FIFO trigger setting
 * @param[in] rx_trg: RX FIFO trigger setting
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
int_t R_SCIFA_SetFIFOThreshold(r_channel_t channel, uint8_t tx_trg, uint8_t rx_trg)
{
    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    /* Check TX FIFO Threshold is correct */
    if (tx_trg > R_PRV_TX_FIFO_MAX_TRG)
    {
        return (DRV_ERROR);
    }

    /* Check RX FIFO Threshold is correct */
    if ((rx_trg > R_PRV_RX_FIFO_MAX_TRG) || (rx_trg < R_PRV_RX_FIFO_MIN_TRG))
    {
        return (DRV_ERROR);
    }

    /* Set TX Threshold */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FTCR.WORD, tx_trg, SCIFA_FTCR_TFTC_SHIFT, SCIFA_FTCR_TFTC);

    /* Set RX Threshold */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FTCR.WORD, rx_trg, SCIFA_FTCR_RFTC_SHIFT, SCIFA_FTCR_RFTC);

    /* TX - Use the FTCR Trigger settings instead of FCR - giving user more control */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FTCR.WORD, 1u, SCIFA_FTCR_TTRGS_SHIFT, SCIFA_FTCR_TTRGS);

    /* RX - Use the FTCR Trigger settings instead of FCR - giving user more control */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FTCR.WORD, 1u, SCIFA_FTCR_RTRGS_SHIFT, SCIFA_FTCR_RTRGS);

    /* TX - Reset suspended FIFO */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FCR.WORD, (uint16_t) 0, SCIFA_FCR_TFRST_SHIFT, SCIFA_FCR_TFRST);

    /* TX - Reset suspended FIFO */
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->FCR.WORD, (uint16_t) 0, SCIFA_FCR_RFRST_SHIFT, SCIFA_FCR_RFRST);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetFIFOThreshold
 *********************************************************************************************************************/

/**
 * @brief Sets the TXD output level
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetTXDOutput (r_channel_t channel, scifa_sptr_init_t level)
{
    if (SCIFA_SPTR_INIT_HIGH == level)
    {
        /* set both SPB2DT and SPB2IO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_SPB2IO | SCIFA_SPTR_SPB2DT, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_SPB2IO | SCIFA_SPTR_SPB2DT);
    }
    else
    {
        /* set SPB2DT low and SPB2IO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_SPB2IO, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_SPB2IO | SCIFA_SPTR_SPB2DT);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetTXDOutput
 *********************************************************************************************************************/

/**
 * @brief Sets the SCK output level
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetSCKOutput (r_channel_t channel, scifa_sptr_init_t level)
{
    if (SCIFA_SPTR_INIT_HIGH == level)
    {
        /* set both SCKDT and SCKIO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_SCKIO | SCIFA_SPTR_SCKDT, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_SCKIO | SCIFA_SPTR_SCKDT);
    }
    else
    {
        /* set SCKDT low and SCKIO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_SCKIO, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_SCKIO | SCIFA_SPTR_SCKDT);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetSCKOutput
 *********************************************************************************************************************/

/**
 * @brief Sets the CTS output level
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetCTSOutput (r_channel_t channel, scifa_sptr_init_t level)
{
    /* not supported on channel 3 or 4 */
    if ((R_PRV_SCIFA_CHANNEL_3 == channel) || (R_PRV_SCIFA_CHANNEL_4 == channel))
    {
        return (DRV_ERROR);
    }

    if (SCIFA_SPTR_INIT_HIGH == level)
    {
        /* set both CTS2DT and CTS2IO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_CTS2IO | SCIFA_SPTR_CTS2DT, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_CTS2IO | SCIFA_SPTR_CTS2DT);
    }
    else
    {
        /* set CTS2DT low and CTS2IO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_CTS2IO, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_CTS2IO | SCIFA_SPTR_CTS2DT);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetCTSOutput
 *********************************************************************************************************************/

/**
 * @brief Sets the RTS output level
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetRTSOutput (r_channel_t channel, scifa_sptr_init_t level)
{
    /* not supported on channel 3 or 4 */
    if ((R_PRV_SCIFA_CHANNEL_3 == channel) || (R_PRV_SCIFA_CHANNEL_4 == channel))
    {
        return (DRV_ERROR);
    }

    if (SCIFA_SPTR_INIT_HIGH == level)
    {
        /* set both RTS2DT and RTS2IO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_RTS2IO | SCIFA_SPTR_RTS2DT, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_RTS2IO | SCIFA_SPTR_RTS2DT);
    }
    else
    {
        /* set RTS2DT low and RTS2IO high */
        RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SPTR.WORD, SCIFA_SPTR_RTS2IO, IOREG_NONSHIFT_ACCESS, SCIFA_SPTR_RTS2IO | SCIFA_SPTR_RTS2DT);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetRTSOutput
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_GetVersion - Get version information
 *
 * @param[out] pVerInfo: version info
 *
 * @retval  0 DRV_SUCCESS (always)
 */
int_t R_SCIFA_GetVersion(st_ver_info_t *pVerInfo)
{
    pVerInfo->lld.p_szdriver_name = STDIO_SCIFA_RZ_LLD_DRV_NAME;
    pVerInfo->lld.version.sub.major = STDIO_SCIFA_RZ_LLD_VERSION_MAJOR;
    pVerInfo->lld.version.sub.minor = STDIO_SCIFA_RZ_LLD_VERSION_MINOR;
    pVerInfo->lld.build = STDIO_SCIFA_RZ_LLD_BUILD_NUM;

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_GetVersion
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_SetupInterrupts - configures interrupts
 *
 * @param[in] channel: the channel number
 * @param[in] txi_priority: TXI interrupt priority
 * @param[in] txei_priority: TXI interrupt priority
 * @param[in] rxi_priority: RXI interrupt priority
 * @param[in] txi_handler: TXI interrupt handler function
 * @param[in] txei_handler: TXEI interrupt handler function
 * @param[in] rxi_handler: RXI interrupt handler function
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetupInterrupts(r_channel_t channel, e_r_drv_intc_priority_t txi_priority, e_r_drv_intc_priority_t txei_priority, e_r_drv_intc_priority_t rxi_priority,
        void (*txi_handler)(uint32_t), void (*txei_handler)(uint32_t), void (*rxi_handler)(uint32_t))
{
    /* set up interrupts */
    switch (channel)
    {
        case R_PRV_SCIFA_CHANNEL_0:
        {
            /* transmit */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_TXI0, txi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_TXI0);
            R_INTC_SetPriority(INTC_ID_SCIFA_TXI0, txi_priority);

            /* transmit end */
            if (NULL != txei_handler)
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI0DRI0, txei_handler);
                R_INTC_Enable(INTC_ID_SCIFA_TEI0DRI0);
                R_INTC_SetPriority(INTC_ID_SCIFA_TEI0DRI0, txei_priority);
            }
            else
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI0DRI0, NULL);
                R_INTC_Disable(INTC_ID_SCIFA_TEI0DRI0);
            }

            /* receive */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_RXI0, rxi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_RXI0);
            R_INTC_SetPriority(INTC_ID_SCIFA_RXI0, rxi_priority);
            break;
        }

        case R_PRV_SCIFA_CHANNEL_1:
        {
            /* transmit */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_TXI1, txi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_TXI1);
            R_INTC_SetPriority(INTC_ID_SCIFA_TXI1,  txi_priority);

            /* transmit end */
            if (NULL != txei_handler)
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI1DRI1, txei_handler);
                R_INTC_Enable(INTC_ID_SCIFA_TEI1DRI1);
                R_INTC_SetPriority(INTC_ID_SCIFA_TEI1DRI1, txei_priority);
            }
            else
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI1DRI1, NULL);
                R_INTC_Disable(INTC_ID_SCIFA_TEI1DRI1);
            }

            /* receive */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_RXI1, rxi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_RXI1);
            R_INTC_SetPriority(INTC_ID_SCIFA_RXI1, rxi_priority);
            break;
        }

        case R_PRV_SCIFA_CHANNEL_2:
        {
            /* transmit */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_TXI2, txi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_TXI2);
            R_INTC_SetPriority(INTC_ID_SCIFA_TXI2,  txi_priority);

            /* transmit end */
            if (NULL != txei_handler)
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI2DRI2, txei_handler);
                R_INTC_Enable(INTC_ID_SCIFA_TEI2DRI2);
                R_INTC_SetPriority(INTC_ID_SCIFA_TEI2DRI2, txei_priority);
            }
            else
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI2DRI2, NULL);
                R_INTC_Disable(INTC_ID_SCIFA_TEI2DRI2);
            }

            /* receive */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_RXI2, rxi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_RXI2);
            R_INTC_SetPriority(INTC_ID_SCIFA_RXI2, rxi_priority);
            break;
        }

        case R_PRV_SCIFA_CHANNEL_3:
        {
            /* transmit */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_TXI3, txi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_TXI3);
            R_INTC_SetPriority(INTC_ID_SCIFA_TXI3,  txi_priority);

            /* transmit end */
            if (NULL != txei_handler)
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI3DRI3, txei_handler);
                R_INTC_Enable(INTC_ID_SCIFA_TEI3DRI3);
                R_INTC_SetPriority(INTC_ID_SCIFA_TEI3DRI3, txei_priority);
            }
            else
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI3DRI3, NULL);
                R_INTC_Disable(INTC_ID_SCIFA_TEI3DRI3);
            }

            /* receive */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_RXI3, rxi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_RXI3);
            R_INTC_SetPriority(INTC_ID_SCIFA_RXI3, rxi_priority);
            break;
        }

        case R_PRV_SCIFA_CHANNEL_4:
        {
            /* transmit */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_TXI4, txi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_TXI4);
            R_INTC_SetPriority(INTC_ID_SCIFA_TXI4, txi_priority);

            /* transmit end */
            if (NULL != txei_handler)
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI4DRI4, txei_handler);
                R_INTC_Enable(INTC_ID_SCIFA_TEI4DRI4);
                R_INTC_SetPriority(INTC_ID_SCIFA_TEI4DRI4, txei_priority);
            }
            else
            {
                R_INTC_RegistIntFunc(INTC_ID_SCIFA_TEI4DRI4, NULL);
                R_INTC_Disable(INTC_ID_SCIFA_TEI4DRI4);
            }

            /* receive */
            R_INTC_RegistIntFunc(INTC_ID_SCIFA_RXI4, rxi_handler);
            R_INTC_Enable(INTC_ID_SCIFA_RXI4);
            R_INTC_SetPriority(INTC_ID_SCIFA_RXI4, rxi_priority);
            break;
        }

        default:
        {
            return (DRV_ERROR);
        }
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_SetupInterrupts
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_StartModule - start or stop SCIFA module
 *
 * @param[in] channel: the channel number
 * @param[in] start: true to start channel, or false to stop it
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartModule(r_channel_t channel, bool_t start)
{
    e_stb_module_t module[] = {MODULE_SCIFA0, MODULE_SCIFA1, MODULE_SCIFA2, MODULE_SCIFA3, MODULE_SCIFA4};

    if ((channel > R_PRV_CFG_SCIF_LAST_CHANNEL) || (channel < 0))
    {
        return (DRV_ERROR);
    }

    if (start)
    {
        R_STB_StartModule(module[channel]);
    }
    else
    {
        R_STB_StopModule(module[channel]);
    }

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_StartModule
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_DisableTxInterrupt - disable transmit interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_DisableTxInterrupt(r_channel_t channel)
{
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_DisableTxInterrupt
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_EnableTxInterrupt - enable transmit interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_EnableTxInterrupt(r_channel_t channel)
{
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 1, SCIFA_SCR_TIE_SHIFT, SCIFA_SCR_TIE);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_EnableTxInterrupt
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_DisableRxInterrupt - disable receive interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_DisableRxInterrupt(r_channel_t channel)
{
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 0, SCIFA_SCR_RIE_SHIFT, SCIFA_SCR_RIE);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_DisableRxInterrupt
 *********************************************************************************************************************/

/**
 * @brief R_SCIFA_EnableRxInterrupt - enable receive interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_EnableRxInterrupt(r_channel_t channel)
{
    RZA_IO_RegWrite_16((volatile uint16_t *) &gsp_scifa[channel]->SCR.WORD, (uint16_t) 1, SCIFA_SCR_RIE_SHIFT, SCIFA_SCR_RIE);
}
/**********************************************************************************************************************
 * End of Function R_SCIFA_EnableRxInterrupt
 *********************************************************************************************************************/

/* End of File */
