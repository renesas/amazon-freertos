/*
 * FreeRTOS V1.1.4
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#include <string.h>
#include "serial_term_uart.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h" /* FIX ME: Delete if you are not using the FreeRTOS-Plus-TCP library. */

/* Test includes */
#include "aws_test_runner.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_secure_sockets.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"
#include "aws_test_tcp.h"

#include "r_cg_macrodriver.h"
#include "r_wifi_sx_ulpgn_if.h"

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 9 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 50 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi 
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME				"RenesasRL78_test" /* FIX ME.*/

/* Static arrays for FreeRTOS-Plus-TCP stack initialization for Ethernet network 
 * connections are declared below. If you are using an Ethernet connection on your MCU 
 * device it is recommended to use the FreeRTOS+TCP stack. The default values are 
 * defined in FreeRTOSConfig.h. */

/* Default MAC address configuration.  The application creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition for information on how to configure
 * the real network connection to use. */
const uint8_t ucMACAddress[ 6 ] =
{
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
};

#if (0)
/* The default IP and MAC address used by the application.  The address configuration
 * defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
 * 1 but a DHCP server could not be contacted.  See the online documentation for
 * more information. */
static const uint8_t ucIPAddress[ 4 ] =
{
    configIP_ADDR0,
    configIP_ADDR1,
    configIP_ADDR2,
    configIP_ADDR3
};
static const uint8_t ucNetMask[ 4 ] =
{
    configNET_MASK0,
    configNET_MASK1,
    configNET_MASK2,
    configNET_MASK3
};
static const uint8_t ucGatewayAddress[ 4 ] =
{
    configGATEWAY_ADDR0,
    configGATEWAY_ADDR1,
    configGATEWAY_ADDR2,
    configGATEWAY_ADDR3
};
static const uint8_t ucDNSServerAddress[ 4 ] =
{
    configDNS_SERVER_ADDR0,
    configDNS_SERVER_ADDR1,
    configDNS_SERVER_ADDR2,
    configDNS_SERVER_ADDR3
};
#endif

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not 
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting 
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Application IP network event hook called by the FreeRTOS+TCP stack for
 * applications using Ethernet. If you are not using Ethernet and the FreeRTOS+TCP stack,
 * start network dependent applications in vApplicationDaemonTaskStartupHook after the
 * network status is up.
 */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent );

/**
 * @brief Connects to Wi-Fi.
 */
static void prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

/*-----------------------------------------------------------*/
extern void Processing_Before_Start_Kernel(void);
extern void main_task(void *pvParameters);

extern wifi_err_t R_WIFI_SX_ULPGN_SetCertificateProfile(uint8_t certificate_id, uint32_t ip_address, const char *server_name);
void prvSetCertificateProfile( void );
void prvFakeSetCertificateProfile(void);
void prvWifiSetCertification(void);

extern const uint8_t sharkSslCAList_PC;
extern const uint32_t sharkSslCAList_PCLength;
extern const uint8_t sharkSslCAList;
extern const uint32_t sharkSslCAListLength;
extern const uint8_t sharkSslRSACert_PC;
extern const uint32_t sharkSslRSACert_PCLength;
extern const uint8_t sharkSslRSACert;
extern const uint32_t sharkSslRSACertLength;

/*-----------------------------------------------------------*/
int putchar (int ch);
void send(unsigned char ch);

/*-----------------------------------------------------------*/


void main( void )
{
    Processing_Before_Start_Kernel();

    vTaskStartScheduler();

    while (1U)
    {
        ;
    }
}

void main_task( void *pvParameters)
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */

    /* Start the scheduler.  Initialization that requires the OS to be running,
     *
     * including the WiFi initialization, is performed in the RTOS daemon task
     * startup hook. */
    // vTaskStartScheduler();

    while(1)
    {
    	vTaskDelay(10000);
    }
}

