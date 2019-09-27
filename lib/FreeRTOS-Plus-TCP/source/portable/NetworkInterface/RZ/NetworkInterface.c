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
* File Name    : NetworkInterface.c
* Device(s)    : RX
* Description  : Interfaces FreeRTOS TCP/IP stack to RX Ethernet driver.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 07.03.2018 0.1     Development
**********************************************************************************************************************/
/**********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
**********************************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* Renesas */
#include "r_ether_rza2_if.h"
#include "r_pinset.h"
#include "r_compiler_abstraction_api.h"




/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define R_NOP() R_COMPILER_Nop()
#define ACTIVE_ETHER_CHANNEL ETHER_CHANNEL_1
#define ETHER_BUFSIZE_MIN (60)

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/
extern int32_t callback_ether_regist();

static TaskHandle_t ether_receive_check_task_handle = 0;
static TaskHandle_t ether_link_check_task_handle = 0;

                    /* set NULL as inital value */
static TaskHandle_t xTaskToNotify = NULL;
static TimerHandle_t timer_handle;
static uint32_t timer_id;

static uint32_t tcpudp_time_cnt;


static int16_t send_data( uint8_t *pucBuffer, size_t length );
static BaseType_t initialize_network(void);
static void check_ether_link(void * pvParameters);
static void prvEMACDeferredInterruptHandlerTask( void *pvParameters );
static void clear_all_ether_rx_discriptors(uint32_t event);

/**********************************************************************************************************************
 * Function Name: xNetworkInterfaceInitialise ()
 * Description  : Initialization of Ethernet driver.
 * Arguments    : none
 * Return Value : pdPASS, pdFAIL
 *********************************************************************************************************************/
BaseType_t xNetworkInterfaceInitialise( void )
{
    BaseType_t x_return;

    /*
     * Perform the hardware specific network initialization here using the Ethernet driver library to initialize the
     * Ethernet hardware, initialize DMA descriptors, and perform a PHY auto-negotiation to obtain a network link.
     *
     * InitialiseNetwork() uses Ethernet peripheral driver library function, and returns 0 if the initialization fails.
     */
    if( initialize_network() == pdFALSE )
    {
        /* returns FAIL */
        x_return = pdFAIL;
    }
    else
    {
        /* returns PASS */
        x_return = pdPASS;
    }

    return x_return;
} /* End of function xNetworkInterfaceInitialise() */


/**********************************************************************************************************************
 * Function Name: xNetworkInterfaceOutput ()
 * Description  : Simple network output interface.
 * Arguments    : pxDescriptor, xReleaseAfterSend
 * Return Value : pdTRUE, pdFALSE
 *********************************************************************************************************************/
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor, BaseType_t xReleaseAfterSend )
{
    /* Simple network interfaces (as opposed to more efficient zero copy network
    interfaces) just use Ethernet peripheral driver library functions to copy
    data from the FreeRTOS+TCP buffer into the peripheral driver's own buffer.
    This example assumes send_data() is a peripheral driver library function that
    takes a pointer to the start of the data to be sent and the length of the
    data to be sent as two separate parameters.  The start of the data is located
    by pxDescriptor->pucEthernetBuffer.  The length of the data is located
    by pxDescriptor->xDataLength. */
    if(0  > send_data( pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength ))
    {
        vReleaseNetworkBufferAndDescriptor( pxDescriptor );

        /* set FALSE */
        return pdFALSE;
    }

    /* Call the standard trace macro to log the send event. */
    iptraceNETWORK_INTERFACE_TRANSMIT();

    /* check argument */
    if( xReleaseAfterSend != pdFALSE )
    {
        /* It is assumed send_data() copies the data out of the FreeRTOS+TCP Ethernet
        buffer.  The Ethernet buffer is therefore no longer needed, and must be
        freed for re-use. */
        vReleaseNetworkBufferAndDescriptor( pxDescriptor );
    }

    /* Succeed */
    return pdTRUE;
} /* End of function xNetworkInterfaceOutput() */


/**********************************************************************************************************************
 * Function Name: prvEMACDeferredInterruptHandlerTask ()
 * Description  : The deferred interrupt handler is a standard RTOS task.
 * Arguments    : pvParameters
 * Return Value : none
 *********************************************************************************************************************/
