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
 * File Name    : r_wifi_sx_ulpgn_recv_task.c
 * Version      : 1.0
 * Description  : RTOS functions definition for SX ULPGN of RX65N.
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
#include <stdlib.h>

#include "FreeRTOS.h"

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#include "platform.h"
#include "r_sci_rx_if.h"
#endif
#include "r_byteq_if.h"
#include "r_wifi_sx_ulpgn_if.h"
#include "r_wifi_sx_ulpgn_private.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
#if(1 == WIFI_CFG_USE_CALLBACK_FUNCTION)
void WIFI_CFG_CALLBACK_FUNCTION_NAME(void * p_args);
void (*const p_wifi_callback)(void *p_args) = WIFI_CFG_CALLBACK_FUNCTION_NAME;
#else
void (* const p_wifi_callback)(void *p_args) = NULL;
#endif

uint32_t g_wifi_response_recv_count;
uint32_t g_wifi_response_recv_status;

char g_wifi_response_last_string[15];
uint8_t g_wifi_response_last_string_recv_count;

uint8_t g_wifi_macaddress[6];
wifi_ip_configuration_t g_wifi_ipconfig;
uint32_t g_wifi_dnsaddress;
uint32_t g_wifi_dnsquery_subcount;

uint32_t g_wifi_atustat_recv;
uint32_t g_wifi_atustat_sent;

wifi_scan_result_t * gp_wifi_ap_results;
uint32_t g_wifi_aplistmax;
uint32_t g_wifi_aplist_stored_num;
uint32_t g_wifi_aplist_count;
uint32_t g_wifi_aplist_subcount;

uint8_t g_wifi_current_ssid[33];

uint8_t g_wifi_socket_status;

wifi_socket_t g_wifi_socket[WIFI_CFG_CREATABLE_SOCKETS];

st_at_exec_queue_t g_wifi_at_execute_queue[10];
uint8_t g_wifi_set_queue_index;
uint8_t g_wifi_get_queue_index;

TaskHandle_t g_wifi_recv_task_handle;
uint8_t g_certificate_str[256] =
{ 0 };
uint8_t g_certificate_numstr[256] =
{ 0 };
wifi_certificate_infomation_t g_wifi_certificate_information[10];
uint8_t g_number_flg = 1;
uint8_t g_certificate_list_flg = 0;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static uint8_t is_number_check (uint8_t c);
static void wifi_recv_task (void * pvParameters);
static void wifi_analyze_ipaddress_string (uint8_t * pstring);
static void wifi_analyze_get_macaddress_string (uint8_t * pstring);
static void wifi_analyze_get_aplist_string (uint8_t * pstring);
static void wifi_analyze_get_dnsquery_string (uint8_t * pstring);
static void wifi_analyze_get_sent_recv_size_string (uint8_t * pstring);
static void wifi_analyze_get_current_ssid_string (uint8_t * pstring);
static void wifi_analyze_get_socket_status_string (uint8_t * pstring);
static void wifi_analyze_get_certificate_string (uint8_t * pstring);

/**********************************************************************************************************************
 * Function Name: is_number_check
 * Description  :
 * Arguments    : c
 * Return Value : 0 - c is not number.
 *                1 - c is number
 *********************************************************************************************************************/
