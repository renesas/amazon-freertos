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
#include "iot_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

/* WiFi configuration includes. */
//#include "platform.h"
//#include "r_sci_rx_if.h"
//#include "r_wifi_sx_ulpgn_if.h"
//#include "FreeRTOSIPConfig.h"
//
#include "r_os_abstraction_api.h"
#include "sx_netbuf.h"
#include "sx_sdmac.h"
#include "wlan_config.h"
#include "qca937x/qca937x_if.h"
/**
 * @brief Wi-Fi initialization status.
 */
#define CONNECT_SCAN_RETRY_MAX  (5)
#define CONNECT_SCAN_BUF        (20)

#define WIFI_CMD_ESSID_OFFSET       0
#define WIFI_CMD_FREQ_LIST_OFFSET   1
#define WIFI_CMD_AUTH_OFFSET        2
#define WIFI_CMD_CIPHER_OFFSET      3
#define WIFI_CMD_KEY_OFFSET         4

static const int const security_list[]={1 /* NONE */,2 /* WEP-OPEN */, 4 /* WPA_PSK */,5 /* WPA2_PSK */ ,6 /* WPA_WPA2_PSK */};
static uint32_t* pmutex = NULL;
static uint32_t xWIFIConnected = 0;
extern int connection_mode_open;

