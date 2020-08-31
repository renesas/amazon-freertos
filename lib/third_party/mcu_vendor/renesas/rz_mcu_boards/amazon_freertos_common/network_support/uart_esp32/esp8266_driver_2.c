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
 * Renesas reserves the right, without notice, to make changes to this
 * software and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "r_typedefs.h"
#include "r_byteq_if.h"
#include "esp8266_driver.h"

#define ESP8266_DATA_RECEIVE_COMMAND "\r\n+IPD,%d,%d"
#define ESP8266_READ_MAC             "+CIPAPMAC"
#define ESP8266_READ_IP              "+CIPSTA"
#define ESP8266_READ_SNTP            "+CIPSNTPTIME"
#define ESP8266_READ_DNS             "+CIPDOMAIN"

extern uint8_t g_recvbuff[2048+20];
uint32_t g_recv_count;
uint32_t g_tmp_recvcnt;
uint8_t g_receive_status;
uint8_t g_receive_sub_status1;
uint8_t g_receive_sub_status2;
uint8_t g_recv_continue;

uint8_t g_macaddress[6];
uint8_t g_ipaddress[4];
uint8_t g_subnetmask[4];
uint8_t g_gateway[4];

uint16_t g_sntp_year;
uint8_t g_sntp_mon[5];
uint8_t g_sntp_day;
uint8_t g_sntp_hour;
uint8_t g_sntp_min;
uint8_t g_sntp_sec;
uint8_t g_sntp_week[5];
uint32_t g_sntp_year_32;
uint8_t g_dnsaddress[4];

esp8266_socket_t g_esp8266_socket[5];

uint8_t g_responce_wait_queue[10][3];
uint8_t g_responce_wait_queue_set_index;
uint8_t g_responce_wait_queue_wait_index;

extern int_t g_esp8266_handle;

static void esp8266_recv_task( void * pvParameters );
static uint8_t esp8266_response_get_now_queue(void);
static void esp8266_response_set_result( uint8_t result );

void vstart_esp8266_recv_task( void )
{
    BaseType_t x;

    /* Create the echo client tasks. */
    xTaskCreate( 
                /* The function that implements the task. */
                esp8266_recv_task, 
                /* Just a text name for the task to aid debugging. */
                "esp8826_recv",
                1024,
                /* The task parameter, not used in this case. */
                /* Cast to an appropriate type */
                ( void * ) 0,
                /* task priority */
                tskIDLE_PRIORITY + 28,
                /* The task handle is not used. */
                NULL );
}


