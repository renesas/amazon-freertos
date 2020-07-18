/*
 * r_sci_wrapper_if.h
 *
 *  Created on: 2020/05/15
 *      Author: hmU11282
 */

#ifndef R_SCI_WRAPPER_IF_H_
#define R_SCI_WRAPPER_IF_H_

#include "rl_platform.h"
#include "r_sci_wrapper_config.h"
#include "r_cg_serial.h"

#define SCI_CLK_INT         (0x00U) /* use internal clock for baud generation */
#define SCI_CLK_EXT8X       (0x03U) /* use external clock 8x baud rate (ASYNC) */
#define SCI_CLK_EXT16X      (0x02U) /* use external clock 16x baud rate (ASYNC) */
#define SCI_DATA_7BIT       (0x40U)
#define SCI_DATA_8BIT       (0x00U)
#define SCI_PARITY_ON       (0x20U)
#define SCI_PARITY_OFF      (0x00U)
#define SCI_ODD_PARITY      (0x10U)
#define SCI_EVEN_PARITY     (0x00U)
#define SCI_STOPBITS_2      (0x08U)
#define SCI_STOPBITS_1      (0x00U)



typedef enum e_sci_ch
{
    SCI_CH0 = 0,
    SCI_CH1,
    SCI_CH2,
    SCI_CH3,
    SCI_NUM_CH
} sci_ch_t;

typedef enum e_sci_mode
{
    SCI_MODE_OFF = 0,
    SCI_MODE_ASYNC,
	SCI_MODE_MAX
} sci_mode_t;


typedef enum e_sci_err
{
    SCI_SUCCESS = 0,
    SCI_ERR_BAD_CHAN,
	SCI_ERR_OMITTED_CHAN,
	SCI_ERR_CH_NOT_CLOSED,
	SCI_ERR_BAD_MODE,
    SCI_ERR_INVALID_ARG,
    SCI_ERR_NULL_PTR,
	SCI_ERR_XCVR_BUSY,
	SCI_ERR_QUEUE_UNAVAILABLE,
	SCI_ERR_INSUFFICIENT_SPACE,
	SCI_ERR_INSUFFICIENT_DATA
} sci_err_t;


typedef struct st_sci_ch_ctrl * sci_hdl_t;


typedef struct st_sci_uart
{
    uint32_t    baud_rate;      // ie 9600, 19200, 115200
    uint8_t     clk_src;        // use SCI_CLK_INT/EXT8X/EXT16X
    uint8_t     data_size;      // use SCI_DATA_nBIT
    uint8_t     parity_en;      // use SCI_PARITY_ON/OFF
    uint8_t     parity_type;    // use SCI_ODD/EVEN_PARITY
    uint8_t     stop_bits;      // use SCI_STOPBITS_1/2
    uint8_t     int_priority;   // interrupt priority; 1=low, 15=high
} sci_uart_t;

typedef union
{
    sci_uart_t      async;
} sci_cfg_t;

typedef enum e_sci_cb_evt
{
    SCI_EVT_TEI,
    SCI_EVT_RX_CHAR,
    SCI_EVT_RXBUF_OVFL = 3,
	SCI_EVT_FRAMING_ERR,
	SCI_EVT_PARITY_ERR,
	SCI_EVT_OVFL_ERR
} sci_cb_evt_t;

typedef struct st_sci_cb_args
{
    sci_hdl_t       hdl;
    sci_cb_evt_t    event;
    uint8_t         byte;
    uint8_t         num;
} sci_cb_args_t;

typedef enum e_sci_cmd
{
    SCI_CMD_CHANGE_BAUD,
    SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ,
    SCI_CMD_RX_Q_FLUSH,
    SCI_CMD_TX_Q_FLUSH,
    SCI_CMD_TX_Q_BYTES_FREE,
	SCI_CMD_EN_CTS_IN
} sci_cmd_t;

typedef struct st_sci_baud
{
    uint32_t    pclk;       // peripheral clock speed; e.g. 24000000 is 24MHz
    uint32_t    rate;       // e.g. 9600, 19200, 115200
} sci_baud_t;


sci_err_t R_SCI_Open(uint8_t const      chan,
                     sci_mode_t const   mode,
                     sci_cfg_t * const  p_cfg,
                     void               (* const p_callback)(void *p_args),
                     sci_hdl_t * const  p_hdl);

sci_err_t R_SCI_Close(sci_hdl_t const hdl);

sci_err_t R_SCI_Send(sci_hdl_t const hdl,
                     uint8_t         *p_src,
                     uint16_t  const length);

sci_err_t R_SCI_Receive(sci_hdl_t const hdl,
                        uint8_t         *p_dst,
                        uint16_t  const length);

sci_err_t R_SCI_Control(sci_hdl_t const hdl,
                        sci_cmd_t const cmd,
                        void            *p_args);

#endif /* R_SCI_WRAPPER_IF_H_ */
