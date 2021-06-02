/*********************************************************************************************************************
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
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
*********************************************************************************************************************/
/*********************************************************************************************************************
* File Name    : renesas_demo_data_uploader.c
* Version      : 1.00
* Description  : Get sensors data and send to AWS.
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 25.05.2021  1.00    First Release
*********************************************************************************************************************/

/**
 * @file iot_demo_mqtt.c
 * @brief Demonstrates usage of the MQTT library.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* MQTT include. */
#include "iot_mqtt.h"

#include "iot_network_manager_private.h"

/* Demo network handling */
#include "aws_iot_demo_network.h"

/* Required for demo task stack and priority */
#include "aws_demo_config.h"
#include "aws_application_version.h"

/* Shadow include. */
#include "aws_iot_shadow.h"

/* JSON utilities include. */
#include "iot_json_utils.h"

#include "renesas_sensors.h"
#include "r_wifi_sx_ulpgn_if.h"

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration settings.
 */
#ifndef IOT_DEMO_MQTT_TOPIC_PREFIX
    #define IOT_DEMO_MQTT_TOPIC_PREFIX           "iotdemo"
#endif
#ifndef IOT_DEMO_MQTT_PUBLISH_BURST_SIZE
    #define IOT_DEMO_MQTT_PUBLISH_BURST_SIZE     ( 10 )
#endif
#ifndef IOT_DEMO_MQTT_PUBLISH_BURST_COUNT
    #define IOT_DEMO_MQTT_PUBLISH_BURST_COUNT    ( 10 )
#endif
/** @endcond */

/* Validate MQTT demo configuration settings. */
#if IOT_DEMO_MQTT_PUBLISH_BURST_SIZE <= 0
    #error "IOT_DEMO_MQTT_PUBLISH_BURST_SIZE cannot be 0 or negative."
#endif
#if IOT_DEMO_MQTT_PUBLISH_BURST_COUNT <= 0
    #error "IOT_DEMO_MQTT_PUBLISH_BURST_COUNT cannot be 0 or negative."
#endif

/**
 * @brief The first characters in the client identifier. A timestamp is appended
 * to this prefix to create a unique client identifer.
 *
 * This prefix is also used to generate topic names and topic filters used in this
 * demo.
 */
#define CLIENT_IDENTIFIER_PREFIX                 "iotdemo"

/**
 * @brief The longest client identifier that an MQTT server must accept (as defined
 * by the MQTT 3.1.1 spec) is 23 characters. Add 1 to include the length of the NULL
 * terminator.
 */
#define CLIENT_IDENTIFIER_MAX_LENGTH             ( 24 )

/**
 * @brief The keep-alive interval used for this demo.
 *
 * An MQTT ping request will be sent periodically at this interval.
 */
#define KEEP_ALIVE_SECONDS                       ( 1200 )

/**
 * @brief The timeout for MQTT operations in this demo.
 */
#define MQTT_TIMEOUT_MS                          ( 5000 )

/**
 * @brief The Last Will and Testament topic name in this demo.
 *
 * The MQTT server will publish a message to this topic name if this client is
 * unexpectedly disconnected.
 */
#define WILL_TOPIC_NAME                          IOT_DEMO_MQTT_TOPIC_PREFIX "/will"

/**
 * @brief The length of #WILL_TOPIC_NAME.
 */
#define WILL_TOPIC_NAME_LENGTH                   ( ( uint16_t ) ( sizeof( WILL_TOPIC_NAME ) - 1 ) )

/**
 * @brief The message to publish to #WILL_TOPIC_NAME.
 */
#define WILL_MESSAGE                             "MQTT demo unexpectedly disconnected."

/**
 * @brief The length of #WILL_MESSAGE.
 */
#define WILL_MESSAGE_LENGTH                      ( ( size_t ) ( sizeof( WILL_MESSAGE ) - 1 ) )

/**
 * @brief How many topic filters will be used in this demo.
 */
#define TOPIC_FILTER_COUNT                       ( 1 )

/**
 * @brief The length of each topic filter.
 *
 * For convenience, all topic filters are the same length.
 */
#define TOPIC_FILTER_LENGTH                      ( ( uint16_t ) ( sizeof( IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/sensor" ) - 1 ) )

/**
 * @brief Format string of the PUBLISH messages in this demo.
 */
#define PUBLISH_PAYLOAD_FORMAT                   "Hello world %d!"

/**
 * @brief Size of the buffer that holds the PUBLISH messages in this demo.
 */
#define PUBLISH_PAYLOAD_BUFFER_LENGTH            ( sizeof( PUBLISH_PAYLOAD_FORMAT ) + 2 )

/**
 * @brief The maximum number of times each PUBLISH in this demo will be retried.
 */
#define PUBLISH_RETRY_LIMIT                      ( 10 )

/**
 * @brief A PUBLISH message is retried if no response is received within this
 * time.
 */
#define PUBLISH_RETRY_MS                         ( 1000 )

/**
 * @brief The topic name on which acknowledgement messages for incoming publishes
 * should be published.
 */
#define ACKNOWLEDGEMENT_TOPIC_NAME               IOT_DEMO_MQTT_TOPIC_PREFIX "/acknowledgements"

/**
 * @brief The length of #ACKNOWLEDGEMENT_TOPIC_NAME.
 */
#define ACKNOWLEDGEMENT_TOPIC_NAME_LENGTH        ( ( uint16_t ) ( sizeof( ACKNOWLEDGEMENT_TOPIC_NAME ) - 1 ) )

/**
 * @brief Format string of PUBLISH acknowledgement messages in this demo.
 */
#define ACKNOWLEDGEMENT_MESSAGE_FORMAT           "Client has received PUBLISH %.*s from server."

/**
 * @brief Size of the buffers that hold acknowledgement messages in this demo.
 */
#define ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH    ( sizeof( ACKNOWLEDGEMENT_MESSAGE_FORMAT ) + 2 )

#define renesasDemoSENSOR_JSON      \
    "{"                             \
	"\"temperature\": %.2f,"        \
	"\"light\": %.2f,"              \
	"\"humidity\": %.2f,"			\
	"\"pressure\": %.2f,"           \
	"\"accel\": {" 					\
		"\"x\": %.2f, \"y\": %.2f, \"z\": %.2f" \
	"},"							\
	"\"gyro\": {"					\
		"\"x\": %.2f, \"y\": %.2f, \"z\": %.2f" \
    "}"								\
    "}"

/* Maximum size of update JSON documents. */
#define renesasDemoBUFFER_LENGTH              512

/* Stack size for task that handles shadow delta and updates. */
#define renesasDemoUPDATE_TASK_STACK_SIZE     ( ( uint16_t ) configMINIMAL_STACK_SIZE * ( uint16_t ) 5 )

#define DemoCONN_TIMEOUT_MS               ( 2000UL )

#define DemoCONN_RETRY_INTERVAL_MS        ( 5000 )

#define DemoCONN_RETRY_LIMIT              ( 100 )

#define DemoKEEPALIVE_SECONDS             ( 1200 )

#define myappONE_SECOND_DELAY_IN_TICKS    pdMS_TO_TICKS( 1000UL )

#define DemoNETWORK_TYPES                 ( AWSIOT_NETWORK_TYPE_ALL )

/**
 * @brief The keep-alive interval used for this demo.
 *
 * An MQTT ping request will be sent periodically at this interval.
 */
#define KEEP_ALIVE_SECONDS    ( 60 )

/**
 * @brief The timeout for Shadow and MQTT operations in this demo.
 */
#define TIMEOUT_MS            ( 5000 )

/**
 * @brief Format string representing a Shadow document with a "desired" state.
 *
 * Note the client token, which is required for all Shadow updates. The client
 * token must be unique at any given time, but may be reused once the update is
 * completed. For this demo, a timestamp is used for a client token.
 */
#define SHADOW_DESIRED_JSON                \
    "{"                                    \
    "\"state\":{"                          \
    "\"desired\":{"                        \
    "\"LEDControl\": \"LED_OFF\","         \
    "\"SWVersion\": \"VER_0.9.3\","        \
    "\"SensorDataUpdateOn\": \"UpdateOn\"" \
    "}"                                    \
    "},"                                   \
    "\"clientToken\":\"%06lu\""            \
    "}"

/**
 * @brief The expected size of #SHADOW_DESIRED_JSON.
 *
 * Because all the format specifiers in #SHADOW_DESIRED_JSON include a length,
 * its full size is known at compile-time.
 */
#define EXPECTED_DESIRED_JSON_SIZE    ( sizeof( SHADOW_DESIRED_JSON ) )


/* LEDControl status pattern */
#define LED_COMMAND_ON      "LED_ON"
#define LED_COMMAND_LIGHT   "LED_LIGHT"
#define LED_COMMAND_TEMP    "LED_TEMP"
#define LED_COMMAND_OFF     "LED_OFF"

/* SWVersion status pattern */
#define SET_COMMAND_VER     "VER_"

/* SensorDataUpdateOn status pattern */
#define SNS_COMMAND_ON		"UpdateOn"
#define SNS_COMMAND_OFF		"UpdateOff"

/* SWVersion & IPAddress PUBLISH */
#define SHADOW_VER_IP_LIST_JSON  \
    "{"                          \
    "\"SWVersion\": \"%d.%d.%d\","  \
    "\"IPAddress\": \"%d.%d.%d.%d\""\
    "}"

static BaseType_t prxCreateNetworkConnection( void );
static IotNetworkError_t prvNetworkDisconnectCallback( void * pvContext );
static void prvNetworkStateChangeCallback( uint32_t ulNetworkType,
                                           AwsIotNetworkState_t xNetworkState,
                                           void * pvContext );

/**
 * @brief Structure which holds the context for an MQTT connection within Demo.
 */
static MqttConnectionContext_t xConnection =
{
    .pvNetworkConnection = NULL,
    .ulNetworkType       = AWSIOT_NETWORK_TYPE_NONE,
    .xNetworkInfo        = IOT_MQTT_NETWORK_INFO_INITIALIZER,
    .xMqttConnection     = IOT_MQTT_CONNECTION_INITIALIZER,
    .xDisconnectCallback = prvNetworkDisconnectCallback
};

/**
 * @brief Network manager subscription callback.
 */

static IotNetworkManagerSubscription_t xSubscriptionHandle = IOT_NETWORK_MANAGER_SUBSCRIPTION_INITIALIZER;

/**
 * @brief Semaphore to indicate a new network is available.
 */
static SemaphoreHandle_t xNetworkAvailableLock = NULL;

/**
 * @brief Flag used to unset, during disconnection of currently connected network. This will
 * trigger a reconnection from the main MQTT task.
 */
static BaseType_t xNetworkConnected = pdFALSE;

int32_t sensorupdatecurrentState = 1;
int32_t ledcurrentState = 0;
int32_t blinkCnt = 0;
wifi_ip_configuration_t ip_cfg;

/*-----------------------------------------------------------*/

/* Declaration of demo function. */
int RenesasSensorDataUploadDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/
static void turn_on_led (void)
{
    st_sensors_t data;

    if (blinkCnt > 0) {
        PORTB.PDR.BIT.B0 ^= 1;
        PORTB.PDR.BIT.B2 ^= 1;
        blinkCnt--;
    }
    else {
	    R_READ_Sensors(&data);
	    switch(ledcurrentState) {
	    case 2:
	        PORTB.PDR.BIT.B0 = 1;
	        PORTB.PDR.BIT.B2 = 1;
	        break;
	    case 3:
	        PORTB.PDR.BIT.B0 = 0;
	        PORTB.PDR.BIT.B2 = 0;
	        break;
	    case 4:
	        if (data.als >= 500) {
	            PORTB.PDR.BIT.B0 = 0;
	            PORTB.PDR.BIT.B2 = 0;
	        }
	        else if (data.als >= 100) {
	            PORTB.PDR.BIT.B0 = 1;
	            PORTB.PDR.BIT.B2 = 0;
	        }
	        else {
	            PORTB.PDR.BIT.B0 = 1;
	            PORTB.PDR.BIT.B2 = 1;
	        }
	        break;
	    case 5:
			data.temperature = (data.temperature - 32) / 1.8;
	        if (data.temperature <= 30) {
	            PORTB.PDR.BIT.B0 = 0;
	            PORTB.PDR.BIT.B2 = 0;
	        }
	        else if (data.temperature <= 40) {
	            PORTB.PDR.BIT.B0 = 1;
	            PORTB.PDR.BIT.B2 = 0;
	        }
	        else {
	            PORTB.PDR.BIT.B0 = 1;
	            PORTB.PDR.BIT.B2 = 1;
	        }
	        break;
	    default:
	        break;
	    }
    }
}

static IotMqttError_t mpttAckCallBack (void)
{
    IotMqttError_t status;
    int acknowledgementLength = 0;
    char pAcknowledgementMessage[ renesasDemoBUFFER_LENGTH ] = { 0 };
    IotMqttPublishInfo_t acknowledgementInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    char ipAddess[4] = {0,0,0,0};

    /* divide IP adress. */
    ipAddess[0] = (char)((ip_cfg.ipaddress >> 24) & 0x000000FF);
    ipAddess[1] = (char)((ip_cfg.ipaddress >> 16) & 0x000000FF);
    ipAddess[2] = (char)((ip_cfg.ipaddress >> 8 ) & 0x000000FF);
    ipAddess[3] = (char)((ip_cfg.ipaddress      ) & 0x000000FF);

    /* Generate an acknowledgement message. */
    acknowledgementLength = snprintf( pAcknowledgementMessage,
                                      renesasDemoBUFFER_LENGTH,
                                      SHADOW_VER_IP_LIST_JSON,
                                      xAppFirmwareVersion.u.x.ucMajor,
                                      xAppFirmwareVersion.u.x.ucMinor,
                                      xAppFirmwareVersion.u.x.usBuild,
                                      ipAddess[0],
                                      ipAddess[1],
                                      ipAddess[2],
                                      ipAddess[3]);

    /* Set the members of the publish info for the acknowledgement message. */
    acknowledgementInfo.qos = IOT_MQTT_QOS_1;
    acknowledgementInfo.pTopicName = ACKNOWLEDGEMENT_TOPIC_NAME;
    acknowledgementInfo.topicNameLength = ACKNOWLEDGEMENT_TOPIC_NAME_LENGTH;
    acknowledgementInfo.pPayload = pAcknowledgementMessage;
    acknowledgementInfo.payloadLength = ( size_t ) acknowledgementLength;
    acknowledgementInfo.retryMs = PUBLISH_RETRY_MS;
    acknowledgementInfo.retryLimit = PUBLISH_RETRY_LIMIT;

    /* Send the acknowledgement for the received message. This demo program
     * will not be notified on the status of the acknowledgement because
     * neither a callback nor IOT_MQTT_FLAG_WAITABLE is set. However,
     * the MQTT library will still guarantee at-least-once delivery (subject
     * to the retransmission strategy) because the acknowledgement message is
     * sent at QoS 1. */
     status = IotMqtt_Publish( xConnection.xMqttConnection,
                               &acknowledgementInfo,
                               0,
                               NULL,
                               NULL );

     return status;
}