static void prvMiscInitialization( void )
{
    /* FIX ME: Perform any hardware initializations, that don't require the RTOS to be 
     * running, here.
     */
	uart_config();
	configPRINT_STRING(("Hello World.\r\n"));
    /* Start logging task. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
    /* FIX ME: Perform any hardware initialization, that require the RTOS to be
     * running, here. */
    
	prvMiscInitialization();

    /* FIX ME: If your MCU is using Wi-Fi, delete surrounding compiler directives to 
     * enable the unit tests and after MQTT, Bufferpool, and Secure Sockets libraries 
     * have been imported into the project. If you are not using Wi-Fi, see the 
     * vApplicationIPNetworkEventHook function. */
    #if 1
        if( SYSTEM_Init() == pdPASS )
        {
            /* Connect to the Wi-Fi before running the tests. */
            prvWifiConnect();

            /* Provision the device with AWS certificate and private key. */
#if(0)
            vDevModeKeyProvisioning();
#endif
            /* Create the task to run unit tests. */
            xTaskCreate( TEST_RUNNER_RunTests_task,
                         "RunTests_task",
                         mainTEST_RUNNER_TASK_STACK_SIZE,
                         NULL,
                         tskIDLE_PRIORITY,
                         NULL );
        }
    #endif /* if 0 */
}
/*-----------------------------------------------------------*/

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    /* FIX ME: If your application is using Ethernet network connections and the 
     * FreeRTOS+TCP stack, delete the surrounding compiler directives to enable the 
     * unit tests and after MQTT, Bufferpool, and Secure Sockets libraries have been 
     * imported into the project. If you are not using Ethernet see the 
     * vApplicationDaemonTaskStartupHook function. */
    #if 0
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* If the network has just come up...*/
    if( eNetworkEvent == eNetworkUp )
    {
        if( ( xTasksAlreadyCreated == pdFALSE ) && ( SYSTEM_Init() == pdPASS ) )
        {
            xTaskCreate( TEST_RUNNER_RunTests_task,
                         "TestRunner",
                         TEST_RUNNER_TASK_STACK_SIZE,
                         NULL,
                         tskIDLE_PRIORITY, NULL );

            xTasksAlreadyCreated = pdTRUE;
        }
    }
    #endif /* if 0 */
}
/*-----------------------------------------------------------*/

void prvWifiConnect( void )
{
    /* FIX ME: Delete surrounding compiler directives when the Wi-Fi library is ported. */
    #if 1
        WIFINetworkParams_t xNetworkParams;
        WIFIReturnCode_t xWifiStatus;
        uint8_t ucTempIp[4] = { 0 };

        xWifiStatus = WIFI_On();

        if( xWifiStatus == eWiFiSuccess )
        {
            configPRINTF( ( "Wi-Fi module initialized. Connecting to AP...\r\n" ) );
        }
        else
        {
            configPRINTF( ( "Wi-Fi module failed to initialize.\r\n" ) );

            /* Delay to allow the lower priority logging task to print the above status. 
             * The while loop below will block the above printing. */
            vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

            while( 1 )
            {
            }
        }

        /* Setup parameters. */
        xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
        xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
        xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
        xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
        xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
        xNetworkParams.cChannel = 0;

#if 1
//    prvWifiSetCertification();
    prvSetCertificateProfile();
#endif

        xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );

        if( xWifiStatus == eWiFiSuccess )
        {
            configPRINTF( ( "Wi-Fi Connected to AP. Creating tasks which use network...\r\n" ) );
            
            xWifiStatus = WIFI_GetIP( ucTempIp );
            if ( eWiFiSuccess == xWifiStatus ) 
            {
                configPRINTF( ( "IP Address acquired %d.%d.%d.%d\r\n",
                                ucTempIp[ 0 ], ucTempIp[ 1 ], ucTempIp[ 2 ], ucTempIp[ 3 ] ) );
            }
        }
        else
        {
            configPRINTF( ( "Wi-Fi failed to connect to AP.\r\n" ) );

            /* Delay to allow the lower priority logging task to print the above status. 
             * The while loop below will block the above printing. */
            vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

            while( 1 )
            {
            }
        }
    #endif /* if 0 */
}
/*-----------------------------------------------------------*/

#if !(defined(ENABLE_UNIT_TESTS) || defined(AMAZON_FREERTOS_ENABLE_UNIT_TESTS))
/**
 * @brief This is to provide memory that is used by the Idle task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() in order to provide memory to
 * the Idle task.
 */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/**
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then application must provide an
 * implementation of vApplicationGetTimerTaskMemory() in order to provide memory
 * to the RTOS daemon/time task.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/
#endif

#if(0)
/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook()
{
    /* The TCP tests will test behavior when the entire heap is allocated. In
     * order to avoid interfering with those tests, this function does nothing. */
}
/*-----------------------------------------------------------*/

