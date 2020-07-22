/*
 * r_sci_wrapper.c
 *
 *  Created on: 2020/05/15
 *      Author: hmU11282
 */

#include "rl_platform.h"
#include "r_sci_wrapper_private.h"

#if (SCI_CFG_CH0_INCLUDED)
static uint8_t ch0_tx_buf[SCI_CFG_CH0_TX_BUFSIZ];
static uint8_t ch0_rx_buf[SCI_CFG_CH0_RX_BUFSIZ];
#endif
#if (SCI_CFG_CH1_INCLUDED)
static uint8_t ch1_tx_buf[SCI_CFG_CH1_TX_BUFSIZ];
static uint8_t ch1_rx_buf[SCI_CFG_CH1_RX_BUFSIZ];
#endif
#if (SCI_CFG_CH2_INCLUDED)
static uint8_t ch2_tx_buf[SCI_CFG_CH2_TX_BUFSIZ];
static uint8_t ch2_rx_buf[SCI_CFG_CH2_RX_BUFSIZ];
#endif
#if (SCI_CFG_CH3_INCLUDED)
static uint8_t ch3_tx_buf[SCI_CFG_CH3_TX_BUFSIZ];
static uint8_t ch3_rx_buf[SCI_CFG_CH3_RX_BUFSIZ];
#endif

extern const sci_hdl_t g_handles[SCI_NUM_CH];

static sci_err_t sci_init_queues(uint8_t const chan);

static sci_err_t sci_init_queues(uint8_t const chan)
{
    byteq_err_t q_err1 = BYTEQ_ERR_INVALID_ARG;
    byteq_err_t q_err2 = BYTEQ_ERR_INVALID_ARG;
    sci_err_t err = SCI_SUCCESS;

    switch(chan)
    {
#if SCI_CFG_CH0_INCLUDED
        case SCI_CH0:
        {
            q_err1 = R_BYTEQ_Open(ch0_tx_buf, SCI_CFG_CH0_TX_BUFSIZ, &g_handles[SCI_CH0]->tx_bq_hdl);
            q_err2 = R_BYTEQ_Open(ch0_rx_buf, SCI_CFG_CH0_RX_BUFSIZ, &g_handles[SCI_CH0]->rx_bq_hdl);
            break;
        }
#endif
#if SCI_CFG_CH1_INCLUDED
        case SCI_CH1:
        {
            q_err1 = R_BYTEQ_Open(ch1_tx_buf, SCI_CFG_CH1_TX_BUFSIZ, &g_handles[SCI_CH1]->tx_bq_hdl);
            q_err2 = R_BYTEQ_Open(ch1_rx_buf, SCI_CFG_CH1_RX_BUFSIZ, &g_handles[SCI_CH1]->rx_bq_hdl);
            break;
        }
#endif
#if SCI_CFG_CH2_INCLUDED
        case SCI_CH2:
        {
            q_err1 = R_BYTEQ_Open(ch2_tx_buf, SCI_CFG_CH2_TX_BUFSIZ, &g_handles[SCI_CH2]->tx_bq_hdl);
            q_err2 = R_BYTEQ_Open(ch2_rx_buf, SCI_CFG_CH2_RX_BUFSIZ, &g_handles[SCI_CH2]->rx_bq_hdl);
            break;
        }
#endif
#if SCI_CFG_CH3_INCLUDED
        case SCI_CH3:
        {
            q_err1 = R_BYTEQ_Open(ch3_tx_buf, SCI_CFG_CH3_TX_BUFSIZ, &g_handles[SCI_CH3]->tx_bq_hdl);
            q_err2 = R_BYTEQ_Open(ch3_rx_buf, SCI_CFG_CH3_RX_BUFSIZ, &g_handles[SCI_CH3]->rx_bq_hdl);
            break;
        }
#endif
        default:
        {
            err = SCI_ERR_QUEUE_UNAVAILABLE;
        }
    }
    if (BYTEQ_SUCCESS != q_err1 || BYTEQ_SUCCESS != q_err2)
    {
        return SCI_ERR_QUEUE_UNAVAILABLE;
    }
    return err;
} // sci_init_queues


sci_err_t R_SCI_Open(uint8_t const      chan,
                     sci_mode_t const   mode,
                     sci_cfg_t * const  p_cfg,
                     void               (* const p_callback)(void *p_args),
                     sci_hdl_t * const  p_hdl)
{

    sci_err_t err;

#if (SCI_CFG_PARAM_CHECKING_ENABLE)

    err = sci_chan_param_check(chan);
    if (SCI_SUCCESS != err)
    {
        return err;
    }

    if (NULL == g_handles[chan])
    {
        return SCI_ERR_OMITTED_CHAN;
    }

    if (SCI_MODE_OFF != g_handles[chan]->mode)
    {
        return SCI_ERR_CH_NOT_CLOSED;
    }

    if (SCI_MODE_OFF == mode || SCI_MODE_MAX <= mode)
    {
        return SCI_ERR_BAD_MODE;
    }

    if (NULL == p_hdl)
    {
        return SCI_ERR_NULL_PTR;
    }
#endif

    g_handles[chan]->mode = mode;
    g_handles[chan]->tx_idle = true;
    g_handles[chan]->callback = p_callback;

    err = sci_init_queues(chan);
    if (SCI_SUCCESS != err)
    {
    	g_handles[chan]->mode = SCI_MODE_OFF;
        return err;
    }

    switch(chan)
    {
#if SCI_CFG_CH0_INCLUDED
        case SCI_CH0:
        {
            R_UART0_Reset( p_cfg->async.baud_rate );
            break;
        }
#endif
#if SCI_CFG_CH1_INCLUDED
        case SCI_CH1:
        {
            R_UART1_Start();
            break;
        }
#endif
#if SCI_CFG_CH2_INCLUDED
        case SCI_CH2:
        {
            R_UART2_Start();
            break;
        }
#endif
#if SCI_CFG_CH3_INCLUDED
        case SCI_CH3:
        {
            R_UART3_Start();
            break;
        }
#endif
    }

    *p_hdl = g_handles[chan];

    return SCI_SUCCESS;
}// R_SCI_Open

