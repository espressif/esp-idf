/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_cam_ctlr_types.h"
#include "hal/cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the ESP CAM controller.
 *
 * @param[in] handle ESP CAM controller handle.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid state.
 */
esp_err_t esp_cam_ctlr_enable(esp_cam_ctlr_handle_t handle);

/**
 * @brief Start the ESP CAM controller.
 *
 * @param[in] handle ESP CAM controller handle.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid state.
 */
esp_err_t esp_cam_ctlr_start(esp_cam_ctlr_handle_t handle);

/**
 * @brief Stop the ESP CAM controller.
 *
 * @param[in] handle ESP CAM controller handle.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid state.
 */
esp_err_t esp_cam_ctlr_stop(esp_cam_ctlr_handle_t handle);

/**
 * @brief Disable the ESP CAM controller.
 *
 * @param[in] handle ESP CAM controller handle.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid state.
 */
esp_err_t esp_cam_ctlr_disable(esp_cam_ctlr_handle_t handle);

/**
 * @brief Receive data for the given transaction.
 *
 * @param[in] handle ESP CAM controller handle.
 * @param[in] trans ESP CAM controller transaction type.
 * @param[in] timeout_ms Timeout. Measurement unit: milliseconds.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid state.
 */
esp_err_t esp_cam_ctlr_receive(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms);

/**
 * @brief Delete the ESP CAM controller handle.
 *
 * @param[in] handle ESP CAM controller handle.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid state.
 */
esp_err_t esp_cam_ctlr_del(esp_cam_ctlr_handle_t handle);

/**
 * @brief Register ESP CAM controller event callbacks.
 *
 * @param[in] handle ESP CAM controller handle.
 * @param[in] cbs ESP CAM controller event callbacks.
 * @param[in] user_data User data.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid state.
 */
esp_err_t esp_cam_ctlr_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data);

/**
 * @brief Get the ESP CAM controller internal allocated backup buffer(s) address.
 *
 * @note Data in the internal buffer is generally ready when the `on_trans_finished` event occurs.
 *
 * @param[in] handle ESP CAM controller handle.
 * @param[in] fb_num Number of frame buffers to get. This value must match the number of subsequent fbN parameters.
 * @param[out] fb0 Address of the first frame buffer.
 * @param[out] ... List of other frame buffers, if any.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_INVALID_STATE: Invalid driver state.
 */
esp_err_t esp_cam_ctlr_get_frame_buffer(esp_cam_ctlr_handle_t handle, uint32_t fb_num, const void **fb0, ...);

/**
 * @brief Get the length of the ESP CAM controller internal backup buffer.
 *
 * @param[in] handle ESP CAM controller handle.
 * @param[out] ret_fb_len Optional. The size of each frame buffer. Measurement unit: bytes.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: NULL pointer.
 *      - ESP_ERR_INVALID_STATE: Invalid driver state.
 */
esp_err_t esp_cam_ctlr_get_frame_buffer_len(esp_cam_ctlr_handle_t handle, size_t *ret_fb_len);

/**
 * @brief Allocate a camera buffer for the ESP CAM controller.
 *
 * @note This function must be called after `esp_cam_new_*_ctlr`.
 *
 * @param[in] handle ESP CAM controller handle.
 * @param[in] size Buffer size. Measurement unit: bytes.
 * @param[in] buf_caps Buffer allocation capabilities:
 *      - `MALLOC_CAP_SPIRAM` || `MALLOC_CAP_DMA`: Memory in external SPI RAM.
 *      - `MALLOC_CAP_INTERNAL` || `MALLOC_CAP_DMA`: Memory in internal SRAM.
 *
 * @return
 *      - Buffer pointer on success.
 *      - NULL on failure.
 */
void *esp_cam_ctlr_alloc_buffer(esp_cam_ctlr_handle_t handle, size_t size, uint32_t buf_caps);

/**
 * @brief Configure format conversion.
 *
 * @param[in] handle ESP CAM controller handle.
 * @param[in] conv_cfg Color conversion configuration, containing source and destination formats.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_NOT_SUPPORTED: Format conversion not supported by this controller.
 */
esp_err_t esp_cam_ctlr_format_conversion(esp_cam_ctlr_handle_t handle, const cam_ctlr_format_conv_config_t *conv_cfg);

#ifdef __cplusplus
}
#endif
