#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "platform.h"
#include "r_sci_rx_if.h"
#include "r_sci_rx_config.h"
#include "r_sci_rx_pinset.h"
#include "r_byteq_if.h"
#include "sx_ulpgn_driver.h"

#if !defined(MY_BSP_CFG_UART_WIFI_SCI)
#error "Error! Need to define MY_BSP_CFG_UART_WIFI_SCI in r_bsp_config.h"
#elif MY_BSP_CFG_UART_WIFI_SCI == (0)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI0()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH0
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH0_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH0_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (1)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI1()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH1
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH1_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH1_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (2)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI2()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH2
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH2_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH2_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (3)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI3()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH3
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH3_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH3_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (4)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI4()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH4
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH4_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH4_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (5)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI5()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH5
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH5_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH5_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (6)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI6()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH6
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH6_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH6_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (7)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI7()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH7
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH7_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH7_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (8)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI8()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH8
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH8_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH8_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (9)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI9()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH9
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH9_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH9_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (10)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI10()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH10
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH10_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH10_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (11)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI11()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH11
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH11_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH11_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SCI == (12)
#define R_SCI_PinSet_sx_ulpgn_serial_default()  R_SCI_PinSet_SCI12()
#define SCI_CH_sx_ulpgn_serial_default          SCI_CH12
#define SCI_TX_BUSIZ_DEFAULT                    SCI_CFG_CH12_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                    SCI_CFG_CH12_RX_BUFSIZ
#else
#error "Error! Invalid setting for MY_BSP_CFG_UART_WIFI_SCI in r_bsp_config.h"
#endif

#if !defined(MY_BSP_CFG_UART_WIFI_SECOND_SCI)
#error "Error! Need to define MY_BSP_CFG_UART_WIFI_SECOND_SCI in r_bsp_config.h"
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (0)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI0()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH0
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH0_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH0_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (1)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI1()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH1
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH1_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH1_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (2)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI2()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH2
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH2_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH2_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (3)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI3()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH3
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH3_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH3_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (4)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI4()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH4
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH4_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH4_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (5)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI5()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH5
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH5_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH5_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (6)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI6()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH6
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH6_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH6_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (7)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI7()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH7
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH7_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH7_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (8)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI8()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH8
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH8_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH8_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (9)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI9()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH9
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH9_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH9_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (10)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI10()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH10
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH10_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH10_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (11)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI11()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH11
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH11_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH11_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (12)
#define R_SCI_PinSet_sx_ulpgn_serial_second()  R_SCI_PinSet_SCI12()
#define SCI_CH_sx_ulpgn_serial_second          SCI_CH12
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH12_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH12_RX_BUFSIZ
#elif MY_BSP_CFG_UART_WIFI_SECOND_SCI == (-1)
#define R_SCI_PinSet_sx_ulpgn_serial_second()
#define SCI_CH_sx_ulpgn_serial_second
#define SCI_TX_BUSIZ_SECOND
#define SCI_RX_BUSIZ_SECOND
#else
#error "Error! Invalid setting for MY_BSP_CFG_UART_WIFI_SECOND_SCI in r_bsp_config.h"
#endif

#define ULPGN_UART_DEFAULT_PORT (0)
#define ULPGN_UART_SECOND_PORT (1)

#define MUTEX_TX (1 << 0)
#define MUTEX_RX (1 << 1)

uint8_t ULPGN_USE_UART_NUM = 2;

uint32_t g_sx_ulpgn_tx_busiz_command;
uint32_t g_sx_ulpgn_tx_busiz_data;
uint32_t g_sx_ulpgn_rx_busiz_command;
uint32_t g_sx_ulpgn_rx_busiz_data;

uint8_t g_sx_ulpgn_cleateble_sockets;
uint8_t ULPGN_UART_COMMAND_PORT;
uint8_t ULPGN_UART_DATA_PORT;

const uint8_t ulpgn_return_text_ok[]          = ULPGN_RETURN_TEXT_OK;
const uint8_t ulpgn_return_text_connect[]     = ULPGN_RETURN_TEXT_CONNECT;
const uint8_t ulpgn_return_text_ring[]        = ULPGN_RETURN_TEXT_RING;
const uint8_t ulpgn_return_text_no_carrier[]  = ULPGN_RETURN_TEXT_NO_CARRIER;
const uint8_t ulpgn_return_text_error[]       = ULPGN_RETURN_TEXT_ERROR;
const uint8_t ulpgn_return_text_no_dialtone[] = ULPGN_RETURN_TEXT_NO_DIALTONE;
const uint8_t ulpgn_return_text_busy[]        = ULPGN_RETURN_TEXT_BUSY;
const uint8_t ulpgn_return_text_no_answer[]   = ULPGN_RETURN_TEXT_NO_ANSWER;

const uint8_t ulpgn_return_numeric_ok[]          = ULPGN_RETURN_NUMERIC_OK;
const uint8_t ulpgn_return_numeric_connect[]     = ULPGN_RETURN_NUMERIC_CONNECT;
const uint8_t ulpgn_return_numeric_ring[]        = ULPGN_RETURN_NUMERIC_RING;
const uint8_t ulpgn_return_numeric_no_carrier[]  = ULPGN_RETURN_NUMERIC_NO_CARRIER;
const uint8_t ulpgn_return_numeric_error[]       = ULPGN_RETURN_NUMERIC_ERROR;
const uint8_t ulpgn_return_numeric_no_dialtone[] = ULPGN_RETURN_NUMERIC_NO_DIALTONE;
const uint8_t ulpgn_return_numeric_busy[]        = ULPGN_RETURN_NUMERIC_BUSY;
const uint8_t ulpgn_return_numeric_no_answer[]   = ULPGN_RETURN_NUMERIC_NO_ANSWER;

const uint8_t ulpgn_socket_status_closed[]       = ULPGN_SOCKET_STATUS_TEXT_CLOSED;
const uint8_t ulpgn_socket_status_socket[]       = ULPGN_SOCKET_STATUS_TEXT_SOCKET;
const uint8_t ulpgn_socket_status_bound[]        = ULPGN_SOCKET_STATUS_TEXT_BOUND;
const uint8_t ulpgn_socket_status_listen[]       = ULPGN_SOCKET_STATUS_TEXT_LISTEN;
const uint8_t ulpgn_socket_status_connected[]    = ULPGN_SOCKET_STATUS_TEXT_CONNECTED;

const uint8_t ulpgn_return_dummy[]   = "";

const uint32_t g_sx_ulpgn_serial_buffsize_table[2][4] =
{
    /*SCI_TX_BUSIZ_DEFAULT*//*SCI_TX_BUSIZ_SIZE*/   /*SCI_RX_BUSIZ_DEFAULT*/ /*SCI_RX_BUSIZ_SIZE*/
    {SCI_TX_BUSIZ_DEFAULT,  SCI_TX_BUSIZ_DEFAULT,  SCI_RX_BUSIZ_DEFAULT,  SCI_RX_BUSIZ_DEFAULT,  }, /*ULPGN_USE_UART_NUM = 1 */
    {SCI_TX_BUSIZ_SECOND,   SCI_TX_BUSIZ_DEFAULT,  SCI_TX_BUSIZ_SECOND,   SCI_RX_BUSIZ_DEFAULT,  }, /*ULPGN_USE_UART_NUM = 2 */
};

const uint8_t * const ulpgn_result_code[ULPGN_RETURN_ENUM_MAX][ULPGN_RETURN_STRING_MAX] =
{
    /* text mode*/                  /* numeric mode */
    {ulpgn_return_text_ok,          ulpgn_return_numeric_ok          },
    {ulpgn_return_text_connect,     ulpgn_return_numeric_connect     },
    {ulpgn_return_text_ring,        ulpgn_return_numeric_ring        },
    {ulpgn_return_text_no_carrier,  ulpgn_return_numeric_no_carrier  },
    {ulpgn_return_text_error,       ulpgn_return_numeric_error       },
    {ulpgn_return_dummy,            ulpgn_return_dummy               },
    {ulpgn_return_text_no_dialtone, ulpgn_return_numeric_no_dialtone },
    {ulpgn_return_text_busy,        ulpgn_return_numeric_busy        },
    {ulpgn_return_text_no_answer,   ulpgn_return_numeric_no_answer   },
};

const uint8_t * const ulpgn_socket_status[ULPGN_SOCKET_STATUS_MAX] =
{
    ulpgn_socket_status_closed,
    ulpgn_socket_status_socket,
    ulpgn_socket_status_bound,
    ulpgn_socket_status_listen,
    ulpgn_socket_status_connected,
};

volatile uint8_t current_socket_index;
uint8_t buff[1000];
uint8_t recvbuff[1500];
sci_cfg_t   g_sx_ulpgn_sci_config[2];
volatile uint32_t g_sx_ulpgn_uart_teiflag[2];
static uint8_t timeout_overflow_flag[2];
static sci_hdl_t sx_ulpgn_uart_sci_handle[2];
static TickType_t starttime[2], thistime[2], endtime[2];
static TickType_t startbytetime[2], thisbytetime[2], endbytetime[2];
static uint8_t byte_timeout_overflow_flag[2];

uint8_t g_sx_ulpgn_return_mode;

static void sx_ulpgn_uart_callback_second_port_for_command(void *pArgs);
static void sx_ulpgn_uart_callback_default_port_for_inititial(void *pArgs);
static void sx_ulpgn_uart_callback_default_port_for_data(void *pArgs);
static void timeout_init(uint8_t serial_ch, uint32_t timeout_ms);
static void bytetimeout_init(uint8_t serial_ch, uint32_t timeout_ms);
static int32_t check_timeout(uint8_t serial_ch, int32_t rcvcount);
static int32_t check_bytetimeout(uint8_t serial_ch, int32_t rcvcount);
static void tcp_timeout_init(uint8_t socket_no, uint32_t timeout_ms, uint8_t flag);
static int32_t tcp_check_timeout(uint8_t socket_no, uint8_t flag);

static int32_t sx_ulpgn_serial_open_for_initial(void);
static int32_t sx_ulpgn_serial_open_for_data(void);
static int32_t sx_ulpgn_serial_close(void);
static int32_t sx_ulpgn_serial_escape(void);
static int32_t sx_ulpgn_serial_second_port_open(void);
static int32_t sx_ulpgn_serial_second_port_close(void);
static int32_t sx_ulpgn_serial_send_basic(uint8_t serial_ch_id, const char *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_ulpgn_return_code_t expect_code);
static int32_t sx_ulpgn_check_uart_state(uint16_t response_type, uint16_t timeout_ms,uint32_t *uart_receive_status, uint32_t *uart_send_status);

static int32_t sx_ulpgn_serial_send_basic_take_mutex(uint8_t mutex_flag);
static void sx_ulpgn_serial_send_basic_give_mutex(uint8_t mutex_flag);
static int32_t sx_ulpgn_get_socket_status(uint8_t socket_no);
static int32_t sx_ulpgn_change_socket_index(uint8_t socket_no);
static TickType_t g_sl_ulpgn_tcp_recv_timeout = 0;       /* ## slowly problem ## unit: 1ms */

