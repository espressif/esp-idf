/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/rmt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Delete an RMT channel
 *
 * @param[in] channel RMT generic channel that created by `rmt_new_tx_channel()` or `rmt_new_rx_channel()`
 * @return
 *      - ESP_OK: Delete RMT channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete RMT channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Delete RMT channel failed because it is still in working
 *      - ESP_FAIL: Delete RMT channel failed because of other error
 */
esp_err_t rmt_del_channel(rmt_channel_handle_t channel);

/**
 * @brief Apply modulation feature for TX channel or demodulation feature for RX channel
 *
 * @param[in] channel RMT generic channel that created by `rmt_new_tx_channel()` or `rmt_new_rx_channel()`
 * @param[in] config Carrier configuration. Specially, a NULL config means to disable the carrier modulation or demodulation feature
 * @return
 *      - ESP_OK: Apply carrier configuration successfully
 *      - ESP_ERR_INVALID_ARG: Apply carrier configuration failed because of invalid argument
 *      - ESP_FAIL: Apply carrier configuration failed because of other error
 */
esp_err_t rmt_apply_carrier(rmt_channel_handle_t channel, const rmt_carrier_config_t *config);

/**
 * @brief Enable the RMT channel
 *
 * @note This function will acquire a PM lock that might be installed during channel allocation
 *
 * @param[in] channel RMT generic channel that created by `rmt_new_tx_channel()` or `rmt_new_rx_channel()`
 * @return
 *      - ESP_OK: Enable RMT channel successfully
 *      - ESP_ERR_INVALID_ARG: Enable RMT channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable RMT channel failed because it's enabled already
 *      - ESP_FAIL: Enable RMT channel failed because of other error
 */
esp_err_t rmt_enable(rmt_channel_handle_t channel);

/**
 * @brief Disable the RMT channel
 *
 * @note This function will release a PM lock that might be installed during channel allocation
 *
 * @param[in] channel RMT generic channel that created by `rmt_new_tx_channel()` or `rmt_new_rx_channel()`
 * @return
 *      - ESP_OK: Disable RMT channel successfully
 *      - ESP_ERR_INVALID_ARG: Disable RMT channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable RMT channel failed because it's not enabled yet
 *      - ESP_FAIL: Disable RMT channel failed because of other error
 */
esp_err_t rmt_disable(rmt_channel_handle_t channel);

#ifdef __cplusplus
}
#endif
