/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/parlio_rx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Force to trigger the EOF interrupt
 * @note This function is a workaround for the case that level delimiter needs to receive more than 64KB data in one transaction.
 *
 * @param rx_unit Parallel IO RX unit that created by `parlio_new_rx_unit`
 * @param need_yield Pointer to a status flag to record whether a task switch is needed if this API is being called in an ISR
 * @return
 *      - ESP_OK: Force to trigger the EOF interrupt successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument like NULL pointer
 *      - ESP_ERR_INVALID_STATE: Tx unit is in using, can't be called when pair tx unit is in using
 */
esp_err_t parlio_rx_unit_force_trigger_eof(parlio_rx_unit_handle_t rx_unit, bool *need_yield);

#ifdef __cplusplus
}
#endif
