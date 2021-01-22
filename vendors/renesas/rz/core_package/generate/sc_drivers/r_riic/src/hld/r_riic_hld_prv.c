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
 * File Name    : r_riic_hld_prv.c
 * Version      : See History
 * Description  : I2C high level device driver private file
 ******************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 06.02.2018 1.00    First Release
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "driver.h"            /* Device Driver common header */
#include "r_riic_hld_prv.h"     /* RIIC High level Driver Header */
#include "r_riic_drv_api.h"
#include "r_riic_hld_api.h"     /* defines and lld include */
#include "r_riic_drv_link.h"    /* Link include to low level driver */
#include "r_gpio_drv_api.h"

/*******************************************************************************
 Macro definitions
 *******************************************************************************/
/* IIC data read/write bit position */
#define SAMPLE_RIIC_RWCODE_PRV_        (0x01)

/* defines for channel enable status */
#define CHANNEL_ENABLED_PRV_    (1)
#define CHANNEL_DISABLED_PRV_   (0)

#define DRV_FLAG_OFF_PRV_ (0)
#define DRV_FLAG_ON_PRV_ (1)

/* set define to 1 to enable software timeout on operations (as opposed to RIIC timeout function */
#define SW_TIMEOUT_ENABLED_PRV_  (0)

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
typedef enum riic_rx_mode
{
    RIIC_RX_MODE_NORMAL, RIIC_RX_MODE_ACK, RIIC_RX_MODE_LOW_HOLD, RIIC_RX_MODE_NACK, RIIC_RX_MODE_STOP
} e_riic_rx_mode_t;

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static int_t riic_single_write (int_t channel, uint8_t data, uint32_t mode);
static int_t riic_write (int_t channel, const uint8_t *pbuffer, uint32_t byte);
static int_t riic_read (int_t channel, uint8_t *pbuffer, uint32_t byte);
static int_t riic_receive (int_t channel, e_riic_rx_mode_t mode, uint8_t *pdata);
static int_t riic_transmit (int_t channel, uint8_t data);
static int_t transmit_start (int_t channel);
static int_t transmit_restart (int_t channel);

/* wait functions for completion of i2c operations */
static int_t wait_rx_full (int_t channel);
static int_t wait_tx_empty (int_t channel);
static int_t wait_tx_end (int_t channel, uint32_t mode);
static int_t wait_bus_master (int_t channel, uint32_t mode);
static int_t wait_stop (int_t channel);

/* timeout monitoring */
static void start_timeout_monitor (int_t channel);
static void stop_timeout_monitor (int_t channel);
static int_t get_timeout_status (int_t channel);

#if (1u == SW_TIMEOUT_ENABLED_PRV_)
static uint32_t gs_sw_timeout_counter = ICC_MAX_TIMEOUT;
#endif /* (1u == SW_TIMEOUT_ENABLED_PRV_) */

/* static array variable holding initialisation status of each low level channel */
static int_t gs_channel_initialise_state[RIIC_CFG_LLD_NUM_CHANNELS];

/* static variable holding overall initialisation status of all i2c channels */
static int_t gs_overall_initialise_state = 0u;

