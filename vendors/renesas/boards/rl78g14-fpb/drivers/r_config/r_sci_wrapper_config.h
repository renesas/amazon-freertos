/*
 * r_sci_wrapper_config.h
 *
 *  Created on: 2020/05/15
 *      Author: hmU11282
 */

#ifndef R_SCI_WRAPPER_CONFIG_H_
#define R_SCI_WRAPPER_CONFIG_H_

#include "rl_platform.h"

#define SCI_CFG_PARAM_CHECKING_ENABLE   (BSP_CFG_PARAM_CHECKING_ENABLE)

#define SCI_CFG_TEI_INCLUDED            (1)

#define SCI_CFG_CH0_INCLUDED            (1)
#define SCI_CFG_CH1_INCLUDED            (0)
#define SCI_CFG_CH2_INCLUDED            (1)
#define SCI_CFG_CH3_INCLUDED            (1)

#define SCI_CFG_CH0_TX_BUFSIZ           (512) /* (1024) */
#define SCI_CFG_CH1_TX_BUFSIZ           (80)
#define SCI_CFG_CH2_TX_BUFSIZ           (128)
#define SCI_CFG_CH3_TX_BUFSIZ           (128) /* (256) */

#define SCI_CFG_CH0_RX_BUFSIZ           (512) /* (1024) */
#define SCI_CFG_CH1_RX_BUFSIZ           (80)
#define SCI_CFG_CH2_RX_BUFSIZ           (128)
#define SCI_CFG_CH3_RX_BUFSIZ           (80)

#endif /* R_SCI_WRAPPER_CONFIG_H_ */
