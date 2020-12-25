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
* File Name    : sensor.h
* Description  : head file for sensor
***********************************************************************************************************************/

#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "r_cg_macrodriver_sensor.h"


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Slave address */
#define HS3001_SLAVE_ADDRESS		(0x88U)
#define ZMOD4450_SLAVE_ADDRESS		(0x64U)
#define ZMOD4410_SLAVE_ADDRESS		(0x64U)

#define TRUE						(1)
#define FALSE						(0)

#define SENSOR_OK					(0x00U)

#define HS3001_CON_ERR				(0x01U)
#define ZMOD4410_CON_ERR			(0x02U)
#define ZMOD4450_CON_ERR			(0x04U)
#define OB1203_CON_ERR				(0x08U)

#define HS3001_READ_ERR				(0x10U)
#define ZMOD4410_READ_ERR			(0x20U)
#define ZMOD4450_READ_ERR			(0x40U)
#define OB1203_READ_ERR				(0x80U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* HS3001 sensor */
typedef struct hs3001_data
{
	uint16_t temp_raw;
    double temperature_C;
    double temperature_F;

    uint16_t humidity_raw;
    double humidity;

}hs3001_data_t;

/* ZMOD4510 */
typedef struct zmod4450_4410_data
{
    float Tvoc_4410;		//Tovc
    float Iaq_4410;         //Iaq
    float Eco2_4410;        //ECO2
}zmod4450_4410_data_t;

/* Sensor and LCD displaying values */
typedef struct sensors
{
	MD_STATUS		HS3001_status;
    hs3001_data_t   HS3001_data; ///< Temperature C type
    MD_STATUS		ZMOD4450_4410_status;
    zmod4450_4410_data_t ZMOD4450_4410_data;

}sensors_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void sensors_init(sensors_t *sensors_data);
void iic20_callback(MD_STATUS flag);
void sensors_ZMOD4450_4410_read(sensors_t * sensor_data);
void sensors_temp_hum_read(sensors_t * sensor_data);
void ZMOD4410_Reset(void);
#endif /* _SENSORS_H_ */