/* static array variables holding flag statuses for each channel */
static volatile uint8_t gs_riic_receive_full_flg[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile uint8_t gs_riic_transmit_empty_flg[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile uint8_t gs_riic_transmit_end_flg[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile uint8_t gs_riic_stop_flg[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile uint8_t gs_riic_start_flg[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile uint8_t gs_riic_nack_flg[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile uint8_t gs_riic_arbitration_flg[RIIC_CFG_LLD_NUM_CHANNELS];
static volatile uint8_t gs_riic_timeout_flg[RIIC_CFG_LLD_NUM_CHANNELS];

/**
 * @brief riic_setup set up the RIIC driver GPIO Pins
 *
 * @param[in] p_config Pointer to configuration struct.
 *
 * @param[in] p_config Channel to setup.
 *
 * @retval 0  Success
 */
static int_t riic_setup (const st_r_drv_riic_sc_config_t *p_config)
{
    int_t gpio_handle = ( -1);
    gpio_handle = direct_open("gpio", 0);

    if (( -1) != gpio_handle)
    {
        /* direct_control as direct_control assumes parameter 3 is i/o, but using the parameter in input only mode is acceptable */
        direct_control(gpio_handle, CTL_GPIO_INIT_BY_TABLE, (st_r_drv_riic_sc_config_t *) &p_config->pin);
        direct_close(gpio_handle);
    }

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of Function riic_setup
 ******************************************************************************/

/**
 * @brief open the specified i2c peripheral channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_SUCCESS       : Success of RIIC operation
 *            DRV_ERROR         : Failure of RIIC operation
 *            DRV_ERROR : IIC operation timed out
 **/
int_t open_channel (int_t channel)
{
    int_t retval = DRV_ERROR;
    int_t sc_index = 0;
    int_t sc_loop = 0;
    int_t sc_table_size = (sizeof(RIIC_SC_TABLE)) / (sizeof(st_r_drv_riic_sc_config_t));

    for (sc_index = 0; (sc_loop < sc_table_size); sc_loop++)
    {
        if (RIIC_SC_TABLE[sc_loop].channel == channel)
        {
            sc_index = sc_loop;
            retval = DRV_SUCCESS;
        }
    }

    if (DRV_SUCCESS == retval)
    {
        /* configure channel */
        retval = riic_setup( &(RIIC_SC_TABLE[sc_index]));

        /* Initialise flags */
        gs_riic_receive_full_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_transmit_empty_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_transmit_end_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_stop_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_start_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_nack_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_arbitration_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_timeout_flg[channel] = DRV_FLAG_OFF_PRV_;
    }

    /* return status */
    return (retval);
}
/*******************************************************************************
 End of function open_channel
 ******************************************************************************/

/**
 * @brief Close the specified i2c channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 **/
void close_channel (int_t channel)
{
    uint32_t loop;
    uint32_t count = 0u;

    /* ensure that channel is valid */
    int_t ret = validate_channel(channel);

    if (DRV_SUCCESS == ret)
    {
        /* mark channel as un-initialised */
        gs_channel_initialise_state[channel] = CHANNEL_DISABLED_PRV_;

        /* call to low level driver to turn channel off */
        R_RIIC_CloseChannel(channel);

        /* test if any channels are still active */
        for (loop = 0; loop < RIIC_CFG_LLD_NUM_CHANNELS; loop++)
        {
            if (CHANNEL_DISABLED_PRV_ != gs_channel_initialise_state[loop])
            {
                count++;
            }
        }

        /* If no channels are open, set the overall initialisation as zero */
        if (0u == count)
        {
            gs_overall_initialise_state = CHANNEL_DISABLED_PRV_;
        }
    }
}
/*******************************************************************************
 End of function close_channel
 ******************************************************************************/

/**
 * @brief Writes 1 byte only of data to specified I2c channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] data    : data byte
 * @param[in] mode    : RIIC_TEND_WAIT_TRANSMIT : Transmission Mode
 *                    : RIIC_TEND_WAIT_RECEIVE  : Receive Mode
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 *            DRV_ERROR   : RIIC operation timed out
 **/
static int_t riic_single_write (int_t channel, uint8_t data, uint32_t mode)
{
    int_t ret;

    /* Write single byte */
    ret = riic_transmit(channel, data);

    if (DRV_SUCCESS == ret)
    {
        /* wait for end of transmission */
        ret = wait_tx_end(channel, mode);

        if (DRV_SUCCESS == ret)
        {
            /* Check for transmission status */
            if (DRV_FLAG_OFF_PRV_ == gs_riic_nack_flg[channel])
            {
                /* ACK */
                ret = DRV_SUCCESS;
            }
            else
            {
                /* NACK */
                gs_riic_nack_flg[channel] = DRV_FLAG_OFF_PRV_;
                ret = DRV_ERROR;
            }
        }
    }

    return (ret);
}
/******************************************************************************
 * End of Function riic_single_write
 ******************************************************************************/

/**
 * @brief Writes data to an already addressed device
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] pbuffer : pointer to allocated space for data
 * @param[in] byte    : number of bytes to receive
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 *            DRV_ERROR_RIIC_NACK : Failure of RIIC transmission due to
 *                                     NACK reception
 *            DRV_ERROR   : RIIC operation timed out
 **/
static int_t riic_write (int_t channel, const uint8_t *pbuffer, uint32_t byte)
{
    uint32_t offset;
    int_t ret = DRV_SUCCESS;

    if (0 < byte)
    {
        for (offset = 0; offset < byte; offset++)
        {
            /* Single byte transmission */
            ret = riic_transmit(channel, *(pbuffer + offset));

            if (DRV_SUCCESS != ret)
            {
                /* Exit the loop */
                offset = byte;
            }
        }

        if (DRV_SUCCESS == ret)
        {
            ret = wait_tx_end(channel, RIIC_TEND_WAIT_TRANSMIT);

            if (DRV_SUCCESS == ret)
            {
                /* clear flag */
                gs_riic_transmit_end_flg[channel] = DRV_FLAG_OFF_PRV_;

                /* test for ACK */
                if (DRV_FLAG_OFF_PRV_ == gs_riic_nack_flg[channel])
                {
                    /* ACK received */
                    ret = DRV_SUCCESS;
                }
                else
                {
                    /* NACK */

                    /* reset flag */
                    gs_riic_nack_flg[channel] = DRV_FLAG_OFF_PRV_;
                    ret = DRV_ERROR;
                }
            }
        }
    }

    /* return status */
    return (ret);
}
/******************************************************************************
 * End of Function riic_write
 ******************************************************************************/

/**
 * @brief Reads data from an already addressed device
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] pbuffer : pointer to allocated space for data
 * @param[in] byte    : number of bytes to receive
 * @return    DRV_SUCCESS       : Success of RIIC operation
 *            DRV_ERROR         : Failure of RIIC operation
 *            DRV_ERROR : RIIC operation timed out
 **/
static int_t riic_read (int_t channel, uint8_t *pbuffer, uint32_t byte)
{
    uint8_t *p_buffer2;
    int_t ret;

    /* Ensure at least 1 byte is specified to receive */
    if (0 == byte)
    {
        return (DRV_SUCCESS);
    }

    p_buffer2 = pbuffer;

    /* 1 byte random read does not need a dummy read */
    if (1 == byte)
    {
        R_RIIC_AssertLowHold(channel);

        /* (In the case of only 1 byte) reception start */
        ret = riic_receive(channel, RIIC_RX_MODE_NACK, p_buffer2);

        if (DRV_SUCCESS != ret)
        {
            return (ret);
        }

        /* Last byte read */
        ret = riic_receive(channel, RIIC_RX_MODE_STOP, p_buffer2);

        if (DRV_SUCCESS != ret)
        {
            return (ret);
        }
    }
    else
    {
        /* Start reading data */
        /* dummy read for read trigger, p_buffer2 is written by next data */
        if (2 == byte)
        {
            /* set wait mode on read of the (last - 2) byte */
            ret = riic_receive(channel, RIIC_RX_MODE_LOW_HOLD, p_buffer2);
        }
        else
        {
            /* normal start trigger */
            ret = riic_receive(channel, RIIC_RX_MODE_ACK, p_buffer2);
        }

        if (DRV_SUCCESS != ret)
        {
            return (ret);
        }

        /* Reads data bytes */
        while (2 < byte)
        {
            if (3 == byte)
            {
                /* set wait mode on read of the (last - 2) byte */
                ret = riic_receive(channel, RIIC_RX_MODE_LOW_HOLD, p_buffer2);
            }
            else
            {
                /* request size is 3byte or more */
                ret = riic_receive(channel, RIIC_RX_MODE_ACK, p_buffer2);
            }

            if (DRV_SUCCESS != ret)
            {
                return (ret);
            }

            p_buffer2++;
            byte--;
        }

        /* transfer NACK on read of the (last - 1) byte */
        ret = riic_receive(channel, RIIC_RX_MODE_NACK, p_buffer2);
        if (DRV_SUCCESS != ret)
        {
            return (ret);
        }

        p_buffer2++;
        byte--;

        if (DRV_SUCCESS == ret)
        {
            /* Data read of the last byte */
            ret = riic_receive(channel, RIIC_RX_MODE_STOP, p_buffer2);
        }
    }

    /* Process next Read */
    R_RIIC_ReleaseLowHold(channel);

    if (DRV_SUCCESS == ret)
    {
        /* Stop condition detection confirmation */
        ret = wait_stop(channel);
    }

    return (ret);
}
/******************************************************************************
 * End of Function riic_read
 ******************************************************************************/

/**
 * @brief Receive data (single byte)
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] mode  : receive mode
 *                  : RIIC_RX_MODE_NORMAL: Normal mode
 *                  : RIIC_RX_MODE_ACK: ACK mode
 *                  : RIIC_RX_MODE_LOW_HOLD: Low hold mode
 *                  : RIIC_RX_MODE_NACK: NACK mode
 *                  : RIIC_RX_MODE_STOP: Stop condition mode
 *            pdata : received data
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR   : RIIC operation timed out
 **/
static int_t riic_receive (int_t channel, e_riic_rx_mode_t mode, uint8_t *pdata)
{
    int_t ret;

    /* Wait until Receive data full conditions and clear receive-data-full
     condition have been satisfied */
    ret = wait_rx_full(channel);

    if (DRV_SUCCESS == ret)
    {
        /* reset rx full flag prior to reception */
        gs_riic_receive_full_flg[channel] = DRV_FLAG_OFF_PRV_;

        switch (mode)
        {
            case RIIC_RX_MODE_ACK:
            {
                /* send ACK */
                R_RIIC_TransmitAck(channel);
            }
            break;

            case RIIC_RX_MODE_LOW_HOLD:
            {
                /* Period between ninth clock cycle and first
                 * clock cycle is held low */
                R_RIIC_AssertLowHold(channel);
            }
            break;

            case RIIC_RX_MODE_NACK:
            {
                /* send NACK */
                R_RIIC_TransmitNack(channel);
            }
            break;

            case RIIC_RX_MODE_STOP:
            {
                gs_riic_stop_flg[channel] = DRV_FLAG_OFF_PRV_;

                /* Stop condition request */
                R_RIIC_TransmitStop(channel);
            }
            break;
            case RIIC_RX_MODE_NORMAL:
            {
                /* Do Nothing */
                ret = DRV_SUCCESS;
            }
            break;
            default:
            {
                /* Invalid selection */
                ret = DRV_ERROR;
            }
            break;
        }

        /* Read data from wire */
        ( *pdata) = R_RIIC_ReadByte(channel);
    }

    /* report status */
    return (ret);
}
/*******************************************************************************
 End of function riic_receive
 *******************************************************************************/

/**
 * @brief Transmit data (single byte)
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] data    : data to transmit
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 *            DRV_ERROR   : RIIC operation timed out
 **/
static int_t riic_transmit (int_t channel, uint8_t data)
{
    int_t ret = DRV_ERROR;

    /* test for ACK */
    if (DRV_FLAG_OFF_PRV_ == gs_riic_nack_flg[channel])
    {
        /* ACK received */
        ret = DRV_SUCCESS;
    }
    else
    {
        /* NACK */

        /* reset flag */
        gs_riic_nack_flg[channel] = DRV_FLAG_OFF_PRV_;
        ret = DRV_ERROR;
    }

    if (DRV_SUCCESS == ret)
    {
        /* ensure no current traffic on bus */
        ret = wait_tx_empty(channel);

        if (DRV_SUCCESS == ret)
        {
            /* clear tx empty flag prior to tx */
            gs_riic_transmit_empty_flg[channel] = DRV_FLAG_OFF_PRV_;

            /* Write data to wire */
            R_RIIC_WriteByte(channel, data);
        }
    }

    return (ret);
}
/*******************************************************************************
 End of function riic_transmit
 *******************************************************************************/

/**
 * @brief Make the request to issue a start condition for the selected channel
 *        Perform the following sequence of actions
 *              1: Wait till bus is free
 *              2: Clear Receive data full flag
 *              3: Clear transmit end flag
 *              4: Clear transmit-data-empty flag
 *              5: Request to issue a start condition
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 **/
static int_t transmit_start (int_t channel)
{
    /* Wait for the bus to be free */
    int_t ret = wait_bus_master(channel, RIIC_BUS_MASTERSHIP_WAIT_FREE);

    /* Start timing for operation */
    start_timeout_monitor(channel);

    if (DRV_SUCCESS == ret)
    {
        /* reset flags */
        gs_riic_receive_full_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_transmit_empty_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_transmit_end_flg[channel] = DRV_FLAG_OFF_PRV_;

        /* initiate start condition */
        R_RIIC_TransmitStart(channel);
    }

    /* report status */
    return (ret);
}
/*******************************************************************************
 End of function transmit_start
 *******************************************************************************/

/**
 * @brief Make the request to issue a restart condition for the selected channel.
 *        Perform the following sequence of actions
 *              1: Wait till bus is busy
 *              2: Clear Receive data full flag
 *              3: Clear transmit end flag
 *              4: Clear transmit-data-empty flag
 *              5: Request to issue a restart condition
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 **/
static int_t transmit_restart (int_t channel)
{
    /* Wait for the bus to be busy */
    int_t ret = wait_bus_master(channel, RIIC_BUS_MASTERSHIP_WAIT_BUSY);

    /* Start timing for operation */
    start_timeout_monitor(channel);

    if (DRV_SUCCESS == ret)
    {
        /* reset flags */
        gs_riic_receive_full_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_transmit_empty_flg[channel] = DRV_FLAG_OFF_PRV_;
        gs_riic_transmit_end_flg[channel] = DRV_FLAG_OFF_PRV_;

        /* set restart condition on bus */
        R_RIIC_TransmitRestart(channel);
    }

    return (ret);
}
/*******************************************************************************
 End of function transmit_restart
 *******************************************************************************/

/**
 * @brief Read data from device
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] p_transfer          : pointer to transfer parameters
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 **/
int_t read_data (int_t channel, st_r_drv_riic_transfer_t *p_transfer)
{
    int_t ret = DRV_ERROR;
    uint8_t pw_buffer[10];
    uint8_t buffer_count = 0u;

    /* ensure that channel is valid */
    ret = validate_channel(channel);

    if (RIIC_SUB_ADDR_MAX <= p_transfer->sub_address_type)
    {
        /* unrecognised sub-address mode */
        ret = DRV_ERROR;
    }

    /* Construct header to send sub address (if required) to device & start operation */
    if (DRV_SUCCESS == ret)
    {
        if (RIIC_SUB_ADDR_NONE != p_transfer->sub_address_type)
        {
            /* Adding device addressing and write control (for sub-address) */
            pw_buffer[buffer_count++] =
                    (uint8_t) (p_transfer->device_address & ((uint8_t) ( ~SAMPLE_RIIC_RWCODE_PRV_)));

            /* Note that 10-bit addressing mode would add an extra byte here to hold the address LSB */
        }

        if (RIIC_SUB_ADDR_WIDTH_8_BITS == p_transfer->sub_address_type)
        {
            /* 8-bit address to be provided */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address & 0x00ff);
        }

        if (RIIC_SUB_ADDR_WIDTH_16_BITS == p_transfer->sub_address_type)
        {
            /* 16-bit address to be provided */

            /* Adding sub-address MSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 8);

            /* Adding sub-address LSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address & 0x00ff);
        }

        if (RIIC_SUB_ADDR_WIDTH_32_BITS == p_transfer->sub_address_type)
        {
            /* 32-bit address to be provided */

            /* Adding slave sub-address MSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 24);

            /* Adding slave sub-address 3rd byte */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 16);

            /* Adding sub-address 2nd byte */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 8);

            /* Adding sub-address LSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address & 0x00ff);
        }

        /* Setting slave device address and read control */
        pw_buffer[buffer_count++] = (uint8_t) (p_transfer->device_address | ((uint8_t) SAMPLE_RIIC_RWCODE_PRV_));

        /* Note that 10-bit addressing mode would add an extra byte here to hold the address LSB */
    }

    if (DRV_SUCCESS == ret)
    {
        /* wait for bus to be free */
        ret = wait_bus_master(channel, RIIC_BUS_MASTERSHIP_WAIT_FREE);
    }

    if (DRV_SUCCESS == ret)
    {
        /* start condition */
        ret = transmit_start(channel);
    }

    if (RIIC_SUB_ADDR_NONE != p_transfer->sub_address_type)
    {
        if (DRV_SUCCESS == ret)
        {
            /* write address header to device if required */
            ret = riic_write(channel, pw_buffer, (buffer_count - 1u));
        }

        /* Now setup read command */
        if (DRV_SUCCESS == ret)
        {
            ret = transmit_restart(channel);
        }
    }

    if (DRV_SUCCESS == ret)
    {
        /* issue read command */
        ret = riic_single_write(channel, pw_buffer[(buffer_count - 1u)], RIIC_TEND_WAIT_RECEIVE);
    }

    if (DRV_SUCCESS == ret)
    {
        /* read data */
        riic_read(channel, p_transfer->p_data_buffer, p_transfer->number_of_bytes);
    }
    else
    {
        gs_riic_stop_flg[channel] = DRV_FLAG_OFF_PRV_;

        /* exit cleanly in event of an error */
        R_RIIC_TransmitStop(channel);
    }

    return (ret);
}
/*******************************************************************************
 End of function read_data
 *******************************************************************************/

/**
 * @brief Read data from slave
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] d_adr     : Slave device address
 *            r_byte    : Number of bytes
 *            pr_buffer : buffer for data
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 **/
int_t read_next_data (int_t channel, uint8_t d_adr, uint32_t r_byte, uint8_t *pr_buffer)
{
    int_t ret;
    uint8_t pw_buffer;

    /* ensure that channel is valid */
    ret = validate_channel(channel);

    /* Setting slave device address and read control */
    pw_buffer = (uint8_t) (d_adr | ((uint8_t) SAMPLE_RIIC_RWCODE_PRV_));

    if (DRV_SUCCESS == ret)
    {
        /* wait for bus to be free */
        ret = wait_bus_master(channel, RIIC_BUS_MASTERSHIP_WAIT_FREE);
    }

    if (DRV_SUCCESS == ret)
    {
        /* start condition */
        ret = transmit_start(channel);
    }

    if (DRV_SUCCESS == ret)
    {
        ret = riic_single_write(channel, pw_buffer, RIIC_TEND_WAIT_RECEIVE);
    }

    if (DRV_SUCCESS == ret)
    {
        /* read data */
        riic_read(channel, pr_buffer, r_byte);
    }
    else
    {
        gs_riic_stop_flg[channel] = DRV_FLAG_OFF_PRV_;

        /* exit cleanly in event of an error */
        R_RIIC_TransmitStop(channel);
    }

    return (ret);
}
/*******************************************************************************
 End of function read_next_data
 *******************************************************************************/

/**
 * @brief Write data to slave in single byte addressing mode
 * @param[in] channel   : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] d_adr     : Slave device address
 *            w_adr     : Slave sub-address
 *            w_byte    : Number of bytes
 *            pw_buffer : buffer for data
 * @return    DRV_SUCCESS         : Success of RIIC operation
 *            DRV_ERROR           : Failure of RIIC operation
 *            DRV_ERROR_RIIC_NACK : Failure of RIIC transmission due to
 *                                     NACK reception
 *            DRV_ERROR   : IIC operation timed out
 **/
int_t write_data (int_t channel, st_r_drv_riic_transfer_t *p_transfer)
{
    int_t ret = DRV_ERROR;
    uint8_t pw_buffer[10];
    uint8_t buffer_count = 0u;

    /* ensure that channel is valid */
    ret = validate_channel(channel);

    if (RIIC_SUB_ADDR_MAX <= p_transfer->sub_address_type)
    {
        /* unrecognised sub-address mode */
        ret = DRV_ERROR;
    }

    /* Construct header to send sub address (if required) to device & start operation */
    if (DRV_SUCCESS == ret)
    {
        /* Adding device addressing and write control (for sub-address) */
        pw_buffer[buffer_count++] = (uint8_t) (p_transfer->device_address & ((uint8_t) ( ~SAMPLE_RIIC_RWCODE_PRV_)));

        /* Note that 10-bit addressing mode would add an extra byte here to hold the address LSB */

        if (RIIC_SUB_ADDR_WIDTH_8_BITS == p_transfer->sub_address_type)
        {
            /* 8-bit address to be provided */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address & 0x00ff);
        }

        if (RIIC_SUB_ADDR_WIDTH_16_BITS == p_transfer->sub_address_type)
        {
            /* 16-bit address to be provided */

            /* Adding sub-address MSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 8);

            /* Adding sub-address LSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address & 0x00ff);
        }

        if (RIIC_SUB_ADDR_WIDTH_32_BITS == p_transfer->sub_address_type)
        {
            /* 32-bit address to be provided */

            /* Adding slave sub-address MSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 24);

            /* Adding slave sub-address 3rd byte */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 16);

            /* Adding sub-address 2nd byte */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address >> 8);

            /* Adding sub-address LSB */
            pw_buffer[buffer_count++] = (uint8_t) (p_transfer->sub_address & 0x00ff);
        }
    }

    if (DRV_SUCCESS == ret)
    {
        /* wait for bus to be free */
        ret = wait_bus_master(channel, RIIC_BUS_MASTERSHIP_WAIT_FREE);
    }

    if (DRV_SUCCESS == ret)
    {
        /* start condition */
        ret = transmit_start(channel);
    }

    if (DRV_SUCCESS == ret)
    {
        /* write address header to device if required */
        ret = riic_write(channel, pw_buffer, buffer_count);
    }

    if (DRV_SUCCESS == ret)
    {
        /* write remaining bytes */
        ret = riic_write(channel, p_transfer->p_data_buffer, p_transfer->number_of_bytes);
    }

    gs_riic_stop_flg[channel] = DRV_FLAG_OFF_PRV_;

    /* stop condition */
    R_RIIC_TransmitStop(channel);

    /* wait for finish */
    wait_stop(channel);

    return (ret);
}
/*******************************************************************************
 End of function write_data
 *******************************************************************************/

/**
 * @brief Wait for transmit end flag set for selected channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] mode    : RIIC_TEND_WAIT_TRANSMIT
 *                    : RIIC_TEND_WAIT_RECEIVE
 * @return    DRV_ERROR or DRV_SUCCESS
 **/
static int_t wait_tx_end (int_t channel, uint32_t mode)
{
    int_t timeout_monitor = DRV_SUCCESS;

    /* Start timing for operation */
    start_timeout_monitor(channel);

    if (RIIC_TEND_WAIT_TRANSMIT == mode)
    {
        R_RIIC_EnableTransEndIntr(channel);
        /* Wait */
        while ((DRV_FLAG_OFF_PRV_ == gs_riic_transmit_end_flg[channel]) && (DRV_SUCCESS == timeout_monitor))
        {
            /* get timeout status */
            timeout_monitor = get_timeout_status(channel);
        }
        R_RIIC_DisableTransEndIntr(channel);
    }
    else
    {
        /* RIIC_TEND_WAIT_RECEIVE */
        while ((DRV_FLAG_OFF_PRV_ == gs_riic_receive_full_flg[channel]) && (DRV_SUCCESS == timeout_monitor))
        {
            /* get timeout status */
            timeout_monitor = get_timeout_status(channel);
        }
    }

    /* Stop timeout monitoring */
    stop_timeout_monitor(channel);

    /* return status */
    return (timeout_monitor);
}
/*******************************************************************************
 End of function wait_tx_end
 ******************************************************************************/

/**
 * @brief Wait for receive full flag set for selected channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_ERROR or DRV_SUCCESS
 **/
static int_t wait_rx_full (int_t channel)
{
    int_t timeout_monitor = DRV_SUCCESS;

    /* Start timing for operation */
    start_timeout_monitor(channel);

    /* Wait */
    while ((DRV_FLAG_OFF_PRV_ == gs_riic_receive_full_flg[channel]) && (DRV_SUCCESS == timeout_monitor))
    {
        /* get timeout status */
        timeout_monitor = get_timeout_status(channel);
    }

    /* Stop timeout monitoring */
    stop_timeout_monitor(channel);

    /* return status */
    return (timeout_monitor);
}
/*******************************************************************************
 End of function wait_rx_full
 *******************************************************************************/

/**
 * @brief Wait for transmit empty flag set for selected channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_ERROR or DRV_SUCCESS
 **/
static int_t wait_tx_empty (int_t channel)
{
    int_t timeout_monitor = DRV_SUCCESS;

    /* Start timing for operation */
    start_timeout_monitor(channel);

    /* Wait */
    while ((DRV_FLAG_OFF_PRV_ == gs_riic_transmit_empty_flg[channel]) && (DRV_SUCCESS == timeout_monitor))
    {
        /* get timeout status */
        timeout_monitor = get_timeout_status(channel);
    }

    /* Stop timeout monitoring */
    stop_timeout_monitor(channel);

    /* return status */
    return (timeout_monitor);
}
/*******************************************************************************
 End of function wait_tx_empty
 ******************************************************************************/

/**
 * @brief Wait for bus busy or bus free state for selected channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @param[in] mode    : RIIC_BUS_MASTERSHIP_WAIT_FREE : wait till bus free
 *                    : RIIC_BUS_MASTERSHIP_WAIT_BUSY : wait till bus busy
 * @return    DRV_ERROR or DRV_SUCCESS
 **/
static int_t wait_bus_master (int_t channel, uint32_t mode)
{
    int_t timeout_monitor = DRV_SUCCESS;
    uint8_t level_check = 0u;

    R_RIIC_ReleaseLowHold(channel);

    /* Start timing for operation */
    start_timeout_monitor(channel);

    if (RIIC_BUS_MASTERSHIP_WAIT_FREE == mode)
    {
        level_check = 1u;
    }

    while ((R_RIIC_IsBusBusy(channel) == level_check) && (DRV_SUCCESS == timeout_monitor))
    {
        /* get timeout status */
        timeout_monitor = get_timeout_status(channel);
    }

    /* Stop timeout monitoring */
    stop_timeout_monitor(channel);

    /* return status */
    return (timeout_monitor);
}
/*******************************************************************************
 End of function wait_bus_master
 ******************************************************************************/

/**
 * @brief Wait for stop bit state for selected channel
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_ERROR or DRV_SUCCESS
 **/
static int_t wait_stop (int_t channel)
{
    int_t timeout_monitor = DRV_SUCCESS;

    /* Start timing for operation */
    start_timeout_monitor(channel);

    /* Wait */
    while ((DRV_FLAG_OFF_PRV_ == gs_riic_stop_flg[channel]) && (DRV_SUCCESS == timeout_monitor))
    {
        /* get timeout status */
        timeout_monitor = get_timeout_status(channel);
    }

    /* Stop timeout monitoring */
    stop_timeout_monitor(channel);

    /* return status */
    return (timeout_monitor);
}
/*******************************************************************************
 End of function wait_stop
 *******************************************************************************/

/**
 * @brief Enable timeout monitor
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_ERROR or DRV_SUCCESS
 **/
static void start_timeout_monitor (int_t channel)
{
#if (1u == SW_TIMEOUT_ENABLED_PRV_)
    gs_sw_timeout_counter = ICC_MAX_TIMEOUT;

#else

    /* reset flag */
    gs_riic_timeout_flg[channel] = DRV_FLAG_OFF_PRV_;

    /* start timeout monitor */
    R_RIIC_DetectTimeoutStart(channel);

#endif /* SW_TIMEOUT_ENABLED_PRV_ */
}
/*******************************************************************************
 End of function start_timeout_monitor
 ******************************************************************************/

/**
 * @brief Disable timeout monitor
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    void
 **/
static void stop_timeout_monitor (int_t channel)
{
#if (1u == SW_TIMEOUT_ENABLED_PRV_)
    /* nothing to do for software timeout */
    ;

#else

    /* reset flag */
    gs_riic_timeout_flg[channel] = DRV_FLAG_OFF_PRV_;

    /* stop timeout monitor */
    R_RIIC_DetectTimeoutStop(channel);

#endif /* SW_TIMEOUT_ENABLED_PRV_ */
}
/*******************************************************************************
 End of function stop_timeout_monitor
 ******************************************************************************/

/**
 * @brief Determine timeout status
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 * @return    DRV_ERROR or DRV_SUCCESS
 **/
static int_t get_timeout_status (int_t channel)
{
    int_t status = DRV_SUCCESS;

#if (1u == SW_TIMEOUT_ENABLED_PRV_)

    /* decrement timer */
    if (gs_sw_timeout_counter)
    {
        gs_sw_timeout_counter--;
    }

    /* check for timeout */
    if (0u == gs_sw_timeout_counter)
    {
        status = DRV_ERROR;
    }

#else

    if (DRV_FLAG_OFF_PRV_ != gs_riic_timeout_flg[channel])
    {
        status = DRV_ERROR;
    }

    /* reset flag if set */
    gs_riic_timeout_flg[channel] = DRV_FLAG_OFF_PRV_;

#endif /* SW_TIMEOUT_ENABLED_PRV_ */

    /* return status */
    return (status);
}
/*******************************************************************************
 End of function get_timeout_status
 ******************************************************************************/

/**
 * @brief Hook for low level driver to indicate receive full for selected
 *        channel to the high level driver
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 **/
void r_riic_hld_set_rx_full (int_t channel)
{
    gs_riic_receive_full_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_rx_full
 *******************************************************************************/

/**
 * @brief Hook for the low level driver to indicate transmit empty for selected
 *        channel to the high level driver
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 **/
void r_riic_hld_set_tx_empty (int_t channel)
{
    gs_riic_transmit_empty_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_tx_empty
 *******************************************************************************/

/**
 * @brief Hook for the low level driver to indicate transmit end for selected
 *        channel to the high level driver
 * @param[in] channel : the device specific channel number (< RIIC_CFG_LLD_NUM_CHANNELS)
 **/
void r_riic_hld_set_tx_end (int_t channel)
{
    gs_riic_transmit_end_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_tx_end
 *******************************************************************************/

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
void r_riic_hld_set_stop_asserted (int_t channel)
{
    gs_riic_stop_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_stop_asserted
 ******************************************************************************/

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
void r_riic_hld_set_start_asserted (int_t channel)
{
    gs_riic_start_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_start_asserted
 ******************************************************************************/

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
void r_riic_hld_set_nack_asserted (int_t channel)
{
    gs_riic_nack_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_nack_asserted
 ******************************************************************************/

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
void r_riic_hld_set_arbitrate_assert (int_t channel)
{
    gs_riic_arbitration_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_arbitrate_assert
 ******************************************************************************/

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
void r_riic_hld_set_timeout_asserted (int_t channel)
{
    gs_riic_timeout_flg[channel] = DRV_FLAG_ON_PRV_;
}
/*******************************************************************************
 End of function r_riic_hld_set_timeout_asserted
 ******************************************************************************/

/* End of File */
