/*********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
* THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
*********************************************************************************************************************/
/*********************************************************************************************************************
* File Name    : renesas_sensors.h
* Version      : 1.00
* Description  : Getting and initialization of sensor data.
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 11.03.2020  1.00    First Release
*********************************************************************************************************************/

#ifndef SENSORS_H_
#define SENSORS_H_

#include <sensorlib/rxdemo_i2c_api.h>
#include "sensorlib/bmi160_sensor.h"
#include "sensorlib/bme680_sensor.h"
#include "sensorlib/isl29035_sensor.h"


/** BMI160 values */
typedef struct bmi160_data_t {
    double     xacc;       ///< Acceleration on X-axis
    double     yacc;       ///< Acceleration on Y-axis
    double     zacc;       ///< Acceleration on Z-axis
} st_bmi160_data_t;

/** Sensor and LED values */
typedef struct sensors_t {
    st_bmi160_data_t      accel;       ///< Accelerometer
    st_bmi160_data_t      gyro;        ///< Gyroscope
    double                  temperature; ///< Temperature
    double                  humidity;    ///< Humidity
    double                  pressure;    ///< Barometric pressure
    double                  als;         ///< Ambient Light sensor
} st_sensors_t;

void R_READ_Sensors(struct sensors_t *sens);
uint8_t R_INIT_Sensors(void);

#endif /* SENSORS_H_ */
