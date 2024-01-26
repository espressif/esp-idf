/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "hal/color_types.h"

/**
 * @brief Async FrameBuffer copy context
 */
typedef struct esp_async_fbcpy_context_t *esp_async_fbcpy_handle_t;

/**
 * @brief Async FrameBuffer copy configuration
 */
typedef struct {
} esp_async_fbcpy_config_t;

/**
 * @brief Install Async FrameBuffer copy driver
 *
 * @param config Async FrameBuffer copy configuration
 * @param mcp Returned Async FrameBuffer copy handle
 * @return
 *      - ESP_OK: Install Async FrameBuffer copy driver successfully
 *      - ESP_ERR_INVALID_ARG: Install Async FrameBuffer copy driver failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Install Async FrameBuffer copy driver failed because of out of memory
 *      - ESP_FAIL: Install Async FrameBuffer copy driver failed because of other error
 */
esp_err_t esp_async_fbcpy_install(const esp_async_fbcpy_config_t *config, esp_async_fbcpy_handle_t *mcp);

/**
 * @brief Uninstall Async FrameBuffer copy driver
 *
 * @param mcp Async FrameBuffer copy handle
 * @return
 *      - ESP_OK: Uninstall Async FrameBuffer copy driver successfully
 *      - ESP_ERR_INVALID_ARG: Uninstall Async FrameBuffer copy driver failed because of invalid argument
 *      - ESP_FAIL: Uninstall Async FrameBuffer copy driver failed because of other error
 */
esp_err_t esp_async_fbcpy_uninstall(esp_async_fbcpy_handle_t mcp);

/**
 * @brief Async FrameBuffer copy transaction descriptor
 */
typedef struct {
    const void *src_buffer;   /*!< Source buffer */
    void *dst_buffer;         /*!< Destination buffer */
    size_t src_buffer_size_x; /*!< Source buffer size in x direction, size count in the number of pixels */
    size_t src_buffer_size_y; /*!< Source buffer size in y direction, size count in the number of pixels */
    size_t dst_buffer_size_x; /*!< Destination buffer size in x direction, size count in the number of pixels */
    size_t dst_buffer_size_y; /*!< Destination buffer size in y direction, size count in the number of pixels */
    size_t src_offset_x;      /*!< Copy action will start from this offset in source buffer in the x direction, offset count in the number of pixels */
    size_t src_offset_y;      /*!< Copy action will start from this offset in source buffer in the y direction, offset count in the number of pixels */
    size_t dst_offset_x;      /*!< Copy action will start from this offset in destination buffer in the x direction, offset count in the number of pixels */
    size_t dst_offset_y;      /*!< Copy action will start from this offset in destination buffer in the y direction, offset count in the number of pixels */
    size_t copy_size_x;       /*!< Copy size in the x direction, size count in the number of pixels */
    size_t copy_size_y;       /*!< Copy size in the y direction, size count in the number of pixels */
    color_space_pixel_format_t pixel_format_unique_id; /*!< Pixel format unique ID */
} esp_async_fbcpy_trans_desc_t;

/**
 * @brief Async FrameBuffer copy event data
 */
typedef struct {
} esp_async_fbcpy_event_data_t;

/**
 * @brief Async FrameBuffer copy event callback prototype
 */
typedef bool (*esp_async_fbcpy_event_callback_t)(esp_async_fbcpy_handle_t mcp, esp_async_fbcpy_event_data_t *event_data, void *cb_args);

/**
 * @brief Start Async FrameBuffer copy transaction
 *
 * @param mcp Async FrameBuffer copy handle
 * @param transaction Async FrameBuffer copy transaction descriptor
 * @param memcpy_done_cb Callback function that will be invoked when Async FrameBuffer copy transaction finishes
 * @param cb_args User data
 * @return
 *      - ESP_OK: Start Async FrameBuffer copy transaction successfully
 *      - ESP_ERR_INVALID_ARG: Start Async FrameBuffer copy transaction failed because of invalid argument
 *      - ESP_FAIL: Start Async FrameBuffer copy transaction failed because of other error
 */
esp_err_t esp_async_fbcpy(esp_async_fbcpy_handle_t mcp, esp_async_fbcpy_trans_desc_t* transaction,
                          esp_async_fbcpy_event_callback_t memcpy_done_cb, void *cb_args);