/**
 * @brief Parses a key in the "state" section of a Shadow delta document.
 *
 * @param[in] pDeltaDocument The Shadow delta document to parse.
 * @param[in] deltaDocumentLength The length of `pDeltaDocument`.
 * @param[in] pDeltaKey The key in the delta document to find. Must be NULL-terminated.
 * @param[out] pDelta Set to the first character in the delta key.
 * @param[out] pDeltaLength The length of the delta key.
 *
 * @return `true` if the given delta key is found; `false` otherwise.
 */
static bool _getDelta( const char * pDeltaDocument,
                       size_t deltaDocumentLength,
                       const char * pDeltaKey,
                       const char ** pDelta,
                       size_t * pDeltaLength )
{
    bool stateFound = false, deltaFound = false;
    const size_t deltaKeyLength = strlen( pDeltaKey );
    const char * pState = NULL;
    size_t stateLength = 0;

    /* Find the "state" key in the delta document. */
    stateFound = IotJsonUtils_FindJsonValue( pDeltaDocument,
                                             deltaDocumentLength,
                                             "state",
                                             5,
                                             &pState,
                                             &stateLength );

    if( stateFound == true )
    {
        /* Find the delta key within the "state" section. */
        deltaFound = IotJsonUtils_FindJsonValue( pState,
                                                 stateLength,
                                                 pDeltaKey,
                                                 deltaKeyLength,
                                                 pDelta,
                                                 pDeltaLength );
    }
    else
    {
        IotLogWarn( "Failed to find \"state\" in Shadow delta document." );
    }

    return deltaFound;
}

/*-----------------------------------------------------------*/

/**
 * @brief Parses the "state" key from the "previous" or "current" sections of a
 * Shadow updated document.
 *
 * @param[in] pUpdatedDocument The Shadow updated document to parse.
 * @param[in] updatedDocumentLength The length of `pUpdatedDocument`.
 * @param[in] pSectionKey Either "previous" or "current". Must be NULL-terminated.
 * @param[out] pState Set to the first character in "state".
 * @param[out] pStateLength Length of the "state" section.
 *
 * @return `true` if the "state" was found; `false` otherwise.
 */
