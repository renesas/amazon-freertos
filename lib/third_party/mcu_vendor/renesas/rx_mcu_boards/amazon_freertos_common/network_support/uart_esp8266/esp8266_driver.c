#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "platform.h"
#include "r_sci_rx_if.h"
#include "r_byteq_if.h"
#include "esp8266_driver.h"

#if !defined(MY_BSP_CFG_UART_WIFI_SCI)
#error "Error! Need to define MY_BSP_CFG_UART_WIFI_SCI in r_bsp_config.h"
#elif MY_BSP_CFG_UART_WIFI_SCI == (0)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI0()
#define SCI_CH_esp8266_serial_default           SCI_CH0
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH0_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH0_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (1)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI1()
#define SCI_CH_esp8266_serial_default           SCI_CH1
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH1_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH1_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (2)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI2()
#define SCI_CH_esp8266_serial_default           SCI_CH2
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH2_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH2_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (3)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI3()
#define SCI_CH_esp8266_serial_default           SCI_CH3
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH3_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH3_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (4)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI4()
#define SCI_CH_esp8266_serial_default           SCI_CH4
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH4_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH4_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (5)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI5()
#define SCI_CH_esp8266_serial_default           SCI_CH5
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH5_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH5_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (6)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI6()
#define SCI_CH_esp8266_serial_default           SCI_CH6
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH6_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH6_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (7)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI7()
#define SCI_CH_esp8266_serial_default           SCI_CH7
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH7_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH7_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (8)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI8()
#define SCI_CH_esp8266_serial_default           SCI_CH8
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH8_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH8_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (9)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI9()
#define SCI_CH_esp8266_serial_default           SCI_CH9
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH9_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH9_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (10)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI10()
#define SCI_CH_esp8266_serial_default           SCI_CH10
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH10_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH10_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (11)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI11()
#define SCI_CH_esp8266_serial_default           SCI_CH11
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH11_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH11_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (12)
#define R_SCI_PinSet_esp8266_serial_default()   R_SCI_PinSet_SCI12()
#define SCI_CH_esp8266_serial_default           SCI_CH12
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH12_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH12_RX_BUFSIZ
#else
#error "Error! Invalid setting for MY_BSP_CFG_UART_WIFI_SCI in r_bsp_config.h"
#endif


const uint8_t esp8266_return_text_ok[]          = ESP8266_RETURN_TEXT_OK;
const uint8_t esp8266_return_text_error[]       = ESP8266_RETURN_TEXT_ERROR;
const uint8_t esp8266_return_text_ready[]       = ESP8266_RETURN_TEXT_READY;
const uint8_t esp8266_return_text_ok_go_send[]  = ESP8266_RETURN_TEXT_OK_GO_SEND;
const uint8_t esp8266_return_text_send_byte[]   = ESP8266_RETURN_TEXT_SEND_BYTE;
const uint8_t esp8266_return_text_send_ok[]     = ESP8266_RETURN_TEXT_SEND_OK;
const uint8_t esp8266_return_text_send_fail[]   = ESP8266_RETURN_TEXT_SEND_FAIL;

const uint8_t esp8266_socket_status_closed[]       = ESP8266_SOCKET_STATUS_TEXT_CLOSED;
const uint8_t esp8266_socket_status_socket[]       = ESP8266_SOCKET_STATUS_TEXT_SOCKET;
const uint8_t esp8266_socket_status_bound[]        = ESP8266_SOCKET_STATUS_TEXT_BOUND;
const uint8_t esp8266_socket_status_listen[]       = ESP8266_SOCKET_STATUS_TEXT_LISTEN;
const uint8_t esp8266_socket_status_connected[]    = ESP8266_SOCKET_STATUS_TEXT_CONNECTED;

const uint8_t esp8266_return_dummy[]   = "";

const uint8_t * const esp8266_result_code[ESP8266_RETURN_ENUM_MAX][ESP8266_RETURN_STRING_MAX] =
{
	/* text mode*/                  /* numeric mode */
	{esp8266_return_text_ok,},
	{esp8266_return_text_error,},
	{esp8266_return_text_ready,},
	{esp8266_return_text_ok_go_send,},
	{esp8266_return_text_send_byte,},
	{esp8266_return_text_send_ok,},
	{esp8266_return_text_send_fail,},
};

const uint8_t * const esp8266_socket_status[ESP8266_SOCKET_STATUS_MAX] =
{
	esp8266_socket_status_closed,
	esp8266_socket_status_socket,
	esp8266_socket_status_bound,
	esp8266_socket_status_listen,
	esp8266_socket_status_connected,
};

volatile uint8_t current_socket_index;
uint8_t buff[1000];
uint8_t recvbuff[2048+20];
sci_cfg_t   g_esp8266_sci_config[2];
sci_err_t   g_esp8266_sci_err;
volatile uint32_t g_esp8266_uart_teiflag[2];
static uint8_t timeout_overflow_flag[2];
sci_hdl_t esp8266_uart_sci_handle[2];
static TickType_t starttime[2], thistime[2], endtime[2];
static TickType_t startbytetime[2], thisbytetime[2], endbytetime[2];
static uint8_t byte_timeout_overflow_flag[2];

uint8_t g_sx_esp8266_return_mode;
byteq_hdl_t socket_byteq_hdl[CREATEABLE_SOCKETS];

