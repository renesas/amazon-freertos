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
 * File Name    : esp8266_driver.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "Task.h"
#include "FreeRTOSIPConfig.h"
#include "semphr.h"
#include "r_typedefs.h"
#include "r_byteq_if.h"
#include "esp8266_driver.h"
#include "r_scifa_drv_api.h"
#include "r_compiler_abstraction_api.h"
#include "compiler_settings.h"

#define R_BSP_CpuInterruptLevelWrite(x)

#define SCI_TX_BUSIZ_DEFAULT        (1460)

const uint8_t g_esp8266_return_text_at[]          = ESP8266_RETURN_TEXT_AT;
const uint8_t g_esp8266_return_text_ok[]          = ESP8266_RETURN_TEXT_OK;
const uint8_t g_esp8266_return_text_error[]       = ESP8266_RETURN_TEXT_ERROR;
const uint8_t g_esp8266_return_text_ready[]       = ESP8266_RETURN_TEXT_READY;
const uint8_t g_esp8266_return_text_ok_go_send[]  = ESP8266_RETURN_TEXT_OK_GO_SEND;
const uint8_t g_esp8266_return_text_send_byte[]   = ESP8266_RETURN_TEXT_SEND_BYTE;
const uint8_t g_esp8266_return_text_send_ok[]     = ESP8266_RETURN_TEXT_SEND_OK;
const uint8_t g_esp8266_return_text_send_fail[]   = ESP8266_RETURN_TEXT_SEND_FAIL;

const uint8_t g_esp8266_socket_status_closed[]       = ESP8266_SOCKET_STATUS_TEXT_CLOSED;
const uint8_t g_esp8266_socket_status_socket[]       = ESP8266_SOCKET_STATUS_TEXT_SOCKET;
const uint8_t g_esp8266_socket_status_bound[]        = ESP8266_SOCKET_STATUS_TEXT_BOUND;
const uint8_t g_esp8266_socket_status_listen[]       = ESP8266_SOCKET_STATUS_TEXT_LISTEN;
const uint8_t g_esp8266_socket_status_connected[]    = ESP8266_SOCKET_STATUS_TEXT_CONNECTED;

const uint8_t g_esp8266_return_dummy[]   = "";

const uint8_t * const gp_esp8266_result_code[ESP8266_RETURN_ENUM_MAX][ESP8266_RETURN_STRING_MAX] =
{
    /* text mode*/                  /* numeric mode */
    {g_esp8266_return_text_ok,},
    {g_esp8266_return_text_error,},
    {g_esp8266_return_text_ready,},
    {g_esp8266_return_text_ok_go_send,},
    {g_esp8266_return_text_send_byte,},
    {g_esp8266_return_text_send_ok,},
    {g_esp8266_return_text_send_fail,},
    {g_esp8266_return_text_at,},
};

const uint8_t * const gp_esp8266_socket_status[ESP8266_SOCKET_STATUS_MAX] =
{
    g_esp8266_socket_status_closed,
    g_esp8266_socket_status_socket,
    g_esp8266_socket_status_bound,
    g_esp8266_socket_status_listen,
    g_esp8266_socket_status_connected,
};

int_t g_esp8266_handle;
uint8_t g_buff[1000];
uint8_t g_recvbuff[2048+20];
volatile uint32_t g_esp8266_uart_teiflag[2];


uint8_t g_sx_esp8266_return_mode;

extern uint8_t g_dnsaddress[4];
extern esp8266_socket_t g_esp8266_socket[5];

static uint8_t g_timeout_overflow_flag[2];
static TickType_t g_starttime[2];
static TickType_t g_thistime[2];
static TickType_t g_endtime[2];
static TickType_t g_startbytetime[2];
static TickType_t g_thisbytetime[2];
static TickType_t g_endbytetime[2];
static uint8_t g_byte_timeout_overflow_flag[2];

static void timeout_init(uint8_t socket_no, uint16_t timeout_ms);
static void bytetimeout_init(uint8_t socket_no, uint16_t timeout_ms);
static int32_t check_timeout(uint8_t socket_no, int32_t rcvcount);
static int32_t check_bytetimeout(uint8_t socket_no, int32_t rcvcount);
static int32_t esp8266_serial_open(uint32_t baudrate);
static int32_t esp8266_serial_close(void);
static int32_t esp8266_serial_send_basic(   uint8_t serial_ch_id, 
                                            uint8_t *ptextstring, 
                                            uint16_t response_type, 
                                            uint16_t timeout_ms, 
                                            sx_esp8266_return_code_t expect_code);
