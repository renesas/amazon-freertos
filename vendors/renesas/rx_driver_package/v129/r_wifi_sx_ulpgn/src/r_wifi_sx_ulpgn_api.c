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
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_wifi_sx_ulpgn_api.c
 * Version      : 1.0
 * Description  : API functions definition for SX ULPGN of RX65N.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2020 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "FreeRTOS.h"

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#include "platform.h"
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#include "r_sci_rx_if.h"
#include "r_sci_rx_pinset.h"
#if defined(BSP_MCU_RX72N)
#include "../r_sci_rx/src/targets/rx72n/r_sci_rx72n_private.h"
#elif defined(BSP_MCU_RX65N)
#include "../r_sci_rx/src/targets/rx65n/r_sci_rx65n_private.h"
#elif defined(BSP_MCU_RX671)
#include "../r_sci_rx/src/targets/rx671/r_sci_rx671_private.h"
#else
#error "Include the appropriate file for your MCU."
#endif
#endif
#include "r_byteq_if.h"
#include "r_wifi_sx_ulpgn_if.h"
#include "r_wifi_sx_ulpgn_private.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#if !defined(WIFI_CFG_SCI_CHANNEL)
#error "Error! Need to define WIFI_CFG_SCI_CHANNEL in r_wifi_sx_ulpgn_config.h"
#elif WIFI_CFG_SCI_CHANNEL == (0)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI0)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH0)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH0_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH0_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (1)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI1)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH1)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH1_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH1_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (2)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI2)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH2)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH2_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH2_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (3)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI3)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH3)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH3_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH3_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (4)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI4)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH4)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH4_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH4_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (5)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI5)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH5)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH5_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH5_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (6)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI6)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH6)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH6_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH6_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (7)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI7)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH7)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH7_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH7_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (8)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI8)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH8)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH8_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH8_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (9)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI9)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH9)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH9_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH9_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (10)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI10)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH10)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH10_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH10_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (11)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI11)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH11)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH11_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH11_RX_BUFSIZ)
#elif WIFI_CFG_SCI_CHANNEL == (12)
#define R_SCI_PINSET_FUNC_DEFAULT            (R_SCI_PinSet_SCI12)
#define SCI_CH_WIFI_DEFAULT                  (SCI_CH12)
#define SCI_TX_BUSIZ_DEFAULT                 (SCI_CFG_CH12_TX_BUFSIZ)
#define SCI_RX_BUSIZ_DEFAULT                 (SCI_CFG_CH12_RX_BUFSIZ)
#else
#error "Error! Invalid setting for WIFI_CFG_SCI_CHANNEL in r_wifi_sx_ulpgn_config.h"
#endif /* !definedWIFI_CFG_SCI_CHANNEL */

#if !defined(WIFI_CFG_SCI_SECOND_CHANNEL)
#error "Error! Need to define WIFI_CFG_SCI_SECOND_CHANNEL in r_bsp_config.h"
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (0)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI0)
#define SCI_CH_WIFI_SECOND                   (SCI_CH0)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH0_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH0_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (1)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI1)
#define SCI_CH_WIFI_SECOND                   (SCI_CH1)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH1_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH1_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (2)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI2)
#define SCI_CH_WIFI_SECOND                   (SCI_CH2)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH2_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH2_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (3)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI3)
#define SCI_CH_WIFI_SECOND                   (SCI_CH3)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH3_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH3_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (4)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI4)
#define SCI_CH_WIFI_SECOND                   (SCI_CH4)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH4_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH4_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (5)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI5)
#define SCI_CH_WIFI_SECOND                   (SCI_CH5)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH5_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH5_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (6)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI6)
#define SCI_CH_WIFI_SECOND                   (SCI_CH6)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH6_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH6_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (7)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI7)
#define SCI_CH_WIFI_SECOND                   (SCI_CH7)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH7_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH7_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (8)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI8)
#define SCI_CH_WIFI_SECOND                   (SCI_CH8)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH8_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH8_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (9)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI9)
#define SCI_CH_WIFI_SECOND                   (SCI_CH9)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH9_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH9_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (10)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI10)
#define SCI_CH_WIFI_SECOND                   (SCI_CH10)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH10_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH10_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (11)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI11)
#define SCI_CH_WIFI_SECOND                   (SCI_CH11)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH11_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH11_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (12)
#define R_SCI_PINSET_FUNC_SECOND             (R_SCI_PinSet_SCI12)
#define SCI_CH_WIFI_SECOND                   (SCI_CH12)
#define SCI_TX_BUSIZ_SECOND                  (SCI_CFG_CH12_TX_BUFSIZ)
#define SCI_RX_BUSIZ_SECOND                  (SCI_CFG_CH12_RX_BUFSIZ)
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (-1)
#define R_SCI_PINSET_FUNC_SECOND
#define SCI_CH_WIFI_SECOND
#define SCI_TX_BUSIZ_SECOND
#define SCI_RX_BUSIZ_SECOND
#else
#error "Error! Invalid setting for WIFI_CFG_SCI_SECOND_CHANNEL in r_bsp_config.h"
#endif /* !definedWIFI_CFG_SCI_SECOND_CHANNEL */

#define ULPGN_UART_DEFAULT_PORT (0)
#define ULPGN_UART_SECOND_PORT  (1)
#define MUTEX_TX                (1 << 0)
#define MUTEX_RX                (1 << 1)
#define NULL_PTR                ((void *)0)   /* Null pointer */

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
uint8_t g_use_uart_num = 2;

uint32_t g_wifi_tx_busiz_command;
uint32_t g_wifi_tx_busiz_data;
uint32_t g_wifi_rx_busiz_command;
uint32_t g_wifi_rx_busiz_data;

uint8_t g_wifi_createble_sockets;
uint8_t g_atcmd_port;
uint8_t g_data_port;

uint8_t g_wifi_transparent_mode = 0;

extern TaskHandle_t g_wifi_recv_task_handle;

const uint8_t g_ulpgn_socket_status_closed[] = ULPGN_SOCKET_STATUS_TEXT_CLOSED;
const uint8_t g_ulpgn_socket_status_socket[] = ULPGN_SOCKET_STATUS_TEXT_SOCKET;
const uint8_t g_ulpgn_socket_status_bound[] = ULPGN_SOCKET_STATUS_TEXT_BOUND;
const uint8_t g_ulpgn_socket_status_listen[] = ULPGN_SOCKET_STATUS_TEXT_LISTEN;
const uint8_t g_ulpgn_socket_status_connected[] = ULPGN_SOCKET_STATUS_TEXT_CONNECTED;
const uint8_t g_ulpgn_socket_status_broken[] = ULPGN_SOCKET_STATUS_TEXT_BROKEN;

const uint8_t g_ulpgn_return_dummy[] = "";

const uint32_t g_wifi_serial_buffsize_table[2][4] =
        {
                /*SCI_TX_BUSIZ_DEFAULT*//*SCI_TX_BUSIZ_SIZE*//*SCI_RX_BUSIZ_DEFAULT*//*SCI_RX_BUSIZ_SIZE*/
                /*g_wifi_using_uart_num = 1 */
                { SCI_TX_BUSIZ_DEFAULT, SCI_TX_BUSIZ_DEFAULT, SCI_RX_BUSIZ_DEFAULT, SCI_RX_BUSIZ_DEFAULT },
                /*g_wifi_using_uart_num = 2 */
                { SCI_TX_BUSIZ_SECOND, SCI_TX_BUSIZ_DEFAULT, SCI_TX_BUSIZ_SECOND, SCI_RX_BUSIZ_DEFAULT }
        };

const uint8_t *const gp_wifi_socket_status_tbl[ULPGN_SOCKET_STATUS_MAX] =
    {
        g_ulpgn_socket_status_closed,
        g_ulpgn_socket_status_socket,
        g_ulpgn_socket_status_bound,
        g_ulpgn_socket_status_listen,
        g_ulpgn_socket_status_connected,
		g_ulpgn_socket_status_broken,
    };

volatile uint8_t g_current_socket_index;
volatile uint8_t g_before_socket_index;

#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
st_cert_profile_t g_cert_profile[5];
#else
st_cert_profile_t g_cert_profile[2];
#endif

/* Wifi Execute Command Timeout */
uint16_t g_atcmd_timeout1 = EXECUTE_COMMAND_TIMEOUT_DEFAULT1;
uint16_t g_atcmd_timeout2 = EXECUTE_COMMAND_TIMEOUT_DEFAULT2;
uint16_t g_atcmd_timeout3 = EXECUTE_COMMAND_TIMEOUT_DEFAULT3;
uint16_t g_atcmd_timeout4 = EXECUTE_COMMAND_TIMEOUT_DEFAULT4;
uint16_t g_atcmd_timeout5 = EXECUTE_COMMAND_TIMEOUT_DEFAULT5;

wifi_system_status_t g_wifi_system_state = WIFI_SYSTEM_CLOSE;
uint8_t g_wifi_atcmd_buf[WIFI_AT_COMMAND_BUFF_SIZE];
uint8_t g_wifi_resp_buf[WIFI_AT_RESPONSE_BUFF_SIZE];

st_atcmd_info_t g_wifi_uart[WIFI_NUMBER_OF_USE_UART];

uint32_t g_wifi_tx_busiz_command;
uint32_t g_wifi_tx_busiz_data;
uint32_t g_wifi_rx_busiz_command;
uint32_t g_wifi_rx_busiz_data;
uint32_t g_wifi_sci_err_flag;


/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static int32_t wifi_execute_at_command (uint8_t serial_ch_id, const uint8_t * ptextstring, uint16_t timeout_ms,
        wifi_return_code_t expect_code, wifi_command_list_t command, int32_t socket_number);

static int32_t wifi_take_mutex (uint8_t mutex_flag);
static void wifi_give_mutex (uint8_t mutex_flag);

static void wifi_uart_callback_second_port_for_command (void * pArgs);
static void wifi_uart_callback_default_port_for_inititial (void * pArgs);
static void wifi_uart_callback_default_port_for_data (void * pArgs);
static void timeout_init (int32_t serial_ch, uint16_t timeout_ms);
static int32_t check_timeout (int32_t serial_ch, int32_t rcvcount);
static void socket_timeout_init (uint8_t socket_number, uint32_t timeout_ms, uint8_t flag);
static int32_t socket_check_timeout (uint8_t socket_number, uint8_t flag);

static int32_t wifi_get_ipaddress (void);
static int32_t wifi_serial_open_for_initial (void);
static int32_t wifi_serial_open_for_data (void);
static int32_t wifi_serial_default_port_close (void);
static int32_t wifi_change_command_mode (void);
static int32_t wifi_change_transparent_mode (void);
static int32_t wifi_serial_second_port_open (void);
static int32_t wifi_serial_second_port_close (void);
static int32_t wifi_check_uart_state (uint32_t * uart_receive_status, uint32_t * uart_send_status);

static int32_t wifi_serial_close (void);

static int32_t wifi_get_socket_status (uint8_t socket_number);
static int32_t wifi_change_socket_index (uint8_t socket_number);
static wifi_err_t wifi_setsslconfiguration (int32_t socket_number, uint8_t ssl_type);
static uint32_t erase_certificate (uint8_t * certificate_name);

/**
 * @brief The global mutex to ensure that only one operation is accessing the
 * s_wifi_tx_semaphore flag at one time.
 */
static SemaphoreHandle_t s_wifi_tx_semaphore = NULL;
static SemaphoreHandle_t s_wifi_rx_semaphore = NULL;

/**
 * @brief Maximum time in ticks to wait for obtaining a semaphore.
 */
