/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013(2014-2019) Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_rspi_rx.c
* Device(s)    : RX Family
* Tool-Chain   : Renesas RX Standard Toolchain 3.01.00
* OS           : None
* H/W Platform :
* Description  : Functions for using RSPI on RX devices.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 25.10.2013 1.00     First Release
*           01.05.2014 1.20     Added support for RX62N. Minor bug fixes.
*           19.01.2014 1.30     Added support for RX113, RX64M, and RX71M
*                               Fixed bug in R_RSPI_Control(). Now allow ABORT command while locking is enabled.
*           30.09.2016 1.50     Added support for RX130, RX23T, RX24T and RX65N.
*                               Supported big endian.
*                               Added nop() to wait for the write completion.
*           31.03.2017 1.60     Added support for RX24U.
*           31.07.2017 1.70     Supported RX65N-2MB and RX130-512KB.
*                               Fixed to correspond to Renesas coding rule.
*           20.09.2018 1.80     Supported RX66T.
*           20.12.2018 2.00     Added double buffer and dmadtc transfer mode.
*                               Supported RX72T.
*         : 04.04.2019 2.01     Added support for GNUC and ICCRX.
*                               Fixed coding style. 
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Defines for RSPI support */
#include "platform.h"
#include "r_rspi_rx_if.h"

#if RSPI_CFG_LONGQ_ENABLE == 1
/* Uses LONGQ driver headef file. */
#include "r_longq_if.h"
#endif /* RSPI_CFG_LONGQ_ENABLE */

#define RSPI_DATA_ALIGN            (0x00000003u)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
  /* Values will be used as bit flags.*/
    RSPI_DO_TX    = 0x1,
    RSPI_DO_RX    = 0x2,
    RSPI_DO_TX_RX = 0x3
} rspi_operation_t;

typedef struct rspi_tcb_s
{
   void     *psrc;
   void     *pdest;
   uint16_t tx_count;
   uint16_t rx_count;
   uint16_t xfr_length;
   uint8_t  bytes_per_transfer;     // Source buffer bytes per transfer: 1, 2, or 4.
   bool     do_rx_now;              // State flag for valid read data available.
   bool     do_tx;                  // State flag for transmit operation.
   rspi_operation_t  transfer_mode; // Transmit only, receive only, or transmit-receive.
   #if RSPI_CFG_MASK_UNUSED_BITS == (1)
   uint32_t unused_bits_mask;       // For masking the unused upper bits of non power-of-2 data.
   #endif
   rspi_str_tranmode_t data_tran_mode; // Data transfer mode
} rspi_tcb_t;

/* Driver internal shadow copy of register settings. */
typedef struct rspi_ctrl_reg_values_s
{
    uint8_t spcr_val;   // RSPI Control Register (SPCR).
    uint8_t sslp_val;   // RSPI Slave Select Polarity Register (SSLP)
    uint8_t sppcr_val;  // RSPI Pin Control Register (SPPCR)
    uint8_t spscr_val;  // RSPI Sequence Control Register (SPSCR)
    uint8_t spbr_val;   // RSPI Bit Rate Register (SPBR).
    uint8_t spdcr_val;  // RSPI Data Control Register (SPDCR)
    uint8_t spckd_val;  // RSPI Clock Delay Register (SPCKD)
    uint8_t sslnd_val;  // RSPI Slave Select Negation Delay Register (SSLND)
    uint8_t spnd_val;   // RSPI Next-Access Delay Register (SPND)
    uint8_t spcr2_val;  // RSPI Control Register 2 (SPCR2)
    uint8_t spdcr2_val; // RSPI Data Control Register 2 (SPDCR2)
} rspi_ctrl_reg_values_t;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#if RSPI_CFG_USE_CHAN2 == 1
    #define RSPI_NUM_CHANNELS (3)
#elif RSPI_CFG_USE_CHAN1 == 1
    #define RSPI_NUM_CHANNELS (2)
#elif RSPI_CFG_USE_CHAN0 == 1
    #define RSPI_NUM_CHANNELS (1)
#else
    #error "ERROR in r_rspi_rx configuration. Must enable at least 1 channel for use."
#endif

#define RSPI_POWER_ON (0)
#define RSPI_POWER_OFF (1)

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#if  RSPI_CFG_LONGQ_ENABLE == 1
static longq_hdl_t       p_rspi_long_que;     // LONGQ handler
#endif  /* RSPI_CFG_LONGQ_ENABLE */

static void r_rspi_get_buffregaddress(uint8_t channel, uint32_t *p_spdr_adr);
static rspi_err_t r_rspi_ch_check(uint8_t channel);

/* Array of channel handles. One for each physical RSPI channel on the device. */
static struct rspi_config_block_s g_rspi_handles[RSPI_NUM_CHANNELS];

/* Used to prevent having duplicate code for each channel. This only works if the channels are identical (just at 
   different locations in memory). This is easy to tell by looking in iodefine.h and seeing if the same structure
   was used for all channels. */
volatile struct st_rspi R_BSP_EVENACCESS_SFR * g_rspi_channels[RSPI_NUM_CHANNELS] =
{
/* Initialize the array for up to 3 channels. Add more as needed. */
#if   RSPI_NUM_CHANNELS == 1
    &RSPI0,
#elif RSPI_NUM_CHANNELS == 2
    &RSPI0, &RSPI1
#elif RSPI_NUM_CHANNELS == 3
    &RSPI0, &RSPI1, &RSPI2
#endif
};

static volatile uint32_t g_rxdata[RSPI_NUM_CHANNELS]; // Space for fast read of RSPI RX data register.

/* Allocate transfer control blocks for all channels. */
static struct rspi_tcb_s g_rspi_tcb[RSPI_NUM_CHANNELS] = {0};
/* Allocate transaction result code storage for all channels. */
static rspi_callback_data_t g_rspi_cb_data[RSPI_NUM_CHANNELS] = {0};

/* Allocate register settings structure for all channels and initialize to defaults. */
static rspi_ctrl_reg_values_t g_ctrl_reg_values[] =
{
    RSPI_SPCR_DEF,      // Control Register (SPCR)
    RSPI_SSLP_DEF,      // Slave Select Polarity Register (SSLP)
    RSPI_SPPCR_DEF,     // Pin Control Register (SPPCR)
    RSPI_SPSCR_DEF,     // Sequence Control Register (SPSCR)
    RSPI_SPBR_DEF,      // Bit Rate Register (SPBR)
    RSPI_SPDCR_DEF,     // Data Control Register (SPDCR)
    RSPI_SPCKD_DEF,     // Clock Delay Register (SPCKD)
    RSPI_SSLND_DEF,     // Slave Select Negation Delay Register (SSLND)
    RSPI_SPND_DEF,      // Next-Access Delay Register (SPND)
    RSPI_SPCR2_DEF,     // Control Register 2 (SPCR2)
    RSPI_SPDCR2_DEF,    // Data Control Register 2 (SPDCR2)
#if   RSPI_NUM_CHANNELS > 1
    RSPI_SPCR_DEF, // Control Register (SPCR)
    RSPI_SSLP_DEF,      // Slave Select Polarity Register (SSLP)
    RSPI_SPPCR_DEF,     // Pin Control Register (SPPCR)
    RSPI_SPSCR_DEF,     // Sequence Control Register (SPSCR)
    RSPI_SPBR_DEF,      // Bit Rate Register (SPBR)
    RSPI_SPDCR_DEF,     // Data Control Register (SPDCR)
    RSPI_SPCKD_DEF,     // Clock Delay Register (SPCKD)
    RSPI_SSLND_DEF,     // Slave Select Negation Delay Register (SSLND)
    RSPI_SPND_DEF,      // Next-Access Delay Register (SPND)
    RSPI_SPCR2_DEF,     // Control Register 2 (SPCR2)
    RSPI_SPDCR2_DEF,    // Data Control Register 2 (SPDCR2)
#endif
#if   RSPI_NUM_CHANNELS >2
    RSPI_SPCR_DEF, // Control Register (SPCR)
    RSPI_SSLP_DEF,      // Slave Select Polarity Register (SSLP)
    RSPI_SPPCR_DEF,     // Pin Control Register (SPPCR)
    RSPI_SPSCR_DEF,     // Sequence Control Register (SPSCR)
    RSPI_SPBR_DEF,      // Bit Rate Register (SPBR)
    RSPI_SPDCR_DEF,     // Data Control Register (SPDCR)
    RSPI_SPCKD_DEF,     // Clock Delay Register (SPCKD)
    RSPI_SSLND_DEF,     // Slave Select Negation Delay Register (SSLND)
    RSPI_SPND_DEF,      // Next-Access Delay Register (SPND)
    RSPI_SPCR2_DEF,     // Control Register 2 (SPCR2)
    RSPI_SPDCR2_DEF,    // Data Control Register 2 (SPDCR2)
#endif
};

#if RSPI_CFG_MASK_UNUSED_BITS == (1)
/* This is a lookup table to hold bit masks for use when the
 * RSPI_CFG_MASK_UNUSED_BITS config option is enabled.
 * The bit-length specifier field in the SPCMD register is
 * used as the index to select the corresponding mask */
static const uint32_t g_unused_bits_masks[16] = {
    0x000FFFFF,     // 0x0 = 20 bits data length
    0x00FFFFFF,     // 0x1 = 24 bits data length
    0xFFFFFFFF,     // 0x2 = 32 bits data length
    0xFFFFFFFF,     // 0x3 = 32 bits data length
    0x000000FF,     // 0x4 = 8 bits data length
    0x000000FF,     // 0x5 = 8 bits data length
    0x000000FF,     // 0x6 = 8 bits data length
    0x000000FF,     // 0x7 = 8 bits data length
    0x000001FF,     // 0x8 = 9 bits data length
    0x000003FF,     // 0x9 = 10 bits data length
    0x000007FF,     // 0xA = 11 bits data length
    0x00000FFF,     // 0xB = 12 bits data length
    0x00001FFF,     // 0xC = 13 bits data length
    0x00003FFF,     // 0xD = 14 bits data length
    0x00007FFF,     // 0xE = 15 bits data length
    0x0000FFFF,     // 0xF = 16 bits data length
};
#endif

/***********************************************************************************************************************
Private function declarations
***********************************************************************************************************************/
/* Common routine used by RSPI API write or read functions. */
static rspi_err_t  rspi_write_read_common(rspi_handle_t handle,
                                          rspi_command_word_t command_word,
                                          void          *psrc,
                                          void          *pdest,
                                          uint16_t      length,
                                          rspi_operation_t   tx_rx_mode);
/* Sets the baud rate registers for a given frequency. */
static uint32_t rspi_baud_set(uint8_t channel, uint32_t baud_target);
/* Determines the primitive data type required for accessing a given RSPI data frame bit length. */
static uint8_t rspi_get_data_type(rspi_command_word_t frame_length_bits);
/* Common RSPI channel power-on utility. */
static void power_on_off (uint8_t channel, uint8_t on_or_off);
/* Set RSPI interrupt priorities. */
static void rspi_ir_priority_set(uint8_t channel, uint8_t rspi_priority);
/* Clear any pending RSPI interrupts. */
static void rspi_interrupts_clear(uint8_t channel);
/* Disable or enable RSPI interrupts. */
static void rspi_interrupts_enable(uint8_t channel, bool enabled);