static int32_t esp8266_serial_send_rst( uint8_t serial_ch_id, 
                                        uint8_t *ptextstring, 
                                        uint16_t response_type, 
                                        uint16_t timeout_ms, 
                                        sx_esp8266_return_code_t expect_code, 
                                        sx_esp8266_return_code_t end_string);
static int32_t esp8266_serial_send_with_recvtask(   uint8_t serial_ch_id, 
                                                    uint8_t *ptextstring, 
                                                    uint16_t response_type, 
                                                    uint16_t timeout_ms, sx_esp8266_return_code_t expect_code,
                                                    uint8_t command,
                                                    uint8_t socket_no);
static TickType_t g_sl_esp8266_tcp_recv_timeout = 3000;     /* ## slowly problem ## unit: 1ms */

/**
 * @brief The global mutex to ensure that only one operation is accessing the
 * g_esp8266_semaphore flag at one time.
 */
static SemaphoreHandle_t g_esp8266_semaphore = NULL;

/**
 * @brief Maximum time in ticks to wait for obtaining a semaphore.
 */
static const TickType_t xMaxSemaphoreBlockTime = pdMS_TO_TICKS( 60000UL );


int32_t esp8266_wifi_init(void)
{
    int32_t ret;
#if ESP8266_PORT_DEBUG == 1
    DEBUG_PORT4_DDR = 1;
    DEBUG_PORT4_DR = 0;
    DEBUG_PORT7_DDR = 1;
    DEBUG_PORT7_DR = 0;
#endif

    /* Wait system waking up */
    while(false == R_OS_Running())
    {
        R_OS_TaskSleep(1);
    }

    /* Wifi Module hardware reset   */
    ret = esp8266_serial_open(76800);
    if(ret != 0)
    {
        return ret;
    }

    /* Cast to an appropriate type */
    ESP8266_RESET_PORT_DDR = 3;
    /* Cast to an appropriate type */
    ESP8266_RESET_PORT_DR = 0; /* Low */
    R_SoftwareDelay(6760000);
    /* Cast to an appropriate type */
    ESP8266_RESET_PORT_DR = 1; /* High */
    R_SoftwareDelay(260000000);

    /* Cast to an appropriate type */
    ret = esp8266_serial_send_rst(  ESP8266_UART_COMMAND_PORT, NULL, 
                                    1000, 
                                    20000, 
                                    ESP8266_RETURN_OK, 
                                    ESP8266_RETURN_READY);

    esp8266_serial_close();
    ret = esp8266_serial_open(115200);
    if(ret != 0)
    {
        return ret;
    }

    ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT\r\n", 1000, 20000, ESP8266_RETURN_AT);
    /* Cast to an appropriate type */
    ESP8266_RESET_PORT_DR = 0; /* Low */
    R_SoftwareDelay(6760000);
    /* Cast to an appropriate type */
    ESP8266_RESET_PORT_DR = 1; /* High */
    R_SoftwareDelay(260000000);

    ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT\r\n", 1000, 20000, ESP8266_RETURN_OK);

    /* reboots the system */
    ret = esp8266_serial_send_rst(  ESP8266_UART_COMMAND_PORT, "AT+RST\r\n", 
                                    1000, 
                                    20000, 
                                    ESP8266_RETURN_OK, 
                                    ESP8266_RETURN_READY);
    if(ret != 0)
    {
        ret = esp8266_serial_send_rst(  ESP8266_UART_COMMAND_PORT, "AT+RST\r\n", 
                                        1000, 
                                        20000, 
                                        ESP8266_RETURN_OK, 
                                        ESP8266_RETURN_READY);
        if(ret != 0)
        {
            return ret;
        }
    }
    g_sx_esp8266_return_mode = 0;
    R_SoftwareDelay(260000000); //190625 modified for RZ/A2M. 1000ms delay.

    /* no echo */
    ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "ATE0\r\n", 3, 1000, ESP8266_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    ret = esp8266_serial_send_basic(0, "AT+GMR\r\n", 3, 200, ESP8266_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }
#if (0)
    ret = esp8266_serial_send_basic(0, "AT+UART_CUR=115200,8,1,0,0\r\n", 3, 200, ESP8266_RETURN_OK);
#else
    ret = esp8266_serial_send_basic(0, "AT+UART_CUR=460800,8,1,0,0\r\n", 3, 200, ESP8266_RETURN_OK);
    esp8266_serial_close();
    ret = esp8266_serial_open(460800);
#endif
    ret = esp8266_serial_send_basic(0, "AT+UART_CUR?\r\n", 3, 200, ESP8266_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    /* Disconnect from currently connected Access Point, */
    ret = esp8266_serial_send_basic(0, "AT+CWQAP\r\n", 3, 200, ESP8266_RETURN_OK);

    /* SoftAP station mode is enabled. */
    ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CWMODE=3\r\n", 3, 200, ESP8266_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }



    ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPMUX=1\r\n", 3, 200, ESP8266_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    return 0;
}

int32_t esp8266_wifi_connect(uint8_t *pssid, uint32_t security, uint8_t *ppass)
{
    int32_t ret;
    char *pstr;
    char pstr2;
    int32_t line_len;
    int32_t scanf_ret;
    volatile char secu[3][10];
    uint32_t security_encrypt_type = 1;

    /* cast to  char * */
    strcpy((char *)g_buff,"AT+CWJAP=\"");
    /* cast to  char * */
    strcat((char *)g_buff,(const char *)pssid);
    /* cast to  char * */
    strcat((char *)g_buff,"\",\"");
    /* cast to  char * */
    strcat((char *)g_buff,(const char *)ppass);
    /* cast to  char * */
    strcat((char *)g_buff,"\"\r\n");

    ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, g_buff, 10000, 20000, ESP8266_RETURN_OK);
    if(0 == ret)
    {
        uint32_t addr;
        ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPAPMAC?\r\n", 3, 5000, ESP8266_RETURN_OK);
        ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPSTA?\r\n", 3, 5000, ESP8266_RETURN_OK);

        ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPSNTPCFG=1,0\r\n", 3, 200, ESP8266_RETURN_OK);
        vstart_esp8266_recv_task();
        vTaskDelay( 2 );
        ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, "AT+CIPSNTPTIME?\r\n",
                                                3, 
                                                200, 
                                                ESP8266_RETURN_OK, 
                                                ESP8266_GET_CIPSNTPTIME, 
                                                0xff);
    }
    return ret;
}

