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
 * File Name    : r_scifa_hld_prv.c
 * Description  : SCIFA driver support for all available SCIFA channels
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 15.05.2018 1.00    Release
 *              : 10.05.2019 1.01    Fixed issue with channel 0 Baud rate setting
 *              : 23.05.2019 1.02    Restructuring to ensure that other LLDs are only accessed from the SCIFA LLD
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <unistd.h>
#include <string.h>

#include "r_os_abstraction_api.h"
#include "r_compiler_abstraction_api.h"

/* Driver Includes */
#include "r_scifa_drv_api.h"     /* SCIFA driver API Header */
#include "r_scifa_drv_link.h"    /* link include to Low Layer Driver */
#include "r_scifa_drv_sc_cfg.h"  /* Smart Configurator set up */
#include "r_scifa_hld_prv.h"     /* private HLD functions */
#include "r_intc_drv_link.h"
#include "driver.h"              /* device driver common header */

/* Middleware */
#include "r_cbuffer.h"

/* Temporary measure to allow for the detection of inclusion of DMA driver */
/* This #include should be removed when r_scifa_drv_sc_cfg.h can conditionally include the DMA API */
#ifndef R_DRV_DMAC_HLD_VERSION_MAJOR
    #include "r_devlink_wrapper_cfg.h"
#endif

#ifdef R_DRV_DMAC_HLD_VERSION_MAJOR
    #if (R_DRV_DMAC_HLD_VERSION_MAJOR >= 2)
        #define R_DRV_DMAC_PRV_ENABLE
    #endif
#endif

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/


typedef struct
{
    scifa_config_t          cfg;                    /*!< Configuration struct */
    st_scifa_rx_error_t     rx_err;                 /*!< Current State of RX */
    scifa_tx_status_t       tx_state;               /*!< Current State of TX */
    st_pcbuff_t             p_rx_cbuff;             /*!< Pointer to RX Buffer */
    st_pcbuff_t             p_tx_cbuff;             /*!< Pointer to TX Buffer */
    void *                  tx_mutex;               /*!< transmit mutex */
    void *                  rx_mutex;               /*!< receive mutex */
    void *                  control_mutex;          /*!< control mutex */
    int32_t                 open_count;             /*!< open count */
    int32_t                 buffer_overflow_count;  /*!< receive buffer overflow count */
    int_t                   dma_driver_handle_tx;   /*!< DMA driver handle for transmit */
    int_t                   dma_driver_handle_rx;   /*!< DMA driver handle for receive */
    int_t                   file_mode;              /*!< read / write access flags */
    r_sc_index_t            sc_config_index;        /*!< smart configuration index */
} st_r_scifa_hld_ch_t;

/**********************************************************************************************************************
 Private Functions
 *********************************************************************************************************************/
static int_t r_scifa_hld_prv_create_buffers(r_channel_t channel, scifa_config_t * p_config, int_t file_mode);
static void r_scifa_hld_prv_destroy_buffers(r_channel_t channel);
static int32_t r_scifa_hld_prv_read_core(r_channel_t channel, uint8_t *p_buffer, int32_t max_count);
static int32_t start_tx(r_channel_t channel, const uint8_t *p_buffer, int32_t ui_count);
static void TXEI_handler(uint32_t sense);
static void TXI_handler(uint32_t sense);
static void RXI_handler(uint32_t sense);

static int32_t r_scifa_hld_prv_dma_write(r_channel_t channel, const uint8_t *p_buffer, int32_t count);
static int32_t r_scifa_hld_prv_dma_read(r_channel_t channel, uint8_t *p_buffer, int32_t max_count);
static int_t r_scifa_hld_prv_configure_dma(r_channel_t channel, const scifa_config_t *p_configuration);

static void r_scifa_hld_prv_direct_write(r_channel_t channel, const uint8_t *p_buffer, int32_t count);

/* channel control */
static st_r_scifa_hld_ch_t gs_ch_ctrl[SCIFA_CFG_LLD_NUM_CHANNELS];


/**
 * @brief r_scifa_hld_prv_create_buffers creates buffers for transmit and receive
 *
 * @param[in] channel: Channel to create buffers for
 * @param[in] p_config: Pointer to SCIFA channel configuration
 * @param[in] file_mode: File mode for R/W access
 *
 * @retval 0  Success
 * @retval -1 Fail
 */
