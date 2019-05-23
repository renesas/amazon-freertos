#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "platform.h"
#include "r_sci_rx_if.h"
#include "r_byteq_if.h"
#include "esp8266_driver.h"

#define ESP8266_DATA_RECEIVE_COMMAND "\r\n+IPD,%d,%d"
#define ESP8266_READ_MAC             "+CIPAPMAC"
#define ESP8266_READ_IP              "+CIPSTA"
#define ESP8266_READ_SNTP            "+CIPSNTPTIME"
#define ESP8266_READ_DNS             "+CIPDOMAIN"

void esp8266_response_set_result( uint8_t result );
uint8_t esp8266_response_get_now_queue(void);

extern uint8_t recvbuff[2048+20];
uint32_t recv_count;
uint32_t tmp_recvcnt;
uint8_t receive_status;
uint8_t receive_sub_status1;
uint8_t receive_sub_status2;
uint8_t recv_continue;

uint8_t macaddress[6];
uint8_t ipaddress[4];
uint8_t subnetmask[4];
uint8_t gateway[4];

uint16_t sntp_year;
uint8_t sntp_mon[5];
uint8_t sntp_day;
uint8_t sntp_hour;
uint8_t sntp_min;
uint8_t sntp_sec;
uint8_t sntp_week[5];

uint8_t dnsaddress[4];

esp8266_socket_t esp8266_socket[5];

uint8_t responce_wait_queue[10][3];
uint8_t responce_wait_queue_set_index;
uint8_t responce_wait_queue_wait_index;

static void esp8266_recv_task( void * pvParameters );

void vStart_esp8266_recv_task( void )
{
    BaseType_t x;

    /* Create the echo client tasks. */
    xTaskCreate( esp8266_recv_task,                               /* The function that implements the task. */
                 "esp8826_recv",                                     /* Just a text name for the task to aid debugging. */
                 512,
                 ( void * ) 0,                                    /* The task parameter, not used in this case. */
                 tskIDLE_PRIORITY + 10,
                 NULL );                                          /* The task handle is not used. */
}