int32_t esp8266_wifi_get_macaddr(uint8_t *ptextstring)
{
    return esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPAPMAC?\r\n", 3, 200, ESP8266_RETURN_OK);
}

int32_t esp8266_socket_create(uint8_t socket_no, uint32_t type,uint32_t ipversion)
{
    int32_t ret;

    /* Cast to an appropriate type */
    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {

    #if DEBUGLOG == 1
        R_BSP_CpuInterruptLevelWrite (14);
        printf("esp8266_socket_create(%d)\r\n",socket_no);
        R_BSP_CpuInterruptLevelWrite (0);
    #endif
        if( g_esp8266_socket[socket_no].socket_create_flag == 1)
        {
            /* Cast to an appropriate type */
            ( void ) xSemaphoreGive( g_esp8266_semaphore );
            return -1;
        }

        g_esp8266_socket[socket_no].socket_create_flag = 1;
        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( g_esp8266_semaphore );
    }
    else
    {
        return -1;
    }

    return 0;
}


int32_t esp8266_tcp_connect(uint8_t socket_no, uint32_t ipaddr, uint16_t port)
{
    int32_t ret;
    /* Cast to an appropriate type */
    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if( g_esp8266_socket[socket_no].socket_create_flag == 0)
        {
            /* Give back the socketInUse mutex. */
            ( void ) xSemaphoreGive( g_esp8266_semaphore );
            return -1;
        }

    #if DEBUGLOG == 1
        R_BSP_CpuInterruptLevelWrite (14);
        printf("esp8266_tcp_connect(%d)\r\n",socket_no);
        R_BSP_CpuInterruptLevelWrite (0);
    #endif
        /* cast to  char * */
        strcpy((char *)g_buff,"AT+CIPSTART=");
        /* cast to  char * */
        sprintf((char *)g_buff+strlen((char *)g_buff),
                "%d,\"TCP\",\"%d.%d.%d.%d\",%d\r\n",
                socket_no, 
                /* separate the value */
                (uint8_t)(ipaddr>>24), (uint8_t)(ipaddr>>16),(uint8_t)(ipaddr>>8),(uint8_t)(ipaddr),port);

        ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, 
                                                g_buff, 
                                                3, 
                                                10000, 
                                                ESP8266_RETURN_OK, 
                                                ESP8266_SET_CIPSTART, 
                                                socket_no);

        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( g_esp8266_semaphore );

    }
    else
    {
        return -1;
    }
    return ret;

}

