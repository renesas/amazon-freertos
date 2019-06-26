/*
 * Amazon FreeRTOS Wi-Fi V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file aws_wifi.c
 * @brief Wi-Fi Interface.
 */
#include <stdio.h>
#include <string.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* Socket and WiFi interface includes. */
#include "aws_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

/* WiFi configuration includes. */
#include "platform.h"
#include "r_sci_rx_if.h"
#include "esp8266_driver.h"

/**
 * @brief Wi-Fi initialization status.
 */
static BaseType_t xWIFIInitDone;
static uint32_t prvConvertSecurityFromSilexAT( WIFISecurity_t xSecurity );

static uint32_t prvConvertSecurityFromSilexAT( WIFISecurity_t xSecurity )
{
    uint32_t xConvertedSecurityType = ESP8266_SECURITY_UNDEFINED;

    switch( xSecurity )
    {
        case eWiFiSecurityOpen:
            xConvertedSecurityType = ESP8266_SECURITY_OPEN;
            break;

        case eWiFiSecurityWEP:
            xConvertedSecurityType = ESP8266_SECURITY_WEP;
            break;

        case eWiFiSecurityWPA:
            xConvertedSecurityType = ESP8266_SECURITY_WPA;
            break;

        case eWiFiSecurityWPA2:
            xConvertedSecurityType = ESP8266_SECURITY_WPA2;
            break;

        case eWiFiSecurityNotSupported:
            xConvertedSecurityType = ESP8266_SECURITY_UNDEFINED;
            break;
    }

    return xConvertedSecurityType;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    /* FIX ME. */
    WIFIReturnCode_t xRetVal = eWiFiFailure;


    /* One time Wi-Fi initialization */
    if( xWIFIInitDone == pdFALSE )
    {
#if 0
    	/* This buffer is used to store the semaphore's data structure
         * and therefore must be static. */
        static StaticSemaphore_t xSemaphoreBuffer;

        /* Start with all the zero. */
        memset( &( xWiFiModule ), 0, sizeof( xWiFiModule ) );

        /* Create the semaphore used to serialize Wi-Fi module operations. */
        xWiFiModule.xSemaphoreHandle = xSemaphoreCreateMutexStatic( &( xSemaphoreBuffer ) );

        /* Initialize semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
#endif
        /* Wi-Fi init done*/
        xWIFIInitDone = pdTRUE;
    }

    if(0 == esp8266_wifi_init())
    {
    	xRetVal = eWiFiSuccess;
    }

	return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

//eWiFiSecurityOpen = 0, /**< Open - No Security. */
//eWiFiSecurityWEP,      /**< WEP Security. */
//eWiFiSecurityWPA,      /**< WPA Security. */
//eWiFiSecurityWPA2,     /**< WPA2 Security. */

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
	int32_t ret;
	uint32_t convert_security;
	convert_security = prvConvertSecurityFromSilexAT(pxNetworkParams->xSecurity);
	ret = esp8266_wifi_connect(
			pxNetworkParams->pcSSID,
			convert_security,
			pxNetworkParams->pcPassword);
	if(ret != 0)
	{
		return eWiFiFailure;
	}


	return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    /* FIX ME. */
//	esp8266_wifi_get_macaddr();
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected(void)
{
	/* FIX ME. */
	return pdFALSE;
}
