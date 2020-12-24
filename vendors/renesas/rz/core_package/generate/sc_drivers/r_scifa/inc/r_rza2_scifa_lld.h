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
 * @headerfile     r_rza2_scifa_lld.h
 * @brief          SCIFA Low Layer Driver interface header
 * H/W Platform    RZ/A2M
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 15.05.2018 1.00    Release
 *              : 23.05.2019 1.01    Restructuring to ensure that other LLDs are only accessed from the SCIFA LLD
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes
 *********************************************************************************************************************/
#include "driver.h"
#include "r_rza2_scifa_lld_api.h"
#include "r_intc_drv_api.h"

#ifndef R_SCIFA_INC_R_RZA2_SCIFA_LLD_H_
    #define R_SCIFA_INC_R_RZA2_SCIFA_LLD_H_
/*********************************************************************//**
 * @ingroup RENESAS_DRIVER_LIBRARY
 * @defgroup R_SCIFA_LLD_API_78 SCIFA RZA2 EBK_RZA2M Low Level Driver
 *
 * @section R_SCIFA_LLD_API_78_SUMMARY Summary
 * 
 * This SCIFA Low Level Driver is developed for the RZA2M EBK_RZA2M board.
 * 
 * The driver depends upon the common SSGSTARC-77 High Level Driver for SCIFA.
 *
 * This is a device and board specific component of an SCIFA driver, 
 * configured for the RZA2M EBK_RZA2M board. This interface is not intended to be
 * used directly without the associated High Level Driver.
 * 
 * @section R_SCIFA_LLD_API_78_INSTANCES Known Implementations:
 * This driver is used in the RZA2M Software Package.
 * @see TODO_PackageReference
 *
 * @section R_SCIFA_LLD_API_78_SEE_ALSO See Also:
 * @see RENESAS_SC_CFG Renesas Smart Configuration Settings
 * @see R_SCIFA_HLD_API_77 RZA2M SCIFA High Level Driver API
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 ************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/** Version Number of API. */
    #define STDIO_SCIFA_RZ_LLD_DRV_NAME           ("LLD EBK_RZA2M SCIFA")
/** Major Version Number of API. */
    #define STDIO_SCIFA_RZ_LLD_VERSION_MAJOR      (2)
/** Minor Version Number of API. */
    #define STDIO_SCIFA_RZ_LLD_VERSION_MINOR      (3)
/** Minor Version Number of API. */
    #define STDIO_SCIFA_RZ_LLD_BUILD_NUM          (0)
/** Unique ID. */
    #define STDIO_SCIFA_RZ_LLD_UID                (78)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**
 * @typedef st_scifa_rx_error_t struct to hold RX error counts.
 */
typedef struct
{
    uint32_t cnt_overrun;  /*!< Counter for FIFO overruns */
    uint32_t cnt_framing;  /*!< Counter for data framing errors */
    uint32_t cnt_parity;   /*!< Counter for data parity errors */
    uint32_t cnt_break;    /*!< Counter for received breaks */
} st_scifa_rx_error_t;

/**********************************************************************************************************************
 Function Prototypes
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
int_t R_SCIFA_Start (r_channel_t channel, int_t read_write);

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
int_t R_SCIFA_Stop (r_channel_t channel);

/**
 * @brief R_SCIFA_GetChannel Converts SC table index into SCIFA channel
 *
 * This function interrogates SC table to determine which
 * channel is described by sc_config
 *
 * @param[in] sc_config_index: configuration ID
 *
 * @retval  =>0: corresponding SCIFA channel
 * @retval  -1 DRV_ERROR
 */
int8_t R_SCIFA_GetChannel (r_sc_index_t sc_config_index);