static void esp8266_recv_task( void * pvParameters )
{
	uint8_t data;
	sci_err_t ercd;
	char * string_ercd;
	int sscanf_ret;
	int i;
	uint32_t socket_no;;
	uint32_t len;
	byteq_err_t byteq_ercd;
	memset(recvbuff,0,sizeof(recvbuff));
	recv_count = 0;

    for( ; ; )
    {
		ercd = R_SCI_Receive(esp8266_uart_sci_handle[ESP8266_UART_COMMAND_PORT], &data, 1);
    	if(SCI_ERR_INSUFFICIENT_DATA == ercd)
    	{
    		/* Pause for a short while to ensure the network is not too
         * congested. */
    		vTaskDelay( 1 );

    	}
    	else if(SCI_SUCCESS == ercd)
    	{
    		recvbuff[recv_count] = data;
    		recv_count++;
#if DEBUGLOG == 2
    		putchar(data);
#endif
    		switch(receive_status)
    		{
	    		case 0:
		    		switch(data)
		    		{
		    			case '\r':
		    				receive_status = 1;
			    			receive_sub_status1 = 0;
			    			receive_sub_status2 = 0;
		    			break;
		    			case '+':
		    				receive_status = 2;
			    			receive_sub_status1 = 0;
			    			receive_sub_status2 = 0;
		    			break;
		    			case '>':
		    				receive_status = 9;
			    			receive_sub_status1 = 0;
			    			receive_sub_status2 = 0;
			    			break;
		    			default:
		    				receive_status = 0xff;
			    			receive_sub_status1 = 0;
			    			receive_sub_status2 = 0;
		    			break;
		    		}
    			break;
	    		case 1:	/* \rから始まる \r\n????\r\n */
	    			switch(receive_sub_status1)
	    			{
		    			case 0:	/*\n待ち*/
		    				if(data == '\n')
		    				{
		    					receive_sub_status1 = 1;
		    				}
		    				break;
		    			case 1:	/*後半\r\n or > 待ち*/
		    				if(data == '+')
		    				{
		    					/*\r\n+IPD... */
		    					receive_status = 2;
		    					receive_sub_status1 = 0;
		    				}
		    				else
		    				{
		    					receive_sub_status1 = 2;
		    				}
		    				break;
		    			case 2:	/*後半\r\n or > 待ち*/
							{
								string_ercd = strstr(&recvbuff[2],"\r\n");
								if(string_ercd != NULL)
								{
									if(0 == strncmp(&recvbuff[2],ESP8266_RETURN_TEXT_OK,4))
									{
										if(ESP8266_SET_CIPSEND == esp8266_response_get_now_queue())
										{
					    					/* AT+CIPSENDのOKパターン */
					    					receive_status = 8;
		   					    			tmp_recvcnt = recv_count;
											break;
										}
										else
										{
											/* 通常のOK　*/
											esp8266_response_set_result( ESP8266_RETURN_OK );
										}
									}
									else if(0 == strncmp(&recvbuff[2],ESP8266_RETURN_TEXT_ERROR,7))
									{
										esp8266_response_set_result( ESP8266_RETURN_ERROR );
									}
									else if(0 == strncmp(&recvbuff[2],ESP8266_RETURN_TEXT_SEND_OK,9))
									{
										esp8266_response_set_result( ESP8266_RETURN_SEND_OK );
									}
									else if(0 == strncmp(&recvbuff[2],ESP8266_RETURN_TEXT_SEND_FAIL,11))
									{
										esp8266_response_set_result( ESP8266_RETURN_SEND_FAIL );
									}
									memset(recvbuff,0,sizeof(recvbuff));
									recv_count = 0;
			    					receive_status = 0;
					    			receive_sub_status1 = 0;
					    			receive_sub_status2 = 0;
								}
							}
		    				break;
	    			}
    				break;
	    		case 2:	/* +からはじまる　 +?????:???? */
	    			switch(receive_sub_status1)
	    			{
		    			case 0:	/* ':'待ち */
			    			if(data == ':')
		    				{
		    					receive_sub_status1 = 1;
   					    		recvbuff[recv_count-1] = '\0';
   					    		sscanf_ret = sscanf(recvbuff,ESP8266_DATA_RECEIVE_COMMAND,&socket_no,&len);
   					    		if(sscanf_ret == 2)
   					    		{
   					    			esp8266_socket[socket_no].receive_num = len;
   					    			esp8266_socket[socket_no].receive_count = 0;
   					    			receive_status = 3;
   					    			receive_sub_status1 = 0;
   					    		}
   					    		else if (0 == strcmp(recvbuff,ESP8266_READ_MAC))
		    					{
   					    			receive_status = 4;
   					    			receive_sub_status1 = 0;
   					    			tmp_recvcnt = recv_count;
		    					}
   					    		else if (0 == strcmp(recvbuff,ESP8266_READ_IP))
   					    		{
   					    			receive_status = 5;
   					    			receive_sub_status1 = 0;
   					    			tmp_recvcnt = recv_count;
   					    		}
   					    		else if (0 == strcmp(recvbuff,ESP8266_READ_SNTP))
   					    		{
   					    			receive_status = 6;
   					    			receive_sub_status1 = 0;
   					    			tmp_recvcnt = recv_count;
   					    		}
   					    		else if (0 == strcmp(recvbuff,ESP8266_READ_DNS))
   					    		{
   					    			receive_status = 7;
   					    			receive_sub_status1 = 0;
   					    			tmp_recvcnt = recv_count;
   					    		}
   					    		else
   					    		{

   					    		}
		    				}
	    					break;
		    			case 1:
	    				break;
	    			}
//    				string_ercd = strchr(const char *s, int c);
	    			break;
	    		case 3:	/* TCPデータ受信 */
	    			byteq_ercd = R_BYTEQ_Put(esp8266_socket[socket_no].socket_byteq_hdl, data);
	    			if(byteq_ercd != BYTEQ_SUCCESS)
	    			{
	    				esp8266_socket[socket_no].put_error_count++;
	    			}
	    			esp8266_socket[socket_no].receive_count++;
	    			if(esp8266_socket[socket_no].receive_count >= esp8266_socket[socket_no].receive_num)
	    			{
						memset(recvbuff,0,sizeof(recvbuff));
						recv_count = 0;
						receive_status = 0;
						receive_sub_status1 = 0;
						receive_sub_status2 = 0;
	    			}
	    			break;
	    		case 4:	/* MAC */
	    			if(0 == strcmp(&recvbuff[tmp_recvcnt],"\r\n"))
					{
	    				if(6 == sscanf(&recvbuff[tmp_recvcnt],"\"%d:%d:%d:%d:%d:%d\"\r\n", &macaddress[0],&macaddress[1],&macaddress[2],&macaddress[3],&macaddress[4],&macaddress[5]))
						{
							memset(recvbuff,0,sizeof(recvbuff));
							recv_count = 0;
							receive_status = 0;
							receive_sub_status1 = 0;
							receive_sub_status2 = 0;
						}
					}
					break;
	    		case 5:	/* IP */
	    			if(0 == strcmp(&recvbuff[tmp_recvcnt],"\r\n"))
					{
	    				if(6 == sscanf(&recvbuff[tmp_recvcnt],"%d:%d:%d:%d:%d:%d\r\n", &macaddress[0],&macaddress[1],&macaddress[2],&macaddress[3],&macaddress[4],&macaddress[5]))
						{
							memset(recvbuff,0,sizeof(recvbuff));
							recv_count = 0;
							receive_status = 0;
							receive_sub_status1 = 0;
							receive_sub_status2 = 0;
						}
					}
					break;
	    		case 6:	/* SNTP */
	    			switch(receive_sub_status1)
	    			{
						case 0:
							if(data == '\n')
							{
								receive_sub_status1 = 1;
								sscanf(&recvbuff[tmp_recvcnt],"%s%s%d %d:%d:%d %d\n",sntp_week,sntp_mon,&sntp_day,&sntp_hour,&sntp_min,&sntp_sec,&sntp_year);
								memset(recvbuff,0,sizeof(recvbuff));
								recv_count = 0;
								receive_status = 0;
								receive_sub_status1 = 0;
								receive_sub_status2 = 0;
							}
							break;
	    			}
					break;
	    		case 7:	/* DNS */
	    			if(NULL != strstr(&recvbuff[tmp_recvcnt],"\r\n"))
					{
	    				sscanf(&recvbuff[tmp_recvcnt],"%d.%d.%d.%d\r\n",&dnsaddress[0],&dnsaddress[1],&dnsaddress[2],&dnsaddress[3]);
						memset(recvbuff,0,sizeof(recvbuff));
						recv_count = 0;
						receive_status = 0;
						receive_sub_status1 = 0;
						receive_sub_status2 = 0;
					}
					break;
	    		case 8:	/* CIPSEND -> OK\r\n>  */
	    			if(0 == strcmp(&recvbuff[tmp_recvcnt],"> "))
					{
						esp8266_response_set_result( ESP8266_RETURN_OK_GO_SEND );
						memset(recvbuff,0,sizeof(recvbuff));
						recv_count = 0;
						receive_status = 0;
						receive_sub_status1 = 0;
						receive_sub_status2 = 0;
					}
					break;
	    		case 9:	/* CIPSEND -> OK\r\n>  */
					if(data == ' ')
					{
						esp8266_response_set_result( ESP8266_RETURN_OK_GO_SEND );
						memset(recvbuff,0,sizeof(recvbuff));
						recv_count = 0;
						receive_status = 0;
						receive_sub_status1 = 0;
						receive_sub_status2 = 0;
					}
					break;

	    		case 0xff:	/* その他  ????\r\n */
	    			switch(receive_sub_status1)
	    			{
		    			case 0:	/* \r\n待ち */
		    				string_ercd = strstr(&recvbuff[1],"\r\n");
		    				if(string_ercd != NULL)
		    				{
								if(NULL != strstr(&recvbuff[1],ESP8266_RETURN_TEXT_SEND_FAIL))
								{
									esp8266_response_set_result( ESP8266_RETURN_SEND_FAIL );
								}

		    					/* 読み捨て */
								memset(recvbuff,0,sizeof(recvbuff));
								recv_count = 0;
		    					receive_status = 0;
				    			receive_sub_status1 = 0;
				    			receive_sub_status2 = 0;
		    				}
		    				break;
	    			}
	    			break;
	    		default:
    				break;
    		}
    	}
    	else
    	{
    	}

    }	
}

