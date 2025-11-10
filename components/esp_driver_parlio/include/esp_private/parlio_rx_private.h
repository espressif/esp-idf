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
 * @brief Trigger the fake EOF interrupt
 * @note This function is a workaround for the case that level delimiter needs to receive more than 64KB data in one transaction.
 *       The hardware can't generate the EOF interrupt when the data length is greater than 64KB due to the limitation of the hardware,
 *       so this function is used to trigger the fake EOF interrupt.
 * @note This function will reset the whole parlio module,
 *       If the pair tx unit is in using,
 *       the reset operation will affect the TX unit and lead to unknown behavior
 * @usage If the application needs to receive more than 64KB data in one transaction, you can follow the steps below:
 *        1. Create a level delimiter with a length greater than 64KB
 *        2. Register the interrupt of the end edge on the valid GPIO
 *        3. Call this function to trigger the fake EOF interrupt in the GPIO interrupt handler
 *        4. Receive the transaction that is greater than 64KB
 *
 * @param rx_unit Parallel IO RX unit that created by `parlio_new_rx_unit`
 * @param need_yield Pointer to a status flag to record whether a task switch is needed if this API is being called in an ISR
 * @return
 *      - ESP_OK: Trigger the fake EOF interrupt successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument like NULL pointer
 *      - ESP_ERR_INVALID_STATE: Tx unit is in using, can't be called when pair tx unit is in using
 */
esp_err_t parlio_rx_unit_trigger_fake_eof(parlio_rx_unit_handle_t rx_unit, bool *need_yield);

#ifdef __cplusplus
}
#endif
