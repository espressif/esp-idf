/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_twai_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- Node Control -------------------------------------------------- */

/**
 * @brief Enable the TWAI node
 *
 * @param node Handle to the TWAI node
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_enable(twai_node_handle_t node);

/**
 * @brief Disable the TWAI node
 *
 * @param node Handle to the TWAI node
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_disable(twai_node_handle_t node);

/**
 * @brief Init the recover process for TWAI node which in bus-off
 * @note  Follow `on_state_change` callback or `twai_node_get_info` to know recover finish or not
 *
 * @param node Handle to the TWAI node
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_recover(twai_node_handle_t node);

/**
 * @brief Delete the TWAI node and release resources
 *
 * @param node Handle to the TWAI node
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_delete(twai_node_handle_t node);

/**
 * @brief Register event callbacks for the TWAI node
 *
 * @param node Handle to the TWAI node
 * @param cbs Pointer to a structure of event callback functions
 * @param user_data User-defined data passed to callback functions
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_register_event_callbacks(twai_node_handle_t node, const twai_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Reconfigure the timing settings of the TWAI node
 *
 * @note You can reconfigure the timing for the arbitration and data phase, separately or together.
 *
 * @param node Handle to the TWAI node
 * @param bit_timing Optional,pointer to new twai cc(classic) or arbitration phase of twai fd timing configuration
 * @param data_timing Optional, pointer to new twai fd timing configuration
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_reconfig_timing(twai_node_handle_t node, const twai_timing_advanced_config_t *bit_timing, const twai_timing_advanced_config_t *data_timing);

/**
 * @brief Configure the mask filter of the TWAI node
 *
 * @param node Handle to the TWAI node
 * @param filter_id Index of the filter to configure
 * @param mask_cfg Pointer to the mask filter configuration
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_config_mask_filter(twai_node_handle_t node, uint8_t filter_id, const twai_mask_filter_config_t *mask_cfg);

/**
 * @brief Configure the range filter of the TWAI node
 *
 * @param node Handle to the TWAI node
 * @param filter_id Index of the filter to configure
 * @param range_cfg Pointer to the range filter configuration
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_config_range_filter(twai_node_handle_t node, uint8_t filter_id, const twai_range_filter_config_t *range_cfg);

/**
 * @brief Get information about the TWAI node
 *
 * @param node Handle to the TWAI node
 * @param status_ret Pointer to store the current node status
 * @param statistics_ret Pointer to store node statistics
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_get_info(twai_node_handle_t node, twai_node_status_t *status_ret, twai_node_record_t *statistics_ret);

/* ----------------------------------------------- Node Communication ----------------------------------------------- */

/**
 * @brief Transmit a TWAI frame
 *
 * @param[in] node Handle to the TWAI node
 * @param[in] frame Pointer to the frame to transmit
 * @param[in] timeout_ms Maximum wait time if the transmission queue is full (milliseconds), -1 to wait forever
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_transmit(twai_node_handle_t node, const twai_frame_t *frame, int timeout_ms);

/**
 * @brief Receive a TWAI frame from 'rx_done_cb'
 *
 * @note This function can only be called from the `rx_done_cb` callback, you can't call it from a task.
 * @note Please also provide `buffer` and `buffer_len` inside the rx_frame
 * @note Can get original data length from `twaifd_dlc2len(rx_frame.header.dlc)`
 *
 * @param[in] node Handle to the TWAI node
 * @param[out] rx_frame Pointer to the frame store rx content
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t twai_node_receive_from_isr(twai_node_handle_t node, twai_frame_t *rx_frame);

#ifdef __cplusplus
}
#endif