static void rspi_spei_grp_isr(void *pdata);
/***********************************************************************************************************************
* Function Name: R_RSPI_Open
* Description  : This function applies power to the RSPI channel,
*                initializes the associated registers,
*                applies user-configurable options,
*                and provides the channel handle for use with other API functions.
* Arguments    : chan -
*                   Number of the RSPI channel to be initialized
*                pconfig -
*                   Pointer to RSPI channel configuration data structure.
*                pcallback -
*                   Pointer to function called from interrupt
*                phandle -
*                   Pointer to user-provided storage for a pointer to the handle data structure.
* Return Value : RSPI_SUCCESS-
*                   Successful; channel initialized
*                RSPI_ERR_BAD_CHAN-
*                   Channel number is invalid for part
*                RSPI_ERR_CH_NOT_CLOSED-
*                   Channel currently in operation; Perform R_RSPI_Close() first
*                RSPI_ERR_NULL_PTR-
*                   pconfig pointer or phandle pointer is NULL
*                RSPI_ERR_ARG_RANGE-
*                   An element of the pconfig structure contains an invalid value.
*                RSPI_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
rspi_err_t   R_RSPI_Open(uint8_t                channel,
                         rspi_chnl_settings_t  *pconfig,
                         rspi_command_word_t  spcmd_command_word,
                         void                 (*pcallback)(void *pcbdat),
                         rspi_handle_t         *phandle)
{
    rspi_ctrl_reg_values_t *my_settings = &(g_ctrl_reg_values[channel]);

    #if RSPI_CFG_REQUIRE_LOCK == 1
    bool        lock_result = false;
    #endif

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    /* Check channel number. */
    if (channel >= RSPI_NUM_CHANNELS)
    {
        /* Invalid channel. */
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_BAD_CHAN;
    }

    if ((NULL == pconfig) || (NULL == phandle))
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }

    /* Check to see if the peripheral has already been initialized. */
    if (true == g_rspi_handles[channel].rspi_chnl_opened)
    {
        /* This channel has already been initialized. */
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_CH_NOT_CLOSED;
    }
    #endif

    #if RSPI_CFG_REQUIRE_LOCK == 1
    /* Attempt to acquire lock for this RSPI channel. Prevents reentrancy conflict. */
    lock_result = R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));

    if (false == lock_result)
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_LOCK; /* The open function is currently locked. */
    }
    #endif

    power_on_off(channel, RSPI_POWER_ON);

    if (0 == channel)
    {
        rspi_ir_priority_set(channel, RSPI_CFG_IR_PRIORITY_CHAN0);
    }
    #if RSPI_NUM_CHANNELS > 1
    else if (1 == channel)
    {
        rspi_ir_priority_set(channel, RSPI_CFG_IR_PRIORITY_CHAN1);
    }
    #endif
    #if RSPI_NUM_CHANNELS > 2
    else if (2 == channel)
    {
        rspi_ir_priority_set(channel, RSPI_CFG_IR_PRIORITY_CHAN2);
    }
    #endif
    else
    {
        /* Nothing else. */
    }

    /* Disable interrupts in ICU. */
    rspi_interrupts_enable(channel, false);

    /* Set the base bit rate. Modifies the SPBR register setting with requested baud rate.*/
    if (0 == rspi_baud_set(channel, pconfig->bps_target))
    {
      /* Failed */
        #if RSPI_CFG_REQUIRE_LOCK == 1
        R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));
        #endif
          R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_ARG_RANGE; /* Could not calculate settings for the requested baud rate. */
    }

    /* Set pin control register (SPPCR) */
    (*g_rspi_channels[channel]).SPPCR.BYTE = (uint8_t)(my_settings->sppcr_val & RSPI_SPPCR_MASK);

    /* Set slave select polarity register (SSLP). */
    (*g_rspi_channels[channel]).SSLP.BYTE = (uint8_t)(my_settings->sslp_val & RSPI_SSLP_MASK);

    /* Apply the SPBR setting. */
    (*g_rspi_channels[channel]).SPBR = my_settings->spbr_val;

    /* Set RSPI data control register (SPDCR). Only SPLW bit supported in this ver. */
    /* Force to long word data access here regardless of user defined setting. */
    (*g_rspi_channels[channel]).SPDCR.BYTE = RSPI_SPDCR_SPLW;

    /* Set RSPI clock delay registers (SPCKD) */
    (*g_rspi_channels[channel]).SPCKD.BYTE = (uint8_t)(my_settings->spckd_val & RSPI_SPCKD_MASK);

    /* Set RSPI slave select negation delay register (SSLND) */
    (*g_rspi_channels[channel]).SSLND.BYTE = (uint8_t)(my_settings->sslnd_val & RSPI_SSLND_MASK);

    /* Set RSPI next-access delay register (SPND) */
    (*g_rspi_channels[channel]).SPND.BYTE = (uint8_t)(my_settings->spnd_val & RSPI_SPND_MASK);

    /* Set RSPI control register 2 (SPCR2) */
    (*g_rspi_channels[channel]).SPCR2.BYTE = (uint8_t)(my_settings->spcr2_val & RSPI_SPCR2_MASK);

    /* Update the SPCMD0 command register with the settings for this transfer. */
    (*g_rspi_channels[channel]).SPCMD0.BIT.CPOL = spcmd_command_word.cpol;

#if defined BSP_MCU_RX65N || defined BSP_MCU_RX66T || defined BSP_MCU_RX72T
    /* Set RSPI data control register 2 (SPDCR2) */
    (*g_rspi_channels[channel]).SPDCR2.BYTE = (uint8_t)(my_settings->spdcr2_val & RSPI_SPDCR2_MASK);
#endif

    /* Determine master/slave mode setting based on channel settings argument.
     * Overrides prior state for this bit . */
    if (RSPI_MS_MODE_MASTER == pconfig->master_slave_mode)
    {
        my_settings->spcr_val |= RSPI_MS_MODE_MASTER;   // Set the master mode bit
    }
    else
    {
        my_settings->spcr_val &= RSPI_MS_MODE_SLAVE;    // Clear the master mode bit
    }

    /* Determine RSPI slave select mode setting based on channel settings argument.
     * Overrides prior state for this bit . */
    if (RSPI_IF_MODE_4WIRE == pconfig->gpio_ssl)
    {
        my_settings->spcr_val &= RSPI_IF_MODE_4WIRE; // Clear the SPMS bit
    }
    else
    {
        my_settings->spcr_val |= RSPI_IF_MODE_3WIRE; // Set the SPMS bit
    }
    /* Set RSPI control register (SPCR) */
    (*g_rspi_channels[channel]).SPCR.BYTE = my_settings->spcr_val;
    if (0 == (*g_rspi_channels[channel]).SPCR.BYTE)
    {
        R_BSP_NOP();
    }
    
    /* Peripheral Initialized */
    g_rspi_handles[channel].rspi_chnl_opened = true;
    g_rspi_handles[channel].pcallback = pcallback;
    g_rspi_handles[channel].channel = channel;

    *phandle = &(g_rspi_handles[channel]);  // Return a pointer to the channel handle structure.

    #if RSPI_CFG_REQUIRE_LOCK == 1
    /* Release lock for this channel. */
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));
    #endif

    g_rspi_tcb[channel].data_tran_mode = pconfig->tran_mode;

    return RSPI_SUCCESS;
}
/* end of function R_RSPI_Open(). */


/***********************************************************************************************************************
* Function Name: R_RSPI_Control
* Description  : This function is responsible for handling special hardware or software operations for the RSPI channel.
* Arguments    : handle-
*                   Handle for the channel
*                cmd
*                   Enumerated command code
*                pcmd_data
*                   Pointer to the command-data structure parameter of type void that is used to reference the location
*                   of any data specific to the command that is needed for its completion.
* Return Value : RSPI_SUCCESS-
*                   Command successfully completed.
*                RSPI_ERR_CH_NOT_OPENED-
*                   The channel has not been opened.  Perform R_RSPI_Open() first
*                RSPI_ERR_UNKNOWN_CMD-
*                   Control command is not recognized.
*                RSPI_ERR_NULL_PTR-
*                   pcmd_data  pointer or handle is NULL
*                RSPI_ERR_ARG_RANGE-
*                   An element of the pcmd_data structure contains an invalid value.
*                RSPI_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
rspi_err_t  R_RSPI_Control(rspi_handle_t handle,
                           rspi_cmd_t     cmd,
                           void          *pcmd_data)
{
    /* Command function data structure definitions. One for each command in rspi_cmd_t. */
    rspi_cmd_baud_t        *p_baud_struct;
    rspi_cmd_setregs_t     *p_setregs_struct;
    uint8_t                 reg_temp = 0;
    uint8_t                 channel  = handle->channel;
    rspi_cmd_trans_mode_t  *p_tran_smode_struct;

    #if RSPI_CFG_REQUIRE_LOCK == 1
    bool        lock_result = false;
    #endif

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ((NULL == handle) || ((NULL == pcmd_data) && ((void *)FIT_NO_PTR != pcmd_data)))
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    if (!g_rspi_handles[channel].rspi_chnl_opened)
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_CH_NOT_OPENED;
    }
    #endif

    #if RSPI_CFG_REQUIRE_LOCK == 1
    /* Attempt to acquire lock for this RSPI channel. Prevents reentrancy conflict. */
    lock_result = R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));

    if ((false == lock_result)&&(RSPI_CMD_ABORT != cmd)) // Only abort cmd is allowed while transfer in progress.
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_LOCK; // The control function is currently locked.
    }
    #endif

    switch(cmd)
    {
        case RSPI_CMD_SET_BAUD:
        {
            p_baud_struct = (rspi_cmd_baud_t *)pcmd_data;

            reg_temp = (*g_rspi_channels[channel]).SPCR.BYTE; // Temporarily save state of the SPCR register.

            /* Temporarily disable the RSPI operation. */
            /* SPE and SPTIE should be cleared simultaneously. */
            (*g_rspi_channels[channel]).SPCR.BYTE = (uint8_t)(reg_temp & (~(RSPI_SPCR_SPTIE | RSPI_SPCR_SPE)));
            if (0 == (*g_rspi_channels[channel]).SPCR.BYTE)
            {
                R_BSP_NOP();
            }

            /* Update the baud rate. */
            /* Get the register settings for requested baud rate. */
            if (0 == rspi_baud_set(channel, p_baud_struct->bps_target))
            {
                #if RSPI_CFG_REQUIRE_LOCK == 1
                /* Release lock for this channel. */
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));
                #endif
                  R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
                return RSPI_ERR_ARG_RANGE; // Could not calculate settings for the requested baud rate.
            }

            (*g_rspi_channels[channel]).SPCR.BYTE = reg_temp; // Re-enable the RSPI operation.
            if (0 == (*g_rspi_channels[channel]).SPCR.BYTE)
            {
                R_BSP_NOP();
            }
        }
        break;

        case RSPI_CMD_ABORT:
        {
             /* Perform immediate abort of the active RSPI transfer on this channel.
             * Does not close the channel. */
            rspi_interrupts_enable(channel, false); // Disable interrupts in ICU.
            (*g_rspi_channels[channel]).SPCR.BIT.SPE = 0;  // Disable RSPI. Forces soft reset.
            if (0 == (*g_rspi_channels[channel]).SPCR.BIT.SPE)
            {
                R_BSP_NOP();
            }

            /* Transfer aborted. Call the user callback function passing pointer to the result structure. */
            if ((FIT_NO_FUNC != g_rspi_handles[channel].pcallback) && (NULL != g_rspi_handles[channel].pcallback))
            {
                g_rspi_cb_data[channel].handle = &(g_rspi_handles[channel]);
                g_rspi_cb_data[channel].event_code = RSPI_EVT_TRANSFER_ABORTED;
                g_rspi_handles[channel].pcallback((void*)&(g_rspi_cb_data[channel]));
            }

        }
        break;

        case RSPI_CMD_SETREGS:   /* Expert use only! Set all user supported RSPI regs in one operation. */
        {
       /* Overrides driver default settings.
       * Copies user-specified register settings into driver's shadow copy.
       * Settings do not take effect until the channel is closed and then reopened.
       */
            p_setregs_struct = (rspi_cmd_setregs_t *)pcmd_data;
        /* Set RSPI clock delay registers (SPCKD) */
            (*g_rspi_channels[channel]).SPCKD.BYTE = (uint8_t)(p_setregs_struct->spckd_val & RSPI_SPCKD_MASK);
        /* Set RSPI control register 2 (SPCR2) */
            (*g_rspi_channels[channel]).SPCR2.BYTE = (uint8_t)(p_setregs_struct->spcr2_val & RSPI_SPCR2_MASK);
        /* Set RSPI next-access delay register (SPND) */
            (*g_rspi_channels[channel]).SPND.BYTE = (uint8_t)(p_setregs_struct->spnd_val & RSPI_SPND_MASK);
                    /* Set pin control register (SPPCR) */
            (*g_rspi_channels[channel]).SPPCR.BYTE = (uint8_t)(p_setregs_struct->sppcr_val & RSPI_SPPCR_MASK);
        /* Set RSPI slave select negation delay register (SSLND) */
            (*g_rspi_channels[channel]).SSLND.BYTE = (uint8_t)(p_setregs_struct->sslnd_val & RSPI_SSLND_MASK);
        /* Set slave select polarity register (SSLP). */
            (*g_rspi_channels[channel]).SSLP.BYTE = (uint8_t)(p_setregs_struct->sslp_val & RSPI_SSLP_MASK);
        #if defined BSP_MCU_RX65N || defined BSP_MCU_RX66T || defined BSP_MCU_RX72T
            /* Set RSPI data control register 2 (SPDCR2) */
            (*g_rspi_channels[channel]).SPDCR2.BYTE = (uint8_t)(p_setregs_struct->spdcr2_val & RSPI_SPDCR2_MASK);
        #endif
        }
        break;

        case RSPI_CMD_SET_TRANS_MODE:
        {
            p_tran_smode_struct = (rspi_cmd_trans_mode_t*)pcmd_data;
            g_rspi_tcb[channel].data_tran_mode = p_tran_smode_struct->transfer_mode;
        }
        break;

        default:
        {
            #if RSPI_CFG_REQUIRE_LOCK == 1
            /* Release lock for this channel. */
            R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));
            #endif
            /* Error, command not recognized. */
              R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
            return RSPI_ERR_UNKNOWN_CMD;
        }
    }

    #if RSPI_CFG_REQUIRE_LOCK == 1
    /* Release lock for this channel. */
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));
    #endif

    return RSPI_SUCCESS;
}
/* end of function R_RSPI_Control(). */