static void prvEMACDeferredInterruptHandlerTask( void *pvParameters )
{
    NetworkBufferDescriptor_t *p_px_buffer_descriptor;
    int32_t xbytes_received;

    /* Used to indicate that xSendEventStructToIPTask() is being called because of an Ethernet receive event. */
    IPStackEvent_t x_rx_event;

    uint8_t *p_buffer_pointer;

    for( ;; )
    {
        xTaskToNotify = ether_receive_check_task_handle;

        /* Wait for the Ethernet MAC interrupt to indicate that another packet
        has been received.  */
        ulTaskNotifyTake( pdFALSE, portMAX_DELAY );

        while(1)
        {
            /* See how much data was received.  */
            xbytes_received = R_ETHER_Read_ZC2(ACTIVE_ETHER_CHANNEL, (void **)&p_buffer_pointer);

            if( xbytes_received < 0 )
            {
                /* This is an error. Ignored. */
                break;
            }
            else if( xbytes_received > 0 )
            {
                /* Allocate a network buffer descriptor that points to a buffer
                large enough to hold the received frame.  As this is the simple
                rather than efficient example the received data will just be copied
                into this buffer. */
                p_px_buffer_descriptor = pxGetNetworkBufferWithDescriptor( (size_t)xbytes_received, 0 );

                /* check the result */
                if( p_px_buffer_descriptor != NULL )
                {
                    /* p_px_buffer_descriptor->pucEthernetBuffer now points to an Ethernet
                    buffer large enough to hold the received data.  Copy the
                    received data into pcNetworkBuffer->pucEthernetBuffer.  Here it
                    is assumed ReceiveData() is a peripheral driver function that
                    copies the received data into a buffer passed in as the function's
                    parameter.  Remember! While is is a simple robust technique -
                    it is not efficient.  An example that uses a zero copy technique
                    is provided further down this page. */
                    memcpy(p_px_buffer_descriptor->pucEthernetBuffer, p_buffer_pointer, (size_t)xbytes_received);
                    #if NOT_USED
                    ReceiveData( p_px_buffer_descriptor->pucEthernetBuffer );
                    #endif

                    /* Set the actual packet length, in case a larger buffer was returned. */
                    p_px_buffer_descriptor->xDataLength = (size_t)xbytes_received;

                    R_ETHER_Read_ZC2_BufRelease(ACTIVE_ETHER_CHANNEL);

                    /* See if the data contained in the received Ethernet frame needs
                    to be processed.  NOTE! It is preferable to do this in
                    the interrupt service routine itself, which would remove the need
                    to unblock this task for packets that don't need processing. */
                    if( eConsiderFrameForProcessing( p_px_buffer_descriptor->pucEthernetBuffer ) == eProcessBuffer )
                    {
                        /* The event about to be sent to the TCP/IP is an Rx event. */
                        x_rx_event.eEventType = eNetworkRxEvent;

                        /* pvData is used to point to the network buffer descriptor that
                        now references the received data. */
                        x_rx_event.pvData = ( void * ) p_px_buffer_descriptor;

                        /* Send the data to the TCP/IP stack. */
                        if( xSendEventStructToIPTask( &x_rx_event, 0 ) == pdFALSE )
                        {
                            /* The buffer could not be sent to the IP task so the buffer must be released. */
                            vReleaseNetworkBufferAndDescriptor( p_px_buffer_descriptor );

                            /* Make a call to the standard trace macro to log the occurrence. */
                            iptraceETHERNET_RX_EVENT_LOST();
                            clear_all_ether_rx_discriptors(0);
                        }
                        else
                        {
                            /* The message was successfully sent to the TCP/IP stack.
                            Call the standard trace macro to log the occurrence. */
                            iptraceNETWORK_INTERFACE_RECEIVE();
                            R_NOP();
                        }
                    }
                    else
                    {
                        /* The Ethernet frame can be dropped, but the Ethernet buffer must be released. */
                        vReleaseNetworkBufferAndDescriptor( p_px_buffer_descriptor );
                    }
                }
                else
                {
                    /* The event was lost because a network buffer was not available.
                    Call the standard trace macro to log the occurrence. */
                    iptraceETHERNET_RX_EVENT_LOST();
                    clear_all_ether_rx_discriptors(1);
                }
            }
            else
            {
                /* nothing is received */
                break;
            }
        }
    }
} /* End of function prvEMACDeferredInterruptHandlerTask() */


