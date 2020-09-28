
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include <stdio.h>
#include <string.h>

/* Renesas. */
#include "serial_term_uart.h"

/* Test application include. */
#include "aws_test_runner.h"

/* Aws Library Includes includes. */
#include "iot_wifi.h"
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "aws_clientcredential.h"
#include "aws_application_version.h"
#include "aws_dev_mode_key_provisioning.h"

#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 8 )

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
    	vTaskDelay(10000);
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

        /* We should wait for the network to be up before we run any demos. */
        while( FreeRTOS_IsNetworkUp() == pdFALSE )
        {
            vTaskDelay(300);
        }
		FreeRTOS_printf( ( "The network is up and running\n" ) );
#endif
		FreeRTOS_printf( ( "The network is up and running\n" ) );

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

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )

const char * pcApplicationHostnameHook( void )
{
    /* Assign the name "FreeRTOS" to this network node.  This function will
     * be called during the DHCP: the machine will be registered with an IP
     * address plus this name. */
    return clientcredentialIOT_THING_NAME;
}

#endif
