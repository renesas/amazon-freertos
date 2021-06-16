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
 * File Name    : r_wifi_sx_ulpgn_private.h
 * Version      : 1.0
 * Description  : Private functions definition for SX ULPGN of RX65N.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2021 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_wifi_sx_ulpgn_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef R_WIFI_SX_ULPGN_PRIVATE_H
#define R_WIFI_SX_ULPGN_PRIVATE_H

/* Configuration */
#define WIFI_NUMBER_OF_USE_UART         (2)
#define WIFI_UART_COMMAND_PORT          (0)
#define WIFI_UART_DATA_PORT             (1)

#define WIFI_SOCKET_IP_PROTOCOL_TCP     (6)
#define WIFI_SOCKET_IP_VERSION_4        (4)

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
/* Reset port pin macros.  */
#define WIFI_RESET_DDR(x, y)               (WIFI_RESET_DDR_PREPROC(x, y))
#define WIFI_RESET_DDR_PREPROC(x, y)       ((PORT ## x .PDR.BIT.B ## y))
#define WIFI_RESET_DR(x, y)                (WIFI_RESET_DR_PREPROC(x, y))
#define WIFI_RESET_DR_PREPROC(x, y)        ((PORT ## x .PODR.BIT.B ## y))

/* RTS port pin macros.  */
#define WIFI_RTS_DDR(x, y)                 (WIFI_RTS_DDR_PREPROC(x, y))
#define WIFI_RTS_DDR_PREPROC(x, y)         ((PORT ## x .PDR.BIT.B ## y))
#define WIFI_RTS_DR(x, y)                  (WIFI_RTS_DR_PREPROC(x, y))
#define WIFI_RTS_DR_PREPROC(x, y)          ((PORT ## x .PODR.BIT.B ## y))
#endif

/* IP address(xxx.xxx.xxx.xxx) into ULONG */
#define WIFI_IPV4BYTE_TO_ULONG(addr1, addr2, addr3, addr4) \
    ((((addr1) & 0x000000FF) << 24) | (((addr2) & 0x000000FF) << 16) |\
    (((addr3) & 0x000000FF) << 8)  | (((addr4) & 0x000000FF)))

/* ULONG into IP address(xxx.xxx.xxx.xxx) */
#define WIFI_ULONG_TO_IPV4BYTE_1(ul)       (((ul) >> 24) & 0x000000FF)   /* IP address byte1 */
#define WIFI_ULONG_TO_IPV4BYTE_2(ul)       (((ul) >> 16) & 0x000000FF)   /* IP address byte2 */
#define WIFI_ULONG_TO_IPV4BYTE_3(ul)       (((ul) >> 8)  & 0x000000FF)   /* IP address byte3 */
#define WIFI_ULONG_TO_IPV4BYTE_4(ul)       ((ul)         & 0x000000FF)   /* IP address byte4 */

#define WIFI_RETURN_TEXT_OK               "OK\r\n"
#define WIFI_RETURN_TEXT_ERROR            "ERROR\r\n"
#define WIFI_RETURN_TEXT_READY            "ready\r\n"
#define WIFI_RETURN_TEXT_OK_GO_SEND       "OK\r\n> "
#define WIFI_RETURN_TEXT_SEND_BYTE        " bytes\r\n"
#define WIFI_RETURN_TEXT_SEND_OK          "SEND OK\r\n"
#define WIFI_RETURN_TEXT_SEND_FAIL        "SEND FAIL\r\n"
#define WIFI_RETURN_TEXT_CONNECT          "CONNECT\r\n"
#define WIFI_RETURN_TEXT_BUSY             "BUSY\r\n"
#define WIFI_RETURN_TEXT_NOCARRIER        "NO CARRIER\r\n"
#define WIFI_RETURN_TEXT_NOANSWER         "NO ANSWER\r\n"

