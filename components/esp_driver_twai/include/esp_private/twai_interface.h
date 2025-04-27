/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_twai_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP TWAI driver API set definition
 */
struct twai_node_base {
    /**
     * @brief Enable the TWAI node
     *
     * @param[in] node Pointer to the TWAI node base
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_FAIL: Failed to enable the node
     */
    esp_err_t (*enable)(struct twai_node_base *node);

    /**
     * @brief Disable the TWAI node
     *
     * @param[in] node Pointer to the TWAI node base
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_FAIL: Failed to disable the node
     */
    esp_err_t (*disable)(struct twai_node_base *node);

    /**
     * @brief Delete the TWAI node and free its resources
     *
     * @param[in] node Pointer to the TWAI node base
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_FAIL: Failed to delete the node
     */
    esp_err_t (*del)(struct twai_node_base *node);

    /**
     * @brief Configure the mask filter of the TWAI node
     *
     * @param node Pointer to the TWAI node base
     * @param filter_id Index of the filter to configure
     * @param mask_cfg Pointer to the mask filter configuration
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*config_mask_filter)(struct twai_node_base *node, uint8_t filter_id, const twai_mask_filter_config_t *mask_cfg);

    /**
     * @brief Configure the range filter of the TWAI node
     *
     * @param node Pointer to the TWAI node base
     * @param filter_id Index of the filter to configure
     * @param range_cfg Pointer to the range filter configuration
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*config_range_filter)(struct twai_node_base *node, uint8_t filter_id, const twai_range_filter_config_t *range_cfg);

    /**
     * @brief Reconfigure the timing parameters for the TWAI node
     *
     * @param[in] node Pointer to the TWAI node base
     * @param bit_timing Pointer to new twai cc(classic) or arbitration phase of twai fd timing configuration
     * @param data_timing Pointer to new twai fd timing configuration
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_ARG: Invalid timing configuration
     */
    esp_err_t (*reconfig_timing)(struct twai_node_base *node, const twai_timing_advanced_config_t *bit_timing, const twai_timing_advanced_config_t *data_timing);

    /**
     * @brief Transmit a TWAI frame through the TWAI node
     *
     * @param[in] node Pointer to the TWAI node base
     * @param[in] frame Pointer to the frame to transmit
     * @param[in] timeout Timeout in milliseconds
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_FAIL: Failed to transmit frame
     */
    esp_err_t (*transmit)(struct twai_node_base *node, const twai_frame_t *frame, int timeout);

    /**
     * @brief Receive a TWAI frame through the ISR callback
     *
     * @param[in] node Pointer to the TWAI node base
     * @param[out] rx_frame Pointer to the frame store rx content
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_TIMEOUT: Reception timeout
     */
    esp_err_t (*receive_isr)(struct twai_node_base *node, twai_frame_t *rx_frame);

    /**
     * @brief Recover the TWAI node from a bus-off state
     *
     * @param[in] node Pointer to the TWAI node base
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_FAIL: Failed to recover node
     */
    esp_err_t (*recover)(struct twai_node_base *node);

    /**
     * @brief Register event callbacks for the TWAI node
     *
     * @param[in] node Pointer to the TWAI node base
     * @param[in] cbs Pointer to the event callback configuration
     * @param[in] user_data User data to pass to the callbacks
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_ARG: Invalid callback configuration
     */
    esp_err_t (*register_cbs)(struct twai_node_base *node, const twai_event_callbacks_t *cbs, void *user_data);

    /**
     * @brief Get the status and statistics of the TWAI node
     *
     * @param[in] node Pointer to the TWAI node base
     * @param[out] status_ret Pointer to return the node status
     * @param[out] record_ret Pointer to return the node statistics
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_FAIL: Failed to retrieve information
     */
    esp_err_t (*get_info)(struct twai_node_base *node, twai_node_status_t *status_ret, twai_node_record_t *record_ret);
};

#ifdef __cplusplus
}
#endif