/**
 * @brief The global mutex to ensure that only one operation is accessing the
 * g_sx_ulpgn_tx_semaphore flag at one time.
 */
static SemaphoreHandle_t g_sx_ulpgn_tx_semaphore = NULL;
static SemaphoreHandle_t g_sx_ulpgn_rx_semaphore = NULL;

/**
 * @brief Maximum time in ticks to wait for obtaining a semaphore.
 */
static const TickType_t xMaxSemaphoreBlockTime = pdMS_TO_TICKS( 10000UL );

typedef struct
{
    byteq_hdl_t socket_byteq_hdl;
    uint8_t socket_recv_buff[16384];
    uint8_t socket_status;
    uint8_t socket_recv_error_count;
    uint8_t socket_create_flag;
    uint8_t socket_not_create_recv_count;
    TickType_t send_starttime;
    TickType_t send_thistime;
    TickType_t send_endtime;
    TickType_t recv_starttime;
    TickType_t recv_thistime;
    TickType_t recv_endtime;
    uint8_t send_timeout_overflow_flag;
    uint8_t recv_timeout_overflow_flag;
} ulpgn_socket_t;

ulpgn_socket_t g_ulpgn_socket[CREATEABLE_SOCKETS];

static uint8_t g_temporary_byteq_enable_flag = 0;
static uint8_t g_sockindex_command_flag = 0;
typedef struct
{
    uint32_t queue_overflow_count;
    uint32_t eri_overflow_count;
    uint32_t eri_framing_count;
    uint32_t eri_parity_count;
} uart_error_count_t;

uart_error_count_t uart_error_count[2];

uint8_t g_sx_ulpgn_ipaddress[4];
uint8_t g_sx_ulpgn_subnetmask[4];
uint8_t g_sx_ulpgn_gateway[4];

uint32_t test_recvdat = 0xa5;
uint32_t test_senddat = 0x5a;

int32_t sx_ulpgn_wifi_init(void)
{
    int32_t ret;
    sci_baud_t change_boud;
#if ULPGN_PORT_DEBUG == 1
    DEBUG_PORT4_DDR = 1;
    DEBUG_PORT4_DR = 0;
    DEBUG_PORT7_DDR = 1;
    DEBUG_PORT7_DR = 0;
#endif

    ULPGN_USE_UART_NUM = 2;
    g_sx_ulpgn_cleateble_sockets = CREATEABLE_SOCKETS;
    ULPGN_UART_COMMAND_PORT = ULPGN_UART_DEFAULT_PORT;
//  ULPGN_UART_DATA_PORT = 1;
    g_sx_ulpgn_tx_busiz_command = g_sx_ulpgn_serial_buffsize_table[1][0];
    g_sx_ulpgn_tx_busiz_data    = g_sx_ulpgn_serial_buffsize_table[1][1];
    g_sx_ulpgn_rx_busiz_command = g_sx_ulpgn_serial_buffsize_table[1][2];
    g_sx_ulpgn_rx_busiz_data    = g_sx_ulpgn_serial_buffsize_table[1][3];

    /* Wifi Module hardware reset   */
    ULPGN_RESET_PORT_PDR = 1;
    ULPGN_RESET_PORT_PODR = 0; /* Low */
    R_BSP_SoftwareDelay(26, BSP_DELAY_MILLISECS); /* 5us mergin 1us */
    ULPGN_RESET_PORT_PODR = 1; /* High */
    R_BSP_SoftwareDelay(200, BSP_DELAY_MILLISECS); /*  */

    for (uint8_t i = 0; i < CREATEABLE_SOCKETS; i++)
    {
        g_ulpgn_socket[i].socket_create_flag = 0;
    }

    if (g_sx_ulpgn_tx_semaphore != NULL)
    {
        vSemaphoreDelete(g_sx_ulpgn_tx_semaphore);
    }

    g_sx_ulpgn_tx_semaphore = xSemaphoreCreateMutex();

    if( g_sx_ulpgn_tx_semaphore == NULL )
    {
        return -1;
    }

    if (g_sx_ulpgn_rx_semaphore != NULL)
    {
        vSemaphoreDelete(g_sx_ulpgn_rx_semaphore);
    }

    g_sx_ulpgn_rx_semaphore = xSemaphoreCreateMutex();

    if( g_sx_ulpgn_rx_semaphore == NULL )
    {
        return -1;
    }

    ret = sx_ulpgn_serial_open_for_initial();
    if(ret != 0)
    {
        return ret;
    }



    g_sx_ulpgn_return_mode = 0;

    /* no echo */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATE0\r", 5, 400, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }
    g_sx_ulpgn_return_mode = 1;
    /* result code format = numeric */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATV0\r", 3, 200, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    /* Check HSUART2 connection */
    /* Command Port = HSUART2 */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATUART=2\r", 3, 2000, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 1 sec */

    ULPGN_UART_COMMAND_PORT = ULPGN_UART_SECOND_PORT;

    ret = sx_ulpgn_serial_second_port_open();
    if(ret != 0)
    {
        return ret;
    }

    sx_ulpgn_serial_close();

    /* Command Port = HSUART2, Data Port = HSUART1 */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATV0\r", 100, 2000, ULPGN_RETURN_OK);
    if(ret == 0)
    {
        /* HSUART2 and SCI are connecting.
         *  -> multiple UART control. */
#if DEBUGLOG == 1
        printf("=-=-= Multiple connection!=-=-=\r\n");
#endif
        ret = sx_ulpgn_serial_open_for_data();
        if(ret != 0)
        {
            return ret;
        }
        /* Change HSUART1 baudrate and flow control. */
        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATBX1=230400,,,,h\r", 3, 200, ULPGN_RETURN_OK);
        if(ret != 0)
        {
            return ret;
        }
        R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], SCI_CMD_EN_CTS_IN, NULL);
        ULPGN_HSUART1_RTS_PODR = 0;
        ULPGN_HSUART1_RTS_PDR = 1;
        R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 5us mergin 1us */

        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATUART=2,1\r", 3, 2000, ULPGN_RETURN_OK);
        if(ret != 0)
        {
            return ret;
        }
        R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 1 sec */

        ULPGN_USE_UART_NUM = 2;
        g_sx_ulpgn_cleateble_sockets = 4;
        ULPGN_UART_COMMAND_PORT = ULPGN_UART_SECOND_PORT;
        ULPGN_UART_DATA_PORT = ULPGN_UART_DEFAULT_PORT;
        g_sx_ulpgn_tx_busiz_command = g_sx_ulpgn_serial_buffsize_table[1][0];
        g_sx_ulpgn_tx_busiz_data    = g_sx_ulpgn_serial_buffsize_table[1][1];
        g_sx_ulpgn_rx_busiz_command = g_sx_ulpgn_serial_buffsize_table[1][2];
        g_sx_ulpgn_rx_busiz_data    = g_sx_ulpgn_serial_buffsize_table[1][3];

    }
    else
    {
        /* HSUART2 and SCI are not connecting.
         * -> single UART */

#if DEBUGLOG == 1
        printf("=-=-= Single connection!=-=-=\r\n");
#endif
        ret = sx_ulpgn_serial_second_port_close();

        ULPGN_USE_UART_NUM = 1;
        g_sx_ulpgn_cleateble_sockets = 1;
        ULPGN_UART_COMMAND_PORT = ULPGN_UART_DEFAULT_PORT;
        ULPGN_UART_DATA_PORT = ULPGN_UART_DEFAULT_PORT;
        g_sx_ulpgn_tx_busiz_command = g_sx_ulpgn_serial_buffsize_table[0][0];
        g_sx_ulpgn_tx_busiz_data    = g_sx_ulpgn_serial_buffsize_table[0][1];
        g_sx_ulpgn_rx_busiz_command = g_sx_ulpgn_serial_buffsize_table[0][2];
        g_sx_ulpgn_rx_busiz_data    = g_sx_ulpgn_serial_buffsize_table[0][3];

        /* Wifi Module hardware reset   */
        ULPGN_RESET_PORT_PDR = 1;
        ULPGN_RESET_PORT_PODR = 0; /* Low */
        R_BSP_SoftwareDelay(26, BSP_DELAY_MILLISECS); /* 5us mergin 1us */
        ULPGN_RESET_PORT_PODR = 1; /* High */
        R_BSP_SoftwareDelay(200, BSP_DELAY_MILLISECS); /*  */

        g_sx_ulpgn_return_mode = 0;

        ret = sx_ulpgn_serial_open_for_initial();
        if(ret != 0)
        {
            return ret;
        }


        /* no echo */
        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATE0\r", 3, 200, ULPGN_RETURN_OK);
        if(ret != 0)
        {
            return ret;
        }
        g_sx_ulpgn_return_mode = 1;
        /* result code format = numeric */
        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATV0\r", 3, 200, ULPGN_RETURN_OK);
        if(ret != 0)
        {
            return ret;
        }

        /* Change HSUART1 baudrate and flow control. */
        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATBX1=115200,,,,h\r", 3, 200, ULPGN_RETURN_OK);
        if(ret != 0)
        {
            return ret;
        }
        change_boud.pclk = BSP_PCLKB_HZ;
        change_boud.rate = 115200;
        R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], SCI_CMD_CHANGE_BAUD, &change_boud);
        R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], SCI_CMD_EN_CTS_IN, NULL);
        ULPGN_HSUART1_RTS_PODR = 0;
        ULPGN_HSUART1_RTS_PDR = 1;
        R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 5us mergin 1us */

    }

    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATS108?\r", 3, 200, ULPGN_RETURN_OK);

    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATS108=1\r", 3, 200, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

//  1875000 921600
    /* Escape guard time = 200msec */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATS12=1\r", 1000 , 200, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    /* Buffer size = 1420byte */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATBSIZE=1420\r", 1000 , 200, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    /* Disconnect from currently connected Access Point, */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATWD\r", 3, 200, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }
    current_socket_index = 0;

    /* Enable DHCP */
    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATNDHCP=1\r", 3, 200, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATTO=1\r", 3, 1000, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return ret;
    }

    ret = sx_ulpgn_check_uart_state(100, 400, &test_recvdat, &test_senddat);
    if(ret != 0)
    {
        R_BSP_NOP();
    }
    R_BSP_NOP();

    ret = sx_ulpgn_check_uart_state(100, 400, &test_recvdat, &test_senddat);
    if(ret != 0)
    {
    	R_BSP_NOP();
    }
    R_BSP_NOP();


    if(0)
    {
        /* Command Port = HSUART1(PMOD-UART), Data Port = Debug　UART */
        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATUART=1,2\r", 3, 2000, ULPGN_RETURN_OK);
        if(ret != 0)
        {
            return ret;
        }

        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_DATA_PORT, NULL, 1000, 200, ULPGN_RETURN_OK);
    }
//    ret = sx_ulpgn_socket_init();

    return ret;
}