/**
 * @brief R_SCIFA_Init initialises a channel for set-up
 * This function configures device registers in preparation for
 * configuration
 *
 * @param[in] channel: channel to initialise
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_Init (r_channel_t channel);

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
int_t R_SCIFA_SetParity (r_channel_t channel, scifa_parity_t desired_parity);

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
int_t R_SICFA_SetTransmissionMode (r_channel_t channel, scifa_sync_mode_t desired_mode);

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
int_t R_SCIFA_SetStopBits (r_channel_t channel, scifa_stop_t desired_stop_bits);

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
int_t R_SCIFA_SetDataBits (r_channel_t channel, scifa_size_t desired_data_bits);

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
int_t R_SCIFA_SetLoopBack (r_channel_t channel, scifa_loop_t loopback);

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
int_t R_SCIFA_SetDataOrder (r_channel_t channel, scifa_dir_t desired_data_order);

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
int_t R_SCIFA_Open (r_sc_index_t sc_config_index, scifa_config_t *p_config_set);

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
int_t R_SCIFA_ChannelConfigure (r_channel_t channel, const scifa_config_t *p_configuration, uint32_t * p_achieved_baud_rate);

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
int_t R_SCIFA_Close (r_channel_t channel);

/**
 * @brief R_SCIFA_StopTransmit stops transmission at the end of a DMA transmit
 *
 * @param[in] channel Channel to stop
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopTransmit (r_channel_t channel);

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
scifa_interrupting_state_t R_SCIFA_GetTXEIState (r_channel_t channel);

/**
 * @brief R_SCIFA_GetTXIState Determines the current TXI state for a channel
 * This function will determine whether a channel has recently triggered
 * a TXI interrupt event
 *
 * @param[in] channel: channel to check TXI status for
 *
 * @retval  SCIFA_INTERRUPTING or SCIFA_NOT_INTERRUPTING
 * @retval -1 DRV_ERROR
 */
scifa_interrupting_state_t R_SCIFA_GetTXIState (r_channel_t channel);

/**
 * @brief R_SCIFA_ClearTXIFlags removes TXI flags on a channel
 * This function is called at the end of an TXI handler routine
 * it will clear necessary TXI flag to mark TXI as handled
 *
 * @param[in] channel: channel to clear TXI flags for
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_ClearTXIFlags (r_channel_t channel);

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
int_t R_SCIFA_CheckRXIError (r_channel_t channel, st_scifa_rx_error_t *p_rx_err);

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
scifa_interrupting_state_t R_SCIFA_GetRXIState (r_channel_t channel);

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
int_t R_SCIFA_ClearRXIFlags (r_channel_t channel);

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
int_t R_SCIFA_IsRxFifoEmpty(r_channel_t channel, bool_t *p_fifo_empty);

/**
 * @brief R_SCIFA_StopTXI stops TXI for a given channel
 *
 * @param[in] channel: channel to disable TXI for
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopTXI (r_channel_t channel);

/**
 * @brief R_SCIFA_StartRX enable receive and receive interrupts
 *
 * @param[in] channel to start receive
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartRX (r_channel_t channel);

/**
 * @brief R_SCIFA_StopRX disable receive
 *
 * @param[in] channel to stop receive
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopRX (r_channel_t channel);

/**
 * @brief R_SCIFA_GetReadDataRegisterAddress return the read data register address
 *
 * @param[in] channel to get the FRDR register address for
 *
 * @retval the address of the FRDR register
 * @retval NULL error
 */
void * R_SCIFA_GetReadDataRegisterAddress (r_channel_t channel);

/**
 * @brief R_SCIFA_StartTX enable transmit and transmit interrupts
 *
 * @param[in] channel to start transmit
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartTX (r_channel_t channel);

/**
 * @brief R_SCIFA_StopTX disable transmit
 *
 * @param[in] channel to stop transmit
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StopTX (r_channel_t channel);

/**
 * @brief R_SCIFA_GetWriteDataRegisterAddress return the read data register address
 *
 * @param[in] channel to get the FTDR register address for
 *
 * @retval the address of the FTDR register
 * @retval NULL error
 */
void * R_SCIFA_GetWriteDataRegisterAddress (r_channel_t channel);