/*-----------------------------------------------------------*/
static int32_t _wifi_disconnect(void)
{
    int32_t ret;
    /* disconnect */
    FreeRTOS_NetworkDown();
    ret = cmd_disconnect(0,NULL,NULL);
    xWIFIConnected = 0;
    
    return ret;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    /* FIX ME. */
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    if(pmutex == NULL)
    {
        pmutex = R_OS_MutexCreate();
    }
    sx_cli_setup();
    if(0 == cmd_wifi_init(0,NULL,NULL))
    {
        xRetVal = eWiFiSuccess;
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    _wifi_disconnect();
    sx_deinit_wifi();
    if(pmutex)
    {
        R_OS_MutexDelete(&pmutex);
        pmutex = NULL;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

static int get_auth_mode(int security)
{
    int auth_mode = WC_FLAG_ENCODE_OPEN;

    switch (security)
    {
    case 3 :
        auth_mode = WC_FLAG_ENCODE_RESTRICTED;
        break;
    case 4 :
        auth_mode = WC_FLAG_ENCODE_WPA;
        break;
    case 5 :
        auth_mode = WC_FLAG_ENCODE_RSN;
        break;
    case 6 :
        auth_mode = WC_FLAG_ENCODE_WPA | WC_FLAG_ENCODE_RSN;
        break;
    default:
        auth_mode = WC_FLAG_ENCODE_OPEN;
        break;
    }

    return auth_mode;
}
/*-----------------------------------------------------------*/
static int loc_cmd_wifi_connect( const WIFINetworkParams_t * const pxNetworkParams )
{
    unsigned int length, auth_mode, key_length,user_retry_count;
    int securityMode, encryption = 0;
    char *freqlist = NULL;
    struct net_device *wifi_ip_ptr;

    wifi_ip_ptr = sx_get_net_dev(WIFI_INTERFACE_NAME);

    length = strlen(pxNetworkParams->pcSSID);
    if ((length == 0) || (length > 32))
    {
        return -1;
    }

    securityMode = security_list[pxNetworkParams->xSecurity];
    if (securityMode == 1)
    {
        connection_mode_open = 1;
    }
    else
    {
        connection_mode_open = 0;
    }

    if (securityMode >= 4)
    {
        encryption = WC_AUTH_CIPHER_TKIP | WC_AUTH_CIPHER_CCMP;
    }
    else if (securityMode >= 2)
    {
        key_length = strlen(pxNetworkParams->pcPassword);
        encryption = WC_AUTH_CIPHER_TKIP | WC_AUTH_CIPHER_CCMP;
    }

    if ((securityMode < 0) || (encryption < 0))
    {
        return -1;
    }
    auth_mode = get_auth_mode(securityMode);

    user_retry_count = 5;

    wpa_cli_connect(wifi_ip_ptr, auth_mode, encryption, pxNetworkParams->pcSSID, pxNetworkParams->pcPassword, freqlist, user_retry_count);
    
    user_retry_count = 30;
    while(user_retry_count)
    {
        /* linked */
        if (wifi_ip_ptr->link_state == 1)
        {
            return 0;
        }
        R_OS_TaskSleep(1000);
        user_retry_count--;
    }
    /* time-out */
    wpa_cli_disconnect(wifi_ip_ptr);
    
    return -1;
}
/*-----------------------------------------------------------*/
WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    int32_t ret = 0;
    WIFIReturnCode_t xret_val = eWiFiFailure;
    uint8_t pucIPAddr[4] = {0};

    if((pmutex==NULL)||(pxNetworkParams==NULL))
    {
        printf("WIFI_ConnectAP error(%d)\n\r",__LINE__);
        ;
    }
    else if(pxNetworkParams->pcSSID==NULL)
    {
        printf("WIFI_ConnectAP error(%d)\n\r",__LINE__);
        ;
    }
    else if((pxNetworkParams->pcPassword == NULL) && (pxNetworkParams->xSecurity != eWiFiSecurityOpen))
    {
        printf("WIFI_ConnectAP error(%d)\n\r",__LINE__);
        ;
    }
    else
    {
        uint32_t SSID_len = strlen(pxNetworkParams->pcSSID);
        uint32_t PASS_len = strlen(pxNetworkParams->pcPassword);
        if ((SSID_len <= 32) && (8 <= PASS_len) && (PASS_len <=64))
        {
            wc_bss_info* bss_list;
            void *wifi_ip_ptr = NULL;
            R_OS_MutexAcquire(pmutex);
            /* If already connected, disconnect existing connection and connect new one */
            if( xWIFIConnected )
            {
                _wifi_disconnect();
            }
            /* connect */
            ret = loc_cmd_wifi_connect(pxNetworkParams);
            if(ret == 0)
            {
                /* stack init */
                if( sx_netdev_stack_init() < 0)
                {
                    printf("sx_netdev_stack_init failed\n");
                }
                else
                {
                    while( (FreeRTOS_IsNetworkUp()==pdFALSE) || (pucIPAddr[0] == 0) )
                    {
                        WIFI_GetIP(pucIPAddr);
                        R_OS_TaskSleep(1);
                    }
                    xWIFIConnected = 1;
                    xret_val = eWiFiSuccess;
                    printf("Connected:%s\n",pxNetworkParams->pcSSID);
                }
            }
            else
            {
                printf("WIFI_ConnectAP error(%d)\n\r",__LINE__);
            }
            R_OS_MutexRelease(pmutex);
        }
        else
        {
            printf("WIFI_ConnectAP error(%d)\n\r",__LINE__);
            printf("SSID_len=%d\n\r",SSID_len);
            printf("PASS_len=%d\n\r",PASS_len);
            printf("SSID=%s\n\r",pxNetworkParams->pcSSID);
            printf("PASS=%s\n\r",pxNetworkParams->pcPassword);
        }
    }
    return xret_val;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    /* FIX ME. */
    int32_t ret;
    if(pmutex)
    {
        R_OS_MutexAcquire(pmutex);
    }
    ret = _wifi_disconnect();
    if(pmutex)
    {
        R_OS_MutexRelease(pmutex);
    }
    if(ret == 0)
    {
        return eWiFiSuccess;
    }

    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    /* FIX ME. */
    WIFIReturnCode_t ret;
    
    WIFI_Off();
    ret = WIFI_On();
    return ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    WIFIReturnCode_t xret_val = eWiFiFailure;
    int i = 0, count = ucNumNetworks;
    void *wifi_ip_ptr = NULL;
    wc_bss_info *bss_list;
    
    wifi_ip_ptr = sx_get_net_dev(WIFI_INTERFACE_NAME);
    if ((pxBuffer) && (wifi_ip_ptr) && (0 < count))
    {
        wc_bss_info *bss_list;
        bss_list = (wc_bss_info *)R_OS_Malloc(sizeof(wc_bss_info) * count, 0);

        if (bss_list)
        {
            wc_set_scan(wifi_ip_ptr);
            /* wait_for_scan_completion */
            R_OS_TaskSleep(3000);
            memset(bss_list, 0, sizeof(wc_bss_info) * count);
            i = wc_site_survey(wifi_ip_ptr, bss_list, count);
            if (i)
            {
                int_t offset;
                int_t len;
                WIFIScanResult_t* p_output = pxBuffer;
                wc_bss_info* p_input = bss_list;

                for (offset = 0; offset < i; offset++)
                {
                    /* SSID */
                    len = p_input->ssid_len;
                    if (len > wificonfigMAX_SSID_LEN)
                    {
                        len = wificonfigMAX_SSID_LEN;
                    }
                    memcpy(p_output->cSSID, p_input->ssid, len);
                
                    /* BSSID */
                    len = WC_MAC_ADDR_LEN;
                    if (len > wificonfigMAX_BSSID_LEN)
                    {
                        len = wificonfigMAX_BSSID_LEN;
                    }
                    memcpy(p_output->ucBSSID, p_input->bssid, len);
                    
                    /* security */
                    p_output->xSecurity = eWiFiSecurityWPA2;
                    
                    /* RSSI */
                    p_output->cRSSI =  p_input->strength;
                    
                    /* Channel */
                    p_output->cChannel =  p_input->channel;
                    
                    /* Hidden */
                    p_output->ucHidden =  p_input->privacy;
                    
                    p_input++;
                    p_output++;
                }
                xret_val = eWiFiSuccess;
            }
            R_OS_TaskSleep(3000);
            R_OS_Free((void**)&bss_list);
        }
    }

    return xret_val;
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
    unsigned char ipv4_addr[SX_NET_IP_STR_LENGTH];
    unsigned char netmask[SX_NET_IP_STR_LENGTH];
    unsigned char gateway[SX_NET_IP_STR_LENGTH];
    unsigned long ipv4_addr_long[4];
    struct net_device *netdev;
    WIFIReturnCode_t xret_val = eWiFiFailure;

    netdev = sx_get_net_dev(WIFI_INTERFACE_NAME);
    if ((pucIPAddr == NULL) || (netdev == NULL))
    {
        ;
    }
    else if (sx_netdev_ipv4_address_get(netdev, ipv4_addr, netmask, gateway) == 0)
    {
        sscanf(ipv4_addr,"%d.%d.%d.%d",&ipv4_addr_long[0],&ipv4_addr_long[1],&ipv4_addr_long[2],&ipv4_addr_long[3]);
        pucIPAddr[0] = (uint8_t)ipv4_addr_long[0];
        pucIPAddr[1] = (uint8_t)ipv4_addr_long[1];
        pucIPAddr[2] = (uint8_t)ipv4_addr_long[2];
        pucIPAddr[3] = (uint8_t)ipv4_addr_long[3];
        xret_val = eWiFiSuccess;
    }
    return xret_val;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    /* FIX ME. */
    struct net_device *netdev;
    WIFIReturnCode_t xret_val = eWiFiFailure;

    netdev = sx_get_net_dev(WIFI_INTERFACE_NAME);
    if ((pucMac == NULL) || (netdev == NULL))
    {
        ;
    }
    else if (wc_get_bssid(netdev, pucMac) == 0)
    {
        xret_val = eWiFiSuccess;
    }
    return xret_val;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    /* FIX ME. */
    WIFIReturnCode_t xret_val = eWiFiFailure;
    if ((pcHost) && (pucIPAddr))
    {
        uint32_t ulIPAddress = FreeRTOS_gethostbyname(pcHost);

        if (ulIPAddress)
        {
            pucIPAddr[0] = ( ( uint8_t ) ( ( ulIPAddress ) & 0xffUL ) );
            pucIPAddr[1] = ( ( uint8_t ) ( ( ( ulIPAddress ) >> 8 ) & 0xffUL ) );
            pucIPAddr[2] = ( ( uint8_t ) ( ( ( ulIPAddress ) >> 16 ) & 0xffUL ) );
            pucIPAddr[3] = ( ( uint8_t ) ( ( ulIPAddress ) >> 24 ) );
            xret_val = eWiFiSuccess;
        }
    }
    return xret_val;
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

WIFIReturnCode_t WIFI_RegisterNetworkStateChangeEventCallback( IotNetworkStateChangeEventCallback_t xCallback )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/


BaseType_t WIFI_IsConnected(void)
{
    /* FIX ME. */
    return FreeRTOS_IsNetworkUp();
}
