/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : r_ether_rza2_if.h
 * Version      : 1.11
 * Description  : Ethernet module device driver
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
    #include <stdint.h>

    #include "r_ether_rza2_config.h"
    #include "phy.h"

/* Guards against multiple inclusion */
#ifndef R_ETHER_RZA2_IF_H
    #define R_ETHER_RZA2_IF_H


#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Version Number of API. */
    #define ETHER_RZA2_VERSION_MAJOR  (1)
    #define ETHER_RZA2_VERSION_MINOR  (11)

/* When using the Read functions, ETHER_NO_DATA is the return value that indicates that no received data. */
    #define ETHER_NO_DATA             (0)

/* The value of flag which indicates that the interrupt of Ethernet occur. */
    #define ETHER_FLAG_OFF            (0)
    #define ETHER_FLAG_ON             (1)
    #define ETHER_FLAG_ON_LINK_ON     (3)
    #define ETHER_FLAG_ON_LINK_OFF    (2)

/* Channel definition of Ethernet */
    #define ETHER_CHANNEL_0           (0)
    #define ETHER_CHANNEL_1           (1)

    #define ETHER_CHANNEL_MAX         (ETHER_CFG_CH0_EN + ETHER_CFG_CH1_EN)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/* ETHER API error codes */
typedef enum
{
    ETHER_SUCCESS = 0,            /* Processing completed successfully */
    ETHER_ERR_INVALID_PTR = -1,   /* Value of the pointer is NULL */
    ETHER_ERR_INVALID_DATA = -2,  /* Value of the argument is out of range */
    ETHER_ERR_INVALID_CHAN = -3,  /* Nonexistent channel number */
    ETHER_ERR_INVALID_ARG = -4,   /* Invalid argument */
    ETHER_ERR_LINK = -5,          /* Auto-negotiation is not completed, and transmission/reception is not enabled. */
    ETHER_ERR_MPDE = -6,          /* As a Magic Packet is being detected, and transmission/reception is not enabled. */
    ETHER_ERR_TACT = -7,          /* Transmit buffer is not empty. */
    ETHER_ERR_CHAN_OPEN = -8,     /* Indicates the Ethernet cannot be opened 
                                      because it is being used by another application */
    ETHER_ERR_MC_FRAME = -9,      /* Detect multicast frame when multicast frame filtering enable */
    ETHER_ERR_RECV_ENABLE = -10,  /* Enable receive function in ETHERC */
    ETHER_ERR_OTHER = -11         /* Other error */
} ether_return_t;

/* Event code of callback function */
typedef enum
{
    ETHER_CB_EVENT_ID_WAKEON_LAN, /* Magic packet detection */
    ETHER_CB_EVENT_ID_LINK_ON,    /* Link up detection */
    ETHER_CB_EVENT_ID_LINK_OFF,   /* Link down detection */
} ether_cb_event_t;

/* Structure of the callback function pointer */
typedef struct
{
    void (*pcb_func) (void *);    /* Callback function pointer */
    void (*pcb_int_hnd) (void*);  /* Interrupt handler function pointer */
} ether_cb_t;

/* Structure to be used when decoding the argument of the callback function */
typedef struct
{
    uint32_t         channel;     /* ETHERC channel */
    ether_cb_event_t event_id;    /* Event code for callback function */
    uint32_t         status_ecsr; /* ETHERC status register for interrupt handler */
    uint32_t         status_eesr; /* ETHERC/EDMAC status register for interrupt handler */
} ether_cb_arg_t;

/* Parameters of the control function (1st argument) */
typedef enum
{
    CONTROL_SET_CALLBACK,         /* Callback function registration */
    CONTROL_SET_PROMISCUOUS_MODE, /* Promiscuous mode setting */
    CONTROL_SET_INT_HANDLER,      /* Interrupt handler function registration */
    CONTROL_POWER_ON,             /* Cancel ETHERC/EDMAC module stop */
    CONTROL_POWER_OFF,            /* Transition to ETHERC/EDMAC module stop */
    CONTROL_MULTICASTFRAME_FILTER,/* Multicast frame filter setting*/
    CONTROL_BROADCASTFRAME_FILTER /* Broadcast frame filter setting*/
} ether_cmd_t;

typedef enum
{
    ETHER_PROMISCUOUS_OFF,        /* ETHERC operates in standard mode */
    ETHER_PROMISCUOUS_ON          /* ETHERC operates in promiscuous mode */
} ether_promiscuous_bit_t;

typedef enum
{
    ETHER_MC_FILTER_OFF,          /* Multicast frame filter disable */
    ETHER_MC_FILTER_ON            /* Multicast frame filter enable */
} ether_mc_filter_t;

typedef struct
{
    uint32_t                channel; /* ETHERC channel */
    ether_promiscuous_bit_t bit;     /* Promiscuous mode */
} ether_promiscuous_t;