/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/
#endif

/**
 * @brief User defined Idle task function.
 *
 * @note Do not make any blocking operations in this function.
 */
void vApplicationIdleHook( void )
{
    /* FIX ME. If necessary, update to application idle periodic actions. */

    static TickType_t xLastPrint = 0;
    TickType_t xTimeNow;
    const TickType_t xPrintFrequency = pdMS_TO_TICKS( 5000 );

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastPrint ) > xPrintFrequency )
    {
        configPRINT( "." );
        xLastPrint = xTimeNow;
    }
}
/*-----------------------------------------------------------*/

/**
* @brief User defined application hook to process names returned by the DNS server.
*/
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )
    BaseType_t xApplicationDNSQueryHook( const char * pcName )
    {
        /* FIX ME. If necessary, update to applicable DNS name lookup actions. */

        BaseType_t xReturn;

        /* Determine if a name lookup is for this node.  Two names are given
         * to this node: that returned by pcApplicationHostnameHook() and that set
         * by mainDEVICE_NICK_NAME. */
        if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )
        {
            xReturn = pdPASS;
        }
        else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
        {
            xReturn = pdPASS;
        }
        else
        {
            xReturn = pdFAIL;
        }

        return xReturn;
    }
	
#endif /* if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) */
/*-----------------------------------------------------------*/

#if !(defined(ENABLE_UNIT_TESTS) || defined(AMAZON_FREERTOS_ENABLE_UNIT_TESTS))
/**
 * @brief User defined assertion call. This function is plugged into configASSERT.
 * See FreeRTOSConfig.h to define configASSERT to something different.
 */