int32_t esp8266_tcp_send(uint8_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
    int32_t timeout;
    volatile int32_t sended_length;
    int32_t lenghttmp1;
    int32_t ret;
    int32_t ercd;
    uint8_t result;

    /* Cast to an appropriate type */
    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if(0 == g_esp8266_socket[socket_no].socket_create_flag)
        {
            /* Give back the socketInUse mutex. */
            ( void ) xSemaphoreGive( g_esp8266_semaphore );
            return -1;
        }
    #if DEBUGLOG == 1
        R_BSP_CpuInterruptLevelWrite (14);
        printf("esp8266_tcp_send(%d),len=%d\r\n",socket_no,length);
        R_BSP_CpuInterruptLevelWrite (0);
    #endif
        sended_length = 0;
        lenghttmp1 = SCI_TX_BUSIZ_DEFAULT;
        while(sended_length < length)
        {
            if((length - sended_length) > 2048)
            {
                lenghttmp1 = 2048;
            }
            else
            {
                lenghttmp1 = (length - sended_length);
            }

            /* cast to  char * */
            strcpy((char *)g_buff,"AT+CIPSEND=");
            /* cast to  char * */
            sprintf((char *)g_buff+strlen((char *)g_buff),"%d,%d\r\n",socket_no,lenghttmp1);
            ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, g_buff, 3, 10000, ESP8266_RETURN_OK_GO_SEND, ESP8266_SET_CIPSEND, socket_no);

            if(ret != 0)
            {
#if DEBUGLOG == 1
            R_BSP_CpuInterruptLevelWrite (14);
            printf("esp8266_serial_send_with_recvtask.error\r\n");
            R_BSP_CpuInterruptLevelWrite (0);
#endif
                /* Give back the socketInUse mutex. */
                ( void ) xSemaphoreGive( g_esp8266_semaphore );
                R_COMPILER_Nop();
                return -1;
            }

            timeout_init(0, timeout_ms);
            timeout = 0;

            esp8266_response_set_queue( ESP8266_SET_CIPSEND_END, socket_no );
            g_esp8266_uart_teiflag[ESP8266_UART_COMMAND_PORT] = 0;
            ercd = write(g_esp8266_handle, pdata+sended_length, lenghttmp1);
            if(ercd != lenghttmp1)
            {
#if DEBUGLOG == 1
            R_BSP_CpuInterruptLevelWrite (14);
            printf("R_SCI_Send\r\n");
            R_BSP_CpuInterruptLevelWrite (0);
#endif
                /* Give back the socketInUse mutex. */
                ( void ) xSemaphoreGive( g_esp8266_semaphore );
                return -1;
            }

            while(1)
            {
                if(ercd == lenghttmp1)
                {
                    break;
                }
            }
            timeout_init(0, timeout_ms);

            while(1)
            {
                ercd = esp8266_response_get_queue( ESP8266_SET_CIPSEND_END, socket_no, &result);
                if(0 == ercd)
                {
                    break;
                }
            }

            if(result != ESP8266_RETURN_SEND_OK)
            {
#if DEBUGLOG == 1
                R_BSP_CpuInterruptLevelWrite (14);
                printf("esp8266_response_get_queue error\r\n");
                R_BSP_CpuInterruptLevelWrite (0);
#endif
                /* Give back the socketInUse mutex. */
                ( void ) xSemaphoreGive( g_esp8266_semaphore );
                R_COMPILER_Nop();
                return -1;
            }

            sended_length += lenghttmp1;

        }

    #if DEBUGLOG == 1
        R_BSP_CpuInterruptLevelWrite (14);
        printf("tcp %d byte send\r\n",sended_length);
        R_BSP_CpuInterruptLevelWrite (0);
    #endif


        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( g_esp8266_semaphore );
    }
    else
    {
#if DEBUGLOG == 1
        R_BSP_CpuInterruptLevelWrite (14);
        printf("xSemaphoreTake error\r\n");
        R_BSP_CpuInterruptLevelWrite (0);
#endif
        return -1;
    }
    return sended_length;
}

