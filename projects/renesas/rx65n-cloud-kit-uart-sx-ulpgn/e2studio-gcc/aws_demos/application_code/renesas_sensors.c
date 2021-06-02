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
* File Name    : renesas_sensors.c
* Version      : 1.00
* Description  : Getting and initialization of sensor data.
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 11.03.2020  1.00    First Release
*********************************************************************************************************************/

#include <math.h>
#include "renesas_sensors.h"
#include "r_riic_rx_if.h"


struct bmi160_dev g_bmi160;
struct bme680_dev g_gas_sensor;
struct isl29035_dev g_isl_dev;

static int8_t init_isl29035(void)
{
    int8_t status = 0;

    /* ISL29035 Sensor Initialization */
    g_isl_dev.id = ISL29035_I2C_ADDR;
    g_isl_dev.interface = ISL29035_I2C_INTF;
    g_isl_dev.read = rxdemo_i2c_read;
    g_isl_dev.write = rxdemo_i2c_write;
    g_isl_dev.delay_ms = rxdemo_delay_ms;

    status = isl29035_init(&g_isl_dev);
    if( ISL29035_OK != status)
    {
        return status;
    }

    /* Configure ISL29035 ALS Sensor */
    status = isl29035_configure(&g_isl_dev);
    if( ISL29035_OK != status)
    {
        return status;
    }

    return status;
}

static int8_t init_bmi160(void)
{
    int8_t status;

    /* BMI160 Sensor Initialization */
    g_bmi160.id = BMI160_I2C_ADDR;
    g_bmi160.interface = BMI160_I2C_INTF;
    g_bmi160.read = rxdemo_i2c_read;
    g_bmi160.write = rxdemo_i2c_write;
    g_bmi160.delay_ms = rxdemo_delay_ms;

    status = bmi160_init(&g_bmi160);
    if( BMI160_OK != status)
    {
    	return status;
    }

    /* Configure BMI160 Accel and Gyro sensor */

    /* Select the Output data rate, range of accelerometer sensor */
    g_bmi160.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
    g_bmi160.accel_cfg.range = BMI160_ACCEL_RANGE_4G;
    g_bmi160.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

    /* Select the power mode of accelerometer sensor */
    g_bmi160.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    /* Select the Output data rate, range of Gyroscope sensor */
    g_bmi160.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
    g_bmi160.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
    g_bmi160.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

    /* Select the power mode of Gyroscope sensor */
    g_bmi160.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

    /* Set the sensor configuration */
    status = bmi160_set_sens_conf(&g_bmi160);

    return status;
}

static int8_t init_bme680(void)
{
    uint8_t set_required_settings;
    int8_t status;

    /* BME680 Sensor Initialization */
    g_gas_sensor.dev_id = BME680_I2C_ADDR_PRIMARY;
    g_gas_sensor.intf = BME680_I2C_INTF;
    g_gas_sensor.read = rxdemo_i2c_read;
    g_gas_sensor.write = rxdemo_i2c_write;
    g_gas_sensor.delay_ms = rxdemo_delay_ms;

    status = bme680_init(&g_gas_sensor);
    if( BME680_OK != status)
    {
        return status;
    }

    /* Configure BME680 Sensor */
    /* Set the temperature, pressure and humidity settings */
    g_gas_sensor.tph_sett.os_hum = BME680_OS_2X;
    g_gas_sensor.tph_sett.os_pres = BME680_OS_4X;
    g_gas_sensor.tph_sett.os_temp = BME680_OS_8X;
    g_gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    g_gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;

    /* Create a ramp heat waveform in 3 steps */
    g_gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
    g_gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    g_gas_sensor.power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL
                            | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    status = bme680_set_sensor_settings(set_required_settings,&g_gas_sensor);
    if( BME680_OK != status)
    {
        return status;
    }

    /* Set the power mode */
    status = bme680_set_sensor_mode(&g_gas_sensor);
    if( BME680_OK != status)
    {
        return status;
    }

    /* Get the total measurement duration so as to sleep or wait till the
     * measurement is complete */
    bme680_get_profile_dur(&g_gas_sensor.meas_period, &g_gas_sensor);

    return status;
}

/*
 * Convert the units for temperature from celcius to Fahrenheit
 */
static double convert_celsius_2_fahrenheit(double *temp_celsius)
{
    return (((*temp_celsius * 9) / 5) + 32);
}


uint8_t R_INIT_Sensors(void)
{
	uint8_t err = 0;

    /* Open I2C driver instance */
	riic_info_t iic_info;
	iic_info.dev_sts = RIIC_NO_INIT;
	iic_info.ch_no = 0;
	err = R_RIIC_Open(&iic_info);

	/* Initialize & Configure the BMI160 Sensor */
	if (0 == err)
	{
		err = init_bmi160();
	}

    /* Initialize & Configure the BME680 Sensor */
    if (0 == err)
    {
    	err = init_bme680();
    }

    /* Initialize & Configure the ISL29035 Sensor */
    if (0 == err)
    {
    	err = init_isl29035();
    }

    return err;
}


void R_READ_Sensors(st_sensors_t *sens)
{
    struct bme680_field_data  bme680_data;
    bmi160_data accel_data;
    bmi160_data gyro_data;
    double temp_value;
    uint8_t status;

    /* To read both Accel and Gyro data */
    status = bmi160_get_sensor_data(BMI160_BOTH_ACCEL_AND_GYRO, &accel_data, &gyro_data, &g_bmi160);
    if(BMI160_OK == status)
    {
        sens->accel.xacc = accel_data.x_axis;
        sens->accel.yacc = accel_data.y_axis;
        sens->accel.zacc = accel_data.z_axis;

        sens->gyro.xacc = gyro_data.x_axis;
        sens->gyro.yacc = gyro_data.y_axis;
        sens->gyro.zacc = gyro_data.z_axis;
    }

    /* Read temperature, pressure and humidity data */
    status = bme680_read_sensor(&bme680_data, &g_gas_sensor);
    if(BME680_OK == status)
    {
        temp_value = bme680_data.temperature/100.0f;
        sens->temperature = convert_celsius_2_fahrenheit(&temp_value);
        sens->humidity = ((double)bme680_data.humidity/1000.0f);
        sens->pressure = ((double)bme680_data.pressure/100.0f);
    }

    status = isl29035_read_als_data(&g_isl_dev, &sens->als);

    return status;
}