/***********************************************************************************************************************
* Function Name: R_RSPI_Read
* Description  : Receives data from a SPI device.
* Arguments    : handle-
*                   Handle for the channel
*                spcmd_command_word-
*                   bitfield data consisting of all the RSPI command register settings for SPCMD for this operation.
*                   This value will be placed directly into the SPCMD register by the function. Caller is required to
*                   provide correctly formatted data.
*                pdest-
*                   Pointer to destination buffer into which data will be copied that is received from a SPI .
*                   It is the responsibility of the caller to insure that adequate space is available to hold the
*                   requested data count.
*                length-
*                   Indicates the number of data words to be transferred. The size of the data word is determined from
*                   the channel configuration data structure referenced by the channel handle.
* Return Value : RSPI_SUCCESS-
*                   Read operation successfully completed.
*                RSPI_ERR_CH_NOT_OPENED-
*                   The channel has not been opened.  Perform R_RSPI_Open() first
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
*                RSPI_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
rspi_err_t  R_RSPI_Read(rspi_handle_t        handle,
                        rspi_command_word_t  spcmd_command_word,
                        void                *pdest,
                        uint16_t             length)
{
    rspi_err_t  result;

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ((NULL == handle) || (NULL == pdest))
    {
          R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    if ((RSPI_TRANS_MODE_DMAC == g_rspi_tcb[handle->channel].data_tran_mode)
      ||(RSPI_TRANS_MODE_DTC == g_rspi_tcb[handle->channel].data_tran_mode))
    {

        /* ---- Check the buffer boundary (4-byte unit). ---- */
        /* Cast the variable to a uint32_t type because the address of this pointer may not be 4-byte unit. */
        if (0 != ((uint32_t)pdest & RSPI_DATA_ALIGN))
        {
            R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
            return RSPI_ERR_INVALID_ARG;
        }

        /* ---- Check the counter (4-byte unit). ---- */
        /* NOTE: Do not support the number of data other than a multiple of 4 using DMAC or DTC. */
        if (0 != (length & RSPI_DATA_ALIGN))
        {
            R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
            return RSPI_ERR_INVALID_ARG;
        }
    }

    if (RSPI_BYTE_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, NULL, pdest, length, RSPI_DO_RX);
    }
    else if (RSPI_WORD_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, NULL, pdest, (length >> 1), RSPI_DO_RX);
    }
    else if (RSPI_LONG_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, NULL, pdest, (length >> 2), RSPI_DO_RX);
    }
    else
    {
        return RSPI_ERR_INVALID_ARG;
    }

    return result;
}
/* end of function R_RSPI_Read(). */


/***********************************************************************************************************************
* Function Name: R_RSPI_Write
* Description  : Transmits data to a SPI  device.  The operation differs slightly depending on whether it is using
*                SPI mode or Clock-Synchronous mode.
* Arguments    : handle-
*                   Handle for the channel
*                spcmd_command_word-
*                   Bitfield data consisting of all the RSPI command register settings for SPCMD for this operation.
*                   This value will be placed directly into the SPCMD register by the function. Caller is required to
*                   provide correctly formatted data.
*                psrc-
*                   Pointer to a source data buffer from which data will be transmitted to a SPI device.
*                   The argument must not be NULL.
*                length-
*                   Indicates the number of data words to be transferred. The size of the data word is determined from
*                   the channel configuration data structure referenced by the channel handle.
* Return Value : RSPI_SUCCESS-
*                   Write operation successfully completed.
*                RSPI_ERR_CH_NOT_OPENED-
*                   The channel has not been opened.  Perform R_RSPI_Open() first
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
***********************************************************************************************************************/
rspi_err_t  R_RSPI_Write(rspi_handle_t        handle,
                         rspi_command_word_t  spcmd_command_word,
                         void                *psrc,
                         uint16_t             length)
{
    rspi_err_t  result;

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ((NULL == handle) || (NULL == psrc))
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    if ((RSPI_TRANS_MODE_DMAC == g_rspi_tcb[handle->channel].data_tran_mode)
      ||(RSPI_TRANS_MODE_DTC == g_rspi_tcb[handle->channel].data_tran_mode))
    {
        /* ---- Check the buffer boundary (4-byte unit). ---- */
        /* Cast the variable to a uint32_t type because the address of this pointer may not be 4-byte unit. */
        if (0 != ((uint32_t)psrc & RSPI_DATA_ALIGN))
        {
            R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
            return RSPI_ERR_INVALID_ARG;
        }

        /* ---- Check the counter (4-byte unit). ---- */
        /* NOTE: Do not support the number of data other than a multiple of 4 using DMAC or DTC. */
        if (0 != (length & RSPI_DATA_ALIGN))
        {
            R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
            return RSPI_ERR_INVALID_ARG;
        }
    }

    if (RSPI_BYTE_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, psrc, NULL, length, RSPI_DO_TX);
    }
    else if (RSPI_WORD_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, psrc, NULL, (length >> 1), RSPI_DO_TX);
    }
    else if (RSPI_LONG_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, psrc, NULL, (length >> 2), RSPI_DO_TX);
    }
    else
    {
        return RSPI_ERR_INVALID_ARG;
    }

    return result;
}
/* end of function R_RSPI_Write(). */


/***********************************************************************************************************************
* Function Name: R_RSPI_WriteRead
* Description  : Simultaneously transmits data to SPI device while receiving data from SPI device
*                (full duplex).
*                The operation differs slightly depending on whether it is using SPI mode or Clock-Synchronous mode.
* Arguments    : handle-
*                   Handle for the channel
*                spcmd_command_word-
*                   bitfield data consisting of all the RSPI command register settings for SPCMD for this operation.
*                   This value will be placed directly into the SPCMD0 register by the function. Caller is required to
*                   provide correctly formatted data.
*                psrc-
*                   Pointer to a source data buffer from which data will be transmitted to a SPI device.
*                   The argument must not be NULL.
*                pdest-
*                   Pointer to destination buffer into which data will be copied that has been received from SPI slave.
*                   Caller must insure that adequate space is available to hold the requested data count.
*                   Argument must not be NULL.
*                length-
*                   Indicates the number of data words to be transferred. The size of the data word is determined from
*                   the channel configuration data structure referenced by the channel handle.
* Return Value : RSPI_SUCCESS
*                RSPI_ERR_CH_NOT_OPENED-
*                   The channel has not been opened.  Perform R_RSPI_Open() first
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
*                RSPI_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
rspi_err_t  R_RSPI_WriteRead(rspi_handle_t        handle,
                             rspi_command_word_t  spcmd_command_word,
                             void                *psrc,
                             void                *pdest,
                             uint16_t             length)
{
    rspi_err_t  result;

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if (((NULL == handle) || (NULL == psrc)) || (NULL == pdest))
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    if ((RSPI_TRANS_MODE_DMAC == g_rspi_tcb[handle->channel].data_tran_mode)
      ||(RSPI_TRANS_MODE_DTC == g_rspi_tcb[handle->channel].data_tran_mode))
    {
        /* ---- Check the buffer boundary (4-byte unit). ---- */
        /* Cast the variable to a uint32_t type because the address of this pointer may not be 4-byte unit. */
        if ((0 != ((uint32_t)psrc & RSPI_DATA_ALIGN))||(0 != ((uint32_t)pdest & RSPI_DATA_ALIGN)))
        {
            R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
            return RSPI_ERR_INVALID_ARG;
        }

        /* ---- Check the counter (4-byte unit). ---- */
        /* NOTE: Do not support the number of data other than a multiple of 4 using DMAC or DTC. */
        if (0 != (length & RSPI_DATA_ALIGN))
        {
            R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
            return RSPI_ERR_INVALID_ARG;
        }
    }

    if (RSPI_BYTE_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, psrc, pdest, length, RSPI_DO_TX_RX);
    }
    else if (RSPI_WORD_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, psrc, pdest, (length >> 1), RSPI_DO_TX_RX);
    }
    else if (RSPI_LONG_DATA == rspi_get_data_type(spcmd_command_word))
    {
        result = rspi_write_read_common(handle, spcmd_command_word, psrc, pdest, (length >> 2), RSPI_DO_TX_RX);
    }
    else
    {
        return RSPI_ERR_INVALID_ARG;
    }

    return result;
}
/* end of function R_RSPI_WriteRead(). */

