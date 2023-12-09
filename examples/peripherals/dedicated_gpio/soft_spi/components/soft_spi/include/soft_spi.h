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
 * @brief Structure defining the configuration for the software SPI bus
 */
typedef struct {
    uint32_t clk_pin;
    uint32_t mosi_pin;
    uint32_t miso_pin;
    uint32_t cs_pin;
} soft_spi_config_t;

/**
 * @brief Abstract type representing a software SPI bus.
 */
typedef struct soft_spi_bus_impl_t* soft_spi_bus_t;

/**
 * @brief Create and configure the software SPI bus.
 *
 * @param config Configuration to apply to the initialized bus.
 * @param bus Output structure representing the freshly initialized software SPI bus.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_spi_new(soft_spi_config_t *config, soft_spi_bus_t *bus);

/**
 * @brief Delete a previously initialized software SPI bus.
 *
 * @param bus Bus to delete, must have been initialized with `soft_spi_new` first.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_spi_del(soft_spi_bus_t bus);

/**
 * @brief Send the given bytes on the software SPI bus.
 *
 * @param bus Software SPI bus to send data on.
 * @param write_buffer Buffer containing the bytes to send on the buffer. Must not be NULL.
 * @param read_buffer Buffer to store the received bytes in. Can be NULL.
 * @param buf_size Size of the given buffers. Must not be 0.
 *
 * @return ESP_OK on success
 */
esp_err_t soft_spi_transfer(soft_spi_bus_t bus,
                            const uint8_t* write_buffer, uint8_t* read_buffer,
                            size_t buf_size);

#ifdef __cplusplus
}
#endif