int32_t esp8266_tcp_recv(uint8_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
    int32_t ercd;
    uint32_t recvcnt = 0;
    int_t scanf_ret;
    uint32_t recv_socket_id;
    uint32_t recv_length;
    uint32_t stored_len;
    uint32_t i;
    volatile int32_t timeout;
    int32_t ret;
    byteq_err_t byteq_ret;
#if DEBUGLOG == 1
    TickType_t tmptime2;
#endif
    TickType_t tmptime1;

    /* Cast to an appropriate type */
    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if(0 == g_esp8266_socket[socket_no].socket_create_flag)
        {
            /* Give back the socketInUse mutex. */
            ( void ) xSemaphoreGive( g_esp8266_semaphore );
            return -1;
        }
#if DEBUGLOG == 1
    R_BSP_CpuInterruptLevelWrite (14);
    printf("esp8266_tcp_recv(%d)\r\n",socket_no);
    R_BSP_CpuInterruptLevelWrite (0);
#endif
        timeout_init(0, timeout_ms);

        stored_len = 0;

        while(1)
        {
            byteq_ret = R_BYTEQ_Get(g_esp8266_socket[socket_no].socket_byteq_hdl, (pdata + stored_len));
            if(BYTEQ_ERR_QUEUE_EMPTY == byteq_ret)
            {
                if((-1) == check_timeout(0, 0))
                {
                    timeout = 1;
                    break;
                }
            }
            if(BYTEQ_SUCCESS == byteq_ret)
            {
                stored_len++;
                if(stored_len >= length)
                {
                    #if DEBUGLOG == 1
                        tmptime2 = xTaskGetTickCount();
                        R_BSP_CpuInterruptLevelWrite (14);
                        printf("r:%06d:tcp %ld byte received.reqsize=%ld\r\n",tmptime2, stored_len, length);
                        R_BSP_CpuInterruptLevelWrite (0);
                    #endif
                    /* Give back the socketInUse mutex. */
                    ( void ) xSemaphoreGive( g_esp8266_semaphore );

                    return length;
                }
            }
        }

        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( g_esp8266_semaphore );
    }
    else
    {
        return -1;
    }
    return stored_len;
}

int32_t esp8266_serial_tcp_recv_timeout_set(uint8_t socket_no, TickType_t timeout_ms)
{
    g_sl_esp8266_tcp_recv_timeout = timeout_ms;
    return 0;
}

int32_t esp8266_tcp_disconnect(uint8_t socket_no)
{
    int32_t ret = 0;
    /* Cast to an appropriate type */
    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if(1 == g_esp8266_socket[socket_no].socket_create_flag)
        {
#if DEBUGLOG == 1
            R_BSP_CpuInterruptLevelWrite (14);
            printf("esp8266_tcp_disconnect(%d)\r\n",socket_no);
            R_BSP_CpuInterruptLevelWrite (0);
#endif
            /* cast to  char * */
            sprintf((char *)g_buff,"AT+CIPCLOSE=%d\r\n",socket_no);
            ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, g_buff, 3, 10000, ESP8266_RETURN_OK, ESP8266_SET_CIPCLOSE, socket_no);

            if(0 == ret)
            {
                g_esp8266_socket[socket_no].socket_create_flag = 0;
#if ESP8266_USE_UART_NUM == 2
                R_BYTEQ_Flush(g_esp8266_socket[socket_no].socket_byteq_hdl);
#endif
            }
            /* Give back the socketInUse mutex. */
            ( void ) xSemaphoreGive( g_esp8266_semaphore );
        }
        else
        {
            /* Give back the socketInUse mutex. */
            ( void ) xSemaphoreGive( g_esp8266_semaphore );
            return -1;
        }
    }
    else
    {
        return -1;
    }
    return ret;

}

int32_t esp8266_dns_query(uint8_t *ptextstring, uint32_t *ulipaddr)
{
    uint32_t result;
    int32_t func_ret;
    int32_t scanf_ret;
    /* cast to  char * */
    strcpy((char *)g_buff,"AT+CIPDOMAIN=\"");
    /* cast to  char * */
    sprintf((char *)g_buff+strlen((char *)g_buff),"%s\"\r\n",ptextstring);

    func_ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, g_buff, 3, 20000, ESP8266_RETURN_OK, ESP8266_SET_CIPDOMAIN, 0xff);
    if(func_ret != 0)
    {
        return -1;
    }
    /* cast to  uint32_t */
    *ulipaddr = (((uint32_t)g_dnsaddress[0]) << 24) | (((uint32_t)g_dnsaddress[1]) << 16) | (((uint32_t)g_dnsaddress[2]) << 8) | ((uint32_t)g_dnsaddress[3]);

    return 0;
}