static void esp8266_recv_task( void * pvParameters )
{
    uint8_t data;
    int_t ercd;
    char * string_ercd;
    int_t sscanf_ret;
    int_t i;
    uint32_t socket_no;
    uint32_t len;
    byteq_err_t byteq_ercd;
    memset(g_recvbuff,0,sizeof(g_recvbuff));
    g_recv_count = 0;

    for( ; ; )
    {
        ercd = read(g_esp8266_handle, &data, 1);
        if(ercd > 0)
        {
            g_recvbuff[g_recv_count] = data;
            if (g_recv_count < ((sizeof(g_recvbuff)) - 2)) 
            {
                g_recv_count++;
            }
#if DEBUGLOG == 2
            putchar(data);
#endif
            switch(g_receive_status)
            {
                case 0:
                    switch(data)
                    {
                        case '\r':
                            g_receive_status = 1;
                            g_receive_sub_status1 = 0;
                            g_receive_sub_status2 = 0;
                        break;
                        case '+':
                            g_receive_status = 2;
                            g_receive_sub_status1 = 0;
                            g_receive_sub_status2 = 0;
                        break;
                        case '>':
                            g_receive_status = 9;
                            g_receive_sub_status1 = 0;
                            g_receive_sub_status2 = 0;
                            break;
                        default:
                            g_receive_status = 0xff;
                            g_receive_sub_status1 = 0;
                            g_receive_sub_status2 = 0;
                        break;
                    }
                break;
                case 1:
                    switch(g_receive_sub_status1)
                    {
                        case 0:
                            if(data == '\n')
                            {
                                g_receive_sub_status1 = 1;
                            }
                            break;
                        case 1:
                            if(data == '+')
                            {
                                /*\r\n+IPD... */
                                g_receive_status = 2;
                                g_receive_sub_status1 = 0;
                            }
                            else
                            {
                                g_receive_sub_status1 = 2;
                            }
                            break;
                        case 2:
                            {
                                string_ercd = strstr(&g_recvbuff[2],"\r\n");
                                /* Cast to an appropriate type */
                                if(string_ercd != NULL)
                                {
                                    if(0 == strncmp(&g_recvbuff[2],ESP8266_RETURN_TEXT_OK,4))
                                    {
                                        if(ESP8266_SET_CIPSEND == esp8266_response_get_now_queue())
                                        {
                                            g_receive_status = 8;
                                            g_tmp_recvcnt = g_recv_count;
                                            break;
                                        }
                                        else
                                        {
                                            esp8266_response_set_result( ESP8266_RETURN_OK );
                                        }
                                    }
                                    else if(0 == strncmp(&g_recvbuff[2],ESP8266_RETURN_TEXT_ERROR,7))
                                    {
                                        esp8266_response_set_result( ESP8266_RETURN_ERROR );
                                    }
                                    else if(0 == strncmp(&g_recvbuff[2],ESP8266_RETURN_TEXT_SEND_OK,9))
                                    {
                                        esp8266_response_set_result( ESP8266_RETURN_SEND_OK );
                                    }
                                    else if(0 == strncmp(&g_recvbuff[2],ESP8266_RETURN_TEXT_SEND_FAIL,11))
                                    {
                                        esp8266_response_set_result( ESP8266_RETURN_SEND_FAIL );
                                    }
                                    memset(g_recvbuff,0,sizeof(g_recvbuff));
                                    g_recv_count = 0;
                                    g_receive_status = 0;
                                    g_receive_sub_status1 = 0;
                                    g_receive_sub_status2 = 0;
                                }
                            }
                            break;
                    }
                    break;
                case 2:
                    switch(g_receive_sub_status1)
                    {
                        case 0:
                            if(data == ':')
                            {
                                g_receive_sub_status1 = 1;
                                g_recvbuff[g_recv_count-1] = '\0';
                                sscanf_ret = sscanf(g_recvbuff,ESP8266_DATA_RECEIVE_COMMAND,&socket_no,&len);
                                if(sscanf_ret == 2)
                                {
                                    g_esp8266_socket[socket_no].receive_num = len;
                                    g_esp8266_socket[socket_no].receive_count = 0;
                                    g_receive_status = 3;
                                    g_receive_sub_status1 = 0;
                                }
                                else if (0 == strcmp(g_recvbuff,ESP8266_READ_MAC))
                                {
                                    g_receive_status = 4;
                                    g_receive_sub_status1 = 0;
                                    g_tmp_recvcnt = g_recv_count;
                                }
                                else if (0 == strcmp(g_recvbuff,ESP8266_READ_IP))
                                {
                                    g_receive_status = 5;
                                    g_receive_sub_status1 = 0;
                                    g_tmp_recvcnt = g_recv_count;
                                }
                                else if (0 == strcmp(g_recvbuff,ESP8266_READ_SNTP))
                                {
                                    g_receive_status = 6;
                                    g_receive_sub_status1 = 0;
                                    g_tmp_recvcnt = g_recv_count;
                                }
                                else if (0 == strcmp(g_recvbuff,ESP8266_READ_DNS))
                                {
                                    g_receive_status = 7;
                                    g_receive_sub_status1 = 0;
                                    g_tmp_recvcnt = g_recv_count;
                                }
                                else
                                {

                                }
                            }
                            break;
                        case 1:
                        break;
                    }
                    break;
                case 3:
                    byteq_ercd = R_BYTEQ_Put(g_esp8266_socket[socket_no].socket_byteq_hdl, data);
                    if(byteq_ercd != BYTEQ_SUCCESS)
                    {
                        g_esp8266_socket[socket_no].put_error_count++;
                    }
                    g_esp8266_socket[socket_no].receive_count++;
                    if(g_esp8266_socket[socket_no].receive_count >= g_esp8266_socket[socket_no].receive_num)
                    {
                        memset(g_recvbuff,0,sizeof(g_recvbuff));
                        g_recv_count = 0;
                        g_receive_status = 0;
                        g_receive_sub_status1 = 0;
                        g_receive_sub_status2 = 0;
                    }
                    break;
                case 4: /* MAC */
                    if(0 == strcmp(&g_recvbuff[g_tmp_recvcnt],"\r\n"))
                    {
                        if(6 == sscanf(&g_recvbuff[g_tmp_recvcnt],"\"%d:%d:%d:%d:%d:%d\"\r\n", &g_macaddress[0],&g_macaddress[1],&g_macaddress[2],&g_macaddress[3],&g_macaddress[4],&g_macaddress[5]))
                        {
                            memset(g_recvbuff,0,sizeof(g_recvbuff));
                            g_recv_count = 0;
                            g_receive_status = 0;
                            g_receive_sub_status1 = 0;
                            g_receive_sub_status2 = 0;
                        }
                    }
                    break;
                case 5: /* IP */
                    if(0 == strcmp(&g_recvbuff[g_tmp_recvcnt],"\r\n"))
                    {
                        if(6 == sscanf(&g_recvbuff[g_tmp_recvcnt],"%d:%d:%d:%d:%d:%d\r\n", &g_macaddress[0],&g_macaddress[1],&g_macaddress[2],&g_macaddress[3],&g_macaddress[4],&g_macaddress[5]))
                        {
                            memset(g_recvbuff,0,sizeof(g_recvbuff));
                            g_recv_count = 0;
                            g_receive_status = 0;
                            g_receive_sub_status1 = 0;
                            g_receive_sub_status2 = 0;
                        }
                    }
                    break;
                case 6: /* SNTP */
                    switch(g_receive_sub_status1)
                    {
                        case 0:
                            if(data == '\n')
                            {
                                g_receive_sub_status1 = 1;
                                sscanf(&g_recvbuff[g_tmp_recvcnt],"%s%s%d %d:%d:%d %d\n",g_sntp_week,g_sntp_mon,&g_sntp_day,&g_sntp_hour,&g_sntp_min,&g_sntp_sec,&g_sntp_year_32);
                                /* Cast to an uint16_t */
                                g_sntp_year = (uint16_t)g_sntp_year_32;
                                memset(g_recvbuff,0,sizeof(g_recvbuff));
                                g_recv_count = 0;
                                g_receive_status = 0;
                                g_receive_sub_status1 = 0;
                                g_receive_sub_status2 = 0;
                            }
                            break;
                    }
                    break;
                case 7: /* DNS */
                    if(NULL != strstr(&g_recvbuff[g_tmp_recvcnt],"\r\n"))
                    {
                        sscanf(&g_recvbuff[g_tmp_recvcnt],"%d.%d.%d.%d\r\n",&g_dnsaddress[0],&g_dnsaddress[1],&g_dnsaddress[2],&g_dnsaddress[3]);
                        memset(g_recvbuff,0,sizeof(g_recvbuff));
                        g_recv_count = 0;
                        g_receive_status = 0;
                        g_receive_sub_status1 = 0;
                        g_receive_sub_status2 = 0;
                    }
                    break;
                case 8: /* CIPSEND -> OK\r\n>  */
                    if(0 == strcmp(&g_recvbuff[g_tmp_recvcnt],"\r\n>"))
                    {
                        esp8266_response_set_result( ESP8266_RETURN_OK_GO_SEND );
                        memset(g_recvbuff,0,sizeof(g_recvbuff));
                        g_recv_count = 0;
                        g_receive_status = 0;
                        g_receive_sub_status1 = 0;
                        g_receive_sub_status2 = 0;
                    }
                    break;
                case 9: /* CIPSEND -> OK\r\n>  */
                    if(data == ' ')
                    {
                        esp8266_response_set_result( ESP8266_RETURN_OK_GO_SEND );
                        memset(g_recvbuff,0,sizeof(g_recvbuff));
                        g_recv_count = 0;
                        g_receive_status = 0;
                        g_receive_sub_status1 = 0;
                        g_receive_sub_status2 = 0;
                    }
                    break;

                case 0xff:
                    switch(g_receive_sub_status1)
                    {
                        case 0:
                            string_ercd = strstr(&g_recvbuff[1],"\r\n");
                            /* Cast to an appropriate type */
                            if(string_ercd != NULL)
                            {
                                /* Cast to an appropriate type */
                                if(NULL != strstr(&g_recvbuff[1],ESP8266_RETURN_TEXT_SEND_FAIL))
                                {
                                    esp8266_response_set_result( ESP8266_RETURN_SEND_FAIL );
                                }
                                
                                memset(g_recvbuff,0,sizeof(g_recvbuff));
                                g_recv_count = 0;
                                g_receive_status = 0;
                                g_receive_sub_status1 = 0;
                                g_receive_sub_status2 = 0;
                            }
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
        else
        {
            vTaskDelay( 1 );
        }

    }   
}

void esp8266_response_set_queue( uint8_t command, uint8_t socket )
{
    g_responce_wait_queue[g_responce_wait_queue_set_index][0] = command;
    g_responce_wait_queue[g_responce_wait_queue_set_index][1] = socket;
    g_responce_wait_queue[g_responce_wait_queue_set_index][2] = 0xff;
    g_responce_wait_queue_set_index++;
    if(g_responce_wait_queue_set_index >= 10)
    {
        g_responce_wait_queue_set_index = 0;
    }
}

int8_t esp8266_response_get_queue( uint8_t command, uint8_t socket, uint8_t *result)
{
    int_t i;

    for(i = 0;i<10;i++)
    {
        if((g_responce_wait_queue[i][0] == command) && 
            (g_responce_wait_queue[i][1] == socket))
        {
            if((g_responce_wait_queue[i][2] != 0xff) && 
                (g_responce_wait_queue[i][2] != 0xfe))
            {
                *result = g_responce_wait_queue[i][2];
                g_responce_wait_queue[i][2] = 0xfe;
                break;
            }
        }
    }
    if(i>= 10)
    {
        return -1;
    }
    return 0;
}

static uint8_t esp8266_response_get_now_queue(void)
{
    return  g_responce_wait_queue[g_responce_wait_queue_wait_index][0];
}

static void esp8266_response_set_result( uint8_t result )
{
    g_responce_wait_queue[g_responce_wait_queue_wait_index][2] = result;
    g_responce_wait_queue_wait_index++;
    if(g_responce_wait_queue_wait_index >= 10)
    {
        g_responce_wait_queue_wait_index = 0;
    }
}