extern esp8266_socket_t esp8266_socket[5];

uint32_t socket_recv_error_count[4];

uint8_t debug_out_crlf;

static void esp8266_uart_callback_command_port(void *pArgs);
static void esp8266_uart_callback_data_port(void *pArgs);
static void timeout_init(uint8_t socket_no, uint16_t timeout_ms);
static void bytetimeout_init(uint8_t socket_no, uint16_t timeout_ms);
static int32_t check_timeout(uint8_t socket_no, int32_t rcvcount);
static int32_t check_bytetimeout(uint8_t socket_no, int32_t rcvcount);
static int32_t esp8266_serial_open(uint32_t);
static int32_t esp8266_serial_close(void);
static int32_t esp8266_serial_data_port_open(void);
static int32_t esp8266_serial_send_basic(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code);
static int32_t esp8266_serial_send_rst(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code, sx_esp8266_return_code_t end_string);
static int32_t esp8266_serial_send_with_recvtask(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code,  uint8_t command, uint8_t socket_no);
static int32_t esp8266_get_socket_status(uint8_t socket_no);
static int32_t esp8266_change_socket_index(uint8_t socket_no);
static TickType_t g_sl_esp8266_tcp_recv_timeout = 3000;		/* ## slowly problem ## unit: 1ms */
static int32_t esp8266_tcp_send_returncode_check(void);

/**
 * @brief The global mutex to ensure that only one operation is accessing the
 * g_esp8266_semaphore flag at one time.
 */
static SemaphoreHandle_t g_esp8266_semaphore = NULL;

/**
 * @brief Maximum time in ticks to wait for obtaining a semaphore.
 */
static const TickType_t xMaxSemaphoreBlockTime = pdMS_TO_TICKS( 60000UL );


int32_t esp8266_wifi_init(void)
{
	int32_t ret;
#if ESP8266_PORT_DEBUG == 1
	DEBUG_PORT4_DDR = 1;
	DEBUG_PORT4_DR = 0;
	DEBUG_PORT7_DDR = 1;
	DEBUG_PORT7_DR = 0;
#endif
	/* Wifi Module hardware reset   */
//	ESP8266_UARTBOOT_PORT_DDR = 1;
//	ESP8266_UARTBOOT_PORT_DR = 0;
	ret = esp8266_serial_open(76800);
	if(ret != 0)
	{
		return ret;
	}

//	ret = esp8266_serial_send_basic2(ESP8266_UART_COMMAND_PORT, NULL, 1000, 20000, ESP8266_RETURN_OK, ESP8266_RETURN_READY);

	/* IO15 = LOW */
	PORTC.PDR.BIT.B4 = 1;
	PORTC.PODR.BIT.B4 = 0;

	/* IO0 = HIGH */
	PORT1.PDR.BIT.B5 = 1;
	PORT1.PODR.BIT.B5 = 1;

	//	ESP8266_UARTBOOT_PORT_DDR = 1;
	//	ESP8266_UARTBOOT_PORT_DR = 0;

//	ESP8266_RESET_PORT_DDR = 1;
//	ESP8266_RESET_PORT_DR = 0; /* Low */
	R_BSP_SoftwareDelay(26, BSP_DELAY_MILLISECS); /* 5us mergin 1us */
//	ESP8266_RESET_PORT_DR = 1; /* High */
	R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 5us mergin 1us */

	ret = esp8266_serial_send_rst(ESP8266_UART_COMMAND_PORT, NULL, 1000, 20000, ESP8266_RETURN_OK, ESP8266_RETURN_READY);

	esp8266_serial_close();
	ret = esp8266_serial_open(115200);
	if(ret != 0)
	{
		return ret;
	}

	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT\r\n", 1000, 20000, ESP8266_RETURN_OK);

	/* reboots the system */
	ret = esp8266_serial_send_rst(ESP8266_UART_COMMAND_PORT, "AT+RST\r\n", 1000, 20000, ESP8266_RETURN_OK, ESP8266_RETURN_READY);
	if(ret != 0)
	{
		ret = esp8266_serial_send_rst(ESP8266_UART_COMMAND_PORT, "AT+RST\r\n", 1000, 20000, ESP8266_RETURN_OK, ESP8266_RETURN_READY);
		if(ret != 0)
		{
			return ret;
		}
	}
	g_sx_esp8266_return_mode = 0;
	R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 5us mergin 1us */

	/* no echo */
	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "ATE0\r\n", 3, 1000, ESP8266_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}

	ret = esp8266_serial_send_basic(0, "AT+GMR\r\n", 3, 200, ESP8266_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}

//	g_sx_esp8266_return_mode = 1;
//	/* result code format = numeric */
//	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "ATV0\r", 3, 200, ESP8266_RETURN_OK);
//	if(ret != 0)
//	{
//		return ret;
//	}

//	/* Escape guard time = 200msec */
//	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "ATS12=1\r", 3, 200, ESP8266_RETURN_OK);
//	if(ret != 0)
//	{
//		return ret;
//	}

	R_SCI_Control(esp8266_uart_sci_handle[ESP8266_UART_COMMAND_PORT], SCI_CMD_RX_Q_FLUSH, NULL);
	ret = esp8266_serial_send_basic(0, "AT+UART_CUR=115200,8,1,0,0\r\n", 3, 200, ESP8266_RETURN_OK);
	ret = esp8266_serial_send_basic(0, "AT+UART_CUR?\r\n", 3, 200, ESP8266_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}

	/* Disconnect from currently connected Access Point, */
	ret = esp8266_serial_send_basic(0, "AT+CWQAP\r\n", 3, 200, ESP8266_RETURN_OK);