sci_err_t R_SCI_Close(sci_hdl_t const hdl)
{

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (NULL == hdl)
    {
        return SCI_ERR_NULL_PTR;
    }
#endif

    if (SCI_MODE_ASYNC == hdl->mode)
    {
        R_BYTEQ_Close(hdl->tx_bq_hdl);
        R_BYTEQ_Close(hdl->rx_bq_hdl);
    }

    hdl->mode = SCI_MODE_OFF;

    switch(hdl->chan)
    {
#if SCI_CFG_CH0_INCLUDED
        case SCI_CH0:
        {
            R_UART0_Stop();
            break;
        }
#endif
#if SCI_CFG_CH1_INCLUDED
        case SCI_CH1:
        {
            R_UART1_Stop();
            break;
        }
#endif
#if SCI_CFG_CH2_INCLUDED
        case SCI_CH2:
        {
            R_UART2_Stop();
            break;
        }
#endif
#if SCI_CFG_CH3_INCLUDED
        case SCI_CH3:
        {
            R_UART3_Stop();
            break;
        }
#endif
    }

    return SCI_SUCCESS;
}// R_SCI_Close

sci_err_t R_SCI_Send(sci_hdl_t const hdl, uint8_t * p_src, uint16_t const length)
{
    sci_err_t err = SCI_SUCCESS;

#if (SCI_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == hdl || NULL == p_src)
    {
        return SCI_ERR_NULL_PTR;
    }
    if (SCI_MODE_OFF == hdl->mode || SCI_MODE_MAX <= hdl->mode)
    {
        return SCI_ERR_BAD_MODE;
    }
    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif

    uint16_t cnt = 0;
    byteq_err_t q_err = BYTEQ_ERR_QUEUE_FULL;

    R_BYTEQ_Unused(hdl->tx_bq_hdl, &cnt);
    if (cnt < length)
    {
    	return SCI_ERR_INSUFFICIENT_SPACE;
    }

    for (cnt = 0; cnt < length; cnt++)
    {
        q_err = put_tx_q(hdl->chan, (const uint8_t)*p_src++);
        if (BYTEQ_SUCCESS != q_err)
        {
            err = SCI_ERR_INSUFFICIENT_SPACE;
            break;
        }
    }

    if (err == SCI_SUCCESS)
    {
        hdl->tx_idle = false;
        sci_transfer(hdl);
    }

    return err;
}// R_SCI_Send

sci_err_t R_SCI_Receive(sci_hdl_t const hdl,
                        uint8_t         *p_dst,
                        uint16_t  const length)
{
    sci_err_t err = SCI_SUCCESS;

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (NULL == hdl || NULL == p_dst)
    {
        return SCI_ERR_NULL_PTR;
    }
    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
    if (SCI_MODE_OFF == hdl->mode || SCI_MODE_MAX <= hdl->mode)
    {
        return SCI_ERR_BAD_MODE;
    }
#endif

    uint16_t cnt;
    byteq_err_t q_err = BYTEQ_SUCCESS;

    R_BYTEQ_Used(hdl->rx_bq_hdl, &cnt);
    if (cnt < length)
    {
        return SCI_ERR_INSUFFICIENT_SPACE;
    }

    for (cnt = 0; cnt < length; cnt++)
    {
        q_err = R_BYTEQ_Get(hdl->rx_bq_hdl, p_dst++);
        if (BYTEQ_SUCCESS != q_err)
        {
            err = SCI_ERR_INSUFFICIENT_SPACE;
            break;
        }
    }

	return err;
}// R_SCI_Receive

sci_err_t R_SCI_Control(sci_hdl_t const hdl,
                        sci_cmd_t const cmd,
                        void            *p_args)
{
    sci_err_t err = SCI_SUCCESS;

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (NULL == hdl)
    {
        return SCI_ERR_NULL_PTR;
    }
    if (NULL == p_args)
    {
        if (SCI_CMD_CHANGE_BAUD == cmd || SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ == cmd || SCI_CMD_TX_Q_BYTES_FREE == cmd)
        {
            return SCI_ERR_NULL_PTR;
        }
    }
    if (SCI_MODE_OFF == hdl->mode || SCI_MODE_MAX <= hdl->mode)
    {
        return SCI_ERR_BAD_MODE;
    }
#endif

    switch(cmd)
    {
        case SCI_CMD_CHANGE_BAUD:
        {

            break;
        }
        case SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ:
        {
            R_BYTEQ_Used(hdl->rx_bq_hdl, (uint16_t *)p_args);
            break;
        }
        case SCI_CMD_RX_Q_FLUSH:
        {
            R_BYTEQ_Flush(hdl->rx_bq_hdl);
            break;
        }
        case SCI_CMD_TX_Q_FLUSH:
        {
            R_BYTEQ_Flush(hdl->tx_bq_hdl);
            break;
        }
        case SCI_CMD_TX_Q_BYTES_FREE:
        {
            R_BYTEQ_Unused(hdl->tx_bq_hdl, (uint16_t *)p_args);
            break;
        }
    }

    return err;
}// R_SCI_Control

