/*
 * Amazon FreeRTOS Secure Socket V1.0.0
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
 * @file aws_secure_sockets.c
 * @brief WiFi and Secure Socket interface implementation.
 */

/* FreeRTOS includes. */
#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "list.h"
#include "aws_secure_sockets.h"
#include "aws_tls.h"
#include "task.h"
#include "aws_pkcs11.h"
#include "aws_crypto.h"
#include "esp8266_driver.h"
#include "r_compiler_abstraction_api.h"

/**
 * @brief Maximum number of sockets.
 *
 * 16 total sockets
 */
#define MAX_NUM_SSOCKETS    (2)

/* Internal context structure. */
typedef struct SSOCKETContext
{
    Socket_t xsocket;
    char * pcdestination;
    void * pvtls_context;
    BaseType_t xrequire_tls;
    BaseType_t xsend_flags;
    BaseType_t xrecv_flags;
    uint32_t ulsend_timeout;
    uint32_t ulrecv_timeout;
    char * pcserver_certificate;
    uint32_t ulserver_certificate_length;
    uint32_t socket_no;
} ssocket_context_t;

typedef struct SSOCKETContext * sscoket_context_ptr_t;

/**
 * @brief Number of secure sockets allocated.
 *
 * Keep a count of the number of open sockets.
 */
static uint8_t ssockets_num_allocated = 0;

static uint8_t get_socket_no(void *pvContext);
static Socket_t SOCKETS_Accept( Socket_t xSocket,
                            SocketsSockaddr_t * pxAddress,
                            Socklen_t * pxAddressLength );
BaseType_t SOCKETS_Init( void );

/*
 * @brief Network send callback.
 */
static BaseType_t prvNetworkSend( void * pvContext,
                                    const unsigned char * pucData,
                                    size_t xDataLength )
{
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) pvContext;

    /* cast (const unsigned char )* to (uint8_t *) */
    return esp8266_tcp_send(pxcontext->socket_no, (uint8_t *)pucData, xDataLength, pxcontext->ulsend_timeout);
}
/*-----------------------------------------------------------*/

/*
 * @brief Network receive callback.
*/
static BaseType_t prvNetworkRecv( void * pvContext,
                                    unsigned char * pucReceiveBuffer,
                                    size_t xReceiveLength )
{
    BaseType_t receive_byte;
    
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) pvContext;

    receive_byte = esp8266_tcp_recv(pxcontext->socket_no, pucReceiveBuffer, xReceiveLength, pxcontext->ulrecv_timeout);

    if(xReceiveLength == 64)
    {
        R_COMPILER_Nop();
    }
    return receive_byte;
}

/*-----------------------------------------------------------*/

/**
 * @brief Creates a TCP socket.
 *
 * This call allocates memory and claims a socket resource.
 *
 * @sa SOCKETS_Close()
 *
 * @param[in] lDomain Must be set to SOCKETS_AF_INET. See @ref SocketDomains.
 * @param[in] lType Set to SOCKETS_SOCK_STREAM to create a TCP socket.
 * No other value is valid.  See @ref SocketTypes.
 * @param[in] lProtocol Set to SOCKETS_IPPROTO_TCP to create a TCP socket.
 * No other value is valid. See @ref Protocols.
 *
 * @return
 * * If a socket is created successfully, then the socket handle is
 * returned
 * * @ref SOCKETS_INVALID_SOCKET is returned if an error occurred.
 */