int32_t sx_ulpgn_wifi_connect(const char *pssid, uint32_t security, const char *ppass)
{
    int32_t ret;
    char *pstr, pstr2;
    int32_t line_len;
    int32_t scanf_ret;
    volatile char secu[3][10];
    uint32_t security_encrypt_type = 1;
    uint8_t mutex_flag;
    uint8_t retry_count;

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

    if(0 ==  is_sx_ulpgn_wifi_connect())
    {
    	/* If Wifi is already connected, do nothing and return fail. */
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    	if(0 != sx_ulpgn_wifi_disconnect())
    	{
    		return -1;
    	}
        if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
    	{
        	return -1;
    	}
    }


    R_BSP_SoftwareDelay(2000, BSP_DELAY_MILLISECS);
//    vTaskDelay(2000); // timing issue

    strcpy((char *)buff, "ATWAWPA=");
    strcat((char *)buff, (const char *)pssid);
    strcat((char *)buff, ",");
    if(security != ULPGN_SECURITY_WPA && security != ULPGN_SECURITY_WPA2)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }
    if(security == ULPGN_SECURITY_WPA)
    {
        strcat((char *)buff, "1,");
    }
    else
    {
        strcat((char *)buff, "2,");
    }
    if(security_encrypt_type == 1)
    {
        strcat((char *)buff, "1,1,");
    }
    else
    {
        strcat((char *)buff, "0,0,");
    }
    strcat((char *)buff, (const char *)ppass);
    strcat((char *)buff, "\r");

    for(retry_count = 0;retry_count < 3;retry_count++)
    {
		ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, buff, 3, 20000, ULPGN_RETURN_OK);
		if((0 == ret) || (-2 == ret))
		{
			if(-2 == ret)
			{
				ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATW\r", 3, 1000, ULPGN_RETURN_OK);
				if(0 == ret)
				{
					ret = -1;
					pstr = strstr(recvbuff,"ssid         =   ");
					if(pstr != NULL)
					{
						pstr2 = recvbuff + strlen("ssid         =   ");
						pstr = strstr(pstr2,"\r\n");
						if(pstr != NULL)
						{
							*pstr = '\0';  // \r -> \0
							if(0 == strcmp(pstr, pssid))
							{
								ret = 0;
							}
						}
					}
				}
			}
		}
		if(0 == ret)
		{
			while(1)
			{
				ret = sx_ulpgn_get_ipaddress();
				if(0 == ret)
				{
					ret = sx_ulpgn_socket_init();
					break;
				}
			}
			break;
		}
		else
		{
			sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATWD\r", 3, 1000, ULPGN_RETURN_OK);
		}
    }
    sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    return ret;
}

int32_t sx_ulpgn_wifi_disconnect(void)
{
	uint8_t mutex_flag;

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

	if(0 != is_sx_ulpgn_wifi_connect())
    {
    	/* If Wifi is not connected, do nothing and return success. */
	    sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    	return 0;
    }


    sx_ulpgn_serial_escape();

    for (uint8_t i = 0; i < CREATEABLE_SOCKETS; i++)
    {
        if (0 != R_BYTEQ_Close(g_ulpgn_socket[i].socket_byteq_hdl))
        {
//            return -1;
        }
    }

    sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATWD\r", 3, 1000, ULPGN_RETURN_OK);
    memset(g_sx_ulpgn_ipaddress, 0, sizeof(g_sx_ulpgn_ipaddress));
    memset(g_sx_ulpgn_subnetmask, 0, sizeof(g_sx_ulpgn_subnetmask));
    memset(g_sx_ulpgn_gateway, 0, sizeof(g_sx_ulpgn_gateway));

    sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    return 0;
}

int32_t sx_ulpgn_wifi_off(void)
{

    sx_ulpgn_wifi_disconnect();

    R_SCI_Close(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT]);
    if(ULPGN_USE_UART_NUM == 2)
    {
        R_SCI_Close(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT]);
    }
    return 0;
}

int32_t is_sx_ulpgn_wifi_connect(void)
{
	int32_t ret = -1;

	if((g_sx_ulpgn_ipaddress[0] | g_sx_ulpgn_ipaddress[1] | g_sx_ulpgn_ipaddress[2] | g_sx_ulpgn_ipaddress[3])!= 0)
	{
		ret = 0;
	}
	return ret;
}

static int32_t sx_ulpgn_serial_escape()
{
//#if ULPGN_USE_UART_NUM == 1 // escape sequence is only needed for single socket
    if(ULPGN_USE_UART_NUM == 1)
    {
        R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
//        vTaskDelay(500 / portTICK_PERIOD_MS);
        if (R_SCI_Send(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], "+++", 3) == SCI_SUCCESS)
        {
            //      transparent_mode = false;
            return 0;
        }
        else
        {
            return 1;
        }
    }
    return -1;
//#endif
}


int32_t sx_ulpgn_wifi_get_macaddr(uint8_t *pmacaddr)
{
    int32_t ret;
    uint8_t macaddr[6];
	uint8_t mutex_flag;

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

    if(pmacaddr == NULL){
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }

    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATW\r", 300, 3000, ULPGN_RETURN_OK);
    if (ret != 0)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }

    ret = sscanf((const char *)recvbuff, "%*[^\n]\n%*[^\n]\n%*[^\n]\nMac Addr     =   %2x:%2x:%2x:%2x:%2x:%2x\r", &macaddr[0], &macaddr[1], &macaddr[2], &macaddr[3], &macaddr[4], &macaddr[5]);

    if (ret == 6)
    {
        memcpy(pmacaddr, &macaddr[0], sizeof(macaddr));
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return 0;
    }
    else
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }
}

int32_t sx_ulpgn_wifi_scan(WIFIScanResult_t *results, uint8_t maxNetworks)
{
    int32_t ret;
    uint8_t idx = 0;
    uint8_t *bssid;
    char *ptr = recvbuff + 2;
	uint8_t mutex_flag;
    uint8_t retry_count;

    if ((NULL == results) || (0 == maxNetworks))
    {
    	return -1;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

    // TODO investigate why this never returns the full response
	for (retry_count = 0; retry_count < 3; retry_count++)
	{
        ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATWS\r", 500, 8000, ULPGN_RETURN_OK);
        if (ret == 0)
        {
        	if(ptr != '\0')
        	{
        		break;
        	}
        }
    }

	if(ret != 0)
	{
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
	}
	do
    {
		results[idx].cSSID[0] = '\0';
        if (strncmp(ptr, "ssid =", 6) != 0)
        {
            break; // end of list
        }
        // SSID
        ret = sscanf(ptr, "ssid = %[^\r]", results[idx].cSSID);
        if (ret != 1)
		{
        	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
			return 0;
		}
        while(*(ptr++) != '\n');

        // BSSID
        bssid = &results[idx].ucBSSID[0];
        ret = sscanf(ptr, "bssid = %x:%x:%x:%x:%x:%x\r", &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5], &bssid[6]);
        if (ret != 6)
        {
        	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
			return 0;
        }
        while(*(ptr++) != '\n');

        ret = sscanf(ptr, "channel = %d\r", &results[idx].cChannel);
        if (ret != 1)
		{
			sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
			return 0;
		}
        while(*(ptr++) != '\n');

        ret = sscanf(ptr, "indicator = %d\r", &results[idx].cRSSI);
        if (ret != 1)
		{
			sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
			return 0;
		}
        while(*(ptr++) != '\n');

        if (strncmp(ptr, "security = NONE!", 16) == 0)
        {
            results[idx].xSecurity = eWiFiSecurityOpen;
            while(*(ptr++) != '\n');
        }
        else
        {
            while(*(ptr++) != '\n');
            if(*(ptr++) !='\r')
			{
				if (strncmp(ptr, "WPA", 3) == 0)
				{
					results[idx].xSecurity = eWiFiSecurityWPA;
				}
				else  if (strncmp(ptr, "RSN/WPA2", 8) == 0)
				{
					results[idx].xSecurity = eWiFiSecurityWPA2;
				}
				// Note WEP is not recognized by the modem
				while(*(ptr++) != '\n');
			}
        }

        while(*ptr != '\0' && *(ptr++) != '\n'); // end of record
    }
    while(++idx < maxNetworks);

	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    return 0;
}

int32_t sx_ulpgn_wifi_ping(uint8_t *pucIPAddr, uint16_t usCount, uint32_t ulIntervalMS)
{
    int32_t ret;
    uint16_t i;
	uint8_t mutex_flag;

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

	sprintf((char *)buff, "ATNPING=%d.%d.%d.%d,,%d\r", \
			(uint8_t)(pucIPAddr[0]),(uint8_t)(pucIPAddr[1]),(uint8_t)(pucIPAddr[2]),(uint8_t)(pucIPAddr[3]),\
			ulIntervalMS);

	for(i = 0; i<usCount; i++)
	{
		// TODO investigate why this never returns the full response
		ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, buff, 3, 8000, ULPGN_RETURN_OK);
		if (ret < 0)
		{
			sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
			return -1;
		}
	}

	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    return 0;
}

int32_t sx_ulpgn_get_ip(uint8_t *ucip_addr)
{
    int32_t ret;
    uint32_t temp_addr[4];
    uint32_t i = 0;
    char *buff = recvbuff;
	uint8_t mutex_flag;

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

    if(ucip_addr == NULL)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }

    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATNSET?\r", 500, 5000, ULPGN_RETURN_OK);
    if (ret != 0)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }
    if (buff[0] == '\n' && buff[1] == '\0')
    {
        buff += 2;
    }

    ret = sscanf((const char *)buff, "IP:%u.%u.%u.%u,", &temp_addr[0], &temp_addr[1],&temp_addr[2],&temp_addr[3]);


    if (ret != 4)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }

    for(i =0;i<4;i++){
           if(temp_addr[i]<=255){
               ucip_addr[i] = (uint8_t)temp_addr[i];
           }else{
           	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
               return -1;
           }
        }


	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    return 0;

}

uint8_t sx_ulpgn_get_avail_socket(void)
{
    for (int8_t i = 0; i < g_sx_ulpgn_cleateble_sockets; i++)
    {
        if (g_ulpgn_socket[i].socket_create_flag == 0)
        {
            return i;
        }
    }
    return 255;
}

int32_t sx_ulpgn_get_tcp_socket_status(uint8_t socket_no)
{
	if(socket_no >= g_sx_ulpgn_cleateble_sockets)
	{
	    return -1;
	}
	return g_ulpgn_socket[socket_no].socket_status;
}