void vAssertCalled(const char * pcFile,
	uint32_t ulLine)
{
    /* FIX ME. If necessary, update to applicable assertion routine actions. */

	const uint32_t ulLongSleep = 1000UL;
	volatile uint32_t ulBlockVariable = 0UL;
	volatile char * pcFileName = (volatile char *)pcFile;
	volatile uint32_t ulLineNumber = ulLine;

	(void)pcFileName;
	(void)ulLineNumber;

	printf("vAssertCalled %s, %ld\n", pcFile, (long)ulLine);
	fflush(stdout);

	/* Setting ulBlockVariable to a non-zero value in the debugger will allow
	* this function to be exited. */
	taskDISABLE_INTERRUPTS();
	{
		while (ulBlockVariable == 0UL)
		{
			vTaskDelay( pdMS_TO_TICKS( ulLongSleep ) );
		}
	}
	taskENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/
#endif

/**
 * @brief User defined application hook need by the FreeRTOS-Plus-TCP library.
 */
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    const char * pcApplicationHostnameHook(void)
    {
        /* FIX ME: If necessary, update to applicable registration name. */

        /* This function will be called during the DHCP: the machine will be registered 
         * with an IP address plus this name. */
        return clientcredentialIOT_THING_NAME;
    }

#endif

void prvSetCertificateProfile(void)
{
    uint32_t ipaddress;

    ipaddress = SOCKETS_inet_addr_quick(tcptestECHO_SERVER_TLS_ADDR3, tcptestECHO_SERVER_TLS_ADDR2, tcptestECHO_SERVER_TLS_ADDR1, tcptestECHO_SERVER_TLS_ADDR0);
    R_WIFI_SX_ULPGN_SetCertificateProfile(0, ipaddress, NULL);
    R_WIFI_SX_ULPGN_SetCertificateProfile(1, 0, (const char*)clientcredentialMQTT_BROKER_ENDPOINT);
}

void prvFakeSetCertificateProfile(void)
{
    uint32_t ipaddress;

    ipaddress = SOCKETS_inet_addr_quick(tcptestECHO_SERVER_TLS_ADDR3, tcptestECHO_SERVER_TLS_ADDR2, tcptestECHO_SERVER_TLS_ADDR1, tcptestECHO_SERVER_TLS_ADDR0);
    R_WIFI_SX_ULPGN_SetCertificateProfile(1, ipaddress, NULL);
    R_WIFI_SX_ULPGN_SetCertificateProfile(0, 0, (const char*)clientcredentialMQTT_BROKER_ENDPOINT);
}

void prvWifiSetCertification(void)
{
	wifi_certificate_infomation_t certificate_information;
	wifi_certificate_infomation_t *pcertificate_information;
	pcertificate_information =(wifi_certificate_infomation_t*)&certificate_information;

//	uint8_t cert_number;
	/* Get Initial Server Certificate Information */
	R_WIFI_SX_ULPGN_GetServerCertificate(pcertificate_information);

#if 0
	/* Erase All Certificate */
	R_WIFI_SX_ULPGN_EraseAllServerCertificate();
	/* Get Server Certificate Information */
	R_WIFI_SX_ULPGN_GetServerCertificate(pcertificate_information);
	/* Erase All Certificate */
	R_WIFI_SX_ULPGN_EraseAllServerCertificate();
#endif

	R_WIFI_SX_ULPGN_EraseAllServerCertificate();
	R_WIFI_SX_ULPGN_GetServerCertificate(pcertificate_information);
#if 0
	if(certificate_information->certificate_number!=0)
	{
		printf("Certificate_file was not Erase\t\n");
	}
	else
	{
		printf("Certificate_file was Erased\t\n");
	}
#endif

	/* Write 2Set Certificate */
	/* Secure Echo Server Certificate cert0.crt(sharkSslRSACert_PC),calist0.crt(sharkSslCAList_PC) */
	/* AWS Broker         Certificate cert1.crt(sharkSslRSACert)   ,calist1.crt(sharkSslCAList)    */
	R_WIFI_SX_ULPGN_WriteServerCertificate (0,1,(const uint8_t*)&sharkSslRSACert_PC, (uint32_t)sharkSslRSACert_PCLength);
	R_WIFI_SX_ULPGN_WriteServerCertificate (0,0,(const uint8_t*)&sharkSslCAList_PC , (uint32_t)sharkSslCAList_PCLength );
	R_WIFI_SX_ULPGN_WriteServerCertificate (1,1,(const uint8_t*)&sharkSslRSACert   , (uint32_t)sharkSslRSACertLength   );
	R_WIFI_SX_ULPGN_WriteServerCertificate (1,0,(const uint8_t*)&sharkSslCAList    , (uint32_t)sharkSslCAListLength    );

#if 0
	/* Get Updated Server Certificate Information */
	R_WIFI_SX_ULPGN_GetServerCertificate(pcertificate_information);
	if(pcertificate_information->certificate_number!=0)
	{
		printf("File Number = %d\r\n",pcertificate_information->certificate_number);
		while (pcertificate_information->certificate_file[0]!=0)
		{
			printf("%s\r\n",(char*)pcertificate_information->certificate_file);
			pcertificate_information = pcertificate_information->next_certificate_name;
		}
	}
	else
	{
		printf("Certificate_file was Erased\t\n");
	}
#endif
}// prvWifiSetCertification

void prvEraseAllCertificateFile(void)
{
	R_WIFI_SX_ULPGN_EraseAllServerCertificate();
}// prvEraseAllCertificateFile

void prvWriteAllCertificateFile(void)
{
	R_WIFI_SX_ULPGN_WriteServerCertificate (0,1,(const uint8_t*)&sharkSslRSACert_PC, (uint32_t)sharkSslRSACert_PCLength);
	R_WIFI_SX_ULPGN_WriteServerCertificate (0,0,(const uint8_t*)&sharkSslCAList_PC , (uint32_t)sharkSslCAList_PCLength );
	R_WIFI_SX_ULPGN_WriteServerCertificate (1,1,(const uint8_t*)&sharkSslRSACert   , (uint32_t)sharkSslRSACertLength   );
	R_WIFI_SX_ULPGN_WriteServerCertificate (1,0,(const uint8_t*)&sharkSslCAList    , (uint32_t)sharkSslCAListLength    );
}// prvWriteAllCertificateFile

int putchar (int ch)
{
    send((unsigned char)ch);	/* 1 byte transmission */
#if 0
	if(ch == '\r')			/* Send CR when LF */
	{
		ch = '\n';
		send((unsigned char)ch);	/* 1 byte transmission */
	}
#endif
	return 0;
}

void send(unsigned char ch)
{
	while((SSR12 & 0x20) != 0)	/* Waiting for empty transmission buffer */
	{
		;
	}
	STMK3 = 1U;		/* disable INTST3 interrupt */
	TXD3 = ch;		/* Write to transmit buffer */
	STMK3 = 0U;		/* enable INTST3 interrupt */
	return;
}