//	if(ret != 0)
//	{
//		return ret;
//	}
	current_socket_index = 0;

	/* SoftAP station mode is enabled. */
	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CWMODE_CUR=3\r\n", 3, 200, ESP8266_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}



	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPMUX=1\r\n", 3, 200, ESP8266_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}


//	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "ATTO=1\r", 3, 1000, ESP8266_RETURN_OK);
//	if(ret != 0)
//	{
//		return ret;
//	}

#if (0)
	/* Command Port = HSUART1(PMOD-UART), Data Port = Debug　UART */
	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "ATUART=1,0\r", 3, 200, ESP8266_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}

	ret = esp8266_serial_data_port_open();
	if(ret != 0)
	{
		return ret;
	}

	ret = esp8266_serial_send_basic(ESP8266_UART_DATA_PORT, NULL, 1000, 200, ESP8266_RETURN_OK);
#endif


	return 0;
}

int32_t esp8266_wifi_connect(uint8_t *pssid, uint32_t security, uint8_t *ppass)
{
	int32_t ret;
	char *pstr,pstr2;
	int32_t line_len;
	int32_t scanf_ret;
	volatile char secu[3][10];
	uint32_t security_encrypt_type = 1;


	strcpy((char *)buff,"AT+CWJAP_CUR=\"");
	strcat((char *)buff,(const char *)pssid);
	strcat((char *)buff,"\",\"");
	strcat((char *)buff,(const char *)ppass);
	strcat((char *)buff,"\"\r\n");

	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, buff, 10000, 20000, ESP8266_RETURN_OK);
	if(0 == ret)
	{
		uint32_t addr;
		ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPAPMAC?\r\n", 3, 5000, ESP8266_RETURN_OK);
		ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPSTA?\r\n", 3, 5000, ESP8266_RETURN_OK);

		ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPSNTPCFG=1,0\r\n", 3, 200, ESP8266_RETURN_OK);
		vStart_esp8266_recv_task();
        vTaskDelay( 2 );
		ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, "AT+CIPSNTPTIME?\r\n", 3, 200, ESP8266_RETURN_OK, ESP8266_GET_CIPSNTPTIME, 0xff);
	}
	return ret;
}

int32_t esp8266_wifi_get_macaddr(uint8_t *ptextstring)
{
	return esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "AT+CIPAPMAC?\r\n", 3, 200, ESP8266_RETURN_OK);
}

int32_t esp8266_socket_create(uint8_t socket_no, uint32_t type,uint32_t ipversion)
{
	int32_t ret;

    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {

	#if DEBUGLOG == 1
		R_BSP_CpuInterruptLevelWrite (14);
		printf("esp8266_socket_create(%d)\r\n",socket_no);
		R_BSP_CpuInterruptLevelWrite (0);
	#endif
		if(	esp8266_socket[socket_no].socket_create_flag == 1)
		{
	        ( void ) xSemaphoreGive( g_esp8266_semaphore );
			return -1;
		}

		esp8266_socket[socket_no].socket_create_flag = 1;
        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( g_esp8266_semaphore );
    }
    else
    {
    	return -1;
    }

	return 0;
}


int32_t esp8266_tcp_connect(uint8_t socket_no, uint32_t ipaddr, uint16_t port)
{
	int32_t ret;
    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
		if(	esp8266_socket[socket_no].socket_create_flag == 0)
		{
			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_esp8266_semaphore );
			return -1;
		}

	#if DEBUGLOG == 1
		R_BSP_CpuInterruptLevelWrite (14);
		printf("esp8266_tcp_connect(%d)\r\n",socket_no);
		R_BSP_CpuInterruptLevelWrite (0);
	#endif
		strcpy((char *)buff,"AT+CIPSTART=");
		sprintf((char *)buff+strlen((char *)buff),"%d,\"TCP\",\"%d.%d.%d.%d\",%d\r\n",socket_no, (uint8_t)(ipaddr>>24),(uint8_t)(ipaddr>>16),(uint8_t)(ipaddr>>8),(uint8_t)(ipaddr),port);

		ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, buff, 3, 10000, ESP8266_RETURN_OK, ESP8266_SET_CIPSTART, socket_no);

		/* Give back the socketInUse mutex. */
		( void ) xSemaphoreGive( g_esp8266_semaphore );

    }
    else
    {
    	return -1;
    }
	return ret;

}

