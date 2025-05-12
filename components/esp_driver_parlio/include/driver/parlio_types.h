/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "hal/parlio_types.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of Parallel IO TX unit handle
 */
typedef struct parlio_tx_unit_t *parlio_tx_unit_handle_t;

/**
 * @brief Type of Parallel IO RX unit handle
 */
typedef struct parlio_rx_unit_t *parlio_rx_unit_handle_t;

/**
 * @brief Type of Parallel IO RX frame delimiter handle
 */
typedef struct parlio_rx_delimiter_t *parlio_rx_delimiter_handle_t;

/**
 * @brief Type of Parallel IO TX done event data
 */
typedef struct {
} parlio_tx_done_event_data_t;

/**
 * @brief Prototype of parlio tx event callback
 * @param[in] tx_unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @param[in] edata Point to Parallel IO TX event data. The lifecycle of this pointer memory is inside this function,
 *                  user should copy it into static memory if used outside this function.
 * @param[in] user_ctx User registered context, passed from `parlio_tx_unit_register_event_callbacks`
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*parlio_tx_done_callback_t)(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx);

#ifdef __cplusplus
}
#endif
