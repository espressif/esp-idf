/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_a2dp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set A2DP connection handle and audio MTU used for sending.
 *
 * @param[in] conn_hdl   Connection handle
 * @param[in] audio_mtu  Audio MTU
 *
 * @note Call after connection is established and before encode_task_launch().
 *       Cleared internally when encode_task_stop() completes.
 */
void encode_task_set_a2dp_link(esp_a2d_conn_hdl_t conn_hdl, uint16_t audio_mtu);

/**
 * @brief Save negotiated stream codec configuration (AAC only).
 *
 * @param[in] mcc  Media codec configuration from ESP_A2D_AUDIO_CFG_EVT.
 *                 Must be ESP_A2D_MCT_M24; NULL or other types clear the stored config.
 */
void encode_task_set_stream_config(const esp_a2d_mcc_t *mcc);

/**
 * @brief Check whether a valid M24 stream configuration has been stored.
 *
 * @return true if encode_task_set_stream_config() was called with ESP_A2D_MCT_M24
 */
bool encode_task_has_stream_config(void);

/**
 * @brief Start the AAC encode/send task.
 *
 * @note Spawns task "a2dp_aac", which loops: pace frame → PCM sine → AAC encode →
 *       A2DP send. Requires prior encode_task_set_stream_config() and encode_task_set_a2dp_link().
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the task is already running or no M24 config
 *      - ESP_FAIL if task creation or encoder open fails
 */
esp_err_t encode_task_launch(void);

/**
 * @brief Stop the AAC stream task and release encoder resources.
 */
void encode_task_stop(void);

/**
 * @brief Query whether the AAC stream task is active.
 *
 * @return true while the internal "a2dp_aac" task is running
 */
bool encode_task_is_running(void);

#ifdef __cplusplus
}
#endif
