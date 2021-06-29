/*
FreeRTOS
Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
// RX65N Cloud Kit 20200923#include "FreeRTOS_IP.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>	// RX65N Cloud Kit 20200923

/* Renesas. */
#include "serial_term_uart.h"

/* Test application include. */
#include "aws_test_runner.h"

/* Aws Library Includes includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "aws_clientcredential.h"
#include "aws_application_version.h"
#include "aws_dev_mode_key_provisioning.h"
#include "iot_wifi.h"	// RX65N Cloud Kit 20200923

#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 8 )

// RX65N Cloud Kit 20200923 -->>
#define _NM_PARAMS( networkType, networkState )    ( ( ( uint32_t ) networkType ) << 16 | ( ( uint16_t ) networkState ) )

#define _NM_GET_NETWORK_TYPE( params )             ( ( uint32_t ) ( ( params ) >> 16 ) & 0x0000FFFFUL )

#define _NM_GET_NETWORK_STATE( params )            ( ( AwsIotNetworkState_t ) ( ( params ) & 0x0000FFFFUL ) )

#define _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS    ( 1000 )

#define _NM_WIFI_CONNECTION_RETRIES              ( 10 )
// RX65N Cloud Kit 20200923 <<--

extern void main_task(void);

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

/**
 * @brief Application task startup hook.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );
static bool _wifiEnable( void );	// RX65N Cloud Kit 20200923

/*-----------------------------------------------------------*/

/**
 * @brief The application entry point from a power on reset is PowerON_Reset_PC()
 * in resetprg.c.
 */
void main( void )
{
//	nop();
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */

    while(1)
    {
    	main_task();
    }
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    /* Initialize UART for serial terminal. */
	uart_config();

    /* Start logging task. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
    prvMiscInitialization();

    if( SYSTEM_Init() == pdPASS )
    {
#if 0
        /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
        are created in the vApplicationIPNetworkEventHook() hook function
        below.  The hook function is called when the network connects. */
        FreeRTOS_IPInit( ucIPAddress,
                         ucNetMask,
                         ucGatewayAddress,
                         ucDNSServerAddress,
                         ucMACAddress );

        /* We should wait for the network to be up before we run any tests. */
        while( FreeRTOS_IsNetworkUp() == pdFALSE )
        {
            vTaskDelay(300);
        }
        FreeRTOS_printf( ( "The network is up and running\n" ) );
#endif

        _wifiEnable();	// RX65N Cloud Kit 20200923

        /* Provision the device with AWS certificate and private key. */
        vDevModeKeyProvisioning();

        vTaskDelay(10000);	// todo: this is renesas issue.
        /* Create the task to run tests. */
        xTaskCreate( TEST_RUNNER_RunTests_task,
                     "RunTests_task",
                     mainTEST_RUNNER_TASK_STACK_SIZE,
                     NULL,
                     tskIDLE_PRIORITY,
                     NULL );
    }
}
/*-----------------------------------------------------------*/

// RX65N Cloud Kit 20200923 -->>
static bool _wifiConnectAccessPoint( void )
{
    bool ret = false;
    size_t xSSIDLength, xPasswordLength;
    static WIFINetworkParams_t xNetworkParams = { 0 };

    /* Setup WiFi parameters to connect to access point. */
	if( clientcredentialWIFI_SSID != NULL )
	{
		xSSIDLength = strlen( clientcredentialWIFI_SSID );
		if( ( xSSIDLength > 0 ) && ( xSSIDLength <= wificonfigMAX_SSID_LEN ) )
		{
			xNetworkParams.ucSSIDLength = xSSIDLength;
			memcpy( xNetworkParams.ucSSID, clientcredentialWIFI_SSID, xSSIDLength );
		}
		else
		{
			configPRINTF(( "Invalid WiFi SSID configured, empty or exceeds allowable length %d.\n", wificonfigMAX_SSID_LEN ));
		}
	}
	else
	{
		configPRINTF(( "WiFi SSID is not configured.\n" ));
	}

	xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
	if( clientcredentialWIFI_SECURITY == eWiFiSecurityWPA2 )
	{
		if( clientcredentialWIFI_PASSWORD != NULL )
		{
			xPasswordLength = strlen( clientcredentialWIFI_PASSWORD );
			if( ( xPasswordLength > 0 ) && ( xSSIDLength <= wificonfigMAX_PASSPHRASE_LEN ) )
			{
				xNetworkParams.xPassword.xWPA.ucLength = xPasswordLength;
				memcpy( xNetworkParams.xPassword.xWPA.cPassphrase, clientcredentialWIFI_PASSWORD, xPasswordLength );
			}
			else
			{
				configPRINTF(( "Invalid WiFi password configured, empty password or exceeds allowable password length %d.\n", wificonfigMAX_PASSPHRASE_LEN ));
			}
		}
		else
		{
			configPRINTF(( "WiFi password is not configured.\n" ));
		}
	}
	xNetworkParams.ucChannel = 0;

    uint32_t numRetries = _NM_WIFI_CONNECTION_RETRIES;
    uint32_t delayMilliseconds = _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS;


    /* Try to connect to wifi access point with retry and exponential delay */
    do
    {
        if( WIFI_ConnectAP( &( xNetworkParams ) ) == eWiFiSuccess )
        {
            ret = true;
            break;
        }
        else
        {
            if( numRetries > 0 )
            {
                IotClock_SleepMs( delayMilliseconds );
                delayMilliseconds = delayMilliseconds * 2;
            }
        }
    } while( numRetries-- > 0 );

    return ret;
}


static bool _wifiEnable( void )
{
    bool ret = true;

    if( WIFI_On() != eWiFiSuccess )
    {
        ret = false;
    }

    #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
        if( ret == true )
        {
            ret = _wifiConnectAccessPoint();
        }
    #else
        if( ret == true )
        {
            if( IotBleWifiProv_Init() != pdTRUE )
            {
                ret = false;
            }
        }

        if( ret == true )
        {
            if( xWiFiConnectTaskInitialize() != pdTRUE )
            {
                ret = false;
            }
        }
    #endif /* if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 ) */

    return ret;
}
// RX65N Cloud Kit 20200923 <<--
/*-----------------------------------------------------------*/


#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )

const char * pcApplicationHostnameHook( void )
{
    /* Assign the name "FreeRTOS" to this network node.  This function will
     * be called during the DHCP: the machine will be registered with an IP
     * address plus this name. */
    return clientcredentialIOT_THING_NAME;
}

#endif
