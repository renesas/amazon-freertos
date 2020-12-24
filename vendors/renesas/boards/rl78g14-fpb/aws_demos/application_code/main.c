/*
Amazon FreeRTOS
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

#include <string.h>
#include "serial_term_uart.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Version includes. */
#include "aws_application_version.h"

/* Demo includes */
#include "aws_demo.h"
#include "aws_demo_config.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_application_version.h"
//#include "aws_dev_mode_key_provisioning.h"

/* Logging Task Defines. */
//#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 10 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* Unit test defines. */
//#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 8 )

#if(0)
/* Declare the firmware version structure for all to see. */
const AppVersion32_t xAppFirmwareVersion = {
   .u.x.ucMajor = APP_VERSION_MAJOR,
   .u.x.ucMinor = APP_VERSION_MINOR,
   .u.x.usBuild = APP_VERSION_BUILD,
};

/* The MAC address array is not declared const as the MAC address will
normally be read from an EEPROM and not hard coded (in real deployed
applications).*/
static uint8_t ucMACAddress[ 6 ] =
{
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
}; //XXX

/* Define the network addressing.  These parameters will be used if either
ipconfigUDE_DHCP is 0 or if ipconfigUSE_DHCP is 1 but DHCP auto configuration
failed. */
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

/* The following is the address of an OpenDNS server. */
static const uint8_t ucDNSServerAddress[ 4 ] =
{
	configDNS_SERVER_ADDR0,
	configDNS_SERVER_ADDR1,
	configDNS_SERVER_ADDR2,
	configDNS_SERVER_ADDR3
};
#endif

/**
 * @brief Application task startup hook.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Connects to WiFi.
 */
//static void prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );
/*-----------------------------------------------------------*/

extern void Processing_Before_Start_Kernel(void);
extern void main_task(void *pvParameters);

/*-----------------------------------------------------------*/
int __far putchar (int ch);
void send(unsigned char ch);

extern void R_WIFI_SX_ULPGN_SetCertificateProfile(uint8_t certificate_id, uint32_t ipaddress,const char *servername);

static void SX_ULPGN_ConnectingHostInformation( void );

/*-----------------------------------------------------------*/

//sensors_t test_g_sensors_data;

/**
 * @brief Application runtime entry point.
 */
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
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    /* FIX ME. */
	uart_config();
	configPRINT_STRING(("Hello World.\r\n"));
    /* Start logging task. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

}
/*-----------------------------------------------------------*/

/* v.1.4.7の方法で呼出す場合に使用 */
#if(0)
void Demo_Task_Test( void );
extern void vStartTCPEchoClientTasks_SingleTasks2( void );

void Demo_Task_Test( void )
{
    vStartTCPEchoClientTasks_SingleTasks2();
}
#endif

void vApplicationDaemonTaskStartupHook( void )
{
    prvMiscInitialization();

    if( SYSTEM_Init() == pdPASS )
    {
#if(0)
        /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
        are created in the vApplicationIPNetworkEventHook() hook function
        below.  The hook function is called when the network connects. */
        FreeRTOS_IPInit( ucIPAddress,
                         ucNetMask,
                         ucGatewayAddress,
                         ucDNSServerAddress,
                         ucMACAddress );
#endif

    	/* Connect to the wifi before running the demos */
//        prvWifiConnect();

        SX_ULPGN_ConnectingHostInformation();

        /* Provision the device with AWS certificate and private key. */
//        vDevModeKeyProvisioning();

        /* Run all demos. */
        DEMO_RUNNER_RunDemos();
//        Demo_Task_Test();
#if(0)
        /* Create the task to run tests. */
        xTaskCreate( TEST_RUNNER_RunTests_task,
                     "RunTests_task",
                     mainTEST_RUNNER_TASK_STACK_SIZE,
                     NULL,
                     tskIDLE_PRIORITY,
                     NULL );
#endif
    }
}
/*-----------------------------------------------------------*/

#if(0)
void prvWifiConnect( void )
{

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

    xWifiStatus = WIFI_ConnectAP( (const WIFINetworkParams_t *)&( xNetworkParams ) );

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
        /* Connection failed, configure SoftAP. */
        configPRINTF( ( "Wi-Fi failed to connect to AP %s.\r\n", xNetworkParams.pcSSID ) );

        xNetworkParams.pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
        xNetworkParams.pcPassword = wificonfigACCESS_POINT_PASSKEY;
        xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
        xNetworkParams.cChannel = wificonfigACCESS_POINT_CHANNEL;

        configPRINTF( ( "Connect to SoftAP %s using password %s. \r\n",
                      xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );

        while( WIFI_ConfigureAP( &xNetworkParams ) != eWiFiSuccess )
        {
            configPRINTF( ( "Connect to SoftAP %s using password %s and configure Wi-Fi. \r\n",
                          xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );
        }

        configPRINTF( ( "Wi-Fi configuration successful. \r\n" ) );
        while( 1 )
        {
        }
    }
}
#endif
/*-----------------------------------------------------------*/

const char * pcApplicationHostnameHook( void )
{
    /* Assign the name "FreeRTOS" to this network node.  This function will
     * be called during the DHCP: the machine will be registered with an IP
     * address plus this name. */
    return "RenesasRL78_FREERTOS_TCP_TEST";
}
/*-----------------------------------------------------------*/

int __far putchar (int ch)
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
}// putchar

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
}// send

static void SX_ULPGN_ConnectingHostInformation( void )
{
	uint32_t host_addr;

    /* regist cretificate information */
    host_addr =	( ( ( ( uint32_t ) ( configECHO_SERVER_ADDR3 ) ) << 24UL ) |
                ( ( ( uint32_t ) ( configECHO_SERVER_ADDR2 ) ) << 16UL ) |
                ( ( ( uint32_t ) ( configECHO_SERVER_ADDR1 ) ) << 8UL ) |
                ( ( uint32_t ) ( configECHO_SERVER_ADDR0 ) ) );
    R_WIFI_SX_ULPGN_SetCertificateProfile(0, host_addr, NULL);
#if defined(CONFIG_MQTT_DEMO_ENABLED)
    R_WIFI_SX_ULPGN_SetCertificateProfile(1, 0, (const char *)clientcredentialMQTT_BROKER_ENDPOINT);
#else
//    R_WIFI_SX_ULPGN_SetCertificateProfile(1, host_addr, host_name);

    R_WIFI_SX_ULPGN_SetCertificateProfile(1, 0, (const char *)clientcredentialMQTT_BROKER_ENDPOINT);

#endif
    /* ----- */
}// SX_ULPGN_ConnectingHostInformation

