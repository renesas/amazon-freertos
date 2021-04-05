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
 * File Name    : r_wifi_sx_ulpgn_queue.c
 * Version      : 1.0
 * Description  : AT command queuing functions definition for SX ULPGN of RX65N.
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

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#include "platform.h"
#endif
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
uint32_t g_wifi_queue_ticket_no;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_init_at_execute_queue
 * Description  :
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void wifi_init_at_execute_queue(void)
{
    memset(g_wifi_at_execute_queue, 0, sizeof(g_wifi_at_execute_queue));
    g_wifi_set_queue_index = 0;
    g_wifi_get_queue_index = 0;
    g_wifi_queue_ticket_no = 0;
}
/**********************************************************************************************************************
 * End of function wifi_init_at_execute_queue
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_set_request_in_queue
 * Description  :
 * Arguments    : command
 *                socket
 * Return Value : ticket_no
 *********************************************************************************************************************/
uint32_t wifi_set_request_in_queue(wifi_command_list_t command, int32_t socket)
{
    uint32_t ticket_no;
    ticket_no = ++g_wifi_queue_ticket_no;
    g_wifi_at_execute_queue[g_wifi_set_queue_index].at_command_id = command;
    g_wifi_at_execute_queue[g_wifi_set_queue_index].socket_number = socket;
    g_wifi_at_execute_queue[g_wifi_set_queue_index].result = WIFI_RETURN_ENUM_PROCESSING;
    g_wifi_at_execute_queue[g_wifi_set_queue_index].ticket_no = ticket_no;
    g_wifi_set_queue_index++;
    if (g_wifi_set_queue_index >= 10)
    {
        g_wifi_set_queue_index = 0;
    }
    return ticket_no;
}
/**********************************************************************************************************************
 * End of function wifi_set_request_in_queue
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_get_current_running_queue
 * Description  :
 * Arguments    : none
 * Return Value : AT execute queue
 *********************************************************************************************************************/
st_at_exec_queue_t* wifi_get_current_running_queue(void)
{
    return &g_wifi_at_execute_queue[g_wifi_get_queue_index];
}
/**********************************************************************************************************************
 * End of function wifi_get_current_running_queue
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_set_result_to_current_running_queue
 * Description  :
 * Arguments    : result
 * Return Value : none
 *********************************************************************************************************************/
void wifi_set_result_to_current_running_queue(wifi_return_code_t result)
{
    if (WIFI_COMMAND_NONE != g_wifi_at_execute_queue[g_wifi_get_queue_index].at_command_id)
    {
        g_wifi_at_execute_queue[g_wifi_get_queue_index].result = result;
        g_wifi_get_queue_index++;
        if (g_wifi_get_queue_index >= 10)
        {
            g_wifi_get_queue_index = 0;
        }
    }
}
/**********************************************************************************************************************
 * End of function wifi_set_result_to_current_running_queue
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wifi_get_result_from_queue
 * Description  :
 * Arguments    : ticket_no
 *                result
 * Return Value : 0  - success
 *                -1 - failed
 *********************************************************************************************************************/
int8_t wifi_get_result_from_queue(uint32_t ticket_no, wifi_return_code_t * result)
{
    uint8_t i;

    for (i = 0; i < 10; i++ )
    {
        if (g_wifi_at_execute_queue[i].ticket_no == ticket_no)
        {
            if ((WIFI_RETURN_ENUM_PROCESSING != g_wifi_at_execute_queue[i].result))
            {
                *result = g_wifi_at_execute_queue[i].result;
                g_wifi_at_execute_queue[i].at_command_id = WIFI_COMMAND_NONE;
                break;
            }
        }
    }
    if (i >= 10)
    {
        return -1;
    }
    return 0;
}
/**********************************************************************************************************************
 * End of function wifi_get_result_from_queue
 *********************************************************************************************************************/