/**
 * @brief R_SCIFA_StartTXI Starts TXI interrupt for a given channel
 *
 * @param[in] channel: channel to start TXI for
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartTXI (r_channel_t channel);

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
int_t R_SCIFA_PutByte (r_channel_t channel, uint8_t data);

/**
 * @brief R_SCIFA_GetByte gets 1 byte from a channel's FIFO.
 * This function retrieves a single byte from a channel's
 * transmit FIFO
 *
 * @param[in] channel: channel to retrieve byte from FIFO
 * @param[out] p_data: location to place retrieved byte
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_GetByte (r_channel_t channel, uint8_t *p_data);

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
int_t R_SCIFA_IsTxFifoFull (r_channel_t channel, bool_t *p_fifo_full);

/**
 * @brief R_SCIFA_SetExtendedCfg sets extended device settings
 * This function sets the extended device settings, these settings 
 * are usually device specific or not considered commonly used
 *
 * @param[in] channel: channel to set extended settings for
 * @param[in] p_ext_cfg: extended Settings to be set
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetExtendedCfg (r_channel_t channel, const scifa_config_t *p_ext_cfg);

/**
 * @brief Sets up modem settings for a channel
 *
 * @param[in] channel: channel to configure modem settings for
 * @param[in] modem: modem setting for the channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetModemControl (r_channel_t channel, scifa_mce_t modem);

/**
 * @brief Sets noise cancellation
 *
 * @param[in] channel: channel to set the noise cancellation for
 * @param[in] noise_cancellation: noise cancellation setting
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetNoiseCanceller (r_channel_t channel, scifa_nfen_t noise_cancellation);

/**
 * @brief Sets the clock source for a channel
 *
 * @param[in] channel: channel to set clock source for
 * @param[in] desired_clock: clock setting for channel
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetClock (r_channel_t channel, scifa_clk_enable_t desired_clock);

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
int_t R_SCIFA_SetFIFOThreshold (r_channel_t channel, uint8_t tx_trg, uint8_t rx_trg);

/**
 * @brief Sets the TXD output level
 *
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetTXDOutput (r_channel_t channel, scifa_sptr_init_t level);

/**
 * @brief Sets the SCK output level
 *
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetSCKOutput (r_channel_t channel, scifa_sptr_init_t level);

/**
 * @brief Sets the CTS output level
 *
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetCTSOutput (r_channel_t channel, scifa_sptr_init_t level);

/**
 * @brief Sets the RTS output level
 *
 * @param[in] channel: the channel to set
 * @param[in] level: the level (low or high)
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetRTSOutput (r_channel_t channel, scifa_sptr_init_t level);

/**
 * @brief R_SCIFA_GetVersion - Get version information
 *
 * @param[out] pVerInfo: version info
 *
 * @retval  0 DRV_SUCCESS (always)
 */
int_t R_SCIFA_GetVersion (st_ver_info_t *pVerInfo);

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
int_t R_SCIFA_SetupInterrupts (r_channel_t channel, e_r_drv_intc_priority_t txi_priority, e_r_drv_intc_priority_t txei_priority,
        e_r_drv_intc_priority_t rxi_priority, void (*txi_handler)(uint32_t), void (*txei_handler)(uint32_t), void (*rxi_handler)(uint32_t));

/**
 * @brief R_SCIFA_StartModule - start or stop SCIFA module
 *
 * @param[in] channel: the channel number
 * @param[in] start: true to start channel, or false to stop it
 *
 * @retval  0 DRV_SUCCESS
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_StartModule (r_channel_t channel, bool_t start);

/**
 * @brief Sets the device baud rate within 1% of value desired.
 *
 * This function determines the best possible settings for the
 * baud rate registers for the specified peripheral clock speed
 * and baud rate. Note that this does not guarantee a low bit
 * error rate, just the best possible one. The bit rate error is
 * returned in .1% increments. If the hardware cannot support
 * the specified combination, DEV_ERROR is returned.
 *
 * NOTE: The transmitter and receiver (TE and RE bits in SCR) must be disabled
 *       prior to calling this function.
 *
 *       The application must pause for 1 bit time after the BRR register
 *       is loaded before transmitting/receiving to allow time for the clock
 *       to settle.
 * @param[in]  channel: Channel number of SCIFA(0-4)
 * @param[in]  desired_baud_rate: Baud rate; 19200, 57600, 115200, etc.
 * @param[in]  clock_freq: Peripheral clock speed; e.g. 24000000 for 24MHz
 * @param[in]  mode: asynchronous or synchronous
 * @param[out] p_achieved_baud_rate - Pointer to achieved Baud rate
 *
 * @retval  0 DRV_SUCCESS - If a Baud rate within 1% of the desired value was
 *                          achieved.
 * @retval -1 DRV_ERROR
 */
int_t R_SCIFA_SetBaud (r_channel_t channel, uint32_t desired_baud_rate, uint32_t clock_freq, scifa_sync_mode_t mode, uint32_t *p_achieved_baud_rate);

/**
 * @brief R_SCIFA_DisableTxInterrupt - disable transmit interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_DisableTxInterrupt (r_channel_t channel);

/**
 * @brief R_SCIFA_EnableTxInterrupt - enable transmit interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_EnableTxInterrupt (r_channel_t channel);

/**
 * @brief R_SCIFA_DisableRxInterrupt - disable receive interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_DisableRxInterrupt (r_channel_t channel);

/**
 * @brief R_SCIFA_EnableRxInterrupt - enable receive interrupt for channel
 *
 * @param[in] channel: the channel number
 *
 * @retval  None
 */
void R_SCIFA_EnableRxInterrupt (r_channel_t channel);

#endif /* R_SCIFA_INC_R_RZA2_SCIFA_LLD_H_ */
