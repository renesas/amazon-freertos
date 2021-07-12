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
#include <stdio.h>
#include <stdbool.h>	

/* Version includes. */
#include "aws_application_version.h"

/* System init includes. */
#include "iot_system_init.h"

/* Logging includes. */
#include "iot_logging_task.h"

/* Key provisioning includes. */
#include "aws_dev_mode_key_provisioning.h"

/* FreeRTOS+TCP includes. */
//#include "FreeRTOS_IP.h"

/* Demo includes */
#include "platform.h"
#include "aws_demo.h"
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"
#include "iot_wifi.h"	
#include "r_cg_port.h"

#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 8 )

 
#define _NM_PARAMS( networkType, networkState )    ( ( ( uint32_t ) networkType ) << 16 | ( ( uint16_t ) networkState ) )

#define _NM_GET_NETWORK_TYPE( params )             ( ( uint32_t ) ( ( params ) >> 16 ) & 0x0000FFFFUL )

#define _NM_GET_NETWORK_STATE( params )            ( ( AwsIotNetworkState_t ) ( ( params ) & 0x0000FFFFUL ) )

#define _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS    ( 1000 )

#define _NM_WIFI_CONNECTION_RETRIES              ( 10 )
 

#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 8 )

extern void main_task(void);
void R_Config_PORT_Create(void);


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
static bool _wifiEnable( void );	
extern void main_task(void);

/*-----------------------------------------------------------*/

/**
 * @brief The application entry point from a power on reset is PowerON_Reset_PC()
 * in resetprg.c.
 */
void main( void )
{
    while(1)
    {
    	main_task();
    }
}
/*-----------------------------------------------------------*/
static void reboot();
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
    WIFIReturnCode_t Wifistatus;
    if( SYSTEM_Init() == pdPASS )
    {
    	/* Set P43 as output to 0 */
    	R_Config_PORT_Create();

#if 0
    	Wifistatus = WIFI_On();	
		if (Wifistatus == eWiFiSuccess){

			configPRINTF( ( "WiFi module initialized.\r\n" ) );
			WIFI_Off();
		}
		else
		{
			configPRINTF( ( "WiFi module failed to initialize.\r\n" ) );
		}

#endif
		/* We should wait for the main_task before we run any demos. */
		vTaskDelay(300);

        /* Provision the device with AWS certificate and private key. */
        vDevModeKeyProvisioning();

        /* Run all demos. */
        DEMO_RUNNER_RunDemos();
    }
}

 
static bool _wifiConnectAccessPoint( void )
{
	bool status = true;
	WIFINetworkParams_t xConnectParams;
	size_t xSSIDLength, xPasswordLength;
	const char * pcSSID = clientcredentialWIFI_SSID;
	const char * pcPassword = clientcredentialWIFI_PASSWORD;
	WIFISecurity_t xSecurity = clientcredentialWIFI_SECURITY;
	uint32_t numRetries = _NM_WIFI_CONNECTION_RETRIES;
	uint32_t delayMilliseconds = _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS;

	if( pcSSID != NULL )
	{
		xSSIDLength = strlen( pcSSID );

		if( ( xSSIDLength > 0 ) && ( xSSIDLength < wificonfigMAX_SSID_LEN ) )
		{
			xConnectParams.ucSSIDLength = xSSIDLength;
			memset( xConnectParams.ucSSID, 0, wificonfigMAX_SSID_LEN );
			memcpy( xConnectParams.ucSSID, clientcredentialWIFI_SSID, xSSIDLength );
		}
		else
		{
			status = false;
		}
	}
	else
	{
		status = false;
	}

	xConnectParams.xSecurity = xSecurity;

	if( xSecurity == eWiFiSecurityWPA2 )
	{
		if( pcPassword != NULL )
		{
			xPasswordLength = strlen( clientcredentialWIFI_PASSWORD );
	    	configPRINTF( ( "pass length %d \r\n", xPasswordLength) );

			if( ( xPasswordLength > 0 ) && ( xPasswordLength < wificonfigMAX_PASSPHRASE_LEN ) )
			{
				xConnectParams.xPassword.xWPA.ucLength = xPasswordLength;
				memset( xConnectParams.xPassword.xWPA.cPassphrase, 0, wificonfigMAX_PASSPHRASE_LEN );
				memcpy( xConnectParams.xPassword.xWPA.cPassphrase, clientcredentialWIFI_PASSWORD, xPasswordLength );
		    	configPRINTF( ( "pass is:%s. \r\n", xConnectParams.xPassword.xWPA.cPassphrase) );
			}
			else
			{
				status = false;
			}
		}
		else
		{
			status = false;
		}
	}

	if( status == true )
	{
		/* Try to connect to wifi access point with retry and exponential delay */
		do
		{
	    	configPRINTF( ( "Try connecting wifi no %d,%s,%s. \r\n", numRetries, xConnectParams.ucSSID, xConnectParams.xPassword.xWPA.cPassphrase ) );

			if( WIFI_ConnectAP( &( xConnectParams ) ) == eWiFiSuccess )
			{
				configPRINTF( ( "Connect wifi OK on retry no %d.\r\n", numRetries ) );
				break;
			}
			else
			{
				configPRINTF( ( "Connect wifi Failed on retry no %d.\r\n", numRetries ) );
				if( numRetries > 0 )
				{
					IotClock_SleepMs( delayMilliseconds );
					delayMilliseconds = delayMilliseconds * 2;
				}
				else
				{
					status = false;
				}
			}
		} while( numRetries-- > 0 );
	}

	return status;
}

void R_Config_PORT_Create(void)
{
    /* Set PORT4 registers */
    PORT4.PODR.BYTE = _00_Pm3_OUTPUT_0;
    PORT4.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORT4.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORT4.PMR.BYTE = _00_Pm3_PIN_GPIO;
    PORT4.PDR.BYTE = _08_Pm3_MODE_OUTPUT;
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
 

static void reboot() {
    //WDT Control Register (WDTCR)
    WDT.WDTCR.BIT.TOPS = 0;
    WDT.WDTCR.BIT.CKS  = 1;
    WDT.WDTCR.BIT.RPES = 3;
    WDT.WDTCR.BIT.RPSS = 3;
    //WDT Status Register
    WDT.WDTSR.BIT.CNTVAL = 0;
    WDT.WDTSR.BIT.REFEF  = 0;
    WDT.WDTSR.BIT.UNDFF  = 0;
    //WDT Reset Control Register
    WDT.WDTRCR.BIT.RSTIRQS = 1;
    //Non-Maskable Interrupt Enable Register (NMIER)
    ICU.NMIER.BIT.WDTEN    = 0;

    WDT.WDTRR = 0;
    WDT.WDTRR = 0xff;

    while (1); // Wait for Watchdog to kick in
}

/*-----------------------------------------------------------*/