int32_t sx_ulpgn_socket_create(uint8_t socket_no, uint32_t type, uint32_t ipversion)
{
    int32_t ret;

    if( xSemaphoreTake( g_sx_ulpgn_tx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
        {
	#if DEBUGLOG == 1
			R_BSP_CpuInterruptLevelWrite (13);
			printf("sx_ulpgn_socket_create(%d)\r\n", socket_no);
			R_BSP_CpuInterruptLevelWrite (0);
	#endif
			if( g_ulpgn_socket[socket_no].socket_create_flag == 1)
			{
				( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
				return -1;
			}

	//#if ULPGN_USE_UART_NUM == 2
			if(ULPGN_USE_UART_NUM == 2)
			{
				ret = sx_ulpgn_change_socket_index(socket_no);
				if(ret != 0)
				{
					( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
					( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
					return -1;
				}
			}
	//#endif
			ret = sx_ulpgn_get_socket_status(socket_no);

			sprintf((char *)buff, "ATNSOCK=%d,%d\r", (uint8_t)(type), (uint8_t)(ipversion));

			ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, (char *)buff, 10, 200, ULPGN_RETURN_OK);
			if(ret != 0 && ret != -2)
			{
				( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
				return ret;
			}
			g_ulpgn_socket[socket_no].socket_create_flag = 1;
			if(ret == 0)
			{
				g_ulpgn_socket[socket_no].socket_status = ULPGN_SOCKET_STATUS_SOCKET;
				R_BYTEQ_Flush(g_ulpgn_socket[socket_no].socket_byteq_hdl);
			}
			//  ret = sx_ulpgn_serial_send_basic(socket_no, "ATNSTAT\r", 3, 200, ULPGN_RETURN_OK);

			ret = sx_ulpgn_get_socket_status(socket_no);

			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
			( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
        }
        else
        {
			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );

        }
    }
    else
    {
        return -1;
    }

    return 0;
}


int32_t sx_ulpgn_tcp_connect(uint8_t socket_no, uint32_t ipaddr, uint16_t port)
{
    int32_t ret;
    if( xSemaphoreTake( g_sx_ulpgn_tx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
        {
			if( g_ulpgn_socket[socket_no].socket_create_flag  == 0)
			{
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
				return -1;
			}

	#if DEBUGLOG == 1
			R_BSP_CpuInterruptLevelWrite (13);
			printf("sx_ulpgn_tcp_connect(%d)\r\n", socket_no);
			R_BSP_CpuInterruptLevelWrite (0);
	#endif
	//#if ULPGN_USE_UART_NUM == 2
			if(ULPGN_USE_UART_NUM == 2)
			{
				ret = sx_ulpgn_change_socket_index(socket_no);
				if(ret != 0)
				{
					/* Give back the socketInUse mutex. */
					( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
					( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
					return -1;
				}
			}
	//#endif
			strcpy((char *)buff, "ATNCTCP=");
			sprintf((char *)buff + strlen((char *)buff), "%d.%d.%d.%d,%d\r", (uint8_t)(ipaddr >> 24), (uint8_t)(ipaddr >> 16), (uint8_t)(ipaddr >> 8), (uint8_t)(ipaddr), port);

	//#if ULPGN_USE_UART_NUM == 2
			if(ULPGN_USE_UART_NUM == 2)
			{
				ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, (char *)buff, 300, 15000, ULPGN_RETURN_OK);
			}
	//#endif
			if(ULPGN_USE_UART_NUM == 1)
			{
	//#if ULPGN_USE_UART_NUM == 1
				ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, (char *)buff, 300, 15000, ULPGN_RETURN_CONNECT);
	//#endif
			}
			if(ret == 0)
			{
				g_ulpgn_socket[socket_no].socket_status = ULPGN_SOCKET_STATUS_CONNECTED;
			}


			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
			( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
        }
        else
        {
			( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
        }
    }
    else
    {
        return -1;
    }
    return ret;

}

int32_t sx_ulpgn_tcp_send(uint8_t socket_no, const uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
    int32_t timeout;
    volatile int32_t sended_length;
    int32_t current_send_length;
    int32_t ret;
    sci_err_t ercd;
    TickType_t starttime;
	TickType_t stoptime;


    if( xSemaphoreTake( g_sx_ulpgn_tx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if(0 == g_ulpgn_socket[socket_no].socket_create_flag )
        {
            /* Give back the socketInUse mutex. */
            ( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
            return -1;
        }
        if(ULPGN_SOCKET_STATUS_CONNECTED != g_ulpgn_socket[socket_no].socket_status)
        {
            /* Give back the socketInUse mutex. */
#if DEBUGLOG == 1
            R_BSP_CpuInterruptLevelWrite (13);
            printf("status not connect.\r\n");
            R_BSP_CpuInterruptLevelWrite (0);
#endif
            ( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
            return -1;
        }

        if(ULPGN_USE_UART_NUM == 2)
        {
        	if(socket_no != current_socket_index)
        	{
        	    if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) != pdTRUE )
        	    {
                    ( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
                    return 0;
        	    }
                ret = sx_ulpgn_change_socket_index(socket_no);
                ( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
                if(ret != 0)
                {
                    /* Give back the socketInUse mutex. */
                    ( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
                    return -1;
                }
        	}
        }

#if DEBUGLOG == 1
        R_BSP_CpuInterruptLevelWrite (13);
        printf("sx_ulpgn_tcp_send(%d)\r\n", socket_no);
        R_BSP_CpuInterruptLevelWrite (0);
#endif

        sended_length = 0;
        tcp_timeout_init(socket_no, timeout_ms, 0);

        timeout = 0;

        while(sended_length < length)
        {
            if(length - sended_length > g_sx_ulpgn_tx_busiz_data)
            {
                current_send_length = g_sx_ulpgn_tx_busiz_data;
            }
            else
            {
                current_send_length = length - sended_length;
            }

            if(current_send_length > 1420)
            {
            	current_send_length = 1420;
            }

            g_sx_ulpgn_uart_teiflag[ULPGN_UART_DATA_PORT] = 0;
            ercd = R_SCI_Send(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], (uint8_t *)pdata + sended_length, current_send_length);
            if(SCI_SUCCESS != ercd)
            {
                /* Give back the socketInUse mutex. */
                ( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
                return -1;
            }

            while(1)
            {
                if(0 != g_sx_ulpgn_uart_teiflag[ULPGN_UART_DATA_PORT])
                {
                    break;
                }
                vTaskDelay(1);
            }

            sended_length += current_send_length;

            if(-1 == tcp_check_timeout(socket_no, 0))
            {
              break;
            }

        }

#if DEBUGLOG == 1
        R_BSP_CpuInterruptLevelWrite (13);
        printf("tcp %d byte send\r\n", sended_length);
        R_BSP_CpuInterruptLevelWrite (0);
#endif

        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
    }
    else
    {
        return 0;
    }
    return sended_length;
}

int32_t sx_ulpgn_tcp_recv(uint8_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
    sci_err_t ercd;
    uint32_t recvcnt = 0;
    int32_t ret;
    byteq_err_t byteq_ret;
#if DEBUGLOG == 1
    TickType_t tmptime2;
#endif
    TickType_t tmptime1;

    if(ULPGN_USE_UART_NUM == 2)
    {
        if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
        {
			if(0 == g_ulpgn_socket[socket_no].socket_create_flag)
			{
	            ( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				return -1;
			}

			if(ULPGN_SOCKET_STATUS_CONNECTED != g_ulpgn_socket[socket_no].socket_status)
			{
				/* Give back the socketInUse mutex. */
	#if DEBUGLOG == 1
				R_BSP_CpuInterruptLevelWrite (13);
				printf("status not connect.\r\n");
				R_BSP_CpuInterruptLevelWrite (0);
	#endif
	            ( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				return -1;
			}

	        if(ULPGN_USE_UART_NUM == 2)
	        {
	        	if(socket_no != current_socket_index)
	        	{
	        		( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
	        	    if( xSemaphoreTake( g_sx_ulpgn_tx_semaphore, xMaxSemaphoreBlockTime ) != pdTRUE )
	        	    {
	                    return 0;
	        	    }
	                if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) != pdTRUE )
	                {
	                	( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
	                    return 0;
	                }
	                ret = sx_ulpgn_change_socket_index(socket_no);
	                ( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
	                if(ret != 0)
	                {
	                    /* Give back the socketInUse mutex. */
	                    ( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
	                    return -1;
	                }
	        	}
	        }

			if((timeout_ms != 0) && (timeout_ms != portMAX_DELAY))
			{
				tcp_timeout_init(socket_no, timeout_ms, 1);
			}

			while(1)
			{
				R_BSP_CpuInterruptLevelWrite (15);
				byteq_ret = R_BYTEQ_Get(g_ulpgn_socket[current_socket_index].socket_byteq_hdl, (pdata + recvcnt));
				R_BSP_CpuInterruptLevelWrite (0);
				if(BYTEQ_SUCCESS == byteq_ret)
				{
	//	        	printf("%02x\r\n",*(pdata + recvcnt));
					recvcnt++;
					if(recvcnt >= length)
					{
						break;
					}
					continue;
				}
				if((timeout_ms != 0) && (timeout_ms != portMAX_DELAY))
				{
					if(-1 == tcp_check_timeout(socket_no, 1))
					{
		#if DEBUGLOG == 1
						R_BSP_CpuInterruptLevelWrite (13);
	//	                printf("recv timeout.%d received. requestsize=%d,lastdata=%02x,data1=%02x\r\n",recvcnt,length,*(pdata + (recvcnt-1)),data1);
						R_BSP_CpuInterruptLevelWrite (0);
		#endif
						R_BSP_NOP();
						break;
					}
					vTaskDelay(1);
				}
			}
			if(recvcnt == 0)
			{
				ret = sx_ulpgn_get_socket_status(socket_no);
				if(ret != ULPGN_SOCKET_STATUS_CONNECTED)
				{
					/* Give back the socketInUse mutex. */
					( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
					return 0;
				}
				/* socket is not closed, and recieve data size is 0. */
				( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				return 0;
			}
			/* socket is not closed, and recieve data size is 0. */
			( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
	#if DEBUGLOG == 1
			tmptime2 = xTaskGetTickCount();
			R_BSP_CpuInterruptLevelWrite (13);
			printf("r:%06d:tcp %ld byte received.reqsize=%ld,%x\r\n", tmptime2, recvcnt, length, (uint32_t)pdata);
			R_BSP_CpuInterruptLevelWrite (0);
	#endif
        }

    }

    if(ULPGN_USE_UART_NUM == 1)
    {
		if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
		{
			if(0 == g_ulpgn_socket[socket_no].socket_create_flag)
			{
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				return -1;
			}

	#if DEBUGLOG == 1
			R_BSP_CpuInterruptLevelWrite (13);
			printf("sx_ulpgn_tcp_recv(%d)\r\n", socket_no);
			R_BSP_CpuInterruptLevelWrite (0);
	#endif
			if((timeout_ms != 0) && (timeout_ms != portMAX_DELAY))
			{
				timeout_init(1, timeout_ms);
			}
			while(1)
			{
	//#if ULPGN_USE_UART_NUM == 1
				if(ULPGN_USE_UART_NUM == 1)
				{
					ercd = R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], (pdata + recvcnt), 1);
					if(SCI_SUCCESS == ercd)
					{
						recvcnt++;
						if(recvcnt >= length)
						{
							break;
						}
					}
					if((timeout_ms != 0) && (timeout_ms != portMAX_DELAY))
					{
						if(-1 == check_timeout(1, 0))
						{
							break;
						}
//						vTaskDelay(1);
					}
				}
	//#endif
			}
	#if DEBUGLOG == 1
			tmptime2 = xTaskGetTickCount();
			R_BSP_CpuInterruptLevelWrite (13);
			printf("r:%06d:tcp %ld byte received.reqsize=%ld,%x\r\n", tmptime2, recvcnt, length, (uint32_t)pdata);
			R_BSP_CpuInterruptLevelWrite (0);
	#endif

			/* Give back the socketInUse mutex. */
			( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
		}
		else
		{
			return 0;
		}
    }
    return recvcnt;
}

int32_t sx_ulpgn_serial_tcp_recv_timeout_set(uint8_t socket_no, TickType_t timeout_ms)
{
    g_sl_ulpgn_tcp_recv_timeout = timeout_ms;
    return 0;
}

int32_t sx_ulpgn_tcp_disconnect(uint8_t socket_no)
{
    int32_t ret = 0;
    if( xSemaphoreTake( g_sx_ulpgn_tx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) == pdTRUE )
        {
			if(1 == g_ulpgn_socket[socket_no].socket_create_flag)
			{
	#if DEBUGLOG == 1
				R_BSP_CpuInterruptLevelWrite (13);
				printf("sx_ulpgn_tcp_disconnect(%d)\r\n", socket_no);
				R_BSP_CpuInterruptLevelWrite (0);
	#endif
	//#if ULPGN_USE_UART_NUM == 2
				if(ULPGN_USE_UART_NUM == 2)
				{
					ret = sx_ulpgn_change_socket_index(socket_no);
					if(ret != 0)
					{
						/* Give back the socketInUse mutex. */
						( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
						( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
						return -1;
					}
				}
	//#endif

	//#if ULPGN_USE_UART_NUM == 1
				if(ULPGN_USE_UART_NUM == 1)
				{
					R_BSP_SoftwareDelay(201, BSP_DELAY_MILLISECS); /* 1s */
					R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], SCI_CMD_RX_Q_FLUSH, NULL);
					ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "+++", 3, 1000, ULPGN_RETURN_OK);
				}
	//#endif
				ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATNCLOSE\r", 3, 1000, ULPGN_RETURN_OK);
				if(0 == ret)
				{
					g_ulpgn_socket[socket_no].socket_create_flag = 0;
					g_ulpgn_socket[socket_no].socket_status = ULPGN_SOCKET_STATUS_CLOSED;
	//#if ULPGN_USE_UART_NUM == 2
					if(ULPGN_USE_UART_NUM == 2)
					{
						R_BYTEQ_Flush(g_ulpgn_socket[socket_no].socket_byteq_hdl);
					}
					R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
	//#endif
				}
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
			}
			else
			{
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_sx_ulpgn_rx_semaphore );
				( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
				return -1;
			}
        }
        else
        {
			( void ) xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
        }
    }
    else
    {
        return -1;
    }
    return ret;

}

int32_t sx_ulpgn_get_ipaddress(void)
{

    uint32_t result;
    uint32_t ipaddr[4];
    uint32_t subnetmask[4];
    uint32_t gateway[4];
    int32_t func_ret;
    int32_t scanf_ret;
    uint32_t count;

    func_ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATNSET=\?\r", 100, 3000, ULPGN_RETURN_OK);
    if(func_ret != 0)
    {
        return -1;
    }
    scanf_ret = sscanf((const char *)recvbuff, "IP:%d.%d.%d.%d, Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\r\n", \
                       &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3], \
                       &subnetmask[0], &subnetmask[1], &subnetmask[2], &subnetmask[3], \
                       &gateway[0], &gateway[1], &gateway[2], &gateway[3]);
    if(scanf_ret != 12)
    {
        return -1;
    }
    if((ipaddr[0] | ipaddr[1] | ipaddr[2] | ipaddr[3]) == 0)
    {
        return -1;
    }
    g_sx_ulpgn_ipaddress[0] = ipaddr[0];
    g_sx_ulpgn_ipaddress[1] = ipaddr[1];
    g_sx_ulpgn_ipaddress[2] = ipaddr[2];
    g_sx_ulpgn_ipaddress[3] = ipaddr[3];
    g_sx_ulpgn_subnetmask[0] = subnetmask[0];
    g_sx_ulpgn_subnetmask[1] = subnetmask[1];
    g_sx_ulpgn_subnetmask[2] = subnetmask[2];
    g_sx_ulpgn_subnetmask[3] = subnetmask[3];
    g_sx_ulpgn_gateway[0] = gateway[0];
    g_sx_ulpgn_gateway[1] = gateway[1];
    g_sx_ulpgn_gateway[2] = gateway[2];
    g_sx_ulpgn_gateway[3] = gateway[3];
    return 0;
}

int32_t sx_ulpgn_dns_query(const char *ptextstring, uint8_t *ip_addr)
{
    uint32_t result;
    int32_t func_ret;
    int32_t scanf_ret;
    uint32_t temp_addr[4];
    int32_t i = 0;
    uint8_t mutex_flag;

    if(ip_addr == NULL){
        return -1;
    }

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}


    strcpy((char *)buff, "ATNDNSQUERY=");
    sprintf((char *)buff + strlen((char *)buff), "%s\r", ptextstring);

    func_ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, (char *)buff, 3, 3000, ULPGN_RETURN_OK);
    if(func_ret != 0)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }
    scanf_ret = sscanf((const char *)recvbuff, "%lu\r\n%u.%u.%u.%u\r\n", &result, &temp_addr[0], &temp_addr[1], &temp_addr[2], &temp_addr[3]);
    if(scanf_ret != 5)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }
    if(result != 1)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }

    for(i =0;i<4;i++){
       if(temp_addr[i]<=255){
           ip_addr[i] = (uint8_t)temp_addr[i];
       }else{
           sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
           return -1;
       }
    }
	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);

    return 0;

}

int32_t sx_ulpgn_set_power_mode(const uint8_t powermode ,const void * submode)
{
    uint32_t result;
    int32_t func_ret;
    int32_t scanf_ret;
    uint8_t mutex_flag;

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

    strcpy((char *)buff, "\ATWPM=");
    sprintf((char *)buff + strlen((char *)buff), "%u\r", powermode);

    func_ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, (char *)buff, 3, 3000, ULPGN_RETURN_OK);
    if(func_ret != 0)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }
    scanf_ret = sscanf((const char *)recvbuff, "%lu\r\n", &result);
    if((scanf_ret != 1)&& (result != 0))
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }

	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    return 0;

}

int32_t sx_ulpgn_get_power_mode(uint8_t *power_mode ,void * sub_mode)
{
    uint32_t result;
    int32_t func_ret;
    int32_t scanf_ret;
    uint8_t response_power_mode[10]={0};
    const char * power_mode_dict[2]={"Max Perf","Power Save"};
    uint8_t mutex_flag;

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != sx_ulpgn_serial_send_basic_take_mutex(mutex_flag))
	{
    	return -1;
	}

    strcpy((char *)buff, "\ATWPM=?");

    func_ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, (char *)buff, 3, 3000, ULPGN_RETURN_OK);
    if(func_ret != 0)
    {
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }
    scanf_ret = sscanf((const char *)recvbuff, "Power Mode = %10s \r\n", response_power_mode);
    if(strcmp((const char *)response_power_mode,power_mode_dict[0]) )
    {
        *power_mode=0;
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return 0;
    }else if(strcmp((const char *)response_power_mode,power_mode_dict[1]))
    {
        *power_mode=1;
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return 0;
    }else{
    	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
        return -1;
    }

	sx_ulpgn_serial_send_basic_give_mutex(mutex_flag);
    return 0;

}



static int32_t sx_ulpgn_serial_send_basic_take_mutex(uint8_t mutex_flag)
{
	if(0 != (mutex_flag & MUTEX_TX))
	{
		if( xSemaphoreTake( g_sx_ulpgn_tx_semaphore, xMaxSemaphoreBlockTime ) != pdTRUE )
		{
        	return -1;
		}
	}

	if(0 != (mutex_flag & MUTEX_RX))
	{
		if( xSemaphoreTake( g_sx_ulpgn_rx_semaphore, xMaxSemaphoreBlockTime ) != pdTRUE )
		{
			if(0 != (mutex_flag & MUTEX_TX))
			{
				xSemaphoreGive( g_sx_ulpgn_tx_semaphore );
			}
			return -1;
		}
	}
	return 0;
}

static void sx_ulpgn_serial_send_basic_give_mutex(uint8_t mutex_flag)
{
	if(0 != (mutex_flag & MUTEX_RX))
	{
		xSemaphoreGive( g_sx_ulpgn_rx_semaphore);
	}
	if(0 != (mutex_flag & MUTEX_TX))
	{
		xSemaphoreGive( g_sx_ulpgn_tx_semaphore);
	}
	return;
}
uint8_t last_data[ULPGN_RETURN_TEXT_LENGTH];

static int32_t sx_ulpgn_serial_send_basic(uint8_t serial_ch_id, const char *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_ulpgn_return_code_t expect_code)
{
#if DEBUGLOG == 1
    TickType_t tmptime1, tmptime2;
#endif
    volatile int32_t timeout;
    sci_err_t ercd;
    uint32_t recvcnt = 0;
    uint8_t receive_data;
    uint8_t last_data_cnt = 0;
    uint8_t retry_count;

	for (retry_count = 0; retry_count < 10; retry_count++)
    {
		last_data_cnt = 0;
	    memset(recvbuff, 0, sizeof(recvbuff));
	    memset(last_data, 0, sizeof(last_data));

	    timeout_init(serial_ch_id, timeout_ms);

	    //uart_string_printf(ptextstring);
	    //uart_string_printf("\r\n");
	    if(ptextstring != NULL)
	    {
	        timeout = 0;
	        recvcnt = 0;
	        g_sx_ulpgn_uart_teiflag[serial_ch_id] = 0;
	        R_SCI_Control(sx_ulpgn_uart_sci_handle[serial_ch_id], SCI_CMD_RX_Q_FLUSH, NULL);
	        ercd = R_SCI_Send(sx_ulpgn_uart_sci_handle[serial_ch_id], (uint8_t *)ptextstring, (uint16_t)strlen((const char *)ptextstring));
	        if(SCI_SUCCESS != ercd)
	        {
	            return -1;
	        }

	        while(1)
	        {
	            if(0 != g_sx_ulpgn_uart_teiflag[serial_ch_id])
	                //      ercd = R_SCI_Control(sx_ulpgn_uart_sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &non_used);
	                //      if(non_used == SCI_TX_BUSIZ)
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
	            R_BSP_CpuInterruptLevelWrite (13);
	            printf("timeout.\r\n", tmptime1, ptextstring);
	            R_BSP_CpuInterruptLevelWrite (0);
	#endif
	            return -1;
	        }

	#if DEBUGLOG == 1
	        tmptime1 = xTaskGetTickCount();
	        if(ptextstring[strlen((const char *)ptextstring) - 1] != '\r')
	        {
	            R_BSP_CpuInterruptLevelWrite (13);
	            printf("s:%06d:%s\r\n", tmptime1, ptextstring);
	            R_BSP_CpuInterruptLevelWrite (0);
	        }
	        else
	        {
	            R_BSP_CpuInterruptLevelWrite (13);
	            printf("s:%06d:%s", tmptime1, ptextstring);
	            printf("\n");
	            R_BSP_CpuInterruptLevelWrite (0);
	        }
	#endif
	    }
	    while(1)
	    {
	        ercd = R_SCI_Receive(sx_ulpgn_uart_sci_handle[serial_ch_id], &receive_data, 1);
	        if(SCI_SUCCESS == ercd)
	        {
	        	recvbuff[recvcnt] = receive_data;
	            recvcnt++;
	            if(last_data_cnt < (ULPGN_RETURN_TEXT_LENGTH - 2))
	            {
	            	last_data[last_data_cnt] = receive_data;
	            	last_data_cnt++;
	            }
	            else
	            {
	            	memmove(&last_data[0],&last_data[1],ULPGN_RETURN_TEXT_LENGTH - 2);
	            	last_data_cnt--;
	            	last_data[last_data_cnt] = receive_data;
	            	last_data_cnt++;
	            }
	            bytetimeout_init(serial_ch_id, response_type);
	            if(recvcnt < 4)
	            {
	                continue;
	            }
	            if(recvcnt == sizeof(recvbuff) - 2)
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
	    if (0 == timeout)
	    {
			/* Success */
		}
	    else if (1 == timeout && 5 == retry_count)
	    {
            /* Error, no response */
	        return -1;
	    }
	    else
	    {
			/* Retry */
			continue;
		}

	    if(recvcnt == sizeof(recvbuff) - 2)
	    {
	        while(1)
	        {
				ercd = R_SCI_Receive(sx_ulpgn_uart_sci_handle[serial_ch_id], &receive_data, 1);
				if(SCI_SUCCESS == ercd)
				{
					memmove(&last_data[0],&last_data[1],ULPGN_RETURN_TEXT_LENGTH - 2);
	            	last_data_cnt--;
	            	last_data[last_data_cnt] = receive_data;
	            	last_data_cnt++;
					bytetimeout_init(serial_ch_id, response_type);
				}
				if(-1 == check_bytetimeout(serial_ch_id, recvcnt))
				{
					break;
				}
	        }
	    }

	#if DEBUGLOG == 1
	    tmptime2 = xTaskGetTickCount();
	    if(recvbuff[recvcnt - 1] != '\r')
	    {
	        recvbuff[recvcnt] = '\r';
	        recvbuff[recvcnt + 1] = '\0';
	    }
	    else
	    {
	        recvbuff[recvcnt] = '\0';
	    }
	    printf("r:%06d:%s", tmptime2, recvbuff);
	#endif
	    /* Response data check */
	    if(recvcnt < strlen((const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode]))
	    {
	        return -1;
	    }

	    const char* expected_result = ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode];
	    const uint32_t expected_len = strlen(expected_result);
	    uint32_t expected_offset = 0;

	    if(0 != strncmp((const char *)&last_data[last_data_cnt - strlen((const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode]) ],
	                    (const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode], strlen((const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode])))
	    {
	        if(0 == strncmp((const char *)&last_data[last_data_cnt - strlen((const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode]) ],
	                        (const char *)ulpgn_result_code[ULPGN_RETURN_BUSY][g_sx_ulpgn_return_mode], strlen((const char *)ulpgn_result_code[ULPGN_RETURN_BUSY][g_sx_ulpgn_return_mode])))
	        {
	            /* busy */
	            return -2;
	        }

		    if (5 == retry_count)
		    {
	            /* Error */
		        return -1;
			}
	    }
	    else
	    {
			/* Success */
			break;
		}
	}

    return 0;
}



/* return code: from 0 to 5 : socket status
 *              -1 : Error
 * */
static int32_t sx_ulpgn_get_socket_status(uint8_t socket_no)
{
    int32_t i;
    int32_t ret;
    char * str_ptr;
    uint8_t str[3][10];

    ret = sx_ulpgn_serial_send_basic(ULPGN_UART_COMMAND_PORT, "ATNSTAT\r", 3, 200, ULPGN_RETURN_OK);
    if(ret != 0)
    {
        return -1;
    }

    str_ptr = strchr((const char *)recvbuff, ',');
    if(str_ptr != NULL)
    {
        for(i = 0; i < ULPGN_SOCKET_STATUS_MAX; i++)
        {
            if(0 == strncmp((const char *)recvbuff, (const char *)ulpgn_socket_status[i], ((uint32_t)str_ptr - (uint32_t)recvbuff)))
            {
                break;
            }
        }
    }
    if(i < ULPGN_SOCKET_STATUS_MAX)
    {
        return i;
    }
    return -1;
}

//#if ULPGN_USE_UART_NUM == 2
static int32_t sx_ulpgn_change_socket_index(uint8_t socket_no)
{
#if DEBUGLOG == 1
    TickType_t tmptime1, tmptime2;
#endif
    volatile int32_t timeout;
    sci_err_t ercd;
    uint32_t receive_count = 0;
    int32_t ret = 0;
    uint16_t i;
    uint16_t stored_data_size = 0;
    uint16_t stored_response_size = 0;
    uint32_t previous_socket_store_data_size = 0;
    uint16_t read_data_size;
    uint8_t before_socket_no;
    int8_t receive_ret;
    uint8_t temprary_buff[64];

    uint32_t previous_atustat_sent,now_atustat_sent,atustat_recv,dummy;
    uint8_t sequence;
    uint8_t resequence_flag;
    uint8_t zero_cnt;

    if(ULPGN_USE_UART_NUM == 2)
    {
  
    	if(socket_no != current_socket_index)
        {
    		sequence = 0;
            before_socket_no = current_socket_index;
            stored_data_size = 0;
            zero_cnt = 0;

            sprintf((char *)buff, "ATNSOCKINDEX=%d\r", socket_no);

            ret = sx_ulpgn_check_uart_state(100, 400, &atustat_recv, &previous_atustat_sent);
            if(ret != 0)
            {
            	return -1;
            }
			R_BSP_SoftwareDelay(ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE, ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD);

            while(sequence < 0x80)
            {
            	switch(sequence)
            	{
            		case 0:
            			/* Read the remaining data three times and proceed to the next step without data. */
            			resequence_flag = 0;
                        /* Enable temporary byte que(default byte que). */
                        g_temporary_byteq_enable_flag = 1;
                        ret = sx_ulpgn_check_uart_state(100, 400, &atustat_recv, &now_atustat_sent);
                        if(ret != 0)
                        {
							ret = -1;
							sequence = 0x80;
							break;
                        }

                        do{

        					if(SCI_SUCCESS != R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
        					{
    							ret = -1;
    							sequence = 0x80;
    							break;
        					}
        					if(stored_data_size > 0)
        					{
        						resequence_flag = 1;
        						zero_cnt = 0;
        						if(sizeof(temprary_buff) >= stored_data_size)
            					{
            						read_data_size = stored_data_size;
            					}
            					else
            					{
            						read_data_size = sizeof(temprary_buff);
            					}

            					R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], temprary_buff, read_data_size);

            					for(i = 0; i < read_data_size; i++ )
            					{
            						if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_ulpgn_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i)))
									{
            			                g_ulpgn_socket[current_socket_index].socket_recv_error_count++;
									}
            					}

            					stored_data_size -= read_data_size;
        					}
        					else
        					{
        					}
        				}while(stored_data_size > 0);
                        if(now_atustat_sent == previous_atustat_sent)
                        {
    						zero_cnt++;
                        }
                        else
                        {
    						zero_cnt = 0;
                        }
                        previous_atustat_sent = now_atustat_sent;
            			if(zero_cnt < 2)
            			{
        					R_BSP_SoftwareDelay(ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE, ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD);
            				sequence = 0;
            			}
            			else
            			{
        					R_BSP_SoftwareDelay(ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE, ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD);
        					zero_cnt = 0;
            				sequence = 2;
            			}
            			break;
            		case 2:
                        receive_count = 0;
                        receive_ret = -1;
                        timeout = 0;
                        g_sx_ulpgn_uart_teiflag[ULPGN_UART_COMMAND_PORT] = 0;

                        /* Send ATNSOCKINDEX command. */

                        g_sockindex_command_flag =1;
						ercd = R_SCI_Send(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], (uint8_t *)buff, strlen((const char *)buff));

                        if(SCI_SUCCESS != ercd)
                        {
							ret = -1;
							sequence = 0x80;
							break;
                        }


                        /* Wait to send command. */
                        while(0 == g_sx_ulpgn_uart_teiflag[ULPGN_UART_COMMAND_PORT]);
                    	g_sockindex_command_flag =0;

                    	/* Just before the socket is switched to the next socket, secure the number of received data in the previous_socket_store_data_size. */
    					if(SCI_SUCCESS != R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
    					{
							ret = -1;
							sequence = 0x80;
							break;
    					}
                        /* Initualize command response. */
                        bytetimeout_init(ULPGN_UART_COMMAND_PORT, 1000);

                        /* Check stored data of command return code. */
                        do{

            	            if(SCI_SUCCESS != R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_response_size))
            	            {
								ret = -1;
								sequence = 0x80;
								break;
            	            }
                            if(-1 == check_bytetimeout(ULPGN_UART_COMMAND_PORT, 1000))
                            {
								ret = -1;
								sequence = 0x80;
								break;
                            }

                        }while(2 > stored_response_size);

                        /* Get command return code. */
                    	ercd = R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], recvbuff, 2);

                    	if(SCI_SUCCESS != ercd)
                        {
							ret = -1;
							sequence = 0x80;
							break;
                        }

                        /* Parse command return code. */
                    	if(recvbuff[1]=='\r')
                    	{
                    		switch(recvbuff[0])
                    		{
                    			case '0':
									/* change socket index to next. */
									current_socket_index = socket_no;

			                        ret = sx_ulpgn_check_uart_state(100, 400, &atustat_recv, &now_atustat_sent);
			                        if(ret != 0)
			                        {
										ret = -1;
										sequence = 0x80;
										break;
			                        }

			        				if(stored_data_size > 0)
			        				{
			        						previous_socket_store_data_size = now_atustat_sent - previous_atustat_sent;
											ULPGN_HSUART1_RTS_PODR = 0;
											/* Copy data(which stored during socket switching) from temporary buff to current socekt buff. */
											while(previous_socket_store_data_size > 0)
											{
												if(SCI_SUCCESS != R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
												{
													ret = -1;
													sequence = 0x80;
													break;
												}
												if(stored_data_size > 0)
												{
													if(sizeof(temprary_buff) >= stored_data_size)
													{
														read_data_size = stored_data_size;
													}
													else
													{
														read_data_size = sizeof(temprary_buff);
													}

													if(read_data_size > previous_socket_store_data_size)
													{
														read_data_size = previous_socket_store_data_size;
													}
													if(SCI_SUCCESS != R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], temprary_buff, read_data_size))
													{
														ret = -1;
														sequence = 0x80;
														break;
													}
													else
													{
														for(i = 0; i < read_data_size; i++ )
														{
															if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_ulpgn_socket[before_socket_no].socket_byteq_hdl, *(temprary_buff + i)))
															{
																g_ulpgn_socket[before_socket_no].socket_recv_error_count++;
															}
														}
														previous_socket_store_data_size -= read_data_size;
													}
												}
											}
			        				}
			        				else
			        				{
										ULPGN_HSUART1_RTS_PODR = 0;
			        				}
//			        				R_BSP_InterruptsDisable();
									/* Copy data(which stored during socket switching) from temporary buff to next socekt buff. */
									do{

										if(SCI_SUCCESS != R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
										{
											ret = -1;
											break;
										}
										if(sizeof(temprary_buff) >= stored_data_size)
										{
											read_data_size = stored_data_size;
										}
										else
										{
											read_data_size = sizeof(temprary_buff);
										}

										R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], temprary_buff, read_data_size);

										for(i = 0; i < read_data_size; i++ )
										{
											R_BYTEQ_Put(g_ulpgn_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i));
										}

										stored_data_size -= read_data_size;

									}while(stored_data_size > 0);
