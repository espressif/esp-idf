/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
#define I2C_SLAVE_SCL_IO     GPIO_NUM_18    /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     GPIO_NUM_19    /*!<gpio number for i2c slave data */

#define I2C_MASTER_SCL_IO    GPIO_NUM_18     /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    GPIO_NUM_19     /*!< gpio number for I2C master data  */
#else
#define I2C_SLAVE_SCL_IO     GPIO_NUM_4    /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     GPIO_NUM_2    /*!<gpio number for i2c slave data */

#define I2C_MASTER_SCL_IO    GPIO_NUM_4     /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    GPIO_NUM_2     /*!< gpio number for I2C master data  */
#endif

#if SOC_LP_I2C_SUPPORTED
#if CONFIG_IDF_TARGET_ESP32P4
#define LP_I2C_SCL_IO   GPIO_NUM_4
#define LP_I2C_SDA_IO   GPIO_NUM_5
#elif CONFIG_IDF_TARGET_ESP32C5
#define LP_I2C_SCL_IO   GPIO_NUM_3
#define LP_I2C_SDA_IO   GPIO_NUM_2
#else
#define LP_I2C_SCL_IO   GPIO_NUM_7
#define LP_I2C_SDA_IO   GPIO_NUM_6
#endif
#endif

#define ESP_SLAVE_ADDR 0x28         /*!< ESP_I2C slave address, you can set any 7bit value */
#define TEST_I2C_PORT     0
#define DATA_LENGTH     100

/**
 * @brief Slave test event
 */
typedef enum {
    I2C_SLAVE_EVT_RX,
    I2C_SLAVE_EVT_TX
} i2c_slave_event_t;

/**
 * @brief Display buffer
 *
 * @param buf Pointer to the buffer
 * @param len buffer length
 */
void disp_buf(uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif
