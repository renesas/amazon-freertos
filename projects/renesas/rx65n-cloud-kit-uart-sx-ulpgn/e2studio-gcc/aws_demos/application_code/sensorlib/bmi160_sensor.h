/*
 * bmi160_sensor.h
 *
 *  Created on: Dec 4, 2017
 *      Author: Rajkumar.Thiagarajan
 */

#ifndef BMI160_SENSOR_H_
#define BMI160_SENSOR_H_

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "platform.h"

/* BIT SLICE GET AND SET FUNCTIONS */
#define BMI160_GET_BITS(regvar, bitname)\
        ((regvar & bitname##_MSK) >> bitname##_POS)
#define BMI160_SET_BITS(regvar, bitname, val)\
        ((regvar & ~bitname##_MSK) | \
        ((val<<bitname##_POS)&bitname##_MSK))

#define BMI160_SET_BITS_POS_0(reg_data, bitname, data) \
                ((reg_data & ~(bitname##_MSK)) | \
                (data & bitname##_MSK))

#define BMI160_GET_BITS_POS_0(reg_data, bitname)  (reg_data & (bitname##_MSK))


#define BMI160_MANUAL_MODE_EN_POS   (7)
#define BMI160_MANUAL_MODE_EN_MSK   (0x80)
#define BMI160_AUX_READ_BURST       (0)
#define BMI160_AUX_READ_BURST_MSK   (0x03)

/* BMI160 I2C address */
#define BMI160_I2C_ADDR                 (0x68)
/* BMI160 secondary IF address */
#define BMI160_AUX_BMM150_I2C_ADDR      (0x10)

/** Enable/disable bit value */
#define BMI160_ENABLE                   (0x01)
#define BMI160_DISABLE                  (0x00)

/** Interface settings */
#define BMI160_SPI_INTF                  (1)
#define BMI160_I2C_INTF                  (0)

/** Error code definitions */
#define BMI160_OK                         (0)
#define BMI160_E_NULL_PTR                 (-1)
#define BMI160_E_COM_FAIL                 (-2)
#define BMI160_E_DEV_NOT_FOUND            (-3)
#define BMI160_E_OUT_OF_RANGE             (-4)
#define BMI160_E_INVALID_INPUT            (-5)
#define BMI160_E_ACCEL_ODR_BW_INVALID     (-6)
#define BMI160_E_GYRO_ODR_BW_INVALID      (-7)
#define BMI160_E_LWP_PRE_FLTR_INT_INVALID (-8)
#define BMI160_E_LWP_PRE_FLTR_INVALID     (-9)
#define BMI160_E_AUX_NOT_FOUND            (-10)
#define BMI160_FOC_FAILURE                (-11)

/** BMI160 Register map */
#define BMI160_CHIP_ID_ADDR         (0x00)
#define BMI160_ERROR_REG_ADDR       (0x02)
#define BMI160_PMU_STATUS_REG_ADDR  (0x03)
#define BMI160_AUX_DATA_ADDR        (0x04)
#define BMI160_GYRO_DATA_ADDR       (0x0C)
#define BMI160_ACCEL_DATA_ADDR      (0x12)
#define BMI160_STATUS_ADDR          (0x1B)
#define BMI160_INT_STATUS_ADDR      (0x1C)
#define BMI160_FIFO_LENGTH_ADDR     (0x22)
#define BMI160_FIFO_DATA_ADDR       (0x24)
#define BMI160_ACCEL_CONFIG_ADDR    (0x40)
#define BMI160_ACCEL_RANGE_ADDR     (0x41)
#define BMI160_GYRO_CONFIG_ADDR     (0x42)
#define BMI160_GYRO_RANGE_ADDR      (0x43)
#define BMI160_AUX_ODR_ADDR         (0x44)
#define BMI160_FIFO_DOWN_ADDR       (0x45)
#define BMI160_FIFO_CONFIG_0_ADDR   (0x46)
#define BMI160_FIFO_CONFIG_1_ADDR   (0x47)
#define BMI160_AUX_IF_0_ADDR        (0x4B)
#define BMI160_AUX_IF_1_ADDR        (0x4C)
#define BMI160_AUX_IF_2_ADDR        (0x4D)
#define BMI160_AUX_IF_3_ADDR        (0x4E)
#define BMI160_AUX_IF_4_ADDR        (0x4F)
#define BMI160_INT_ENABLE_0_ADDR    (0x50)
#define BMI160_INT_ENABLE_1_ADDR    (0x51)
#define BMI160_INT_ENABLE_2_ADDR    (0x52)
#define BMI160_INT_OUT_CTRL_ADDR    (0x53)
#define BMI160_INT_LATCH_ADDR       (0x54)
#define BMI160_INT_MAP_0_ADDR       (0x55)
#define BMI160_INT_MAP_1_ADDR       (0x56)
#define BMI160_INT_MAP_2_ADDR       (0x57)
#define BMI160_INT_DATA_0_ADDR      (0x58)
#define BMI160_INT_DATA_1_ADDR      (0x59)
#define BMI160_INT_LOWHIGH_0_ADDR   (0x5A)
#define BMI160_INT_LOWHIGH_1_ADDR   (0x5B)
#define BMI160_INT_LOWHIGH_2_ADDR   (0x5C)
#define BMI160_INT_LOWHIGH_3_ADDR   (0x5D)
#define BMI160_INT_LOWHIGH_4_ADDR   (0x5E)
#define BMI160_INT_MOTION_0_ADDR    (0x5F)
#define BMI160_INT_MOTION_1_ADDR    (0x60)
#define BMI160_INT_MOTION_2_ADDR    (0x61)
#define BMI160_INT_MOTION_3_ADDR    (0x62)
#define BMI160_INT_TAP_0_ADDR       (0x63)
#define BMI160_INT_TAP_1_ADDR       (0x64)
#define BMI160_INT_ORIENT_0_ADDR    (0x65)
#define BMI160_INT_ORIENT_1_ADDR    (0x66)
#define BMI160_INT_FLAT_0_ADDR      (0x67)
#define BMI160_INT_FLAT_1_ADDR      (0x68)
#define BMI160_FOC_CONF_ADDR        (0x69)
#define BMI160_CONF_ADDR            (0x6A)
#define BMI160_IF_CONF_ADDR         (0x6B)
#define BMI160_COMMAND_REG_ADDR     (0x7E)

/** BMI160 unique chip identifier */
#define BMI160_CHIP_ID                   (0xD1)

/** Soft reset command */
#define BMI160_SOFT_RESET_CMD            (0xb6)
#define BMI160_SOFT_RESET_DELAY_MS       (15)

/** Power mode settings */
/* Accel power mode */
#define BMI160_ACCEL_NORMAL_MODE         (0x11)
#define BMI160_ACCEL_LOWPOWER_MODE       (0x12)
#define BMI160_ACCEL_SUSPEND_MODE        (0x10)

/* Gyro power mode */
#define BMI160_GYRO_SUSPEND_MODE         (0x14)
#define BMI160_GYRO_NORMAL_MODE          (0x15)
#define BMI160_GYRO_FASTSTARTUP_MODE     (0x17)

/* Aux power mode */
#define BMI160_AUX_SUSPEND_MODE     (0x18)
#define BMI160_AUX_NORMAL_MODE      (0x19)
#define BMI160_AUX_LOWPOWER_MODE    (0x1A)

/** Range settings */
/* Accel Range */
#define BMI160_ACCEL_RANGE_2G            (0x03)
#define BMI160_ACCEL_RANGE_4G            (0x05)
#define BMI160_ACCEL_RANGE_8G            (0x08)
#define BMI160_ACCEL_RANGE_16G           (0x0C)

/* Gyro Range */
#define BMI160_GYRO_RANGE_2000_DPS       (0x00)
#define BMI160_GYRO_RANGE_1000_DPS       (0x01)
#define BMI160_GYRO_RANGE_500_DPS        (0x02)
#define BMI160_GYRO_RANGE_250_DPS        (0x03)
#define BMI160_GYRO_RANGE_125_DPS        (0x04)

/** Bandwidth settings */
/* Accel Bandwidth */
#define BMI160_ACCEL_BW_OSR4_AVG1        (0x00)
#define BMI160_ACCEL_BW_OSR2_AVG2        (0x01)
#define BMI160_ACCEL_BW_NORMAL_AVG4      (0x02)
#define BMI160_ACCEL_BW_RES_AVG8         (0x03)
#define BMI160_ACCEL_BW_RES_AVG16        (0x04)
#define BMI160_ACCEL_BW_RES_AVG32        (0x05)
#define BMI160_ACCEL_BW_RES_AVG64        (0x06)
#define BMI160_ACCEL_BW_RES_AVG128       (0x07)

#define BMI160_GYRO_BW_OSR4_MODE         (0x00)
#define BMI160_GYRO_BW_OSR2_MODE         (0x01)
#define BMI160_GYRO_BW_NORMAL_MODE       (0x02)

/* Output Data Rate settings */
/* Accel Output data rate */
#define BMI160_ACCEL_ODR_RESERVED        (0x00)
#define BMI160_ACCEL_ODR_0_78HZ          (0x01)
#define BMI160_ACCEL_ODR_1_56HZ          (0x02)
#define BMI160_ACCEL_ODR_3_12HZ          (0x03)
#define BMI160_ACCEL_ODR_6_25HZ          (0x04)
#define BMI160_ACCEL_ODR_12_5HZ          (0x05)
#define BMI160_ACCEL_ODR_25HZ            (0x06)
#define BMI160_ACCEL_ODR_50HZ            (0x07)
#define BMI160_ACCEL_ODR_100HZ           (0x08)
#define BMI160_ACCEL_ODR_200HZ           (0x09)
#define BMI160_ACCEL_ODR_400HZ           (0x0A)
#define BMI160_ACCEL_ODR_800HZ           (0x0B)
#define BMI160_ACCEL_ODR_1600HZ          (0x0C)
#define BMI160_ACCEL_ODR_RESERVED0       (0x0D)
#define BMI160_ACCEL_ODR_RESERVED1       (0x0E)
#define BMI160_ACCEL_ODR_RESERVED2       (0x0F)

/* Gyro Output data rate */
#define BMI160_GYRO_ODR_RESERVED         (0x00)
#define BMI160_GYRO_ODR_25HZ             (0x06)
#define BMI160_GYRO_ODR_50HZ             (0x07)
#define BMI160_GYRO_ODR_100HZ            (0x08)
#define BMI160_GYRO_ODR_200HZ            (0x09)
#define BMI160_GYRO_ODR_400HZ            (0x0A)
#define BMI160_GYRO_ODR_800HZ            (0x0B)
#define BMI160_GYRO_ODR_1600HZ           (0x0C)
#define BMI160_GYRO_ODR_3200HZ           (0x0D)

/* Maximum limits definition */
#define BMI160_ACCEL_ODR_MAX             (15)
#define BMI160_ACCEL_BW_MAX              (2)
#define BMI160_ACCEL_RANGE_MAX           (12)
#define BMI160_GYRO_ODR_MAX              (13)
#define BMI160_GYRO_BW_MAX               (2)
#define BMI160_GYRO_RANGE_MAX            (4)

/** Mask definitions */
#define BMI160_ACCEL_BW_MASK                    (0x70)
#define BMI160_ACCEL_ODR_MASK                   (0x0F)
#define BMI160_ACCEL_UNDERSAMPLING_MASK         (0x80)
#define BMI160_ACCEL_RANGE_MASK                 (0x0F)
#define BMI160_GYRO_BW_MASK                     (0x30)
#define BMI160_GYRO_ODR_MASK                    (0x0F)
#define BMI160_GYRO_RANGE_MSK                   (0x07)

#define BMI160_ONE_MS_DELAY              (1)
#define BMI160_ACCEL_DELAY_MS            (5)
#define BMI160_GYRO_DELAY_MS             (81)
#define BMI160_AUX_COM_DELAY             (10)
/* Error code mask */
#define BMI160_ERR_REG_MASK             (0x0F)

/* Sensor select mask*/
#define BMI160_SEN_SEL_MASK             (0x07)

/* Sensor & time select definition*/
#define BMI160_ACCEL_SEL                (0x01)
#define BMI160_GYRO_SEL                 (0x02)
#define BMI160_TIME_SEL                 (0x04)

/*****************************************************************************/
/* type definitions */
typedef int8_t (*bmi160_com_fptr_t)(uint8_t dev_addr, uint8_t reg_addr,
        uint8_t *data, uint16_t len);

typedef void (*bmi160_delay_fptr_t)(uint32_t period);

/*!
 * @brief Aux sensor configuration structure
 */
struct bmi160_aux_cfg {
    /*! Aux sensor, 1 - enable 0 - disable */
    uint8_t aux_sensor_enable : 1;
    /*! Aux manual/auto mode status */
    uint8_t manual_enable : 1;
    /*! Aux read burst length */
    uint8_t aux_rd_burst_len : 2;
    /*! output data rate */
    uint8_t aux_odr :4;
    /*! i2c addr of auxiliary sensor */
    uint8_t aux_i2c_addr;
};

/*!
 * @brief bmi160 sensor data structure which comprises of accel data
 */
struct bmi160_sensor_data {
    /*! X-axis sensor data */
    int16_t x;
    /*! Y-axis sensor data */
    int16_t y;
    /*! Z-axis sensor data */
    int16_t z;
    /*! sensor time */
    uint32_t sensortime;
};

typedef struct bmi160_sensor_data_t {
    /*! X-axis sensor data */
    double x_axis;
    /*! Y-axis sensor data */
    double y_axis;
    /*! Z-axis sensor data */
    double z_axis;
    /*! sensor time */
    uint32_t sensortime;
}bmi160_data;

/*!
 * @brief bmi160 sensor configuration structure
 */
struct bmi160_cfg {
    /*! power mode */
    uint8_t power;
    /*! output data rate */
    uint8_t odr;
    /*! range */
    uint8_t range;
    /*! bandwidth */
    uint8_t bw;
};

/*!
 * @brief bmi160 sensor select structure
 */
enum bmi160_select_sensor {
    BMI160_ACCEL_ONLY = 1,
    BMI160_GYRO_ONLY,
    BMI160_BOTH_ACCEL_AND_GYRO
};

/*!
 * @brief enum for auxiliary burst read selection
 */
enum bm160_aux_read_len {
    BMI160_AUX_READ_LEN_0,
    BMI160_AUX_READ_LEN_1,
    BMI160_AUX_READ_LEN_2,
    BMI160_AUX_READ_LEN_3
};

struct bmi160_dev {
    /*! Chip Id */
    uint8_t chip_id;
    /*! Device Id */
    uint8_t id;
    /*! 0 - I2C , 1 - SPI Interface */
    uint8_t interface;
    /*! Structure to configure Accel sensor */
    struct bmi160_cfg accel_cfg;
    /*! Structure to hold previous/old accel config parameters.
     * This is used at driver level to prevent overwriting of same
     * data, hence user does not change it in the code */
    struct bmi160_cfg prev_accel_cfg;
    /*! Structure to hold previous/old gyro config parameters.
     * This is used at driver level to prevent overwriting of same
     * data, hence user does not change it in the code */
    struct bmi160_cfg prev_gyro_cfg;
    /*! Structure to configure the auxiliary sensor */
    struct bmi160_aux_cfg aux_cfg;
    /*! Structure to hold previous/old aux config parameters.
     * This is used at driver level to prevent overwriting of same
     * data, hence user does not change it in the code */
    struct bmi160_aux_cfg prev_aux_cfg;
    /*! Structure to configure Gyro sensor */
    struct bmi160_cfg gyro_cfg;
    /*! Read function pointer */
    bmi160_com_fptr_t read;
    /*! Write function pointer */
    bmi160_com_fptr_t write;
    /*!  Delay function pointer */
    bmi160_delay_fptr_t delay_ms;
};

/* Function Prototypes */
int8_t bmi160_init(struct bmi160_dev *dev);
int8_t bmi160_set_sens_conf(struct bmi160_dev *dev);
int8_t bmi160_set_power_mode(struct bmi160_dev *dev);
int8_t bmi160_soft_reset(struct bmi160_dev *dev);
int8_t bmi160_get_sensor_data(uint8_t select_sensor, bmi160_data *accel_data, bmi160_data *gyro_data,
                const struct bmi160_dev *dev);
int8_t bmi160_aux_write(uint8_t reg_addr, uint8_t *aux_data, uint16_t len, const struct bmi160_dev *dev);
int8_t bmi160_aux_read(uint8_t reg_addr, uint8_t *aux_data, uint16_t len, const struct bmi160_dev *dev);
int8_t bmi160_config_aux_mode(const struct bmi160_dev *dev);
int8_t bmi160_set_aux_auto_mode(uint8_t *data_addr, struct bmi160_dev *dev);
int8_t bmi160_read_aux_data_auto_mode(uint8_t *aux_data, const struct bmi160_dev *dev);
int8_t bmi160_aux_init(const struct bmi160_dev *dev);
#endif /* BMI160_SENSOR_H_ */