Socket_t SOCKETS_Socket( int32_t lDomain,
                            int32_t lType,
                            int32_t lProtocol )
{
    int32_t lstatus = SOCKETS_ERROR_NONE;
    int32_t ret;
    int32_t i;
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = NULL;

    /* Ensure that only supported values are supplied. */
    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );


        if(ssockets_num_allocated >= MAX_NUM_SSOCKETS)
        {
            lstatus = SOCKETS_SOCKET_ERROR;
        }

        if( SOCKETS_ERROR_NONE == lstatus )
        {
            /* Allocate the internal context structure. */
            if( NULL == 
                ( pxcontext = pvPortMalloc( sizeof( ssocket_context_t ) ) ) )
            {
                lstatus = SOCKETS_ENOMEM;
            }
        }

        if( SOCKETS_ERROR_NONE == lstatus )
        {
            memset( pxcontext, 0, sizeof( ssocket_context_t ) );

            for(i = 0;i<MAX_NUM_SSOCKETS; i++)
            {
                /* Create the wrapped socket. */
                ret = esp8266_socket_create(i, 0, 4);
                if((-1) == ret)
                {
                }
                else
                {
                    pxcontext->socket_no = i;
                    pxcontext->xsocket = pxcontext;
                    pxcontext->ulrecv_timeout = socketsconfigDEFAULT_RECV_TIMEOUT;
                    pxcontext->ulsend_timeout = socketsconfigDEFAULT_SEND_TIMEOUT;
                    break;
                }
            }
            if(i == MAX_NUM_SSOCKETS)
            {
                lstatus = SOCKETS_SOCKET_ERROR;
            }

        }

        if( SOCKETS_ERROR_NONE != lstatus )
        {
            vPortFree( pxcontext );
        }

        else
        {
            if(ssockets_num_allocated < MAX_NUM_SSOCKETS)
            {
                ssockets_num_allocated++;
            }
        }

    if( SOCKETS_ERROR_NONE != lstatus )
    {
        /* Cast to an appropriate type */
        return SOCKETS_INVALID_SOCKET;
    }
    else
    {
        return pxcontext;
    }
}
/*-----------------------------------------------------------*/

static Socket_t SOCKETS_Accept( Socket_t xSocket,
                            SocketsSockaddr_t * pxAddress,
                            Socklen_t * pxAddressLength )
{
    /* FIX ME. */
    return SOCKETS_INVALID_SOCKET;
}
/*-----------------------------------------------------------*/

