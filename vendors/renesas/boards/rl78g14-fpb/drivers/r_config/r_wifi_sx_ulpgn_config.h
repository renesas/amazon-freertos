#ifndef R_WIFI_SX_ULPGN_CONFIG_H
#define R_WIFI_SX_ULPGN_CONFIG_H

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#include "platform.h"
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "rl_platform.h"
#endif

#define WIFI_CFG_SCI_CHANNEL					0

#define WIFI_CFG_SCI_INTERRUPT_LEVEL    		14

#define WIFI_CFG_SCI_SECOND_CHANNEL     		2

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#define WIFI_CFG_SCI_BAUDRATE                   460800
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#define WIFI_CFG_SCI_BAUDRATE                   230400
#endif

#define WIFI_CFG_SCI_USE_FLOW_CONTROL			1

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#define WIFI_CFG_RESET_PORT					D
#define WIFI_CFG_RESET_PIN					0

#define WIFI_CFG_RTS_PORT					2
#define WIFI_CFG_RTS_PIN					2
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#define WIFI_CFG_RESET_PORT_PODR                P13_bit.no0
#define WIFI_CFG_UART_DEFAULT_SCI_RTS_PODR      P3_bit.no0
#endif

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#define WIFI_CFG_CREATABLE_SOCKETS   			4
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#define WIFI_CFG_CREATABLE_SOCKETS   			2
#endif

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#define WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE  	( 8192 )
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
//#define WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE      ( 1024*7)
//#define WIFI_CFG_SOCKETS_RECEIVE_BUFFER_THRESHOLD ( 1024*3 )

#define WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE      ( 768 )
#define WIFI_CFG_SOCKETS_RECEIVE_BUFFER_THRESHOLD ( 60 )

#endif

#define WIFI_CFG_USE_CALLBACK_FUNCTION   		0

#define WIFI_CFG_CALLBACK_FUNCTION_NAME   		NULL

#endif /* #define R_WIFI_SX_ULPGN_CONFIG_H */
