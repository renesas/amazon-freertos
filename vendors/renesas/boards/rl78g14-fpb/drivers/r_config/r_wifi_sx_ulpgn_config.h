#ifndef R_WIFI_SX_ULPGN_CONFIG_H
#define R_WIFI_SX_ULPGN_CONFIG_H

#include "rl_platform.h"

#define WIFI_CFG_SCI_CHANNEL					0

#define WIFI_CFG_SCI_INTERRUPT_LEVEL    		14

#define WIFI_CFG_SCI_SECOND_CHANNEL     		2

#define WIFI_CFG_SCI_BAUDRATE                   460800

#define WIFI_CFG_SCI_USE_FLOW_CONTROL			1

#if(__CCRX__)
#define WIFI_CFG_RESET_PORT					D
#define WIFI_CFG_RESET_PIN					0

#define WIFI_CFG_RTS_PORT					2
#define WIFI_CFG_RTS_PIN					2
#elif(__CCRL__)
#define WIFI_CFG_RESET_PORT_PODR                P13_bit.no0
#define WIFI_CFG_UART_DEFAULT_SCI_RTS_PODR      P3_bit.no0
#endif

//#define WIFI_CFG_CREATABLE_SOCKETS   			4
#define WIFI_CFG_CREATABLE_SOCKETS   			2

#if(__CCRX__)
#define WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE  	( 8192 )
#elif(__CCRL__)
#define WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE  	(512) /*( 2048 )*/
#endif

#define WIFI_CFG_USE_CALLBACK_FUNCTION   		0

#define WIFI_CFG_CALLBACK_FUNCTION_NAME   		NULL

#endif /* #define R_WIFI_SX_ULPGN_CONFIG_H */