/***********************************************************************************************************************
* Function Name: rspi_write_read_common
* Description  : Initiates write or read process. Common routine used by RSPI API write or read functions.
* Arguments    : handle-
*                   Handle for the channel
*                command_word-
*                   bitfield data consisting of all the RSPI command register settings for SPCMD for this operation.
*                   This value will be placed directly into the SPCMD0 register by the function. Caller is required to
*                   provide correctly formatted data.
*                psrc-
*                   For write operations, pointer to the source buffer of the data to be sent.
*                pdest-
*                   For read operations, pointer to destination buffer into which receuved data will be copied.
*                length-
*                   The number of data words to be transferred.
* Return Value : RSPI_SUCCESS
*                RSPI_ERR_CH_NOT_OPENED-
*                   The channel has not been opened.  Perform R_RSPI_Open() first
*                RSPI_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
static rspi_err_t  rspi_write_read_common(rspi_handle_t handle,
                                          rspi_command_word_t command_word,
                                          void          *psrc,
                                          void          *pdest,
                                          uint16_t      length,
                                          rspi_operation_t   tx_rx_mode)
{
    uint8_t  channel = handle->channel;

    #if RSPI_CFG_REQUIRE_LOCK == 1
    bool     lock_result = false;
    #endif

    if (!g_rspi_handles[channel].rspi_chnl_opened)
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_CH_NOT_OPENED;
    }

    #if RSPI_CFG_REQUIRE_LOCK == 1
    /* Attempt to acquire lock for this RSPI channel. Prevents reentrancy conflict. */
    lock_result = R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));

    if (false == lock_result)
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_LOCK; // The control function is currently locked.
    }
    #endif

    rspi_interrupts_enable(channel, false);     // Disable interrupts in ICU.

    g_rspi_tcb[channel].xfr_length = length;
    g_rspi_tcb[channel].tx_count = 0;
    g_rspi_tcb[channel].rx_count = 0;
    g_rspi_tcb[channel].bytes_per_transfer = rspi_get_data_type(command_word);
    g_rspi_tcb[channel].psrc = psrc;
    g_rspi_tcb[channel].pdest = pdest;
    g_rspi_tcb[channel].transfer_mode = tx_rx_mode;

    if (tx_rx_mode & RSPI_DO_TX)
    {
        g_rspi_tcb[channel].do_tx = true;
    }
    else
    {
        g_rspi_tcb[channel].do_tx = false;
    }

    g_rspi_tcb[channel].do_rx_now = false;  // Initialize receive state flag.

    #if RSPI_CFG_MASK_UNUSED_BITS == (1)
    /* Get the data frame bit mask. */
    g_rspi_tcb[channel].unused_bits_mask = g_unused_bits_masks[command_word.bit_length];
    #endif

    /* WAIT_LOOP */
    /* Wait for channel to be idle before making changes to registers. */
    while ((*g_rspi_channels[channel]).SPSR.BIT.IDLNF)
    {
        /* Wait for channel to be idle before making changes to registers. */
    }

    /* Update the SPCMD0 command register with the settings for this transfer. */
#if RSPI_LITTLE_ENDIAN == 1
    (*g_rspi_channels[channel]).SPCMD0.WORD = command_word.word[0];
#else
    (*g_rspi_channels[channel]).SPCMD0.WORD = command_word.word[1];
#endif /* RSPI_LITTLE_ENDIAN == 1 */

    /* If slave mode, force CPHA bit in command register to 1 to properly support 'burst' operation. */
    if (0 == ((*g_rspi_channels[channel]).SPCR.BIT.MSTR))
    {
        (*g_rspi_channels[channel]).SPCMD0.BIT.CPHA = 1;
    }

    /* WAIT_LOOP */
    /* Clear error sources: the SPSR.MODF, OVRF, PERF and UDRF flags. */
    while((*g_rspi_channels[channel]).SPSR.BYTE & (((RSPI_SPSR_OVRF | RSPI_SPSR_MODF) | RSPI_SPSR_PERF) | RSPI_SPSR_UDRF))
    {
        (*g_rspi_channels[channel]).SPSR.BYTE = RSPI_SPSR_MASK;
    }

    (*g_rspi_channels[channel]).SPCR2.BIT.SPIIE = 0; // Disable idle interrrupt.
    if (0 == (*g_rspi_channels[channel]).SPCR2.BIT.SPIIE)
    {
        R_BSP_NOP();
    }

    rspi_interrupts_clear(channel);
    rspi_interrupts_enable(channel, true);           // Enable interrupts in ICU.

    /* Enable transmit buffer empty interrupt, Receive buffer full interrupt,
     * and enable RSPI simultaneously. This will generate an SPTI interrupt,
     * and data transfer will proceed in the ISRs. */
    (*g_rspi_channels[channel]).SPCR.BYTE |= (((RSPI_SPCR_SPTIE | RSPI_SPCR_SPRIE) | RSPI_SPCR_SPEIE) | RSPI_SPCR_SPE);
    if (0 == (*g_rspi_channels[channel]).SPCR.BYTE)
    {
        R_BSP_NOP();
    }

    return RSPI_SUCCESS;
}
/* end of function rspi_write_read_common(). */

/*******************************************************************************
* Function Name: R_RSPI_IntSptiIerClear
* Description  : Clears the ICU.IERm.IENj bit of SPTI to 0.
* Arguments    : handle -
*                    handle for the channel
* Return Value : RSPI_SUCCESS
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
*******************************************************************************/
rspi_err_t R_RSPI_IntSptiIerClear(rspi_handle_t handle)
{
    uint8_t channel = handle->channel;

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ( NULL == handle )
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    switch (channel)
    {
        case 0:
#if RSPI_CFG_USE_CHAN0 == 1
            R_BSP_InterruptRequestDisable(VECT(RSPI0,SPTI0));                // Disable RSPI0.SPTI0 interrupt request.
            IR(RSPI0,SPTI0)     = 0;                // Clear RSPI0.SPTI0 interrupt status flag.
            if (0 == IR(RSPI0, SPTI0))
            {
                /* Wait for the write completion. */
                R_BSP_NOP();
            }
#endif  /* #if RSPI_CFG_USE_CHAN0 */
        break;

        case 1:
#if RSPI_CFG_USE_CHAN1 == 1
        	R_BSP_InterruptRequestDisable(VECT(RSPI1,SPTI1));                // Disable RSPI1.SPTI1 interrupt request.
            IR(RSPI1,SPTI1)     = 0;                // Clear RSPI1.SPTI1 interrupt status flag.
            if (0 == IR(RSPI1, SPTI1))
            {
                /* Wait for the write completion. */
                R_BSP_NOP();
            }
#endif  /* #if RSPI_CFG_USE_CHAN1 */
        break;

        case 2:
#if RSPI_CFG_USE_CHAN2 == 1
        	R_BSP_InterruptRequestDisable(VECT(RSPI2,SPTI2));                // Disable RSPI2.SPTI2 interrupt request.
            IR(RSPI2,SPTI2)     = 0;                // Clear RSPI2.SPTI2 interrupt status flag.
            if (0 == IR(RSPI2, SPTI2))
            {
                /* Wait for the write completion. */
                R_BSP_NOP();
            }
#endif  /* #if RSPI_CFG_USE_CHAN2 */
        break;
        default:
            /* Should never get here. Valid channel number is checked above. */
        break;
    }

    return RSPI_SUCCESS;
}


/*******************************************************************************
* Function Name: R_RSPI_IntSpriIerClear
* Description  : Clears the ICU.IERm.IENj bit of SPRI to 0.
* Arguments    : handle -
*                    handle for the channel
* Return Value : RSPI_SUCCESS
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
*******************************************************************************/
rspi_err_t R_RSPI_IntSpriIerClear(rspi_handle_t handle)
{
    uint8_t channel = handle->channel;

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ( NULL == handle )
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    switch (channel)
    {
        case 0:
#if RSPI_CFG_USE_CHAN0 == 1
        	R_BSP_InterruptRequestDisable(VECT(RSPI0,SPRI0));                // Disable RSPI0.SPRI0 interrupt request.
            IR(RSPI0,SPRI0)     = 0;                // Clear RSPI0.SPRI0 interrupt status flag.
            if (0 == IR(RSPI0, SPRI0))
            {
                /* Wait for the write completion. */
                R_BSP_NOP();
            }
#endif  /* #if RSPI_CFG_USE_CHAN0 */
        break;

        case 1:
#if RSPI_CFG_USE_CHAN1 == 1
        	R_BSP_InterruptRequestDisable(VECT(RSPI1,SPRI1));                // Disable RSPI1.SPRI1 interrupt request.
            IR(RSPI1,SPRI1)     = 0;                // Clear RSPI1.SPRI1 interrupt status flag.
            if (0 == IR(RSPI1, SPRI1))
            {
                /* Wait for the write completion. */
                R_BSP_NOP();
            }
#endif  /* #if RSPI_CFG_USE_CHAN1 */
        break;

        case 2:
#if RSPI_CFG_USE_CHAN2 == 1
        	R_BSP_InterruptRequestDisable(VECT(RSPI2,SPRI2));                // Disable RSPI2.SPRI2 interrupt request.
            IR(RSPI2,SPRI2)     = 0;                // Clear RSPI2.SPRI2 interrupt status flag.
            if (0 == IR(RSPI2, SPRI2))
            {
                /* Wait for the write completion. */
                R_BSP_NOP();
            }
#endif  /* #if RSPI_CFG_USE_CHAN2 */
        break;
        default:
            /* Should never get here. Valid channel number is checked above. */
        break;
    }

    return RSPI_SUCCESS;
}


/*******************************************************************************
* Function Name: R_RSPI_IntSptiDmacdtcFlagSet
* Description  : Sets a DMAC or DTC transfer end flag for SPTI.
* Arguments    : handle -
*                    handle for the channel
* Return Value : RSPI_SUCCESS -
*                    Successful operation
*                RSPI_ERR_INVALID_ARG -
*                    Parameter error
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
*******************************************************************************/
rspi_err_t R_RSPI_IntSptiDmacdtcFlagSet(rspi_handle_t handle, rspi_trans_flg_t flg)
{

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ( NULL == handle )
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif


    if ((RSPI_SET_TRANS_STOP != flg) && (RSPI_SET_TRANS_START != flg))
    {
        return RSPI_ERR_INVALID_ARG;
    }

    handle->spti_dmacdtc_flg = flg;

    return RSPI_SUCCESS;
}


/*******************************************************************************
* Function Name: R_RSPI_IntSpriDmacdtcFlagSet
* Description  : Sets DMAC/DTC transfer end flag for SPRI.
* Arguments    : handle -
*                    handle for the channel
* Return Value : RSPI_SUCCESS -
*                    Successful operation
*                RSPI_ERR_INVALID_ARG -
*                    Parameter error
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
*******************************************************************************/
rspi_err_t R_RSPI_IntSpriDmacdtcFlagSet(rspi_handle_t handle, rspi_trans_flg_t flg)
{

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ( NULL == handle )
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    if ((RSPI_SET_TRANS_STOP != flg) && (RSPI_SET_TRANS_START != flg))
    {
        return RSPI_ERR_INVALID_ARG;
    }

    handle->spri_dmacdtc_flg = flg;

    return RSPI_SUCCESS;
}


/*******************************************************************************
* Function Name: r_rspi_get_buffregaddress
* Description  : Gets address of RSPI data register (SPDR).
* Arguments    : channel -
*                    Which channel to use
*                p_spdr_adr -
*                    Address of RSPI data register (SPDR)
* Return Value : none
*******************************************************************************/
static void r_rspi_get_buffregaddress(uint8_t channel, uint32_t *p_spdr_adr)
{
    *p_spdr_adr = (uint32_t)(&(*g_rspi_channels[channel]).SPDR.LONG);
}

/*******************************************************************************
* Function Name: r_rspi_ch_check
* Description  : Checks whether channel is valid or not.
* Arguments    : channel -
*                    Which channel to use
* Return Value : RSPI_SUCCESS -
*                    The channel is valid.
*                RSPI_ERR_INVALID_ARG -
*                    Parameter error
*******************************************************************************/
static rspi_err_t r_rspi_ch_check(uint8_t channel)
{
    rspi_err_t ret = RSPI_SUCCESS;

    switch (channel)
    {
        case 0:
#if RSPI_CFG_USE_CHAN0 == 1
            /* Do nothing */
#else
            /* RSPI channel 0 is invalid. */
            ret = RSPI_ERR_INVALID_ARG;
#endif
        break;

        case 1:
#if RSPI_CFG_USE_CHAN1 == 1
            /* Do nothing */
#else
            /* RSPI channel 1 is invalid. */
            ret = RSPI_ERR_INVALID_ARG;
#endif
        break;

        case 2:
#if RSPI_CFG_USE_CHAN2 == 1
            /* Do nothing */
#else
            /* RSPI channel 2 is invalid. */
            ret = RSPI_ERR_INVALID_ARG;
#endif
        break;

        default:
            /* The channel number is invalid. */
            ret = RSPI_ERR_INVALID_ARG;
        break;
    }

    return ret;
}