typedef struct
{
    uint32_t          channel;    /* ETHERC channel */
    ether_mc_filter_t flag;       /* Multicast frame filter */
} ether_multicast_t;

typedef struct
{
    uint32_t channel;             /* ETHERC channel */
    uint32_t counter;             /* Continuous reception number of Broadcast frame */
} ether_broadcast_t;

/* Parameters of the control function (2nd argument) */
typedef union
{
    ether_cb_t          ether_callback;        /* Callback function pointer */
    ether_promiscuous_t * p_ether_promiscuous; /* Promiscuous mode setting */
    ether_cb_t          ether_int_hnd;         /* Interrupt handler function pointer */
    uint32_t            channel;               /* ETHERC channel number */
    ether_multicast_t   * p_ether_multicast;   /* Multicast frame filter setting */
    ether_broadcast_t   * p_ether_broadcast;   /* Broadcast frame filter setting */
} ether_param_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * @fn            R_ETHER_Initial
 * @brief         Initialization of Ether driver..
 * @warning       None.
 * @param [in]    None.
 * @param [out]   None.
 * @retval        None.
 *********************************************************************************************************************/
extern void R_ETHER_Initial (void);

/**********************************************************************************************************************
 * @fn            R_ETHER_Open_ZC2
 * @brief         After ETHERC, EDMAC and PHY-LSI are reset in software, an auto negotiation of PHY-LSI is begun.
 *                Afterwards, the link signal change interrupt is permitted.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 *                const uint8_t mac_addr[] - The MAC address of ETHERC
 *                uint8_t pause - Specifies whether flow control functionality is enabled or disabled.
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_INVALID_PTR - Value of the pointer is NULL.
 *                ETHER_ERR_INVALID_DATA - Value of the argument is out of range.
 *                ETHER_ERR_OTHER - Initialization of PHY-LSI failed.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_Open_ZC2 (uint32_t channel, const uint8_t mac_addr[], uint8_t pause);

/**********************************************************************************************************************
 * @fn            R_ETHER_Close_ZC2
 * @brief         Disables Ethernet peripheral
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_Close_ZC2 (uint32_t channel);

/**********************************************************************************************************************
 * @fn            R_ETHER_Read
 * @brief         Receive Ethernet frame. Receives data to the location specified by the pointer to the receive buffer,
 *                using non-zero-copy communication.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 * @param [out]   void *pbuf - The receive buffer (to store the receive data).
 * @retval        ETHER_NO_DATA - A zero value indicates no data is received.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number
 *                ETHER_ERR_INVALID_PTR - Value of the pointer is NULL
 *                ETHER_ERR_LINK - Auto-negotiation is not completed, and reception is not enabled.
 *                ETHER_ERR_MPDE - As a Magic Packet is being detected, transmission and reception is not enabled.
 *                ETHER_ERR_MC_FRAME - Multicast Frame filter is enable, and Multicast Address Frame is received.
 *********************************************************************************************************************/
extern int32_t R_ETHER_Read (uint32_t channel, void *pbuf);

/**********************************************************************************************************************
 * @fn            R_ETHER_Read_ZC2
 * @brief         Receives an Ethernet frame.
 *                Sets the passed buffer pointer to the Ethernet frame buffer from the driver.
 *                This makes the data available to the caller as a zero-copy operation.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 * @param [out]   void **pbuf - Pointer to buffer that holds the receive data.
 * @retval        Returns the number of bytes received.
 *                ETHER_NO_DATA - A zero value indicates no data is received.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_INVALID_PTR - Value of the pointer is NULL.
 *                ETHER_ERR_LINK - Auto-negotiation is not completed, and reception is not enabled.
 *                ETHER_ERR_MPDE - As a Magic Packet is being detected, transmission and reception is not enabled.
 *                ETHER_ERR_MC_FRAME - Multicast Frame filter is enable, and Multicast Address Frame is received.
 *********************************************************************************************************************/
extern int32_t R_ETHER_Read_ZC2 (uint32_t channel, void **pbuf);

/**********************************************************************************************************************
 * @fn            R_ETHER_Read_ZC2_BufRelease
 * @brief         Release the receive buffer.
 * @warning       None.
 * @param [in]    uint32_t channel - Specifies the ETHERC channel number..
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_LINK - Auto-negotiation is not completed, and reception is not enabled.
 *                ETHER_ERR_MPDE - As a Magic Packet is being detected, transmission and reception is not enabled.
 *********************************************************************************************************************/
extern int32_t R_ETHER_Read_ZC2_BufRelease (uint32_t channel);