static uint8_t is_number_check(uint8_t c)
{
    if ((0x30u <= c) && (0x39u >= c))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**********************************************************************************************************************
 * End of function is_number_check
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_get_certificate_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_get_certificate_string(uint8_t * pstring)
{
    uint8_t unum = 0;
    uint8_t start_num = 0;
    uint8_t ufnum = 0;

    /* pstring */
    if (0 == strcmp((char *)pstring, "OK\r\n"))
    {
        ; /* Do nothing */
    }
    else
    {
        if ('\r' == (*pstring))
        {
            g_certificate_list_flg = 1;
            g_number_flg = 1;
            unum = 0;
            while ('\0' != g_certificate_str[unum])
            {
                if ('\r' == g_certificate_str[unum])
                {
                    memcpy(&g_wifi_certificate_information[ufnum].certificate_file, &g_certificate_str[start_num],
                            (unum - start_num));
                    g_wifi_certificate_information[ufnum].next_certificate_name =
                            &g_wifi_certificate_information[ufnum + 1];
                    start_num = unum + 2;
                    ufnum++;
                }
                unum++;
            }
            memset(g_certificate_str, 0, sizeof(g_certificate_str));
            memset(g_certificate_numstr, 0, sizeof(g_certificate_numstr));
        }
        else
        {
            if (1 == g_number_flg)
            {
                memset(g_certificate_str, 0, sizeof(g_certificate_str));
                memset(g_certificate_numstr, 0, sizeof(g_certificate_numstr));

                /* certificate name */
                strcat((char *)g_certificate_numstr, (char *)pstring);

                /* certificate number */
                for (unum = 0; unum < strlen((char *)&g_certificate_numstr); unum++ )
                {
                    if ('\r' == g_certificate_numstr[unum])
                    {
                        break;
                    }
                    if (0 == is_number_check(g_certificate_numstr[unum]))
                    {
                        g_number_flg = 0;
                        break;
                    }
                }
                if (1 == g_number_flg)
                {
                    if (1 == unum)
                    {
                        g_wifi_certificate_information[0].certificate_number = g_certificate_numstr[0] - 0x30;
                    }
                    else if (2 == unum)
                    {
                        g_wifi_certificate_information[0].certificate_number =
                                ((g_certificate_numstr[0] - 0x30) * 10) + (g_certificate_numstr[1] - 0x30);
                    }
                    else
                    {
                        g_wifi_certificate_information[0].certificate_number = 0;
                    }
                }
                g_number_flg = 0;
                g_certificate_list_flg = 0;
            }
            else
            {
                /* certificate name */
                strcat((char *)g_certificate_str, (char *)pstring);
            }
        }
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_get_certificate_string
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_start_recv_task
 * Description  :
 * Arguments    : none
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
int32_t wifi_start_recv_task(void)
{
    int32_t ret = -1;
    BaseType_t xReturned;

    /* Create wifi driver at response tasks. */
    xReturned = xTaskCreate(wifi_recv_task,            /* The function that implements the task. */
                            "sx_ulpgn_recv",           /* Just a text name for the task to aid debugging. */
                            1024,
                            (void*) 0,                 /* The task parameter, not used in this case. */
                            WIFI_TASK_PRIORITY,
                            &g_wifi_recv_task_handle); /* The task handle is not used. */
    if (pdPASS == xReturned)
    {
        ret = 0;
    }
    return ret;
}
/**********************************************************************************************************************
 * End of function wifi_start_recv_task
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_delete_recv_task
 * Description  :
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void wifi_delete_recv_task(void)
{
    /* Delete wifi driver at response tasks. */
    if (NULL != g_wifi_recv_task_handle)
    {
        vTaskDelete(g_wifi_recv_task_handle);
        g_wifi_recv_task_handle = NULL;
    }
}
/**********************************************************************************************************************
 * End of function wifi_delete_recv_task
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_recv_task
 * Description  :
 * Arguments    : pvParameters
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_recv_task(void * pvParameters)
{
    uint8_t   data;
    uint8_t * p_response_buff;
    sci_err_t sci_ercd;
    int i;
    byteq_err_t byteq_ercd;
    uint32_t psw_value;
    st_at_exec_queue_t * p_queue;
    wifi_err_event_t event;
    uint8_t command_port;
    g_wifi_response_recv_count = 0;
    g_wifi_response_last_string_recv_count = 0;
    memset(g_wifi_response_last_string, 0, sizeof(g_wifi_response_last_string));

    for (;;)
    {
        command_port = g_atcmd_port;
        p_response_buff = g_wifi_uart[command_port].p_respbuf;
        sci_ercd = R_SCI_Receive(g_wifi_uart[command_port].wifi_uart_sci_handle, &data, 1);
        if (SCI_ERR_INSUFFICIENT_DATA == sci_ercd)
        {
            /* Pause for a short while to ensure the network is not too
             * congested. */
            ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        }
        else if (SCI_SUCCESS == sci_ercd)
        {
#if DEBUGLOG == 2
            R_BSP_CpuInterruptLevelWrite (WIFI_CFG_SCI_INTERRUPT_LEVEL-1);
            putchar(data);
            R_BSP_CpuInterruptLevelWrite (0);
#endif
            if (1 == g_wifi_transparent_mode)
            {
                psw_value = R_BSP_CpuInterruptLevelRead();
                R_BSP_CpuInterruptLevelWrite(WIFI_CFG_SCI_INTERRUPT_LEVEL - 1);
                byteq_ercd = R_BYTEQ_Put(g_wifi_socket[0].socket_byteq_hdl, data);
                R_BSP_CpuInterruptLevelWrite(psw_value);
                if (BYTEQ_SUCCESS != byteq_ercd)
                {
                    g_wifi_socket[0].put_error_count++;
                }
            }
            else
            {
                if (g_wifi_response_recv_count >= (g_wifi_uart[command_port].respbuf_size - 1))
                {
                    event.event = WIFI_EVENT_RCV_TASK_RXB_OVF_ERR;
                    if (NULL != p_wifi_callback)
                    {
                        p_wifi_callback( &event);
                    }
                }
                else
                {
                    p_response_buff[g_wifi_response_recv_count] = data;
                }
                g_wifi_response_recv_count++;
            }
            if (g_wifi_response_last_string_recv_count >= ((sizeof(g_wifi_response_last_string) - 1)))
            {
                memmove( &g_wifi_response_last_string[0], &g_wifi_response_last_string[1],
                        (sizeof(g_wifi_response_last_string) - 1));
                g_wifi_response_last_string[(sizeof(g_wifi_response_last_string) - 2)] = data;
                g_wifi_response_last_string[(sizeof(g_wifi_response_last_string) - 1)] = '\0';
            }
            else
            {
                g_wifi_response_last_string[g_wifi_response_last_string_recv_count] = data;
                g_wifi_response_last_string[g_wifi_response_last_string_recv_count + 1] = '\0';
                g_wifi_response_last_string_recv_count++;

            }
            if ('\n' == data)
            {
                p_queue = wifi_get_current_running_queue();

                if (p_queue->at_command_id == WIFI_COMMAND_GET_IPADDRESS)
                {
                    /* IP address */
                    if (0 == strncmp((char *)p_response_buff, "IP:", 3))
                    {
                        wifi_analyze_ipaddress_string(p_response_buff);
                    }
                }
                if (p_queue->at_command_id == WIFI_COMMAND_GET_MACADDRESS)
                {
                    /* MAC address */
                    if (0 == strncmp((char *)p_response_buff, "Mac Addr", 8))
                    {
                        wifi_analyze_get_macaddress_string(p_response_buff);
                    }
                }
                if (p_queue->at_command_id == WIFI_COMMAND_SET_DNSQUERY)
                {
                    wifi_analyze_get_dnsquery_string(p_response_buff);
                }
                if (p_queue->at_command_id == WIFI_COMMAND_GET_APLIST)
                {
                    wifi_analyze_get_aplist_string(p_response_buff);
                }
                if (p_queue->at_command_id == WIFI_COMMAND_GET_SENT_RECV_SIZE)
                {
                    wifi_analyze_get_sent_recv_size_string(p_response_buff);
                }
                if (p_queue->at_command_id == WIFI_COMMAND_GET_CURRENT_SSID)
                {
                    wifi_analyze_get_current_ssid_string(p_response_buff);
                }
                if (p_queue->at_command_id == WIFI_COMMAND_GET_SOCKET_STATUS)
                {
                    wifi_analyze_get_socket_status_string(p_response_buff);
                }
                /* Error */
                if (0 == strcmp((const char* )p_response_buff, WIFI_RETURN_TEXT_ERROR))
                {
                    wifi_set_result_to_current_running_queue(WIFI_RETURN_ENUM_ERROR);
                }
                /* Busy */
                else if (0 == strcmp((const char* )p_response_buff, WIFI_RETURN_TEXT_BUSY))
                {
                    wifi_set_result_to_current_running_queue(WIFI_RETURN_ENUM_BUSY);
                }
                /* No carrier */
                else if (0 == strcmp((const char* )p_response_buff, WIFI_RETURN_TEXT_NOCARRIER))
                {
                    wifi_set_result_to_current_running_queue(WIFI_RETURN_ENUM_NOCARRIER);
                }
                /* No answer */
                else if (0 == strcmp((const char* )p_response_buff, WIFI_RETURN_TEXT_NOANSWER))
                {
                    wifi_set_result_to_current_running_queue(WIFI_RETURN_ENUM_ERROR);
                }
                /* OK */
                else if (0 == strcmp((const char* )p_response_buff, WIFI_RETURN_TEXT_OK))
                {
                    wifi_set_result_to_current_running_queue(WIFI_RETURN_ENUM_OK);
                }
                else
                {
                    ; /* Do nothing */
                }
                if (p_queue->at_command_id == WIFI_COMMAND_SET_SYSFALSH_READ_DATA)
                {
                    /* check certificate */
                    wifi_analyze_get_certificate_string(p_response_buff);
                }

                memset(p_response_buff, 0, g_wifi_uart[command_port].respbuf_size);
                g_wifi_response_recv_count = 0;
                g_wifi_response_last_string_recv_count = 0;
                g_wifi_response_recv_status = 0x0000;
            }
        }
        else
        {
            ; /* Do nothing */
        }
        if (0 != g_wifi_sci_err_flag)
        {
            switch (g_wifi_sci_err_flag)
            {
                case 1:
                    event.event = WIFI_EVENT_SERIAL_RXQ_OVF_ERR;
                    break;
                case 2:
                    event.event = WIFI_EVENT_SERIAL_OVF_ERR;
                    break;
                case 3:
                    event.event = WIFI_EVENT_SERIAL_FLM_ERR;
                    break;
                default:
                    break;
            }
            g_wifi_sci_err_flag = 0;
            if (NULL != p_wifi_callback)
            {
                p_wifi_callback(&event);
            }
        }
        for (i = 0; i < g_wifi_createble_sockets; i++ )
        {
            if (g_wifi_socket[i].put_error_count > 0)
            {
                g_wifi_socket[i].put_error_count = 0;
                event.event = WIFI_EVENT_SOCKET_RXQ_OVF_ERR;
                event.socket_number = i;
                if (NULL != p_wifi_callback)
                {
                    p_wifi_callback(&event);
                }
            }

        }
    }
}
/**********************************************************************************************************************
 * End of function wifi_recv_task
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_ipaddress_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_ipaddress_string(uint8_t * pstring)
{
    int scanf_ret;
    uint32_t ipaddr[4];
    uint32_t subnetmask[4];
    uint32_t gateway[4];

    /* IP address */
    scanf_ret = sscanf((const char*) pstring, "IP:%d.%d.%d.%d, Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\r\n",
            &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3],
            &subnetmask[0], &subnetmask[1], &subnetmask[2], &subnetmask[3],
            &gateway[0], &gateway[1], &gateway[2], &gateway[3]);
    if (12 == scanf_ret)
    {
        g_wifi_ipconfig.ipaddress = WIFI_IPV4BYTE_TO_ULONG(ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        g_wifi_ipconfig.subnetmask = WIFI_IPV4BYTE_TO_ULONG(subnetmask[0], subnetmask[1], subnetmask[2], subnetmask[3]);
        g_wifi_ipconfig.gateway = WIFI_IPV4BYTE_TO_ULONG(gateway[0], gateway[1], gateway[2], gateway[3]);
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_ipaddress_string
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_get_macaddress_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_get_macaddress_string(uint8_t * pstring)
{
    int scanf_ret;
    uint32_t macaddr[6];

    /* mac address */
    scanf_ret = sscanf((const char *)pstring, "Mac Addr     =   %2x:%2x:%2x:%2x:%2x:%2x\r\n",
            &macaddr[0], &macaddr[1], &macaddr[2], &macaddr[3], &macaddr[4], &macaddr[5]);

    if (6 == scanf_ret)
    {
        g_wifi_macaddress[0] = macaddr[0];
        g_wifi_macaddress[1] = macaddr[1];
        g_wifi_macaddress[2] = macaddr[2];
        g_wifi_macaddress[3] = macaddr[3];
        g_wifi_macaddress[4] = macaddr[4];
        g_wifi_macaddress[5] = macaddr[5];
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_get_macaddress_string
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_get_aplist_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_get_aplist_string(uint8_t * pstring)
{
    int scanf_ret;
    uint32_t bssid[6];
    uint32_t channel;
    int32_t indicator;
    char * pstr1;
    char * pstr2;

    /* AP list */
    if (0 == strncmp((char *)pstring, "ssid = ", 7))
    {
        pstr1 = (char *)(pstring + 7);              /* AP list length */
        pstr2 = strchr((const char *)pstr1, '\r');  /* Search \r      */
        if (NULL != pstr2)
        {
            *pstr2 = '\0';
            if (strlen(pstr2) <= 32)
            {
                if (g_wifi_aplist_subcount >= 1)
                {
                    g_wifi_aplist_stored_num++;
                    gp_wifi_ap_results++;
                }
                if (g_wifi_aplist_stored_num < g_wifi_aplistmax)
                {
                    /* ssid */
                    strcpy((char *)(gp_wifi_ap_results->ssid), pstr1);
                }
                g_wifi_aplist_subcount = 1;
            }
        }
    }
    /* bssid */
    else if (0 == strncmp((char *)pstring, "bssid =", 7))
    {
        /* bssid string */
        scanf_ret = sscanf((char *)pstring, "bssid = %2x:%2x:%2x:%2x:%2x:%2x\r\n",
                &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]);
        if (6 == scanf_ret)
        {
            if (g_wifi_aplist_subcount >= 2)
            {
                g_wifi_aplist_stored_num++;
                gp_wifi_ap_results++;
            }
            if (g_wifi_aplist_stored_num < g_wifi_aplistmax)
            {
                gp_wifi_ap_results->bssid[0] = bssid[0];
                gp_wifi_ap_results->bssid[1] = bssid[1];
                gp_wifi_ap_results->bssid[2] = bssid[2];
                gp_wifi_ap_results->bssid[3] = bssid[3];
                gp_wifi_ap_results->bssid[4] = bssid[4];
                gp_wifi_ap_results->bssid[5] = bssid[5];
            }
            g_wifi_aplist_subcount = 2;

        }
    }
    /* channel string */
    else if (0 == strncmp((char *)pstring, "channel =", 9))
    {
        /* channel string */
        scanf_ret = sscanf((const char *)pstring, "channel = %d\r\n", &channel);
        if (1 == scanf_ret)
        {
            if (g_wifi_aplist_subcount >= 3)
            {
                g_wifi_aplist_stored_num++;
                gp_wifi_ap_results++;
            }
            if (g_wifi_aplist_stored_num < g_wifi_aplistmax)
            {
                gp_wifi_ap_results->channel = channel;
            }
            g_wifi_aplist_subcount = 3;
        }
    }
    /* indicator */
    else if (0 == strncmp((char *)pstring, "indicator =", 11))
    {
        /* indicator string */
        scanf_ret = sscanf((char *)pstring, "indicator = %d\r\n", &indicator);
        if (1 == scanf_ret)
        {
            if (g_wifi_aplist_subcount >= 4)
            {
                g_wifi_aplist_stored_num++;
                gp_wifi_ap_results++;
            }
            if (g_wifi_aplist_stored_num < g_wifi_aplistmax)
            {
                gp_wifi_ap_results->rssi = indicator;
            }
            g_wifi_aplist_subcount = 4;
        }
    }
    /* security = none */
    else if (0 == strncmp((char *)pstring, "security = NONE!", 16))
    {
        if (g_wifi_aplist_subcount >= 5)
        {
            g_wifi_aplist_stored_num++;
            gp_wifi_ap_results++;
        }
        if (g_wifi_aplist_stored_num < g_wifi_aplistmax)
        {
            gp_wifi_ap_results->security = WIFI_SECURITY_OPEN;
        }
        g_wifi_aplist_subcount = 5;
        g_wifi_aplist_count++;
    }
    /* security = WPA */
    else if (0 == strncmp((char *)pstring, "WPA", 3))
    {
        if (g_wifi_aplist_subcount >= 5)
        {
            g_wifi_aplist_stored_num++;
            gp_wifi_ap_results++;
        }
        if (g_wifi_aplist_stored_num < g_wifi_aplistmax)
        {
            gp_wifi_ap_results->security = WIFI_SECURITY_WPA;
        }
        g_wifi_aplist_subcount = 5;
        g_wifi_aplist_count++;
    }
    /* security = RSN/WPA2 */
    else if (0 == strncmp((char *)pstring, "RSN/WPA2", 8))
    {
        if (g_wifi_aplist_subcount >= 5)
        {
            g_wifi_aplist_stored_num++;
            gp_wifi_ap_results++;
        }
        if (g_wifi_aplist_stored_num < g_wifi_aplistmax)
        {
            gp_wifi_ap_results->security = WIFI_SECURITY_WPA2;
        }
        g_wifi_aplist_subcount = 5;
        g_wifi_aplist_count++;
    }
    else
    {
        ; /* Do Nothing */
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_get_aplist_string
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_get_dnsquery_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_get_dnsquery_string(uint8_t * pstring)
{
    uint32_t ipaddr[4];

    /* subcount */
    if (0 == strncmp((char *)pstring, "1\r\n", 3))
    {
        g_wifi_dnsquery_subcount = 1;
    }
    /* dns IP address */
    else if (4 == sscanf((char *)pstring, "%d.%d.%d.%d\r\n", &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3]))
    {
        if (1 == g_wifi_dnsquery_subcount)
        {
            g_wifi_dnsaddress = WIFI_IPV4BYTE_TO_ULONG(ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
            g_wifi_dnsquery_subcount = 2;
        }
    }
    else
    {
        ; /* Do nothing */
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_get_dnsquery_string
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_get_sent_recv_size_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_get_sent_recv_size_string(uint8_t * pstring)
{
    uint32_t recv;
    uint32_t sent;

    /* recv/send size */
    if (2 == sscanf((char *)pstring, "recv=%lu sent=%lu\r\n", (uint8_t*) &recv, &sent))
    {
        g_wifi_atustat_recv = recv;
        g_wifi_atustat_sent = sent;
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_get_sent_recv_size_string
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_get_current_ssid_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_get_current_ssid_string(uint8_t * pstring)
{
    char tag[] = "ssid         =   ";
    char * ptr1;
    char * ptr2;

    /* ssid string */
    if (0 == strncmp((char *)pstring, tag, strlen(tag)))
    {
        /* ssid */
        ptr1 = (char *)pstring + strlen(tag);
        ptr2 = strchr(ptr1, '\r');
        if (NULL == ptr2)
        {
            ptr2 = '\0';
            if (strlen(ptr2) < 32)
            {
                /* current ssid */
                strcpy((char *)g_wifi_current_ssid, ptr2);
            }
        }
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_get_current_ssid_string
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_analyze_get_socket_status_string
 * Description  :
 * Arguments    : pstring
 * Return Value : none
 *********************************************************************************************************************/
static void wifi_analyze_get_socket_status_string(uint8_t * pstring)
{
    char * str_ptr;
    uint16_t i;

    /* focus 1st line "CLOSED", "SOCKET", "BOUND", "LISTEN" or "CONNECTED".*/
    if (('A' <= pstring[0]) && (pstring[0] <= 'Z'))
    {
        /* socket status */
        str_ptr = strchr((const char *)pstring, ',');
        if (NULL != str_ptr)
        {
            str_ptr = (char *)&pstring[0];
            for (i = 0; i < ULPGN_SOCKET_STATUS_MAX; i++ )
            {
                /* socket status */
                if (NULL != strstr((const char *)str_ptr, (const char *)gp_wifi_socket_status_tbl[i]))
                {
                    break;
                }
            }
            if (i < ULPGN_SOCKET_STATUS_MAX)
            {
                g_wifi_socket_status = i;
            }
        }
    }
}
/**********************************************************************************************************************
 * End of function wifi_analyze_get_socket_status_string
 *********************************************************************************************************************/