//			        				R_BSP_InterruptsEnable();
									/* Disable temporary byte que(default byte que). */
//#endif

									g_temporary_byteq_enable_flag = 0;
									ret = 0;
									sequence = 0x80;
									break;
                    			case '7':  /* switch(recvbuff[0]) */
                                    /* If dont change socket because of wifi module buzy, copy data from temporary buff to socekt buff
                                     * and retry change socket command. */

                 		            ULPGN_HSUART1_RTS_PODR = 0;

                					/* Wait 100us for module to complate send data. */
                					R_BSP_SoftwareDelay(100, BSP_DELAY_MICROSECS);
                		            ULPGN_HSUART1_RTS_PODR = 1;

                                    /* Copy data(which stored during socket switching) from temporary buff to current socekt buff. */
                					do{

                			            if(SCI_SUCCESS != R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
                			            {
        									ret = -1;
        									sequence = 0x80;
                			            }
                			            if(stored_data_size > 0)
                			            {
											if(sizeof(temprary_buff) >= stored_data_size)
											{
												read_data_size = stored_data_size;
											}
											else
											{
												read_data_size = sizeof(temprary_buff);
											}

											R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], temprary_buff, read_data_size);

											for(i = 0; i < read_data_size; i++ )
											{
												if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_ulpgn_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i)))
												{
													g_ulpgn_socket[current_socket_index].socket_recv_error_count++;
												}
											}

											stored_data_size -= read_data_size;
                			            }
									}while(stored_data_size > 0);
                					sequence = 0;
                    				break;
                        		default:   /* switch(recvbuff[0]) */
                        			/* This is command error and recovery socket status. */
									ret = -1;
									sequence = 0x80;
									break;
                    		}   /* end of switch(recvbuff[0]) */
                    	}
                    	else
                    	{
                           	ret = -1;
                    		sequence = 0x80;
                    	}
            			break;
            	}
            }
            
			if(-1 == ret)
			{
				
				R_BSP_InterruptsDisable();

				/* Copy data(which stored during socket switching) from temporary buff to current socekt buff. */
				do{						

					if(SCI_SUCCESS != R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
					{
						break;
					}
					if(stored_data_size > 0)
					{
						if(sizeof(temprary_buff) >= stored_data_size)
						{
							read_data_size = stored_data_size;
						}
						else
						{
							read_data_size = sizeof(temprary_buff);
						}

						R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_DATA_PORT], temprary_buff, read_data_size);

						for(i = 0; i < read_data_size; i++ )
						{
							if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_ulpgn_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i)))
							{
								g_ulpgn_socket[current_socket_index].socket_recv_error_count++;
							}
						}
						stored_data_size -= read_data_size;
					}
				}while(stored_data_size > 0);
				
				/* Disable temporary byte que(default byte que). */
				g_temporary_byteq_enable_flag = 0;
				ULPGN_HSUART1_RTS_PODR = 0;

				R_BSP_InterruptsEnable();
				
			}
            
        }
    
    }
    
    return ret;
}
//#endif