/**********************************************************************************************************************
 * Function Name: xApplicationDNSQueryHook ()
 * Description  :
 * Arguments    : pcName string pointer
 * Return Value :
 *********************************************************************************************************************/
BaseType_t xApplicationDNSQueryHook(const char *pcName )
{
    return strcmp( pcName, "RZA2M" ); //TODO complete stub function
} /* End of function xApplicationDNSQueryHook() */


/**********************************************************************************************************************
 * Function Name: vNetworkInterfaceAllocateRAMToBuffers ()
 * Description  : .
 * Arguments    : pxNetworkBuffers
 * Return Value : none
 *********************************************************************************************************************/
void vNetworkInterfaceAllocateRAMToBuffers( 
        NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
    uint32_t ul;
    uint8_t *p_buffer_address;

#if defined(__CCRX__)
    p_buffer_address = __sectop("B_ETHERNET_BUFFERS_1");
#elif defined(__GNUC__)
    extern void *__sectop__ETHERNET_BUFFERS;
    p_buffer_address = __sectop__ETHERNET_BUFFERS;
#elif defined(__ICCRX__)
    p_buffer_address = __section_begin("_ETHERNET_BUFFERS");
#endif

    for( ul = 0; ul < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; ul++ )
    {
        pxNetworkBuffers[ul].pucEthernetBuffer = (p_buffer_address + (ETHER_CFG_BUFSIZE * ul));
    }
}  /* End of function vNetworkInterfaceAllocateRAMToBuffers() */


/**********************************************************************************************************************
 * Function Name: initialize_network ()
 * Description  :
 * Arguments    : none
 * Return Value : pdTRUE, pdFALSE
 *********************************************************************************************************************/
static BaseType_t initialize_network( void )
{
    ether_return_t eth_ret;

    /* set initial value */
    BaseType_t return_code = pdFALSE;
    ether_param_t   param;
    uint8_t myethaddr[6] =
    {
        configMAC_ADDR0,
        configMAC_ADDR1,
        configMAC_ADDR2,
        configMAC_ADDR3,
        configMAC_ADDR4,
        configMAC_ADDR5
    };

    R_ETHER_PinSet_ETHERC_RMII();       /* RZ/A2M RTK */

    R_ETHER_Initial();
    callback_ether_regist();

    param.channel = ETHER_CHANNEL_1;
    eth_ret = R_ETHER_Control(CONTROL_POWER_ON, param);        /* PHY mode settings, module stop cancellation */

    if (ETHER_SUCCESS != eth_ret)
    {
        /* FALSE */
        return pdFALSE;
    }

    eth_ret = R_ETHER_Open_ZC2(param.channel, myethaddr, ETHER_FLAG_OFF);

    if (ETHER_SUCCESS != eth_ret)
    {
        /* FALSE */
        return pdFALSE;
    }

    return_code = xTaskCreate(prvEMACDeferredInterruptHandlerTask,
                                "ETHER_RECEIVE_CHECK_TASK",

                                /* use small stack size */
                                configSMALL_STACK_SIZE,
                                0,
                                configMAX_PRIORITIES,
                                &ether_receive_check_task_handle);

    /* Check the result */
    if (pdFALSE == return_code)
    {
        /* FALSE */
        return pdFALSE;
    }

    do
    {
        eth_ret = R_ETHER_CheckLink_ZC(param.channel);
    } while(ETHER_SUCCESS != eth_ret);

    return_code = xTaskCreate(check_ether_link,
                                "CHECK_ETHER_LINK_TIMER",

                                /* set small stack */
                                configSMALL_STACK_SIZE,
                                0,
                                configMAX_PRIORITIES,
                                &ether_link_check_task_handle);

    /* check the result */
    if (pdFALSE == return_code)
    {
        /* set FALSE */
        return pdFALSE;
    }

    /* set TRUE */
    return pdTRUE;
} /* End of function initialize_network() */


/**********************************************************************************************************************
 * Function Name: send_data ()
 * Description  :
 * Arguments    : pucBuffer, length
 * Return Value : 0 success, negative fail
 *********************************************************************************************************************/
