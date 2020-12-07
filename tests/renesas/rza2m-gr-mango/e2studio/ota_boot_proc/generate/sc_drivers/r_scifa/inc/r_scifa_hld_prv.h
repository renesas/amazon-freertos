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
 * @headerfile     r_scifa_hld_prv.h
 * @brief          SCIFA High Layer Driver private header
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History       : DD.MM.YYYY Version Description
 *              : 15.05.2018 1.00    Release
 *              : 23.05.2019 1.01    Restructuring to ensure that other LLDs are only accessed from the SCIFA LLD
 *********************************************************************************************************************/
#include "r_scifa_drv_sc_cfg.h"     /* Smart Configurator Settings */

#ifndef SRC_RENESAS_AUTOMATED_TEST_R_SCIFA_INC_R_SCIFA_HLD_PRV_H_
#define SRC_RENESAS_AUTOMATED_TEST_R_SCIFA_INC_R_SCIFA_HLD_PRV_H_

/**********************************************************************************************************************
 Function prototypes
 *********************************************************************************************************************/

/**
 * @brief r_scifa_hld_prv_open opens the SCIFA device driver.
 *
 * Configures the SCIFA channel, transmission and reception buffer depending on
 * the Smart Configurator
 *
 * @param[in]  sc_config_index: Configuration index to open
 * @param[in]  file_mode: File mode for R/W access
 *
 * @retval 0  Success
 * @retval -1 Fail
 */
int_t r_scifa_hld_prv_open(r_sc_index_t sc_config_index, int_t file_mode);

/**
 * @brief r_scifa_hld_prv_close closes the SCIFA device driver
 *
 * The close function destroys the resources used in opening
 * a channel
 *
 * @param[in] sc_config_index: Configuration index to close
 *
 * @return None.
 *
*/
void  r_scifa_hld_prv_close(r_sc_index_t sc_config_index);

/**
 * @brief r_scifa_hld_prv_read reads data from the SCIFA
 *
 * This function reads the data received from the chosen channel
 * and stores it in the passed by reference buffer
 *
 * @param[in] sc_config_index: Configuration index to read from
 * @param[out] p_buffer: Buffer used to store data
 * @param[in] max_count: Max amount of bytes to be read
 *
 * @retval  N Where N is the number of bytes read
 * @retval -1 Fail
*/
int32_t r_scifa_hld_prv_read(r_sc_index_t sc_config_index, uint8_t *p_buffer, int32_t max_count);

/**
 * @brief r_scifa_hld_prv_write writes data to the SCIFA device
 *
 * Contains a state machine to handle writing when the SCIFA,
 * is closed, Running, idle or broken
 *
 * @param[in] sc_config_index: Configuration index to write to
 * @param[in] p_buffer: Buffer of data to be written
 * @param[in] ui_count: Number of bytes to be written
 *
 * @retval >0 The number of bytes written
 * @retval -1 Failed
 */
int32_t r_scifa_hld_prv_write(r_sc_index_t sc_config_index, const uint8_t *p_buffer, int32_t ui_count);

/**
 * @brief r_scifa_hld_prv_set_config sets SCIFA configuration
 *
 * This function configures and starts the SCIFA
 *
 * @param[in] sc_config_index: Configuration index to configure
 * @param[in] p_config: The data to be used to configure the channel
 *
 * @retval 0  Success
 * @retval -1 Fail
 */
int_t r_scifa_hld_prv_set_config(r_sc_index_t sc_config_index, scifa_config_t* p_config);

/**
 * @brief r_scifa_hld_prv_get_config Retrieves the current configuration
 * of the SCIFA driver
 *
 * Allows for the reading of current SCIFA settings. This allows for safe
 * changes to the current configuration of the SCIFA
 *
 * @param[in] sc_config_index: Configuration index to get configuration for
 * @param[out] p_config: Configuration struct to populate with
 *                       retrieved configuration
 *
 * @retval  0 Success
 * @retval -1 Fail
*/
int_t r_scifa_hld_prv_get_config(r_sc_index_t sc_config_index, scifa_config_t* p_config);

/**
 * @brief r_scifa_hld_tx_flush waits for all data in the transmit buffer to have been transmitted
 *
 * @param[in] sc_config_index: Configuration index to flush
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_tx_flush(r_sc_index_t sc_config_index);

/**
 * @brief r_scifa_hld_dma_read_abort aborts a DMA read
 *
 * @param[in] sc_config_index: Configuration index to abort
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_read_abort(r_sc_index_t sc_config_index);

/**
 * @brief r_scifa_hld_dma_write_abort aborts a DMA write
 *
 * @param[in] sc_config_index: Configuration index to abort
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_write_abort(r_sc_index_t sc_config_index);

/**
 * @brief r_scifa_hld_dma_read_next
 *
 * Configures the read to be automatically executed as soon as the current
 * one completes
 *
 * @param[in] sc_config_index: Configuration index
 * @param[in] read_next: addresses and data length for the next DMA read
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_read_next(r_sc_index_t sc_config_index, st_scifa_dma_rw_t *read_next);

/**
 * @brief r_scifa_hld_dma_write_next
 *
 * Configures the write to be automatically executed as soon as the current
 * one completes
 *
 * @param[in] sc_config_index: Configuration index
 * @param[in] write_next: addresses and data length for the next DMA write
 *
 * @return DRV_ERROR or DRV_SUCCESS
 */
int_t r_scifa_hld_dma_write_next(r_sc_index_t sc_config_index, st_scifa_dma_rw_t *write_next);

#endif /* SRC_RENESAS_AUTOMATED_TEST_R_SCIFA_INC_R_SCIFA_HLD_PRV_H_ */
