/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "driver/gptimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wiegand interface configuration
 */
typedef struct {
    gpio_num_t data_pin0;        /*!< Pin of Data0 */
    gpio_num_t data_pin1;        /*!< Pin of Data1 */
    uint8_t pulse_width_us;      /*!< Negative pulse width, in microseconds */
    uint16_t interval_width_us;  /*!< Wiegand data interval, a.k.a. period, in microseconds */
} wiegand_io_config_t;

/**
 * @brief Type of Wiegand interface handle
 */
typedef struct wiegand_interface_t *wiegand_io_handle_t;

/**
 * @brief Create a new wiegand interface io, and return the handle
 *
 * @param[in] config Wiegand interface configuration
 * @param[out] ret_handle Returned wiegand interface handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating wiegand interface
 *      - ESP_OK if creating wiegand interface successfully
 */
esp_err_t wiegand_new_io(const wiegand_io_config_t *config, wiegand_io_handle_t *ret_handle);

/**
 * @brief Send data via created wiegand interface io
 *
 * @param[in] wiegand_io Wiegand interface io handle
 * @param[in] data Wiegand data
 * @param[in] num_of_bits The bits number of wiegand data
 * @return
 *      - ESP_ERR_INVALID_STATE if previous data has not sent completely
 *      - ESP_OK if start sending data successfully
 */
esp_err_t wiegand_io_send(wiegand_io_handle_t wiegand_io, void *data, uint8_t num_of_bits);

#ifdef __cplusplus
}
#endif
