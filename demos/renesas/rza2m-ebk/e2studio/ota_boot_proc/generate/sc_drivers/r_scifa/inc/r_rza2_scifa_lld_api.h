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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_rza2_scifa_lld_api.h
 * Description  : RZA2M SCIFA Low Layer Driver header file
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
#include "r_typedefs.h"

#ifndef SRC_RENESAS_DRIVERS_R_SCIFA_INC_R_RZA2_SCIFA_LLD_API_H_
#define SRC_RENESAS_DRIVERS_R_SCIFA_INC_R_RZA2_SCIFA_LLD_API_H_

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/* List channels supported */
#define R_CFG_SCIFA_CHANNELS_SUPPORTED         (R_CH0 | R_CH1 | R_CH2 | R_CH3 | R_CH4)

/* Maximum number of modules that can be granted simultaneous access to this driver  */
#define  R_CFG_SCIFA_DRV_MAXIMUM_ACCESS_PRV (1)

typedef int8_t r_channel_t;
typedef int16_t r_sc_index_t;

typedef enum e_scifa_tx_dt_mode
{
    SCIFA_TX_MODE_OFF       = 0, // not in use
    SCIFA_TX_POLLING_MODE   = 1, // polling mode
    SCIFA_TX_INTERRUPT_MODE = 2, // interrupt mode
    SCIFA_TX_DMA_MODE       = 3  // DMA mode
} scifa_tx_dt_mode_t;

typedef enum e_scifa_rx_dt_mode
{
    SCIFA_RX_MODE_OFF       = 0, // not in use
    SCIFA_RX_POLLING_MODE   = 1, // polling mode
    SCIFA_RX_INTERRUPT_MODE = 2, // interrupt mode
    SCIFA_RX_DMA_MODE       = 3  // DMA mode
} scifa_rx_dt_mode_t;

typedef enum e_scifa_sync_mode    // SCIFA operational modes
{
    SCIFA_MODE_ASYNC = 0,     // Asynchronous
    SCIFA_MODE_SYNC  = 1      // Synchronous
} scifa_sync_mode_t;

typedef enum e_scifa_clk_enable
{
    SCIFA_CLK_SRC_INT_SCK_IN    = 0,  // use internal clock and set SCK port as input
    SCIFA_CLK_SRC_INT_SCK_OUT   = 1,  // use internal clock and set SCK port as output
    SCIFA_CLK_SRC_EXT_SCK_IN    = 2   // use external clock and set SCK port as input
} scifa_clk_enable_t;

typedef enum e_scifa_base_clk_sel
{
    SCIFA_CLK_16X = 0,  // use clock as 16x mode (asynchronous mode only)
    SCIFA_CLK_8X  = 1   // use clock as 8x mode (asynchronous mode only)
} scifa_base_clk_sel_t;

typedef enum e_scifa_size
{
    SCIFA_DATA_8BIT = 0,
    SCIFA_DATA_7BIT = 1
} scifa_size_t;

typedef enum e_scifa_parity_en
{
    SCIFA_PARITY_OFF = 0,
    SCIFA_PARITY_ON  = 1
} scifa_parity_en_t;

typedef enum e_scifa_parity
{
    SCIFA_NONE_PARITY = 0,
    SCIFA_EVEN_PARITY = 1,
    SCIFA_ODD_PARITY  = 2
} scifa_parity_t;

typedef enum e_scifa_stop
{
    SCIFA_STOPBITS_1 = 0,
    SCIFA_STOPBITS_2 = 1
} scifa_stop_t;

/**
 * @typedef e_scifa_noise_t Enable Noise Cancellation
 */
typedef enum e_scifa_nfen
{
    SCIFA_NOISE_CANCEL_DISABLE = 0,
    SCIFA_NOISE_CANCEL_ENABLE = 1
} scifa_nfen_t;

typedef enum e_scifa_dir
{
    SCIFA_LSB_FIRST = 0,
    SCIFA_MSB_FIRST = 1
} scifa_dir_t;

typedef enum e_scifa_loop
{
    SCIFA_LOOPBACK_DISABLE = 0,
    SCIFA_LOOPBACK_ENABLE  = 1
} scifa_loop_t;