/**
 * @brief Connects the socket to the specified IP address and port.
 *
 * The socket must first have been successfully created by a call to 
 * SOCKETS_Socket().
 *
 * @param[in] xSocket The handle of the socket to be connected.
 * @param[in] pxAddress A pointer to a SocketsSockaddr_t structure that 
 * contains the the address to connect the socket to.
 * @param[in] xAddressLength Should be set to sizeof( @ref SocketsSockaddr_t ).
 *
 * @return
 * * @ref SOCKETS_ERROR_NONE if a connection is established.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Connect( Socket_t xSocket,
                            SocketsSockaddr_t * pxAddress,
                            Socklen_t xAddressLength )
{
    int32_t lstatus = SOCKETS_ERROR_NONE;
    int32_t ret;
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) xSocket;
    TLSParams_t xtls_params = { 0 };
    
    /* Cast to an appropriate type */
    if( ( pxcontext != SOCKETS_INVALID_SOCKET ) && ( pxAddress != NULL ) )
    {
        /* Cast to an appropriate type */
        ret = esp8266_tcp_connect(pxcontext->socket_no, SOCKETS_ntohl(pxAddress->ulAddress), SOCKETS_ntohs(pxAddress->usPort));
        if( 0 != ret )
        {
            lstatus = SOCKETS_SOCKET_ERROR;
        }
        /* Negotiate TLS if requested. */
        if( ( SOCKETS_ERROR_NONE == lstatus ) && 
            /* Cast to an appropriate type */
            ( pdTRUE == pxcontext->xrequire_tls ) )
        {
            xtls_params.ulSize = sizeof( xtls_params );
            xtls_params.pcDestination = pxcontext->pcdestination;
            xtls_params.pcServerCertificate = pxcontext->pcserver_certificate;
            xtls_params.ulServerCertificateLength = pxcontext->ulserver_certificate_length;
            xtls_params.pvCallerContext = pxcontext;
            xtls_params.pxNetworkRecv = prvNetworkRecv;
            xtls_params.pxNetworkSend = prvNetworkSend;
            lstatus = TLS_Init( &pxcontext->pvtls_context, &xtls_params );

            if( SOCKETS_ERROR_NONE == lstatus )
            {
                lstatus = TLS_Connect( pxcontext->pvtls_context );
                if( lstatus < 0 )
                {
                    lstatus = SOCKETS_TLS_HANDSHAKE_ERROR;
                }
            }
        }
    }
    else
    {
        lstatus = SOCKETS_SOCKET_ERROR;
    }

    return lstatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Receive data from a TCP socket.
 *
 * The socket must have already been created using a call to SOCKETS_Socket()
 * and connected to a remote socket using SOCKETS_Connect().
 *
 * @param[in] xSocket The handle of the socket from which data is being 
 *                    received.
 * @param[out] pvBuffer The buffer into which the received data will be placed.
 * @param[in] xBufferLength The maximum number of bytes which can be received.
 * pvBuffer must be at least xBufferLength bytes long.
 * @param[in] ulFlags Not currently used. Should be set to 0.
 *
 * @return
 * * If the receive was successful then the number of bytes received (placed 
 *   in the buffer pointed to by pvBuffer) is returned.
 * * If a timeout occurred before data could be received then 0 is returned 
 *   (timeout is set using @ref SOCKETS_SO_RCVTIMEO).
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Recv( Socket_t xSocket,
                        void * pvBuffer,
                        size_t xBufferLength,
                        uint32_t ulFlags )
{
    int32_t lstatus = SOCKETS_SOCKET_ERROR;
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) xSocket;

    /* cast to BaseType_t */
    pxcontext->xrecv_flags = ( BaseType_t ) ulFlags;

    /* Cast to an appropriate type */
    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        /* Cast to an appropriate type */
        ( pvBuffer != NULL ) )
    {
        /* Cast to an appropriate type */
        if( pdTRUE == pxcontext->xrequire_tls )
        {
            /* Receive through TLS pipe, if negotiated. */
            lstatus = TLS_Recv( pxcontext->pvtls_context, pvBuffer, xBufferLength );
        }
        else
        {
            /* Receive unencrypted. */
            lstatus = prvNetworkRecv( pxcontext, pvBuffer, xBufferLength );
        }
    }

    return lstatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Transmit data to the remote socket.
 *
 * The socket must have already been created using a call to SOCKETS_Socket() 
 * and connected to a remote socket using SOCKETS_Connect().
 *
 * @param[in] xSocket The handle of the sending socket.
 * @param[in] pvBuffer The buffer containing the data to be sent.
 * @param[in] xDataLength The length of the data to be sent.
 * @param[in] ulFlags Not currently used. Should be set to 0.
 *
 * @return
 * * On success, the number of bytes actually sent is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Send( Socket_t xSocket,
                        const void * pvBuffer,
                        size_t xDataLength,
                        uint32_t ulFlags )
{
    int32_t lstatus = SOCKETS_SOCKET_ERROR;
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) xSocket;

    /* Cast to an appropriate type */
    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        /* Cast to an appropriate type */
        ( pvBuffer != NULL ) )
    {
        /* cast to BaseType_t */
        pxcontext->xsend_flags = ( BaseType_t ) ulFlags;

        /* Cast to an appropriate type */
        if( pdTRUE == pxcontext->xrequire_tls )
        {
            /* Send through TLS pipe, if negotiated. */
            lstatus = TLS_Send( pxcontext->pvtls_context, pvBuffer, xDataLength );
        }
        else
        {
            /* Send unencrypted. */
            lstatus = prvNetworkSend( pxcontext, pvBuffer, xDataLength );
        }
    }

    return lstatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Closes all or part of a full-duplex connection on the socket.
 *
 * @param[in] xSocket The handle of the socket to shutdown.
 * @param[in] ulHow SOCKETS_SHUT_RD, SOCKETS_SHUT_WR or SOCKETS_SHUT_RDWR.
 * @ref ShutdownFlags
 *
 * @return
 * * If the operation was successful, 0 is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Shutdown( Socket_t xSocket, uint32_t ulHow )
{
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) xSocket;

    return esp8266_tcp_disconnect(pxcontext->socket_no);
}
/*-----------------------------------------------------------*/