/*******************************************************************************
* Function Name: R_RSPI_GetBuffRegAddress
* Description  : Gets address of RSPI data register (SPDR).
*              : Use the address of RSPI data register to set the transfer source
*              : register or transfer destination register in DMAC or DTC.
* Arguments    : handle -
*                    handle for the channel
*                p_spdr_adr -
*                    Address of RSPI data register (SPDR)
* Return Value : RSPI_SUCCESS -
*                    Successful operation
*                RSPI_ERR_INVALID_ARG -
*                    Parameter error
*                RSPI_ERR_NULL_PTR-
*                   A required pointer argument is NULL
*******************************************************************************/
rspi_err_t R_RSPI_GetBuffRegAddress(rspi_handle_t handle, uint32_t *p_spdr_adr)
{
    rspi_err_t ret = RSPI_SUCCESS;
    uint8_t channel = handle->channel;

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if ( NULL == handle )
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    /* ---- Check argument. ---- */
    ret = r_rspi_ch_check(channel);
    if (RSPI_SUCCESS != ret)
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_INVALID_ARG;
    }

    if (NULL == p_spdr_adr)
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_INVALID_ARG;
    }

    r_rspi_get_buffregaddress(channel, p_spdr_adr);

    return RSPI_SUCCESS;
}


/***********************************************************************************************************************
* Function Name: R_RSPI_Close
* Description  : Removes power to the RSPI channel designated by the handle and disables the associated interrupts.
* Arguments    : handle-
*                    Handle for the channel
* Return Value : RSPI_SUCCESS-
*                    Successful; channel closed
*                RSPI_ERR_CH_NOT_OPENED-
*                    The channel has not been opened so closing has no effect.
*                RSPI_ERR_NULL_PTR-
*                    A required pointer argument is NULL
***********************************************************************************************************************/
rspi_err_t  R_RSPI_Close(rspi_handle_t handle)
{
    uint8_t channel;

    #if RSPI_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == handle)
    {
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_NULL_PTR;
    }
    #endif

    channel = handle->channel;

    /* Check to see if the channel is currently initialized. */
    if (false == g_rspi_handles[channel].rspi_chnl_opened)
    {
        /* This channel is not open so need not be closed. */
        R_RSPI_LOG_FUNC(RSPI_DEBUG_ERR_ID, (uint32_t)RSPI_STR, __LINE__);
        return RSPI_ERR_CH_NOT_OPENED;
    }

    /* Disable the RSPI operation. */
    /* SPE and SPTIE should be cleared simultaneously. */
    (*g_rspi_channels[channel]).SPCR.BYTE &= (uint8_t)(~(RSPI_SPCR_SPTIE | RSPI_SPCR_SPE));
    if (0 == (*g_rspi_channels[channel]).SPCR.BYTE)
    {
        R_BSP_NOP();
    }

    power_on_off(channel, RSPI_POWER_OFF);

    rspi_interrupts_enable(channel, false); // Disable interrupts.

    g_rspi_handles[channel].rspi_chnl_opened = false;

    return RSPI_SUCCESS;
}
/* end of function R_RSPI_Close(). */


/***********************************************************************************************************************
* Function Name: rspi_baud_set
* Description  : Determines the RSPI channel SPBR register setting for the requested baud rate.
*                Returns the actual bit rate that the setting will achieve which may differ from requested.
*                If the requested bit rate cannot be exactly achieved, the next lower bit rate setting will be applied.
*                If successful, applies the calculated setting to the SPBR register.
* Arguments    :
* Return Value :
* Note: Target baud must be >= PCLK/4 to get anything out.
* Limitations   : Does not track dynamically changing PCLK. Relies on constant BSP_PCLKB_HZ
***********************************************************************************************************************/
static uint32_t rspi_baud_set(uint8_t channel, uint32_t bps_target)
{
    uint8_t     spbr_result = 0;
    uint32_t    bps_calc = 0;
    int32_t     f;  // Frequency
    int32_t     n;  // n term in equation
    int32_t     N;  // N term in equation

    /* Starting with RX63x MCUs and later, there are 2 peripheral clocks: PCLKA and PCLKB.
     * PCLKB matches the functionality of PCLK in RX62x devices as far as the RSPI is concerned. */
    #if defined(BSP_MCU_RX62_ALL)
        f = BSP_PCLK_HZ;
    #elif defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) ||defined (BSP_MCU_RX65N) ||defined (BSP_MCU_RX66T) || defined (BSP_MCU_RX72T)
        f = BSP_PCLKA_HZ;
    #else
        f = BSP_PCLKB_HZ;
    #endif

    /* Get the register settings for requested baud rate. */
    if ((f / bps_target) < 2)
    {
        return 0;   // baud_bps_target too high for the PCLK.
    }
    /*
     * From Hardware manual: Bit rate = f / (2(n + 1)(2^N))
     * where:
     *      f = PCLK, n = SPBR setting, N = BRDV bits
     * Solving for n:
     *      n = (((f/(2^N))/2) / bps) - 1
     *
     */

    /* Only calculate for BRDV value of 0 (div/1) to get SPBR setting for the board PCLK.
     * BRDV setting will be done during write/read operations. */
    N = 0;
    n = ((f >> (N+1)) / (int32_t)bps_target) - 1;  // Solve for SPBR setting.

    if ((n >= 0) && (n <= 0xff))   /* Must be <= SPBR register max value. Must not be negative*/
    {
        /* Now plug n back into the formula for BPS and check it. */
        bps_calc = (uint32_t)(f / (2 *((n + 1) << N)));

        if (bps_calc > bps_target)
        {
            n += 1;
            if (n > 0xff)
            {
                return 0; // result out of range for the PCLK.
            }
        }
        spbr_result = n;

        (*g_rspi_channels[channel]).SPBR = spbr_result;    // Apply the SPBR register value.
        if (0 == (*g_rspi_channels[channel]).SPBR)
        {
            R_BSP_NOP();
        }
        g_ctrl_reg_values[channel].spbr_val = spbr_result; // Update the channel settings record.
    }
    else
    {
        bps_calc = 0;  // result out of range for the PCLK.
    }

    return bps_calc;    // Return the actual BPS rate achieved.
}
/* end of function rspi_baud_set(). */

/***********************************************************************************************************************
* Function Name: rspi_get_data_type
* Description  : Identifies whether the data must be type-cast as 8-bit, 16-bit, or 32-bit for purposes of accessing the
*                source or destination buffers with the right type and index.
* Arguments    : frame_length_bits-
*                   16-bit word containing the bits that define the bits per frame in th SPCMD register.
*                   Only the bits corresponding to "SPB[3:0] RSPI Data Length Setting" of the SPCMDn register are
*                   checked in this argument.
* Return Value : RSPI_BYTE_DATA-
*                   Data is 8-bit.
*                RSPI_WORD_DATA-
*                   Data is > 8-bit and <= 16-bit.
*                RSPI_LONG_DATA-
*                   Data is > 16-bit.
***********************************************************************************************************************/
static uint8_t rspi_get_data_type(rspi_command_word_t command_word)
{
    uint8_t data_type;
    uint8_t frame_length_bits;

#if RSPI_LITTLE_ENDIAN == 1
    frame_length_bits = (uint8_t)((command_word.word[0] & RSPI_SPCMD_SPB) >> 8);
#else
    frame_length_bits = (uint8_t)((command_word.word[1] & RSPI_SPCMD_SPB) >> 8);
#endif /* RSPI_LITTLE_ENDIAN == 1 */

    switch (frame_length_bits)
    {
        case RSPI_SPCMD_BIT_LENGTH_8: // (0x07) 0100 to 0111 = 8 bits data length
        {
            data_type = RSPI_BYTE_DATA;
        }
        break;

        case RSPI_SPCMD_BIT_LENGTH_9:   // (0x08)  1000 = 9 bits data length
        case RSPI_SPCMD_BIT_LENGTH_10:  // (0x09)  1001 = 10 bits data length
        case RSPI_SPCMD_BIT_LENGTH_11:  // (0x0A)  1010 = 11 bits data length
        case RSPI_SPCMD_BIT_LENGTH_12:  // (0x0B)  1011 = 12 bits data length
        case RSPI_SPCMD_BIT_LENGTH_13:  // (0x0C)  1100 = 13 bits data length
        case RSPI_SPCMD_BIT_LENGTH_14:  // (0x0D)  1101 = 14 bits data length
        case RSPI_SPCMD_BIT_LENGTH_15:  // (0x0E)  1110 = 15 bits data length
        case RSPI_SPCMD_BIT_LENGTH_16:  // (0x0F)  1111 = 16 bits data length
        {
            data_type = RSPI_WORD_DATA;
        }
        break;

        case RSPI_SPCMD_BIT_LENGTH_20:  // (0x00)  0000 = 20 bits data length
        case RSPI_SPCMD_BIT_LENGTH_24:  // (0x01)  0001 = 24 bits data length
        case RSPI_SPCMD_BIT_LENGTH_32:  // (0x03)  0011 = 32 bits data length
        case 0x0002:            // Alternate setting for 32 bit.
        {
            data_type = RSPI_LONG_DATA;
        }
        break;

        default:
        {
            data_type = 0;
        }
    }
    return data_type;
}
/* End of function rspi_get_data_access(). */


/***********************************************************************************************************************
* Function Name: power_on_off
* Description : Switches power to an RSPI channel.  Required by FIT spec.
* Arguments : channel -
*                   Which channel to use.
*             on_or_off -
*                   What it says.
* Return Value : none
***********************************************************************************************************************/
static void power_on_off (uint8_t channel, uint8_t on_or_off)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

    switch (channel)
    {
        #if RSPI_CFG_USE_CHAN0 == 1
        case 0:
            MSTP(RSPI0) = on_or_off;
            if (0 == MSTP(RSPI0))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        #if RSPI_CFG_USE_CHAN1 == 1
        case 1:
            MSTP(RSPI1) = on_or_off;
            if (0 == MSTP(RSPI1))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        #if RSPI_CFG_USE_CHAN2 == 1
        case 2:
            MSTP(RSPI2) = on_or_off;
            if (0 == MSTP(RSPI2))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        default:
        break;
    }

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
}
/* End of function power_on(). */