#define ULPGN_RETURN_TEXT_OK              "OK\r\n"
#define ULPGN_RETURN_TEXT_CONNECT         "CONNECT\r\n"
#define ULPGN_RETURN_TEXT_RING            "RING\r\n"
#define ULPGN_RETURN_TEXT_NO_CARRIER      "NO_CARRIER\r\n"
#define ULPGN_RETURN_TEXT_ERROR           "ERROR\r\n"
#define ULPGN_RETURN_TEXT_NO_DIALTONE     "NO_DIALTONE\r\n"
#define ULPGN_RETURN_TEXT_BUSY            "BUSY\r\n"
#define ULPGN_RETURN_TEXT_NO_ANSWER       "NO_ANSWER\r\n"
#define ULPGN_RETURN_TEXT_LENGTH      (13+1) /* strlen(ULPGN_RETURN_TEXT_NO_DIALTONE)+1 */
#define ULPGN_SOCKET_STATUS_TEXT_CLOSED      "CLOSED"
#define ULPGN_SOCKET_STATUS_TEXT_SOCKET      "SOCKET"
#define ULPGN_SOCKET_STATUS_TEXT_BOUND       "BOUND"
#define ULPGN_SOCKET_STATUS_TEXT_LISTEN      "LISTEN"
#define ULPGN_SOCKET_STATUS_TEXT_CONNECTED   "CONNECTED"
#define ULPGN_SOCKET_STATUS_TEXT_BROKEN		 "BROKEN"

#define WIFI_AT_COMMAND_BUFF_SIZE         (512)
#define WIFI_AT_RESPONSE_BUFF_SIZE        (512)
#define WIFI_SOCKET_SENDABLE_DATASIZE     (1420)
#define WIFI_UART_BAUDRATE_DEFAULT        (115200)

/* Timeout Define for at command */
#define EXECUTE_COMMAND_TIMEOUT_DEFAULT1  (3000)
#define EXECUTE_COMMAND_TIMEOUT_DEFAULT2  (500)
#define EXECUTE_COMMAND_TIMEOUT_DEFAULT3  (2000)
#define EXECUTE_COMMAND_TIMEOUT_DEFAULT4  (2000)
#define EXECUTE_COMMAND_TIMEOUT_DEFAULT5  (4000)

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#define WIFI_TASK_PRIORITY (tskIDLE_PRIORITY + 5)  /* WiFi task priority */
#endif

/* Debug print mode */
#define DEBUGLOG (0)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef enum
{
    WIFI_RETURN_ENUM_OK = 0,
    WIFI_RETURN_ENUM_ERROR,
    WIFI_RETURN_ENUM_READY,
    WIFI_RETURN_ENUM_CONNECT,
    WIFI_RETURN_ENUM_BUSY,
    WIFI_RETURN_ENUM_NOCARRIER,
    WIFI_RETURN_ENUM_PROCESSING,
    WIFI_RETURN_ENUM_INTERNAL_TIMEOUT,
    WIFI_RETURN_ENUM_MAX,
} wifi_return_code_t;

typedef enum
{
    WIFI_SYSTEM_CLOSE=0,
    WIFI_SYSTEM_OPEN,
    WIFI_SYSTEM_CONNECT,
} wifi_system_status_t;

typedef enum
{
    WIFI_SOCKET_STATUS_CLOSED=0,    //CLOSED
    WIFI_SOCKET_STATUS_SOCKET,      //SOCKET
    WIFI_SOCKET_STATUS_BOUND,       //BOUND
    WIFI_SOCKET_STATUS_LISTEN,      //LISTEN
    WIFI_SOCKET_STATUS_CONNECTED,   //CONNECTED
    WIFI_SOCKET_STATUS_MAX,         //MAX
} wifi_socket_status_t;