static bool _getUpdatedState( const char * pUpdatedDocument,
                              size_t updatedDocumentLength,
                              const char * pSectionKey,
                              const char ** pState,
                              size_t * pStateLength )
{
    bool sectionFound = false, stateFound = false;
    const size_t sectionKeyLength = strlen( pSectionKey );
    const char * pSection = NULL;
    size_t sectionLength = 0;

    /* Find the given section in the updated document. */
    sectionFound = IotJsonUtils_FindJsonValue( pUpdatedDocument,
                                               updatedDocumentLength,
                                               pSectionKey,
                                               sectionKeyLength,
                                               &pSection,
                                               &sectionLength );

    if( sectionFound == true )
    {
        /* Find the "state" key within the "previous" or "current" section. */
        stateFound = IotJsonUtils_FindJsonValue( pSection,
                                                 sectionLength,
                                                 "state",
                                                 5,
                                                 pState,
                                                 pStateLength );
    }
    else
    {
        IotLogWarn( "Failed to find section %s in Shadow updated document.",
                    pSectionKey );
    }

    return stateFound;
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow delta callback, invoked when the desired and updates Shadow
 * states differ.
 *
 * This function simulates a device updating its state in response to a Shadow.
 *
 * @param[in] pCallbackContext Not used.
 * @param[in] pCallbackParam The received Shadow delta document.
 */
static void _shadowDeltaCallback( void * pCallbackContext,
                                  AwsIotShadowCallbackParam_t * pCallbackParam )
{
    bool deltaFound = false;
    const char * pDelta = NULL;
    size_t deltaLength = 0;
    IotSemaphore_t * pDeltaSemaphore = pCallbackContext;
    int updateDocumentLength = 0;
    AwsIotShadowError_t updateStatus = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t updateDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    char cmdBuff[16] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    int i,j;
    int swVer[3] = {0x0,0x0,0x0};
    char seVerIdx = 0;
    char staIdx = 5;
    char endIdx = 0;
    int num = 0;
    bool swVerFix = false;

    /* Stored state. */
    static int32_t currentState = 0;

	/* LEDControl */
    /* Check if there is a different "LEDControl" state in the Shadow. */
    deltaFound = _getDelta( pCallbackParam->u.callback.pDocument,
                            pCallbackParam->u.callback.documentLength,
                            "LEDControl",
                            &pDelta,
                            &deltaLength );

    if( deltaFound == true )
    {
        /* Change the current state based on the value in the delta document. */
        if( *pDelta == '"' )
        {
            for( i = 0; i < ( deltaLength - 2 ) ; i++ ) 
            {
                cmdBuff[i] = *(pDelta + i + 1);
            }

            if( 0 == strcmp(cmdBuff,LED_COMMAND_ON))
            {
                IotLogInfo( "%.*s changing state from %d to 2.",
                            pCallbackParam->thingNameLength,
                            pCallbackParam->pThingName,
                            currentState );

                currentState = 2;
                ledcurrentState = currentState;
            }
            else if( 0 == strcmp(cmdBuff,LED_COMMAND_OFF))
            {
                IotLogInfo( "%.*s changing state from %d to 3.",
                            pCallbackParam->thingNameLength,
                            pCallbackParam->pThingName,
                            currentState );

                currentState = 3;
                ledcurrentState = currentState;
            }
            else if( 0 == strcmp(cmdBuff,LED_COMMAND_LIGHT))
            {
                IotLogInfo( "%.*s changing state from %d to 4.",
                            pCallbackParam->thingNameLength,
                            pCallbackParam->pThingName,
                            currentState );

                currentState = 4;
                ledcurrentState = currentState;
            }
            else if( 0 == strcmp(cmdBuff,LED_COMMAND_TEMP))
            {
                IotLogInfo( "%.*s changing state from %d to 5.",
                            pCallbackParam->thingNameLength,
                            pCallbackParam->pThingName,
                            currentState );

                currentState = 5;
                ledcurrentState = currentState;
            }
            else
            {
                IotLogWarn( "Unknown LEDControl state parsed from delta document." );
            }
        }
        else
        {
            IotLogWarn( "Unknown LEDControl state parsed from delta document." );
        }
    }
    else{
        IotLogWarn( "Failed to parse LEDControl state from delta document." );
    }

	/* SWVersion */
    /* Check if there is a different "SWVersion" state in the Shadow. */
    deltaFound = _getDelta( pCallbackParam->u.callback.pDocument,
                            pCallbackParam->u.callback.documentLength,
                            "SWVersion",
                            &pDelta,
                            &deltaLength );

    if( deltaFound == true )
    {
        /* Change the current state based on the value in the delta document. */
        if( *pDelta == '"' )
        {
            for( i = 0; i < 16 ; i++ ) 
            {
                cmdBuff[i] = '\0';
            }
            for( i = 0; i < ( deltaLength - 2 ) ; i++ ) 
            {
                cmdBuff[i] = *(pDelta + i + 1);
            }

            if( 0 == strncmp(cmdBuff,SET_COMMAND_VER,4))
            {
                if( ( *(pDelta + staIdx) >= '0' ) && ( *(pDelta + staIdx) <= '9' ) )
                {
                    i = staIdx;
                    while( (   *(pDelta + i) != '.' ) &&
                           ( ( *(pDelta + i) >= '0' ) && ( *(pDelta + i) <= '9' ) ) ){
                        i++;
                    }
                    if ( *( pDelta + i ) == '.' ){
                        endIdx = i;
                        for(i = staIdx ; i < endIdx ;i++){
                            num = ( (*( pDelta + i )) - 0x30 );
                            for(j = 0; j < (endIdx - i - 1) ;j++){
                                num *= 10;
                            }
                            swVer[0] += num;
                            num = 0;
                        }
                    	staIdx = endIdx + 1;
	                    i = staIdx;
	                    while( (   *(pDelta + i) != '.' ) &&
	                           ( ( *(pDelta + i) >= '0' ) && ( *(pDelta + i) <= '9' ) ) ){
	                        i++;
	                    }
	                    if ( *( pDelta + i ) == '.' ){
	                        endIdx = i;
	                        for(i = staIdx ; i < endIdx ;i++){
	                            num = ( (*( pDelta + i )) - 0x30 );
	                            for(j = 0; j < (endIdx - i - 1) ;j++){
	                                num *= 10;
	                            }
	                            swVer[1] += num;
                                num = 0;
	                        }
		                    staIdx = endIdx + 1;
		                    i = staIdx;
		                    while( (   *(pDelta + i) != '"' ) &&
		                           ( ( *(pDelta + i) >= '0' ) && ( *(pDelta + i) <= '9' ) ) ){
		                        i++;
		                    }
		                    if ( *( pDelta + i ) == '"' ){
		                        endIdx = i;
		                        for(i = staIdx ; i < endIdx ;i++){
		                            num = ( (*( pDelta + i )) - 0x30 );
		                            for(j = 0; j < (endIdx - i - 1) ;j++){
		                                num *= 10;
		                            }
		                            swVer[2] += num;
                                    num = 0;
                                    swVerFix = true;
		                        }
		                    }
	                    }
                    }

                    if(swVerFix){
                        if( ( (xAppFirmwareVersion.u.x.ucMajor != swVer[0])   || 
                              (xAppFirmwareVersion.u.x.ucMinor != swVer[1]) ) ||
                              (xAppFirmwareVersion.u.x.usBuild != swVer[2]) ) {
                            xAppFirmwareVersion.u.x.ucMajor = swVer[0];
                            xAppFirmwareVersion.u.x.ucMinor = swVer[1];
                            xAppFirmwareVersion.u.x.usBuild = swVer[2];
                            blinkCnt = 10;
                        }
                    }
                    else{
                        IotLogWarn( "Failed to parse SWVersion state from delta document." );
                    }
                }
                else
                {
                    IotLogWarn( "Failed to parse SWVersion state from delta document." );
                }
            }
            else
            {
                IotLogWarn( "Unknown SWVersion state parsed from delta document." );
            }
        }
        else
        {
            IotLogWarn( "Unknown SWVersion state parsed from delta document." );
        }
    }
    else{
        IotLogWarn( "Failed to parse SWVersion state from delta document." );
    }

	/* SensorDataUpdateOn */
    /* Check if there is a different "SensorDataUpdateOn" state in the Shadow. */
    deltaFound = _getDelta( pCallbackParam->u.callback.pDocument,
                            pCallbackParam->u.callback.documentLength,
                            "SensorDataUpdateOn",
                            &pDelta,
                            &deltaLength );

    if( deltaFound == true )
    {
        /* Change the current state based on the value in the delta document. */
        if( *pDelta == '"' )
        {
            for( i = 0; i < 16 ; i++ ) 
            {
                cmdBuff[i] = '\0';
            }
            for( i = 0; i < ( deltaLength - 2 ) ; i++ ) 
            {
                cmdBuff[i] = *(pDelta + i + 1);
            }

            if( 0 == strcmp(cmdBuff,SNS_COMMAND_ON))
            {
                sensorupdatecurrentState = 1;
            }
            else if( 0 == strcmp(cmdBuff,SNS_COMMAND_OFF))
            {
                sensorupdatecurrentState = 0;
            }
            else
            {
                IotLogWarn( "Unknown SensorDataUpdateOn state parsed from delta document." );
            }
        }
        else
        {
            IotLogWarn( "Unknown SensorDataUpdateOn state parsed from delta document." );
        }
    }
    else{
        IotLogWarn( "Failed to parse SensorDataUpdateOn state from delta document." );
    }

    /* Post to the delta semaphore to unblock the thread sending Shadow updates. */
    IotSemaphore_Post( pDeltaSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow updated callback, invoked when the Shadow document changes.
 *
 * This function reports when a Shadow has been updated.
 *
 * @param[in] pCallbackContext Not used.
 * @param[in] pCallbackParam The received Shadow updated document.
 */
static void _shadowUpdatedCallback( void * pCallbackContext,
                                    AwsIotShadowCallbackParam_t * pCallbackParam )
{
    bool previousFound = false, currentFound = false;
    const char * pPrevious = NULL, * pCurrent = NULL;
    size_t previousLength = 0, currentLength = 0;

    /* Silence warnings about unused parameters. */
    ( void ) pCallbackContext;

    /* Find the previous Shadow document. */
    previousFound = _getUpdatedState( pCallbackParam->u.callback.pDocument,
                                      pCallbackParam->u.callback.documentLength,
                                      "previous",
                                      &pPrevious,
                                      &previousLength );

    /* Find the current Shadow document. */
    currentFound = _getUpdatedState( pCallbackParam->u.callback.pDocument,
                                     pCallbackParam->u.callback.documentLength,
                                     "current",
                                     &pCurrent,
                                     &currentLength );

    /* Log the previous and current states. */
    if( ( previousFound == true ) && ( currentFound == true ) )
    {
        IotLogInfo( "Shadow was updated!\r\n"
                    "Previous: {\"state\":%.*s}\r\n"
                    "Current:  {\"state\":%.*s}",
                    previousLength,
                    pPrevious,
                    currentLength,
                    pCurrent );
    }
    else
    {
        if( previousFound == false )
        {
            IotLogWarn( "Previous state not found in Shadow updated document." );
        }

        if( currentFound == false )
        {
            IotLogWarn( "Current state not found in Shadow updated document." );
        }
    }
}

/**
 * @brief Set the Shadow callback functions used in this demo.
 *
 * @param[in] pDeltaSemaphore Used to synchronize Shadow updates with the delta
 * callback.
 * @param[in] mqttConnection The MQTT connection used for Shadows.
 * @param[in] pThingName The Thing Name for Shadows in this demo.
 * @param[in] thingNameLength The length of `pThingName`.
 *
 * @return `EXIT_SUCCESS` if all Shadow callbacks were set; `EXIT_FAILURE`
 * otherwise.
 */
static int _setShadowCallbacks( IotSemaphore_t * pDeltaSemaphore,
                                IotMqttConnection_t mqttConnection,
                                const char * pThingName,
                                size_t thingNameLength )
{
    int status = EXIT_SUCCESS;
    AwsIotShadowError_t callbackStatus = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowCallbackInfo_t deltaCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER,
                               updatedCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;

    /* Set the functions for callbacks. */
    deltaCallback.pCallbackContext = pDeltaSemaphore;
    deltaCallback.function = _shadowDeltaCallback;
    updatedCallback.function = _shadowUpdatedCallback;

    /* Set the delta callback, which notifies of different desired and reported
     * Shadow states. */
    callbackStatus = AwsIotShadow_SetDeltaCallback( mqttConnection,
                                                    pThingName,
                                                    thingNameLength,
                                                    0,
                                                    &deltaCallback );

    if( callbackStatus == AWS_IOT_SHADOW_SUCCESS )
    {
        /* Set the updated callback, which notifies when a Shadow document is
         * changed. */
        callbackStatus = AwsIotShadow_SetUpdatedCallback( mqttConnection,
                                                          pThingName,
                                                          thingNameLength,
                                                          0,
                                                          &updatedCallback );
    }

    if( callbackStatus != AWS_IOT_SHADOW_SUCCESS )
    {
        IotLogError( "Failed to set demo shadow callback, error %s.",
                     AwsIotShadow_strerror( callbackStatus ) );

        status = EXIT_FAILURE;
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Try to delete any Shadow document in the cloud.
 *
 * @param[in] mqttConnection The MQTT connection used for Shadows.
 * @param[in] pThingName The Shadow Thing Name to delete.
 * @param[in] thingNameLength The length of `pThingName`.
 */
static void _clearShadowDocument( IotMqttConnection_t mqttConnection,
                                  const char * const pThingName,
                                  size_t thingNameLength )
{
    AwsIotShadowError_t deleteStatus = AWS_IOT_SHADOW_STATUS_PENDING;

    /* Delete any existing Shadow document so that this demo starts with an empty
     * Shadow. */
    deleteStatus = AwsIotShadow_TimedDelete( mqttConnection,
                                             pThingName,
                                             thingNameLength,
                                             0,
                                             TIMEOUT_MS );

    /* Check for return values of "SUCCESS" and "NOT FOUND". Both of these values
     * mean that the Shadow document is now empty. */
    if( ( deleteStatus == AWS_IOT_SHADOW_SUCCESS ) || ( deleteStatus == AWS_IOT_SHADOW_NOT_FOUND ) )
    {
        IotLogInfo( "Successfully cleared Shadow of %.*s.",
                    thingNameLength,
                    pThingName );
    }
    else
    {
        IotLogWarn( "Shadow of %.*s not cleared.",
                    thingNameLength,
                    pThingName );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Send the Shadow updates that will trigger the Shadow callbacks.
 *
 * @param[in] pDeltaSemaphore Used to synchronize Shadow updates with the delta
 * callback.
 * @param[in] mqttConnection The MQTT connection used for Shadows.
 * @param[in] pThingName The Thing Name for Shadows in this demo.
 * @param[in] thingNameLength The length of `pThingName`.
 *
 * @return `EXIT_SUCCESS` if all Shadow updates were sent; `EXIT_FAILURE`
 * otherwise.
 */
static int _sendShadowUpdates( IotSemaphore_t * pDeltaSemaphore,
                               IotMqttConnection_t mqttConnection,
                               const char * const pThingName,
                               size_t thingNameLength )
{
    int status = EXIT_SUCCESS;
    int32_t i = 0;
    AwsIotShadowError_t updateStatus = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t updateDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;

    /* A buffer containing the update document. It has static duration to prevent
     * it from being placed on the call stack. */
    static char pUpdateDocument[ EXPECTED_DESIRED_JSON_SIZE + 1 ] = { 0 };

    /* Set the common members of the Shadow update document info. */
    updateDocument.pThingName = pThingName;
    updateDocument.thingNameLength = thingNameLength;
    updateDocument.u.update.pUpdateDocument = pUpdateDocument;
    updateDocument.u.update.updateDocumentLength = EXPECTED_DESIRED_JSON_SIZE;

	/* Generate a Shadow desired state document, using a timestamp for the client
	 * token. To keep the client token within 6 characters, it is modded by 1000000. */
	status = snprintf(pUpdateDocument,
	EXPECTED_DESIRED_JSON_SIZE + 1,
	SHADOW_DESIRED_JSON,
			(long unsigned ) (IotClock_GetTimeMs() % 1000000));

	/* Check for errors from snprintf. The expected value is the length of
	 * the desired JSON document less the format specifier for the state. */
	if ((size_t ) status != EXPECTED_DESIRED_JSON_SIZE) {
		IotLogError(
				"Failed to generate desired state document for Shadow update"
						" %d of %d.", i, AWS_IOT_DEMO_SHADOW_UPDATE_COUNT);

		status = EXIT_FAILURE;
		return status;
	} else {
		status = EXIT_SUCCESS;
	}

	IotLogInfo("Sending Shadow update %d of %d: %s", i,
			AWS_IOT_DEMO_SHADOW_UPDATE_COUNT, pUpdateDocument);

	/* Send the Shadow update. Because the Shadow is constantly updated in
	 * this demo, the "Keep Subscriptions" flag is passed to this function.
	 * Note that this flag only needs to be passed on the first call, but
	 * passing it for subsequent calls is fine.
	 */
    updateStatus = AwsIotShadow_TimedUpdate( mqttConnection,
                                             &updateDocument,
                                             AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS,
                                             TIMEOUT_MS );

	/* Check the status of the Shadow update. */
	if (updateStatus != AWS_IOT_SHADOW_SUCCESS) {
		IotLogError("Failed to send Shadow update %d of %d, error %s.", i,
				AWS_IOT_DEMO_SHADOW_UPDATE_COUNT,
				AwsIotShadow_strerror(updateStatus));

		status = EXIT_FAILURE;
		return status;
	} else {
		IotLogInfo("Successfully sent Shadow update %d of %d.", i,
				AWS_IOT_DEMO_SHADOW_UPDATE_COUNT);

		/* Wait for the delta callback to change its state before continuing. */
		if (IotSemaphore_TimedWait(pDeltaSemaphore, TIMEOUT_MS) == false) {
			IotLogError("Timed out waiting on delta callback to change state.");

			status = EXIT_FAILURE;
			return status;
		}
	}

    return status;
}

static void prvNetworkStateChangeCallback( uint32_t ulNetworkType,
                                           AwsIotNetworkState_t xNetworkState,
                                           void * pvContext )
{
    ( void ) pvContext;

    if( xNetworkState == eNetworkStateEnabled )
    {
        /* Release the semaphore, to indicate other tasks that a network is available */
        if( xNetworkConnected == pdFALSE )
        {
            xSemaphoreGive( xNetworkAvailableLock );
        }
    }
    else if( xNetworkState == eNetworkStateDisabled )
    {
        /* If the publish task is already connected to this network, set connected network flag to none,
         * to trigger a reconnect.
         */
        if( ( xNetworkConnected == pdTRUE ) && ( xConnection.ulNetworkType == ulNetworkType ) )
        {
            xNetworkConnected = pdFALSE;
        }
    }
}

static IotNetworkError_t prvNetworkDisconnectCallback( void * pvContext )
{
    ( void ) pvContext;
    xNetworkConnected = pdFALSE;

    return IOT_NETWORK_SUCCESS;
}

static BaseType_t prxCreateNetworkConnection( void )
{
    BaseType_t xRet = pdFALSE;
    uint32_t ulRetriesLeft = DemoCONN_RETRY_LIMIT;
    uint32_t ulRetryIntervalMS = DemoCONN_RETRY_INTERVAL_MS;

    do
    {
        /* No networks are available, block for a physical network connection. */
        if( ( AwsIotNetworkManager_GetConnectedNetworks() & DemoNETWORK_TYPES ) == AWSIOT_NETWORK_TYPE_NONE )
        {
            /* Block for a Network Connection. */
            configPRINTF( ( "Waiting for a network connection.\r\n" ) );
            ( void ) xSemaphoreTake( xNetworkAvailableLock, portMAX_DELAY );
        }

        #if defined( CONFIG_UPDATE_DEMO_ENABLED )
            /* Connect to one of the network type.*/
            xRet = xMqttDemoCreateNetworkConnection( &xConnection, DemoNETWORK_TYPES );
        #endif

        if( xRet == pdTRUE )
        {
            break;
        }
        else
        {
            /* Connection failed. Retry for a configured number of retries. */
            if( ulRetriesLeft > 0 )
            {
                configPRINTF( ( "Network Connection failed, retry delay %lu ms, retries left %lu\r\n", ulRetryIntervalMS, ulRetriesLeft ) );
                vTaskDelay( pdMS_TO_TICKS( ulRetryIntervalMS ) );
            }
        }
    } while( ulRetriesLeft-- > 0 );

    return xRet;
}

static uint32_t prvGenerateSensorJSON( char * Buffer )
{
	char * Buff = Buffer;
	st_sensors_t data;

#if (3 == APP_VERSION_BUILD)
	if (1 == sensorupdatecurrentState)
	{
		R_READ_Sensors(&data);
		data.temperature = (data.temperature - 32) / 1.8;
		return ( uint32_t ) snprintf( Buff,
									  renesasDemoBUFFER_LENGTH,
									  renesasDemoSENSOR_JSON,
									  data.temperature,
									  data.als,
									  data.humidity,
									  data.pressure,
									  data.accel.xacc,
									  data.accel.yacc,
									  data.accel.zacc,
									  data.gyro.xacc,
									  data.gyro.yacc,
									  data.gyro.zacc );
	}
	else
	{
		return ( uint32_t ) snprintf( Buff,
		                              renesasDemoBUFFER_LENGTH,
		                              renesasDemoSENSOR_JSON,
									  0,
									  0,
									  0,
									  0,
									  0,
									  0,
									  0,
									  0,
									  0,
									  0 );
	}
#else
	return ( uint32_t ) snprintf( Buff,
	                              renesasDemoBUFFER_LENGTH,
	                              renesasDemoSENSOR_JSON,
								  0,
								  0,
								  0,
								  0,
								  0,
								  0,
								  0,
								  0,
								  0,
								  0 );
#endif
}

/**
 * @brief Called by the MQTT library when an operation completes.
 *
 * The demo uses this callback to determine the result of PUBLISH operations.
 * @param[in] param1 The number of the PUBLISH that completed, passed as an intptr_t.
 * @param[in] pOperation Information about the completed operation passed by the
 * MQTT library.
 */
static void _operationCompleteCallback( void * param1,
                                        IotMqttCallbackParam_t * const pOperation )
{
    intptr_t publishCount = ( intptr_t ) param1;

    /* Silence warnings about unused variables. publishCount will not be used if
     * logging is disabled. */
    ( void ) publishCount;

    /* Print the status of the completed operation. A PUBLISH operation is
     * successful when transmitted over the network. */
    if( pOperation->u.operation.result == IOT_MQTT_SUCCESS )
    {
        IotLogInfo( "MQTT %s %d successfully sent.",
                    IotMqtt_OperationType( pOperation->u.operation.type ),
                    ( int ) publishCount );
    }
    else
    {
        IotLogError( "MQTT %s %d could not be sent. Error %s.",
                     IotMqtt_OperationType( pOperation->u.operation.type ),
                     ( int ) publishCount,
                     IotMqtt_strerror( pOperation->u.operation.result ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Called by the MQTT library when an incoming PUBLISH message is received.
 *
 * The demo uses this callback to handle incoming PUBLISH messages. This callback
 * prints the contents of an incoming message and publishes an acknowledgement
 * to the MQTT server.
 * @param[in] param1 Counts the total number of received PUBLISH messages. This
 * callback will increment this counter.
 * @param[in] pPublish Information about the incoming PUBLISH message passed by
 * the MQTT library.
 */
static void _mqttSubscriptionCallback( void * param1,
                                       IotMqttCallbackParam_t * const pPublish )
{
    IotSemaphore_t * pPublishesReceived = ( IotSemaphore_t * ) param1;

    /* Increment the number of PUBLISH messages received. */
    IotSemaphore_Post( pPublishesReceived );
}

/*-----------------------------------------------------------*/

/**
 * @brief Initialize the MQTT library.
 *
 * @return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise.
 */
static int _initializeDemo( void )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus = IOT_MQTT_SUCCESS;
    AwsIotShadowError_t shadowInitStatus = AWS_IOT_SHADOW_SUCCESS;

    mqttInitStatus = IotMqtt_Init();

    if( mqttInitStatus != IOT_MQTT_SUCCESS )
    {
        /* Failed to initialize MQTT library. */
        status = EXIT_FAILURE;
    }

    /* Use the default MQTT timeout. */
    shadowInitStatus = AwsIotShadow_Init( 0 );

    if( shadowInitStatus != AWS_IOT_SHADOW_SUCCESS )
    {
        status = EXIT_FAILURE;
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Clean up the MQTT library.
 */
static void _cleanupDemo( void )
{
	AwsIotShadow_Cleanup();
    IotMqtt_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Establish a new connection to the MQTT server.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 * @param[out] pMqttConnection Set to the handle to the new MQTT connection.
 *
 * @return `EXIT_SUCCESS` if the connection is successfully established; `EXIT_FAILURE`
 * otherwise.
 */
static int _establishMqttConnection( bool awsIotMqttMode,
                                     const char * pIdentifier,
                                     void * pNetworkServerInfo,
                                     void * pNetworkCredentialInfo,
                                     const IotNetworkInterface_t * pNetworkInterface,
                                     IotMqttConnection_t * pMqttConnection )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t connectStatus;

    IotMqttConnectInfo_t xConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    
    memset( &xConnectInfo, 0, sizeof( xConnectInfo ) );

    xNetworkConnected = prxCreateNetworkConnection();
    if( xNetworkConnected )
    {
    	configPRINTF( ( "Connecting to broker...\r\n" ) );
		if( xConnection.ulNetworkType == AWSIOT_NETWORK_TYPE_BLE )
		{
			xConnectInfo.awsIotMqttMode = false;
			xConnectInfo.keepAliveSeconds = 0;
		}
		else
		{
			xConnectInfo.awsIotMqttMode = true;
			xConnectInfo.keepAliveSeconds = DemoKEEPALIVE_SECONDS;
		}

		xConnectInfo.cleanSession = true;
		xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );
		xConnectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;

		/* Establish the MQTT connection. */
		if( status == EXIT_SUCCESS )
		{
			IotLogInfo( "MQTT demo client identifier is %.*s (length %hu).",
						xConnectInfo.clientIdentifierLength,
						xConnectInfo.pClientIdentifier,
						xConnectInfo.clientIdentifierLength );
	        IotLogInfo( "Shadow Thing Name is %.*s (length %hu).",
	                    xConnectInfo.clientIdentifierLength,
	                    xConnectInfo.pClientIdentifier,
	                    xConnectInfo.clientIdentifierLength );

			connectStatus = IotMqtt_Connect( &( xConnection.xNetworkInfo ),
											 &xConnectInfo,
											 DemoCONN_TIMEOUT_MS,
											 &( xConnection.xMqttConnection ) );

			if( connectStatus != IOT_MQTT_SUCCESS )
			{
				IotLogError( "MQTT CONNECT returned error %s.",
							 IotMqtt_strerror( connectStatus ) );

				status = EXIT_FAILURE;
			}
		}
    }
    else
    {
        configPRINTF( ( "Failed to create MQTT client.\r\n" ) );
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Add or remove subscriptions by either subscribing or unsubscribing.
 *
 * @param[in] mqttConnection The MQTT connection to use for subscriptions.
 * @param[in] operation Either #IOT_MQTT_SUBSCRIBE or #IOT_MQTT_UNSUBSCRIBE.
 * @param[in] pTopicFilters Array of topic filters for subscriptions.
 * @param[in] pCallbackParameter The parameter to pass to the subscription
 * callback.
 *
 * @return `EXIT_SUCCESS` if the subscription operation succeeded; `EXIT_FAILURE`
 * otherwise.
 */
static int _modifySubscriptions( IotMqttConnection_t mqttConnection,
                                 IotMqttOperationType_t operation,
                                 const char ** pTopicFilters,
                                 void * pCallbackParameter )
{
    int status = EXIT_SUCCESS;
    int32_t i = 0;
    IotMqttError_t subscriptionStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t pSubscriptions[ TOPIC_FILTER_COUNT ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* Set the members of the subscription list. */
    for( i = 0; i < TOPIC_FILTER_COUNT; i++ )
    {
        pSubscriptions[ i ].qos = IOT_MQTT_QOS_1;
        pSubscriptions[ i ].pTopicFilter = pTopicFilters[ i ];
        pSubscriptions[ i ].topicFilterLength = TOPIC_FILTER_LENGTH;
        pSubscriptions[ i ].callback.pCallbackContext = pCallbackParameter;
        pSubscriptions[ i ].callback.function = _mqttSubscriptionCallback;
    }

    /* Modify subscriptions by either subscribing or unsubscribing. */
    if( operation == IOT_MQTT_SUBSCRIBE )
    {
        subscriptionStatus = IotMqtt_TimedSubscribe( xConnection.xMqttConnection,
                                                     pSubscriptions,
                                                     TOPIC_FILTER_COUNT,
                                                     0,
                                                     MQTT_TIMEOUT_MS );

        /* Check the status of SUBSCRIBE. */
        switch( subscriptionStatus )
        {
            case IOT_MQTT_SUCCESS:
                IotLogInfo( "All demo topic filter subscriptions accepted." );
                break;

            case IOT_MQTT_SERVER_REFUSED:

                /* Check which subscriptions were rejected before exiting the demo. */
                for( i = 0; i < TOPIC_FILTER_COUNT; i++ )
                {
                    if( IotMqtt_IsSubscribed( xConnection.xMqttConnection,
                                              pSubscriptions[ i ].pTopicFilter,
                                              pSubscriptions[ i ].topicFilterLength,
                                              NULL ) == true )
                    {
                        IotLogInfo( "Topic filter %.*s was accepted.",
                                    pSubscriptions[ i ].topicFilterLength,
                                    pSubscriptions[ i ].pTopicFilter );
                    }
                    else
                    {
                        IotLogError( "Topic filter %.*s was rejected.",
                                     pSubscriptions[ i ].topicFilterLength,
                                     pSubscriptions[ i ].pTopicFilter );
                    }
                }

                status = EXIT_FAILURE;
                break;

            default:

                status = EXIT_FAILURE;
                break;
        }
    }
    else if( operation == IOT_MQTT_UNSUBSCRIBE )
    {
        subscriptionStatus = IotMqtt_TimedUnsubscribe( xConnection.xMqttConnection,
                                                       pSubscriptions,
                                                       TOPIC_FILTER_COUNT,
                                                       0,
                                                       MQTT_TIMEOUT_MS );

        /* Check the status of UNSUBSCRIBE. */
        if( subscriptionStatus != IOT_MQTT_SUCCESS )
        {
            status = EXIT_FAILURE;
        }
    }
    else
    {
        /* Only SUBSCRIBE and UNSUBSCRIBE are valid for modifying subscriptions. */
        IotLogError( "MQTT operation %s is not valid for modifying subscriptions.",
                     IotMqtt_OperationType( operation ) );

        status = EXIT_FAILURE;
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Transmit all messages and wait for them to be received on topic filters.
 *
 * @param[in] mqttConnection The MQTT connection to use for publishing.
 * @param[in] pTopicNames Array of topic names for publishing. These were previously
 * subscribed to as topic filters.
 * @param[in] pPublishReceivedCounter Counts the number of messages received on
 * topic filters.
 *
 * @return `EXIT_SUCCESS` if all messages are published and received; `EXIT_FAILURE`
 * otherwise.
 */
static int _publishAllMessages( IotMqttConnection_t mqttConnection,
                                const char ** pTopicNames,
                                IotSemaphore_t * pPublishReceivedCounter,
								const IotNetworkInterface_t * pNetworkInterface,
								void * pNetworkCredentialInfo )
{
    int status = EXIT_SUCCESS;
    intptr_t publishCount = 0, i = 0;
    IotMqttError_t publishStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttCallbackInfo_t publishComplete = IOT_MQTT_CALLBACK_INFO_INITIALIZER;
    wifi_err_t wifiStatus;

    char cDataBuffer[ renesasDemoBUFFER_LENGTH ] = { 0 };

    IotMqttConnectInfo_t xConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

    configPRINTF( ( "SWVersion %u.%u.%u\r\n",
                    xAppFirmwareVersion.u.x.ucMajor,
                    xAppFirmwareVersion.u.x.ucMinor,
                    xAppFirmwareVersion.u.x.usBuild ) );
    configPRINTF( ( "Creating MQTT Client...\r\n" ) );

    /* The MQTT library should invoke this callback when a PUBLISH message
     * is successfully transmitted. */
    publishComplete.function = _operationCompleteCallback;

    /* Set the common members of the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.topicNameLength = TOPIC_FILTER_LENGTH;
    publishInfo.pPayload = cDataBuffer;
    publishInfo.retryMs = PUBLISH_RETRY_MS;
    publishInfo.retryLimit = PUBLISH_RETRY_LIMIT;

    wifiStatus = R_WIFI_SX_ULPGN_GetIpAddress(&ip_cfg);

    /* Loop to PUBLISH all messages of this demo. */
    while(1)
    {
        configPRINTF( ( "Connected to broker.\r\n" ) );
		while (1)
		{
			/* Wait forever traffic but allow other tasks to run and output statistics only once per second. */
			vTaskDelay( myappONE_SECOND_DELAY_IN_TICKS);

			/* Sensor data upload */
			/* Pass the PUBLISH number to the operation complete callback. */
			publishComplete.pCallbackContext = (void*) publishCount;

			/* Choose a topic name (round-robin through the array of topic names). */
			publishInfo.pTopicName = pTopicNames[publishCount
					% TOPIC_FILTER_COUNT];

			/* Generate the payload for the PUBLISH. */
			prvGenerateSensorJSON(cDataBuffer);
			turn_on_led();

			/* Check for errors from snprintf. */
			if (status < 0) {
				IotLogError(
						"Failed to generate MQTT PUBLISH payload for PUBLISH %d.",
						(int ) publishCount);
				status = EXIT_FAILURE;

				break;
			} else {
				publishInfo.payloadLength = (uint32_t ) strlen(cDataBuffer);
				status = EXIT_SUCCESS;
			}

			/* PUBLISH a message. This is an asynchronous function that notifies of
			 * completion through a callback. */
			publishStatus = IotMqtt_Publish(xConnection.xMqttConnection,
					&publishInfo, 0, &publishComplete,
					NULL);
			if (publishStatus != IOT_MQTT_STATUS_PENDING) {
				IotLogError("MQTT PUBLISH %d returned error %s.",
						(int ) publishCount, IotMqtt_strerror(publishStatus));
				status = EXIT_FAILURE;

				break;
			}

			/* PUBLISH SWVersion & IPAddress */
		    publishStatus = mpttAckCallBack();
			if (publishStatus != IOT_MQTT_STATUS_PENDING) {
				IotLogError("MQTT PUBLISH %d returned error %s.",
							(int ) publishCount, IotMqtt_strerror(publishStatus));
				status = EXIT_FAILURE;
			}

			/* If a complete burst of messages has been published, wait for an equal
			 * number of messages to be received. Note that messages may be received
			 * out-of-order, especially if a message was lost and had to be retried. */
			if ((publishCount % IOT_DEMO_MQTT_PUBLISH_BURST_SIZE)
					== ( IOT_DEMO_MQTT_PUBLISH_BURST_SIZE - 1)) {
				IotLogInfo("Waiting for %d publishes to be received.",
						IOT_DEMO_MQTT_PUBLISH_BURST_SIZE);

				for (i = 0; i < IOT_DEMO_MQTT_PUBLISH_BURST_SIZE; i++) {
					if (IotSemaphore_TimedWait(pPublishReceivedCounter,
					MQTT_TIMEOUT_MS) == false) {
						IotLogError(
								"Timed out waiting for incoming PUBLISH messages.");
						status = EXIT_FAILURE;
						break;
					}
				}

				IotLogInfo("%d publishes received.", i);
			}

			/* Stop publishing if there was an error. */
			if (status == EXIT_FAILURE) {
				break;
			}

			publishCount++;
		}
		IotMqtt_Disconnect(xConnection.xMqttConnection, false);
#if defined( CONFIG_UPDATE_DEMO_ENABLED )
		vMqttDemoDeleteNetworkConnection(&xConnection);
#endif
		/* After failure to connect or a disconnect, wait an arbitrary one second before retry. */
		vTaskDelay( myappONE_SECOND_DELAY_IN_TICKS);

		if (status == EXIT_FAILURE)
		{
			break;
		}
	}
	return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the MQTT demo, called by the demo runner.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RenesasSensorDataUploadDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface )
{
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_SUCCESS;

    /* Handle of the MQTT connection used in this demo. */
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    /* Counts the number of incoming PUBLISHES received (and allows the demo
     * application to wait on incoming PUBLISH messages). */
    IotSemaphore_t publishesReceived;

    /* Allows the Shadow update function to wait for the delta callback to complete
     * a state change before continuing. */
    IotSemaphore_t deltaSemaphore;

    /* Length of Shadow Thing Name. */
    size_t thingNameLength = 0;

    /* Topics used as both topic filters and topic names in this demo. */
    const char * pTopics[ TOPIC_FILTER_COUNT ] =
    {
        IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/sensor",
    };

    /* Flags for tracking which cleanup functions must be called. */
    bool librariesInitialized = false, connectionEstablished = false, deltaSemaphoreCreated = false;

    xNetworkAvailableLock = xSemaphoreCreateBinary();
    if( xNetworkAvailableLock == NULL )
    {
        configPRINTF( ( "Failed to create semaphore.\r\n" ) );
        return EXIT_FAILURE;
    }

    /* for sensor API */
    configPRINTF( ( "Sensor API Initialized\r\n" ) );
    R_INIT_Sensors();

    /* Initialize the libraries required for this demo. */
    status = _initializeDemo();

    if( AwsIotNetworkManager_SubscribeForStateChange( DemoNETWORK_TYPES,
                                                      prvNetworkStateChangeCallback,
                                                      NULL,
                                                      &xSubscriptionHandle ) != pdTRUE )
    {
        configPRINTF( ( "Failed to create Network Manager subscription.\r\n" ) );
        return EXIT_FAILURE;
    }

    if( status == EXIT_SUCCESS )
    {
        /* Mark the libraries as initialized. */
        librariesInitialized = true;

        /* Establish a new MQTT connection. */
        status = _establishMqttConnection( awsIotMqttMode,
                                           pIdentifier,
                                           pNetworkServerInfo,
                                           pNetworkCredentialInfo,
                                           pNetworkInterface,
										   &( xConnection.xMqttConnection ) );
    }

    if( status == EXIT_SUCCESS )
    {
        /* Mark the MQTT connection as established. */
        connectionEstablished = true;

        /* Add the topic filter subscriptions used in this demo. */
        status = _modifySubscriptions( ( xConnection.xMqttConnection ),
                                       IOT_MQTT_SUBSCRIBE,
                                       pTopics,
                                       &publishesReceived );
    }

    /* for processing */
    if( DemoNETWORK_TYPES == AWSIOT_NETWORK_TYPE_NONE )
    {
        configPRINTF( ( "There are no networks configured for the demo.\r\n" ) );
        return EXIT_FAILURE;
    }

    /* for Device shadow */
    /* Create the semaphore that synchronizes with the delta callback. */
    /* Determine the length of the Thing Name. */
    if( pIdentifier != NULL )
    {
        thingNameLength = strlen( pIdentifier );

        if( thingNameLength == 0 )
        {
            IotLogError( "The length of the Thing Name (identifier) must be nonzero." );

            status = EXIT_FAILURE;
        }
    }
    else
    {
        IotLogError( "A Thing Name (identifier) must be provided for the Shadow demo." );

        status = EXIT_FAILURE;
    }

    deltaSemaphoreCreated = IotSemaphore_Create( &deltaSemaphore, 0, 1 );
    if( deltaSemaphoreCreated == false )
    {
        status = EXIT_FAILURE;
    }

    if( status == EXIT_SUCCESS )
    {
        /* Set the Shadow callbacks for this demo. */
        status = _setShadowCallbacks( &deltaSemaphore,
        							  ( xConnection.xMqttConnection ),
                                      pIdentifier,
                                      thingNameLength );
    }

    if( status == EXIT_SUCCESS )
    {
        /* Clear the Shadow document so that this demo starts with no existing
         * Shadow. */
        _clearShadowDocument( ( xConnection.xMqttConnection ), pIdentifier, thingNameLength );

        /* Send Shadow updates. */
        status = _sendShadowUpdates( &deltaSemaphore,
        							 ( xConnection.xMqttConnection ),
                                     pIdentifier,
                                     thingNameLength );

        /* Create the semaphore to count incoming PUBLISH messages. */
        if( IotSemaphore_Create( &publishesReceived,
                                 0,
                                 IOT_DEMO_MQTT_PUBLISH_BURST_SIZE ) == true )
        {
            /* PUBLISH (and wait) for all messages. */
            status = _publishAllMessages( ( xConnection.xMqttConnection ),
                                          pTopics,
                                          &publishesReceived,
										  pNetworkInterface,
										  pNetworkCredentialInfo );

            /* Destroy the incoming PUBLISH counter. */
            IotSemaphore_Destroy( &publishesReceived );
            IotSemaphore_Destroy( &deltaSemaphore );
        }
        else
        {
            /* Failed to create incoming PUBLISH counter. */
            status = EXIT_FAILURE;
        }
    }

    /* Clean up libraries if they were initialized. */
    if( librariesInitialized == true )
    {
        _cleanupDemo();
    }

    return status;
}

/*-----------------------------------------------------------*/
