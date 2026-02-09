/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_lcd_panel_ops.h"
#include "esp_check.h"
#include "example_crop.h"
#include "example_buffer.h"
#include "example_config.h"

static const char *TAG = "example_crop";

esp_err_t example_isp_crop_init(example_pingpong_buffer_ctx_t *ctx, int crop_h_res, int crop_v_res)
{
    ESP_RETURN_ON_FALSE(ctx != NULL, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");

    ctx->crop_h_res = crop_h_res;
    ctx->crop_v_res = crop_v_res;
    ctx->pending_buffer = NULL;
    ctx->frame_ready_sem = xSemaphoreCreateBinary();
    ESP_RETURN_ON_FALSE(ctx->frame_ready_sem != NULL, ESP_ERR_NO_MEM, TAG, "Failed to create frame ready semaphore");

    return ESP_OK;
}

/**
 * @brief Process frame: Add blank areas to fill full screen resolution
 *
 * Algorithm: Fill from bottom to top to avoid overwriting crop data
 * - Cropped image is placed at original position (relative to full frame)
 * - Other areas are filled with white (0xFFFF for RGB565)
 *
 * Illustration of crop operation:
 *
 * @verbatim
 * Full Frame (h_res x v_res)              Cropped Area
 * ┌─────────────────────────┐            ┌───────────────────────┐
 * │(0,0)                    │            │(crop_left,            │
 * │        Blank Area       │            │ crop_top)             │
 * │     ┌──────────────┐    │            │                       │
 * │     │              │    │    ===>    │                       │
 * │     │  CROP  Area  │    │            │      CROP  Area       │
 * │     │              │    │            │                       │
 * │     └──────────────┘    │            │                       │
 * │                 (h_res, │            │          (crop_right, │
 * │                  v_res) │            │           crop_bottom)│
 * └─────────────────────────┘            └───────────────────────┘
 *                                         crop_width x crop_height
 * @endverbatim
 *
 * @param[in,out] buffer Frame buffer to process (contains cropped image at start)
 * @param[in] ctx Ping-pong buffer context
 *
 * @note This function is necessary to ensure that the display matches the cropped image resolution
 */
static void example_isp_process_frame_with_blanks(void *buffer, example_pingpong_buffer_ctx_t *ctx)
{
    if (ctx == NULL || buffer == NULL) {
        return;
    }

    if (ctx->crop_v_res == ctx->v_res) {
        // No cropping, no need to process
        return;
    }

    uint16_t *fb = (uint16_t *)buffer;

    const int crop_left   = CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_H;
    const int crop_top    = CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_V;
    const int crop_right  = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_H;
    const int crop_bottom = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_V;
    const int crop_width  = crop_right - crop_left + 1;

    const int full_width  = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES;
    const int full_height = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES;

    // Helper macros for pixel indexing
#define SRC_PIXEL(x, y)  fb[(y) * crop_width + (x)]
#define DST_PIXEL(x, y)  fb[(y) * full_width + (x)]

    // ========== Step 1: Fill bottom blank region [crop_bottom+1, full_height) ==========
    if (crop_bottom + 1 < full_height) {
        memset(&DST_PIXEL(0, crop_bottom + 1),
               0xFF,
               full_width * (full_height - crop_bottom - 1) * EXAMPLE_RGB565_BYTES_PER_PIXEL);
    }

    // ========== Step 2: Process cropped region [crop_top, crop_bottom] ==========
    for (int y = crop_bottom; y >= crop_top; y--) {
        int src_y = y - crop_top;  // Corresponding row in cropped data (0-based)

        // Fill right blank region first (crop_right+1, full_width)
        if (crop_right + 1 < full_width) {
            memset(&DST_PIXEL(crop_right + 1, y),
                   0xFF,
                   (full_width - crop_right - 1) * EXAMPLE_RGB565_BYTES_PER_PIXEL);
        }

        // Copy crop data from source to destination
        memcpy(&DST_PIXEL(crop_left, y),
               &SRC_PIXEL(0, src_y),
               crop_width * EXAMPLE_RGB565_BYTES_PER_PIXEL);

        // Fill left blank region [0, crop_left)
        if (crop_left > 0) {
            memset(&DST_PIXEL(0, y),
                   0xFF,
                   crop_left * EXAMPLE_RGB565_BYTES_PER_PIXEL);
        }
    }

    // ========== Step 3: Fill top blank region [0, crop_top) ==========
    if (crop_top > 0) {
        memset(&DST_PIXEL(0, 0),
               0xFF,
               full_width * crop_top * EXAMPLE_RGB565_BYTES_PER_PIXEL);
    }

#undef SRC_PIXEL
#undef DST_PIXEL
}

/**
 * @brief Frame processing task
 *
 * This task processes frames when CROP is enabled:
 * - Waits for frame ready signal from ISR
 * - Processes the frame (adds blank areas if needed)
 * - Swaps buffers
 * - Triggers display update
 */
static void example_frame_processing_task(void *arg)
{
    example_pingpong_buffer_ctx_t *ctx = (example_pingpong_buffer_ctx_t *)arg;

    while (1) {
        // Wait for frame ready signal from ISR
        if (xSemaphoreTake(ctx->frame_ready_sem, portMAX_DELAY) == pdTRUE) {
            // Process the frame: add blank areas if needed
            example_isp_process_frame_with_blanks(ctx->pending_buffer, ctx);

            // Ping-Pong switch: swap CSI write buffer and DSI display buffer
            example_isp_buffer_swap(ctx);

            // Trigger buffer switch by calling draw_bitmap
            // DPI driver will detect which buffer we're using and switch to it
            esp_err_t ret = esp_lcd_panel_draw_bitmap(ctx->panel,
                                                      0, 0,
                                                      ctx->h_res,
                                                      ctx->crop_v_res,
                                                      ctx->pending_buffer);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to draw bitmap: %d", ret);
            }

            ESP_LOGD(TAG, "Frame displayed: %p", ctx->pending_buffer);
        }
    }
}

esp_err_t example_isp_crop_start_frame_processing_task(example_pingpong_buffer_ctx_t *ctx,
                                                       int task_priority,
                                                       int core_id)
{
    if (ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BaseType_t ret = xTaskCreatePinnedToCore(example_frame_processing_task,
                                             "frame_proc",
                                             4096,
                                             ctx,
                                             task_priority,
                                             NULL,
                                             core_id);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create frame processing task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Frame processing task created");
    return ESP_OK;
}

bool example_isp_crop_frame_ready_routine(example_pingpong_buffer_ctx_t *ctx, void *buffer)
{
    if (ctx == NULL || buffer == NULL) {
        return false;
    }

    BaseType_t high_task_wakeup = pdFALSE;
    ctx->pending_buffer = buffer;
    xSemaphoreGiveFromISR(ctx->frame_ready_sem, &high_task_wakeup);
    return (high_task_wakeup == pdTRUE);
}
