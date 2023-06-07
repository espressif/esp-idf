/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal/i2c_types.h"
#include "hal/gpio_types.h"
#include "esp_err.h"

/**
 * @brief LP Core I2C pin config parameters
 */
typedef struct {
    gpio_num_t sda_io_num;      /*!< GPIO pin for SDA signal. Only GPIO#6 can be used as the SDA pin. */
    gpio_num_t scl_io_num;      /*!< GPIO pin for SCL signal. Only GPIO#7 can be used as the SCL pin. */
    bool sda_pullup_en;         /*!< SDA line enable internal pullup. Can be configured if external pullup is not used. */
    bool scl_pullup_en;         /*!< SCL line enable internal pullup. Can be configured if external pullup is not used. */
} lp_core_i2c_pin_cfg_t;


/**
 * @brief LP Core I2C timing config parameters
 */
typedef struct {
    uint32_t clk_speed_hz;      /*!< LP I2C clock speed for master mode */
} lp_core_i2c_timing_cfg_t;


/**
 * @brief LP Core I2C config parameters
 */
typedef struct {
    lp_core_i2c_pin_cfg_t i2c_pin_cfg;        /*!< LP I2C pin configuration */
    lp_core_i2c_timing_cfg_t i2c_timing_cfg;  /*!< LP I2C timing configuration */
    soc_periph_lp_i2c_clk_src_t i2c_src_clk;  /*!< LP I2C source clock type */
} lp_core_i2c_cfg_t;

/* Default LP I2C GPIO settings */
#define LP_I2C_DEFAULT_GPIO_CONFIG()            \
        .i2c_pin_cfg.sda_io_num = GPIO_NUM_6,   \
        .i2c_pin_cfg.scl_io_num = GPIO_NUM_7,   \
        .i2c_pin_cfg.sda_pullup_en = true,      \
        .i2c_pin_cfg.scl_pullup_en = true,      \

/* LP I2C fast mode config. Max SCL freq of 400 KHz. */
#define LP_I2C_FAST_MODE_CONFIG()               \
        .i2c_timing_cfg.clk_speed_hz = 400000,  \

/* LP I2C standard mode config. Max SCL freq of 100 KHz. */
#define LP_I2C_STANDARD_MODE_CONFIG()           \
        .i2c_timing_cfg.clk_speed_hz = 100000,  \

#define LP_I2C_DEFAULT_SRC_CLK()                \
        .i2c_src_clk = LP_I2C_SCLK_LP_FAST,     \

/* Default LP I2C GPIO settings and timing parametes */
#define LP_CORE_I2C_DEFAULT_CONFIG()            \
    {                                           \
        LP_I2C_DEFAULT_GPIO_CONFIG()            \
        LP_I2C_FAST_MODE_CONFIG()               \
        LP_I2C_DEFAULT_SRC_CLK()                \
    }

/**
 * @brief Initialize and configure the LP I2C for use by the LP core
 * Currently LP I2C can only be used in master mode
 *
 * @param lp_i2c_num    LP I2C port number
 * @param cfg           Configuration parameters
 * @return esp_err_t    ESP_OK when successful
 *
 * @note The internal pull-up resistors for SDA and SCL pins, if enabled, will
 * provide a weak pull-up value of about 30-50 kOhm. Users are adviced to enable
 * external pull-ups for better performance at higher SCL frequencies.
 */
esp_err_t lp_core_i2c_master_init(i2c_port_t lp_i2c_num, const lp_core_i2c_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