/***********************************************************************************************************************
* Function Name: rspi_ir_priority_set
* Description  : sets the shared interrupt priority level for a channel.
* Arguments : channel -
*                 Which channel to use.
*             rspi_priority-
*                 0-15 priority value. 15 = highest priority.
* Return Value : none
***********************************************************************************************************************/
static void rspi_ir_priority_set(uint8_t channel, uint8_t rspi_priority)
{
    #if defined BSP_MCU_RX64M || defined BSP_MCU_RX71M || defined BSP_MCU_RX65N || defined BSP_MCU_RX66T || defined BSP_MCU_RX72T
    bsp_int_ctrl_t int_ctrl_data;
    int_ctrl_data.ipl = (uint32_t)rspi_priority;
    #endif

    switch (channel)
    {
        #if RSPI_CFG_USE_CHAN0 == 1
        case 0:
            IPR(RSPI0, SPRI0) = rspi_priority; // One priority register for all RSPI interrupts (exc RX64M).
             /* RX64M has individual RSPI interrupt priority registers. Set the rest of them */
            #if defined BSP_MCU_RX64M || defined BSP_MCU_RX71M || defined BSP_MCU_RX65N || defined BSP_MCU_RX66T || defined BSP_MCU_RX72T
            IPR(RSPI0, SPTI0) = rspi_priority;
            /* Enable the group interrupt for the SPEI0 interrupt. (sets priority also). */
            R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI0_SPEI0, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, &int_ctrl_data);
            #endif

            if (0 == IPR(RSPI0, SPRI0))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        #if RSPI_CFG_USE_CHAN1 == 1
        case 1:
            IPR(RSPI1, SPRI1) = rspi_priority;
            /* RX71M has individual RSPI interrupt priority registers. Set the rest of them */
            #if defined BSP_MCU_RX71M || defined BSP_MCU_RX65N
            IPR(RSPI1, SPTI1) = rspi_priority;
            /* Enable the group interrupt for the SPEI1 interrupt. (sets priority also). */
            R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI1_SPEI1, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, &int_ctrl_data);
            #endif

            if (0 == IPR(RSPI1, SPRI1))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        #if RSPI_CFG_USE_CHAN2 == 1
        case 2:
            IPR(RSPI2, SPRI2) = rspi_priority;
            #if defined BSP_MCU_RX65N
            IPR(RSPI2, SPTI2) = rspi_priority;
            /* Enable the group interrupt for the SPEI2 interrupt. (sets priority also). */
            R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI2_SPEI2, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, &int_ctrl_data);
            #endif

            if (0 == IPR(RSPI2, SPRI2))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        default:
        break;
    }
}
/* End of function rspi_ir_priority_set(). */

/***********************************************************************************************************************
* Function Name: rspi_interrupts_clear
* Description  : Clear RSPI interrupts.
* Arguments : channel -
*                 Which channel to use.
* Return Value : none
***********************************************************************************************************************/
static void rspi_interrupts_clear(uint8_t channel)
{
    switch (channel)
    {
        #if RSPI_CFG_USE_CHAN0 == 1
        case 0:
            /* Clear any pending receive buffer full interrupts */
            IR(RSPI0, SPRI0) = 0 ;
            /* Clear any pending transmit buffer empty interrupts */
            IR(RSPI0, SPTI0) = 0 ;
            #ifndef BSP_MCU_RX63_ALL
            #ifndef BSP_MCU_RX64M
            #ifndef BSP_MCU_RX71M
            #ifndef BSP_MCU_RX65N
            #ifndef BSP_MCU_RX66T
            #ifndef BSP_MCU_RX72T
            /* Clear any pending error interrupt */
            IR(RSPI0, SPEI0) = 0;
            #endif
            #endif
            #endif
            #endif
            #endif
            #endif

            if (0 == IR(RSPI0, SPTI0))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        #if RSPI_CFG_USE_CHAN1 == 1
        case 1:
            /* Clear any pending receive buffer full interrupts */
            IR(RSPI1, SPRI1) = 0 ;
            /* Clear any pending transmit buffer empty interrupts */
            IR(RSPI1, SPTI1) = 0 ;
            #ifndef BSP_MCU_RX63_ALL
            #ifndef BSP_MCU_RX64M
            #ifndef BSP_MCU_RX71M
            #ifndef BSP_MCU_RX65N
            /* Clear any pending error interrupt */
            IR(RSPI1, SPEI1) = 0;
            #endif
            #endif
            #endif
            #endif

            if (0 == IR(RSPI1, SPTI1))
            {
                R_BSP_NOP();
            }
        break;

        #endif

        #if RSPI_CFG_USE_CHAN2 == 1
        case 2:
            IR(RSPI2, SPRI2) = 0 ;
            IR(RSPI2, SPTI2) = 0 ;
            #ifndef BSP_MCU_RX63_ALL
            #ifndef BSP_MCU_RX65N
            IR(RSPI2, SPEI2) = 0;
            #endif
            #endif

            if (0 == IR(RSPI2, SPTI2))
            {
                R_BSP_NOP();
            }
        break;
        #endif

        default:
        break;
    }
    #ifdef BSP_MCU_RX63_ALL
        #if RSPI_CFG_USE_RX63_ERROR_INTERRUPT == 1
        IR(ICU, GROUP12) = 0;
        if (0 == IR(ICU, GROUP12))
        {
            R_BSP_NOP();
        }
        #endif
    #endif
}
/* End of function rspi_interrupts_clear(). */

/***********************************************************************************************************************
* Function Name: rspi_interrupts_enable
* Description  : Disable or enable RSPI interrupts.
* Arguments : channel -
*                 Which channel to use.
*             enabled-
*                 true = enable, false = disable.
* Return Value : none
***********************************************************************************************************************/
static void rspi_interrupts_enable(uint8_t channel, bool enabled)
{
    switch (channel)
    {
        #if RSPI_CFG_USE_CHAN0 == 1
        case 0:
            /* Disable or enable receive buffer full interrupt */
            R_BSP_InterruptRequestEnable(VECT(RSPI0,SPRI0));
            /* Disable  or enable transmit buffer empty interrupt */
            R_BSP_InterruptRequestEnable(VECT(RSPI0,SPTI0));
            #ifndef BSP_MCU_RX63_ALL
            #ifndef BSP_MCU_RX64M
            #ifndef BSP_MCU_RX71M
            #ifndef BSP_MCU_RX65N
            #ifndef BSP_MCU_RX66T
            #ifndef BSP_MCU_RX72T
            /* Disable or enable error interrupt */
            R_BSP_InterruptRequestEnable(VECT(RSPI0,SPEI0));
            #endif
            #endif
            #endif
            #endif
            #endif
            #endif

            if (0 == IEN(RSPI0, SPTI0))
            {
                R_BSP_NOP();
            }
            
            #if defined BSP_MCU_RX64M || defined BSP_MCU_RX71M || defined BSP_MCU_RX65N || defined BSP_MCU_RX66T || defined BSP_MCU_RX72T
            if (enabled)
            {
                /* Register the error callback function with the BSP group interrupt handler. */
                R_BSP_InterruptWrite(BSP_INT_SRC_AL0_RSPI0_SPEI0, (bsp_int_cb_t)rspi_spei_grp_isr);
                /* Enable error interrupt source bit */
                ICU.GENAL0.BIT.EN17 = 1;
                if (0 == ICU.GENAL0.BIT.EN17)
                {
                    R_BSP_NOP();
                }
            }
            else
            {
                /* De-register the error callback function with the BSP group interrupt handler. */
                R_BSP_InterruptWrite(BSP_INT_SRC_AL0_RSPI0_SPEI0, FIT_NO_FUNC);
                /* Disable error interrupt source bit */
                ICU.GENAL0.BIT.EN17 = 0;
                if (0 == ICU.GENAL0.BIT.EN17)
                {
                    R_BSP_NOP();
                }
            }
            #endif
        break;
        #endif

        #if RSPI_CFG_USE_CHAN1 == 1
        case 1:
        	R_BSP_InterruptRequestEnable(VECT(RSPI1,SPRI1));
        	R_BSP_InterruptRequestEnable(VECT(RSPI1,SPTI1));
            #ifndef BSP_MCU_RX63_ALL
            #ifndef BSP_MCU_RX64M
            #ifndef BSP_MCU_RX71M
            #ifndef BSP_MCU_RX65N
            /* Disable or enable error interrupt */
            R_BSP_InterruptRequestEnable(VECT(RSPI1,SPEI1));
            #endif
            #endif
            #endif
            #endif

            if (0 == IEN(RSPI1, SPTI1))
            {
                R_BSP_NOP();
            }

            #if defined BSP_MCU_RX64M || defined BSP_MCU_RX71M || defined BSP_MCU_RX65N
            if (enabled)
            {
                /* Register the error callback function with the BSP group interrupt handler. */
                R_BSP_InterruptWrite(BSP_INT_SRC_AL0_RSPI1_SPEI1, (bsp_int_cb_t)rspi_spei_grp_isr);
                /* Enable error interrupt source bit */
                ICU.GENAL0.BIT.EN19 = 1;
                if (0 == ICU.GENAL0.BIT.EN19)
                {
                    R_BSP_NOP();
                }
            }
            else
            {
                /* De-register the error callback function with the BSP group interrupt handler. */
                R_BSP_InterruptWrite(BSP_INT_SRC_AL0_RSPI1_SPEI1, FIT_NO_FUNC);
                /* Disable error interrupt source bit */
                ICU.GENAL0.BIT.EN19 = 0;
                if (0 == ICU.GENAL0.BIT.EN19)
                {
                    R_BSP_NOP();
                }
            }
            #endif
        break;
        #endif

        #if RSPI_CFG_USE_CHAN2 == 1
        case 2:
        	R_BSP_InterruptRequestEnable(VECT(RSPI2,SPRI2));
        	R_BSP_InterruptRequestEnable(VECT(RSPI2,SPTI2));
            #ifndef BSP_MCU_RX63_ALL
            #ifndef BSP_MCU_RX65N
        	R_BSP_InterruptRequestEnable(VECT(RSPI2,SPEI2));
            #endif
            #endif

            if (0 == IEN(RSPI2, SPTI2))
            {
                R_BSP_NOP();
            }

            #if defined BSP_MCU_RX65N
            if (enabled)
            {
                /* Register the error callback function with the BSP group interrupt handler. */
                R_BSP_InterruptWrite(BSP_INT_SRC_AL0_RSPI2_SPEI2, (bsp_int_cb_t)rspi_spei_grp_isr);
                /* Enable error interrupt source bit */
                ICU.GENAL0.BIT.EN21 = 1;
                if (0 == ICU.GENAL0.BIT.EN21)
                {
                    R_BSP_NOP();
                }
            }
            else
            {
                /* De-register the error callback function with the BSP group interrupt handler. */
                R_BSP_InterruptWrite(BSP_INT_SRC_AL0_RSPI2_SPEI2, FIT_NO_FUNC);
                /* Disable error interrupt source bit */
                ICU.GENAL0.BIT.EN21 = 0;
                if (0 == ICU.GENAL0.BIT.EN21)
                {
                    R_BSP_NOP();
                }
            }
            #endif
        break;
        #endif

        default:
        break;
    }

    #ifdef BSP_MCU_RX63_ALL
        #if RSPI_CFG_USE_RX63_ERROR_INTERRUPT == 1
        R_BSP_InterruptRequestEnable(VECT(ICU,GROUP12));
        if (0 == IEN(ICU, GROUP12))
        {
            R_BSP_NOP();
        }
        #endif
    #endif

}
/* End of function rspi_interrupts_enable(). */


/***********************************************************************************************************************
* Function Name: R_RSPI_GetVersion
* Description : Returns the version of this module. The version number is
* encoded where the top 2 bytes are the major version number and
* the bottom 2 bytes are the minor version number.
* For example, Rev 4.25 would be 0x00040019.
* NOTE: This function is inlined using #pragma inline directive.
* Arguments : none
* Return Value : Version Number
***********************************************************************************************************************/
uint32_t R_RSPI_GetVersion(void)
{
    uint32_t version_number = 0;
    /* Bring in major version number. */
    version_number = ((uint16_t)RSPI_RX_VERSION_MAJOR) << 16;
    /* Bring in minor version number. */
    version_number |= (uint16_t)RSPI_RX_VERSION_MINOR;
    return version_number;
}

#if RSPI_CFG_LONGQ_ENABLE == 1
/*******************************************************************************
* Function Name: R_RSPI_Set_LogHdlAddress
* Description  : Sets handler address.
* Arguments    : user_long_que -
*                    Handler address
* Return Value : RSPI_SUCCESS -
*                    Successful operation
*******************************************************************************/
rspi_err_t R_RSPI_SetLogHdlAddress(uint32_t user_long_que)
{
    p_rspi_long_que = (longq_hdl_t)user_long_que;
    return RSPI_SUCCESS;
}