static int32_t esp8266_serial_send_basic(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code)
{
#if DEBUGLOG == 1
    TickType_t tmptime1;
    TickType_t tmptime2;
#endif
    volatile int32_t timeout;
    int32_t ercd;
    uint32_t recvcnt = 0;
    int32_t ret;
    memset(g_recvbuff,0,sizeof(g_recvbuff));

    timeout_init(serial_ch_id, timeout_ms);

    /* Cast to an appropriate type */
    if(ptextstring != NULL)
    {
        timeout = 0;
        recvcnt = 0;
        /* cast to  char * */
        ercd = write(g_esp8266_handle, ptextstring, strlen((const char *)ptextstring));
        if(ercd == 0)
        {
            return -1;
        }

        while(1)
        {
            /* cast to  char * */
            if(ercd == strlen((const char *)ptextstring))
            {
                break;
            }
            if((-1) == check_timeout(serial_ch_id, recvcnt))
            {
                timeout = 1;
                break;
            }
        }
        if(timeout == 1)
        {
#if DEBUGLOG == 1
            R_BSP_CpuInterruptLevelWrite (14);
            printf("timeout.\r\n",tmptime1,ptextstring);
            R_BSP_CpuInterruptLevelWrite (0);
#endif
            return -1;
        }

    #if DEBUGLOG == 1
        tmptime1 = xTaskGetTickCount();
        /* cast to  char * */
        if(ptextstring[strlen((const char *)ptextstring)-1] != '\r')
        {
            R_BSP_CpuInterruptLevelWrite (14);
            printf("s:%06d:%s\r\n",tmptime1,ptextstring);
            R_BSP_CpuInterruptLevelWrite (0);
        }
        else
        {
            R_BSP_CpuInterruptLevelWrite (14);
            printf("s:%06d:%s",tmptime1,ptextstring);
            printf("\n");
            R_BSP_CpuInterruptLevelWrite (0);
        }
    #endif
    }

    ret = -1;

    while(1)
    {
        ercd = read(g_esp8266_handle, &g_recvbuff[recvcnt], 1);
        if(0 < ercd)
        {
            recvcnt++;
            bytetimeout_init(serial_ch_id, response_type);

            g_recvbuff[recvcnt] = '\0';
            /* cast to  char * */
            if(recvcnt >= strlen((const char *)gp_esp8266_result_code[expect_code][g_sx_esp8266_return_mode]))
            {
                /* cast to  char * */
                if(0 == strncmp((const char *)&g_recvbuff[recvcnt - strlen((const char *)gp_esp8266_result_code[expect_code][g_sx_esp8266_return_mode]) ],
                        /* cast to  char * */
                        (const char *)gp_esp8266_result_code[expect_code][g_sx_esp8266_return_mode],
                        /* cast to  char * */
                        strlen((const char *)gp_esp8266_result_code[expect_code][g_sx_esp8266_return_mode])))
                {
                    ret = 0;
                    break;
                }
            }
        }
        if((-1) == check_bytetimeout(serial_ch_id, recvcnt))
        {
            break;
        }
        if((-1) == check_timeout(serial_ch_id, recvcnt))
        {
            timeout = 1;
            break;
        }
    }
    if(timeout == 1)
    {
        return (-1);
    }

#if DEBUGLOG == 1
    tmptime2 = xTaskGetTickCount();
    printf("r:%06d:%s",tmptime2,g_recvbuff);
#endif
    return ret;
}