static const TickType_t s_xMaxSemaphoreBlockTime = pdMS_TO_TICKS(10000UL);

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_Open
 * Description  : WiFi Module Open.
 * Arguments    : none.
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_SERIAL_OPEN
 *                WIFI_ERR_SOCKET_BYTEQ
 *                WIFI_ERR_ALREADY_OPEN
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_Open(void)
{
    int32_t ret;
    sci_baud_t change_baud;
    wifi_err_t api_ret = WIFI_SUCCESS;
    uint8_t open_phase;
    uint8_t byteq_open_count;
    int32_t k;

    if (WIFI_SYSTEM_CLOSE != g_wifi_system_state)
    {
        return WIFI_ERR_ALREADY_OPEN;
    }

    /* Memory initialize */
    g_atcmd_port = WIFI_UART_COMMAND_PORT;
    memset(g_wifi_uart, 0, sizeof(g_wifi_uart));
    g_wifi_uart[WIFI_UART_COMMAND_PORT].p_cmdbuf     = g_wifi_atcmd_buf;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].cmdbuf_size  = sizeof(g_wifi_atcmd_buf);
    g_wifi_uart[WIFI_UART_COMMAND_PORT].p_respbuf    = g_wifi_resp_buf;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].respbuf_size = sizeof(g_wifi_resp_buf);
    g_wifi_uart[WIFI_UART_DATA_PORT].p_cmdbuf        = g_wifi_atcmd_buf;
    g_wifi_uart[WIFI_UART_DATA_PORT].cmdbuf_size     = sizeof(g_wifi_atcmd_buf);
    g_wifi_uart[WIFI_UART_DATA_PORT].p_respbuf       = g_wifi_resp_buf;
    g_wifi_uart[WIFI_UART_DATA_PORT].respbuf_size    = sizeof(g_wifi_resp_buf);
    g_wifi_tx_busiz_command = g_wifi_serial_buffsize_table[0][0];
    g_wifi_tx_busiz_data = g_wifi_serial_buffsize_table[0][0];
    g_wifi_rx_busiz_command = g_wifi_serial_buffsize_table[0][1];
    g_wifi_rx_busiz_data = g_wifi_serial_buffsize_table[0][1];

    memset(g_wifi_socket, 0, sizeof(g_wifi_socket));
    wifi_init_at_execute_queue();
    open_phase = 0;

    /* Phase 1: ByteQ initialize */
    for (byteq_open_count = 0; byteq_open_count < WIFI_CFG_CREATABLE_SOCKETS; byteq_open_count++ )
    {
        if (BYTEQ_SUCCESS != R_BYTEQ_Open(g_wifi_socket[byteq_open_count].socket_recv_buff,
                sizeof(g_wifi_socket[byteq_open_count].socket_recv_buff),
                &g_wifi_socket[byteq_open_count].socket_byteq_hdl))
        {
            break;
        }
    }

    if (WIFI_CFG_CREATABLE_SOCKETS >= byteq_open_count)
    {
        /* Success */
        open_phase |= 0x01;
    }
    else
    {
        api_ret = WIFI_ERR_BYTEQ_OPEN;
    }

    if (WIFI_SUCCESS == api_ret)
    {
        /* Phase 3 Semaphore tx initialize */
        if (NULL != s_wifi_tx_semaphore)
        {
            /* tx semaphore */
            vSemaphoreDelete(s_wifi_tx_semaphore);
        }
        s_wifi_tx_semaphore = xSemaphoreCreateMutex();

        if (NULL != s_wifi_tx_semaphore)
        {
            /* Success */
            open_phase |= 0x02;
        }
        else
        {
            api_ret = WIFI_ERR_TAKE_MUTEX;
        }
    }

    if (WIFI_SUCCESS == api_ret)
    {
        /* Phase 4 Semaphore rx initialize */
        if (NULL != s_wifi_rx_semaphore)
        {
            /* rx semaphore */
            vSemaphoreDelete(s_wifi_rx_semaphore);
        }

        s_wifi_rx_semaphore = xSemaphoreCreateMutex();

        if (NULL != s_wifi_rx_semaphore)
        {
            /* Success */
            open_phase |= 0x04;
        }
        else
        {
            api_ret = WIFI_ERR_TAKE_MUTEX;
        }
    }

    if (WIFI_SUCCESS == api_ret)
    {
        /* Phase 4 Serial initialize */
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
        WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 0;
        WIFI_RESET_DDR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
#endif
        vTaskDelay(26);
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
        WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
#endif
        vTaskDelay(250);

        ret = wifi_serial_open_for_initial();
        if (0 == ret)
        {
            /* Success */
            open_phase |= 0x08;
        }
        else
        {
            api_ret = WIFI_ERR_SERIAL_OPEN;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        ret = wifi_serial_second_port_open();
        if (0 == ret)
        {
            /* Success */
            open_phase |= 0x10;
        }
        else
        {
            api_ret = WIFI_ERR_SERIAL_OPEN;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Phase 5 Task initialize */
        if (0 == wifi_start_recv_task())
        {
            vTaskDelay(2);

            /* Success */
            open_phase |= 0x20;
        }
        else
        {
            api_ret = WIFI_ERR_SERIAL_OPEN;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* no echo */
        ret = wifi_execute_at_command(g_atcmd_port, "ATE0\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ECHO_OFF, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }

    if (WIFI_SUCCESS == api_ret)
    {
        /* no echo */
        ret = wifi_execute_at_command(g_atcmd_port, "ATWREV\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_MODULE_VERSION, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Command Port = HSUART2 */
        ret = wifi_execute_at_command(g_atcmd_port, "ATUART=2\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_CHANGE_TO_2, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
        else
        {
        	vTaskDelay(1000); /* 1 sec */
            g_atcmd_port = ULPGN_UART_SECOND_PORT;
            wifi_serial_default_port_close();
            open_phase &= (~0x08);
        }
    }

    if (WIFI_SUCCESS == api_ret)
    {
        /* Command Port = HSUART2, Data Port = HSUART1 */
        ret = wifi_execute_at_command(g_atcmd_port, "ATE0\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ECHO_OFF, 0xff);
        if (0 == ret)
        {
            /* Multiple Connection */
            /* Change HSUART1 baudrate and flow control. */
#if (WIFI_CFG_SCI_USE_FLOW_CONTROL == 1)
            /* AT command */
            sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATBX1=%d,,,,h\r", WIFI_CFG_SCI_BAUDRATE);
#else
            sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATBX1=%d,,,,\r", WIFI_CFG_SCI_BAUDRATE);
#endif
            ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                    g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_HISPEED, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_MODULE_COM;
            }
            if (WIFI_SUCCESS == api_ret)
            {
                ret = wifi_serial_open_for_data();
                if (0 == ret)
                {
                    open_phase |= 0x08;
                }
                else
                {
                    api_ret = WIFI_ERR_SERIAL_OPEN;
                }
            }
            if (WIFI_SUCCESS == api_ret)
            {
                R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_EN_CTS_IN, NULL);
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
                WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
                WIFI_RTS_DDR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;
#endif
                vTaskDelay(1000);

                ret = wifi_execute_at_command(g_atcmd_port, "ATUART=2,1\r", g_atcmd_timeout1,
                        WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_CHANGE_TO_21, 0xff);
                if (0 != ret)
                {
                    api_ret = WIFI_ERR_MODULE_COM;
                }
            }
            if (WIFI_SUCCESS == api_ret)
            {
            	vTaskDelay(1000); /* 1 sec */

                g_use_uart_num = 2;
                g_wifi_createble_sockets = WIFI_CFG_CREATABLE_SOCKETS;
                g_atcmd_port = ULPGN_UART_SECOND_PORT;
                g_data_port = ULPGN_UART_DEFAULT_PORT;
                g_wifi_tx_busiz_command = g_wifi_serial_buffsize_table[1][0];
                g_wifi_tx_busiz_data = g_wifi_serial_buffsize_table[1][1];
                g_wifi_rx_busiz_command = g_wifi_serial_buffsize_table[1][2];
                g_wifi_rx_busiz_data = g_wifi_serial_buffsize_table[1][3];
            }
        }
        else
        {
            /* Single Connection */
            ret = wifi_serial_second_port_close();
            open_phase &= (~0x10);

            wifi_init_at_execute_queue();
            g_use_uart_num = 1;
            g_wifi_createble_sockets = 1;
            g_atcmd_port = ULPGN_UART_DEFAULT_PORT;
            g_data_port = ULPGN_UART_DEFAULT_PORT;
            g_wifi_tx_busiz_command = g_wifi_serial_buffsize_table[0][0];
            g_wifi_tx_busiz_data = g_wifi_serial_buffsize_table[0][1];
            g_wifi_rx_busiz_command = g_wifi_serial_buffsize_table[0][2];
            g_wifi_rx_busiz_data = g_wifi_serial_buffsize_table[0][3];

            /* Wifi Module hardware reset   */
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
            WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 0;
            WIFI_RESET_DDR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
#endif
            vTaskDelay(26);
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
            WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
#endif
            vTaskDelay(200);

            ret = wifi_serial_open_for_initial();
            if (0 == ret)
            {
                open_phase |= 0x10;
            }
            else
            {
                api_ret = WIFI_ERR_SERIAL_OPEN;
            }
            if (WIFI_SUCCESS == api_ret)
            {
                /* no echo */
                ret = wifi_execute_at_command(g_atcmd_port, "ATE0\r", g_atcmd_timeout1,
                        WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ECHO_OFF, 0xff);
                if (0 != ret)
                {
                    api_ret = WIFI_ERR_MODULE_COM;
                }
            }
            if (WIFI_SUCCESS == api_ret)
            {
#if (WIFI_CFG_SCI_USE_FLOW_CONTROL == 1)
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
                /* Change HSUART1 baudrate and flow control. */
                sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATBX1=%d,,,,h\r", WIFI_CFG_SCI_BAUDRATE);
#endif
#else
                sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATBX1=%d,,,,\r", WIFI_CFG_SCI_BAUDRATE);
#endif
                ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                        g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_HISPEED, 0xff);
                if (0 != ret)
                {
                    api_ret = WIFI_ERR_MODULE_COM;
                }
            }
            if (WIFI_SUCCESS == api_ret)
            {
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
            	change_baud.pclk = g_wifi_uart[g_atcmd_port].wifi_uart_sci_handle->pclk_speed;
                change_baud.rate = WIFI_CFG_SCI_BAUDRATE;
                R_SCI_Control(g_wifi_uart[g_atcmd_port].wifi_uart_sci_handle, SCI_CMD_CHANGE_BAUD, &change_baud);
                R_SCI_Control(g_wifi_uart[g_atcmd_port].wifi_uart_sci_handle, SCI_CMD_EN_CTS_IN, NULL);
                WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
                WIFI_RTS_DDR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;
#endif

                vTaskDelay(1000);
            }
        }

    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* UART transmission flow control busy retry timeout */
        ret = wifi_execute_at_command(g_atcmd_port, "ATS108=1\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_FLOW_TIMEOUT, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Escape guard time = 200msec */
        ret = wifi_execute_at_command(g_atcmd_port, "ATS12=1\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ESCAPE_GUARD_TIME, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Buffer size = 1420byte */
        ret = wifi_execute_at_command(g_atcmd_port, "ATBSIZE=1420\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_BUFFER_THRESHOLD, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Disconnect from currently connected Access Point, */
        ret = wifi_execute_at_command(g_atcmd_port, "ATWD\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
        else
        {
            g_current_socket_index = 0;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Receive timeout  */
        ret = wifi_execute_at_command(g_atcmd_port, "ATTO=1\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AT_RECV_TIMEOUT, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Receive timeout  */
    	ret = wifi_execute_at_command(g_atcmd_port, "ATS110=1\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTOCLOSE, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* Receive timeout  */
        ret = wifi_execute_at_command(g_atcmd_port, "ATS105=0\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }

    if (WIFI_SUCCESS == api_ret)
    {
        g_wifi_system_state = WIFI_SYSTEM_OPEN;
    }
    else
    {
        if (0 != (open_phase & 0x10))
        {
            wifi_serial_second_port_close();
        }
        if (0 != (open_phase & 0x08))
        {
            wifi_serial_default_port_close();
        }
        if (0 != (open_phase & 0x20))
        {
            wifi_delete_recv_task();
        }
        if (0 != (open_phase & 0x04))
        {
            /* delete WiFi RX Semaphore */
            vSemaphoreDelete(s_wifi_rx_semaphore);
            s_wifi_rx_semaphore = NULL;
        }
        if (0 != (open_phase & 0x02))
        {
            /* delete WiFi TX Semaphore */
            vSemaphoreDelete(s_wifi_tx_semaphore);
            s_wifi_tx_semaphore = NULL;
        }
        if (0 != (open_phase & 0x01))
        {
            for (k = 0; k < byteq_open_count; k++ )
            {
                R_BYTEQ_Close(g_wifi_socket[k].socket_byteq_hdl);
            }
        }
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_Open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_Close
 * Description  : WiFi Module Close.
 * Arguments    : none.
 * Return Value : WIFI_SUCCESS
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_Close(void)
{
    uint16_t i;
    wifi_err_t api_ret = WIFI_SUCCESS;

    if (0 == R_WIFI_SX_ULPGN_IsConnected())
    {
        R_WIFI_SX_ULPGN_Disconnect();
    }
    wifi_serial_close();
    wifi_delete_recv_task();
    for (i = 0; i < WIFI_CFG_CREATABLE_SOCKETS; i++ )
    {
        R_BYTEQ_Close(g_wifi_socket[i].socket_byteq_hdl);
    }
    g_wifi_system_state = WIFI_SYSTEM_CLOSE;

    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_Close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_SetDnsServerAddress
 * Description  : Set DNS Server Address.
 * Arguments    : dns_address1 - First DNS address
 *                dns_address2 - Second DNS address
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_SetDnsServerAddress(uint32_t dns_address1, uint32_t dns_address2)
{
    int32_t ret;
    wifi_err_t api_ret = WIFI_SUCCESS;
    uint8_t mutex_flag;
    uint8_t * p_cbuff = g_wifi_uart[g_atcmd_port].p_cmdbuf;

    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        wifi_change_command_mode();
        if (0 != dns_address1)
        {
            /* set dns address1 */
            sprintf((char *) p_cbuff, "ATNDNSSVR1=%d.%d.%d.%d\r",
                    WIFI_ULONG_TO_IPV4BYTE_1(dns_address1), /* addr1 */
                    WIFI_ULONG_TO_IPV4BYTE_2(dns_address1), /* addr2 */
                    WIFI_ULONG_TO_IPV4BYTE_3(dns_address1), /* addr3 */
                    WIFI_ULONG_TO_IPV4BYTE_4(dns_address1)); /* addr4 */
            ret = wifi_execute_at_command(g_atcmd_port, p_cbuff, g_atcmd_timeout1,
                    WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DNS_SRV_ADDRESS, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_MODULE_COM;
            }
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        if (0 != dns_address2)
        {
            /* set dns address2 */
            sprintf((char *) p_cbuff, "ATNDNSSVR2=%d.%d.%d.%d\r",
                    WIFI_ULONG_TO_IPV4BYTE_1(dns_address2), /* addr1 */
                    WIFI_ULONG_TO_IPV4BYTE_2(dns_address2), /* addr2 */
                    WIFI_ULONG_TO_IPV4BYTE_3(dns_address2), /* addr3 */
                    WIFI_ULONG_TO_IPV4BYTE_4(dns_address2)); /* addr4 */
            ret = wifi_execute_at_command(g_atcmd_port, p_cbuff, g_atcmd_timeout1,
                    WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DNS_SRV_ADDRESS, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_MODULE_COM;
            }
        }
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }
    return api_ret;

}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_SetDnsServerAddress
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_Connect
 * Description  : WiFi module connect to Access Point.
 * Arguments    : ssid
 *                pass
 *                security
 *                dhcp_enable
 *                ip_config
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_Connect(const uint8_t * ssid, const uint8_t * pass,
        uint32_t security, uint8_t dhcp_enable, wifi_ip_configuration_t * ip_config)
{
    int32_t ret;
    uint8_t mutex_flag;
    uint8_t security_type;
    uint8_t retry_count;
    uint8_t getip_retry_count;
    wifi_err_t api_ret = WIFI_SUCCESS;
    uint8_t * p_cbuff = g_wifi_uart[g_atcmd_port].p_cmdbuf;

    if ((NULL == ssid) || (NULL == pass) || ((WIFI_SECURITY_WPA != security) && (WIFI_SECURITY_WPA2 != security))
            || (NULL == ip_config))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }
    if (0 == R_WIFI_SX_ULPGN_IsConnected())
    {
        /* Nothing to do. */
        return WIFI_SUCCESS;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        if (0 == dhcp_enable)
        {
            /* DHCP Not Use */
            ret = wifi_execute_at_command(g_atcmd_port, "ATNDHCP=0\r", g_atcmd_timeout1,
                    WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DHCP_MODE, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_MODULE_COM;
            }
            if (WIFI_SUCCESS == api_ret)
            {
                /* AT command */
                sprintf((char *)p_cbuff, "ATNSET=%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d\r\n",
                        WIFI_ULONG_TO_IPV4BYTE_1(ip_config->ipaddress),  /* addr1 */
                        WIFI_ULONG_TO_IPV4BYTE_2(ip_config->ipaddress),  /* addr2 */
                        WIFI_ULONG_TO_IPV4BYTE_3(ip_config->ipaddress),  /* addr3 */
                        WIFI_ULONG_TO_IPV4BYTE_4(ip_config->ipaddress),  /* addr4 */
                        WIFI_ULONG_TO_IPV4BYTE_1(ip_config->subnetmask), /* addr1 */
                        WIFI_ULONG_TO_IPV4BYTE_2(ip_config->subnetmask), /* addr2 */
                        WIFI_ULONG_TO_IPV4BYTE_3(ip_config->subnetmask), /* addr3 */
                        WIFI_ULONG_TO_IPV4BYTE_4(ip_config->subnetmask), /* addr4 */
                        WIFI_ULONG_TO_IPV4BYTE_1(ip_config->gateway),    /* addr1 */
                        WIFI_ULONG_TO_IPV4BYTE_2(ip_config->gateway),    /* addr2 */
                        WIFI_ULONG_TO_IPV4BYTE_3(ip_config->gateway),    /* addr3 */
                        WIFI_ULONG_TO_IPV4BYTE_4(ip_config->gateway));   /* addr4 */

                /* Set Static IP address */
                ret = wifi_execute_at_command(g_atcmd_port, p_cbuff, g_atcmd_timeout1,
                        WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_STATIC_IP, 0xff);
                if (0 != ret)
                {
                    api_ret = WIFI_ERR_MODULE_COM;
                }
            }
        }
        else
        {
            /* DHCP Use */
            ret = wifi_execute_at_command(g_atcmd_port, "ATNDHCP=1\r", g_atcmd_timeout1,
                    WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DHCP_MODE, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_MODULE_COM;
            }
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        if (0 == R_WIFI_SX_ULPGN_IsConnected())
        {
            /* If Wifi is already connected, do nothing and return fail. */
            wifi_give_mutex(mutex_flag);
            if (0 != R_WIFI_SX_ULPGN_Disconnect())
            {
                api_ret = WIFI_ERR_MODULE_COM;
            }
            if (WIFI_SUCCESS == api_ret)
            {
                if (0 != wifi_take_mutex(mutex_flag))
                {
                    api_ret = WIFI_ERR_TAKE_MUTEX;
                }
            }
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
    	vTaskDelay(2000);

        if (WIFI_SECURITY_WPA == security)
        {
            security_type = 1;
        }
        else
        {
            security_type = 2;
        }

        for (retry_count = 0; retry_count < 3; retry_count++ )
        {
            /* AT command */
            sprintf((char *)p_cbuff, "ATWAWPA=%s,%d,1,1,%s\r", ssid, security_type, pass);
            ret = wifi_execute_at_command(g_atcmd_port, p_cbuff, g_atcmd_timeout1 * 2,
                    WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_WIFI_CONNECT, 0xff);
            if ((0 == ret) || ((-2) == ret))
            {
                if ((-2) == ret)
                {
                    memset(g_wifi_current_ssid, 0, sizeof(g_wifi_current_ssid));
                    ret = wifi_execute_at_command(g_atcmd_port, "ATW\r", g_atcmd_timeout2,
                            WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_CURRENT_SSID, 0xff);
                    if (0 == ret)
                    {
                        ret = -1;
                        if (0 == strcmp((const char *)g_wifi_current_ssid, (const char *)ssid)) /* ssid */
                        {
                            ret = 0;
                        }
                    }
                }
            }
            if (0 != ret)
            {
                wifi_execute_at_command(g_atcmd_port, "ATWD\r", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                        WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
            }
            else
            {
                break;
            }
        }
        if (retry_count >= 3)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        if (0 != dhcp_enable)
        {
            for (getip_retry_count = 0; getip_retry_count < 10; getip_retry_count++ )
            {
                ret = wifi_get_ipaddress();
                if ((0 == ret) && (0 != g_wifi_ipconfig.ipaddress))
                {
                    break;
                }
                else
                {
                    vTaskDelay(1000);
                }
            }
            if (getip_retry_count >= 10)
            {
                wifi_execute_at_command(g_atcmd_port, "ATWD\r", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                        WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
                api_ret = WIFI_ERR_MODULE_COM;
            }
        }
    }

    if (WIFI_SUCCESS == api_ret)
    {
        g_wifi_system_state = WIFI_SYSTEM_CONNECT;
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_Connect
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_Disconnect
 * Description  : WiFi Module disconnect from Access Point.
 * Arguments    : none.
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_Disconnect(void)
{
    uint8_t mutex_flag;
    wifi_err_t api_ret = WIFI_SUCCESS;

    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }
    if (WIFI_SYSTEM_OPEN == g_wifi_system_state)
    {
        /* Nothing to do. */
        return WIFI_SUCCESS;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        wifi_change_command_mode();
        wifi_execute_at_command(g_atcmd_port, "ATWD\r", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
        memset( &g_wifi_ipconfig, 0, sizeof(g_wifi_ipconfig));
        g_wifi_system_state = WIFI_SYSTEM_OPEN;
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }

    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_Disconnect
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_IsConnected
 * Description  : Check connected Status.
 * Arguments    : none.
 * Return Value : 0  - connected
 *                -1 - not connected
 *********************************************************************************************************************/
int32_t R_WIFI_SX_ULPGN_IsConnected(void)
{
    int32_t ret = -1;

    if (WIFI_SYSTEM_CONNECT == g_wifi_system_state)
    {
        ret = 0;
    }
    return ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_IsConnected
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_GetMacAddress
 * Description  : Get WiFi module MAC Address.
 * Arguments    : mac_address.
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_GetMacAddress(uint8_t * mac_address)
{
    int32_t ret;
    uint8_t mutex_flag;
    wifi_err_t api_ret = WIFI_SUCCESS;

    if (NULL == mac_address)
    {
        return WIFI_ERR_PARAMETER;
    }
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }
    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }

    if (WIFI_SUCCESS == api_ret)
    {
        ret = wifi_execute_at_command(g_atcmd_port, "ATW\r\n", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                WIFI_COMMAND_GET_MACADDRESS, 0xff);
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
        memcpy(mac_address, g_wifi_macaddress, sizeof(g_wifi_macaddress));
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_GetMacAddress
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_GetIpAddress
 * Description  : Get WiFi module IP Address.
 * Arguments    : ip_config.
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_GetIpAddress(wifi_ip_configuration_t * ip_config)
{
    int32_t ret;
    uint8_t mutex_flag;
    wifi_err_t api_ret = WIFI_SUCCESS;

    if (NULL == ip_config)
    {
        return WIFI_ERR_PARAMETER;
    }
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }

    if (WIFI_SUCCESS == api_ret)
    {
        ret = wifi_get_ipaddress();
        if (0 != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        memcpy(ip_config, &g_wifi_ipconfig, sizeof(wifi_ip_configuration_t));
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_GetIpAddress
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_Scan
 * Description  : Scan Access points.
 * Arguments    : ap_results
 *                max_networks
 *                exist_ap_list
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_Scan(wifi_scan_result_t * ap_results, uint32_t max_networks, uint32_t * exist_ap_list)
{
    int32_t i;
    int32_t ret;
    uint8_t retry_count;
    uint8_t mutex_flag;
    wifi_err_t api_ret = WIFI_SUCCESS;

    if ((NULL == ap_results) || (0 == max_networks) || (NULL == exist_ap_list))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        gp_wifi_ap_results = ap_results;
        g_wifi_aplistmax = max_networks;
        g_wifi_aplist_stored_num = 0;
        g_wifi_aplist_count = 0;
        g_wifi_aplist_subcount = 0;
        memset(gp_wifi_ap_results, 0x00, sizeof(wifi_scan_result_t) * max_networks);
        for (i = 0; i < max_networks; i++ )
        {
            gp_wifi_ap_results[i].security = WIFI_SECURITY_UNDEFINED;
        }

        for (retry_count = 0; retry_count < 5; retry_count++ )
        {
            ret = wifi_execute_at_command(g_atcmd_port, "ATWS\r", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                    WIFI_COMMAND_GET_APLIST, 0xff);
            if (0 == ret)
            {
                *exist_ap_list = g_wifi_aplist_count;
                break;
            }
            vTaskDelay(1);
        }
        if (retry_count >= 5)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }

    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_Scan
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_GetTcpSocketStatus
 * Description  : Get tcp socket status.
 * Arguments    : socket_number
 * Return Value : -1    - not exist
 *                other - socket table pointer
 *********************************************************************************************************************/
int32_t R_WIFI_SX_ULPGN_GetTcpSocketStatus(uint8_t socket_number)
{
    if (socket_number >= g_wifi_createble_sockets)
    {
        return -1;
    }
    g_wifi_socket[socket_number].socket_status = wifi_get_socket_status(socket_number);
    return g_wifi_socket[socket_number].socket_status;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_GetTcpSocketStatus
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_CreateSocket
 * Description  : Get tcp socket status.
 * Arguments    : type
 *                ip_version
 * Return Value : Positive number - created socket number
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_CREATE
 *********************************************************************************************************************/
int32_t R_WIFI_SX_ULPGN_CreateSocket(uint32_t type, uint32_t ip_version)
{
    int32_t i;
    int32_t ret = WIFI_ERR_SOCKET_CREATE;
    uint8_t mutex_flag;
    wifi_err_t api_ret = WIFI_SUCCESS;

    if ((WIFI_SOCKET_IP_PROTOCOL_TCP != type) || (WIFI_SOCKET_IP_VERSION_4 != ip_version))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    for (i = 0; i < g_wifi_createble_sockets; i++ )
    {
        if (0 == g_wifi_socket[i].socket_create_flag)
        {
            break;
        }
    }
    if (i >= g_wifi_createble_sockets)
    {
        api_ret = WIFI_ERR_SOCKET_NUM;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        g_wifi_socket[i].socket_create_flag = 1;
        g_wifi_socket[i].ipversion = (uint8_t)ip_version; /* ip_version */
        g_wifi_socket[i].protocol =  (uint8_t)type;       /* type */
        g_wifi_socket[i].socket_status = WIFI_SOCKET_STATUS_SOCKET;
        g_wifi_socket[i].ssl_flag = 0;
        g_wifi_socket[i].ssl_type = 0;
        g_wifi_socket[i].ssl_certificate_id = 0;
        g_wifi_socket[i].timeout_count = 0;
        g_wifi_socket[i].processed_data_size = 0;
        g_wifi_socket[i].start_processed_data_size = 0;
        g_wifi_socket[i].end_processed_data_size = 0;
        R_BYTEQ_Flush(g_wifi_socket[i].socket_byteq_hdl);
        ret = i;
    }
    else
    {
        ret = WIFI_ERR_SOCKET_CREATE;
    }

    /* Give back the socketInUse mutex. */
    wifi_give_mutex(mutex_flag);

    return ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_CreateSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_ConnectSocket
 * Description  : Get host by name.
 * Arguments    : socket_number
 *                ip_address
 *                port
 *                destination
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
uint32_t r_sx_ulpgn_get_host_by_name(const char * pcHostName)
{
    uint32_t ulAddr = 0;

    /* DnsQuery */
    R_WIFI_SX_ULPGN_DnsQuery((uint8_t *)pcHostName, &ulAddr);
    return ulAddr;
}
/**********************************************************************************************************************
 * End of function r_sx_ulpgn_get_host_by_name
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_ConnectSocket
 * Description  : Get host by name.
 * Arguments    : socket_number
 *                ip_address
 *                port
 *                destination
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_ConnectSocket(int32_t socket_number,
        uint32_t ip_address, uint16_t port, char * destination)
{
    int32_t ret;
    uint8_t mutex_flag;
    wifi_err_t api_ret = WIFI_SUCCESS;
    uint8_t certificate_count;

    wifi_certificate_infomation_t * p_cert_info;
    wifi_certificate_infomation_t cert_info;
    char certificate_file[32];
    char calist_file[32];
    uint8_t certificate_flg = 0;
    uint8_t calist_flg = 0;

    /* certificate information */
    p_cert_info = (wifi_certificate_infomation_t *) &cert_info;

    if ((0 == ip_address) || (0 == port))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }
    if ((socket_number >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_number < 0)
            || (WIFI_SOCKET_STATUS_SOCKET != g_wifi_socket[socket_number].socket_status))
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    if (1 == g_wifi_socket[socket_number].ssl_flag)
    {
        for (certificate_count = 0; certificate_count <
        (sizeof((st_cert_profile_t *)g_cert_profile));
                certificate_count++ )
        {
            if (ip_address == g_cert_profile[certificate_count].host_address)
            {
                g_wifi_socket[socket_number].ssl_certificate_id = certificate_count;
                break;
            }
            else if ('\0' != g_cert_profile[certificate_count].host_name[0])
            {
                if (r_sx_ulpgn_get_host_by_name(g_cert_profile[certificate_count].host_name) == ip_address)
                {
                    g_wifi_socket[socket_number].ssl_certificate_id = certificate_count;
                    break;
                }
            }
            else
            {
                ; /* Do nothing */
            }
        }

        api_ret = R_WIFI_SX_ULPGN_GetServerCertificate(p_cert_info);
    }
    if (WIFI_SUCCESS != api_ret)
    {
        return api_ret;
    }
    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
        return api_ret;
    }

    if (WIFI_SUCCESS == api_ret)
    {
        if (2 == g_use_uart_num)
        {
            /* socket index */
            ret = wifi_change_socket_index((uint8_t)socket_number);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_CHANGE_SOCKET;
            }
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        vTaskDelay(200);
        if (1 == g_wifi_socket[socket_number].ssl_flag)
        {
            /* certificate file */
            sprintf((char *)certificate_file, "cert%d.crt", g_wifi_socket[socket_number].ssl_certificate_id);

            /* calist */
            sprintf((char *)calist_file, "calist%d.crt", g_wifi_socket[socket_number].ssl_certificate_id);

            /* exist certificate file */
            while (0 != p_cert_info->certificate_file[0])
            {
                /* certificate file */
                if (0 == strcmp((char *)(p_cert_info->certificate_file), (char *)certificate_file))
                {
                    certificate_flg = 1;
                }

                /* calist file */
                if (0 == strcmp((char *)(p_cert_info->certificate_file), (char *)calist_file))
                {
                    calist_flg = 1;
                }
                if ((1 == certificate_flg) & (1 == calist_flg))
                {
                    break;
                }
                p_cert_info = p_cert_info->next_certificate_name;
            }

            /* AT command */
            sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSOCK=%d,%d\r", 2, 4);
            ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                    g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CREATE, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_SOCKET_CREATE;
            }

            /* Receive timeout  */
            ret = wifi_execute_at_command(g_atcmd_port, "ATNSSL=2,1\r", g_atcmd_timeout1,
                    WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_MODULE_COM;
                printf("ATNSSL ERROR\r\n");
            }

            if (1 == calist_flg)
            {
                /* calist */
                sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSSLLD=2,%s,2\r", (char *)calist_file);
                ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                        g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE, 0xff);
                if (0 != ret)
                {
                    api_ret = WIFI_ERR_MODULE_COM;
                    printf("ATNSSLLD_CA ERROR\r\n");
                }
            }
            if (1 == certificate_flg)
            {
                /* certificate file */
                sprintf((char *) g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSSLLD=2,%s,1\r",
                        (char *) certificate_file);
                ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                        g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE, 0xff);
                if (0 != ret)
                {
                    api_ret = WIFI_ERR_MODULE_COM;
                    printf("ATNSSLLS_CERT ERROR\r\n");
                }
            }
            if ('\0' != destination)
            {
                /* destination */
                sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSSLCFG=2,4,%s,0,1\r",
                        (char *)destination);
            }
            else
            {
                /* destination */
                sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSSLCFG=2,4,,0,1\r");
            }
            ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                    g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_MODULE_COM;
                printf("ATNSSLCFG ERROR\r\n");
            }
        }
        else
        {
            /* AT command */
            sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSOCK=%d,%d\r", 0, 4);
            ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                    g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CREATE, 0xff);
            if (0 != ret)
            {
                api_ret = WIFI_ERR_SOCKET_CREATE;
            }
        }

    }

    if (WIFI_SUCCESS == api_ret)
    {
        /* AT command */
        sprintf((char *) g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNCTCP=%d.%d.%d.%d,%d\r",
                WIFI_ULONG_TO_IPV4BYTE_1(ip_address),  /* addr1 */
                WIFI_ULONG_TO_IPV4BYTE_2(ip_address),  /* addr2 */
                WIFI_ULONG_TO_IPV4BYTE_3(ip_address),  /* addr3 */
                WIFI_ULONG_TO_IPV4BYTE_4(ip_address),  /* addr4 */
                port);
        if (2 == g_use_uart_num)
        {
            ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                    g_atcmd_timeout5, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CONNECT, socket_number);
        }
        else
        {
            ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                    g_atcmd_timeout5, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CONNECT, socket_number);
        }
        if (0 != ret)
        {
            wifi_execute_at_command(g_atcmd_port, "ATNCLOSE\r", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                    WIFI_COMMAND_SET_SOCKET_CLOSE, socket_number);
            vTaskDelay(500);
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    else
    {
        wifi_execute_at_command(g_atcmd_port, "ATNCLOSE\r", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                WIFI_COMMAND_SET_SOCKET_CLOSE, socket_number);
        vTaskDelay(500);
        api_ret = WIFI_ERR_MODULE_COM;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        g_wifi_socket[socket_number].socket_status = WIFI_SOCKET_STATUS_CONNECTED;
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }

    return api_ret;

}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_ConnectSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_SendSocket
 * Description  : Send data on connecting socket.
 * Arguments    : socket_number
 *                data
 *                length
 *                timeout_ms
 * Return Value : Positive number - number of sent data
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
int32_t R_WIFI_SX_ULPGN_SendSocket(int32_t socket_number, uint8_t * data, int32_t length, uint32_t timeout_ms)
{
    volatile int32_t timeout;
    volatile int32_t sended_length;
    int32_t current_send_length;
    int32_t ret;
    sci_err_t ercd;
    int32_t api_ret = 0;

    if ((NULL == data) || (length < 0))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    if ((socket_number >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_number < 0)
            || (0 == g_wifi_socket[socket_number].socket_create_flag)
            || (WIFI_SOCKET_STATUS_CONNECTED != g_wifi_socket[socket_number].socket_status))
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    if (0 == wifi_take_mutex(MUTEX_TX))
    {
        sended_length = 0;

        if (2 == g_use_uart_num)
        {
            if (socket_number != g_current_socket_index)
            {
                if (0 != wifi_take_mutex(MUTEX_RX))
                {
                    wifi_give_mutex(MUTEX_TX);
                    return WIFI_ERR_TAKE_MUTEX;
                }
                ret = wifi_change_socket_index((uint8_t)socket_number); /* socket number */
                wifi_give_mutex(MUTEX_RX);
                if (0 != ret)
                {
                    /* Give back the socketInUse mutex. */
                    wifi_give_mutex(MUTEX_TX);
                    return WIFI_ERR_CHANGE_SOCKET;
                }
            }
        }
        if (1 == g_use_uart_num)
        {
            if (0 == g_wifi_transparent_mode)
            {
                if (0 != wifi_take_mutex(MUTEX_RX))
                {
                    wifi_give_mutex(MUTEX_TX);
                    return WIFI_ERR_TAKE_MUTEX;
                }
                ret = wifi_change_transparent_mode();
                wifi_give_mutex(MUTEX_RX);
                if (0 != ret)
                {
                    /* Give back the socketInUse mutex. */
                    wifi_give_mutex(MUTEX_TX);
                    return WIFI_ERR_MODULE_COM;
                }
            }
        }

        if (0 != timeout_ms)
        {
            socket_timeout_init(socket_number, timeout_ms, 0);
        }

        timeout = 0;

        while (sended_length < length)
        {
            if ((length - sended_length) > g_wifi_tx_busiz_data)
            {
                current_send_length = g_wifi_tx_busiz_data;
            }
            else
            {
                current_send_length = length - sended_length;
            }

            if (current_send_length > WIFI_SOCKET_SENDABLE_DATASIZE)
            {
                current_send_length = WIFI_SOCKET_SENDABLE_DATASIZE;
            }

            g_wifi_uart[g_data_port].tx_end_flag = 0;
            ercd = R_SCI_Send(g_wifi_uart[g_data_port].wifi_uart_sci_handle,
                    (uint8_t *)data + sended_length, current_send_length); /* send */
            if (SCI_SUCCESS != ercd)
            {
                break;
            }

            while (1)
            {
                if (0 != g_wifi_uart[g_data_port].tx_end_flag)
                {
                    break;
                }
                vTaskDelay(1);
            }

            sended_length += current_send_length;
            if ((-1) == socket_check_timeout(socket_number, 0))
            {
                break;
            }
        }
        api_ret = sended_length;

        /* Give back the socketInUse mutex. */
        wifi_give_mutex(MUTEX_TX);
    }
    else
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_SendSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_ReceiveSocket
 * Description  : Receive data on connecting socket.
 * Arguments    : socket_number
 *                data
 *                length
 *                timeout_ms
 * Return Value : Positive number - number of received data
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
int32_t R_WIFI_SX_ULPGN_ReceiveSocket(int32_t socket_number, uint8_t * data, int32_t length, uint32_t timeout_ms)
{
    uint32_t recvcnt = 0;
    int32_t ret;
    byteq_err_t byteq_ret;
    int32_t api_ret = WIFI_ERR_TAKE_MUTEX;
    volatile int32_t timeout;
    uint32_t ipl;

    if ((NULL == data) || (length <= 0))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    if ((socket_number >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_number < 0)
            || (0 == g_wifi_socket[socket_number].socket_create_flag)
            || (WIFI_SOCKET_STATUS_CONNECTED != g_wifi_socket[socket_number].socket_status))
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    if (0 == wifi_take_mutex(MUTEX_RX))
    {
        if (2 == g_use_uart_num)
        {
            if (socket_number != g_current_socket_index)
            {
                if (0 != wifi_take_mutex(MUTEX_TX))
                {
                	wifi_give_mutex(MUTEX_RX);
                    return WIFI_ERR_TAKE_MUTEX;
                }
                ret = wifi_change_socket_index((uint8_t)socket_number); /* socket index */
                wifi_give_mutex(MUTEX_TX);
                if (0 != ret)
                {
                    /* Give back the socketInUse mutex. */
                    wifi_give_mutex(MUTEX_RX);
                    return WIFI_ERR_CHANGE_SOCKET;
                }
            }
        }
        if (1 == g_use_uart_num)
        {
            if (0 == g_wifi_transparent_mode)
            {
                if (0 != wifi_take_mutex(MUTEX_TX))
                {
                	wifi_give_mutex(MUTEX_RX);
                    return WIFI_ERR_TAKE_MUTEX;
                }
                ret = wifi_change_transparent_mode();
                wifi_give_mutex(MUTEX_TX);
                if (0 != ret)
                {
                    /* Give back the socketInUse mutex. */
                    wifi_give_mutex(MUTEX_RX);
                    return WIFI_ERR_MODULE_COM;

                }
            }
        }
        if ((0 != timeout_ms) && (portMAX_DELAY != timeout_ms))
        {
            socket_timeout_init(socket_number, timeout_ms, 1);
        }

        while (1)
        {
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
            ipl = R_BSP_CpuInterruptLevelRead();
            R_BSP_CpuInterruptLevelWrite(WIFI_CFG_SCI_INTERRUPT_LEVEL);
            byteq_ret = R_BYTEQ_Get(g_wifi_socket[g_current_socket_index].socket_byteq_hdl, (data + recvcnt));
            R_BSP_CpuInterruptLevelWrite(ipl);
#endif
            if (BYTEQ_SUCCESS == byteq_ret)
            {
                recvcnt++;
                if (recvcnt >= length)
                {
                    break;
                }
                continue;
            }
            if (WIFI_SOCKET_STATUS_CONNECTED != g_wifi_socket[socket_number].socket_status)
            {
                break;
            }
            if (WIFI_SYSTEM_CONNECT != g_wifi_system_state)
            {
                break;
            }
            if ((0 != timeout_ms) && (portMAX_DELAY != timeout_ms))
            {
                if ((-1) == socket_check_timeout(socket_number, 1))
                {
#if DEBUGLOG == 1
                    R_BSP_CpuInterruptLevelWrite (13);
                    R_BSP_CpuInterruptLevelWrite (0);
#endif
                    if (0 == recvcnt)
                    {
                    	g_wifi_socket[socket_number].timeout_count++;
						if (g_wifi_socket[socket_number].timeout_count >= ULPGN_CFG_SOCKET_STATUS_CHECK_FREQUENCY)
						{
							ret = wifi_get_socket_status(socket_number);
							if (ULPGN_SOCKET_STATUS_CONNECTED != ret)
							{
								wifi_give_mutex(MUTEX_RX);
								R_WIFI_SX_ULPGN_CloseSocket(socket_number);
#if DEBUGLOG == 1
								R_BSP_CpuInterruptLevelWrite (13);
								printf("Socket_%d is not connected\r\n", socket_number);
								R_BSP_CpuInterruptLevelWrite (0);
#endif
							}
							g_wifi_socket[socket_number].timeout_count = 0;
						}
                    }
                    else
                    {
                    	g_wifi_socket[socket_number].timeout_count = 0;
                    }

                    break;
                }
            }
            vTaskDelay(1);
        }

        /* socket is not closed, and recieve data size is 0. */
        /* Give back the socketInUse mutex. */
        api_ret = recvcnt;
        wifi_give_mutex(MUTEX_RX);
#if DEBUGLOG == 1
        tmptime2 = xTaskGetTickCount();
        R_BSP_CpuInterruptLevelWrite (13);
        printf("r:%06d:tcp %ld byte received.reqsize=%ld,%x\r\n", tmptime2, recvcnt, length, (uint32_t)pdata);
        R_BSP_CpuInterruptLevelWrite (0);
#endif

    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_ReceiveSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_ShutdownSocket
 * Description  : Shutdown connecting socket.
 * Arguments    : socket_number
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_ShutdownSocket(int32_t socket_number)
{
    wifi_err_t api_ret = WIFI_SUCCESS;
    int32_t subroutain_ret;

    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_CONNECT;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }
    if ((socket_number >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_number < 0) ||
            (0 == g_wifi_socket[socket_number].socket_create_flag)
            || (g_wifi_socket[socket_number].socket_status <= WIFI_SOCKET_STATUS_SOCKET))
    {
        return WIFI_ERR_SOCKET_NUM;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        if (2 == g_use_uart_num)
        {
            subroutain_ret = wifi_change_socket_index((uint8_t)socket_number); /* socket index */
            if (0 != subroutain_ret)
            {
                api_ret = WIFI_ERR_CHANGE_SOCKET;
            }
        }
        if (1 == g_use_uart_num)
        {
            wifi_change_command_mode();
        }
    }
    if (WIFI_SUCCESS == api_ret)
    {
        subroutain_ret = wifi_execute_at_command(g_atcmd_port, "ATNCLOSE\r", g_atcmd_timeout1,
                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CLOSE, socket_number);
        if (0 != subroutain_ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
        else
        {
            g_wifi_socket[socket_number].socket_status = WIFI_SOCKET_STATUS_SOCKET;
            vTaskDelay(500);
        }
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_ShutdownSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_CloseSocket
 * Description  : Disconnect connecting socket.
 * Arguments    : socket_number
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_CloseSocket(int32_t socket_number)
{
    wifi_err_t api_ret = WIFI_SUCCESS;
    uint8_t mutex_flag;

    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_CONNECT;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }
    if ((socket_number >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_number < 0))
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }

    if (WIFI_SUCCESS == api_ret)
    {
        if (1 == g_wifi_socket[socket_number].socket_create_flag)
        {
            if (g_wifi_socket[socket_number].socket_status > WIFI_SOCKET_STATUS_SOCKET)
            {
                api_ret = R_WIFI_SX_ULPGN_ShutdownSocket(socket_number);
            }
            R_BYTEQ_Flush(g_wifi_socket[socket_number].socket_byteq_hdl);
            g_wifi_socket[socket_number].ipversion = 0;
            g_wifi_socket[socket_number].protocol = 0;
            g_wifi_socket[socket_number].ssl_flag = 0;
            g_wifi_socket[socket_number].ssl_type = 0;
            g_wifi_socket[socket_number].ssl_certificate_id = 0;
            g_wifi_socket[socket_number].socket_status = WIFI_SOCKET_STATUS_CLOSED;
            g_wifi_socket[socket_number].socket_create_flag = 0;
            g_wifi_socket[socket_number].timeout_count = 0;
            g_wifi_socket[socket_number].processed_data_size = 0;
            g_wifi_socket[socket_number].start_processed_data_size = 0;
            g_wifi_socket[socket_number].end_processed_data_size = 0;
        }
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        /* Give back the socketInUse mutex. */
        wifi_give_mutex(mutex_flag);
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_CloseSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_DnsQuery
 * Description  : Execute DNS query.
 * Arguments    : domain_name
 *                ip_address
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_DnsQuery(uint8_t * domain_name, uint32_t * ip_address)
{
    int32_t func_ret;
    uint8_t mutex_flag;
    wifi_err_t api_ret = WIFI_SUCCESS;

    if ((NULL == domain_name) || (NULL == ip_address))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_CONNECT;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        g_wifi_dnsquery_subcount = 0;

        /* AT command */
        sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNDNSQUERY=%s\r", domain_name);
        func_ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                g_atcmd_timeout4, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DNSQUERY, 0xff);
        if (0 != func_ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
        else
        {
            *ip_address = g_wifi_dnsaddress;
        }
    }
    if (WIFI_ERR_TAKE_MUTEX != api_ret)
    {
        wifi_give_mutex(mutex_flag);
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_DnsQuery
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_Ping
 * Description  : Execute Ping command.
 * Arguments    : ip_address
 *                count
 *                interval_ms
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_Ping(uint32_t ip_address, uint16_t count, uint32_t interval_ms)
{
    int32_t func_ret;
    uint8_t mutex_flag;
    uint32_t i;
    wifi_err_t api_ret = WIFI_SUCCESS;
    uint32_t success_count;

    if ((0 == ip_address) || (0 == count))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_CONNECT;
    }
    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    if (WIFI_SUCCESS == api_ret)
    {
        success_count = 0;
        for (i = 0; i < count; i++ )
        {
            mutex_flag = (MUTEX_TX | MUTEX_RX);
            if (0 != wifi_take_mutex(mutex_flag))
            {
                api_ret = WIFI_ERR_TAKE_MUTEX;
            }
            if (WIFI_SUCCESS == api_ret)
            {
                /* AT command */
                sprintf((char *) g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNPING=%d.%d.%d.%d\r",
                        WIFI_ULONG_TO_IPV4BYTE_1(ip_address), /* addr1 */
                        WIFI_ULONG_TO_IPV4BYTE_2(ip_address), /* addr2 */
                        WIFI_ULONG_TO_IPV4BYTE_3(ip_address), /* addr3 */
                        WIFI_ULONG_TO_IPV4BYTE_4(ip_address)); /* addr4 */
                func_ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                        g_atcmd_timeout3, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_PING, 0xff);
                if (0 != func_ret)
                {
                }
                else
                {
                    success_count++;
                }
            }
            if (WIFI_ERR_TAKE_MUTEX != api_ret)
            {
                wifi_give_mutex(mutex_flag);
            }
            if ((i + 1) < count)
            {
                vTaskDelay(interval_ms);
            }
        }
    }
    if (0 == success_count)
    {
        if (WIFI_ERR_TAKE_MUTEX != api_ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
        }
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_Ping
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_GetVersion
 * Description  : Get FIT module version.
 * Arguments    : none.
 * Return Value : FIT module version
 *********************************************************************************************************************/
uint32_t R_WIFI_SX_ULPGN_GetVersion(void)
{
    /* These version macros are defined in r_wifi_sx_ulpgn_if.h. */
    return ((((uint32_t) WIFI_SX_ULPGN_CFG_VERSION_MAJOR) << 16) | (uint32_t) WIFI_SX_ULPGN_CFG_VERSION_MINOR);
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_GetVersion
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_get_ipaddress
 * Description  : Get IP address.
 * Arguments    : none.
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_get_ipaddress(void)
{
    int32_t func_ret;

    func_ret = wifi_execute_at_command(g_atcmd_port, "ATNSET=\?\r",
            g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_IPADDRESS, 0xff);
    if (0 != func_ret)
    {
        return -1;
    }
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_get_ipaddress
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_get_socket_status
 * Description  : Get socket status.
 * Arguments    : none.
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_get_socket_status(uint8_t socket_number)
{
    int32_t ret;

    ret = wifi_execute_at_command(g_atcmd_port, "ATNSTAT\r",
            g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
            WIFI_COMMAND_GET_SOCKET_STATUS, socket_number);
    if (0 != ret)
    {
        return -1;
    }

    if (g_wifi_socket_status < ULPGN_SOCKET_STATUS_MAX)
    {
        return g_wifi_socket_status;
    }
    else
    {
        return -1;
    }
}
/**********************************************************************************************************************
 * End of function wifi_get_ipaddress
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_change_transparent_mode
 * Description  : change to transparent mode.
 * Arguments    : none.
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_change_transparent_mode(void)
{
    int32_t ret;

    if ((1 == g_use_uart_num) && (0 == g_wifi_transparent_mode))
    {
        ret = wifi_execute_at_command(g_atcmd_port, "ATO\r",
                g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                WIFI_COMMAND_SET_TRANSPARENT_MODE, 0xff);
        if (0 != ret)
        {
            return -1;
        }
        else
        {
            g_wifi_transparent_mode = 1;
        }
    }
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_change_transparent_mode
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_change_command_mode
 * Description  : change to command mode.
 * Arguments    : none.
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_change_command_mode(void)
{
    int32_t ret;
    if ((1 == g_use_uart_num) && (1 == g_wifi_transparent_mode))
    {
    	vTaskDelay(202);
        g_wifi_transparent_mode = 0;
        ret = wifi_execute_at_command(g_atcmd_port, "+++",
                g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                WIFI_COMMAND_SET_COMMAND_MODE, 0xff);
        if (0 == ret)
        {
        	vTaskDelay(202);
        }
        else
        {
            return -1;
        }
    }
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_change_command_mode
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_change_socket_index
 * Description  : change socket index.
 * Arguments    : socket_number
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_change_socket_index(uint8_t socket_number)
{
    uint8_t sequence = 0;
    int32_t ret = 0;
    uint32_t atustat_recv;

    if (2 == g_use_uart_num)
    {
        if (socket_number != g_current_socket_index)
        {
            g_before_socket_index = g_current_socket_index;

            /* AT command */
            sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSOCKINDEX=%d\r", socket_number);
            while (sequence < 0x80)
            {
                switch (sequence)
                {
                    case 0:
                    	/* RTS_ON */
                    	WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;
                        vTaskDelay(ULPGN_CFG_SOCKET_CHANGE_BEFORE_WAIT);

                        if (0 != wifi_check_uart_state( &atustat_recv, &g_wifi_socket[g_before_socket_index].end_processed_data_size))
                        {
                            ret = -1;
                            sequence = 0x80;
                            break;
                        }
                        /* Send ATNSOCKINDEX command. */
                        ret = wifi_execute_at_command(g_atcmd_port,
                                g_wifi_uart[g_atcmd_port].p_cmdbuf,
                                g_atcmd_timeout1,
                                WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CHANGE,
                                socket_number);

                        /* Parse command return code. */
                        switch (ret)
                        {
                            case 0:/* change socket index to next. */
                            	g_current_socket_index = socket_number;
                            	g_wifi_socket[g_current_socket_index].start_processed_data_size = g_wifi_socket[g_before_socket_index].end_processed_data_size;
                            	g_wifi_socket[g_current_socket_index].processed_data_size = 0;
                            	/* RTS_OFF */
                            	WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
                                sequence = 0x80;
                                break;

                            case -1: /* Communication with module failed. */
                            	sequence = 0x80;
                            	break;

                            case -2: /* BUSY */
                                /* If dont change socket because of wifi module busy,
                                 * retry change socket command. */
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
                                WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
#endif
                                /* Wait for module to complete send data. */
                                vTaskDelay(g_wifi_uart[g_data_port].socket_change_delay_time);
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
                                WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;
#endif
                                sequence = 0;
                                break;

                            default:/* This is command error and recovery socket status. */
                                ret = -1;
                                sequence = 0x80;
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return ret;
}
/**********************************************************************************************************************
 * End of function wifi_change_socket_index
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_check_uart_state
 * Description  : check uart state.
 * Arguments    : uart_receive_status
 *                uart_send_status
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_check_uart_state(uint32_t * uart_receive_status, uint32_t * uart_send_status)
{
#if DEBUGLOG == 1
    TickType_t tmptime1, tmptime2;
#endif

    uint8_t retry_count;
    int32_t ret;

    for (retry_count = 0; retry_count < 10; retry_count++ )
    {
        ret = wifi_execute_at_command(g_atcmd_port, "ATUSTAT\r", g_atcmd_timeout1, WIFI_RETURN_ENUM_OK,
                WIFI_COMMAND_GET_SENT_RECV_SIZE, 0xff);
        if (0 == ret)
        {
            *uart_receive_status = g_wifi_atustat_recv;
            *uart_send_status = g_wifi_atustat_sent;
            return 0;
        }
        if ((-1) == ret)
        {
            return -1;
        }
        if ((-2) == ret)
        {
            continue;
        }
    }
    return -1;
}
/**********************************************************************************************************************
 * End of function wifi_check_uart_state
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_serial_open_for_initial
 * Description  : Open serial port for initial.
 * Arguments    : none.
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_serial_open_for_initial(void)
{
    sci_err_t my_sci_err;

#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
    R_SCI_PINSET_FUNC_DEFAULT();
#endif

    memset(&g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, 0, sizeof(sci_hdl_t));
    g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.baud_rate = WIFI_UART_BAUDRATE_DEFAULT;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.clk_src = SCI_CLK_INT;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.data_size = SCI_DATA_8BIT;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.parity_en = SCI_PARITY_OFF;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.parity_type = SCI_EVEN_PARITY;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.stop_bits = SCI_STOPBITS_1;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.int_priority = WIFI_CFG_SCI_INTERRUPT_LEVEL;

    my_sci_err = R_SCI_Open(SCI_CH_WIFI_DEFAULT, SCI_MODE_ASYNC,
            &g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config,
            wifi_uart_callback_default_port_for_inititial,
            &g_wifi_uart[WIFI_UART_COMMAND_PORT].wifi_uart_sci_handle);
    if (SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    return 0;

}
/**********************************************************************************************************************
 * End of function wifi_serial_open_for_initial
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_serial_open_for_data
 * Description  : Open serial port for data.
 * Arguments    : none.
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_serial_open_for_data(void)
{
    sci_err_t my_sci_err;
    uint8_t level;

#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
    R_SCI_PINSET_FUNC_DEFAULT();
#endif

    memset(&g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, 0, sizeof(sci_hdl_t));
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.baud_rate = WIFI_CFG_SCI_BAUDRATE;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.clk_src = SCI_CLK_INT;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.data_size = SCI_DATA_8BIT;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.parity_en = SCI_PARITY_OFF;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.parity_type = SCI_EVEN_PARITY;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.stop_bits = SCI_STOPBITS_1;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.int_priority = WIFI_CFG_SCI_INTERRUPT_LEVEL;

    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].socket_change_delay_time = ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE
    																* ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD / (WIFI_CFG_SCI_BAUDRATE / 8);

    my_sci_err = R_SCI_Open(SCI_CH_WIFI_DEFAULT, SCI_MODE_ASYNC,
            &g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config,
            wifi_uart_callback_default_port_for_data,
            &g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle);

    if (SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
    level = g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.int_priority - 1;
    my_sci_err = R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle,
            SCI_CMD_SET_TXI_PRIORITY, &level);
    if (SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }
#endif

    return 0;

}
/**********************************************************************************************************************
 * End of function wifi_serial_open_for_data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_serial_second_port_open
 * Description  : Open serial port for secondary.
 * Arguments    : none.
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_serial_second_port_open(void)
{
    sci_err_t my_sci_err;

#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
    R_SCI_PINSET_FUNC_SECOND();
#endif

    memset(&g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle, 0, sizeof(sci_hdl_t));
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.baud_rate = 115200;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.clk_src = SCI_CLK_INT;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.data_size = SCI_DATA_8BIT;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.parity_en = SCI_PARITY_OFF;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.parity_type = SCI_EVEN_PARITY;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.stop_bits = SCI_STOPBITS_1;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.int_priority = WIFI_CFG_SCI_INTERRUPT_LEVEL;

    my_sci_err = R_SCI_Open(SCI_CH_WIFI_SECOND, SCI_MODE_ASYNC,
            &g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config,
            wifi_uart_callback_second_port_for_command,
            &g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle);

    if (SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_serial_second_port_open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_serial_close
 * Description  : Close serial port.
 * Arguments    : none.
 * Return Value : 0  - success
 *********************************************************************************************************************/
static int32_t wifi_serial_close(void)
{
    wifi_serial_default_port_close();
    if (2 == g_use_uart_num)
    {
        wifi_serial_second_port_close();
    }
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_serial_close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_serial_default_port_close
 * Description  : Close serial default port.
 * Arguments    : none.
 * Return Value : 0  - success
 *********************************************************************************************************************/
static int32_t wifi_serial_default_port_close(void)
{
    R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_RX_Q_FLUSH, NULL);
    R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_TX_Q_FLUSH, NULL);
    R_SCI_Close(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle);
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_serial_default_port_close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_serial_second_port_close
 * Description  : Close serial default port.
 * Arguments    : none.
 * Return Value : 0  - success
 *********************************************************************************************************************/
static int32_t wifi_serial_second_port_close(void)
{
    R_SCI_Control(g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle, SCI_CMD_RX_Q_FLUSH, NULL);
    R_SCI_Control(g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle, SCI_CMD_TX_Q_FLUSH, NULL);
    R_SCI_Close(g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle);
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_serial_second_port_close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_uart_callback_default_port_for_inititial
 * Description  : SCI callback function of serial default port.
 * Arguments    : pArgs
 * Return Value : none.
 *********************************************************************************************************************/
static void wifi_uart_callback_default_port_for_inititial(void * pArgs)
{
    sci_cb_args_t * p_args;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* callback event */
    p_args = (sci_cb_args_t*) pArgs;
    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        R_BSP_NOP();
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
        g_wifi_uart[ULPGN_UART_DEFAULT_PORT].tx_end_flag = 1;
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
         You will need to increase buffer size or reduce baud rate */
        g_wifi_sci_err_flag = 1;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
        g_wifi_sci_err_flag = 2;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
        g_wifi_sci_err_flag = 3;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
    }
    else
    {
        /* Do nothing */
    }
    vTaskNotifyGiveFromISR(g_wifi_recv_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); /* RTOS ISR */

}
/**********************************************************************************************************************
 * End of function wifi_uart_callback_default_port_for_inititial
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_uart_callback_second_port_for_command
 * Description  : SCI callback function of serial secondary port.
 * Arguments    : pArgs
 * Return Value : none.
 *********************************************************************************************************************/
static void wifi_uart_callback_second_port_for_command(void * pArgs)
{
    sci_cb_args_t * p_args;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* callback event */
    p_args = (sci_cb_args_t*) pArgs;
    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        R_BSP_NOP();
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
        g_wifi_uart[ULPGN_UART_SECOND_PORT].tx_end_flag = 1;
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
         You will need to increase buffer size or reduce baud rate */
        g_wifi_sci_err_flag = 1;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
        g_wifi_sci_err_flag = 2;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
        g_wifi_sci_err_flag = 3;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
    }
    else
    {
        /* Do nothing */
    }
    vTaskNotifyGiveFromISR(g_wifi_recv_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); /* RTOS ISR */

}
/**********************************************************************************************************************
 * End of function wifi_uart_callback_second_port_for_command
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_uart_callback_default_port_for_data
 * Description  : SCI callback function of serial data port.
 * Arguments    : pArgs
 * Return Value : none.
 *********************************************************************************************************************/
static void wifi_uart_callback_default_port_for_data(void * pArgs)
{
    sci_cb_args_t * p_args;
    byteq_err_t byteq_ret;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* callback event */
    p_args = (sci_cb_args_t*) pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_RX_Q_FLUSH, NULL);

    	if ((g_wifi_socket[g_before_socket_index].socket_create_flag == 1)
    			&& (0 != g_wifi_socket[g_before_socket_index].end_processed_data_size
        					- g_wifi_socket[g_before_socket_index].start_processed_data_size
								- g_wifi_socket[g_before_socket_index].processed_data_size))
    	{
    		byteq_ret = R_BYTEQ_Put(g_wifi_socket[g_before_socket_index].socket_byteq_hdl, p_args->byte);
            if (BYTEQ_SUCCESS != byteq_ret)
            {
                g_wifi_socket[g_before_socket_index].put_error_count++;
            }
            g_wifi_socket[g_before_socket_index].processed_data_size++;
    	}
    	else if (g_wifi_socket[g_current_socket_index].socket_create_flag == 1)
    	{
    		byteq_ret = R_BYTEQ_Put(g_wifi_socket[g_current_socket_index].socket_byteq_hdl, p_args->byte);
            if (BYTEQ_SUCCESS != byteq_ret)
            {
                g_wifi_socket[g_current_socket_index].put_error_count++;
            }
            g_wifi_socket[g_current_socket_index].processed_data_size++;
    	}
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
        g_wifi_uart[ULPGN_UART_DEFAULT_PORT].tx_end_flag = 1;
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
         You will need to increase buffer size or reduce baud rate */
        g_wifi_sci_err_flag = 1;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
        g_wifi_sci_err_flag = 2;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
        g_wifi_sci_err_flag = 3;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
         Error condition is cleared in calling interrupt routine */
    }
    else
    {
        /* Do nothing */
    }
    vTaskNotifyGiveFromISR(g_wifi_recv_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); /* RTOS ISR */

}
/**********************************************************************************************************************
 * End of function wifi_uart_callback_default_port_for_data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_execute_at_command
 * Description  : Execute AT command.
 * Arguments    : serial_ch_id
 *                p_cmdstr
 *                timeout_ms
 *                expect_code
 *                command
 *                socket_number
 * Return Value : ATCMD_OK      - success
 *                ATCMD_TIMEOUT - timeout
 *                ATCMD_BUSY    - busy
 *********************************************************************************************************************/
static int32_t wifi_execute_at_command(uint8_t serial_ch_id, const uint8_t * ptextstring,
        uint16_t timeout_ms, wifi_return_code_t expect_code, wifi_command_list_t command, int32_t socket_number)
{
    volatile int32_t timeout;
    sci_err_t ercd;
    int8_t get_queue;
    uint32_t recvcnt = 0;
    int32_t ret;
    wifi_return_code_t result;
    uint32_t ticket_no;


    timeout_init(serial_ch_id, timeout_ms);

    if (NULL != ptextstring)
    {
        timeout = 0;
        recvcnt = 0;
#if DEBUGLOG == 2
        R_BSP_CpuInterruptLevelWrite (WIFI_CFG_SCI_INTERRUPT_LEVEL-1);
        printf("%s\r\n",ptextstring);
        R_BSP_CpuInterruptLevelWrite (0);
#endif

        ticket_no = wifi_set_request_in_queue(command, socket_number);
        g_wifi_uart[serial_ch_id].tx_end_flag = 0;
#if defined(__CCRX__) || defined(__ICCRX__) || defined (__RX__)
        /* AT command */
        ercd = R_SCI_Send(g_wifi_uart[serial_ch_id].wifi_uart_sci_handle, (uint8_t *)ptextstring,
                strlen((const char *)ptextstring)); /* textstring size */
#endif
        if (SCI_SUCCESS != ercd)
        {
            return -1;

        }

        while (1)
        {
            if (0 != g_wifi_uart[serial_ch_id].tx_end_flag)
            {
                break;
            }

            /* check timeout */
            if ((-1) == check_timeout((uint32_t) serial_ch_id, recvcnt))
            {
                wifi_set_result_to_current_running_queue(WIFI_RETURN_ENUM_INTERNAL_TIMEOUT);
                timeout = 1;
                break;
            }
            vTaskDelay(1);
        }
        if (1 == timeout)
        {
            return -1;
        }
    }
    else
    {
        ticket_no = wifi_set_request_in_queue(command, socket_number);
    }
    while (1)
    {
        get_queue = wifi_get_result_from_queue(ticket_no, &result);
        if (0 == get_queue)
        {
            break;
        }

        /* check timeout */
        if ((-1) == check_timeout((uint32_t)serial_ch_id, recvcnt))
        {
            wifi_set_result_to_current_running_queue(WIFI_RETURN_ENUM_INTERNAL_TIMEOUT);
            timeout = 1;
            break;
        }
        vTaskDelay(1);
    }
    if (1 == timeout)
    {
        return -1;
    }

    ret = -1;
    if (result == expect_code)
    {
        ret = 0;
    }
    else if (WIFI_RETURN_ENUM_BUSY == result)
    {
        ret = -2;
    }
    else
    {
        ; /* Do nothing */
    }
    return ret;
}
/**********************************************************************************************************************
 * End of function wifi_execute_at_command
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: timeout_init
 * Description  : initialize timeout counter.
 * Arguments    : serial_ch
 *                timeout_ms
 * Return Value : none.
 *********************************************************************************************************************/
static void timeout_init(int32_t serial_ch, uint16_t timeout_ms)
{
    st_atcmd_info_t * p_uart;
    p_uart = &g_wifi_uart[serial_ch];

    p_uart->starttime = xTaskGetTickCount();
    p_uart->endtime = p_uart->starttime + timeout_ms;
    if (p_uart->endtime < p_uart->starttime)
    {
        /* endtime value is overflow */
        p_uart->timeout_overflow_flag = 1;
    }
    else
    {
        p_uart->timeout_overflow_flag = 0;
    }
}
/**********************************************************************************************************************
 * End of function timeout_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: check_timeout
 * Description  : Check timeout occurred.
 * Arguments    : serial_ch
 *                rcvcount
 * Return Value : 0  - timeout not occurred
 *                -1 - timeout occurred
 *********************************************************************************************************************/
static int32_t check_timeout(int32_t serial_ch, int32_t rcvcount)
{
    st_atcmd_info_t * p_uart;
    p_uart = &g_wifi_uart[serial_ch];

    if (0 == rcvcount)
    {
        p_uart->thistime = xTaskGetTickCount();
        if (0 == p_uart->timeout_overflow_flag)
        {
            if ((p_uart->thistime >= p_uart->endtime) || (p_uart->thistime < p_uart->starttime))
            {
                /* Timeout  */
                return -1;
            }
        }
        else
        {
            if ((p_uart->thistime < p_uart->starttime) && (p_uart->thistime >= p_uart->endtime))
            {
                /* Timeout  */
                return -1;
            }
        }
    }

    /* Not timeout  */
    return 0;
}
/**********************************************************************************************************************
 * End of function check_timeout
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: socket_timeout_init
 * Description  : .
 * Arguments    : socket_number
 *                timeout_ms
 *                flag
 * Return Value : none.
 *********************************************************************************************************************/
static void socket_timeout_init(uint8_t socket_number, uint32_t timeout_ms, uint8_t flag)
{
    TickType_t * p_starttime;
    TickType_t * p_endtime;
    uint8_t * p_timeout_overflow_flag;

    wifi_socket_t * p_socket;
    p_socket = &g_wifi_socket[socket_number];

    if (0 == flag)
    {
        p_starttime = &p_socket->send_starttime;
        p_endtime = &p_socket->send_endtime;
        p_timeout_overflow_flag = &p_socket->send_timeout_overflow_flag;
    }
    else
    {
        p_starttime = &p_socket->recv_starttime;
        p_endtime = &p_socket->recv_endtime;
        p_timeout_overflow_flag = &p_socket->recv_timeout_overflow_flag;
    }
    *p_starttime = xTaskGetTickCount();
    *p_endtime = (*p_starttime) + timeout_ms;
    if ((*p_endtime) < (*p_starttime))
    {
        /* overflow */
        *p_timeout_overflow_flag = 1;
    }
    else
    {
        *p_timeout_overflow_flag = 0;
    }
}
/**********************************************************************************************************************
 * End of function socket_timeout_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: socket_check_timeout
 * Description  : Check timeout occurred on connecting socket.
 * Arguments    : socket_number
 *                flag
 * Return Value : 0  - timeout not occurred
 *                -1 - timeout occurred
 *********************************************************************************************************************/
static int32_t socket_check_timeout(uint8_t socket_number, uint8_t flag)
{
    TickType_t * p_starttime;
    TickType_t * p_thistime;
    TickType_t * p_endtime;
    uint8_t * p_timeout_overflow_flag;
    wifi_socket_t * p_socket;

    p_socket = &g_wifi_socket[socket_number];
    if (0 == flag)
    {
        p_starttime = &p_socket->send_starttime;
        p_thistime = &p_socket->send_thistime;
        p_endtime = &p_socket->send_endtime;
        p_timeout_overflow_flag = &p_socket->send_timeout_overflow_flag;
    }
    else
    {
        p_starttime = &p_socket->recv_starttime;
        p_thistime = &p_socket->recv_thistime;
        p_endtime = &p_socket->recv_endtime;
        p_timeout_overflow_flag = &p_socket->recv_timeout_overflow_flag;
    }

    *p_thistime = xTaskGetTickCount();
    if (0 == (*p_timeout_overflow_flag))
    {
        if (((*p_thistime) >= (*p_endtime)) || ((*p_thistime) < (*p_starttime)))
        {
            return -1;
        }
    }
    else
    {
        if (((*p_thistime) < (*p_starttime)) && ((*p_thistime) <= (*p_endtime)))
        {
            return -1;
        }
    }

    /* Not timeout  */
    return 0;
}
/**********************************************************************************************************************
 * End of function socket_check_timeout
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_take_mutex
 * Description  : Take mutex for WiFi module.
 * Arguments    : mutex_flag
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static int32_t wifi_take_mutex(uint8_t mutex_flag)
{
    if (0 != (mutex_flag & MUTEX_TX))
    {
        if (pdTRUE != xSemaphoreTake(s_wifi_tx_semaphore, s_xMaxSemaphoreBlockTime))
        {
            return -1;
        }
    }

    if (0 != (mutex_flag & MUTEX_RX))
    {
        if (pdTRUE != xSemaphoreTake(s_wifi_rx_semaphore, s_xMaxSemaphoreBlockTime))
        {
            if (0 != (mutex_flag & MUTEX_TX))
            {
                /* tx semaphore */
                xSemaphoreGive(s_wifi_tx_semaphore);
            }
            return -1;
        }
    }
#if DEBUGLOG ==2
    printf("Semaphore Get\n\n");
#endif
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_take_mutex
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_give_mutex
 * Description  : Give mutex for WiFi module.
 * Arguments    : mutex_flag
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
static void wifi_give_mutex(uint8_t mutex_flag)
{
    if (0 != (mutex_flag & MUTEX_RX))
    {
        /* rx semaphore */
        xSemaphoreGive(s_wifi_rx_semaphore);
        vTaskDelay(1);
    }
    if (0 != (mutex_flag & MUTEX_TX))
    {
        /* tx semaphore */
        xSemaphoreGive(s_wifi_tx_semaphore);
        vTaskDelay(1);
    }
#if DEBUGLOG ==2
    printf("Semaphore Give\r\n");
#endif
    return;
}
/**********************************************************************************************************************
 * End of function wifi_give_mutex
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_RegistServerCertificate
 * Description  : Register server certificate on WiFi module.
 * Arguments    : data_id
 *                datatype
 *                pdata
 *                length
 *                timeout_ms
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_RegistServerCertificate(uint32_t data_id, uint32_t datatype,
        const uint8_t * pdata, int32_t length, uint32_t timeout_ms)
{
    volatile int32_t timeout;
    volatile int32_t sended_length;
    int32_t current_send_length;
    wifi_err_t api_ret = WIFI_SUCCESS;
    sci_err_t ercd;
    int8_t get_queue;
    wifi_return_code_t result;
    uint8_t mutex_flag;
    uint32_t ticket_no;

    if ((NULL == pdata) || (length < 0))
    {
        return WIFI_ERR_PARAMETER;
    }
    if (datatype >= 2)
    {
        return WIFI_ERR_PARAMETER;
    }

    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }

    mutex_flag = MUTEX_TX | MUTEX_RX;
    if (0 == wifi_take_mutex(mutex_flag))
    {
        sended_length = 0;

        switch (datatype)
        {
            case 0x00: /* RootCA */
                sprintf((char *) g_wifi_uart[g_atcmd_port].p_cmdbuf,
                        "ATNSSLCERT=calist%d.crt,%d\r", data_id, length);
                break;
            case 0x01: /* Certificate & Key */
                sprintf((char *) g_wifi_uart[g_atcmd_port].p_cmdbuf,
                        "ATNSSLCERT=cert%d.crt,%d\r", data_id, length);
                break;
            default:
                break;
        }
        wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                g_atcmd_timeout2, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SYSFALSH_WRITE_DATA, 0xff);

        timeout_init(g_atcmd_port, 2000);
        ticket_no = wifi_set_request_in_queue(WIFI_COMMAND_SET_SYSFALSH_WRITE_DATA, 0xff);
        g_wifi_uart[g_atcmd_port].tx_end_flag = 0;

        while (sended_length < length)
        {
            if ((length - sended_length) > g_wifi_tx_busiz_command)
            {
                current_send_length = g_wifi_tx_busiz_command;
            }
            else
            {
                current_send_length = length - sended_length;
            }
            timeout = 0;

            /* data send */
            ercd = R_SCI_Send(g_wifi_uart[g_atcmd_port].wifi_uart_sci_handle, (uint8_t*) pdata + sended_length,
                    current_send_length);
            if (SCI_SUCCESS != ercd)
            {
                break;
            }

            while (1)
            {
                if (0 != g_wifi_uart[g_atcmd_port].tx_end_flag)
                {
                    break;
                }
                vTaskDelay(1);
            }
            sended_length += current_send_length;
        }
        while (1)
        {
            get_queue = wifi_get_result_from_queue(ticket_no, &result);
            if (0 == get_queue)
            {
                break;
            }

            if ((-1) == check_timeout(g_atcmd_port, 0))
            {
                timeout = 1;
                break;
            }
            vTaskDelay(1);
        }
        if (1 == timeout)
        {
            wifi_give_mutex(mutex_flag);
            return WIFI_ERR_MODULE_COM;
        }
        if (WIFI_RETURN_ENUM_OK != result)
        {
            wifi_give_mutex(mutex_flag);
            return WIFI_ERR_MODULE_COM;
        }

        /* Give back the socketInUse mutex. */
        wifi_give_mutex(mutex_flag);
    }
    else
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_RegistServerCertificate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_RequestTlsSocket
 * Description  : Request TLS socket communication.
 * Arguments    : socket_number
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_RequestTlsSocket(int32_t socket_number)
{
    wifi_err_t api_ret = WIFI_SUCCESS;

    if (0 != R_WIFI_SX_ULPGN_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    if ((socket_number >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_number < 0) ||
            (WIFI_SOCKET_STATUS_SOCKET != g_wifi_socket[socket_number].socket_status) ||
            (WIFI_SOCKET_IP_PROTOCOL_TCP != g_wifi_socket[socket_number].protocol))
    {
        api_ret = WIFI_ERR_SOCKET_NUM;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        api_ret = wifi_setsslconfiguration(socket_number, 0);
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_RequestTlsSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_setsslconfiguration
 * Description  : Set TLS socket configuration.
 * Arguments    : socket_number
 *                ssl_type
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_SOCKET_NUM
 *********************************************************************************************************************/
static wifi_err_t wifi_setsslconfiguration(int32_t socket_number, uint8_t ssl_type)
{
    wifi_err_t api_ret = WIFI_SUCCESS;

    if ((socket_number >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_number < 0) || (ssl_type > 3) ||
            (WIFI_SOCKET_STATUS_SOCKET != g_wifi_socket[socket_number].socket_status) ||
            (WIFI_SOCKET_IP_PROTOCOL_TCP != g_wifi_socket[socket_number].protocol))
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    g_wifi_socket[socket_number].ssl_flag = 1;
    g_wifi_socket[socket_number].ssl_type = ssl_type;

    return api_ret;
}
/**********************************************************************************************************************
 * End of function wifi_setsslconfiguration
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: erase_certificate
 * Description  : Erase certificate on WiFi module.
 * Arguments    : certificate_name
 * Return Value : 0  - success
 *                -1 - timeout
 *                -2 - busy
 *********************************************************************************************************************/
static uint32_t erase_certificate(uint8_t * certificate_name)
{
    int32_t ret;

    /* erase */
    sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSSLCERT=%s,0\r", (char *)certificate_name);
    ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf, 1000,
            WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SYSFALSH_ERASE_DATA, 0xff);

    return ret;
}
/**********************************************************************************************************************
 * End of function erase_certificate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_EraseServerCertificate
 * Description  : Erase server certificate on WiFi module.
 * Arguments    : certificate_name
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_EraseServerCertificate(uint8_t * certificate_name)
{
    wifi_err_t api_ret = WIFI_SUCCESS;
    int32_t ret;
    uint8_t mutex_flag;
    uint8_t certificate_flg = 0;
    wifi_certificate_infomation_t * p_cert_info;
    wifi_certificate_infomation_t cert_info;
    p_cert_info = (wifi_certificate_infomation_t *) &cert_info; /* cert info */

    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }

    if (0 == certificate_name)
    {
        return WIFI_ERR_PARAMETER;
    }

    ret = R_WIFI_SX_ULPGN_GetServerCertificate(p_cert_info);
    if (0 != ret)
    {
        return (wifi_err_t)ret; /* cast */
    }

    mutex_flag = MUTEX_TX | MUTEX_RX;
    if (0 == wifi_take_mutex(mutex_flag))
    {
        /* Exist certificate file */
        while (0 != p_cert_info->certificate_file[0])
        {
            /* certificate name Comparison */
            if (0 == strcmp((char *)(p_cert_info->certificate_file), (char *)certificate_name))
            {
                certificate_flg = 1;
                break;
            }
            p_cert_info = p_cert_info->next_certificate_name;
        }

        if (0 == certificate_flg)
        {
            wifi_give_mutex(mutex_flag);
            return WIFI_ERR_PARAMETER;
        }

        /* erase certificate */
        ret = (wifi_err_t)erase_certificate(certificate_name);
        if (0 != ret)
        {
            api_ret = WIRI_ERR_FLASH_ERASE;
            wifi_give_mutex(mutex_flag);
            return api_ret;
        }

        /* Give back the socketInUse mutex. */
        wifi_give_mutex(mutex_flag);
    }
    else
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_EraseServerCertificate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_GetServerCertificate
 * Description  : Get stored server certificate on WiFi module.
 * Arguments    : wifi_certificate_information
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_GetServerCertificate(wifi_certificate_infomation_t * wifi_certificate_information)
{
    wifi_err_t api_ret = WIFI_SUCCESS;
    int32_t ret;
    uint8_t mutex_flag;
    wifi_certificate_infomation_t * p_cert_info;

    if (NULL == wifi_certificate_information)
    {
        return WIFI_ERR_PARAMETER;
    }

    p_cert_info = g_wifi_certificate_information;

    /* Clear Current Certificate Information */
    while (0 != p_cert_info->certificate_file[0])
    {
        memset(&p_cert_info->certificate_file, 0, sizeof(p_cert_info->certificate_file));
        p_cert_info = p_cert_info->next_certificate_name;
    }
    g_wifi_certificate_information[0].certificate_number = 0;

    api_ret = WIFI_SUCCESS;
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        memcpy(wifi_certificate_information, &g_wifi_certificate_information[0], sizeof(wifi_certificate_infomation_t));
        return WIFI_ERR_NOT_OPEN;
    }

    mutex_flag = MUTEX_TX | MUTEX_RX;
    if (0 != wifi_take_mutex(mutex_flag))
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
        return api_ret;
    }
    if (WIFI_SUCCESS == api_ret)
    {
        /* AT command */
        sprintf((char *)g_wifi_uart[g_atcmd_port].p_cmdbuf, "ATNSSLCERT=?\r");
        ret = wifi_execute_at_command(g_atcmd_port, g_wifi_uart[g_atcmd_port].p_cmdbuf,
                g_atcmd_timeout1, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SYSFALSH_READ_DATA, 0xff);

        if (WIFI_SUCCESS != ret)
        {
            api_ret = WIFI_ERR_MODULE_COM;
#if DEBUGLOG == 2
            printf("COMMAND DOES NOT SUCCESS\r\n");
#endif
        }
        else if (1 != g_certificate_list_flg)
        {
            api_ret = WIRI_ERR_FLASH_READ;
#if DEBUGLOG == 2
            printf("Get the Certificate information was Fail\r\n");
#endif
        }
        else
        {
            api_ret = WIFI_SUCCESS;
        }

        /* Give back the socketInUse mutex. */
        wifi_give_mutex(mutex_flag);
    }
    else
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    memcpy(wifi_certificate_information, &g_wifi_certificate_information[0], sizeof(wifi_certificate_infomation_t));
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_GetServerCertificate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_WriteServerCertificate
 * Description  : Write server certificate on WiFi module.
 * Arguments    : data_id
 *                data_type
 *                certificate
 *                certificate_length
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_WriteServerCertificate(uint32_t data_id, uint32_t data_type,
        const uint8_t * certificate, uint32_t certificate_length)
{
    wifi_err_t api_ret;
    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }
    if ((NULL == certificate) || (certificate_length <= 0))
    {
        return WIFI_ERR_PARAMETER;
    }
    api_ret = R_WIFI_SX_ULPGN_RegistServerCertificate(data_id, data_type, certificate, certificate_length, 0);
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_WriteServerCertificate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_EraseAllServerCertificate
 * Description  : Erase all stored server certificate on WiFi module.
 * Arguments    : none
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_EraseAllServerCertificate(void)
{
    wifi_err_t api_ret;
    uint8_t retry_no;
    wifi_certificate_infomation_t * p_cert_info;
    wifi_certificate_infomation_t cert_info;
    p_cert_info = (wifi_certificate_infomation_t*) &cert_info; /* cert info */

    if (WIFI_SYSTEM_CLOSE == g_wifi_system_state)
    {
        return WIFI_ERR_NOT_OPEN;
    }
    api_ret = R_WIFI_SX_ULPGN_GetServerCertificate(p_cert_info);
    if (0 != api_ret)
    {
        return api_ret;
    }
    if (0 == p_cert_info->certificate_number)
    {
        /* Certificate File is nothing */
        return WIFI_SUCCESS;
    }
    while (0 != p_cert_info->certificate_file[0])
    {
        /* erase certificate */
        api_ret = (wifi_err_t)erase_certificate(p_cert_info->certificate_file);
        if (WIFI_SUCCESS != api_ret)
        {
            for (retry_no = 0; retry_no < 3; retry_no++ )
            {
                /* retry */
                api_ret = (wifi_err_t)erase_certificate(p_cert_info->certificate_file);
                if (WIFI_SUCCESS == api_ret)
                {
                    break;
                }
            }
            if (WIFI_SUCCESS != api_ret)
            {
                return WIRI_ERR_FLASH_ERASE;
            }
        }
        p_cert_info = p_cert_info->next_certificate_name;
    }
    return WIFI_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_EraseAllServerCertificate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_WIFI_SX_ULPGN_SetCertificateProfile
 * Description  : Associate server information to certificate.
 * Arguments    : certificate_id
 *                ip_address
 *                server_name
 * Return Value : WIFI_SUCCESS
 *********************************************************************************************************************/
wifi_err_t R_WIFI_SX_ULPGN_SetCertificateProfile(uint8_t certificate_id, uint32_t ip_address, char * server_name)
{
    if ('\0' == server_name)
    {
        memset(g_cert_profile[certificate_id].host_name, 0, sizeof(g_cert_profile[certificate_id].host_name));
    }
    else
    {
        strcpy(g_cert_profile[certificate_id].host_name, server_name);
    }
    g_cert_profile[certificate_id].host_address = ip_address;
    g_cert_profile[certificate_id].cert_id = certificate_id;
    return WIFI_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_WIFI_SX_ULPGN_SetCertificateProfile
 *********************************************************************************************************************/
