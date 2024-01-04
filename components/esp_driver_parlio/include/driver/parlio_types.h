/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

#ifdef __cplusplus
}
#endif