/*******************************************************************************
* Function Name: r_rspi_log
* Description  : Stores error information to LONGQ buffer.
* Arguments    : flg -
*                    Breakpoint processing
*                fid -
*                    RSPI driver file No.
*                line -
*                    RSPI driver line number
* Return Value : 0 -
*                    Successful operation
*                1 -
*                    Error
*******************************************************************************/
uint32_t r_rspi_log(uint32_t flg, uint32_t fid, uint32_t line)
{
    longq_err_t err;
    uint32_t    ul_tmp;

    /* Long to Byte */
    ul_tmp = 0;                                         // log     Reserved
    ul_tmp = (ul_tmp | (line << 8));                    // log     LINE
    ul_tmp = (ul_tmp | (fid  << 21));                   // log     FID FILE No.
    ul_tmp = (ul_tmp | (RSPI_DRIVER_ID << 27));         // log     FID DRIVER No.
    ul_tmp = (ul_tmp | (flg  << 31));                   // log     Breakpoint processing

    err = R_LONGQ_Put(p_rspi_long_que, ul_tmp);
    if (err != LONGQ_SUCCESS)
    {
        return 1;
    }

    /* Breakpoint processing */
    if (0x80000000 == (ul_tmp & 0x80000000))
    {
        return 1;
    }
    return 0;
}
#endif

#if RSPI_CFG_LONGQ_ENABLE == 0
/*******************************************************************************
* Function Name: R_RSPI_SetLogHdlAddress
* Description  : Sets handler address.
* Arguments    : user_long_que -
*                    Handler address
* Return Value : RSPI_SUCCESS -
*                    Successful operation
*******************************************************************************/
rspi_err_t R_RSPI_SetLogHdlAddress(uint32_t user_long_que)
{
    return RSPI_SUCCESS;
}
#endif  /* RSPI_CFG_LONGQ_ENABLE */

/******************************************************************************
* Function Name:    rspi_tx_common
* Description  :    common ISR handler for SPTI
* Arguments    :    RSPI channel
* Return Value :    N/A
******************************************************************************/
static void rspi_tx_common(uint8_t channel)
{
    void*       psrc      = g_rspi_tcb[channel].psrc;
    uint16_t    tx_count  = g_rspi_tcb[channel].tx_count;
    uint8_t     data_size = g_rspi_tcb[channel].bytes_per_transfer;

    /* Service the hardware first to keep it busy. */
    /* Feed the TX. */
    if (tx_count < g_rspi_tcb[channel].xfr_length)   // Don't write transmit buffer more than length.
    {
        if (g_rspi_tcb[channel].do_tx)
        {
            /* Transmit the data. TX data register accessed in long words. */
            if (RSPI_BYTE_DATA == data_size)
            {
                (*g_rspi_channels[channel]).SPDR.LONG = ((uint8_t *)psrc)[tx_count];
            }
            else if (RSPI_WORD_DATA == data_size)
            {
                (*g_rspi_channels[channel]).SPDR.LONG = ((uint16_t *)psrc)[tx_count];
            }
            else /* Must be long data. if (RSPI_LONG_DATA == data_size) */
            {
                (*g_rspi_channels[channel]).SPDR.LONG = ((uint32_t *)psrc)[tx_count];
            }
        }
        else /* Must be RX only mode, so transmit dummy data for clocking.*/
        {
            /* TX data register accessed in long words. */
            (*g_rspi_channels[channel]).SPDR.LONG = RSPI_CFG_DUMMY_TXDATA;
        }
        g_rspi_tcb[channel].tx_count++;
    }
    
    else
    {
        /* Last data was transferred. */
        (*g_rspi_channels[channel]).SPCR.BIT.SPTIE = 0;  // Disable SPTI interrupt.
    }
 }

/******************************************************************************
* Function Name:    rspi_rx_common
* Description  :    common ISR handler for  SPRI
* Arguments    :    RSPI channel
* Return Value :    N/A
******************************************************************************/
static void rspi_rx_common(uint8_t channel)
{

    void*       pdest     = g_rspi_tcb[channel].pdest;
    uint16_t    rx_count  = g_rspi_tcb[channel].rx_count;
    uint8_t     data_size = g_rspi_tcb[channel].bytes_per_transfer;
    uint32_t    rx_data   = g_rxdata[channel];

    if (g_rspi_tcb[channel].do_rx_now)
    {
        if (RSPI_BYTE_DATA == data_size)
        {
            ((uint8_t *)pdest)[rx_count-1] = (uint8_t)rx_data;
        }
        else if (RSPI_WORD_DATA == data_size)
        {
            #if RSPI_CFG_MASK_UNUSED_BITS == (1)
            /* Clear unused upper bits of non-standard bit length data transfers. */
            rx_data = (rx_data & g_rspi_tcb[channel].unused_bits_mask);
            #endif
            ((uint16_t *)pdest)[rx_count-1] = (uint16_t)rx_data;
        }
        else  /* Must be long data. if (RSPI_LONG_DATA == data_size) */
        {
            #if RSPI_CFG_MASK_UNUSED_BITS == (1)
            /* Clear unused upper bits of non-standard bit length data transfers. */
            rx_data &= g_rspi_tcb[channel].unused_bits_mask;
            #endif
            ((uint32_t *)pdest)[rx_count-1] = rx_data;
        }
    }

    /* Check for last data.  */
     if (rx_count == g_rspi_tcb[channel].xfr_length)
     {   /* Last data was transferred. */
#if RSPI_CFG_HIGH_SPEED_READ == 1
         (*g_rspi_channels[channel]).SPCR.BIT.SPTIE = 0;  // Disable SPTI interrupt.
#endif
         (*g_rspi_channels[channel]).SPCR.BIT.SPRIE = 0;  // Disable SPRI interrupt.
         (*g_rspi_channels[channel]).SPCR.BIT.SPE   = 0;  // Disable RSPI.
         if (0 == (*g_rspi_channels[channel]).SPCR.BIT.SPE)
         {
             R_BSP_NOP();
         }

         #if RSPI_CFG_REQUIRE_LOCK == 1
         /* Release lock for this channel. */
         R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));
         #endif

         /* Transfer complete. Call the user callback function passing pointer to the result structure. */
         if ((FIT_NO_FUNC != g_rspi_handles[channel].pcallback) && (NULL != g_rspi_handles[channel].pcallback))
         {
             g_rspi_cb_data[channel].handle = &(g_rspi_handles[channel]);
             g_rspi_cb_data[channel].event_code = RSPI_EVT_TRANSFER_COMPLETE;
             g_rspi_handles[channel].pcallback((void*)&(g_rspi_cb_data[channel]));
         }
    }
}

#if RSPI_CFG_USE_CHAN0 == 1
R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spri0_isr, VECT(RSPI0, SPRI0))
/******************************************************************************
* Function Name:    rspi_spri0_isr
* Description  :    RSPI SPRI receive buffer full ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spri0_isr(void)
{
    if (RSPI_TRANS_MODE_SW == g_rspi_tcb[0].data_tran_mode)
    {
        g_rxdata[0] = (*g_rspi_channels[0]).SPDR.LONG; // Need to read RX data reg ASAP.
        g_rspi_tcb[0].rx_count++;
#if RSPI_CFG_HIGH_SPEED_READ == 0
        rspi_tx_common(0);
#endif
        rspi_rx_common(0);
    }
    else
    {
        R_RSPI_IntSpriIerClear(&g_rspi_handles[0]);
        R_RSPI_IntSpriDmacdtcFlagSet(&g_rspi_handles[0], RSPI_SET_TRANS_STOP);
        /* Transfer complete. Call the user callback function passing pointer to the result structure. */
        if ((FIT_NO_FUNC != g_rspi_handles[0].pcallback) && (NULL != g_rspi_handles[0].pcallback))
        {
            g_rspi_cb_data[0].handle = &(g_rspi_handles[0]);
            g_rspi_cb_data[0].event_code = RSPI_EVT_TRANSFER_COMPLETE;
            g_rspi_handles[0].pcallback((void*)&(g_rspi_cb_data[0]));
        }
    }
} /* end rspi_spri0_isr */
#endif

#if RSPI_CFG_USE_CHAN1 == 1
R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spri1_isr, VECT(RSPI1, SPRI1))
/******************************************************************************
* Function Name:    rspi_spri1_isr
* Description  :    RSPI SPRI receive buffer full ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spri1_isr(void)
{
    if (RSPI_TRANS_MODE_SW == g_rspi_tcb[1].data_tran_mode)
    {
        g_rxdata[1] = (*g_rspi_channels[1]).SPDR.LONG; // Need to read RX data reg ASAP.
        g_rspi_tcb[1].rx_count++;
#if RSPI_CFG_HIGH_SPEED_READ == 0
        rspi_tx_common(1);
#endif
        rspi_rx_common(1);
    }
    else
    {
        R_RSPI_IntSpriIerClear(&g_rspi_handles[1]);
        R_RSPI_IntSpriDmacdtcFlagSet(&g_rspi_handles[1], RSPI_SET_TRANS_STOP);
        /* Transfer complete. Call the user callback function passing pointer to the result structure. */
        if ((FIT_NO_FUNC != g_rspi_handles[1].pcallback) && (NULL != g_rspi_handles[1].pcallback))
        {
            g_rspi_cb_data[1].handle = &(g_rspi_handles[1]);
            g_rspi_cb_data[1].event_code = RSPI_EVT_TRANSFER_COMPLETE;
            g_rspi_handles[1].pcallback((void*)&(g_rspi_cb_data[1]));
        }
    }
} /* end rspi_spri1_isr */
#endif

#if RSPI_CFG_USE_CHAN2 == 1
R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spri2_isr, VECT(RSPI2, SPRI2))
/******************************************************************************
* Function Name:    rspi_spri2_isr
* Description  :    RSPI SPRI receive buffer full ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spri2_isr(void)
{
    if (RSPI_TRANS_MODE_SW == g_rspi_tcb[2].data_tran_mode)
    {
        g_rxdata[2] = (*g_rspi_channels[2]).SPDR.LONG; // Need to read RX data reg ASAP.
        g_rspi_tcb[2].rx_count++;
#if RSPI_CFG_HIGH_SPEED_READ == 0
        rspi_tx_common(2);
#endif
        rspi_rx_common(2);
    }
    else
    {
        R_RSPI_IntSpriIerClear(&g_rspi_handles[2]);
        R_RSPI_IntSpriDmacdtcFlagSet(&g_rspi_handles[2], RSPI_SET_TRANS_STOP);
        /* Transfer complete. Call the user callback function passing pointer to the result structure. */
        if ((FIT_NO_FUNC != g_rspi_handles[2].pcallback) && (NULL != g_rspi_handles[2].pcallback))
        {
            g_rspi_cb_data[2].handle = &(g_rspi_handles[2]);
            g_rspi_cb_data[2].event_code = RSPI_EVT_TRANSFER_COMPLETE;
            g_rspi_handles[2].pcallback((void*)&(g_rspi_cb_data[2]));
        }
    }
} /* end rspi_spri2_isr */
#endif
/* end SPRI  */

#if RSPI_CFG_USE_CHAN0 == 1
R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spti0_isr, VECT(RSPI0, SPTI0))
/******************************************************************************
* Function Name:    rspi_spti0_isr
* Description  :    RSPI SPTI transmit buffer empty ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spti0_isr(void)
{
    if (RSPI_TRANS_MODE_SW == g_rspi_tcb[0].data_tran_mode)
    {
        g_rxdata[0] = RSPI0.SPDR.LONG; // Read rx-data register into temp buffer.

            /* If master mode then disable further spti interrupts on first transmit.
            If slave mode then we do two transmits to fill the double buffer,
            then disable spti interrupts.
            The receive interrupt will handle any remaining data. */
