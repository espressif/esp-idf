/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN YOUR APPLICATIONS
// The following APIs are for internal test.

#pragma once

#include "esp_err.h"
#include "driver/rmt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the unique ID of RMT channel
 *
 * @param[in] channel RMT generic channel that created by `rmt_new_tx_channel()` or `rmt_new_rx_channel()`
 * @param[out] ret_id The unique ID of RMT channel
 * @return
 *      - ESP_OK: Get RMT channel ID successfully
 *      - ESP_ERR_INVALID_ARG: Get RMT channel ID failed because of invalid argument
 *      - ESP_FAIL: Get RMT channel ID failed because of other reasons
 */
esp_err_t rmt_get_channel_id(rmt_channel_handle_t channel, int *ret_id);

#ifdef __cplusplus
}
#endif