/**
 * @brief Closes the socket and frees the related resources.
 *
 * @param[in] xSocket The handle of the socket to close.
 *
 * @return
 * * On success, 0 is returned.
 * * If an error occurred, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Close( Socket_t xSocket )
{
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) xSocket;

    /* Cast to an appropriate type */
    if( NULL != pxcontext )
    {
        /* Cast to an appropriate type */
        if( NULL != pxcontext->pcdestination )
        {
            vPortFree( pxcontext->pcdestination );
        }

        /* Cast to an appropriate type */
        if( NULL != pxcontext->pcserver_certificate )
        {
            vPortFree( pxcontext->pcserver_certificate );
        }

        /* Cast to an appropriate type */
        if( pdTRUE == pxcontext->xrequire_tls )
        {
            TLS_Cleanup( pxcontext->pvtls_context );
        }

        esp8266_tcp_disconnect(pxcontext->socket_no);
        vPortFree( pxcontext );
    }

    if(ssockets_num_allocated > 0)
    {
        ssockets_num_allocated--;
    }
    else
    {
        ssockets_num_allocated = 0;
    }
    return pdFREERTOS_ERRNO_NONE;
}
/*-----------------------------------------------------------*/

/**
 * @brief Manipulates the options for the socket.
 *
 * @param[in] xSocket The handle of the socket to set the option for.
 * @param[in] lLevel Not currently used. Should be set to 0.
 * @param[in] lOptionName See @ref SetSockOptOptions.
 * @param[in] pvOptionValue A buffer containing the value of the option to set.
 * @param[in] xOptionLength The length of the buffer pointed to by 
 *            pvOptionValue.
 *
 * @note Socket option support and possible values vary by port. Please see
 * PORT_SPECIFIC_LINK to check the valid options and limitations of your 
 * device.
 *
 *  - Berkeley Socket Options
 *    - @ref SOCKETS_SO_RCVTIMEO
 *      - Sets the receive timeout
 *      - pvOptionValue (TickType_t) is the number of milliseconds that the
 *      receive function should wait before timing out.
 *      - Setting pvOptionValue = 0 causes receive to wait forever.
 *      - See PORT_SPECIFIC_LINK for device limitations.
 *    - @ref SOCKETS_SO_SNDTIMEO
 *      - Sets the send timeout
 *      - pvOptionValue (TickType_t) is the number of milliseconds that the
 *      send function should wait before timing out.
 *      - Setting pvOptionValue = 0 causes send to wait forever.
 *      - See PORT_SPECIFIC_LINK for device limitations.
 *  - Non-Standard Options
 *    - @ref SOCKETS_SO_NONBLOCK
 *      - Makes a socket non-blocking.
 *      - pvOptionValue is ignored for this option.
 *  - Security Sockets Options
 *    - @ref SOCKETS_SO_REQUIRE_TLS
 *      - Use TLS for all connect, send, and receive on this socket.
 *      - This socket options MUST be set for TLS to be used, even
 *        if other secure socket options are set.
 *      - pvOptionValue is ignored for this option.
 *    - @ref SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE
 *      - Set the root of trust server certificiate for the socket.
 *      - This socket option only takes effect if @ref SOCKETS_SO_REQUIRE_TLS
 *        is also set.  If @ref SOCKETS_SO_REQUIRE_TLS is not set,
 *        this option will be ignored.
 *      - pvOptionValue is a pointer to the formatted server certificate.
 *        TODO: Link to description of how to format certificates with \n
 *      - xOptionLength (BaseType_t) is the length of the certificate
 *        in bytes.
 *    - @ref SOCKETS_SO_SERVER_NAME_INDICATION
 *      - Use Server Name Indication (SNI)
 *      - This socket option only takes effect if @ref SOCKETS_SO_REQUIRE_TLS
 *        is also set.  If @ref SOCKETS_SO_REQUIRE_TLS is not set,
 *        this option will be ignored.
 *      - pvOptionValue is a pointer to a string containing the hostname
 *      - xOptionLength is the length of the hostname string in bytes.
 *
 * @return
 * * On success, 0 is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    int32_t lstatus = SOCKETS_ERROR_NONE;
    TickType_t xtimeout;
    /*lint !e9087 cast used for portability. */
    /* Cast to an appropriate type */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) xSocket;

    switch( lOptionName )
    {
        case SOCKETS_SO_SERVER_NAME_INDICATION:

            /* Non-NULL destination string indicates that SNI extension should
             * be used during TLS negotiation. */
            if( NULL == ( pxcontext->pcdestination =
                                /* cast to char * */
                                ( char * ) pvPortMalloc( 1U + xOptionLength ) ) )
            {
                lstatus = SOCKETS_ENOMEM;
            }
            else
            {
                memcpy( pxcontext->pcdestination, pvOptionValue, xOptionLength );
                pxcontext->pcdestination[ xOptionLength ] = '\0';
            }

            break;

        case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

            /* Non-NULL server certificate field indicates that the default 
             * trust list should not be used. */
            if( NULL == ( pxcontext->pcserver_certificate =
                            /* cast to char * */
                            ( char * ) pvPortMalloc( xOptionLength ) ) )
            {
                lstatus = SOCKETS_ENOMEM;
            }
            else
            {
                memcpy( pxcontext->pcserver_certificate, pvOptionValue, xOptionLength );
                pxcontext->ulserver_certificate_length = xOptionLength;
            }

            break;

        case SOCKETS_SO_REQUIRE_TLS:
            /* Cast to an appropriate type */
            pxcontext->xrequire_tls = pdTRUE;
            break;

        case SOCKETS_SO_NONBLOCK:
            pxcontext->ulsend_timeout = 1000;
            pxcontext->ulrecv_timeout = 2;
            break;

        case SOCKETS_SO_RCVTIMEO:
            /* Comply with Berkeley standard - a 0 timeout is wait forever. */
            /*lint !e9087 pvOptionValue passed should be of TickType_t */
            xtimeout = *( ( const TickType_t * ) pvOptionValue );

            if( xtimeout == 0U )
            {
                /* Cast to an appropriate type */
                xtimeout = portMAX_DELAY;
            }
            pxcontext->ulrecv_timeout = xtimeout;
            esp8266_serial_tcp_recv_timeout_set(pxcontext->socket_no, xtimeout);
            break;
        case SOCKETS_SO_SNDTIMEO:
            /* Comply with Berkeley standard - a 0 timeout is wait forever. */
            /*lint !e9087 pvOptionValue passed should be of TickType_t */
            xtimeout = *( ( const TickType_t * ) pvOptionValue );

            if( xtimeout == 0U )
            {
                /* Cast to an appropriate type */
                xtimeout = portMAX_DELAY;
            }
            pxcontext->ulsend_timeout = xtimeout;
            break;

        default:
            lstatus = SOCKETS_ENOPROTOOPT;
            break;
    }

    return lstatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Resolve a host name using Domain Name Service.
 *
 * @param[in] pcHostName The host name to resolve.
 * @return
 * * The IPv4 address of the specified host.
 * * If an error has occured, 0 is returned.
 */
uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    uint32_t uladdr = 0;
    uint32_t ret;

    /* Cast to uint8_t * */
    ret = esp8266_dns_query((uint8_t *)pcHostName, &uladdr);
    if(0 == ret)
    {
        /* Cast to an appropriate type */
        uladdr = SOCKETS_htonl( uladdr );
    }
    return uladdr;
}
/*-----------------------------------------------------------*/

/**
 * @brief Secure Sockets library initialization function.
 *
 * This function does general initialization and setup. It must be called once
 * and only once before calling any other function.
 *
 * @return
 * * @ref pdPASS if everything succeeds
 * * @ref pdFAIL otherwise.
 */
BaseType_t SOCKETS_Init( void )
{
    /* FIX ME. */
    BaseType_t xresult = pdFAIL;

    /* Create the global mutex which is used to ensure
     * that only one socket is accessing the ucInUse bits in
     * the socket array. */

    if(0 == esp8266_socket_init())
    {
        /* Cast to an appropriate type */
        xresult = pdPASS;
    }
    /* Success. */

    return xresult;
}
/*-----------------------------------------------------------*/

static CK_RV prvSocketsGetCryptoSession( CK_SESSION_HANDLE * pxSession,
                                            CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{
    CK_RV xresult = 0;
    /* Cast to an appropriate type */
    CK_C_GetFunctionList pxck_get_function_list = NULL;
    static CK_SESSION_HANDLE xpkcs11_session = 0;
    /* Cast to an appropriate type */
    static CK_FUNCTION_LIST_PTR pxpkcs11_function_list = NULL;
    CK_ULONG ulcount = 1;
    CK_SLOT_ID xslot_id = 0;

    portENTER_CRITICAL();

    if( 0 == xpkcs11_session )
    {
        /* One-time initialization. */

        /* Ensure that the PKCS#11 module is initialized. */
        if( 0 == xresult )
        {
            pxck_get_function_list = C_GetFunctionList;
            xresult = pxck_get_function_list( &pxpkcs11_function_list );
        }

        if( 0 == xresult )
        {
            /* Cast to an appropriate type */
            xresult = pxpkcs11_function_list->C_Initialize( NULL );
        }

        /* Get the default slot ID. */
        if( ( 0 == xresult ) || 
            ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xresult ) )
        {
            xresult = pxpkcs11_function_list->C_GetSlotList( CK_TRUE,
                                                            &xslot_id,
                                                            &ulcount );
        }

        /* Start a session with the PKCS#11 module. */
        if( 0 == xresult )
        {
            xresult = pxpkcs11_function_list->C_OpenSession( xslot_id,
                                                            CKF_SERIAL_SESSION,
                                                            /* Cast to an appropriate type */
                                                            NULL,
                                                            /* Cast to an appropriate type */
                                                            NULL,
                                                            &xpkcs11_session );
        }
    }

    portEXIT_CRITICAL();

    /* Output the shared function pointers and session handle. */
    *ppxFunctionList = pxpkcs11_function_list;
    *pxSession = xpkcs11_session;

    return xresult;
}
/*-----------------------------------------------------------*/

