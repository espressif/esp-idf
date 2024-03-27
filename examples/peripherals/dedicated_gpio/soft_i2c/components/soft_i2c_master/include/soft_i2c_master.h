/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration for the usable frequencies by the software I2C master bus
 */
typedef enum {
    SOFT_I2C_100KHZ,
    SOFT_I2C_200KHZ,
    SOFT_I2C_300KHZ,
    SOFT_I2C_FREQ_END
} soft_i2c_master_freq_t;

/**
 * @brief Structure defining the configuration for the software I2C master bus
 */
typedef struct {
    uint32_t scl_pin;
    uint32_t sda_pin;
    soft_i2c_master_freq_t freq;
} soft_i2c_master_config_t;

/**
 * @brief Abstract type representing a software I2C bus.
 */
typedef struct i2c_master_bus_impl_t* soft_i2c_master_bus_t;

/**
 * @brief Create and configure the software I2C bus.
 *
 * @param config Configuration to apply to the initialized bus.
 * @param bus Output structure representing the freshly initialized software I2C bus.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_i2c_master_new(soft_i2c_master_config_t *config, soft_i2c_master_bus_t *bus);

/**
 * @brief Delete a previously initialized I2C software bus.
 *
 * @param bus Bus to delete, must have been initialized with `soft_i2c_master_new` first.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_i2c_master_del(soft_i2c_master_bus_t bus);

/**
 * @brief Perform a write to the given device on the software I2C bus.
 *
 * @param bus Software I2C bus to perform the transfer on.
 * @param device_address 7-bit device address to communicate with.
 * @param write_buffer Buffer containing the bytes to send on the buffer. Must not be NULL.
 * @param write_size Size of the write buffer. Must not be 0.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_i2c_master_write(soft_i2c_master_bus_t bus,
                                uint8_t device_address,
                                const uint8_t* write_buffer, size_t write_size);

/**
 * @brief Perform a read from the given device on the software I2C bus.
 *
 * @param bus Software I2C bus to perform the transfer on.
 * @param device_address 7-bit device address to communicate with.
 * @param read_buffer Buffer that will contain the bytes received. Must not be NULL.
 * @param read_size Size of the read buffer. Must not be 0.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_i2c_master_read(soft_i2c_master_bus_t bus,
                               uint8_t device_address,
                               uint8_t* read_buffer, size_t read_size);

/**
 * @brief Perform a write followed by a read to the given device on the software I2C bus.
 *
 * @param bus Software I2C bus to perform the transfer on.
 * @param device_address 7-bit device address to communicate with.
 * @param read_buffer Buffer that will contain the bytes received. Must not be NULL.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_i2c_master_write_read(soft_i2c_master_bus_t bus,
                                     uint8_t device_address,
                                     const uint8_t* write_buffer, size_t write_size,
                                     uint8_t* read_buffer, size_t read_size);

#ifdef __cplusplus
}
#endif
