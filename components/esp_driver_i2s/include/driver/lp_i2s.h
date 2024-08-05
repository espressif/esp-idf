/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/i2s_types.h"
#include "hal/i2s_types.h"

#include "esp_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LP_I2S max timeout value
 */
#define LP_I2S_MAX_DELAY UINT32_MAX

/**
 * @brief LP I2S controller channel configuration
*/
typedef struct {
    int                 id;                 /*!< LP I2S port id */
    i2s_role_t          role;               /*!< LP I2S role, only I2S_ROLE_SLAVE supported */
    size_t              threshold;          /*!< When LP I2S received bytes are bigger than this value, the `on_thresh_met` callback will be triggered. Must be in multiple of 4 */
} lp_i2s_chan_config_t;

/**
 * @brief LP I2S event callbacks
 */
typedef struct {
    lp_i2s_callback_t on_thresh_met;           ///< Triggered when the received bytes are bigger than `lp_i2s_chan_config_t:threshold`
    lp_i2s_callback_t on_request_new_trans;    ///< Triggered when a new transaction buffer is needed, when this callback is registered, you don't need to use `lp_i2s_channel_read` to get data, you can get data via this callback asynchronously
} lp_i2s_evt_cbs_t;

/**
 * @brief Allocate new LP I2S channel(s)
 *
 * @param[in]   chan_cfg        LP I2S controller channel configurations
 * @param[out]  ret_tx_handle   LP I2S channel handler used for managing the sending channel(optional), this one is not supported and is kept here for future-proof.
 * @param[out]  ret_rx_handle   LP I2S channel handler used for managing the receiving channel(optional)
 * @return
 *      - ESP_OK                 Allocate new channel(s) success
 *      - ESP_ERR_NOT_SUPPORTED  The communication mode is not supported on the current chip
 *      - ESP_ERR_INVALID_ARG    NULL pointer or illegal parameter in lp_i2s_chan_config_t
 *      - ESP_ERR_NOT_FOUND      No available LP I2S channel found
 */
esp_err_t lp_i2s_new_channel(const lp_i2s_chan_config_t *chan_cfg, lp_i2s_chan_handle_t *ret_tx_handle, lp_i2s_chan_handle_t *ret_rx_handle);

/**
 * @brief Register LP I2S event callbacks
 *
 * @param[in] handle     LP I2S channel handle
 * @param[in] cbs        Callbacks
 * @param[in] user_data  User data
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t lp_i2s_register_event_callbacks(lp_i2s_chan_handle_t handle, const lp_i2s_evt_cbs_t *cbs, void *user_data);

/**
 * @brief Enable LP I2S driver
 *
 * @param[in] handle     LP I2S channel handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t lp_i2s_channel_enable(lp_i2s_chan_handle_t chan);

/**
 * @brief Read LP I2S received data
 *
 * @param[in] handle      LP I2S channel handle
 * @param[in] trans       LP I2S transaction
 * @param[in] timeout_ms  Timeout in ms, set to `LP_I2S_MAX_DELAY` to wait until read is done
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state, e.g. `on_request_new_trans` callback is registered
 */
esp_err_t lp_i2s_channel_read(lp_i2s_chan_handle_t chan, lp_i2s_trans_t *trans, uint32_t timeout_ms);

/**
 * @brief Read LP I2S received data until certain bytes
 *
 * @param[in] handle      LP I2S channel handle
 * @param[in] trans       LP I2S transaction
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state, e.g. `on_request_new_trans` callback is registered
 */
esp_err_t lp_i2s_channel_read_until_bytes(lp_i2s_chan_handle_t chan, lp_i2s_trans_t *trans);

/**
 * @brief Disable LP I2S driver
 *
 * @param[in] handle     LP I2S channel handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t lp_i2s_channel_disable(lp_i2s_chan_handle_t chan);

/**
 * @brief Delete the LP I2S channel
 *
 * @param[in]   handle      LP I2S channel handler
 *
 * @return
 *      - ESP_OK                Delete successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 */
esp_err_t lp_i2s_del_channel(lp_i2s_chan_handle_t handle);

#ifdef __cplusplus
}
#endif