static int_t r_scifa_hld_prv_create_buffers(r_channel_t channel, scifa_config_t * p_config, int_t file_mode)
{
    /* if the write attribute is set and we're in interrupt mode, then allocate the transmit buffer */
    if (((file_mode & __SWR) > 0) && (SCIFA_TX_INTERRUPT_MODE == p_config->tx_dt_mode))
    {
        /* allocate transmit buffer */
        if (NULL == gs_ch_ctrl[channel].p_tx_cbuff)
        {
            gs_ch_ctrl[channel].p_tx_cbuff = cbCreate(p_config->transmit_buffer_length);

            if (NULL == gs_ch_ctrl[channel].p_tx_cbuff)
            {
                return (DRV_ERROR);
            }

            /* set transmit to idle state */
            gs_ch_ctrl[channel].tx_state = SCIFA_TX_STATUS_IDLE;
        }
    }

    /* if the read attribute is set and we're in interrupt mode, then allocate the receive buffer */
    if (((file_mode & __SRD) > 0) && (SCIFA_RX_INTERRUPT_MODE == p_config->rx_dt_mode))
    {
        /* allocate receive buffer */
        if (NULL == gs_ch_ctrl[channel].p_rx_cbuff)
        {
            gs_ch_ctrl[channel].p_rx_cbuff = cbCreate(p_config->receive_buffer_length);

            if (NULL == gs_ch_ctrl[channel].p_rx_cbuff)
            {
                if (NULL != gs_ch_ctrl[channel].p_tx_cbuff)
                {
                    /* we failed to create the receive buffer - so tidy up */
                    cbDestroy((void *) &gs_ch_ctrl[channel].p_tx_cbuff);
                    gs_ch_ctrl[channel].p_tx_cbuff = NULL;
                }

                return (DRV_ERROR);
            }
        }
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_create_buffers
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_destroy_buffers destroys transmit and receive buffers
 *
 * @param[in]  channel: Channel
 */
static void r_scifa_hld_prv_destroy_buffers(r_channel_t channel)
{
    if (NULL != gs_ch_ctrl[channel].p_tx_cbuff)
    {
        /* delete transmit buffer */
        cbDestroy(gs_ch_ctrl[channel].p_tx_cbuff);
        gs_ch_ctrl[channel].p_tx_cbuff = NULL;
    }

    if (NULL != gs_ch_ctrl[channel].p_rx_cbuff)
    {
        /* delete receive buffer */
        cbDestroy(gs_ch_ctrl[channel].p_rx_cbuff);
        gs_ch_ctrl[channel].p_rx_cbuff = NULL;
    }
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_destroy_buffers
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_open opens the SCIFA device driver
 * Configures the SCIFA channel, transmission and reception buffer depending on
 * the Smart Configurator
 *
 * @param[in] sc_config_index: Smart Configuration index to open
 * @param[in] file_mode: File mode for R/W access
 *
 * @retval 0  Success
 * @retval -1 Fail
 */
int_t r_scifa_hld_prv_open(r_sc_index_t sc_config_index, int_t file_mode)
{
    int_t ret = DRV_ERROR;
    r_channel_t channel;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* create (if it hasn't been created yet) and acquire the control mutex */
    R_OS_EnterCritical();
    if (NULL == gs_ch_ctrl[channel].control_mutex)
    {
        gs_ch_ctrl[channel].control_mutex = R_OS_MutexCreate();
    }
    R_OS_ExitCritical();

    R_OS_MutexAcquire(gs_ch_ctrl[channel].control_mutex);

    if (0 == gs_ch_ctrl[channel].open_count)
    {
        gs_ch_ctrl[channel].file_mode = file_mode;

        /* open the specified configuration, this will initialise the channel with configured settings */
        ret = R_SCIFA_Open(sc_config_index, &gs_ch_ctrl[channel].cfg);

        /* configure interrupts */
        if (DRV_SUCCESS == ret)
        {
            /* only enable the transmit end interrupt handler in DMA mode */
            void ((*txei_handler)(uint32_t)) =
                                (SCIFA_TX_DMA_MODE == gs_ch_ctrl[channel].cfg.tx_dt_mode) ? TXEI_handler : NULL;

            ret = R_SCIFA_SetupInterrupts(channel, gs_ch_ctrl[channel].cfg.txi_priority, gs_ch_ctrl[channel].cfg.txi_priority,
                    gs_ch_ctrl[channel].cfg.rxi_priority, TXI_handler, txei_handler, RXI_handler);
        }

        /* start channel if successful */
        if (DRV_SUCCESS == ret)
        {
            /* create mutexes */
            gs_ch_ctrl[channel].tx_mutex = R_OS_MutexCreate();
            gs_ch_ctrl[channel].rx_mutex = R_OS_MutexCreate();

            /* store the configuration index for this first open so that we can prevent subsequent */
            /* openings that pass in a different configuration */
            gs_ch_ctrl[channel].sc_config_index = sc_config_index;

            ret = r_scifa_hld_prv_create_buffers(channel, &gs_ch_ctrl[channel].cfg, file_mode);

            if (DRV_SUCCESS == ret)
            {
                /* Start channel */
                ret = R_SCIFA_Start(channel, file_mode);
            }
        }
    }
    else
    {
        /* check that this is the same configuration that was first opened */
        if (sc_config_index != gs_ch_ctrl[channel].sc_config_index)
        {
            ret = DRV_ERROR;
        }
        else
        {
            /* merge read / write access for all handles */
            gs_ch_ctrl[channel].file_mode |= file_mode;

            /* allocate buffers if needed by different access flags */
            ret = r_scifa_hld_prv_create_buffers(channel, &gs_ch_ctrl[channel].cfg, file_mode);
        }
    }

    /* open the DMA driver if configured */
    if (DRV_SUCCESS == ret)
    {
        r_scifa_hld_prv_configure_dma(channel, &gs_ch_ctrl[channel].cfg);
    }

    /* if we were successful then update the open counter */
    if (DRV_SUCCESS == ret)
    {
        gs_ch_ctrl[channel].open_count++;
    }

    R_OS_MutexRelease(gs_ch_ctrl[channel].control_mutex);

    /* return status */
    return (ret);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_open
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_close closes the SCIFA device driver
 * The close function destroys the resources used in opening a channel
 *
 * @param[in] sc_config_index: Smart Configuration index to close
 *
 * @return None
 */
void r_scifa_hld_prv_close(r_sc_index_t sc_config_index)
{
    r_channel_t channel;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return;
    }

    /* if the mutex has been destroyed, then we must already be closed */
    if (0 == gs_ch_ctrl[channel].control_mutex)
    {
        return;
    }

    R_OS_MutexAcquire(gs_ch_ctrl[channel].control_mutex);

    gs_ch_ctrl[channel].open_count--;

    if (0 == gs_ch_ctrl[channel].open_count)
    {
        /* stop transmit and receive */
        R_SCIFA_Stop(channel);

        /* stop the SCIFA module */
        R_SCIFA_StartModule(channel, false);

        /* destroy the circular buffers */
        r_scifa_hld_prv_destroy_buffers(channel);

        if (NULL != gs_ch_ctrl[channel].tx_mutex)
        {
            /* delete transmit mutex */
            R_OS_MutexDelete(&gs_ch_ctrl[channel].tx_mutex);
            gs_ch_ctrl[channel].tx_mutex = NULL;
        }

        if (NULL != gs_ch_ctrl[channel].rx_mutex)
        {
            /* delete receive mutex */
            R_OS_MutexDelete(&gs_ch_ctrl[channel].rx_mutex);
            gs_ch_ctrl[channel].rx_mutex = NULL;
        }

        /* close the transmit DMA driver handle if it was opened */
        if (gs_ch_ctrl[channel].dma_driver_handle_tx > 0)
        {
            close(gs_ch_ctrl[channel].dma_driver_handle_tx);
            gs_ch_ctrl[channel].dma_driver_handle_tx = -1;
        }

        /* close the receive DMA driver handle if it was opened */
        if (gs_ch_ctrl[channel].dma_driver_handle_rx > 0)
        {
            close(gs_ch_ctrl[channel].dma_driver_handle_rx);
            gs_ch_ctrl[channel].dma_driver_handle_rx = -1;
        }

        R_OS_EnterCritical();
        R_OS_MutexDelete(&gs_ch_ctrl[channel].control_mutex);
        gs_ch_ctrl[channel].control_mutex = 0;
        R_OS_ExitCritical();
    }
    else
    {
        R_OS_MutexRelease(gs_ch_ctrl[channel].control_mutex);
    }
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_close
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_dma_read performs a read using DMA
 *
 * Starts a DMA read operation
 *
 * @param[in] channel: Channel to use
 * @param[out] p_buffer: Address to store received data
 * @param[out] max_count: The number of bytes to receive
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
static int32_t r_scifa_hld_prv_dma_read(r_channel_t channel, uint8_t *p_buffer, int32_t max_count)
{
    int_t result = DRV_ERROR;

#ifdef R_DRV_DMAC_PRV_ENABLE
    static st_r_drv_dmac_config_t dma_config;

    /* check if the DMA driver is open */
    if (gs_ch_ctrl[channel].dma_driver_handle_rx < 0)
    {
        return (DRV_ERROR);
    }

    R_SCIFA_StopRX(channel);

    /* get the configuration, this will have been set by Smart Configurator */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_rx, CTL_DMAC_GET_CONFIGURATION, (void *) &dma_config);
    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* configure the DMA transfer */
    dma_config.config.p_dmaComplete = gs_ch_ctrl[channel].cfg.read_complete_callback;
    dma_config.config.p_dmaError = NULL;
    dma_config.config.source_address = R_SCIFA_GetReadDataRegisterAddress(channel);

    if (NULL != p_buffer)
    {
        dma_config.config.destination_address = (void *) p_buffer;
        dma_config.config.count = max_count;
    }

    result = control(gs_ch_ctrl[channel].dma_driver_handle_rx, CTL_DMAC_SET_CONFIGURATION, (void *) &dma_config);
    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* enable the DMA transfer */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_rx, CTL_DMAC_ENABLE, NULL);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    R_SCIFA_StartRX(channel);

    result = DRV_SUCCESS;

#else /* #ifdef R_DRV_DMAC_PRV_ENABLE */

    (void) channel;
    (void) p_buffer;
    (void) max_count;
#endif /* #ifdef R_DRV_DMAC_PRV_ENABLE */

    return (result);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_dma_read
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_dma_read_next
 *
 * Configures the read to be automatically executed as soon as the current
 * one completes
 *
 * @param[in] sc_config_index: Smart Configuration index
 * @param[in] read_next: addresses and data length for the next DMA read
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_read_next(r_sc_index_t sc_config_index, st_scifa_dma_rw_t *read_next)
{
    int_t result = DRV_ERROR;

#ifdef R_DRV_DMAC_PRV_ENABLE
    r_channel_t channel;
    st_r_drv_dmac_next_transfer_t dmac_next_transfer;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* check if the DMA driver is open */
    if (gs_ch_ctrl[channel].dma_driver_handle_rx < 0)
    {
        return (DRV_ERROR);
    }

    /* configure the next DMA transfer */
    dmac_next_transfer.source_address = R_SCIFA_GetReadDataRegisterAddress(channel);
    dmac_next_transfer.destination_address = (void *) read_next->buffer;
    dmac_next_transfer.count = read_next->length;

    /* set the DMA driver next transfer */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_rx, CTL_DMAC_NEXT_TRANSFER, (void *) &dmac_next_transfer);
    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = DRV_SUCCESS;

#else /* #ifdef R_DRV_DMAC_PRV_ENABLE */
    (void) sc_config_index;
    (void) read_next;
#endif /* #ifdef R_DRV_DMAC_PRV_ENABLE */

    return (result);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_dma_read_next
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_read_core reads data from the SCIFA
 * This function reads the data received from the chosen channel 
 * and stores it in the passed by reference buffer.
 *
 * @param[in] channel: Channel to read from
 * @param[out] *p_buffer: Buffer used to store data
 * @param[in] max_count: Max amount of bytes to be read
 *
 * @retval  N Where N is the number of bytes read
 * @retval -1 Fail
 */
static int32_t r_scifa_hld_prv_read_core(r_channel_t channel, uint8_t *p_buffer, int32_t max_count)
{
    int32_t stored_data_length;
    int32_t bytes_read = 0;
    uint8_t data;
    bool_t fifo_empty;

    /* control access by multiple threads */
    R_OS_MutexAcquire(gs_ch_ctrl[channel].rx_mutex);

    /* in DMA mode we need to configure a DMA transfer */
    if (SCIFA_RX_DMA_MODE == gs_ch_ctrl[channel].cfg.rx_dt_mode)
    {
        r_scifa_hld_prv_dma_read(channel, p_buffer, max_count);
        bytes_read = 0;
    }
    else
    {
        /* don't allow the receive interrupt routine to change the buffer while we're accessing it */
        R_SCIFA_DisableRxInterrupt(channel);

        stored_data_length = (int32_t) cbUsed(gs_ch_ctrl[channel].p_rx_cbuff);

        if (stored_data_length > 0)
        {
            /* transfer data from the receive buffer */
            for (bytes_read = 0; (bytes_read < stored_data_length) && (bytes_read < max_count); bytes_read++)
            {
                cbGet(gs_ch_ctrl[channel].p_rx_cbuff, &p_buffer[bytes_read]);
            }
        }

        /* if we haven't read as many characters as requested */
        if (bytes_read < max_count)
        {
            /* there may be a few characters in the FIFO that haven't reached the level that triggers a receive interrupt */
            do
            {
                R_SCIFA_IsRxFifoEmpty(channel, &fifo_empty);

                if (!fifo_empty)
                {
                    R_SCIFA_GetByte(channel, &data);
                    p_buffer[bytes_read] = data;
                    bytes_read++;
                }
            }
            while ((bytes_read < max_count) && (!fifo_empty));
        }
    }

    /* we've finished with the buffer */
    R_SCIFA_EnableRxInterrupt(channel);

    R_OS_MutexRelease(gs_ch_ctrl[channel].rx_mutex);

    return ((int_t) bytes_read);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_read_core
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_read reads data from the SCIFA
 * This function reads the data received from the chosen configuration index
 * and stores it in the passed by reference buffer.
 *
 * @param[in] sc_config_index: Smart Configuration index
 * @param[out] *p_buffer: Buffer used to store data
 * @param[in] max_count: Max amount of bytes to be read
 *
 * @retval  N Where N is the number of bytes read
 * @retval -1 Fail
 */
int32_t r_scifa_hld_prv_read(r_sc_index_t sc_config_index, uint8_t *p_buffer, int32_t max_count)
{
    int32_t ret;
    r_channel_t channel;
    bool_t looping;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* wait for data to be received if we're in blocking mode, otherwise return what we've got */
    do
    {
        ret = r_scifa_hld_prv_read_core(channel, p_buffer, max_count);

        /* return immediately if we're in DMA mode */
        if (SCIFA_RX_DMA_MODE == gs_ch_ctrl[channel].cfg.rx_dt_mode)
        {
            looping = false;
        }
        else
        {
            looping = ((0 == ret) && (gs_ch_ctrl[channel].cfg.read_blocking_mode == SCIFA_READ_BLOCKING_MODE_ENABLE));
        }

        if (looping)
        {
            R_OS_TaskSleep(1);
        }
    }
    while (looping);

    return (ret);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_read
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_dma_write writes data to the SCIFA using DMA
 * @param[in] channel: Channel to be written to
 * @param[in] p_buffer: Buffer of data to be written
 * @param[in] count: Number of bytes to be written
 * @retval >0 The number of bytes written
 * @retval -1 Fail
 */
static int32_t r_scifa_hld_prv_dma_write(r_channel_t channel, const uint8_t *p_buffer, int32_t count)
{
    int_t result = DRV_ERROR;

#ifdef R_DRV_DMAC_PRV_ENABLE
    static st_r_drv_dmac_config_t dma_config;

    /* check if the DMA driver is open */
    if (gs_ch_ctrl[channel].dma_driver_handle_tx < 0)
    {
        return (DRV_ERROR);
    }

    R_SCIFA_StopTX(channel);

    /* get the configuration, this will have been set by Smart Configurator */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_tx, CTL_DMAC_GET_CONFIGURATION, (void *) &dma_config);
    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* configure the DMA transfer */
    dma_config.config.p_dmaComplete = gs_ch_ctrl[channel].cfg.write_complete_callback;
    dma_config.config.p_dmaError = NULL;
    dma_config.config.destination_address = (void *) R_SCIFA_GetWriteDataRegisterAddress(channel);

    if (NULL != p_buffer)
    {
        dma_config.config.source_address = (void *) p_buffer;
        dma_config.config.count = count;
    }

    result = control(gs_ch_ctrl[channel].dma_driver_handle_tx, CTL_DMAC_SET_CONFIGURATION, (void *) &dma_config);
    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    /* enable the DMA transfer */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_tx, CTL_DMAC_ENABLE, NULL);

    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    R_SCIFA_StartTX(channel);

#else  /* #ifdef R_DRV_DMAC_PRV_ENABLE */
    (void) channel;
    (void) p_buffer;
    (void) count;
#endif /* #ifdef R_DRV_DMAC_PRV_ENABLE */

    return (count);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_dma_write
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_dma_write_next
 *
 * Configures the write to be automatically executed as soon as the current
 * one completes
 *
 * @param[in] sc_config_index: Smart Configuration index
 * @param[in] write_next: addresses and data length for the next DMA write
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_write_next(r_sc_index_t sc_config_index, st_scifa_dma_rw_t *write_next)
{
    int_t result = DRV_ERROR;

#ifdef R_DRV_DMAC_PRV_ENABLE

    volatile struct st_scifa *gsp_scifa[SCIFA_CFG_LLD_NUM_CHANNELS] = {&SCIFA0, &SCIFA1, &SCIFA2, &SCIFA3, &SCIFA4};
    r_channel_t channel;
    st_r_drv_dmac_next_transfer_t dmac_next_transfer;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* check if the DMA driver is open */
    if (gs_ch_ctrl[channel].dma_driver_handle_tx < 0)
    {
        return (DRV_ERROR);
    }

    /* configure the next DMA transfer */
    dmac_next_transfer.source_address = (void *) write_next->buffer;
    dmac_next_transfer.destination_address = (void *) &gsp_scifa[channel]->FTDR.BYTE;
    dmac_next_transfer.count = write_next->length;

    /* set the DMA driver next transfer */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_tx, CTL_DMAC_NEXT_TRANSFER, (void *) &dmac_next_transfer);
    if (DRV_SUCCESS != result)
    {
        return (result);
    }

    result = DRV_SUCCESS;

#else /* #ifdef R_DRV_DMAC_PRV_ENABLE */
    (void) sc_config_index;
    (void) write_next;
#endif /* #ifdef R_DRV_DMAC_PRV_ENABLE */

    return (result);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_dma_write_next
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_direct_write writes data directly to the TX FIFO
 *
 * @param[in] channel: channel to write to
 * @param[in] p_buffer: Buffer of data to be written
 * @param[in] count: Number of bytes to be written
 *
 * @return None
 */
static void r_scifa_hld_prv_direct_write(r_channel_t channel, const uint8_t *p_buffer, int32_t count)
{
    int32_t i;
    bool_t p_fifo_full;

    for (i = 0; i < count; i++)
    {
        do
        {
            R_SCIFA_IsTxFifoFull(channel, &p_fifo_full);
        }
        while (p_fifo_full);

        R_SCIFA_PutByte(channel, p_buffer[i]);
    }
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_direct_write
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_write writes data to the SCIFA device
 *
 * Contains a state machine to handle writing when the SCIFA,
 * is closed, Running, idle or broken
 *
 * @param[in] sc_config_index: Smart Configuration index to write to
 * @param[in] p_buffer: Buffer of data to be written
 * @param[in] ui_count: Number of bytes to be written
 *
 * @retval >0 The number of bytes written
 * @retval -1 Failed
 */
int32_t r_scifa_hld_prv_write(r_sc_index_t sc_config_index, const uint8_t *p_buffer, int32_t count)
{
    r_channel_t channel;
    int32_t ret = DRV_ERROR;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* check to see if the OS has started */
    /* if not we directly write to the FIFO and block until the last character to be sent has been queued in the FIFO */
    /* at this time there is only one thread, so no need to worry about thread safety */
    if (!R_OS_Running())
    {
    	r_scifa_hld_prv_direct_write(channel, p_buffer, count);
        return (count);
    }

    /* control access by multiple threads */
    R_OS_MutexAcquire(gs_ch_ctrl[channel].tx_mutex);

    /* in DMA mode we need to configure a DMA transfer */
    if (SCIFA_TX_DMA_MODE == gs_ch_ctrl[channel].cfg.tx_dt_mode)
    {
        ret = r_scifa_hld_prv_dma_write(channel, p_buffer, count);
    }
    else
    {
        switch (gs_ch_ctrl[channel].tx_state)
        {
            /* driver not open */
            case SCIFA_TX_STATUS_CLOSED:
            {
                ret = DRV_ERROR;
                break;
            }

            /* currently running */
            case SCIFA_TX_STATUS_RUNNING:
            {
                /* set to busy */
                gs_ch_ctrl[channel].tx_state = SCIFA_TX_STATUS_RUNNING;

                /* start TX */
                ret = (int32_t) start_tx(channel, p_buffer, count);
                break;
            }

            /* TX break */
            case SCIFA_TX_STATUS_BREAK:
            {
                R_COMPILER_Nop();
                break;
            }

            /* TX available */
            case SCIFA_TX_STATUS_IDLE:
            {
                /* set to busy */
                gs_ch_ctrl[channel].tx_state = SCIFA_TX_STATUS_RUNNING;

                /* start TX */
                ret = (int32_t) start_tx(channel, p_buffer, count);
                break;
            }

            default:
            {
                /* strange place */
                ret = DRV_ERROR;
                break;
            }
        }
    }

    R_OS_MutexRelease(gs_ch_ctrl[channel].tx_mutex);

    /* return the number of bytes written */
    if (DRV_SUCCESS == ret)
    {
    	ret = count;
    }

    return (ret);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_write
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_configure_dma performs any DMA configuration
 *
 * @param[in] channel: Channel to configure
 * @param[in] p_configuration: SCIFA configuration
 *
 * @retval 0  Success
 * @retval -1 Fail
 */
static int_t r_scifa_hld_prv_configure_dma(r_channel_t channel, const scifa_config_t *p_configuration)
{
    int_t result = DRV_SUCCESS;

#ifdef R_DRV_DMAC_PRV_ENABLE

    static char drv_name[DEVICE_MAX_STRING_SIZE];

    /* transmit DMA mode? */
    if (SCIFA_TX_DMA_MODE == p_configuration->tx_dt_mode)
    {
        if (gs_ch_ctrl[channel].dma_driver_handle_tx <= 0)
        {
            /* open the DMA driver */
            sprintf(drv_name, "%s%s", DEVICE_INDENTIFIER, p_configuration->dma_tx);
            gs_ch_ctrl[channel].dma_driver_handle_tx = open(drv_name, O_RDWR);

            if (gs_ch_ctrl[channel].dma_driver_handle_tx < 0)
            {
                result = DRV_ERROR;
            }
        }

        /* save the callback setting */
        gs_ch_ctrl[channel].cfg.write_complete_callback = p_configuration->write_complete_callback;
    }
    else
    {
        gs_ch_ctrl[channel].dma_driver_handle_tx = -1;
    }

    /* receive DMA mode? */
    if (SCIFA_RX_DMA_MODE == p_configuration->rx_dt_mode)
    {
        if (gs_ch_ctrl[channel].dma_driver_handle_rx <= 0)
        {
            /* open the DMA driver */
            sprintf(drv_name, "%s%s", DEVICE_INDENTIFIER, p_configuration->dma_rx);
            gs_ch_ctrl[channel].dma_driver_handle_rx = open(drv_name, O_RDWR);

            if (gs_ch_ctrl[channel].dma_driver_handle_rx < 0)
            {
                result = DRV_ERROR;
            }
        }

        /* save the callback setting */
        gs_ch_ctrl[channel].cfg.read_complete_callback = p_configuration->read_complete_callback;
    }
    else
    {
        gs_ch_ctrl[channel].dma_driver_handle_rx = -1;
    }

    /* save driver mode settings */
    gs_ch_ctrl[channel].cfg.tx_dt_mode = p_configuration->tx_dt_mode;
    gs_ch_ctrl[channel].cfg.rx_dt_mode = p_configuration->rx_dt_mode;
    gs_ch_ctrl[channel].cfg.dma_tx = p_configuration->dma_tx;
    gs_ch_ctrl[channel].cfg.dma_rx = p_configuration->dma_rx;

#else /* #ifdef R_DRV_DMAC_PRV_ENABLE */
    (void) channel;
    (void) p_configuration;
#endif /* #ifdef R_DRV_DMAC_PRV_ENABLE */

    return (result);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_configure_dma
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_set_config sets SCIFA configuration
 * This function configures and starts the SCIFA
 *
 * @param[in] sc_config_index: The index to configure
 * @param[in] p_config: The data to be used to configure the channel
 *
 * @retval 0  Success
 * @retval -1 Fail
 */
int_t r_scifa_hld_prv_set_config(r_sc_index_t sc_config_index, scifa_config_t * p_config)
{
    r_channel_t channel;
    int_t result;
    uint32_t achieved_baud_rate;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    R_OS_MutexAcquire(gs_ch_ctrl[channel].control_mutex);

    result = R_SCIFA_Stop(channel);

    if (DRV_SUCCESS == result)
    {
        /* buffer sizes may have changed - delete the old buffers and create new ones */
        r_scifa_hld_prv_destroy_buffers(channel);
        result = r_scifa_hld_prv_create_buffers(channel, p_config, gs_ch_ctrl[channel].file_mode);
    }

    if (DRV_SUCCESS == result)
    {
        /* call setup */
        result = R_SCIFA_ChannelConfigure(channel, p_config, &achieved_baud_rate);
    }

    if (DRV_SUCCESS == result)
    {
        /* if we were successful, then update the stored configuration data */
        memcpy(&gs_ch_ctrl[channel].cfg, p_config, sizeof(scifa_config_t));
        gs_ch_ctrl[channel].cfg.baud_rate = achieved_baud_rate;
    }

    if (DRV_SUCCESS == result)
    {
        /* perform any DMA configuration */
        result = r_scifa_hld_prv_configure_dma(channel, p_config);
    }

    if (DRV_SUCCESS == result)
    {
        /* only enable the transmit end interrupt handler in DMA mode */
        void (*txei_handler)(uint32_t) = (gs_ch_ctrl[channel].cfg.tx_dt_mode == SCIFA_TX_DMA_MODE) ? TXEI_handler : NULL;

        result = R_SCIFA_SetupInterrupts(channel, gs_ch_ctrl[channel].cfg.txi_priority, gs_ch_ctrl[channel].cfg.txi_priority,
                gs_ch_ctrl[channel].cfg.rxi_priority, TXI_handler, txei_handler, RXI_handler);
    }

    if (DRV_SUCCESS == result)
    {
        /* start channel */
        result = R_SCIFA_Start(channel, gs_ch_ctrl[channel].file_mode);
    }

    R_OS_MutexRelease(gs_ch_ctrl[channel].control_mutex);

    return (result);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_set_config
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_get_config Retrieves the current configuration 
 * of the SCIFA driver
 *
 * Allows for the reading of current SCIFA settings. This allows for safe
 * changes to the current configuration of the SCIFA
 *
 * @param[in] sc_config_index: The index to retrieve configuration for
 * @param[in] p_config: Configuration struct to populate with the retrieved configuration
 *
 * @retval  0 Success
 * @retval -1 Fail
 */
int_t r_scifa_hld_prv_get_config(r_sc_index_t sc_config_index, scifa_config_t* p_config)
{
    r_channel_t channel;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    R_OS_MutexAcquire(gs_ch_ctrl[channel].control_mutex);

    /* copy currently held settings for channel */
    memcpy(p_config, &gs_ch_ctrl[channel].cfg, sizeof(scifa_config_t));

    R_OS_MutexRelease(gs_ch_ctrl[channel].control_mutex);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_prv_get_config
 *********************************************************************************************************************/

/**
 * @brief start_tx starts data transmission
 *
 * @param[in] channel: Channel to send data
 * @param[in] p_buffer: Location of data to send
 * @param[in] ui_count: Length of data to send
 *
 * @retval  The number of bytes written
 */
static int32_t start_tx(r_channel_t channel, const uint8_t *p_buffer, int32_t ui_count)
{
    int32_t free_space;
    int32_t bytes_written;

    /* we mustn't be interrupted while manipulating the buffer */
    R_SCIFA_DisableTxInterrupt(channel);

    /* find out how much free space there is in the buffer */
    free_space = (int32_t) cbFree(gs_ch_ctrl[channel].p_tx_cbuff);

    /* copy as much of the data as possible into the transmit buffer */
    for (bytes_written = 0; (bytes_written < ui_count) && (bytes_written < free_space); bytes_written++)
    {
        cbPut(gs_ch_ctrl[channel].p_tx_cbuff, p_buffer[bytes_written]);
    }

    /* we're done with the buffer */
    R_SCIFA_EnableTxInterrupt(channel);

    /* set SCR.TIE bit to start transmission */
    R_SCIFA_StartTXI(channel);

    /* if write blocking mode is enabled, then block until there is enough space in the buffer */
    if (SCIFA_WRITE_BLOCKING_MODE_ENABLE == gs_ch_ctrl[channel].cfg.write_blocking_mode)
    {
        /* put the remaining data into the buffer as space becomes available */
        while (bytes_written < ui_count)
        {
            /* we mustn't be interrupted while manipulating the buffer */
            R_SCIFA_DisableTxInterrupt(channel);

            free_space = (int32_t) cbFree(gs_ch_ctrl[channel].p_tx_cbuff);

            if (free_space > 0)
            {
                /* fill up the free space */
                while ((bytes_written < ui_count) && (free_space > 0))
                {
                    cbPut(gs_ch_ctrl[channel].p_tx_cbuff, p_buffer[bytes_written]);
                    bytes_written++;
                    free_space--;
                }

                /* we're done with the buffer */
                R_SCIFA_EnableTxInterrupt(channel);

                /* just in case the buffer emptied completely */
                R_SCIFA_StartTXI(channel);
            }
            else
            {
                /* we're done with the buffer */
                R_SCIFA_EnableTxInterrupt(channel);

                /* hand over CPU cycles to other tasks while we wait */
                R_OS_TaskSleep(5);
            }
        }
    }

    return (bytes_written);
}
/**********************************************************************************************************************
 * End of Function start_tx
 *********************************************************************************************************************/

/**
 * @brief TXEI_handler handles TXEI interrupts
 *
 * @param[in] sense: Not used, but required by the INTC driver
 *
 * @return None
 */
static void TXEI_handler(uint32_t sense)
{
    (void) sense;
    r_channel_t channel;                            /* channel search index */
    r_channel_t active_int = -1;                    /* active interrupt channel */

    for (channel = 0; channel < SCIFA_CFG_LLD_NUM_CHANNELS; channel++)
    {
        if (SCIFA_INTERRUPTING == R_SCIFA_GetTXEIState(channel))
        {
            active_int = channel;
            break;
        }
    }

    if (active_int >= 0)
    {
        /* clears transmit and transmit end interrupt enable flags */
        R_SCIFA_StopTransmit(active_int);
    }
}
/**********************************************************************************************************************
 * End of Function TXEI_handler
 *********************************************************************************************************************/

/**
 * @brief TXI_handler handles TXI interrupts
 *
 * @param[in] sense: Not used, but required by the INTC driver
 *
 * @return None
 */
static void TXI_handler(uint32_t sense)
{
    (void) sense;
    r_channel_t channel;                            /* channel search index */
    scifa_interrupting_state_t channel_state;
    r_channel_t active_int = -1;                    /* active interrupt channel */
    uint8_t data_to_send;
    bool_t fifo_full;

    for (channel = 0; channel < SCIFA_CFG_LLD_NUM_CHANNELS; channel++)
    {
        /* exclude the channel from checking if it's in DMA mode */
        if (SCIFA_TX_DMA_MODE == gs_ch_ctrl[channel].cfg.tx_dt_mode)
        {
            continue;
        }

        /* get channel state */
        channel_state = R_SCIFA_GetTXIState(channel);

        /* check if interrupt flag is high */
        if (SCIFA_INTERRUPTING == channel_state)
        {
            /* assign active interrupt */
            active_int = channel;

            /* break out of loop */
            break;
        }
    }

    if (active_int < 0)
    {
        /* no interrupt found */
        return;
    }

    /* clear the interrupt flags */
    R_SCIFA_ClearTXIFlags(active_int);

    /* while the FIFO is not full, and we still have data to transmit */
    while ((R_SCIFA_IsTxFifoFull(active_int, &fifo_full) == DRV_SUCCESS) && (!fifo_full) && (cbUsed(gs_ch_ctrl[active_int].p_tx_cbuff) > 0))
    {
        /* Retrieve byte from buffer */
        if (cbGet(gs_ch_ctrl[active_int].p_tx_cbuff, &data_to_send))
        {
            /* Send a byte (write to FIFO) */
            R_SCIFA_PutByte(active_int, data_to_send);
        }
    }

    /* Check if data transfer finished */
    if (0 == cbUsed(gs_ch_ctrl[active_int].p_tx_cbuff))
    {
        /* Stop TX */
        R_SCIFA_StopTXI(active_int);

        /* Return to Idle */
        gs_ch_ctrl[active_int].tx_state = SCIFA_TX_STATUS_IDLE;
    }
}
/**********************************************************************************************************************
 * End of Function TXI_handler
 *********************************************************************************************************************/

/**
 * @brief RXI_handler handles RXI interrupts
 *
 * @param[in] sense: Not used, but required by the INTC driver
 *
 * @return None
 */
static void RXI_handler(uint32_t sense)
{
    (void) sense;
    r_channel_t channel;                                /* channel search index */
    scifa_interrupting_state_t channel_state;
    int8_t active_int = -1;                             /* active interrupt channel */
    uint8_t received_data;
    bool_t fifo_empty;

    for (channel = 0; channel < SCIFA_CFG_LLD_NUM_CHANNELS; channel++)
    {
        /* get channel state */
        channel_state = R_SCIFA_GetRXIState(channel);

        /* check if interrupt flag is high */
        if (SCIFA_INTERRUPTING == channel_state)
        {
            /* assign active interrupt */
            active_int = channel;

            /* break out of loop */
            break;
        }
    }

    if (active_int < 0)
    {
        /* no interrupt found */
        return;
    }

    /* check error RX flags for channel */
    if (R_SCIFA_CheckRXIError(active_int, &gs_ch_ctrl[active_int].rx_err) == DRV_SUCCESS)
    {
        /* transfer data from the FIFO to the receive buffer */
        while ((R_SCIFA_IsRxFifoEmpty(active_int, &fifo_empty) == DRV_SUCCESS) && (!fifo_empty))
        {
            /* retrieve byte from FIFO */
            if (R_SCIFA_GetByte(active_int, &received_data) == DRV_SUCCESS)
            {
                /* if there's any space in the buffer, store the character, otherwise lose it */
                if (cbFree(gs_ch_ctrl[active_int].p_rx_cbuff) > 0)
                {
                    /* Store byte in buffer */
                    cbPut(gs_ch_ctrl[active_int].p_rx_cbuff, received_data);
                }
                else
                {
                    gs_ch_ctrl[active_int].buffer_overflow_count++;
                }
            }
        }
    }

    /* Clear RX Flags */
    R_SCIFA_ClearRXIFlags(active_int);
}
/**********************************************************************************************************************
 * End of Function RXI_handler
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_dma_read_abort aborts a DMA read
 *
 * @param[in] sc_config_index: Smart Configuration index to abort
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_read_abort(r_sc_index_t sc_config_index)
{
    int_t result = DRV_ERROR;

#ifdef R_DRV_DMAC_PRV_ENABLE

    uint32_t remaining;
    r_channel_t channel;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* check if the DMA driver is open */
    if (gs_ch_ctrl[channel].dma_driver_handle_rx < 0)
    {
        return (DRV_ERROR);
    }

    /* call disable on the DMA driver to abort the transfer */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_rx, CTL_DMAC_DISABLE, &remaining);

#else
    (void) sc_config_index;
#endif /* ifdef R_DRV_DMAC_PRV_ENABLE */

    return (result);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_dma_read_abort
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_dma_write_abort aborts a DMA write
 *
 * @param[in] sc_config_index: Smart Configuration index to abort
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_write_abort(r_sc_index_t sc_config_index)
{
    int_t result = DRV_ERROR;

#ifdef R_DRV_DMAC_PRV_ENABLE

    uint32_t remaining;
    r_channel_t channel;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* check if the DMA driver is open */
    if (gs_ch_ctrl[channel].dma_driver_handle_tx < 0)
    {
        return (DRV_ERROR);
    }

    /* call disable on the DMA driver to abort the transfer */
    result = control(gs_ch_ctrl[channel].dma_driver_handle_tx, CTL_DMAC_DISABLE, &remaining);

#else
    (void) sc_config_index;
#endif /* ifdef R_DRV_DMAC_PRV_ENABLE */

    return (result);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_dma_write_abort
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_tx_flush waits for all data in the transmit buffer to have been transmitted
 *
 * @param[in] sc_config_index: Smart Configuration index to flush
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_tx_flush(r_sc_index_t sc_config_index)
{
    r_channel_t channel;
    size_t remaining_bytes;

    /* get the channel for this configuration */
    channel = R_SCIFA_GetChannel(sc_config_index);

    if (DRV_ERROR == channel)
    {
        return (DRV_ERROR);
    }

    /* wait for the transmit buffer to become empty */
    do
    {
        /* we mustn't be interrupted while manipulating the buffer */
        R_SCIFA_DisableTxInterrupt(channel);

        /* find out how much free space there is in the buffer */
        remaining_bytes = cbUsed(gs_ch_ctrl[channel].p_tx_cbuff);

        /* we're done with the buffer */
        R_SCIFA_EnableTxInterrupt(channel);

        if (remaining_bytes > 0)
        {
            R_OS_TaskSleep(1);
        }
    }
    while (remaining_bytes > 0);

    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function r_scifa_hld_tx_flush
 *********************************************************************************************************************/