static int16_t send_data( uint8_t *pucBuffer, size_t length )//TODO complete stub function
{
    ether_return_t ret;
    uint8_t * p_write_buffer;
    uint16_t write_buf_size;

    /* (1) Retrieve the transmit buffer location controlled by the  descriptor. */
    ret = R_ETHER_Write_ZC2_GetBuf(ACTIVE_ETHER_CHANNEL, (void **) &p_write_buffer, &write_buf_size);

    if (ETHER_SUCCESS == ret)
    {
        if (write_buf_size >= length)
        {
            /* copy buffer */
            memcpy((void *)p_write_buffer, pucBuffer, length);
        }
        if (length < ETHER_BUFSIZE_MIN)                                         /*under minimum*/
        {
            memset((p_write_buffer + length), 0, (ETHER_BUFSIZE_MIN - length));  /*padding*/
            length = ETHER_BUFSIZE_MIN;                                         /*resize*/
        }

        /* write data */
        ret = R_ETHER_Write_ZC2_SetBuf(ACTIVE_ETHER_CHANNEL, (uint16_t)length);
        ret = R_ETHER_CheckWrite(ACTIVE_ETHER_CHANNEL);
    }

    if (ETHER_SUCCESS != ret)
    {
        return -5;
    }
    else
    {
        return 0;
    }
} /* End of function send_data() */


/**********************************************************************************************************************
* Function Name: EINT_Trig_isr
* Description  : Standard frame received interrupt handler
* Arguments    : ectrl - EDMAC and ETHERC control structure
* Return Value : None
* Note         : This callback function is executed when EINT0 interrupt occurred.
**********************************************************************************************************************/
void EINT_Trig_isr(void *ectrl)
{
    /* set the initial values */
    BaseType_t x_higher_priority_task_woken = pdFALSE;
    ether_cb_arg_t *p_decode;

    /* set temporary pointer */
    p_decode = (ether_cb_arg_t*)ectrl;

    if (p_decode->status_eesr & 0x00040000)// EDMAC FR (Frame Receive Event) interrupt
    {
        /* Check where this function is called from? */
        if(xTaskToNotify != NULL)
        {
            vTaskNotifyGiveFromISR(ether_receive_check_task_handle, &x_higher_priority_task_woken);
        }
        /* If x_higher_priority_task_woken is now set to pdTRUE then a context switch
        should be performed to ensure the interrupt returns directly to the highest
        priority task.  The macro used for this purpose is dependent on the port in
        use and may be called portEND_SWITCHING_ISR(). */
        portYIELD_FROM_ISR( x_higher_priority_task_woken );
    }
} /* End of function EINT_Trig_isr() */


/**********************************************************************************************************************
 * Function Name: check_ether_link ()
 * Description  : Checks status of Ethernet link channel 0. Should be executed on a periodic basis.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void check_ether_link(void * pvParameters)
{
    /* unused argument */
    (void)pvParameters;

    while(1)
    {
        vTaskDelay(1000);
        R_ETHER_LinkProcess(ACTIVE_ETHER_CHANNEL);
    }
} /* End of function check_ether_link() */

/**********************************************************************************************************************
 * Function Name: clear_all_ether_rx_discriptors ()
 * Description  : Clear all Ether RX descriptors
 * Arguments    : event
 * Return Value : none
 *********************************************************************************************************************/
static void clear_all_ether_rx_discriptors(uint32_t event)
{
    int32_t xbytes_received;
    uint8_t *p_buffer_pointer;

    while(1)
    {
        /* See how much data was received.  */
        xbytes_received = R_ETHER_Read_ZC2(ACTIVE_ETHER_CHANNEL, (void **)&p_buffer_pointer);
        if(0 > xbytes_received)
        {
            /* This is an error. Ignored. */
        }
        else if(0 < xbytes_received)
        {
            R_ETHER_Read_ZC2_BufRelease(ACTIVE_ETHER_CHANNEL);
            iptraceETHERNET_RX_EVENT_LOST();
        }
        else
        {
            break;
        }
    }
}

/**********************************************************************************************************************
 End of file "NetworkInterface.c"
 *********************************************************************************************************************/
