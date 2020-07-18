/*
 * r_sci_wrapper_private.h
 *
 *  Created on: 2020/05/15
 *      Author: hmU11282
 */

#ifndef R_SCI_WRAPPER_PRIVATE_H_
#define R_SCI_WRAPPER_PRIVATE_H_

#include "r_byteq_if.h"
#include "r_sci_wrapper_if.h"

typedef struct st_sci_ch_ctrl
{
    sci_ch_t    chan;
    sci_mode_t  mode;
    uint32_t    boud_rate;
    void        (*callback)(void *p_args);
    byteq_hdl_t tx_bq_hdl;
    byteq_hdl_t rx_bq_hdl;
    bool        tx_idle;
} sci_ch_ctrl_t;

sci_err_t sci_chan_param_check(uint8_t const chan);

byteq_err_t put_tx_q(sci_ch_t chan, uint8_t byte);

void te_handler(sci_ch_t chan);
void sci_transfer(sci_hdl_t hdl);

void sci_receive(sci_ch_t chan, uint8_t byte);

#endif /* R_SCI_WRAPPER_PRIVATE_H_ */
