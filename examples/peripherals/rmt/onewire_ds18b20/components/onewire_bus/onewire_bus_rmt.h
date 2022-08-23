/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"

/**
 * @brief 1-wire bus config
 *
 */
typedef struct {
    gpio_num_t gpio_pin; /*!< gpio used for 1-wire bus */
    uint8_t max_rx_bytes; /*!< should be larger than the largest possible single receive size */
} onewire_rmt_config_t;

/**
 * @brief Type of 1-wire bus handle
 *
 */
typedef struct onewire_bus_t *onewire_bus_handle_t;

/**
 * @brief Install new 1-wire bus
 *
 * @param[in] config 1-wire bus configurations
 * @param[out] handle_out Installed new 1-wire bus' handle
 * @return
 *         - ESP_OK                1-wire bus is installed successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 *         - ESP_ERR_NO_MEM        Memory allocation failed.
 */
esp_err_t onewire_new_bus_rmt(onewire_rmt_config_t *config, onewire_bus_handle_t *handle_out);

/**
 * @brief Delete existing 1-wire bus
 *
 * @param[in] handle 1-wire bus handle to be deleted
 * @return
 *         - ESP_OK                1-wire bus is deleted successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_del_bus(onewire_bus_handle_t handle);

/**
 * @brief Send reset pulse on 1-wire bus, and detect if there are devices on the bus
 *
 * @param[in] handle 1-wire bus handle
 * @return
 *         - ESP_OK                There are devices present on 1-wire bus.
 *         - ESP_ERR_NOT_FOUND     There is no device present on 1-wire bus.
 */
esp_err_t onewire_bus_reset(onewire_bus_handle_t handle);

/**
 * @brief Write bytes to 1-wire bus, this is a blocking function
 *
 * @param[in] handle 1-wire bus handle
 * @param[in] tx_data pointer to data to be sent
 * @param[in] tx_data_count number of data to be sent
 * @return
 *         - ESP_OK                Write bytes to 1-wire bus successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_bus_write_bytes(onewire_bus_handle_t handle, const uint8_t *tx_data, uint8_t tx_data_size);

/**
 * @brief Read bytes from 1-wire bus
 *
 * @note While receiving data, we use rmt transmit channel to send 0xFF to generate read pulse,
 *       at the same time, receive channel is used to record weather the bus is pulled down by device.
 *
 * @param[in] handle 1-wire bus handle
 * @param[out] rx_data pointer to received data
 * @param[in] rx_data_count number of received data
 * @return
 *         - ESP_OK                Read bytes from 1-wire bus successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_bus_read_bytes(onewire_bus_handle_t handle, uint8_t *rx_data, size_t rx_data_size);

/**
 * @brief Write a bit to 1-wire bus, this is a blocking function
 *
 * @param[in] handle 1-wire bus handle
 * @param[in] tx_bit bit to transmit, 0 for zero bit, other for one bit
 * @return
 *         - ESP_OK                Write bit to 1-wire bus successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_bus_write_bit(onewire_bus_handle_t handle, uint8_t tx_bit);

/**
 * @brief Read a bit from 1-wire bus
 *
 * @param[in] handle 1-wire bus handle
 * @param[out] rx_bit received bit, 0 for zero bit, 1 for one bit
 * @return
 *         - ESP_OK                Read bit from 1-wire bus successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_bus_read_bit(onewire_bus_handle_t handle, uint8_t *rx_bit);