/**********************************************************************************************************************
 * @fn            R_ETHER_Write
 * @brief         Transmit Ethernet frame. Transmits data from the location specified by the pointer to the transmit.
 *                buffer, with the data size equal to the specified frame length, using non-zero-copy communication.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 *                void *pbuf - Transmit buffer pointer
 *                uint32_t len - The size (60 to 1,514 bytes) which is the Ethernet frame length minus 4 bytes of CRC
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_INVALID_DATA - Value of the argument is out of range.
 *                ETHER_ERR_INVALID_PTR - Value of the pointer is NULL.
 *                ETHER_ERR_LINK - Auto-negotiation is not completed, and reception is not enabled.
 *                ETHER_ERR_MPDE - As a Magic Packet is being detected, transmission and reception is not enabled.
 *                ETHER_ERR_TACT - Transmit buffer is not empty.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_Write (uint32_t channel, void *pbuf, uint32_t len);

/**********************************************************************************************************************
 * @fn            R_ETHER_Write_ZC2_GetBuf
 * @brief         Get Points to the buffer pointer used by the stack.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 *                void **pbuf - Pointer to the starting address of the transmit data destination.
 *                uint16_t *pbuf_size - The Maximum size to write to the buffer.
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_INVALID_PTR - Value of the pointer is NULL.
 *                ETHER_ERR_LINK - Auto-negotiation is not completed, and reception is not enabled.
 *                ETHER_ERR_MPDE - As a Magic Packet is being detected, transmission and reception is not enabled.
 *                ETHER_ERR_TACT - Transmit buffer is not empty.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_Write_ZC2_GetBuf (uint32_t channel, void **pbuf, uint16_t *pbuf_size);

/**********************************************************************************************************************
 * @fn            R_ETHER_Write_ZC2_SetBuf
 * @brief         Transmits an Ethernet frame.
 *                The transmit descriptor points to the data to transmit.
 *                Data is sent directly from memory as a "zero copy" operation.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 *                uint32_t len - The size (60 to 1,514 bytes) which is the Ethernet frame length minus 4 bytes of CRC
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_INVALID_DATA - Value of the argument is out of range
 *                ETHER_ERR_LINK - Auto-negotiation is not completed, and reception is not enabled.
 *                ETHER_ERR_MPDE - As a Magic Packet is being detected, transmission and reception is not enabled.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_Write_ZC2_SetBuf (uint32_t channel, const uint32_t len);

/**********************************************************************************************************************
 * @fn            R_ETHER_CheckLink_ZC
 * @brief         Verifies the Etherent link is up or not..
 * @warning       none.
 * @param [in]    uint32_t channel - ETHERC channel number.
 * @param [out]   none.
 * @retval        ETHER_SUCCESS - Link is up.
 *                ETHER_ERR_OTHER - Link is down.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_CheckLink_ZC (uint32_t channel);

/**********************************************************************************************************************
 * @fn            R_ETHER_LinkProcess
 * @brief         The Link up processing, the Link down processing, and the magic packet detection processing are
 *                executed.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 * @param [out]   None.
 * @retval        None.
 *********************************************************************************************************************/
extern void R_ETHER_LinkProcess (uint32_t channel);

/**********************************************************************************************************************
 * @fn            R_ETHER_WakeOnLAN
 * @brief         The setting of ETHERC is changed from a usual sending and
 *                receiving mode to the magic packet detection mode.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number..
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_LINK - Auto-negotiation is not completed, and reception is not enabled.
 *                ETHER_ERR_OTHER - A switch to magic packet detection was performed when the link state was link is
 *                                  down.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_WakeOnLAN (uint32_t channel);

/**********************************************************************************************************************
 * @fn            R_ETHER_CheckWrite
 * @brief         Checking that the data has been sent.
 * @warning       None.
 * @param [in]    uint32_t channel - ETHERC channel number.
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_CheckWrite (uint32_t channel);

/**********************************************************************************************************************
 * @fn            R_ETHER_Control
 * @brief         By command argument is a function to change the settings of Ether driver.
 * @warning       None.
 * @param [in]    ether_cmd_t const cmd - Control code.
 *                ether_param_t const control - Parameters according to the control code.
 * @param [out]   None.
 * @retval        ETHER_SUCCESS - Processing completed successfully.
 *                ETHER_ERR_INVALID_CHAN - Nonexistent channel number.
 *                ETHER_ERR_CHAN_OPEN - Indicates the Ethernet cannot be opened because it is being used by another
 *                                      application.
 *                ETHER_ERR_INVALID_ARG - Invalid argument.
 *                ETHER_ERR_RECV_ENABLE - Receive function is enable.
 *********************************************************************************************************************/
extern ether_return_t R_ETHER_Control (ether_cmd_t const cmd, ether_param_t const control);

/**********************************************************************************************************************
 * @fn            R_ETHER_GetVersion
 * @brief         Returns this module's version information.
 *                The version number is encoded where the top 2 bytes are the major version number and the bottom
 *                2 bytes.
 * @warning       None.
 * @param [in]    None.
 * @param [out]   None.
 * @retval        Version number.
 *********************************************************************************************************************/
extern uint32_t R_ETHER_GetVersion (void);

#ifdef __cplusplus
}
#endif

#endif  /* R_ETHER_RZA2_IF_H*/
