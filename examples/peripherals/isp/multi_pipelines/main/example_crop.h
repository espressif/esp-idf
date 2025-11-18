// SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0
//
// ESP-IDF header for cropping configuration (example_crop.h)

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include "example_buffer.h"

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE

/**
 * @brief Initialize cropping configuration
 *
 * @param[in] ctx Pointer to ping-pong buffer context
 * @param[in] crop_h_res Cropped horizontal resolution
 * @param[in] crop_v_res Cropped vertical resolution
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_NO_MEM: Failed to create semaphore (if CROP enabled)
 */
esp_err_t example_isp_crop_init(example_pingpong_buffer_ctx_t *ctx, int crop_h_res, int crop_v_res);
/**
 * @brief Start frame processing task
 *
 * This task handles frame processing and buffer swapping when CROP is enabled.
 *
 * @param[in] ctx Pointer to ping-pong buffer context
 * @param[in] task_priority Task priority
 * @param[in] core_id Core ID to pin the task
 * @return
 *         - ESP_OK: Success
 *         - ESP_FAIL: Failed to create frame processing task
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t example_isp_crop_start_frame_processing_task(example_pingpong_buffer_ctx_t *ctx,
                                                       int task_priority,
                                                       int core_id);

/**
 * @brief Signal that a frame is ready for processing
 *
 * This function should be called from ISR context when a frame is ready.
 *
 * @param[in,out] ctx Pointer to ping-pong buffer context
 * @param[in] buffer Pointer to the completed frame buffer
 * @return true if a higher priority task was woken
 */
bool example_isp_crop_frame_ready_routine(example_pingpong_buffer_ctx_t *ctx, void *buffer);
#endif // CONFIG_EXAMPLE_ISP_CROP_ENABLE

#ifdef __cplusplus
}
#endif
