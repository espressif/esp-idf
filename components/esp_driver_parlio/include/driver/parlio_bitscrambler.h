/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/parlio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Decorate Parlio TX units with BitScrambler
 *
 * @note This function creates a BitScrambler instance and associates it with the Parlio TX unit.
 *       The decorated TX unit will be able to do some pre-process to the user data with the help of BitScrambler
 *       Only can be called before enable the TX unit
 *
 * @param[in] tx_unit Parlio TX unit handle
 * @return
 *      - ESP_OK: Decorate Parlio TX units with BitScrambler success
 *      - ESP_ERR_INVALID_ARG: Failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Failed because the TX unit is already decorated with BitScrambler
 *      - ESP_FAIL: Failed because of other error
 */
esp_err_t parlio_tx_unit_decorate_bitscrambler(parlio_tx_unit_handle_t tx_unit);

/**
 * @brief Remove the BitScrambler decoration from the Parlio TX unit
 *
 * @note This function removes the BitScrambler decoration from the Parlio TX unit, restoring the original functionality.
 *       Only can be called before enable the TX unit
 *
 * @param[in] tx_unit Parlio TX unit handle
 * @return
 *      - ESP_OK: Remove the BitScrambler decoration from the Parlio TX unit success
 *      - ESP_ERR_INVALID_ARG: Failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Failed because the TX unit is not decorated with BitScrambler
 *      - ESP_FAIL: Failed because of other error
 */
esp_err_t parlio_tx_unit_undecorate_bitscrambler(parlio_tx_unit_handle_t tx_unit);

#ifdef __cplusplus
}
#endif
