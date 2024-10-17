#ifndef SHT21_H
#define SHT21_H
#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"

typedef enum
{
    sht21_i2c_speed_standard = 100000,
    sht21_i2c_speed_fast = 400000,
} sht21_i2c_speed_t;

/**
 * @brief Initialize the I2C peripheral.
 *
 * @param i2c_num I2C port to configure
 * @param sda_pin GPIO number for I2C sda signal
 * @param scl_pin GPIO number for I2C scl signal
 * @param i2c_speed I2C clock frequency for master mode
 *
 * @return esp_err_t
 */
esp_err_t sht21_init(i2c_port_t i2c_num, gpio_num_t sda_pin, gpio_num_t scl_pin,
                     sht21_i2c_speed_t i2c_speed);

/**
 * @brief Read the sensor and calculate the relative humidity [%RH].
 *
 * @param dst pointer to destination variable
 *
 * @return esp_err_t
 */
esp_err_t sht21_get_temperature(float *dst);

/**
 * @brief Read the sensor and calculate the temperature [°C].
 *
 * @param dst pointer to destination variable
 *
 * @return esp_err_t
 */
esp_err_t sht21_get_humidity(float *dst);

/**
 * @brief Perform a reset without switching the power off and on again.
 *
 * @return esp_err_t
 */
esp_err_t sht21_soft_reset(void);

/**
 * @brief Release the allocated I2C resources.
 *
 * @return esp_err_t
 */
esp_err_t sht21_deinit(void);

#endif