typedef enum e_scifa_mce
{
    SCIFA_MODEM_CONTROL_DISABLE = 0,
    SCIFA_MODEM_CONTROL_ENABLE  = 1
} scifa_mce_t;

typedef enum e_scifa_rstrg
{
    SCIFA_RTS_ACTIVE_TRIGGER_15 = 0,
    SCIFA_RTS_ACTIVE_TRIGGER_1  = 1,
    SCIFA_RTS_ACTIVE_TRIGGER_4  = 2,
    SCIFA_RTS_ACTIVE_TRIGGER_6  = 3,
    SCIFA_RTS_ACTIVE_TRIGGER_8  = 4,
    SCIFA_RTS_ACTIVE_TRIGGER_10 = 5,
    SCIFA_RTS_ACTIVE_TRIGGER_12 = 6,
    SCIFA_RTS_ACTIVE_TRIGGER_14 = 7
} scifa_rstrg_t;

typedef enum e_scifa_sptr_init
{
    SCIFA_SPTR_INIT_LOW  = 0,
    SCIFA_SPTR_INIT_HIGH = 1
} scifa_sptr_init_t;


typedef enum e_scifa_err_t          // SCI API error codes
{
    SCIFA_SUCCESS = 0,
    SCIFA_ERR_BAD_CHAN,             // non-existent channel number
    SCIFA_ERR_CH_NOT_CLOSED,        // channel still running in another mode
    SCIFA_ERR_BAD_MODE,             // unsupported or incorrect mode for channel
    SCIFA_ERR_INVALID_ARG,          // argument is not one of the predefined values
    SCIFA_ERR_NULL_PTR,             // received null pointer; missing required argument
    SCIFA_ERR_BUSY,                 // already handling requests
    SCIFA_ERR_CH_NOT_OPENED,        // channel not opened
    SCIFA_ERR_CH_NOT_STARTED,       // channel not started
    SCIFA_ERR_CH_NOT_ACCEPTABLE_CMD // called command can't accept in current state
} scifa_err_t;

typedef enum e_scifa_tx_trigger
{
    SCIFA_TX_FIFO_TRIGGER_8  = 0,
    SCIFA_TX_FIFO_TRIGGER_4  = 1,
    SCIFA_TX_FIFO_TRIGGER_2  = 2,
    SCIFA_TX_FIFO_TRIGGER_0  = 3
} scifa_tx_trigger_t;

typedef enum e_scifa_rx_trigger
{
    SCIFA_RX_FIFO_TRIGGER_1  = 0,
    SCIFA_RX_FIFO_TRIGGER_4  = 1,
    SCIFA_RX_FIFO_TRIGGER_8  = 2,
    SCIFA_RX_FIFO_TRIGGER_14 = 3
} scifa_rx_trigger_t;

typedef enum e_scifa_ttrgs
{
    SCIFA_TXTRIG_USE_FCR_TTRG  = 0,
    SCIFA_TXTRIG_USE_FTCR_TFTC = 1
} scifa_ttrgs_t;

typedef enum e_scifa_rtrgs
{
    SCIFA_RXTRIG_USE_FCR_RTRG  = 0,
    SCIFA_RXTRIG_USE_FTCR_RFTC = 1
} scifa_rtrgs_t;

typedef enum
{
    SCIFA_NOT_INTERRUPTING  = 0,
    SCIFA_INTERRUPTING      = 1
} scifa_interrupting_state_t;