typedef enum
{
    WIFI_COMMAND_NONE = 0,
    WIFI_COMMAND_SET_REBOOT,
    WIFI_COMMAND_SET_ECHO_OFF,
    WIFI_COMMAND_SET_UART_CHANGE_TO_2,
    WIFI_COMMAND_SET_UART_CHANGE_TO_21,
    WIFI_COMMAND_SET_UART_HISPEED,
    WIFI_COMMAND_SET_UART_FLOW_TIMEOUT,
    WIFI_COMMAND_SET_ESCAPE_GUARD_TIME,
    WIFI_COMMAND_SET_BUFFER_THRESHOLD,
    WIFI_COMMAND_SET_WIFI_DISCONNECT,
    WIFI_COMMAND_SET_AT_RECV_TIMEOUT,
    WIFI_COMMAND_SET_AUTOCLOSE,
    WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE,
    WIFI_COMMAND_SET_DNS_SRV_ADDRESS,
    WIFI_COMMAND_SET_STATIC_IP,
    WIFI_COMMAND_SET_WIFI_AUTOCONNECT,
    WIFI_COMMAND_SET_WIFI_ACT_MODE,
    WIFI_COMMAND_SET_DHCP_MODE,
    WIFI_COMMAND_SET_MULTIPLE_SOCKET,
    WIFI_COMMAND_SET_WIFI_CONNECT,
    WIFI_COMMAND_SET_DNSQUERY,
    WIFI_COMMAND_SET_PING,
    WIFI_COMMAND_SET_SSLCONFIG,
    WIFI_COMMAND_SET_SOCKET_CREATE,
    WIFI_COMMAND_SET_SOCKET_CONNECT,
    WIFI_COMMAND_SET_SOCKET_SEND_START,
    WIFI_COMMAND_SET_SOCKET_SEND_DATA,
    WIFI_COMMAND_SET_SOCKET_CLOSE,
    WIFI_COMMAND_SET_SOCKET_CHANGE,
    WIFI_COMMAND_SET_TRANSPARENT_MODE,
    WIFI_COMMAND_SET_COMMAND_MODE,
    WIFI_COMMAND_GET_SOCKET_STATUS,
    WIFI_COMMAND_GET_MODULE_VERSION,
    WIFI_COMMAND_GET_UART_BAUDRATE,
    WIFI_COMMAND_GET_APLIST,
    WIFI_COMMAND_GET_MACADDRESS,
    WIFI_COMMAND_GET_IPADDRESS,
    WIFI_COMMAND_GET_SENT_RECV_SIZE,
    WIFI_COMMAND_GET_CURRENT_SSID,
    WIFI_COMMAND_SET_SYSFALSH_WRITE_DATA,
    WIFI_COMMAND_SET_SYSFALSH_READ_DATA,
    WIFI_COMMAND_SET_SYSFALSH_ERASE_DATA,
    WIFI_COMMAND_LIST_MAX
} wifi_command_list_t;

typedef enum
{
    WIFI_RETURN_STRING_TEXT = 0,
    WIFI_RETURN_STRING_MAX,
} wifi_return_string_t;

typedef struct wifi_socket_tag
{
    uint8_t socket_create_flag;
    uint8_t socket_status;
    uint8_t ipversion;
    uint8_t protocol;
    uint32_t receive_num;
    uint32_t receive_count;
    uint32_t put_error_count;
    uint32_t extracted_data_size;
    uint32_t total_data_size;
    uint32_t processed_data_size;
    uint32_t start_processed_data_size;
    uint32_t end_processed_data_size;
    uint8_t socket_recv_buff[WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE];
    byteq_hdl_t socket_byteq_hdl;
    TickType_t send_starttime;
    TickType_t send_thistime;
    TickType_t send_endtime;
    TickType_t recv_starttime;
    TickType_t recv_thistime;
    TickType_t recv_endtime;
    uint8_t timeout_count;
    uint8_t send_timeout_overflow_flag;
    uint8_t recv_timeout_overflow_flag;
    uint8_t ssl_flag;
    uint8_t ssl_type;
    uint8_t ssl_certificate_id;
} wifi_socket_t;

