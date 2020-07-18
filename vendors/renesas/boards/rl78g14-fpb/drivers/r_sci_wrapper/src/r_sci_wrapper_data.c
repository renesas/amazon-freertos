/*
 * r_sci_wrapper_data.c
 *
 *  Created on: 2020/05/15
 *      Author: hmU11282
 */

#include "r_sci_wrapper_private.h"

#if (SCI_CFG_CH0_INCLUDED)
sci_ch_ctrl_t ch0_ctrl = {SCI_CH0, SCI_MODE_OFF, 0, NULL, NULL, NULL, true};
#endif
#if (SCI_CFG_CH1_INCLUDED)
sci_ch_ctrl_t ch1_ctrl = {SCI_CH1, SCI_MODE_OFF, 0, NULL, NULL, NULL, true};
#endif
#if (SCI_CFG_CH2_INCLUDED)
sci_ch_ctrl_t ch2_ctrl = {SCI_CH2, SCI_MODE_OFF, 0, NULL, NULL, NULL, true};
#endif
#if (SCI_CFG_CH3_INCLUDED)
sci_ch_ctrl_t ch3_ctrl = {SCI_CH3, SCI_MODE_OFF, 0, NULL, NULL, NULL, true};
#endif

const sci_hdl_t g_handles[SCI_NUM_CH] =
{
#if (SCI_CFG_CH0_INCLUDED)
    &ch0_ctrl,
#else
	NULL,
#endif
#if (SCI_CFG_CH1_INCLUDED)
    &ch1_ctrl,
#else
	NULL,
#endif
#if (SCI_CFG_CH2_INCLUDED)
    &ch2_ctrl,
#else
	NULL,
#endif
#if (SCI_CFG_CH3_INCLUDED)
    &ch3_ctrl,
#else
	NULL,
#endif
};

sci_err_t sci_chan_param_check(uint8_t const chan)
{
    if (SCI_NUM_CH <= chan)
    {
        return SCI_ERR_BAD_CHAN;
    }
    return SCI_SUCCESS;
}

byteq_err_t put_tx_q(sci_ch_t chan, uint8_t byte)
{
    byteq_err_t err = BYTEQ_ERR_QUEUE_FULL;

    err = R_BYTEQ_Put(g_handles[chan]->tx_bq_hdl, byte);

    return err;
}

void tx_resume(sci_ch_t chan)
{
	sci_hdl_t shdl;
	uint16_t num;

	shdl = g_handles[chan];

	R_BYTEQ_Used(shdl->tx_bq_hdl, &num);
	if (0 < num)
	{
		sci_transfer(shdl);
	}
}

void te_handler(sci_ch_t chan)
{
    sci_transfer(g_handles[chan]);
}

void sci_transfer(sci_hdl_t hdl)
{
    uint16_t num;
    uint8_t byte;

    R_BYTEQ_Used(hdl->tx_bq_hdl, &num);
    if (0 >= num)
    {
        hdl->tx_idle = true;

        sci_cb_args_t arg;
        arg.event = SCI_EVT_TEI;

        if (NULL != hdl->callback)
        {
            arg.hdl = hdl;
            hdl->callback((void *)&arg);
        }
    }
    else
    {
        (void)R_BYTEQ_Get(hdl->tx_bq_hdl, (uint8_t *)&byte);

        switch(hdl->chan)
        {
#if SCI_CFG_CH0_INCLUDED
            case SCI_CH0:
            {
                while(0 != SSR00 & 0x0020);
                TXD0 = byte;
                break;
            }
#endif
#if SCI_CFG_CH1_INCLUDED
            case SCI_CH1:
            {
                while(0 != SSR02 & 0x0020);
                TXD1 = byte;
                break;
            }
#endif
#if SCI_CFG_CH2_INCLUDED
            case SCI_CH2:
            {
                while(0 != SSR10 & 0x0020);
                TXD2 = byte;
                break;
            }
#endif
#if SCI_CFG_CH3_INCLUDED
            case SCI_CH3:
            {
                while(0 != SSR12 & 0x0020);
                TXD3 = byte;
                break;
            }
#endif
    	}
    }
} // sci_transfer


void sci_receive(sci_ch_t chan, uint8_t byte)
{
    byteq_err_t err = BYTEQ_ERR_QUEUE_FULL;
    sci_cb_args_t arg;

    err = R_BYTEQ_Put(g_handles[chan]->rx_bq_hdl, byte);
    if (BYTEQ_SUCCESS == err)
    {
        arg.event = SCI_EVT_RX_CHAR;
    }
    else
    {
        arg.event = SCI_EVT_RXBUF_OVFL;
    }

    if (NULL != g_handles[chan]->callback)
    {
        arg.hdl = g_handles[chan];
        arg.byte = byte;

        g_handles[chan]->callback((void *)&arg);
    }
}

