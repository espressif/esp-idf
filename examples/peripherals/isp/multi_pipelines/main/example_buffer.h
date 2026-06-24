/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file example_buffer.h
 * @brief Ping-Pong Buffer Management Module
 *
 * This module implements a ping-pong buffer management mechanism for synchronizing
 * data between CSI image capture and DSI display.
 *
 * Why use dual buffers?
 * - Ensure smooth display: Avoid frame tearing caused by different rates of CSI and DSI transfer
 *
 * How it works:
 * - Two buffers (fb0 and fb1) are used alternately
 * - CSI writes to the buffer pointed by csi_buffer
 * - DSI reads and displays from the buffer pointed by dsi_buffer
 * - After each frame is completed, the roles of the two buffers are swapped
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Ping-Pong buffer context structure
 */
typedef struct {
    void *fb0;                   // Frame buffer 0
    void *fb1;                   // Frame buffer 1
    void *csi_buffer;            // Current buffer for CSI to write
    void *dsi_buffer;            // Current buffer for DSI to display
    esp_lcd_panel_handle_t panel;// DPI panel handle
    int h_res;                   // Horizontal resolution
    int v_res;                   // Vertical resolution (full screen)
#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    int crop_h_res;              // Cropped horizontal resolution
    int crop_v_res;              // Cropped vertical resolution
    void *pending_buffer;        // Buffer pending to be displayed
    SemaphoreHandle_t frame_ready_sem; // Semaphore to signal frame ready
#endif
} example_pingpong_buffer_ctx_t;

/**
 * @brief Initialize ping-pong buffer context
 *
 * @param[in] ctx Pointer to ping-pong buffer context
 * @param[in] fb0 Pointer to frame buffer 0
 * @param[in] fb1 Pointer to frame buffer 1
 * @param[in] panel LCD panel handle
 * @param[in] h_res Horizontal resolution
 * @param[in] v_res Vertical resolution
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t example_isp_buffer_init(example_pingpong_buffer_ctx_t *ctx,
                                  void *fb0,
                                  void *fb1,
                                  esp_lcd_panel_handle_t panel,
                                  int h_res,
                                  int v_res);

/**
 * @brief Swap CSI write buffer and DSI display buffer
 *
 * @param[in,out] ctx Pointer to ping-pong buffer context
 */
void example_isp_buffer_swap(example_pingpong_buffer_ctx_t *ctx);

/**
 * @brief Get current CSI write buffer
 *
 * @param[in] ctx Pointer to ping-pong buffer context
 * @return Pointer to current CSI buffer
 */
void *example_isp_buffer_get_csi_buffer(example_pingpong_buffer_ctx_t *ctx);

/**
 * @brief Get current DSI display buffer
 *
 * @param[in] ctx Pointer to ping-pong buffer context
 * @return Pointer to current DSI buffer
 */
void *example_isp_buffer_get_dsi_buffer(example_pingpong_buffer_ctx_t *ctx);

#ifdef __cplusplus
}
#endif