static int32_t sx_ulpgn_check_uart_state(uint16_t response_type, uint16_t timeout_ms,uint32_t *uart_receive_status, uint32_t *uart_send_status){

#if DEBUGLOG == 1
    TickType_t tmptime1, tmptime2;
#endif

    volatile int32_t timeout;
    sci_err_t ercd;
    uint32_t recvcnt = 0;
    uint8_t receive_data;
    uint8_t last_data_cnt = 0;
    uint8_t retry_count;
    uint8_t ptextstring[] = "ATUSTAT\r";
    uint8_t presponsestring[] = "recv=%lu sent=%lu\r\n%d\r";
    uint32_t ret;
    uint32_t previous_atustat_sent,now_atustat_sent,atustat_recv;

	for (retry_count = 0; retry_count < 10; retry_count++)
    {
		last_data_cnt = 0;
	    memset(recvbuff, 0, sizeof(recvbuff));
	    memset(last_data, 0, sizeof(last_data));

	    timeout_init(ULPGN_UART_COMMAND_PORT, timeout_ms);

	    if(ptextstring != NULL)
	    {
	        timeout = 0;
	        recvcnt = 0;
	        g_sx_ulpgn_uart_teiflag[ULPGN_UART_COMMAND_PORT] = 0;
	        R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], SCI_CMD_RX_Q_FLUSH, NULL);
	        ercd = R_SCI_Send(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], (uint8_t *)ptextstring, (uint16_t)strlen((const char *)ptextstring));
	        if(SCI_SUCCESS != ercd)
	        {
	            return -1;
	        }

	        while(1)
	        {
	            if(0 != g_sx_ulpgn_uart_teiflag[ULPGN_UART_COMMAND_PORT])
	            {
	                break;
	            }
	            if(-1 == check_timeout(ULPGN_UART_COMMAND_PORT, recvcnt))
	            {
	                timeout = 1;
	                break;
	            }
	        }
	        if(timeout == 1)
	        {
	#if DEBUGLOG == 1
	            R_BSP_CpuInterruptLevelWrite (13);
	            printf("timeout.\r\n", tmptime1, ptextstring);
	            R_BSP_CpuInterruptLevelWrite (0);
	#endif
	            return -1;
	        }

	#if DEBUGLOG == 1
	        tmptime1 = xTaskGetTickCount();
	        if(ptextstring[strlen((const char *)ptextstring) - 1] != '\r')
	        {
	            R_BSP_CpuInterruptLevelWrite (13);
	            printf("s:%06d:%s\r\n", tmptime1, ptextstring);
	            R_BSP_CpuInterruptLevelWrite (0);
	        }
	        else
	        {
	            R_BSP_CpuInterruptLevelWrite (13);
	            printf("s:%06d:%s", tmptime1, ptextstring);
	            printf("\n");
	            R_BSP_CpuInterruptLevelWrite (0);
	        }
	#endif
	    }
	    while(1)
	    {
	        ercd = R_SCI_Receive(sx_ulpgn_uart_sci_handle[ULPGN_UART_COMMAND_PORT], &receive_data, 1);
	        if(SCI_SUCCESS == ercd)
	        {
	        	recvbuff[recvcnt] = receive_data;
	            recvcnt++;
	            if(last_data_cnt < (ULPGN_RETURN_TEXT_LENGTH - 2))
	            {
	            	last_data[last_data_cnt] = receive_data;
	            	last_data_cnt++;
	            }
	            else
	            {
	            	memmove(&last_data[0],&last_data[1],ULPGN_RETURN_TEXT_LENGTH - 2);
	            	last_data_cnt--;
	            	last_data[last_data_cnt] = receive_data;
	            	last_data_cnt++;
	            }
	            bytetimeout_init(ULPGN_UART_COMMAND_PORT, response_type);
	            if(recvcnt < 4)
	            {
	                continue;
	            }
	            if(recvcnt == sizeof(recvbuff) - 2)
	            {
	                break;
	            }
	        }

            if(3 == sscanf(recvbuff,presponsestring,&atustat_recv, &now_atustat_sent, &ret))
            {
            	if(receive_data == '\r')
            	{
            		break;
            	}
            }

	        if(-1 == check_bytetimeout(ULPGN_UART_COMMAND_PORT, recvcnt))
	        {
	            break;
	        }
	        if(-1 == check_timeout(ULPGN_UART_COMMAND_PORT, recvcnt))
	        {
	            timeout = 1;
	            break;
	        }
	    }
	    if (0 == timeout)
	    {
			/* Success */
		}
	    else if (1 == timeout && 10 == retry_count)
	    {
            /* Error, no response */
	        return -1;
	    }
	    else
	    {
			/* Retry */
			continue;
		}


	#if DEBUGLOG == 1
	    tmptime2 = xTaskGetTickCount();
	    if(recvbuff[recvcnt - 1] != '\r')
	    {
	        recvbuff[recvcnt] = '\r';
	        recvbuff[recvcnt + 1] = '\0';
	    }
	    else
	    {
	        recvbuff[recvcnt] = '\0';
	    }
	    printf("r:%06d:%s", tmptime2, recvbuff);
	#endif
	    /* Response data check */
	    if(recvcnt < strlen((const char *)ulpgn_result_code[ULPGN_RETURN_OK][g_sx_ulpgn_return_mode]))
	    {
	        return -1;
	    }

	    const char* expected_result = ulpgn_result_code[ULPGN_RETURN_OK][g_sx_ulpgn_return_mode];
	    const uint32_t expected_len = strlen(expected_result);
	    uint32_t expected_offset = 0;

	    if(0 != strncmp((const char *)&last_data[last_data_cnt - strlen((const char *)ulpgn_result_code[ULPGN_RETURN_OK][g_sx_ulpgn_return_mode]) ],
	                    (const char *)ulpgn_result_code[ULPGN_RETURN_OK][g_sx_ulpgn_return_mode], strlen((const char *)ulpgn_result_code[ULPGN_RETURN_OK][g_sx_ulpgn_return_mode])))
	    {
	        if(0 == strncmp((const char *)&last_data[last_data_cnt - strlen((const char *)ulpgn_result_code[ULPGN_RETURN_OK][g_sx_ulpgn_return_mode]) ],
	                        (const char *)ulpgn_result_code[ULPGN_RETURN_BUSY][g_sx_ulpgn_return_mode], strlen((const char *)ulpgn_result_code[ULPGN_RETURN_BUSY][g_sx_ulpgn_return_mode])))
	        {
	            /* busy */
	            return -2;
	        }

		    if (10 == retry_count)
		    {
	            /* Error */
		        return -1;
			}
	    }
	    else
	    {
			/* Success */
	    	*uart_receive_status = atustat_recv;
	    	*uart_send_status = now_atustat_sent;
			break;
		}
	}

    return 0;
}