int32_t esp8266_tcp_send(uint8_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
	int32_t timeout;
	volatile int32_t sended_length;
	int32_t lenghttmp1;
//	int32_t lenghttmp2;
	int32_t ret;
	sci_err_t ercd;
	uint8_t result;
//	sci_baud_t baud;

    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
    	if(0 == esp8266_socket[socket_no].socket_create_flag)
    	{
			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_esp8266_semaphore );
			return -1;
    	}
	#if DEBUGLOG == 1
		R_BSP_CpuInterruptLevelWrite (14);
		printf("esp8266_tcp_send(%d),len=%d\r\n",socket_no,length);
		R_BSP_CpuInterruptLevelWrite (0);
	#endif
		sended_length = 0;
		lenghttmp1 = SCI_TX_BUSIZ_DEFAULT;
		while(sended_length < length)
		{
			if((length - sended_length) > 2048)
			{
				lenghttmp1 = 2048;
			}
			else
			{
				lenghttmp1 = (length - sended_length);
			}

			strcpy((char *)buff,"AT+CIPSEND=");
			sprintf((char *)buff+strlen((char *)buff),"%d,%d\r\n",socket_no,lenghttmp1);

			ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, buff, 3, 10000, ESP8266_RETURN_OK_GO_SEND, ESP8266_SET_CIPSEND, socket_no);

			if(ret != 0)
			{
#if DEBUGLOG == 1
	R_BSP_CpuInterruptLevelWrite (14);
	printf("esp8266_serial_send_with_recvtask.error\r\n");
	R_BSP_CpuInterruptLevelWrite (0);
#endif
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_esp8266_semaphore );
				nop();
				return -1;
			}

			timeout_init(0, timeout_ms);
			timeout = 0;

			esp8266_response_set_queue( ESP8266_SET_CIPSEND_END, socket_no );
			g_esp8266_uart_teiflag[ESP8266_UART_COMMAND_PORT] = 0;
			ercd = R_SCI_Send(esp8266_uart_sci_handle[ESP8266_UART_COMMAND_PORT], pdata+sended_length, lenghttmp1);
			if(SCI_SUCCESS != ercd)
			{
#if DEBUGLOG == 1
	R_BSP_CpuInterruptLevelWrite (14);
	printf("R_SCI_Send\r\n");
	R_BSP_CpuInterruptLevelWrite (0);
#endif
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_esp8266_semaphore );
				return -1;
			}

#if 1
			while(1)
			{
				if(0 != g_esp8266_uart_teiflag[ESP8266_UART_COMMAND_PORT])
				{
					break;
				}
			}
#else
			while(1)
			{
				if(0 != g_esp8266_uart_teiflag[ESP8266_UART_COMMAND_PORT])
				{
					break;
				}
				if(-1 == check_timeout(0, 0))
				{
					timeout = 1;
					break;
				}

			}
			if(timeout == 1 )
			{
#if DEBUGLOG == 1
	R_BSP_CpuInterruptLevelWrite (14);
	printf("g_esp8266_uart_teiflag timeout\r\n");
	R_BSP_CpuInterruptLevelWrite (0);
#endif
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_esp8266_semaphore );
				return -1;
			}
#endif
			timeout_init(0, timeout_ms);

			while(1)
			{
				ercd = esp8266_response_get_queue( ESP8266_SET_CIPSEND_END, socket_no, &result);
				if(0 == ercd)
				{
					break;
				}
#if 0
				if(-1 == check_timeout(0, 0))
				{
					timeout = 1;
					break;
				}
#endif
			}

			if(result != ESP8266_RETURN_SEND_OK)
			{
#if DEBUGLOG == 1
	R_BSP_CpuInterruptLevelWrite (14);
	printf("esp8266_response_get_queue error\r\n");
	R_BSP_CpuInterruptLevelWrite (0);
#endif
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_esp8266_semaphore );
				nop();
				return -1;
			}

			sended_length += lenghttmp1;

		}

	#if DEBUGLOG == 1
		R_BSP_CpuInterruptLevelWrite (14);
		printf("tcp %d byte send\r\n",sended_length);
		R_BSP_CpuInterruptLevelWrite (0);
	#endif


		/* Give back the socketInUse mutex. */
		( void ) xSemaphoreGive( g_esp8266_semaphore );
    }
    else
    {
#if DEBUGLOG == 1
	R_BSP_CpuInterruptLevelWrite (14);
	printf("xSemaphoreTake error\r\n");
	R_BSP_CpuInterruptLevelWrite (0);
#endif
    	return -1;
    }
	return sended_length;
}

int32_t esp8266_tcp_recv(uint8_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
	sci_err_t ercd;
	uint32_t recvcnt = 0;
	int scanf_ret;
	uint32_t recv_socket_id;
	uint32_t recv_length;
	uint32_t stored_len;
	uint32_t i;
	volatile int32_t timeout;
	int32_t ret;
	byteq_err_t byteq_ret;
#if DEBUGLOG == 1
	TickType_t tmptime2;
#endif
	TickType_t tmptime1;

    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
    	if(0 == esp8266_socket[socket_no].socket_create_flag)
    	{
			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_esp8266_semaphore );
			return -1;
    	}
#if DEBUGLOG == 1
	R_BSP_CpuInterruptLevelWrite (14);
	printf("esp8266_tcp_recv(%d)\r\n",socket_no);
	R_BSP_CpuInterruptLevelWrite (0);
#endif
		timeout_init(0, timeout_ms);

        stored_len = 0;
    	while(1)
    	{
			byteq_ret = R_BYTEQ_Get(esp8266_socket[socket_no].socket_byteq_hdl, (pdata + stored_len));
			if(BYTEQ_ERR_QUEUE_EMPTY == byteq_ret)
			{
				if(-1 == check_timeout(0, 0))
				{
					timeout = 1;
					break;
				}
			}
			if(BYTEQ_SUCCESS == byteq_ret)
			{
				stored_len++;
				if(stored_len >= length)
				{
					#if DEBUGLOG == 1
						tmptime2 = xTaskGetTickCount();
						R_BSP_CpuInterruptLevelWrite (14);
						printf("r:%06d:tcp %ld byte received.reqsize=%ld\r\n",tmptime2, stored_len, length);
						R_BSP_CpuInterruptLevelWrite (0);
					#endif
					/* Give back the socketInUse mutex. */
					( void ) xSemaphoreGive( g_esp8266_semaphore );

					return length;
				}
			}
    	}

		/* Give back the socketInUse mutex. */
		( void ) xSemaphoreGive( g_esp8266_semaphore );
    }
    else
    {
    	return -1;
    }
	return stored_len;
}

int32_t esp8266_serial_tcp_recv_timeout_set(uint8_t socket_no, TickType_t timeout_ms)
{
	g_sl_esp8266_tcp_recv_timeout = timeout_ms;
	return 0;
}

int32_t esp8266_tcp_disconnect(uint8_t socket_no)
{
	int32_t ret = 0;
    if( xSemaphoreTake( g_esp8266_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
		if(1 == esp8266_socket[socket_no].socket_create_flag)
		{
#if DEBUGLOG == 1
			R_BSP_CpuInterruptLevelWrite (14);
			printf("esp8266_tcp_disconnect(%d)\r\n",socket_no);
			R_BSP_CpuInterruptLevelWrite (0);
#endif
#if 0
#if ESP8266_USE_UART_NUM == 1
			R_BSP_SoftwareDelay(201, BSP_DELAY_MILLISECS); /* 1s */
			R_SCI_Control(esp8266_uart_sci_handle[ESP8266_UART_COMMAND_PORT],SCI_CMD_RX_Q_FLUSH,NULL);
			ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "+++", 3, 1000, ESP8266_RETURN_OK);
#endif
#endif
			sprintf((char *)buff,"AT+CIPCLOSE=%d\r\n",socket_no);
			ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, buff, 3, 10000, ESP8266_RETURN_OK, ESP8266_SET_CIPCLOSE, socket_no);

//			ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, buff, 3, 1000, ESP8266_RETURN_OK);
			if(0 == ret)
			{
				esp8266_socket[socket_no].socket_create_flag = 0;
#if ESP8266_USE_UART_NUM == 2
				R_BYTEQ_Flush(esp8266_socket[socket_no].socket_byteq_hdl);
#endif
			}
			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_esp8266_semaphore );
		}
		else
		{
			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_esp8266_semaphore );
			return -1;
		}
    }
    else
    {
    	return -1;
    }
	return ret;

}

int32_t esp8266_dns_query(uint8_t *ptextstring, uint32_t *ulipaddr)
{
	uint32_t result;
	int32_t func_ret;
	int32_t scanf_ret;
	strcpy((char *)buff,"AT+CIPDOMAIN=\"");
	sprintf((char *)buff+strlen((char *)buff),"%s\"\r\n",ptextstring);

	func_ret = esp8266_serial_send_with_recvtask(ESP8266_UART_COMMAND_PORT, buff, 3, 20000, ESP8266_RETURN_OK, ESP8266_SET_CIPDOMAIN, 0xff);
//	func_ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, buff, 3, 20000, ESP8266_RETURN_OK);
	if(func_ret != 0)
	{
		return -1;
	}
	*ulipaddr = (((uint32_t)dnsaddress[0]) << 24) | (((uint32_t)dnsaddress[1]) << 16) | (((uint32_t)dnsaddress[2]) << 8) | ((uint32_t)dnsaddress[3]);

	return 0;
}


static int32_t esp8266_serial_send_basic(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code)
{
#if DEBUGLOG == 1
	TickType_t tmptime1,tmptime2;
#endif
	volatile int32_t timeout;
	sci_err_t ercd;
	uint32_t recvcnt = 0;
	int32_t ret;
	memset(recvbuff,0,sizeof(recvbuff));

	timeout_init(serial_ch_id, timeout_ms);

	if(ptextstring != NULL)
	{
		timeout = 0;
		recvcnt = 0;
		g_esp8266_uart_teiflag[serial_ch_id] = 0;
		ercd = R_SCI_Send(esp8266_uart_sci_handle[serial_ch_id], ptextstring, strlen((const char *)ptextstring));
		if(SCI_SUCCESS != ercd)
		{
			return -1;
		}

		while(1)
		{
			if(0 != g_esp8266_uart_teiflag[serial_ch_id])
	//		ercd = R_SCI_Control(esp8266_uart_sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &non_used);
	//		if(non_used == SCI_TX_BUSIZ)
			{
				break;
			}
			if(-1 == check_timeout(serial_ch_id, recvcnt))
			{
				timeout = 1;
				break;
			}
		}
		if(timeout == 1)
		{
#if DEBUGLOG == 1
			R_BSP_CpuInterruptLevelWrite (14);
			printf("timeout.\r\n",tmptime1,ptextstring);
			R_BSP_CpuInterruptLevelWrite (0);
#endif
			return -1;
		}

	#if DEBUGLOG == 1
		tmptime1 = xTaskGetTickCount();
		if(ptextstring[strlen((const char *)ptextstring)-1] != '\r')
		{
			R_BSP_CpuInterruptLevelWrite (14);
			printf("s:%06d:%s\r\n",tmptime1,ptextstring);
			R_BSP_CpuInterruptLevelWrite (0);
		}
		else
		{
			R_BSP_CpuInterruptLevelWrite (14);
			printf("s:%06d:%s",tmptime1,ptextstring);
			printf("\n");
			R_BSP_CpuInterruptLevelWrite (0);
		}
	#endif
	}
	while(1)
	{
		ercd = R_SCI_Receive(esp8266_uart_sci_handle[serial_ch_id], &recvbuff[recvcnt], 1);
		if(SCI_SUCCESS == ercd)
		{
			recvcnt++;
			bytetimeout_init(serial_ch_id, response_type);
			if(recvcnt < 4)
			{
				continue;
			}
			if(recvcnt == sizeof(recvbuff)-2)
			{
				break;
			}
		}
		if(-1 == check_bytetimeout(serial_ch_id, recvcnt))
		{
			break;
		}
		if(-1 == check_timeout(serial_ch_id, recvcnt))
		{
			timeout = 1;
			break;
		}
	}
	if(timeout == 1)
	{
		return -1;
	}

	/* Response data check */
	ret = -1;
	recvbuff[recvcnt] = '\0';
	if(recvcnt >= strlen((const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode]))
	{
		if(0 == strncmp((const char *)&recvbuff[recvcnt - strlen((const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode]) ],
				(const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode],
				strlen((const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode])))
		{
			ret = 0;
		}
	}
#if 0
	if(recvbuff[recvcnt-1] != '\n')
	{
		recvbuff[recvcnt] = '\r';
		recvbuff[recvcnt+1] = '\n';
		recvbuff[recvcnt+2] = '\0';
		debug_out_crlf = 1;
	}
	else
	{
		recvbuff[recvcnt] = '\0';
		debug_out_crlf = 0;
	}
#endif
#if DEBUGLOG == 1
	tmptime2 = xTaskGetTickCount();
	printf("r:%06d:%s",tmptime2,recvbuff);
#endif
	return ret;
}


static int32_t esp8266_serial_send_with_recvtask(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code,  uint8_t command, uint8_t socket_no)
{
#if DEBUGLOG == 1
	TickType_t tmptime1,tmptime2;
#endif
	volatile int32_t timeout;
	sci_err_t ercd;
	uint32_t recvcnt = 0;
	int32_t ret;
	uint8_t result;

	timeout_init(serial_ch_id, timeout_ms);

	if(ptextstring != NULL)
	{
		timeout = 0;
		recvcnt = 0;

		esp8266_response_set_queue( command, socket_no );
		g_esp8266_uart_teiflag[serial_ch_id] = 0;
		ercd = R_SCI_Send(esp8266_uart_sci_handle[serial_ch_id], ptextstring, strlen((const char *)ptextstring));
		if(SCI_SUCCESS != ercd)
		{
			return -1;
		}

		while(1)
		{
			if(0 != g_esp8266_uart_teiflag[serial_ch_id])
	//		ercd = R_SCI_Control(esp8266_uart_sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &non_used);
	//		if(non_used == SCI_TX_BUSIZ)
			{
				break;
			}
			if(-1 == check_timeout(serial_ch_id, recvcnt))
			{
				timeout = 1;
				break;
			}
		}
		if(timeout == 1)
		{
#if DEBUGLOG == 1
			R_BSP_CpuInterruptLevelWrite (14);
			printf("timeout.\r\n",tmptime1,ptextstring);
			R_BSP_CpuInterruptLevelWrite (0);
#endif
			return -1;
		}
	}
	while(1)
	{
		ercd = esp8266_response_get_queue( command, socket_no, &result );
		if(0 == ercd )
		{
			break;
		}

		if(-1 == check_timeout(serial_ch_id, recvcnt))
		{
			timeout = 1;
			break;
		}
	}
	if(timeout == 1)
	{
		return -1;
	}

	ret = -1;
	if(result == expect_code)
	{
		ret = 0;
	}
	return ret;
}

static int32_t esp8266_serial_send_rst(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_esp8266_return_code_t expect_code, sx_esp8266_return_code_t end_string)
{
#if DEBUGLOG == 1
	TickType_t tmptime1,tmptime2;
#endif
	volatile int32_t timeout;
	sci_err_t ercd;
	uint32_t recvcnt = 0;

	timeout_init(serial_ch_id, timeout_ms);

	if(ptextstring != NULL)
	{
		timeout = 0;
		recvcnt = 0;
		g_esp8266_uart_teiflag[serial_ch_id] = 0;
		ercd = R_SCI_Send(esp8266_uart_sci_handle[serial_ch_id], ptextstring, strlen((const char *)ptextstring));
		if(SCI_SUCCESS != ercd)
		{
			return -1;
		}

		while(1)
		{
			if(0 != g_esp8266_uart_teiflag[serial_ch_id])
	//		ercd = R_SCI_Control(esp8266_uart_sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &non_used);
	//		if(non_used == SCI_TX_BUSIZ)
			{
				break;
			}
			if(-1 == check_timeout(serial_ch_id, recvcnt))
			{
				timeout = 1;
				break;
			}
		}
		if(timeout == 1)
		{
#if DEBUGLOG == 1
			R_BSP_CpuInterruptLevelWrite (14);
			printf("timeout.\r\n",tmptime1,ptextstring);
			R_BSP_CpuInterruptLevelWrite (0);
#endif
			return -1;
		}

	#if DEBUGLOG == 1
		tmptime1 = xTaskGetTickCount();
		if(ptextstring[strlen((const char *)ptextstring)-1] != '\r')
		{
			R_BSP_CpuInterruptLevelWrite (14);
			printf("s:%06d:%s\r\n",tmptime1,ptextstring);
			R_BSP_CpuInterruptLevelWrite (0);
		}
		else
		{
			R_BSP_CpuInterruptLevelWrite (14);
			printf("s:%06d:%s",tmptime1,ptextstring);
			printf("\n");
			R_BSP_CpuInterruptLevelWrite (0);
		}
	#endif
	}
	while(1)
	{
		ercd = R_SCI_Receive(esp8266_uart_sci_handle[serial_ch_id], &recvbuff[recvcnt], 1);
		if(SCI_SUCCESS == ercd)
		{
			recvcnt++;
			bytetimeout_init(serial_ch_id, response_type);
			if(recvcnt < 4)
			{
				continue;
			}
			if(recvcnt == sizeof(recvbuff)-2)
			{
				break;
			}
		}
		if(-1 == check_bytetimeout(serial_ch_id, recvcnt))
		{
			break;
		}
		if(-1 == check_timeout(serial_ch_id, recvcnt))
		{
			timeout = 1;
			break;
		}
	}
	if(timeout == 1)
	{
		return -1;
	}

#if 0
	if(recvbuff[recvcnt-1] != '\n')
	{
		recvbuff[recvcnt] = '\r';
		recvbuff[recvcnt+1] = '\n';
		recvbuff[recvcnt+2] = '\0';
		debug_out_crlf = 1;
	}
	else
	{
		recvbuff[recvcnt] = '\0';
		debug_out_crlf = 0;
	}
#endif
#if DEBUGLOG == 1
	tmptime2 = xTaskGetTickCount();
	printf("r:%06d:%s",tmptime2,recvbuff);
#endif
	/* Response data check */
	if(recvcnt < strlen((const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode]))
	{
		return -1;
	}

	if(end_string != 0xff)
	{
		if(0 != strncmp((const char *)&recvbuff[recvcnt - strlen((const char *)esp8266_result_code[end_string][g_sx_esp8266_return_mode]) ],
				(const char *)esp8266_result_code[end_string][g_sx_esp8266_return_mode], strlen((const char *)esp8266_result_code[end_string][g_sx_esp8266_return_mode])))
		{
			return -1;
		}
		return 0;
	}
	if(0 != strncmp((const char *)&recvbuff[recvcnt - strlen((const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode]) ],
			(const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode], strlen((const char *)esp8266_result_code[expect_code][g_sx_esp8266_return_mode])))
	{
		return -1;
	}
	return 0;
}


/* return code: from 0 to 5 : socket status
 *              -1 : Error
 * */
static int32_t esp8266_get_socket_status(uint8_t socket_no)
{
	int32_t i;
	int32_t ret;
	char * str_ptr;
	uint8_t str[3][10];

	ret = esp8266_serial_send_basic(ESP8266_UART_COMMAND_PORT, "ATNSTAT\r", 3, 200, ESP8266_RETURN_OK);
	if(ret != 0)
	{
		return -1;
	}

	str_ptr = strchr((const char *)recvbuff, ',');
	if(str_ptr != NULL)
	{
		for(i = 0;i<ESP8266_SOCKET_STATUS_MAX;i++)
		{
			if(0 == strncmp((const char *)recvbuff, (const char *)esp8266_socket_status[i], ((uint32_t)str_ptr - (uint32_t)recvbuff)))
			{
				break;
			}
		}
	}
	if(i < ESP8266_SOCKET_STATUS_MAX)
	{
		return i;
	}
	return -1;
}


static void timeout_init(uint8_t socket_no, uint16_t timeout_ms)
{
	starttime[socket_no] = xTaskGetTickCount();
	endtime[socket_no] = starttime[socket_no] + timeout_ms;
	if((starttime[socket_no] + endtime[socket_no]) < starttime[socket_no])
	{
		/* overflow */
		timeout_overflow_flag[socket_no] = 1;
	}
	else
	{
		timeout_overflow_flag[socket_no] = 0;
	}
}

static int32_t check_timeout(uint8_t socket_no, int32_t rcvcount)
{
	if(0 == rcvcount)
	{
		thistime[socket_no] = xTaskGetTickCount();
		if(timeout_overflow_flag[socket_no] == 0)
		{
			if(thistime[socket_no] >= endtime[socket_no] || thistime[socket_no] < starttime[socket_no])
			{
				return -1;
			}
		}
		else
		{
			if(thistime[socket_no] < starttime[socket_no] && thistime[socket_no] <= endtime[socket_no])
			{
				/* Not timeout  */
				return -1;
			}
		}
	}
	/* Not timeout  */
	return 0;
}

static void bytetimeout_init(uint8_t socket_no, uint16_t timeout_ms)
{
	startbytetime[socket_no] = xTaskGetTickCount();
	endbytetime[socket_no] = startbytetime[socket_no] + timeout_ms;
	if((startbytetime[socket_no] + endbytetime[socket_no]) < startbytetime[socket_no])
	{
		/* overflow */
		byte_timeout_overflow_flag[socket_no] = 1;
	}
	else
	{
		byte_timeout_overflow_flag[socket_no] = 0;
	}
}

static int32_t check_bytetimeout(uint8_t socket_no, int32_t rcvcount)
{
	if(0 != rcvcount)
	{
		thisbytetime[socket_no] = xTaskGetTickCount();
		if(byte_timeout_overflow_flag[socket_no] == 0)
		{
			if(thisbytetime[socket_no] >= endbytetime[socket_no] || thisbytetime[socket_no] < startbytetime[socket_no])
			{
				return -1;
			}
		}
		else
		{
			if(thisbytetime[socket_no] < startbytetime[socket_no] && thisbytetime[socket_no] <= endbytetime[socket_no])
			{
				/* Not timeout  */
				return -1;
			}
		}
	}
	/* Not timeout  */
	return 0;
}

static int32_t esp8266_serial_open(uint32_t baudrate)
{
	sci_err_t   my_sci_err;

	R_SCI_PinSet_esp8266_serial_default();

	g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT].async.baud_rate    = baudrate;
	g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT].async.clk_src      = SCI_CLK_INT;
	g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT].async.data_size    = SCI_DATA_8BIT;
	g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT].async.parity_en    = SCI_PARITY_OFF;
	g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT].async.parity_type  = SCI_EVEN_PARITY;
	g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT].async.stop_bits    = SCI_STOPBITS_1;
	g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT].async.int_priority = 15;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH_esp8266_serial_default, SCI_MODE_ASYNC, &g_esp8266_sci_config[ESP8266_UART_COMMAND_PORT], esp8266_uart_callback_command_port, &esp8266_uart_sci_handle[ESP8266_UART_COMMAND_PORT]);

    if(SCI_SUCCESS != my_sci_err)
    {
    	return -1;
    }

//    PORT2.PCR.BIT.B5 = 1;
    return 0;

}

static int32_t esp8266_serial_close(void)
{
    R_SCI_Close(esp8266_uart_sci_handle[ESP8266_UART_COMMAND_PORT]);
    return 0;
}

int32_t esp8266_socket_init(void)
{
	int i;
	for(i = 0;i<CREATEABLE_SOCKETS; i++)
	{
		if(BYTEQ_SUCCESS != R_BYTEQ_Open(esp8266_socket[i].socket_recv_buff, sizeof(esp8266_socket[i].socket_recv_buff), &esp8266_socket[i].socket_byteq_hdl))
		{
			return -1;
		}
	}

    g_esp8266_semaphore = xSemaphoreCreateMutex();

    if( g_esp8266_semaphore == NULL )
    {
    	return -1;
    }
	/* Success. */
	return 0;

}

int effl;
static void esp8266_uart_callback_command_port(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
    	R_BSP_NOP();
    }
#if SCI_CFG_TEI_INCLUDED
	else if (SCI_EVT_TEI == p_args->event)
	{
		g_esp8266_uart_teiflag[ESP8266_UART_COMMAND_PORT] = 1;
		R_BSP_NOP();

	}
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	R_BSP_NOP();
        effl = 1;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	R_BSP_NOP();
        effl = 2;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	R_BSP_NOP();
        effl = 3;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	R_BSP_NOP();
        effl = 4;
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */


int32_t esp8266_tcp_send_returncode_check(void)
{
	uint32_t recvcnt;
	uint32_t timeout;
	sci_err_t sci_ret;
	char * string_ret;
	memset(recvbuff,0,sizeof(recvbuff));
	timeout = 0;
	recvcnt = 0;

	if(g_sl_esp8266_tcp_recv_timeout != 0)
	{
		timeout_init(1, 2000);
	}
	while(1)
	{
		sci_ret = R_SCI_Receive(esp8266_uart_sci_handle[ESP8266_UART_COMMAND_PORT], &recvbuff[recvcnt], 1);
		if(SCI_SUCCESS == sci_ret)
		{
			string_ret = strstr( (const char *)recvbuff , (const char *)esp8266_return_text_send_ok );
			if(string_ret !=NULL)
			{
#if DEBUGLOG == 1
				printf("tcp send is oK\r\n");
#endif
				return 0;
			}
			string_ret = strstr( (const char *)recvbuff , (const char *)esp8266_return_text_error );
			if(string_ret !=NULL)
			{
#if DEBUGLOG == 1
				printf("tcp send is error\r\n");
#endif
				return -1;
			}
			string_ret = strstr( (const char *)recvbuff , (const char *)esp8266_return_text_send_fail );
			if(string_ret !=NULL)
			{
#if DEBUGLOG == 1
				printf("tcp send is failed\r\n");
#endif
				return -1;
			}
			recvcnt++;
			bytetimeout_init(ESP8266_UART_COMMAND_PORT, 1000);
			if(recvcnt < 4)
			{
				continue;
			}
			if(recvcnt == sizeof(recvbuff)-2)
			{
				break;
			}
		}
		if(-1 == check_bytetimeout(ESP8266_UART_COMMAND_PORT, recvcnt))
		{
			timeout = 1;
			break;
		}
		if(-1 == check_timeout(ESP8266_UART_COMMAND_PORT, recvcnt))
		{
			timeout = 1;
			break;
		}
	}
	if(timeout == 1)
	{
		nop();
		return -1;
	}
	return 0;
}
