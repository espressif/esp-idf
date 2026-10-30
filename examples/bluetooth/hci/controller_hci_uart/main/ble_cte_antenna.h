/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"

/**
 * @brief Configure 1–4 GPIOs as encoded CTE antenna selection bits, LSB first.
 *
 * Call after controller initialization and before enabling the controller.
 * The caller owns the board-specific GPIO selection.
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG for a null array or invalid
 *         count, or the error returned by gpio_config().
 */
esp_err_t ble_cte_antenna_init(const uint8_t *gpios, uint8_t count);