uint32_t ulRand( void )
{
    CK_RV xResult = 0;
    CK_SESSION_HANDLE xPkcs11Session = 0;
    CK_FUNCTION_LIST_PTR pxPkcs11FunctionList = NULL;
    uint32_t ulRandomValue = 0;

    xResult = prvSocketsGetCryptoSession( &xPkcs11Session,
                                          &pxPkcs11FunctionList );

    if( 0 == xResult )
    {
        /* Request a sequence of cryptographically random byte values using
         * PKCS#11. */
        xResult = pxPkcs11FunctionList->C_GenerateRandom( xPkcs11Session,
                                                          ( CK_BYTE_PTR ) &ulRandomValue,
                                                          sizeof( ulRandomValue ) );
    }

    /* Check if any of the API calls failed. */
    if( 0 != xResult )
    {
        ulRandomValue = 0;
    }

    return ulRandomValue;
}

/*-----------------------------------------------------------*/

/**
 * @brief Generate a TCP Initial Sequence Number that is reasonably difficult
 * to predict, per https://tools.ietf.org/html/rfc6528.
 */
uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                                uint16_t usSourcePort,
                                                uint32_t ulDestinationAddress,
                                                uint16_t usDestinationPort )
{
    CK_RV xresult = 0;
    CK_SESSION_HANDLE xpkcs11_session = 0;
    /* Cast to an appropriate type */
    CK_FUNCTION_LIST_PTR pxpkcs11_function_list = NULL;
    CK_MECHANISM xmech_sha256 = { 0 };
    uint8_t ucsha256_result[ cryptoSHA256_DIGEST_BYTES ];
    CK_ULONG ullength = sizeof( ucsha256_result );
    uint32_t ulnext_sequence_number = 0;
    static uint64_t ullkey = 0;

    /* Acquire a crypto session handle. */
    xresult = prvSocketsGetCryptoSession( &xpkcs11_session,
                                            &pxpkcs11_function_list );

    if( 0 == xresult )
    {
        if( 0 == ullkey )
        {
            /* One-time initialization, per boot, of the random seed. */
            xresult = pxpkcs11_function_list->C_GenerateRandom( xpkcs11_session,
                                                                /* cast to CK_BYTE_PTR */
                                                                ( CK_BYTE_PTR ) &ullkey,
                                                                sizeof( ullkey ) );
        }
    }

    /* Lock the shared crypto session. */
    portENTER_CRITICAL();

    /* Start a hash. */
    if( 0 == xresult )
    {
        xmech_sha256.mechanism = CKM_SHA256;
        xresult = pxpkcs11_function_list->C_DigestInit( xpkcs11_session, &xmech_sha256 );
    }

    /* Hash the seed. */
    if( 0 == xresult )
    {
        xresult = pxpkcs11_function_list->C_DigestUpdate( xpkcs11_session,
                                                        /* cast to CK_BYTE_PTR */
                                                        ( CK_BYTE_PTR ) &ullkey,
                                                        sizeof( ullkey ) );
    }

    /* Hash the source address. */
    if( 0 == xresult )
    {
        xresult = pxpkcs11_function_list->C_DigestUpdate( xpkcs11_session,
                                                        /* cast to CK_BYTE_PTR */
                                                        ( CK_BYTE_PTR ) &ulSourceAddress,
                                                        sizeof( ulSourceAddress ) );
    }

    /* Hash the source port. */
    if( 0 == xresult )
    {
        xresult = pxpkcs11_function_list->C_DigestUpdate( xpkcs11_session,
                                                        /* cast to CK_BYTE_PTR */
                                                        ( CK_BYTE_PTR ) &usSourcePort,
                                                        sizeof( usSourcePort ) );
    }

    /* Hash the destination address. */
    if( 0 == xresult )
    {
        xresult = pxpkcs11_function_list->C_DigestUpdate( xpkcs11_session,
                                                        /* cast to CK_BYTE_PTR */
                                                        ( CK_BYTE_PTR ) &ulDestinationAddress,
                                                        sizeof( ulDestinationAddress ) );
    }

    /* Hash the destination port. */
    if( 0 == xresult )
    {
        xresult = pxpkcs11_function_list->C_DigestUpdate( xpkcs11_session,
                                                        /* cast to CK_BYTE_PTR */
                                                        ( CK_BYTE_PTR ) &usDestinationPort,
                                                        sizeof( usDestinationPort ) );
    }

    /* Get the hash. */
    if( 0 == xresult )
    {
        xresult = pxpkcs11_function_list->C_DigestFinal( xpkcs11_session,
                                                        ucsha256_result,
                                                        &ullength );
    }

    portEXIT_CRITICAL();

    /* Use the first four bytes of the hash result as the starting point for
     * all initial sequence numbers for connections based on the input 
     * 4-tuple. */
    if( 0 == xresult )
    {
        memcpy( &ulnext_sequence_number,
                ucsha256_result,
                sizeof( ulnext_sequence_number ) );

        /* Add the tick count of four-tick intervals. In theory, per the RFC
         * (see above), this approach still allows server equipment to 
         * optimize handling of connections from the same device that haven't
         * fully timed out. */
        ulnext_sequence_number += (xTaskGetTickCount() / 4);
    }

    return ulnext_sequence_number;
}

static uint8_t get_socket_no(void *pvContext)
{
    /* Cast to an appropriate type */
    /*lint !e9087 cast used for portability. */
    sscoket_context_ptr_t pxcontext = ( sscoket_context_ptr_t ) pvContext;

    return pxcontext->socket_no;
}
/*-----------------------------------------------------------*/
