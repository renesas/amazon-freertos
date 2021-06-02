/**
 * Copyright (C) 2015 - 2016 Bosch Sensortec GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 *
 * @file    bmi160.c
 * @date    16 Oct 2017
 * @version 3.7.2
 * @brief
 *
 */
#include <sensorlib/rxdemo_i2c_api.h>
#include "bmi160_sensor.h"


/*!
 * @brief This API reads the data from the given register address
 * of sensor.
 */
static int8_t bmi160_get_regs(uint8_t reg_addr, uint8_t *data, uint16_t len, const struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;

    /* Null-pointer check */
    if ((dev == NULL) || (dev->read == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        rslt = dev->read(dev->id,reg_addr, data, len);
        /* Kindly refer section 3.2.4 of data-sheet*/
        dev->delay_ms(1);
        if (rslt != BMI160_OK)
            rslt = BMI160_E_COM_FAIL;
    }

    return rslt;
}

/*!
 * @brief This API writes the given data to the register address
 * of sensor.
 */
static int8_t bmi160_set_regs(uint8_t reg_addr, uint8_t *data, uint16_t len, const struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;
    uint8_t count = 0;
    /* Null-pointer check */
    if ((dev == NULL) || (dev->write == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        if ((dev->prev_accel_cfg.power == BMI160_ACCEL_NORMAL_MODE) ||
            (dev->prev_gyro_cfg.power == BMI160_GYRO_NORMAL_MODE)) {

            rslt = dev->write(dev->id,reg_addr, data, len);
            /* Kindly refer section 3.2.4 of data-sheet*/
            dev->delay_ms(1);
        } else {
            /*Burst write is not allowed in
            suspend & low power mode */
            for (; count < len; count++) {
                rslt = dev->write(dev->id,reg_addr, &data[count], 1);
                reg_addr++;
                /* Kindly refer section 3.2.4 of data-sheet*/
                dev->delay_ms(1);
            }
        }

        if (rslt != BMI160_OK)
            rslt = BMI160_E_COM_FAIL;
    }

    return rslt;
}

/*!
 * @brief This API sets the default configuration parameters of accel & gyro.
 * Also maintain the previous state of configurations.
 */
static void default_param_settg(struct bmi160_dev *dev)
{
    /* Initializing accel and gyro params with
    * default values */
    dev->accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;
    dev->accel_cfg.odr = BMI160_ACCEL_ODR_100HZ;
    dev->accel_cfg.power = BMI160_ACCEL_SUSPEND_MODE;
    dev->accel_cfg.range = BMI160_ACCEL_RANGE_2G;
    dev->gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;
    dev->gyro_cfg.odr = BMI160_GYRO_ODR_100HZ;
    dev->gyro_cfg.power = BMI160_GYRO_SUSPEND_MODE;
    dev->gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;

    /* To maintain the previous state of accel configuration */
    dev->prev_accel_cfg = dev->accel_cfg;
    /* To maintain the previous state of gyro configuration */
    dev->prev_gyro_cfg = dev->gyro_cfg;
}

/*!
 * @brief This internal API is used to validate the device structure pointer for
 * null conditions.
 */
static int8_t null_ptr_check(const struct bmi160_dev *dev)
{
    int8_t rslt;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_ms == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        /* Device structure is fine */
        rslt = BMI160_OK;
    }

    return rslt;
}

/*!
 * @brief This API resets and restarts the device.
 * All register values are overwritten with default parameters.
 */
int8_t bmi160_soft_reset(struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t data = BMI160_SOFT_RESET_CMD;

    /* Null-pointer check */
    if ((dev == NULL) || (dev->delay_ms == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        /* Reset the device */
        rslt = bmi160_set_regs(BMI160_COMMAND_REG_ADDR, &data, 1, dev);
        dev->delay_ms(BMI160_SOFT_RESET_DELAY_MS);

        if (rslt == BMI160_OK) {
            /* Update the default parameters */
            default_param_settg(dev);
        }
    }

    return rslt;
}

/*!
 * @brief This API process the accel odr.
 */
static int8_t process_accel_odr(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t temp = 0;
    uint8_t odr = 0;

    if (dev->accel_cfg.odr <= BMI160_ACCEL_ODR_MAX) {
        if (dev->accel_cfg.odr != dev->prev_accel_cfg.odr) {
            odr = (uint8_t)dev->accel_cfg.odr;
            temp = *data & (uint8_t)(~BMI160_ACCEL_ODR_MASK);
            /* Adding output data rate */
            *data = (uint8_t)(temp | (odr & BMI160_ACCEL_ODR_MASK));
        }
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}

/*!
 * @brief This API process the accel bandwidth.
 */
static int8_t process_accel_bw(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t temp = 0;
    uint8_t bw = 0;

    if (dev->accel_cfg.bw <= BMI160_ACCEL_BW_MAX) {
        if (dev->accel_cfg.bw != dev->prev_accel_cfg.bw) {
            bw = (uint8_t)dev->accel_cfg.bw;
            temp = *data & (uint8_t)(~BMI160_ACCEL_BW_MASK);
            /* Adding bandwidth */
            *data = temp | ((bw << 4) & BMI160_ACCEL_ODR_MASK);
        }
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}

/*!
 * @brief This API process the accel range.
 */
static int8_t process_accel_range(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t temp = 0;
    uint8_t range = 0;

    if (dev->accel_cfg.range <= BMI160_ACCEL_RANGE_MAX) {
        if (dev->accel_cfg.range != dev->prev_accel_cfg.range) {
            range = (uint8_t)dev->accel_cfg.range;
            temp = *data & (uint8_t)(~BMI160_ACCEL_RANGE_MASK);
            /* Adding range */
            *data = (uint8_t)(temp | (range & BMI160_ACCEL_RANGE_MASK));
        }
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}

/*!
* @brief This API check the accel configuration.
*/
static int8_t check_accel_config(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt;

    /* read accel Output data rate and bandwidth */
    rslt = bmi160_get_regs(BMI160_ACCEL_CONFIG_ADDR, data, 2, dev);
    if (rslt == BMI160_OK) {
        rslt = process_accel_odr(&data[0], dev);
        if (rslt == BMI160_OK) {
            rslt = process_accel_bw(&data[0], dev);
            if (rslt == BMI160_OK)
                rslt = process_accel_range(&data[1], dev);
        }
    }

    return rslt;
}

/*!
 * @brief This API set the accel configuration.
 */
static int8_t set_accel_conf(struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t data[2]  = {0};

    rslt = check_accel_config(data, dev);
    if (rslt == BMI160_OK) {
        /* Write output data rate and bandwidth */
        rslt = bmi160_set_regs(BMI160_ACCEL_CONFIG_ADDR, &data[0], 1, dev);
        if (rslt == BMI160_OK) {
            dev->prev_accel_cfg.odr = dev->accel_cfg.odr;
            dev->prev_accel_cfg.bw = dev->accel_cfg.bw;
            dev->delay_ms(BMI160_ONE_MS_DELAY);
            /* write accel range */
            rslt = bmi160_set_regs(BMI160_ACCEL_RANGE_ADDR, &data[1], 1, dev);
            if (rslt == BMI160_OK)
                dev->prev_accel_cfg.range = dev->accel_cfg.range;
        }
    }

    return rslt;
}

/*!
 * @brief This API process the gyro odr.
 */
static int8_t process_gyro_odr(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t temp = 0;
    uint8_t odr = 0;

    if (dev->gyro_cfg.odr <= BMI160_GYRO_ODR_MAX) {
        if (dev->gyro_cfg.odr != dev->prev_gyro_cfg.odr) {
            odr = (uint8_t)dev->gyro_cfg.odr;
            temp = (*data & (uint8_t)(~BMI160_GYRO_ODR_MASK));
            /* Adding output data rate */
            *data = (uint8_t)(temp | (odr & BMI160_GYRO_ODR_MASK));
        }
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}

/*!
 * @brief This API process the gyro bandwidth.
 */
static int8_t process_gyro_bw(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t temp = 0;
    uint8_t bw = 0;

    if (dev->gyro_cfg.bw <= BMI160_GYRO_BW_MAX) {
        bw = (uint8_t)dev->gyro_cfg.bw;
        temp = *data & (uint8_t)(~BMI160_GYRO_BW_MASK);
        /* Adding bandwidth */
        *data = (uint8_t)(temp | ((bw << 4) & BMI160_GYRO_BW_MASK));
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}

/*!
 * @brief This API process the gyro range.
 */
static int8_t process_gyro_range(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t temp = 0;
    uint8_t range = 0;

    if (dev->gyro_cfg.range <= BMI160_GYRO_RANGE_MAX) {
        if (dev->gyro_cfg.range != dev->prev_gyro_cfg.range) {
            range = (uint8_t)dev->gyro_cfg.range;
            temp = *data & (uint8_t)(~BMI160_GYRO_RANGE_MSK);
            /* Adding range */
            *data = (uint8_t)(temp | (range & BMI160_GYRO_RANGE_MSK));
        }
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}


/*!
* @brief This API check the gyro configuration.
*/
static int8_t check_gyro_config(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt;

    /* read gyro Output data rate and bandwidth */
    rslt = bmi160_get_regs(BMI160_GYRO_CONFIG_ADDR, data, 2, dev);
    if (rslt == BMI160_OK) {
        rslt = process_gyro_odr(&data[0], dev);
        if (rslt == BMI160_OK) {
            rslt = process_gyro_bw(&data[0], dev);
            if (rslt == BMI160_OK)
                rslt = process_gyro_range(&data[1], dev);
        }
    }

    return rslt;

}

static int8_t set_gyro_conf(struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t data[2] = {0};

    rslt = check_gyro_config(data, dev);
    if (rslt == BMI160_OK) {
        /* Write output data rate and bandwidth */
        rslt = bmi160_set_regs(BMI160_GYRO_CONFIG_ADDR, &data[0], 1, dev);
        if (rslt == BMI160_OK) {
            dev->prev_gyro_cfg.odr = dev->gyro_cfg.odr;
            dev->prev_gyro_cfg.bw = dev->gyro_cfg.bw;
            dev->delay_ms(BMI160_ONE_MS_DELAY);
            /* Write gyro range */
            rslt = bmi160_set_regs(BMI160_GYRO_RANGE_ADDR, &data[1], 1, dev);
            if (rslt == BMI160_OK)
                dev->prev_gyro_cfg.range = dev->gyro_cfg.range;
        }
    }

    return rslt;
}

/*!
 * @brief This API process the undersampling setting of Accel.
 */
static int8_t process_under_sampling(uint8_t *data, const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t temp = 0;
    uint8_t pre_filter = 0;

    rslt = bmi160_get_regs(BMI160_ACCEL_CONFIG_ADDR, data, 1, dev);
    if (rslt == BMI160_OK) {
        if (dev->accel_cfg.power == BMI160_ACCEL_LOWPOWER_MODE) {
            temp = *data & (uint8_t)(~BMI160_ACCEL_UNDERSAMPLING_MASK);
            /* Set under-sampling parameter */
            *data = temp | ((1 << 7) & BMI160_ACCEL_UNDERSAMPLING_MASK);
            /* Write data */
            rslt = bmi160_set_regs(BMI160_ACCEL_CONFIG_ADDR, data, 1, dev);
            /* disable the pre-filter data in
             * low power mode */
            if (rslt == BMI160_OK)
                /* Disable the Pre-filter data*/
                rslt = bmi160_set_regs(BMI160_INT_DATA_0_ADDR, &pre_filter, 2, dev);
        } else {
            if (*data & BMI160_ACCEL_UNDERSAMPLING_MASK) {

                temp = *data & (uint8_t)(~BMI160_ACCEL_UNDERSAMPLING_MASK);
                /* disable under-sampling parameter
                if already enabled */
                *data = temp;
                /* Write data */
                rslt = bmi160_set_regs(BMI160_ACCEL_CONFIG_ADDR, data, 1, dev);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This API sets the accel power.
 */
static int8_t set_accel_pwr(struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t data = 0;

    if ((dev->accel_cfg.power >= BMI160_ACCEL_SUSPEND_MODE) &&
        (dev->accel_cfg.power <= BMI160_ACCEL_LOWPOWER_MODE)) {
        if (dev->accel_cfg.power != dev->prev_accel_cfg.power) {
            rslt = process_under_sampling(&data, dev);
            if (rslt == BMI160_OK) {
                /* Write accel power */
                rslt = bmi160_set_regs(BMI160_COMMAND_REG_ADDR, &dev->accel_cfg.power, 1, dev);
                /* Add delay of 5 ms */
                if (dev->prev_accel_cfg.power == BMI160_ACCEL_SUSPEND_MODE)
                    dev->delay_ms(BMI160_ACCEL_DELAY_MS);
                dev->prev_accel_cfg.power = dev->accel_cfg.power;

                rslt = bmi160_get_regs(BMI160_PMU_STATUS_REG_ADDR, &data, 1, dev);

            }
        }
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}

/*!
 * @brief This API sets the gyro power mode.
 */
static int8_t set_gyro_pwr(struct bmi160_dev *dev)
{
    int8_t rslt = 0;
    uint8_t data = 0;

    if ((dev->gyro_cfg.power == BMI160_GYRO_SUSPEND_MODE) || (dev->gyro_cfg.power == BMI160_GYRO_NORMAL_MODE)
        || (dev->gyro_cfg.power == BMI160_GYRO_FASTSTARTUP_MODE)) {

        if (dev->gyro_cfg.power != dev->prev_gyro_cfg.power) {

            /* Write gyro power */
            rslt = bmi160_set_regs(BMI160_COMMAND_REG_ADDR, &dev->gyro_cfg.power, 1, dev);
            if (dev->prev_gyro_cfg.power == BMI160_GYRO_SUSPEND_MODE) {
                /* Delay of 81 ms */
                dev->delay_ms(BMI160_GYRO_DELAY_MS);
            } else if ((dev->prev_gyro_cfg.power == BMI160_GYRO_FASTSTARTUP_MODE)
                && (dev->gyro_cfg.power == BMI160_GYRO_NORMAL_MODE)) {
                /* This delay is required for transition from
                fast-startup mode to normal mode */
                dev->delay_ms(10);
            } else {
                /* do nothing */
            }
            dev->prev_gyro_cfg.power = dev->gyro_cfg.power;


            rslt = bmi160_get_regs(BMI160_PMU_STATUS_REG_ADDR, &data, 1, dev);
        }
    } else {
        rslt = BMI160_E_OUT_OF_RANGE;
    }

    return rslt;
}

/*!
 * @brief This API sets the power mode of the sensor.
 */
int8_t bmi160_set_power_mode(struct bmi160_dev *dev)
{
    int8_t rslt = 0;

    /* Null-pointer check */
    if ((dev == NULL) || (dev->delay_ms == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        rslt = set_accel_pwr(dev);
        if (rslt == BMI160_OK)
            rslt = set_gyro_pwr(dev);
    }

    return rslt;

}
/*!
 * @brief This API checks the invalid settings for ODR & Bw for
 * Accel and Gyro.
 */
static int8_t check_invalid_settg(const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t data = 0;

    /* read the error reg */
    rslt = bmi160_get_regs(BMI160_ERROR_REG_ADDR, &data, 1, dev);

    data = data >> 1;
    data = data & BMI160_ERR_REG_MASK;
    if (data == 1)
        rslt = BMI160_E_ACCEL_ODR_BW_INVALID;
    else if (data == 2)
        rslt = BMI160_E_GYRO_ODR_BW_INVALID;
    else if (data == 3)
        rslt = BMI160_E_LWP_PRE_FLTR_INT_INVALID;
    else if (data == 7)
        rslt = BMI160_E_LWP_PRE_FLTR_INVALID;

    return rslt;
}

/*!
 * @brief This API reads accel data along with sensor time if time is requested
 * by user. Kindly refer the user guide(README.md) for more info.
 */
static int8_t get_gyro_data(uint8_t len, struct bmi160_sensor_data *gyro, const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t idx = 0;
    uint8_t data_array[15] = {0};
    uint8_t time_0 = 0;
    uint16_t time_1 = 0;
    uint32_t time_2 = 0;
    uint8_t lsb;
    uint8_t msb;
    int16_t msblsb;

    if (len == 0) {
        /* read gyro data only */
        rslt = bmi160_get_regs(BMI160_GYRO_DATA_ADDR, data_array, 6, dev);
        if (rslt == BMI160_OK) {
            /* Gyro Data */
            lsb = data_array[idx++];
            msb = data_array[idx++];
            msblsb = (int16_t)((msb << 8) | lsb);
            gyro->x = msblsb; /* Data in X axis */

            lsb = data_array[idx++];
            msb = data_array[idx++];
            msblsb = (int16_t)((msb << 8) | lsb);
            gyro->y = msblsb; /* Data in Y axis */

            lsb = data_array[idx++];
            msb = data_array[idx++];
            msblsb = (int16_t)((msb << 8) | lsb);
            gyro->z = msblsb; /* Data in Z axis */
            gyro->sensortime = 0;

        } else {
            rslt = BMI160_E_COM_FAIL;
        }
    } else {
        /* read gyro sensor data along with time */
        rslt = bmi160_get_regs(BMI160_GYRO_DATA_ADDR, data_array, (uint16_t)(12 + len), dev);
        if (rslt == BMI160_OK) {
            /* Gyro Data */
            lsb = data_array[idx++];
            msb = data_array[idx++];
            msblsb = (int16_t)((msb << 8) | lsb);
            gyro->x = msblsb; /* gyro X axis data */

            lsb = data_array[idx++];
            msb = data_array[idx++];
            msblsb = (int16_t)((msb << 8) | lsb);
            gyro->y = msblsb; /* gyro Y axis data */

            lsb = data_array[idx++];
            msb = data_array[idx++];
            msblsb = (int16_t)((msb << 8) | lsb);
            gyro->z = msblsb; /* gyro Z axis data */

            idx = (uint8_t)(idx + 6);
            time_0 = data_array[idx++];
            time_1 = (uint16_t)(data_array[idx++] << 8);
            time_2 = (uint32_t)(data_array[idx++] << 16);
            gyro->sensortime = (uint32_t)(time_2 | time_1 | time_0);

        } else {
            rslt = BMI160_E_COM_FAIL;
        }
    }

    return rslt;
}

/*!
 * @brief This API reads accel and gyro data along with sensor time
 * if time is requested by user.
 *  Kindly refer the user guide(README.md) for more info.
 */
static int8_t get_accel_gyro_data(uint8_t len, struct bmi160_sensor_data *accel, struct bmi160_sensor_data *gyro,
                    const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t idx = 0;
    uint8_t data_array[15] = {0};
    uint8_t time_0 = 0;
    uint16_t time_1 = 0;
    uint32_t time_2 = 0;
    uint8_t lsb;
    uint8_t msb;
    int16_t msblsb;

    /* read both accel and gyro sensor data
     * along with time if requested */
    rslt = bmi160_get_regs(BMI160_GYRO_DATA_ADDR, data_array, (uint16_t)(12 + len), dev);
    if (rslt == BMI160_OK) {
        /* Gyro Data */
        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        gyro->x = msblsb; /* gyro X axis data */

        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        gyro->y = msblsb; /* gyro Y axis data */

        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        gyro->z = msblsb; /* gyro Z axis data */

        /* Accel Data */
        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        accel->x = (int16_t)msblsb; /* accel X axis data */

        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        accel->y = (int16_t)msblsb; /* accel Y axis data */

        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        accel->z = (int16_t)msblsb; /* accel Z axis data */

        if (len == 3) {
            time_0 = data_array[idx++];
            time_1 = (uint16_t)(data_array[idx++] << 8);
            time_2 = (uint32_t)(data_array[idx++] << 16);
            accel->sensortime = (uint32_t)(time_2 | time_1 | time_0);
            gyro->sensortime = (uint32_t)(time_2 | time_1 | time_0);
        } else {
            accel->sensortime = 0;
            gyro->sensortime = 0;
        }
    } else {
        rslt = BMI160_E_COM_FAIL;
    }

    return rslt;
}

/*!
 * @brief This API reads accel data along with sensor time if time is requested
 * by user. Kindly refer the user guide(README.md) for more info.
 */
static int8_t get_accel_data(uint8_t len, struct bmi160_sensor_data *accel, const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t idx = 0;
    uint8_t data_array[9] = {0};
    uint8_t time_0 = 0;
    uint16_t time_1 = 0;
    uint32_t time_2 = 0;
    uint8_t lsb;
    uint8_t msb;
    int16_t msblsb;

    /* read accel sensor data along with time if requested */
    rslt = bmi160_get_regs(BMI160_ACCEL_DATA_ADDR, data_array, (uint16_t)(6 + len), dev);
    if (rslt == BMI160_OK) {

        /* Accel Data */
        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        accel->x = msblsb; /* Data in X axis */

        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        accel->y = msblsb; /* Data in Y axis */

        lsb = data_array[idx++];
        msb = data_array[idx++];
        msblsb = (int16_t)((msb << 8) | lsb);
        accel->z = msblsb; /* Data in Z axis */

        if (len == 3) {
            time_0 = data_array[idx++];
            time_1 = (uint16_t)(data_array[idx++] << 8);
            time_2 = (uint32_t)(data_array[idx++] << 16);
            accel->sensortime = (uint32_t)(time_2 | time_1 | time_0);
        } else {
            accel->sensortime = 0;
        }
    } else {
        rslt = BMI160_E_COM_FAIL;
    }

    return rslt;
}

/*!
 * @brief This API configures the 0x4C register and settings like
 * Auxiliary sensor manual enable/ disable and aux burst read length.
 */
int8_t bmi160_config_aux_mode(const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t aux_if[2] = {(uint8_t)(dev->aux_cfg.aux_i2c_addr * 2), 0};

    rslt = bmi160_get_regs(BMI160_AUX_IF_1_ADDR, &aux_if[1], 1, dev);
    if (rslt == BMI160_OK) {
        /* update the Auxiliary interface to manual/auto mode */
        aux_if[1] = (uint8_t)BMI160_SET_BITS(aux_if[1], BMI160_MANUAL_MODE_EN, dev->aux_cfg.manual_enable);
        /* update the burst read length defined by user */
        aux_if[1] = (uint8_t)BMI160_SET_BITS_POS_0(aux_if[1], BMI160_AUX_READ_BURST, dev->aux_cfg.aux_rd_burst_len);
        /* Set the secondary interface address and manual mode
         * along with burst read length */
        rslt = bmi160_set_regs(BMI160_AUX_IF_0_ADDR, &aux_if[0], 2, dev);
        dev->delay_ms(BMI160_AUX_COM_DELAY);
    }

    return rslt;
}

/*!
 * @brief This API configure the secondary interface.
 */
static int8_t config_sec_if(const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t if_conf = 0;
    uint8_t cmd = BMI160_AUX_NORMAL_MODE;

    /* set the aux power mode to normal*/
    rslt = bmi160_set_regs(BMI160_COMMAND_REG_ADDR, &cmd, 1, dev);
    if (rslt == BMI160_OK) {
        dev->delay_ms(60);
        rslt = bmi160_get_regs(BMI160_IF_CONF_ADDR, &if_conf, 1, dev);
        if_conf |= (uint8_t)(1 << 5);
        if (rslt == BMI160_OK)
            /*enable the secondary interface also*/
            rslt = bmi160_set_regs(BMI160_IF_CONF_ADDR, &if_conf, 1, dev);
    }

    return rslt;
}

/*!
 * @brief This API configure the settings of auxiliary sensor.
 */
static int8_t config_aux_settg(const struct bmi160_dev *dev)
{
    int8_t rslt;

    rslt = config_sec_if(dev);
    if (rslt == BMI160_OK) {
        /* Configures the auxiliary interface settings */
        rslt = bmi160_config_aux_mode(dev);
    }

    return rslt;
}

/*!
 * @brief This API extract the read data from auxiliary sensor.
 */
static int8_t extract_aux_read(uint16_t map_len, uint8_t reg_addr, uint8_t *aux_data, uint16_t len,
                                            const struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;
    uint8_t data[8] = {0,};
    uint8_t read_addr = BMI160_AUX_DATA_ADDR;
    uint8_t count = 0;
    uint8_t read_count;
    uint8_t read_len = (uint8_t)map_len;

    for (; count < len;) {
        /* set address to read */
        rslt = bmi160_set_regs(BMI160_AUX_IF_2_ADDR, &reg_addr, 1, dev);
        dev->delay_ms(BMI160_AUX_COM_DELAY);
        if (rslt == BMI160_OK) {
            rslt = bmi160_get_regs(read_addr, data, map_len, dev);
            if (rslt == BMI160_OK) {
                read_count = 0;
                /* if read len is less the burst read len
                 * mention by user*/
                if (len < map_len) {
                    read_len = (uint8_t)len;
                } else {
                    if ((len - count) < map_len)
                        read_len = (uint8_t)(len - count);
                }

                for (; read_count < read_len; read_count++)
                    aux_data[count + read_count] = data[read_count];

                reg_addr = (uint8_t)(reg_addr + map_len);
                count = (uint8_t)(count + map_len);
            } else {
                rslt = BMI160_E_COM_FAIL;
                break;
            }
        }
    }

    return rslt;
}

/*!
 * @brief This API is used to read the raw uncompensated auxiliary sensor
 * data of 8 bytes from BMI160 register address 0x04 to 0x0B
 */
int8_t bmi160_read_aux_data_auto_mode(uint8_t *aux_data, const struct bmi160_dev *dev)
{
    int8_t rslt;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);

    if (rslt != BMI160_OK) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        if ((dev->aux_cfg.aux_sensor_enable == BMI160_ENABLE) &&
            (dev->aux_cfg.manual_enable == BMI160_DISABLE)) {
            /* Read the aux. sensor's raw data */
            rslt = bmi160_get_regs(BMI160_AUX_DATA_ADDR, aux_data, 8, dev);
        } else {
            rslt = BMI160_E_INVALID_INPUT;
        }
    }

    return rslt;
}

/*!
 * @brief This API configure the ODR of the auxiliary sensor.
 */
static int8_t config_aux_odr(const struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t aux_odr;

    rslt = bmi160_get_regs(BMI160_AUX_ODR_ADDR, &aux_odr, 1, dev);
    if (rslt == BMI160_OK) {
        aux_odr = (uint8_t)(dev->aux_cfg.aux_odr);
        /* Set the secondary interface ODR
           i.e polling rate of secondary sensor */
        rslt = bmi160_set_regs(BMI160_AUX_ODR_ADDR, &aux_odr, 1, dev);
        dev->delay_ms(BMI160_AUX_COM_DELAY);
    }

    return rslt;
}

/*!
 * @brief This API is used to setup the auxiliary sensor of bmi160 in auto mode
 * Thus enabling the auto update of 8 bytes of data from auxiliary sensor
 * to BMI160 register address 0x04 to 0x0B
 */
int8_t bmi160_set_aux_auto_mode(uint8_t *data_addr, struct bmi160_dev *dev)
{
    int8_t rslt;
    /* Null-pointer check */
    rslt = null_ptr_check(dev);

    if (rslt != BMI160_OK) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        if (dev->aux_cfg.aux_sensor_enable == BMI160_ENABLE) {
            /* Write the aux. address to read in 0x4D of BMI160*/
            rslt = bmi160_set_regs(BMI160_AUX_IF_2_ADDR, data_addr, 1, dev);
            dev->delay_ms(BMI160_AUX_COM_DELAY);
            if (rslt == BMI160_OK) {
                /* Configure the polling ODR for
                auxiliary sensor */
                    rslt = config_aux_odr(dev);

                if (rslt == BMI160_OK) {
                    /* Disable the aux. manual mode, i.e aux.
                     * sensor is in auto-mode (data-mode) */
                    dev->aux_cfg.manual_enable = BMI160_DISABLE;
                    rslt = bmi160_config_aux_mode(dev);

                    /*  Auxiliary sensor data is obtained
                     * in auto mode from this point */
                }
            }
        } else {
            rslt = BMI160_E_INVALID_INPUT;
        }
    }

    return rslt;
}

/*!
 * @brief This API maps the actual burst read length set by user.
 */
static int8_t map_read_len(uint16_t *len, const struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;

    switch (dev->aux_cfg.aux_rd_burst_len) {

    case BMI160_AUX_READ_LEN_0:
        *len = 1;
        break;
    case BMI160_AUX_READ_LEN_1:
        *len = 2;
        break;
    case BMI160_AUX_READ_LEN_2:
        *len = 6;
        break;
    case BMI160_AUX_READ_LEN_3:
        *len = 8;
        break;
    default:
        rslt = BMI160_E_INVALID_INPUT;
        break;
    }

    return rslt;
}


/*!
 * @brief This API reads the mention no of byte of data from the given
 * register address of auxiliary sensor.
 */
int8_t bmi160_aux_read(uint8_t reg_addr, uint8_t *aux_data, uint16_t len, const struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;
    uint16_t map_len = 0;

    /* Null-pointer check */
    if ((dev == NULL) || (dev->read == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        if (dev->aux_cfg.aux_sensor_enable == BMI160_ENABLE) {
            rslt = map_read_len(&map_len, dev);
            if (rslt == BMI160_OK)
                rslt = extract_aux_read(map_len, reg_addr, aux_data, len, dev);
        } else {
            rslt = BMI160_E_INVALID_INPUT;
        }
    }

    return rslt;
}

/*!
 * @brief This API writes the mention no of byte of data to the given
 * register address of auxiliary sensor.
 */
int8_t bmi160_aux_write(uint8_t reg_addr, uint8_t *aux_data, uint16_t len, const struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;
    uint8_t count = 0;

    /* Null-pointer check */
    if ((dev == NULL) || (dev->write == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        for (; count < len; count++) {
            /* set data to write */
            rslt = bmi160_set_regs(BMI160_AUX_IF_4_ADDR, aux_data, 1, dev);
            dev->delay_ms(BMI160_AUX_COM_DELAY);
            if (rslt == BMI160_OK) {
                /* set address to write */
                rslt = bmi160_set_regs(BMI160_AUX_IF_3_ADDR, &reg_addr, 1, dev);
                dev->delay_ms(BMI160_AUX_COM_DELAY);
                if (rslt == BMI160_OK && (count < len - 1)) {
                    aux_data++;
                    reg_addr++;
                }
            }
        }
    }

    return rslt;
}

/*!
 * @brief This API initialize the auxiliary sensor
 * in order to access it.
 */
int8_t bmi160_aux_init(const struct bmi160_dev *dev)
{
    int8_t rslt;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);

    if (rslt != BMI160_OK) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        if (dev->aux_cfg.aux_sensor_enable == BMI160_ENABLE) {
            /* Configures the auxiliary sensor interface settings */
            rslt = config_aux_settg(dev);
        } else {
            rslt = BMI160_E_INVALID_INPUT;
        }
    }

    return rslt;
}

int8_t bmi160_init(struct bmi160_dev *dev)
{
    int8_t rslt;
    uint8_t chip_id;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);

    if (rslt == BMI160_OK) {
        /* Read chip_id */
        rslt = bmi160_get_regs(BMI160_CHIP_ID_ADDR, &chip_id, 1, dev);

        if ((rslt == BMI160_OK) && (chip_id == BMI160_CHIP_ID)) {
            dev->chip_id = chip_id;

            /* Soft reset */
            rslt = bmi160_soft_reset(dev);

        } else {
            rslt = BMI160_E_DEV_NOT_FOUND;
        }
    }
    return rslt;
}

/*!
 * @brief This API configures the power mode, range and bandwidth
 * of sensor.
 */
int8_t bmi160_set_sens_conf(struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;

    /* Null-pointer check */
    if ((dev == NULL) || (dev->delay_ms == NULL)) {
        rslt = BMI160_E_NULL_PTR;
    } else {
        /* write power mode for accel and gyro */
        rslt = bmi160_set_power_mode(dev);
        if (rslt == BMI160_OK) {
            rslt = check_invalid_settg(dev);
            if (rslt == BMI160_OK)
                rslt = set_accel_conf(dev);
            if (rslt == BMI160_OK) {
                rslt = set_gyro_conf(dev);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This API reads sensor data, stores it in
 * the bmi160_sensor_data structure pointer passed by the user.
 */
int8_t bmi160_get_sensor_data(uint8_t select_sensor, bmi160_data *accel_data, bmi160_data *gyro_data,
                const struct bmi160_dev *dev)
{
    int8_t rslt = BMI160_OK;
    uint8_t time_sel;
    uint8_t sen_sel;
    uint8_t len = 0;
    struct bmi160_sensor_data accel;
    struct bmi160_sensor_data gyro;

    /* use the following scaling depending upon accelerometer range
     *   2G  = 0.00061
     *   4G  = 0.00122
     *   8G  = 0.00244
     *   16G = 0.00488
     */
    double accel_scaling = 0.0;
    double gyro_scaling = 0.0;

    /*Extract the sensor  and time select information*/
    sen_sel = select_sensor & BMI160_SEN_SEL_MASK;
    time_sel = (uint8_t)((sen_sel & BMI160_TIME_SEL) >> 2);
    sen_sel = sen_sel & (BMI160_ACCEL_SEL | BMI160_GYRO_SEL);
    if (time_sel == 1)
        len = 3;

    /* Null-pointer check */
    if (dev != NULL) {
        switch (sen_sel) {
        case BMI160_ACCEL_ONLY:
            rslt = get_accel_data(len, &accel, dev);
            if(rslt == BMI160_OK)
            {
                if(dev->accel_cfg.range == BMI160_ACCEL_RANGE_2G)
                    accel_scaling = 0.00061;
                else if(dev->accel_cfg.range == BMI160_ACCEL_RANGE_4G)
                    accel_scaling = 0.00122;
                else if(dev->accel_cfg.range == BMI160_ACCEL_RANGE_8G)
                    accel_scaling = 0.00244;
                else
                    accel_scaling = 0.00488;

                accel_data->x_axis = (accel.x * accel_scaling);
                accel_data->y_axis = (accel.y * accel_scaling);
                accel_data->z_axis = (accel.z * accel_scaling);
            }
            break;
        case BMI160_GYRO_ONLY:
            rslt = get_gyro_data(len, &gyro, dev);
            if(rslt == BMI160_OK)
            {
                if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_2000_DPS)
                    gyro_scaling = 0.06;
                else if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_1000_DPS)
                    gyro_scaling = 0.023;
                else if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_500_DPS)
                    gyro_scaling = 0.014;
                else if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_250_DPS)
                    gyro_scaling = 0.00734;
                else
                    gyro_scaling = 0.00369;

                gyro_data->x_axis = (gyro.x * gyro_scaling);
                gyro_data->y_axis = (gyro.y * gyro_scaling);
                gyro_data->z_axis = (gyro.z * gyro_scaling);
            }
            break;
        case BMI160_BOTH_ACCEL_AND_GYRO:
            rslt = get_accel_gyro_data(len, &accel, &gyro, dev);
            if(rslt == BMI160_OK)
            {
                if(dev->accel_cfg.range == BMI160_ACCEL_RANGE_2G)
                    accel_scaling = 0.00061;
                else if(dev->accel_cfg.range == BMI160_ACCEL_RANGE_4G)
                    accel_scaling = 0.00122;
                else if(dev->accel_cfg.range == BMI160_ACCEL_RANGE_8G)
                    accel_scaling = 0.00244;
                else
                    accel_scaling = 0.00488;

                accel_data->x_axis = (accel.x * accel_scaling);
                accel_data->y_axis = (accel.y * accel_scaling);
                accel_data->z_axis = (accel.z * accel_scaling);

                if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_2000_DPS)
                    gyro_scaling = 0.06;
                else if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_1000_DPS)
                    gyro_scaling = 0.023;
                else if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_500_DPS)
                    gyro_scaling = 0.014;
                else if(dev->gyro_cfg.range == BMI160_GYRO_RANGE_250_DPS)
                    gyro_scaling = 0.00734;
                else
                    gyro_scaling = 0.00369;

                gyro_data->x_axis = (gyro.x * gyro_scaling);
                gyro_data->y_axis = (gyro.y * gyro_scaling);
                gyro_data->z_axis = (gyro.z * gyro_scaling);
            }
            break;
        default:
            rslt = BMI160_E_INVALID_INPUT;
            break;
        }
    } else {
        rslt = BMI160_E_NULL_PTR;
    }

    return rslt;

}
