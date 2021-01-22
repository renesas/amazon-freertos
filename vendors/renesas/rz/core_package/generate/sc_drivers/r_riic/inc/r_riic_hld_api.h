/******************************************************************************
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
 * and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/

/******************************************************************************
 * @headerfile     r_riic_hld_api.h
 * @brief          High Level to Low Level Interface for RIIC
 * @version        See history
 * H/W Platform    Platform independent
 *****************************************************************************/
/*****************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *              : 01.08.2019 1.03 Bump version number for V5 release
 *****************************************************************************/

/**************************************************************************//**
 * @ingroup SSGSTARC_56_RIIC_API
 * @defgroup RIIC_HLD_LLD_IF RIIC Driver High to Low level Interface
 * @brief This is the interface of callback functions for use low level ISRs
 * for the RIIC driver.
 * @anchor RIIC_HLD_LLD_IF_SUMMARY
 * @par Summary
 * @brief This file provides function definitions for the high-level functions
 * to handle conditions such as receive buffer full, transmit buffer empty,
 * transmit end. They should be connected to the relevant interrupt handlers in
 * the low level driver.
 *
 * @anchor RIIC_DRIVER_HLD_API_LIMITATIONS
 * @par Known Limitations
 * @ref NONE_YET
 *
 * @anchor SSGSTARC_56_RIIC_API_INSTANCES
 * @par Known Implementations:
 * @ref RENESAS_APPLICATION_SOFTWARE_PACKAGE
 * @{
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"

/* Multiple inclusion prevention macro */
#ifndef RENESAS_DRIVERS_R_RIIC_INC_R_RIIC_API_H_
    #define RENESAS_DRIVERS_R_RIIC_INC_R_RIIC_API_H_

/*******************************************************************************
 Macro definitions
 *******************************************************************************/

/******************************************************************************
 Functions Prototypes
 ******************************************************************************/

/* api hooks for low level driver to set flags in high level driver */

/**
 * @brief Function to inform high layer driver when the RIIC channel's receive
 * buffer is full. This function should be called by the low layer driver
 * in it's receive buffer-full interrupt service routine for each channel in use.
 * It informs the high layer driver that the receive buffer is full and to
 * act appropriately.
 *
 * @param[in] channel RIIC channel that has that has a full receive buffer
 *                    (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing 
 */
void r_riic_hld_set_rx_full (int_t channel);

/**
 * @brief Function to inform high layer driver when the RIIC channel's transmit
 * buffer is empty. This function should be called by the low layer driver in
 * it's transmit buffer-empty interrupt service routine for each channel in use.
 * It informs the high layer driver that the transmit buffer is empty and to
 * act appropriately.
 *
 * @param[in] channel RIIC channel that has that has an empty transmit buffer
 *                    (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing 
 */
void r_riic_hld_set_tx_empty (int_t channel);

/**
 * @brief Function to inform high layer driver when the RIIC channel's transmit
 * operation has completed. This function should be called by the low layer
 * driver in it's transmit end interrupt service routine for each channel in use.
 * It informs the high layer driver that the transmission has completed and to
 * act appropriately.
 *
 * @param[in] channel RIIC channel that has that has completed transmission
 *                    (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing 
 */
void r_riic_hld_set_tx_end (int_t channel);

/**
 * @brief Function to inform high layer driver when the RIIC channel's stop bit
 * has been asserted. This function should be called by the low layer
 * driver in it's stop bit asserted interrupt service routine for each channel
 * in use.
 * It informs the high layer driver that a stop bit has been detected and to
 * act appropriately.
 *
 * @param[in] channel RIIC channel (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing
 */
void r_riic_hld_set_stop_asserted (int_t channel);

/**
 * @brief Function to inform high layer driver when the RIIC channel's start bit
 * has been asserted. This function should be called by the low layer
 * driver in it's start bit asserted interrupt service routine for each channel
 * in use.
 * It informs the high layer driver that a start bit has been detected and to
 * act appropriately.
 *
 * @param[in] channel RIIC channel (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing
 */
void r_riic_hld_set_start_asserted (int_t channel);

/**
 * @brief Function to inform high layer driver when the RIIC channel's NACK
 * has been asserted. This function should be called by the low layer
 * driver in it's NACK interrupt service routine for each channel
 * in use.
 * It informs the high layer driver that a NACK has been detected and to
 * act appropriately.
 *
 * @param[in] channel RIIC channel (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing
 */
void r_riic_hld_set_nack_asserted (int_t channel);

/**
 * @brief Function to inform high layer driver when a RIIC channel
 * arbitration failure has been detected. This function should be called
 * by the low layer driver in it's arbitration failure interrupt service
 * routine for each channel in use.
 * It informs the high layer driver that a arbitration failure has been
 * detected and to act appropriately.
 *
 * @param[in] channel RIIC channel (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing
 */
void r_riic_hld_set_arbitrate_assert (int_t channel);

/**
 * @brief Function to inform high layer driver when a RIIC channel
 * timeout has occurred. This function should be called
 * by the low layer driver in it's timeout interrupt service
 * routine for each channel in use.
 * It informs the high layer driver that a timeout has been
 * detected and to act appropriately.
 *
 * @param[in] channel RIIC channel (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return Nothing
 */
void r_riic_hld_set_timeout_asserted (int_t channel);

#endif  /* RENESAS_DRIVERS_R_RIIC_INC_R_RIIC_API_H_ */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/* End of File */