/* SCIFA init structure */
typedef struct st_scifa_init
{
    scifa_sync_mode_t    sync_mode;           // SMR(7).CM
    uint32_t             baud_rate;           // ie 9600, 19200, 115200
    scifa_clk_enable_t   clk_enable;          // SCR(1:0).CKE[1:0]
    scifa_base_clk_sel_t clk_base;            // SEMR(0).ABCS0
    scifa_size_t         data_size;           // SMR(6).CHR
    scifa_parity_en_t    parity_en;           // SMR(5).PE
    scifa_parity_t       parity_type;         // SMR(4).PM
    scifa_stop_t         stop_bits;           // SMR(3).STOP
    scifa_nfen_t         noise_canceller;     // SEMR(2).NFEN
    scifa_dir_t          data_direction;      // SEMR(3).DIR
    scifa_loop_t         loopback;            // FCR(0).LOOP
    scifa_mce_t          modem_control;       // FCR(3).MCE
    scifa_tx_trigger_t   tx_trigger;          // FCR(5:4).TTRG
    scifa_rx_trigger_t   rx_trigger;          // FCR(7:6).RTRG
    scifa_rstrg_t        rts_trigger;         // FCR(10:8).RSTRG
    scifa_sptr_init_t    txd_init_value;      // SPTR(0).SPB2DT
    scifa_sptr_init_t    sck_init_value;      // SPTR(2).SCKDT
    scifa_sptr_init_t    cts_init_value;      // SPTR(4).CTS2DT
    scifa_sptr_init_t    rts_init_value;      // SPTR(6).RTS2DT
    scifa_ttrgs_t        ttrgs;               // FTCR(7).TTRGS
    scifa_rtrgs_t        rtrgs;               // FTCR(15).RTRGS
    uint8_t              tftc;                // FTCR(4:0).TFTC
    uint8_t              rftc;                // FTCR(12:8).RFTC
} scifa_init_t;

typedef enum e_scifa_break_dt
{
    SCIFA_SPTR_BREAK_DT_LOW  = 0,
    SCIFA_SPTR_BREAK_DT_HIGH = 1
} scifa_break_dt_t;

typedef enum e_scifa_sck_dt
{
    SCIFA_SPTR_SCK_DT_LOW  = 0,
    SCIFA_SPTR_SCK_DT_HIGH = 1
} scifa_sck_dt_t;

typedef enum e_scifa_cts_dt
{
    SCIFA_SPTR_CTS_DT_LOW  = 0,
    SCIFA_SPTR_CTS_DT_HIGH = 1
} scifa_cts_dt_t;

typedef enum e_scifa_rts_dt
{
    SCIFA_SPTR_RTS_DT_LOW  = 0,
    SCIFA_SPTR_RTS_DT_HIGH = 1
} scifa_rts_dt_t;

/* BAUD DIVISOR INFO */

/* BRR = (PCLK / (divisor * baud)) - 1
   when ABCS = 1, divisor = 32 * pow(2, 2n - 1)
   when ABCS = 0, divisor = 64 * pow(2, 2n - 1) */

typedef struct st_baud_divisorb
{
    int16_t     divisor;    // clock divisor
    uint8_t     abcs;       // ABCS value to get divisor
    uint8_t     bgdm;       // BGDM value to get divisor
    uint8_t     cks;        // CKS  value to get divisor (CKS = n)
} baud_divisorb_t;

#define NUM_DIVISORS_ASYNC  (9)

/* ROM INFO */

typedef struct st_scifa_ch_rom       // SCIF ROM info for channel control block
{
    uint16_t                    tx_buf_size;     // size of TX buffer
    uint16_t                    rx_buf_size;     // size of RX buffer
    uint16_t                    txi_id;          // Interrupt ID number of TXI
    uint16_t                    rxi_id;          // Interrupt ID number of RXI
    uint16_t                    eri_bri_id;      // Interrupt ID number of ERI_BRI
    uint16_t                    tei_dri_id;      // Interrupt ID number of TEI_DRI
    void                       *txi_handler;     // Interrupt handler address of TXI
    void                       *rxi_handler;     // Interrupt handler address of RXI
    void                       *eri_bri_handler; // Interrupt handler address of ERI_BRI
} scifa_ch_rom_t;

/* CHANNEL CONTROL BLOCK */

typedef enum e_scifa_tx_status
{
    SCIFA_TX_STATUS_CLOSED  = 0, // channel closed
    SCIFA_TX_STATUS_OPENED  = 1, // channel opened
    SCIFA_TX_STATUS_IDLE    = 2, // channel started
    SCIFA_TX_STATUS_RUNNING = 3, // channel busy SCIFA_TX_STATUS_BUSY
    SCIFA_TX_STATUS_BREAK   = 4  // break sending
} scifa_tx_status_t;