#if RSPI_CFG_HIGH_SPEED_READ == 0
            if ((RSPI0.SPCR.BIT.MSTR) || (g_rspi_tcb[0].tx_count > 0))
            {
                RSPI0.SPCR.BIT.SPTIE = 0;  // Disable SPTI interrupt.
                if (0 == RSPI0.SPCR.BIT.SPTIE)
                {
                    R_BSP_NOP();
                }
            }
#endif
        rspi_tx_common(0);

        if (g_rspi_tcb[0].transfer_mode & RSPI_DO_RX)
        {    /* Count was incremented in the call to rspi_tx_rx_common. */
            if ((RSPI0.SPCR.BIT.MSTR) || (g_rspi_tcb[0].tx_count > 1))
            {
                g_rspi_tcb[0].do_rx_now = true; // Enables saving of receive data on next receive interrupt.
            }
        }
    }
    else
    {
        R_RSPI_IntSptiIerClear(&g_rspi_handles[0]);
        R_RSPI_IntSptiDmacdtcFlagSet(&g_rspi_handles[0], RSPI_SET_TRANS_STOP);
    }
} /* end rspi_spti0_isr */
#endif

#if RSPI_CFG_USE_CHAN1 == 1
R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spti1_isr, VECT(RSPI1, SPTI1))
/******************************************************************************
* Function Name:    rspi_spti1_isr
* Description  :    RSPI SPTI transmit buffer empty ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spti1_isr(void)
{
    if (RSPI_TRANS_MODE_SW == g_rspi_tcb[1].data_tran_mode)
    {
        g_rxdata[1] = RSPI1.SPDR.LONG; // Read rx-data register into temp buffer.

            /* If master mode then disable further spti interrupts on first transmit.
            If slave mode then we do two transmits to fill the double buffer,
            then disable spti interrupts.
            The receive interrupt will handle any remaining data. */
#if RSPI_CFG_HIGH_SPEED_READ == 0
            if ((RSPI1.SPCR.BIT.MSTR) || (g_rspi_tcb[1].tx_count > 0))
            {
                RSPI1.SPCR.BIT.SPTIE = 0;  // Disable SPTI interrupt.
                if (0 == RSPI1.SPCR.BIT.SPTIE)
                {
                    R_BSP_NOP();
                }
            }
#endif
        rspi_tx_common(1);

        if (g_rspi_tcb[1].transfer_mode & RSPI_DO_RX)
        {    /* Count was incremented in the call to rspi_tx_rx_common. */
            if ((RSPI1.SPCR.BIT.MSTR) || (g_rspi_tcb[1].tx_count > 1))
            {
                g_rspi_tcb[1].do_rx_now = true; // Enables saving of receive data on next receive interrupt.
            }
        }
    }
    else
    {
        R_RSPI_IntSptiIerClear(&g_rspi_handles[1]);
        R_RSPI_IntSptiDmacdtcFlagSet(&g_rspi_handles[1], RSPI_SET_TRANS_STOP);
    }
} /* end rspi_spti1_isr */
#endif

#if RSPI_CFG_USE_CHAN2 == 1
R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spti2_isr, VECT(RSPI2, SPTI2))
/******************************************************************************
* Function Name:    rspi_spti2_isr
* Description  :    RSPI SPTI transmit buffer empty ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spti2_isr(void)
{
    if (RSPI_TRANS_MODE_SW == g_rspi_tcb[2].data_tran_mode)
    {
        g_rxdata[2] = RSPI2.SPDR.LONG; // Read rx-data register into temp buffer.

            /* If master mode then disable further spti interrupts on first transmit.
            If slave mode then we do two transmits to fill the double buffer,
            then disable spti interrupts.
            The receive interrupt will handle any remaining data. */
#if RSPI_CFG_HIGH_SPEED_READ == 0
            if ((RSPI2.SPCR.BIT.MSTR) || (g_rspi_tcb[2].tx_count > 0))
            {
                RSPI2.SPCR.BIT.SPTIE = 0;  // Disable SPTI interrupt.
                if (0 == RSPI2.SPCR.BIT.SPTIE)
                {
                    R_BSP_NOP();
                }
            }
#endif
        rspi_tx_common(2);

        if (g_rspi_tcb[2].transfer_mode & RSPI_DO_RX)
        {    /* Count was incremented in the call to rspi_tx_rx_common. */
            if ((RSPI2.SPCR.BIT.MSTR) || (g_rspi_tcb[2].tx_count > 1))
            {
                g_rspi_tcb[2].do_rx_now = true; // Enables saving of receive data on next receive interrupt.
            }
        }
    }
    else
    {
        R_RSPI_IntSptiIerClear(&g_rspi_handles[2]);
        R_RSPI_IntSptiDmacdtcFlagSet(&g_rspi_handles[2], RSPI_SET_TRANS_STOP);
    }
} /* end rspi_spti2_isr */
#endif
/* end SPTI  */

/******************************************************************************
* Function Name:    rspi_spei_isr_common
* Description  :    common ISR handler for SPEI RSPI-error
* Arguments    :    RSPI channel
* Return Value :    N/A
******************************************************************************/
static void rspi_spei_isr_common(uint8_t channel)
{
    uint8_t status_flags = (*g_rspi_channels[channel]).SPSR.BYTE;
    rspi_evt_t event = RSPI_EVT_ERR_UNDEF;

    /* Identify and clear error condition. */
    if (status_flags & RSPI_SPSR_OVRF) // Overrun error.
    {
        if (RSPI_EVT_ERR_UNDEF == event)
        {
            event = RSPI_EVT_ERR_READ_OVF;
        }
        /* Clear error source: OVRF flag. */
        (*g_rspi_channels[channel]).SPSR.BIT.OVRF = 0;
    }
    
    if (status_flags & RSPI_SPSR_MODF)
    {
        if (status_flags & RSPI_SPSR_UDRF)
        {
            if (RSPI_EVT_ERR_UNDEF == event)
            {
                event = RSPI_EVT_ERR_UNDER_RUN;
            }
            /* Clear error source: MODF flag and UDRF. */
            (*g_rspi_channels[channel]).SPSR.BYTE &= RSPI_SPSR_MODF_UDRF_MASK;
        }
        else
        {
            if (RSPI_EVT_ERR_UNDEF == event)
            {
                event = RSPI_EVT_ERR_MODE_FAULT;
            }
            /* Clear error source: MODF flag. */
            (*g_rspi_channels[channel]).SPSR.BIT.MODF = 0;
        }
    }
    
    if (status_flags & RSPI_SPSR_PERF)
    {
        if (RSPI_EVT_ERR_UNDEF == event)
        {
            event = RSPI_EVT_ERR_PARITY;
        }
        /* Clear error source: PERF flag. */
        (*g_rspi_channels[channel]).SPSR.BIT.PERF = 0;
    }

    g_rspi_cb_data[channel].event_code = event;

    /* Disable the RSPI operation. */
    (*g_rspi_channels[channel]).SPCR.BYTE &= (uint8_t)(~((RSPI_SPCR_SPTIE | RSPI_SPCR_SPRIE) | RSPI_SPCR_SPE));
    if (0 == (*g_rspi_channels[channel]).SPCR.BYTE)
    {
        R_BSP_NOP();
    }

    #if RSPI_CFG_REQUIRE_LOCK == 1
    /* Release lock for this channel. */
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_RSPI0 + channel));
    #endif

    /* Call the user callback function passing pointer to the result structure. */
    if ((FIT_NO_FUNC != g_rspi_handles[channel].pcallback) && (NULL != g_rspi_handles[channel].pcallback))
    {
        g_rspi_cb_data[channel].handle = &(g_rspi_handles[channel]);
        g_rspi_handles[channel].pcallback((void*)&(g_rspi_cb_data[channel]));
    }
} /* end rspi_spei_isr_common() */


/******************************************************************************
* Function Name:    rspi_spei0_isr, rspi_spei1_isr, rspi_spei2_isr
* Description  :    RSPI SPEI RSPI-error ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
#ifndef BSP_MCU_RX63_ALL  /* This interrupt not present in RX63 or RX64M series. */
#ifndef BSP_MCU_RX64M
#ifndef BSP_MCU_RX65N
#ifndef BSP_MCU_RX66T
#ifndef BSP_MCU_RX71M
#ifndef BSP_MCU_RX72T
    #if RSPI_CFG_USE_CHAN0 == 1
    R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spei0_isr, VECT(RSPI0, SPEI0))
    R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spei0_isr(void)
    {
        rspi_spei_isr_common(0);
    } /* end rspi_spei0_isr */
    #endif

    #if RSPI_CFG_USE_CHAN1 == 1
    R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spei1_isr, VECT(RSPI1, SPEI1))
    R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spei1_isr(void)
    {
        rspi_spei_isr_common(1);
    } /* end rspi_spei1_isr */
    #endif

    #if RSPI_CFG_USE_CHAN2 == 1
    R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spei2_isr, VECT(RSPI2, SPEI2))
    R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spei2_isr(void)
    {
        rspi_spei_isr_common(2);
    } /* end rspi_spei2_isr */
    #endif
#endif
#endif
#endif
#endif
#endif
#endif

#ifdef BSP_MCU_RX63_ALL
    #if RSPI_CFG_USE_RX63_ERROR_INTERRUPT == 1
    R_BSP_PRAGMA_STATIC_INTERRUPT(rspi_spei_63_isr, VECT(ICU, GROUP12))
    R_BSP_ATTRIB_STATIC_INTERRUPT void rspi_spei_63_isr(void)
    {
        /* Get the interrupt source from the group interrupt source register. */
        #if RSPI_CFG_USE_CHAN0 == 1
        if (IS(RSPI0, SPEI0))
        {
            rspi_spei_isr_common(0);
        }
        #endif

        #if RSPI_CFG_USE_CHAN1 == 1
        if (IS(RSPI1, SPEI1))
        {
            rspi_spei_isr_common(1);
        }
        #endif

        #if RSPI_CFG_USE_CHAN2 == 1
        if (IS(RSPI2, SPEI2))
        {
            rspi_spei_isr_common(2);
        }
        #endif
    } /* end rspi_spei_63_isr */
    #endif
#endif

/******************************************************************************
* Function Name:    rspi_spei_grp_isr
* Description  :    BSP group interrupt handler for register the error callback function
* Arguments    :    pdata
* Return Value :    N/A
******************************************************************************/
static void rspi_spei_grp_isr(void *pdata)
{
    /* Called from BSP group interrupt handler. */
    #if defined BSP_MCU_RX64M || defined BSP_MCU_RX71M || defined BSP_MCU_RX65N || defined BSP_MCU_RX66T || defined BSP_MCU_RX72T
    #if RSPI_CFG_USE_CHAN0 == 1
    if (IS(RSPI0, SPEI0))
    {
        rspi_spei_isr_common(0);
    }
    #endif
    #endif

    #if defined BSP_MCU_RX71M || defined BSP_MCU_RX65N
    #if RSPI_CFG_USE_CHAN1 == 1
    if (IS(RSPI1, SPEI1))
    {
        rspi_spei_isr_common(1);
    }
    #endif
    #endif
    
    #if defined BSP_MCU_RX65N
    #if RSPI_CFG_USE_CHAN2 == 1
    if (IS(RSPI2, SPEI2))
    {
        rspi_spei_isr_common(2);
    }
    #endif
    #endif
}

/* end SPRI  */
