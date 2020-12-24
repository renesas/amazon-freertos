/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.

**********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : sensor.c
* Description  : sensor file
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <stddef.h>

#include "r_cg_userdefine_sensor.h"
#include "r_cg_serial_sensor.h"
#include "sensor.h"
#include "sw_delay.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
static MD_STATUS iic20_status;

/* Sensor target variables */
uint8_t adc_result[32] = { 0 };

/* Polling related variables -  if interrupt is used, they can be deleted */
uint8_t eoc_flag = 0;
uint8_t s_step_last = 0;
uint8_t s_step_new = 0;

static MD_STATUS  sensors_temp_hum_init(void);

#define ZMOD4410_RST_PIN 	P11_bit.no0
#define HIGH				1
#define LOW					0

void ZMOD4410_Reset(void)
{
	ZMOD4410_RST_PIN = HIGH;
	R_SoftwareDelay(100, DELAY_UNITS_MILLISECONDS);
	ZMOD4410_RST_PIN = LOW;
	R_SoftwareDelay(100, DELAY_UNITS_MILLISECONDS);
	ZMOD4410_RST_PIN = HIGH;
}

/*******************************************************************************************************************
* Name:       sensors_init
* Function:   Initialize sensors
* Parameters: None
* Return:     uint8_t, result of initializing sensors
*******************************************************************************************************************/
void sensors_init(sensors_t *sensors_data)
{
	iic20_status = MD_ERROR;

	iic20_status = sensors_temp_hum_init();
	sensors_data->HS3001_status = iic20_status;

	R_SoftwareDelay(100, DELAY_UNITS_MILLISECONDS);

}

/*******************************************************************************************************************
* Name:       sensors_temp_hum_init
* Function:   Initialize HS3001
* Parameters: None
* Return:     None
*******************************************************************************************************************/
static MD_STATUS sensors_temp_hum_init(void)
{
	iic20_status = MD_OK;

	iic20_status = write_device(HS3001_SLAVE_ADDRESS, NULL, 0, 5);  /* send address for data request */

    return (iic20_status);

}

/*******************************************************************************************************************
* Name:       sensors_temp_hum_read
* Function:   Read temperature and humidity data from HS3001
* Parameters: None
* Return:     None
*******************************************************************************************************************/
void sensors_temp_hum_read(sensors_t * sensor_data)
{

    unsigned int tem_data = 0;
    unsigned int hum_data = 0;
    unsigned char data[4] = {0, 0, 0, 0};

    if(MD_OK == sensor_data->HS3001_status)
  	{
    	iic20_status = MD_OK;
    	iic20_status = write_device(HS3001_SLAVE_ADDRESS, NULL, 0, 5);  /* send address for data request */
		//R_SoftwareDelay(50, DELAY_UNITS_MILLISECONDS);
		R_SoftwareDelay(100, DELAY_UNITS_MILLISECONDS);//modified by xj 20200619 different between GCC and CC-RL
		iic20_status = read_device(HS3001_SLAVE_ADDRESS,(uint8_t*)&data[0],sizeof(data),5);

		/* calculate temperature data(C) */
		tem_data = (unsigned int)data[2];
		tem_data = (tem_data << 8) + data[3];
		tem_data = tem_data >> 2;       /* Clear status bit: bit 15, bit 14*/
		sensor_data->HS3001_data.temp_raw = tem_data;
		sensor_data->HS3001_data.temperature_C= (long)tem_data;
		sensor_data->HS3001_data.temperature_C = (sensor_data->HS3001_data.temperature_C/(16384 - 1)) * 165 - 40;

		/* calculate temperature data(F)*/
		sensor_data->HS3001_data.temperature_F = sensor_data->HS3001_data.temperature_C * (long)9/(long)5 + (long)32;

		/* calculate humidity data */
		hum_data = (unsigned int)data[0];
		hum_data = (hum_data << 8 )+ data[1];
		hum_data = hum_data & 0x3FFFF;    /* get the data D13-D0*/
		sensor_data->HS3001_data.humidity_raw = hum_data;
		sensor_data->HS3001_data.humidity = (long)hum_data;
		sensor_data->HS3001_data.humidity = (sensor_data->HS3001_data.humidity/(16384 - 1)) * 100;
  	}
}