typedef enum e_scifa_rx_status
{
    SCIFA_RX_STATUS_CLOSED = 0x00, // channel closed
    SCIFA_RX_STATUS_OPENED = 0x01, // channel opened
    SCIFA_RX_STATUS_IDLE   = 0x02, // channel started
    SCIFA_RX_STATUS_BUSY   = 0x04, // channel busy
    SCIFA_RX_STATUS_BRK    = 0x08, // BRI dedected
    SCIFA_RX_STATUS_FER    = 0x10, // FER detected
    SCIFA_RX_STATUS_PER    = 0x20, // PER detected
    SCIFA_RX_STATUS_ORERH  = 0x40, // ORER on FIFO detected
    SCIFA_RX_STATUS_ORERS  = 0x80  // ORER on Buffer detected
} scifa_rx_status_t;

typedef struct st_scifa_ch_ctrl     // SCIFA Channel Control Block
{
    scifa_ch_rom_t const *p_rom;     // pointer to rom info
    void                 (*callback)(void *p_args); // function ptr from Open()
    scifa_tx_dt_mode_t   tx_dt_mode; // data transfer mode of TX
    scifa_rx_dt_mode_t   rx_dt_mode; // data transfer mode of RX
    uint8_t              *p_twp;     // write pointer of TX software buffer
    uint8_t              *p_trp;     // read  pointer of TX software buffer
    uint16_t             tx_wc;      // write data counter of TX software buffer
    uint8_t              tftc;       // TX FIFO threshold value
    uint16_t             *p_rwp;     // write pointer of RX software buffer
    uint16_t             *p_rrp;     // read  pointer of RX software buffer
    uint16_t             rx_wc;      // write data counter of RX software buffer
    uint16_t             rx_rc;      // read  data counter of RX software buffer
    uint8_t              rftc;       // RX FIFO threshold value
    scifa_tx_status_t    tx_status;  // status code of TX high-level-driver
    scifa_rx_status_t    rx_status;  // status code of RX high-level-driver
} scifa_ch_ctrl_t;

typedef enum e_scifa_ch_t      // SCIF channel numbers
{
    SCIFA_CH0 = 0,
    SCIFA_CH1 = 1,
    SCIFA_CH2 = 2,
    SCIFA_CH3 = 3,
    SCIFA_CH4 = 4,
    SCIFA_NUM_CH = 5
} scifa_ch_t;

typedef enum
{
    SCIFA_MODE_BUFFERED,
    SCIFA_MODE_DMA
} scifa_driver_mode_t;

typedef enum
{
    SCIFA_READ_BLOCKING_MODE_ENABLE,
    SCIFA_READ_BLOCKING_MODE_DISABLE
} scifa_read_blocking_mode_t;

typedef enum
{
    SCIFA_WRITE_BLOCKING_MODE_ENABLE,
    SCIFA_WRITE_BLOCKING_MODE_DISABLE
} scifa_write_blocking_mode_t;