static void timeout_init(uint8_t serial_ch, uint32_t timeout_ms)
{
    starttime[serial_ch] = xTaskGetTickCount();
    endtime[serial_ch] = starttime[serial_ch] + timeout_ms;
    if(endtime[serial_ch] < starttime[serial_ch])
    {
        /* overflow */
        timeout_overflow_flag[serial_ch] = 1;
    }
    else
    {
        timeout_overflow_flag[serial_ch] = 0;
    }
}

static void tcp_timeout_init(uint8_t socket_no, uint32_t timeout_ms, uint8_t flag)
{
	TickType_t *starttime;
	TickType_t *thistime;
	TickType_t *endtime;
	uint8_t    *timeout_overflow_flag;
	if(0 == flag)
	{
		starttime             = &g_ulpgn_socket[socket_no].send_starttime ;
		thistime              = &g_ulpgn_socket[socket_no].send_thistime ;
		endtime               = &g_ulpgn_socket[socket_no].send_endtime ;
		timeout_overflow_flag = &g_ulpgn_socket[socket_no].send_timeout_overflow_flag;
	}
	else
	{
		starttime             = &g_ulpgn_socket[socket_no].recv_starttime ;
		thistime              = &g_ulpgn_socket[socket_no].recv_thistime ;
		endtime               = &g_ulpgn_socket[socket_no].recv_endtime ;
		timeout_overflow_flag = &g_ulpgn_socket[socket_no].recv_timeout_overflow_flag;
	}
    *starttime = xTaskGetTickCount();
    *endtime = *starttime + timeout_ms;
    if(*endtime < *starttime)
    {
        /* overflow */
        *timeout_overflow_flag = 1;
    }
    else
    {
        *timeout_overflow_flag = 0;
    }

}
static int32_t tcp_check_timeout(uint8_t socket_no, uint8_t flag)
{
	TickType_t *starttime;
	TickType_t *thistime;
	TickType_t *endtime;
	uint8_t    *timeout_overflow_flag;
	if(0 == flag)
	{
		starttime             = &g_ulpgn_socket[socket_no].send_starttime ;
		thistime              = &g_ulpgn_socket[socket_no].send_thistime ;
		endtime               = &g_ulpgn_socket[socket_no].send_endtime ;
		timeout_overflow_flag = &g_ulpgn_socket[socket_no].send_timeout_overflow_flag;
	}
	else
	{
		starttime             = &g_ulpgn_socket[socket_no].recv_starttime ;
		thistime              = &g_ulpgn_socket[socket_no].recv_thistime ;
		endtime               = &g_ulpgn_socket[socket_no].recv_endtime ;
		timeout_overflow_flag = &g_ulpgn_socket[socket_no].recv_timeout_overflow_flag;
	}
    *thistime = xTaskGetTickCount();
    if(*timeout_overflow_flag == 0)
    {
        if(*thistime >= *endtime || *thistime < *starttime)
        {
            return -1;
        }
    }
    else
    {
        if(*thistime < *starttime && *thistime <= *endtime)
        {
            /* Not timeout  */
            return -1;
        }
    }
    /* Not timeout  */
    return 0;
}

