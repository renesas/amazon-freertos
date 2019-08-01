#ifndef ESP32_DRIVER_H
#define ESP32_DRIVER_H

#include "r_sci_rx_if.h"
#include "r_byteq_if.h"

#define DEBUGLOG  0
#define ESP8266_PORT_DEBUG 0

/* Configuration */
#define ESP8266_USE_UART_NUM (1)



#define ESP8266_RETURN_TEXT_OK          "OK\r\n"
#define ESP8266_RETURN_TEXT_ERROR       "ERROR\r\n"
#define ESP8266_RETURN_TEXT_READY       "\r\nready\r\n"
#define ESP8266_RETURN_TEXT_OK_GO_SEND  "OK\r\n> "
#define ESP8266_RETURN_TEXT_SEND_BYTE   " bytes\r\n"
#define ESP8266_RETURN_TEXT_SEND_OK     "SEND OK\r\n"
#define ESP8266_RETURN_TEXT_SEND_FAIL   "SEND FAIL\r\n"

#define ESP8266_SOCKET_STATUS_TEXT_CLOSED          "CLOSED"
#define ESP8266_SOCKET_STATUS_TEXT_SOCKET          "SOCKET"
#define ESP8266_SOCKET_STATUS_TEXT_BOUND           "BOUND"
#define ESP8266_SOCKET_STATUS_TEXT_LISTEN          "LISTEN"
#define ESP8266_SOCKET_STATUS_TEXT_CONNECTED       "CONNECTED"

#define ESP8266_UART_COMMAND_PORT 0

#define CREATEABLE_SOCKETS (5)

#if ESP8266_PORT_DEBUG == 1
#define DEBUG_PORT4_DDR PORT4.PDR.BIT.B7    /* TFT-46 */
#define DEBUG_PORT4_DR  PORT4.PODR.BIT.B7   /* TFT-46 */
#define DEBUG_PORT7_DDR PORT1.PDR.BIT.B1    /* TFT-14 */
#define DEBUG_PORT7_DR  PORT1.PODR.BIT.B1   /* TFT-14 */
#else
#endif

#define ESP8266_UARTBOOT_PORT_DDR    PORT1.PDR.BIT.B5
#define ESP8266_UARTBOOT_PORT_DR     PORT1.PODR.BIT.B5
#define ESP8266_RESET_PORT_DDR    WIFI_RESET_PORT_PDR
#define ESP8266_RESET_PORT_DR     WIFI_RESET_PORT_PODR


typedef enum
{
	ESP8266_RETURN_OK            = 0,
	ESP8266_RETURN_ERROR,
	ESP8266_RETURN_READY,
	ESP8266_RETURN_OK_GO_SEND,
	ESP8266_RETURN_SEND_BYTE,
	ESP8266_RETURN_SEND_OK,
	ESP8266_RETURN_SEND_FAIL,
	ESP8266_RETURN_ENUM_MAX,
}sx_esp8266_return_code_t;

typedef enum
{
	ESP8266_RETURN_STRING_TEXT            = 0,
	ESP8266_RETURN_STRING_MAX,
}sx_esp8266_return_string_t;

typedef enum
{
	ESP8266_SECURITY_OPEN            = 0,
	ESP8266_SECURITY_WPA,
	ESP8266_SECURITY_WPA2,
	ESP8266_SECURITY_WEP,
	ESP8266_SECURITY_UNDEFINED,
	ESP8266_SECURITY_MAX,
}esp8266_security_t;

typedef enum
{
	ESP8266_SOCKET_STATUS_CLOSED            = 0,
	ESP8266_SOCKET_STATUS_SOCKET,
	ESP8266_SOCKET_STATUS_BOUND,
	ESP8266_SOCKET_STATUS_LISTEN,
	ESP8266_SOCKET_STATUS_CONNECTED,
	ESP8266_SOCKET_STATUS_MAX,
}esp8266_socket_status_t;

typedef enum
{
	ESP8266_SET_CWMODE_CUR = 0,
	ESP8266_SET_CIPMUX,
	ESP8266_SET_CWJAP_CUR,
	ESP8266_SET_CWLIF,
	ESP8266_GET_CIPAPMAC,
	ESP8266_GET_CIPSTA,
	ESP8266_GET_CIPSNTPTIME,
	ESP8266_SET_CIPDOMAIN,
	ESP8266_SET_CIPSTART,
	ESP8266_SET_CIPSEND,
	ESP8266_SET_CIPSEND_END,
	ESP8266_SET_CIPCLOSE,
	ESP8266_LIST_MAX
}esp8266_command_list;

typedef struct esp8266_socket_tag
{
	uint32_t receive_num;
	uint32_t receive_count;
	uint32_t put_error_count;
	uint8_t socket_recv_buff[4096];
	byteq_hdl_t socket_byteq_hdl;
	uint8_t socket_create_flag;
}esp8266_socket_t;

int32_t esp8266_wifi_init(void);
int32_t esp8266_socket_init(void);
int32_t esp8266_wifi_get_macaddr(uint8_t *ptextstring);
int32_t esp8266_wifi_connect(uint8_t *pssid, uint32_t security, uint8_t *ppass);
int32_t esp8266_socket_create(uint8_t socket_no, uint32_t type,uint32_t ipversion);
int32_t esp8266_tcp_connect(uint8_t socket_no, uint32_t ipaddr, uint16_t port);

int32_t esp8266_ssl_connect(uint8_t socket_no, uint32_t ipaddr, uint16_t port);

int32_t esp8266_tcp_send(uint8_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout);
int32_t esp8266_tcp_recv(uint8_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout);
int32_t esp8266_tcp_disconnect(uint8_t socket_no);
int32_t esp8266_dns_query(uint8_t *ptextstring, uint32_t *ulipaddr);
int32_t esp8266_serial_tcp_recv_timeout_set(uint8_t socket_no, TickType_t timeout_ms);

extern sci_hdl_t esp8266_uart_sci_handle[2];

extern uint8_t macaddress[6];
extern uint8_t ipaddress[4];
extern uint8_t subnetmask[4];
extern uint8_t gateway[4];

extern uint16_t sntp_year;
extern uint8_t sntp_mon[5];
extern uint8_t sntp_day;
extern uint8_t sntp_hour;
extern uint8_t sntp_min;
extern uint8_t sntp_sec;
extern uint8_t sntp_week[5];

extern uint8_t dnsaddress[4];


#endif /* #define ESP32_DRIVER_H */