/* SCI_OPEN() ARGUMENT DEFINITIONS (do NOT change values) */
/* Open() p_cfg structure when mode=SCI_MODE_ASYNC */
typedef struct st_scifa_config_t
{
    /* initialise data of low level driver */
    scifa_sync_mode_t           sync_mode;                         // SMR(7).CM
    uint32_t                    baud_rate;                         // e.g. 9600, 19200, 115200
    scifa_clk_enable_t          clk_enable;                        // SCR(1:0).CKE[1:0]
    scifa_base_clk_sel_t        clk_base;                          // SEMR(0).ABCS0
    scifa_size_t                data_size;                         // SMR(6).CHR
    scifa_parity_en_t           parity_en;                         // SMR(5).PE
    scifa_parity_t              parity_type;                       // SMR(4).PM
    scifa_stop_t                stop_bits;                         // SMR(3).STOP
    scifa_nfen_t                noise_canceller;                   // SEMR(2).NFEN
    scifa_dir_t                 data_direction;                    // SEMR(3).DIR
    scifa_loop_t                loopback;                          // FCR(0).LOOP
    scifa_mce_t                 modem_control;                     // FCR(3).MCE
    scifa_rstrg_t               rts_trigger;                       // FCR(10:8).RSTRG
    uint8_t                     tftc;                              // FTCR(4:0).TFTC
    uint8_t                     rftc;                              // FTCR(12:8).RFTC
    scifa_sptr_init_t           txd_init_value;                    // SPTR(0).SPB2DT
    scifa_sptr_init_t           sck_init_value;                    // SPTR(2).SCKDT
    scifa_sptr_init_t           cts_init_value;                    // SPTR(4).CTS2DT
    scifa_sptr_init_t           rts_init_value;                    // SPTR(6).RTS2DT
    scifa_tx_dt_mode_t          tx_dt_mode;                        // ISR, DMA or software polling
    scifa_rx_dt_mode_t          rx_dt_mode;                        // ISR, DMA or software polling
    uint8_t                     eri_bri_priority;                  // interrupt priority level of ERI_BRI
    uint8_t                     rxi_priority;                      // interrupt priority level of RXI
    uint8_t                     txi_priority;                      // interrupt priority level of TXI
    uint8_t                     tei_dri_priority;                  // interrupt priority level of TEI_DRI
    void                        (*write_complete_callback)(void);  // callback for DMA write complete
    void                        (*read_complete_callback)(void);   // callback for DMA read complete
    char *                      dma_tx;                            // the DMA module to use for transmit
    char *                      dma_rx;                            // the DMA module to use for receive
    scifa_read_blocking_mode_t  read_blocking_mode;                // wait if there is no data, or return immediately
    scifa_write_blocking_mode_t write_blocking_mode;               // wait for space in the transmit buffer, or return immediately
    uint16_t                    receive_buffer_length;             // the length of the receive buffer
    uint16_t                    transmit_buffer_length;            // the length of the transmit buffer
} scifa_config_t;

typedef struct
{
    char *buffer;                                       // data to be transmitted / buffer for received data
    uint32_t length;                                    // length of data to transmit / max length of received data
} st_scifa_dma_rw_t;

/* CALLBACK FUNCTION ARGUMENT DEFINITIONS */

typedef enum e_scifa_cb_evt_t    // callback function events
{
    SCIFA_EVT_TX_DONE,           // Send() requests processed; last bit transmitted
    SCIFA_EVT_RX_DONE,           // Receive() request processed; some or no data may be in RX FIFO
    SCIFA_EVT_RX_BREAK,          // received BREAK condition
    SCIFA_EVT_RX_OVERRUNH,       // receive FIFO overrun error
    SCIFA_EVT_RX_OVERRUNS,       // receive buffer overrun error
    SCIFA_EVT_RX_FRAMING_ERR,    // received framing error
    SCIFA_EVT_RX_PARITY_ERR      // received parity error
} scifa_cb_evt_t;

typedef struct st_scifa_cb_args_t  // callback arguments
{
    scifa_ch_ctrl_t  hdl;
    scifa_cb_evt_t   event;
} scifa_cb_args_t;


/* SCI_CONTROL() ARGUMENT DEFINITIONS */

/* commands */
typedef enum e_scifa_cmd_t
{
    SCIFA_CMD_BREAK_ON,           // set break condition to ON
    SCIFA_CMD_BREAK_OFF,          // set break condition to OFF
    SCIFA_CMD_RESET_TX,           // abort TX requests; flush transmit FIFO
    SCIFA_CMD_RESET_RX,           // abort RX requests; flush receive FIFO
    SCIFA_CMD_INT_CONFIG_TXI,     // register TXI handler
    SCIFA_CMD_INT_CONFIG_RXI,     // register RXI handler
    SCIFA_CMD_INT_CONFIG_ERIBRI,  // register ERI/BRI handler
    SCIFA_CMD_INT_CONFIG_TEIDRI   // register TEI/DRI handler
} scifa_cmd_t;


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Total channels supported */
#define R_CFG_SCIFA_DRV_CHANNELS_AVAIL_PRV    (5)

/* Version Number of API */
#define SCIFA_ASYNC_VERSION_MAJOR  (0)
#define SCIFA_ASYNC_VERSION_MINOR  (10)


/**********************************************************************************************************************
 Variable externs
 *********************************************************************************************************************/


/**********************************************************************************************************************
 Function prototypes
 *********************************************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SRC_RENESAS_DRIVERS_R_SCIFA_INC_R_RZA2_SCIFA_LLD_API_H_ */

/* End of File */