static int32_t esp8266_serial_send_with_recvtask(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code,  uint8_t command, uint8_t socket_no)
{
#if DEBUGLOG == 1
    TickType_t tmptime1;
    TickType_t tmptime2;
#endif
    volatile int32_t timeout;
    int32_t ercd;
    uint32_t recvcnt = 0;
    int32_t ret;
    uint8_t result;

    timeout_init(serial_ch_id, timeout_ms);

    /* Cast to an appropriate type */
    if(ptextstring != NULL)
    {
        timeout = 0;
        recvcnt = 0;

        esp8266_response_set_queue( command, socket_no );
        g_esp8266_uart_teiflag[serial_ch_id] = 0;
        ercd = 0;
        /* cast to  char * */
        ercd = write(g_esp8266_handle, ptextstring, strlen((const char *)ptextstring));
        if(ercd == 0)
        {
            return -1;
        }

        while(1)
        {
            /* cast to  char * */
            if(ercd == strlen((const char *)ptextstring))
            {
                break;
            }
            if((-1) == check_timeout(serial_ch_id, recvcnt))
            {
                timeout = 1;
                break;
            }
        }
        if(timeout == 1)
        {
#if DEBUGLOG == 1
            R_BSP_CpuInterruptLevelWrite (14);
            printf("timeout.\r\n",tmptime1,ptextstring);
            R_BSP_CpuInterruptLevelWrite (0);
#endif
            return -1;
        }
    }
    while(1)
    {
        ercd = esp8266_response_get_queue( command, socket_no, &result );
        if(0 == ercd )
        {
            break;
        }

        if((-1) == check_timeout(serial_ch_id, recvcnt))
        {
            timeout = 1;
            break;
        }
    }
    if(timeout == 1)
    {
        return -1;
    }

    ret = -1;
    if(result == expect_code)
    {
        ret = 0;
    }
    return ret;
}

static int32_t esp8266_serial_send_rst(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code, sx_esp8266_return_code_t end_string)
{
#if DEBUGLOG == 1
    TickType_t tmptime1;
    TickType_t tmptime2;
#endif
    volatile int32_t timeout;
    int32_t ercd;
    uint32_t recvcnt = 0;

    timeout_init(serial_ch_id, timeout_ms);

    /* Cast to an appropriate type */
    if(ptextstring != NULL)
    {
        timeout = 0;
        recvcnt = 0;
        g_esp8266_uart_teiflag[serial_ch_id] = 0;
        ercd = 0;
        /* cast to  char * */
        ercd = write(g_esp8266_handle, ptextstring, strlen((const char *)ptextstring));
        if(ercd == 0)
        {
            return -1;
        }

        while(1)
        {
            /* cast to  char * */
            if(ercd == strlen((const char *)ptextstring))   //190625 modified for RZ/A2M. the return value type is different from RX.
            {
                break;
            }
            if((-1) == check_timeout(serial_ch_id, recvcnt))
            {
                timeout = 1;
                break;
            }
        }
        if(timeout == 1)
        {
#if DEBUGLOG == 1
            R_BSP_CpuInterruptLevelWrite (14);
            printf("timeout.\r\n",tmptime1,ptextstring);
            R_BSP_CpuInterruptLevelWrite (0);
#endif
            return -1;
        }

    #if DEBUGLOG == 1
        tmptime1 = xTaskGetTickCount();
        /* cast to  char * */
        if(ptextstring[strlen((const char *)ptextstring)-1] != '\r')
        {
            R_BSP_CpuInterruptLevelWrite (14);
            printf("s:%06d:%s\r\n",tmptime1,ptextstring);
            R_BSP_CpuInterruptLevelWrite (0);
        }
        else
        {
            R_BSP_CpuInterruptLevelWrite (14);
            printf("s:%06d:%s",tmptime1,ptextstring);
            printf("\n");
            R_BSP_CpuInterruptLevelWrite (0);
        }
    #endif
    }
    ercd = 0;
    while(1)
    {
        ercd = read(g_esp8266_handle, &g_recvbuff[recvcnt], 1);
        if(0 < ercd)
        {
            recvcnt++;

            if(end_string != 0xff)
            {
                /* cast to  char * */
                if(0 == strncmp((const char *)&g_recvbuff[recvcnt - strlen((const char *)gp_esp8266_result_code[end_string][g_sx_esp8266_return_mode]) ],
                        /* cast to  char * */
                        (const char *)gp_esp8266_result_code[end_string][g_sx_esp8266_return_mode], strlen((const char *)gp_esp8266_result_code[end_string][g_sx_esp8266_return_mode])))
                {
                    return 0;
                }
            }
        }
        if((-1) == check_bytetimeout(serial_ch_id, recvcnt))
        {
            break;
        }
        if((-1) == check_timeout(serial_ch_id, recvcnt))
        {
            timeout = 1;
            break;
        }
    }
    if(timeout == 1)
    {
        return -1;
    }

#if DEBUGLOG == 1
    tmptime2 = xTaskGetTickCount();
    printf("r:%06d:%s",tmptime2,g_recvbuff);
#endif

    return 0;
}


static void timeout_init(uint8_t socket_no, uint16_t timeout_ms)
{
    g_starttime[socket_no] = xTaskGetTickCount();
    g_endtime[socket_no] = g_starttime[socket_no] + timeout_ms;
    if((g_starttime[socket_no] + g_endtime[socket_no]) < g_starttime[socket_no])
    {
        /* overflow */
        g_timeout_overflow_flag[socket_no] = 1;
    }
    else
    {
        g_timeout_overflow_flag[socket_no] = 0;
    }
}

static int32_t check_timeout(uint8_t socket_no, int32_t rcvcount)
{
    if(0 == rcvcount)
    {
        g_thistime[socket_no] = xTaskGetTickCount();
        if(g_timeout_overflow_flag[socket_no] == 0)
        {
            if((g_thistime[socket_no] >= g_endtime[socket_no]) || (g_thistime[socket_no] < g_starttime[socket_no]))
            {
                return -1;
            }
        }
        else
        {
            if((g_thistime[socket_no] < g_starttime[socket_no]) && (g_thistime[socket_no] <= g_endtime[socket_no]))
            {
                /* Not timeout  */
                return -1;
            }
        }
    }
    /* Not timeout  */
    return 0;
}

static void bytetimeout_init(uint8_t socket_no, uint16_t timeout_ms)
{
    g_startbytetime[socket_no] = xTaskGetTickCount();
    g_endbytetime[socket_no] = g_startbytetime[socket_no] + timeout_ms;
    if((g_startbytetime[socket_no] + g_endbytetime[socket_no]) < g_startbytetime[socket_no])
    {
        /* overflow */
        g_byte_timeout_overflow_flag[socket_no] = 1;
    }
    else
    {
        g_byte_timeout_overflow_flag[socket_no] = 0;
    }
}

static int32_t check_bytetimeout(uint8_t socket_no, int32_t rcvcount)
{
    if(0 != rcvcount)
    {
        g_thisbytetime[socket_no] = xTaskGetTickCount();
        if(g_byte_timeout_overflow_flag[socket_no] == 0)
        {
            if((g_thisbytetime[socket_no] >= g_endbytetime[socket_no]) || (g_thisbytetime[socket_no] < g_startbytetime[socket_no]))
            {
                return -1;
            }
        }
        else
        {
            if((g_thisbytetime[socket_no] < g_startbytetime[socket_no]) && (g_thisbytetime[socket_no] <= g_endbytetime[socket_no]))
            {
                /* Not timeout  */
                return -1;
            }
        }
    }
    /* Not timeout  */
    return 0;
}

static int32_t esp8266_serial_open(uint32_t baudrate)
{
    int32_t   my_sci_err;
    scifa_config_t scifa_cfg;

    g_esp8266_handle = open(DEVICE_INDENTIFIER "scifa3", O_RDWR); //190625 modified for RZ/A2M. use the function of SCIFA.

    if (0 > g_esp8266_handle)
    {
        return -1;
    }

    /* Cast to an appropriate type */
    control(g_esp8266_handle, CTL_SCIFA_GET_CONFIGURATION, (void *)&scifa_cfg);

    scifa_cfg.baud_rate = baudrate;
    scifa_cfg.clk_enable = SCIFA_CLK_SRC_INT_SCK_IN;
    scifa_cfg.data_size = SCIFA_DATA_8BIT;
    scifa_cfg.parity_en = SCIFA_PARITY_OFF;
    scifa_cfg.stop_bits = SCIFA_STOPBITS_1;

    /* Cast to an appropriate type */
    control(g_esp8266_handle, CTL_SCIFA_SET_CONFIGURATION, (void *)&scifa_cfg);

    return 0;

}

static int32_t esp8266_serial_close(void)
{

    close(g_esp8266_handle);
    return 0;
}

int32_t esp8266_socket_init(void)
{
    int_t i;
    for(i = 0;i<CREATEABLE_SOCKETS; i++)
    {
        if(BYTEQ_SUCCESS != R_BYTEQ_Open(g_esp8266_socket[i].socket_recv_buff, sizeof(g_esp8266_socket[i].socket_recv_buff), &g_esp8266_socket[i].socket_byteq_hdl))
        {
            return -1;
        }
    }

    /* Cast to an appropriate type */
    g_esp8266_semaphore = xSemaphoreCreateMutex();

    /* Cast to an appropriate type */
    if( g_esp8266_semaphore == NULL )
    {
        return -1;
    }
    /* Success. */
    return 0;

}

