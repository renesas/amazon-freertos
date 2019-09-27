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
 * @headerfile     r_riic_hld_prv.h
 * @brief          RIIC high level driver private include header
 * @version        See history

 * H/W Platform    Platform independent
 *****************************************************************************/
/*****************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *              : 01.08.2019 1.03 Bump version number for V5 release
 *****************************************************************************/

/*************************************************************************//**
 * @ingroup SSGSTARC_56_RIIC_API
 * @defgroup SSGSTARC_56_RIIC_HLD_PRV RIIC High Level Driver Private Functions
 * @brief Private header for the RIIC High Level Driver.
 * See @ref SSGSTARC_56_RIIC_HLD_API for details.
 *
 * @anchor SSGSTARC_56_RIIC_PRV_SUMMARY
 * @par Summary
 * @brief Private header for the RIIC High Level Driver.
 *
 * @anchor SSGSTARC_56_RIIC_PRV_LIMITATIONS
 * @par Known Limitations
 * @ref NONE_YET
 *
 * @anchor SSGSTARC_56_RIIC_PRV_INSTANCES
 * @par Known Implementations
 * @ref RENESAS_APPLICATION_SOFTWARE_PACKAGE
 * @{
 *****************************************************************************/

#include "r_typedefs.h"
#include "r_riic_drv_sc_cfg.h"
#include "r_riic_drv_api.h"

/* Multiple inclusion prevention macro */
#ifndef R_RIIC_HLD_PRV_H
    #define R_RIIC_HLD_PRV_H

/*******************************************************************************
 Macro definitions
 *******************************************************************************/
/** ==== Transmission mode ==== */
    #define RIIC_TX_MODE_START      (0) /** Start condition issuance request   */
    #define RIIC_TX_MODE_RESTART    (1) /** Restart condition issuance request */
    #define RIIC_TX_MODE_STOP       (2) /** Stop condition issuance request    */

/** software loop timeout for i2c operations */
    #define ICC_MAX_TIMEOUT         (20000L)

/** ==== Wait operation after transmitting slave address ==== */
    #define RIIC_TEND_WAIT_TRANSMIT (0) /** Wait for the continuation of transmission mode to have been transmitted */
    #define RIIC_TEND_WAIT_RECEIVE  (1) /** Wait for the transition of reception mode to have been transmitted      */

/** ==== Bus mastership wait mode ==== */
    #define RIIC_BUS_MASTERSHIP_WAIT_FREE   (0) /** Wait for bus free */
    #define RIIC_BUS_MASTERSHIP_WAIT_BUSY   (1) /** Wait for bus busy */

/** ==== Return values ==== */
    #define DEVDRV_ERROR_RIIC_NACK  (-2)    /** NACK reception (No acknowledge from the slave device) */

/******************************************************************************
 Functions Prototypes
 ******************************************************************************/

/* High level functions accessed from devlink files */

/**
 * @brief open the specified i2c peripheral channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DEVDRV_SUCCESS       : Success of RIIC operation
 *            DEVDRV_ERROR         : Failure of RIIC operation
 **/
int_t open_channel (int_t channel);

/**
 * @brief Close the specified i2c channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 **/
void close_channel (int_t channel);

/**
 * @brief Read data from slave in single byte addressing mode
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] p_transfer : transfer parameters
 * @return    DEVDRV_SUCCESS         : Success of RIIC operation
 *            DEVDRV_ERROR           : Failure of RIIC operation
 **/
int_t read_data (int_t channel, st_r_drv_riic_transfer_t *p_transfer);

/**
 * @brief Read data from slave
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] d_adr     : Slave device address
 *            r_byte    : Number of bytes
 *            pr_buffer : buffer for data
 * @return    DEVDRV_SUCCESS         : Success of RIIC operation
 *            DEVDRV_ERROR           : Failure of RIIC operation
 **/
int_t read_next_data (int_t channel, uint8_t d_adr, uint32_t r_byte, uint8_t *r_buffer);

/**
 * @brief Write data to slave in single byte addressing mode
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] p_transfer : transfer parameters
 * @return    DEVDRV_SUCCESS         : Success of RIIC operation
 *            DEVDRV_ERROR           : Failure of RIIC operation
 **/
int_t write_data (int_t channel, st_r_drv_riic_transfer_t *p_transfer);

/**
 * @brief Ensure RIIC channel is supported
 * @param[in] channel   : the device specific channel number (in RIIC_CFG_LLD_SUPPORTED_CHANNELS)
 * @return    DEVDRV_SUCCESS         : Channel found
 *            DEVDRV_ERROR           : Channel not found
 **/
int_t validate_channel (int_t channel);

#endif  /* R_RIIC_HLD_PRV_H */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/* End of File */
