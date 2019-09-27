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

/*******************************************************************************
* File Name    : freertos_usr_func.c
* Version      : 1.0
* Description  : Contains FreeRTOS user-defined functions.
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 30.06.2016 1.00     First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define SIZE_OF_IP_ADDRESS (16)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/************* semaphore initialization *****************/


/************* mutex initialization *********************/


/************** queues initialization *******************/


/************** event groups initialization *************/


/************** mailbox initialization ******************/


/************** memory pool initialization **************/


/************** task initialization *********************/


/* FreeRTOS's system timer. */
void vApplicationSetupTimerInterrupt(void);

/* Hook functions used by FreeRTOS.
 * void vAssertCalled(void);
 * void vApplicationIdleHook(void);
 * void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
 */
void vApplicationTickHook(void);
void vApplicationMallocFailedHook(void);

/* FreeRTOS's processing before start the kernel.
 *void Processing_Before_Start_Kernel(void);
 */

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name: vApplicationSetupTimerInterrupt
* Description  : Initialize system timer for FreeRTOS with tick interrupt 1ms.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vApplicationSetupTimerInterrupt(void)
{
} /* End of function vApplicationSetupTimerInterrupt() */

/******************************************************************************
* Function Name: vAssertCalled
* Description  : This function is used to validate the input parameters.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vAssertCalled(void)
{

} /* End of function vAssertCalled() */

/******************************************************************************
* Function Name: vApplicationTickHook
* Description  : This function will be called every tick interrupt.
*                NOTE: vApplicationTickHook() EXECUTES FROM WITHIN AN ISR,
*                      SO MUST BE VERY SHORT AND NOT USE MUCH STACK.
*                      IN ADDITION, NOT CALL ANY APIs WITHOUT "FromISR" OR
*                      "FROM_ISR" AT THE END.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vApplicationTickHook(void)
{
    /* Implement user-code for user own purpose. */

} /* End of function vApplicationTickHook() */

/******************************************************************************
* Function Name: vApplicationMallocFailedHook
* Description  : This function is to capture the failure while
*                memory allocation.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vApplicationMallocFailedHook(void)
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    taskDISABLE_INTERRUPTS();
    for( ; ; )
    {
        /* Loop here */
    };

} /* End of function vApplicationMallocFailedHook() */

/**********************************************************************************************************************
 * Function Name: vApplicationIPNetworkEventHook
 * Description  : .
 * Arguments    : eNetworkEvent.
 * Return Value : None.
 *********************************************************************************************************************/
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    uint32_t ip_address;
    uint32_t net_mask;
    uint32_t gateway_address;
    uint32_t dns_server_address;
    char buffer[ SIZE_OF_IP_ADDRESS ];

    /* If the network has just come up...*/
    if( eNetworkEvent == eNetworkUp )
    {
        /* Print out the network configuration, which may have come from a DHCP
         * server. */
        FreeRTOS_GetAddressConfiguration(
            &ip_address,
            &net_mask,
            &gateway_address,
            &dns_server_address );

        /* set IP address */
        FreeRTOS_inet_ntoa( ip_address, buffer );
        FreeRTOS_printf( ( "IP Address: %s\n", buffer ) );

        /* set Subnet mask */
        FreeRTOS_inet_ntoa( net_mask, buffer );
        FreeRTOS_printf( ( "Subnet Mask: %s\n", buffer ) );

        /* set Default Gateway */
        FreeRTOS_inet_ntoa( gateway_address, buffer );
        FreeRTOS_printf( ( "Gateway Address: %s\n", buffer ) );

        /* set DNS server address */
        FreeRTOS_inet_ntoa( dns_server_address, buffer );
        FreeRTOS_printf( ( "DNS Server Address: %s\n", buffer ) );
    }
}