typedef struct
{
    sci_hdl_t   wifi_uart_sci_handle;
    sci_cfg_t   sci_config;
    uint8_t     * p_cmdbuf;
    uint32_t    cmdbuf_size;
    uint8_t     * p_respbuf;
    uint32_t    respbuf_size;
    TickType_t  starttime;
    TickType_t  thistime;
    TickType_t  endtime;
    TickType_t  startbytetime;
    TickType_t  thisbytetime;
    TickType_t  endbytetime;
    volatile uint8_t     tx_end_flag;
    uint8_t     timeout_overflow_flag;
    uint8_t     byte_timeout_overflow_flag;
    uint32_t	socket_change_delay_time;
} st_atcmd_info_t;

typedef struct
{
    wifi_command_list_t     at_command_id;
    int32_t                 socket_number;
    wifi_return_code_t      result;
    uint32_t                ticket_no;
} st_at_exec_queue_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern st_atcmd_info_t g_wifi_uart[WIFI_NUMBER_OF_USE_UART];

extern wifi_system_status_t g_wifi_system_state;
extern uint8_t g_wifi_atcmd_buf[WIFI_AT_COMMAND_BUFF_SIZE];
extern uint8_t g_wifi_resp_buf[WIFI_AT_RESPONSE_BUFF_SIZE];

extern wifi_socket_t g_wifi_socket[WIFI_CFG_CREATABLE_SOCKETS];

extern uint8_t g_wifi_macaddress[6];
extern wifi_ip_configuration_t g_wifi_ipconfig;
extern uint32_t g_wifi_dnsaddress;
extern uint32_t g_wifi_dnsquery_subcount;


extern wifi_scan_result_t * gp_wifi_ap_results;
extern uint32_t g_wifi_aplistmax;
extern uint32_t g_wifi_aplist_stored_num;
extern uint32_t g_wifi_aplist_count;
extern uint32_t g_wifi_aplist_subcount;

extern uint8_t g_wifi_current_ssid[33];

extern uint32_t g_wifi_atustat_recv;
extern uint32_t g_wifi_atustat_sent;

extern st_at_exec_queue_t g_wifi_at_execute_queue[10];
extern uint8_t g_wifi_set_queue_index;
extern uint8_t g_wifi_get_queue_index;

extern uint32_t g_wifi_sci_err_flag;

extern uint8_t g_wifi_socket_status;
extern uint8_t g_wifi_transparent_mode;

extern const uint8_t * const gp_wifi_socket_status_tbl[];

extern wifi_certificate_infomation_t g_wifi_certificate_information[10];
extern uint8_t g_certificate_list_flg;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_init_at_execute_queue
 * Description  :
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void wifi_init_at_execute_queue (void);

/**********************************************************************************************************************
 * Function Name: wifi_set_request_in_queue
 * Description  :
 * Arguments    : command
 *                socket
 * Return Value :
 *********************************************************************************************************************/
uint32_t wifi_set_request_in_queue (wifi_command_list_t command, int32_t socket);

/**********************************************************************************************************************
 * Function Name: wifi_get_current_running_queue
 * Description  :
 * Arguments    : none
 * Return Value :
 *********************************************************************************************************************/
st_at_exec_queue_t * wifi_get_current_running_queue (void);

/**********************************************************************************************************************
 * Function Name: wifi_set_result_to_current_running_queue
 * Description  :
 * Arguments    : result
 * Return Value :
 *********************************************************************************************************************/
void wifi_set_result_to_current_running_queue (wifi_return_code_t result);

/**********************************************************************************************************************
 * Function Name: wifi_get_result_from_queue
 * Description  :
 * Arguments    : result
 * Return Value :
 *********************************************************************************************************************/
int8_t wifi_get_result_from_queue (uint32_t ticket_no, wifi_return_code_t * result);

/**********************************************************************************************************************
 * Function Name: wifi_start_recv_task
 * Description  :
 * Arguments    : none
 * Return Value :
 *********************************************************************************************************************/
int32_t wifi_start_recv_task (void);

/**********************************************************************************************************************
 * Function Name: wifi_delete_recv_task
 * Description  :
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void wifi_delete_recv_task (void);

#endif /* R_WIFI_SX_ULPGN_PRIVATE_H */