void esp8266_response_set_queue( uint8_t command, uint8_t socket )
{
	responce_wait_queue[responce_wait_queue_set_index][0] = command;
	responce_wait_queue[responce_wait_queue_set_index][1] = socket;
	responce_wait_queue[responce_wait_queue_set_index][2] = 0xff;
	responce_wait_queue_set_index++;
	if(responce_wait_queue_set_index >= 10)
	{
		responce_wait_queue_set_index = 0;
	}
}

int8_t esp8266_response_get_queue( uint8_t command, uint8_t socket, uint8_t *result)
{
	int i;

	for(i = 0;i<10;i++)
	{
		if(responce_wait_queue[i][0] == command && responce_wait_queue[i][1] == socket)
		{
			if((responce_wait_queue[i][2] != 0xff) && (responce_wait_queue[i][2] != 0xfe))
			{
				*result = responce_wait_queue[i][2];
				responce_wait_queue[i][2] = 0xfe;
				break;
			}
		}
	}
	if(i>= 10)
	{
		return -1;
	}
	return 0;
}

uint8_t esp8266_response_get_now_queue(void)
{
	return 	responce_wait_queue[responce_wait_queue_wait_index][0];
}

void esp8266_response_set_result( uint8_t result )
{
	responce_wait_queue[responce_wait_queue_wait_index][2] = result;
	responce_wait_queue_wait_index++;
	if(responce_wait_queue_wait_index >= 10)
	{
		responce_wait_queue_wait_index = 0;
	}
}