static int32_t check_timeout(uint8_t serial_ch, int32_t rcvcount)
{
    if(0 == rcvcount)
    {
        thistime[serial_ch] = xTaskGetTickCount();
        if(timeout_overflow_flag[serial_ch] == 0)
        {
            if(thistime[serial_ch] >= endtime[serial_ch] || thistime[serial_ch] < starttime[serial_ch])
            {
                return -1;
            }
        }
        else
        {
            if(thistime[serial_ch] < starttime[serial_ch] && thistime[serial_ch] <= endtime[serial_ch])
            {
                /* Not timeout  */
                return -1;
            }
        }
    }
    /* Not timeout  */
    return 0;
}

static void bytetimeout_init(uint8_t serial_ch, uint32_t timeout_ms)
{
    startbytetime[serial_ch] = xTaskGetTickCount();
    endbytetime[serial_ch] = startbytetime[serial_ch] + timeout_ms;
    if((endbytetime[serial_ch]) < startbytetime[serial_ch])
    {
        /* overflow */
        byte_timeout_overflow_flag[serial_ch] = 1;
    }
    else
    {
        byte_timeout_overflow_flag[serial_ch] = 0;
    }
}

static int32_t check_bytetimeout(uint8_t serial_ch, int32_t rcvcount)
{
    if(0 != rcvcount)
    {
        thisbytetime[serial_ch] = xTaskGetTickCount();
        if(byte_timeout_overflow_flag[serial_ch] == 0)
        {
            if(thisbytetime[serial_ch] >= endbytetime[serial_ch] || thisbytetime[serial_ch] < startbytetime[serial_ch])
            {
                return -1;
            }
        }
        else
        {
            if(thisbytetime[serial_ch] < startbytetime[serial_ch] && thisbytetime[serial_ch] <= endbytetime[serial_ch])
            {
                /* timeout  */
                return -1;
            }
        }
    }
    /* Not timeout  */
    return 0;
}

static int32_t sx_ulpgn_serial_open_for_initial(void)
{
    sci_err_t   my_sci_err;

    R_SCI_PinSet_sx_ulpgn_serial_default();

    memset(&sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], 0, sizeof(sci_hdl_t));
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.baud_rate    = 115200;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.clk_src      = SCI_CLK_INT;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.data_size    = SCI_DATA_8BIT;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.parity_en    = SCI_PARITY_OFF;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.parity_type  = SCI_EVEN_PARITY;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.stop_bits    = SCI_STOPBITS_1;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.int_priority = 14;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH_sx_ulpgn_serial_default, SCI_MODE_ASYNC, &g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT], sx_ulpgn_uart_callback_default_port_for_inititial, &sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT]);

    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    return 0;

}

static int32_t sx_ulpgn_serial_open_for_data(void)
{
    sci_err_t   my_sci_err;
    uint8_t level;

    R_SCI_PinSet_sx_ulpgn_serial_default();

    memset(&sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], 0, sizeof(sci_hdl_t));
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.baud_rate    = 230400;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.clk_src      = SCI_CLK_INT;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.data_size    = SCI_DATA_8BIT;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.parity_en    = SCI_PARITY_OFF;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.parity_type  = SCI_EVEN_PARITY;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.stop_bits    = SCI_STOPBITS_1;
    g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.int_priority = 14;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH_sx_ulpgn_serial_default, SCI_MODE_ASYNC, &g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT], sx_ulpgn_uart_callback_default_port_for_data, &sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT]);

    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    level = g_sx_ulpgn_sci_config[ULPGN_UART_DEFAULT_PORT].async.int_priority + 1;
    my_sci_err = R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], SCI_CMD_SET_RXI_PRIORITY,&level);
    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    return 0;

}


static int32_t sx_ulpgn_serial_second_port_open(void)
{
    sci_err_t   my_sci_err;
//#if ULPGN_USE_UART_NUM == 2
//  if(ULPGN_USE_UART_NUM == 2)
//  {

    R_SCI_PinSet_sx_ulpgn_serial_second();

    memset(&sx_ulpgn_uart_sci_handle[ULPGN_UART_SECOND_PORT], 0, sizeof(sci_hdl_t));
    g_sx_ulpgn_sci_config[ULPGN_UART_SECOND_PORT].async.baud_rate    = 115200;
    g_sx_ulpgn_sci_config[ULPGN_UART_SECOND_PORT].async.clk_src      = SCI_CLK_INT;
    g_sx_ulpgn_sci_config[ULPGN_UART_SECOND_PORT].async.data_size    = SCI_DATA_8BIT;
    g_sx_ulpgn_sci_config[ULPGN_UART_SECOND_PORT].async.parity_en    = SCI_PARITY_OFF;
    g_sx_ulpgn_sci_config[ULPGN_UART_SECOND_PORT].async.parity_type  = SCI_EVEN_PARITY;
    g_sx_ulpgn_sci_config[ULPGN_UART_SECOND_PORT].async.stop_bits    = SCI_STOPBITS_1;
    g_sx_ulpgn_sci_config[ULPGN_UART_SECOND_PORT].async.int_priority = 15;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH_sx_ulpgn_serial_second, SCI_MODE_ASYNC, &g_sx_ulpgn_sci_config[ULPGN_UART_DATA_PORT], sx_ulpgn_uart_callback_second_port_for_command, &sx_ulpgn_uart_sci_handle[ULPGN_UART_SECOND_PORT]);

    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }
//  }
//#endif

    return 0;
}

static int32_t sx_ulpgn_serial_close(void)
{
    sci_err_t   my_sci_err;

    my_sci_err = R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], SCI_CMD_RX_Q_FLUSH, NULL);
    my_sci_err = R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], SCI_CMD_TX_Q_FLUSH, NULL);
    my_sci_err = R_SCI_Close(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT]);
    return 0;
}

static int32_t sx_ulpgn_serial_second_port_close(void)
{
    sci_err_t   my_sci_err;

    my_sci_err = R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_SECOND_PORT], SCI_CMD_RX_Q_FLUSH, NULL);
    my_sci_err = R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_SECOND_PORT], SCI_CMD_TX_Q_FLUSH, NULL);
    my_sci_err = R_SCI_Close(sx_ulpgn_uart_sci_handle[ULPGN_UART_SECOND_PORT]);
    return 0;
}

int32_t sx_ulpgn_socket_init(void)
{
    int i;
    if(ULPGN_USE_UART_NUM == 2)
    {
        for(i = 0; i < g_sx_ulpgn_cleateble_sockets; i++)
        {
            if(BYTEQ_SUCCESS != R_BYTEQ_Open(g_ulpgn_socket[i].socket_recv_buff, sizeof(g_ulpgn_socket[i].socket_recv_buff), &g_ulpgn_socket[i].socket_byteq_hdl))
            {
                return -1;
            }
        }
    }

    /* Success. */
    return 0;

}
static void sx_ulpgn_uart_callback_default_port_for_inititial(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;
    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        R_BSP_NOP();
        R_BSP_NOP();
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
        g_sx_ulpgn_uart_teiflag[ULPGN_UART_DEFAULT_PORT] = 1;
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
        R_BSP_NOP();
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */



static void sx_ulpgn_uart_callback_second_port_for_command(void *pArgs)
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
        g_sx_ulpgn_uart_teiflag[ULPGN_UART_SECOND_PORT] = 1;
        if(g_sockindex_command_flag == 1)
        {
        	ULPGN_HSUART1_RTS_PODR = 1;
        }
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	uart_error_count[ULPGN_UART_SECOND_PORT].queue_overflow_count++;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	uart_error_count[ULPGN_UART_SECOND_PORT].eri_overflow_count++;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	uart_error_count[ULPGN_UART_SECOND_PORT].eri_framing_count++;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	uart_error_count[ULPGN_UART_SECOND_PORT].eri_parity_count++;
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */

static void sx_ulpgn_uart_callback_default_port_for_data(void *pArgs)
{
    sci_cb_args_t   *p_args;
    sci_err_t ret;
    byteq_err_t byteq_ret;
    uint8_t data;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        if((0 == g_temporary_byteq_enable_flag)&&(g_ulpgn_socket[current_socket_index].socket_create_flag == 1))
        {
            /* From RXI interrupt; received character data is in p_args->byte */
            R_SCI_Control(sx_ulpgn_uart_sci_handle[ULPGN_UART_DEFAULT_PORT], SCI_CMD_RX_Q_FLUSH, NULL);
            byteq_ret = R_BYTEQ_Put(g_ulpgn_socket[current_socket_index].socket_byteq_hdl, p_args->byte);
            if (byteq_ret != 0)
            {
                g_ulpgn_socket[current_socket_index].socket_recv_error_count++;
                return;
            }
            else
            {
            }
        }
        else
        {
        	g_ulpgn_socket[current_socket_index].socket_not_create_recv_count= p_args->byte;
        }
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
        g_sx_ulpgn_uart_teiflag[ULPGN_UART_DEFAULT_PORT] = 1;
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	uart_error_count[ULPGN_UART_DEFAULT_PORT].queue_overflow_count++;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	uart_error_count[ULPGN_UART_DEFAULT_PORT].eri_overflow_count++;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	uart_error_count[ULPGN_UART_DEFAULT_PORT].eri_framing_count++;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	uart_error_count[ULPGN_UART_DEFAULT_PORT].eri_parity_count++;
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */
