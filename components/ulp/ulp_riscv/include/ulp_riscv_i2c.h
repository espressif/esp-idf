/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal/gpio_types.h"
#include "esp_err.h"

/**
 * @brief ULP RISC-V RTC I2C pin config
 */
typedef struct {
    uint32_t sda_io_num;     /*!< GPIO pin for SDA signal. Only GPIO#1 or GPIO#3 can be used as the SDA pin. */
    uint32_t scl_io_num;     /*!< GPIO pin for SCL signal. Only GPIO#0 or GPIO#2 can be used as the SCL pin. */
    bool sda_pullup_en;      /*!< SDA line enable internal pullup. Can be configured if external pullup is not used. */
    bool scl_pullup_en;      /*!< SCL line enable internal pullup. Can be configured if external pullup is not used. */
} ulp_riscv_i2c_pin_cfg_t;

/**
 * @brief ULP RISC-V RTC I2C timing config
 */
typedef struct {
    float scl_low_period;    /*!< SCL low period in micro seconds */
    float scl_high_period;   /*!< SCL high period in micro seconds */
    float sda_duty_period;   /*!< Period between the SDA switch and the falling edge of SCL in micro seconds */
    float scl_start_period;  /*!< Waiting time after the START condition in micro seconds */
    float scl_stop_period;   /*!< Waiting time before the END condition in micro seconds */
    float i2c_trans_timeout; /*!< I2C transaction timeout in micro seconds */
} ulp_riscv_i2c_timing_cfg_t;

/**
 * @brief ULP RISC-V RTC I2C init parameters
 */
typedef struct {
    ulp_riscv_i2c_pin_cfg_t i2c_pin_cfg;        /*!< RTC I2C pin configuration */
    ulp_riscv_i2c_timing_cfg_t i2c_timing_cfg;  /*!< RTC I2C timing configuration */
} ulp_riscv_i2c_cfg_t;

/* Default RTC I2C GPIO settings */
#define ULP_RISCV_I2C_DEFAULT_GPIO_CONFIG()     \
        .i2c_pin_cfg.sda_io_num = GPIO_NUM_3,   \
        .i2c_pin_cfg.scl_io_num = GPIO_NUM_2,   \
        .i2c_pin_cfg.sda_pullup_en = true,      \
        .i2c_pin_cfg.scl_pullup_en = true,      \

#if CONFIG_IDF_TARGET_ESP32S3
/* Nominal I2C bus timing parameters for I2C fast mode. Max SCL freq of 400 KHz. */
#define ULP_RISCV_I2C_FAST_MODE_CONFIG()        \
        .i2c_timing_cfg.scl_low_period = 1.4,   \
        .i2c_timing_cfg.scl_high_period = 0.3,  \
        .i2c_timing_cfg.sda_duty_period = 1,    \
        .i2c_timing_cfg.scl_start_period = 2,   \
        .i2c_timing_cfg.scl_stop_period = 1.3,  \
        .i2c_timing_cfg.i2c_trans_timeout = 20,
#elif CONFIG_IDF_TARGET_ESP32S2
/* Nominal I2C bus timing parameters for I2C fast mode. Max SCL freq on S2 is about 233 KHz due to timing constraints. */
#define ULP_RISCV_I2C_FAST_MODE_CONFIG()        \
        .i2c_timing_cfg.scl_low_period = 2,     \
        .i2c_timing_cfg.scl_high_period = 0.7,  \
        .i2c_timing_cfg.sda_duty_period = 1.7,  \
        .i2c_timing_cfg.scl_start_period = 2.4, \
        .i2c_timing_cfg.scl_stop_period = 1.3,  \
        .i2c_timing_cfg.i2c_trans_timeout = 20,
#endif

/* Nominal I2C bus timing parameters for I2C standard mode. Max SCL freq of 100 KHz. */
#define ULP_RISCV_I2C_STANDARD_MODE_CONFIG()    \
        .i2c_timing_cfg.scl_low_period = 5,     \
        .i2c_timing_cfg.scl_high_period = 5,    \
        .i2c_timing_cfg.sda_duty_period = 2,    \
        .i2c_timing_cfg.scl_start_period = 3,   \
        .i2c_timing_cfg.scl_stop_period = 6,    \
        .i2c_timing_cfg.i2c_trans_timeout = 20, \

/* Default RTC I2C configuration settings. Uses I2C fast mode. */
//TODO: Move to smaller units of time in the future like nano seconds to avoid floating point operations.
#define ULP_RISCV_I2C_DEFAULT_CONFIG()          \
    {                                           \
        ULP_RISCV_I2C_DEFAULT_GPIO_CONFIG()     \
        ULP_RISCV_I2C_FAST_MODE_CONFIG()        \
    }

/**
 * @brief Set the I2C slave device address
 *
 * @param slave_addr    I2C slave address (7 bit)
 */
void ulp_riscv_i2c_master_set_slave_addr(uint8_t slave_addr);

/**
 * @brief Set the I2C slave device sub register address
 *
 * @note The RTC I2C peripheral always expects a slave sub register address to be programmed. If it is not, the I2C
 * peripheral uses the SENS_SAR_I2C_CTRL_REG[18:11] as the sub register address for the subsequent read or write
 * operation.
 *
 * @param slave_reg_addr    I2C slave sub register address
 */
void ulp_riscv_i2c_master_set_slave_reg_addr(uint8_t slave_reg_addr);

/**
 * @brief Read from I2C slave device
 *
 * @note The I2C slave device address must be configured at least once before invoking this API.
 *
 * @param data_rd       Buffer to hold data to be read
 * @param size          Size of data to be read in bytes
 */
void ulp_riscv_i2c_master_read_from_device(uint8_t *data_rd, size_t size);

/**
 * @brief Write to I2C slave device
 *
 * @note The I2C slave device address must be configured at least once before invoking this API.
 *
 * @param data_wr       Buffer which holds the data to be written
 * @param size          Size of data to be written in bytes
 */
void ulp_riscv_i2c_master_write_to_device(uint8_t *data_wr, size_t size);

/**
 * @brief Initialize and configure the RTC I2C for use by ULP RISC-V
 * Currently RTC I2C can only be used in master mode
 *
 * @param cfg           Configuration parameters
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_i2c_master_init(const ulp_riscv_i2c_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
