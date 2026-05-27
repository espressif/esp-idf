/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle of async color conversion driver instance
 */
typedef struct async_color_convert_context_t *async_color_convert_handle_t;

/**
 * @brief Async color conversion event data
 */
typedef struct {
} async_color_convert_event_data_t;

/**
 * @brief Async color conversion callback type
 *
 * @note This callback runs in ISR context.
 *
 * @param[in] conv_hdl Driver handle that produced this event
 * @param[in] edata Event data for the completed request
 * @param[in] cb_args User context passed to :cpp:func:`esp_async_color_convert`
 *
 * @return
 *      - true: a higher-priority task was woken and a yield is requested
 *      - false: no yield request
 */
typedef bool (*async_color_convert_isr_cb_t)(async_color_convert_handle_t conv_hdl,
                                             async_color_convert_event_data_t *edata,
                                             void *cb_args);

/**
 * @brief Async color conversion driver configuration
 */
typedef struct {
    uint32_t backlog;        /*!< Number of in-flight/pending requests. 0 means driver default. */
    size_t dma_burst_size;   /*!< DMA burst length in bytes. 0 means driver default. */
    uint32_t intr_priority;  /*!< Interrupt priority. 0 means default low/medium priority. */
} async_color_convert_config_t;

/**
 * @brief Install async color conversion driver with the DMA2D backend
 *
 * This API allocates internal resources and creates a conversion context.
 *
 * @param[in] config Driver configuration
 * @param[out] ret_hdl Returned driver handle
 *
 * @return
 *      - ESP_OK: Driver installed successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_ERR_NO_MEM: Out of memory
 *      - ESP_ERR_NOT_FOUND: Required DMA2D resource is unavailable
 *      - others: Error from lower-level DMA2D driver
 */
esp_err_t esp_async_color_convert_install_dma2d(const async_color_convert_config_t *config,
                                                async_color_convert_handle_t *ret_hdl);

/**
 * @brief Uninstall async color conversion driver
 *
 * @param[in] conv_hdl Driver handle returned by :cpp:func:`esp_async_color_convert_install_dma2d`
 *
 * @return
 *      - ESP_OK: Driver uninstalled successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_ERR_INVALID_STATE: There are pending requests in the queue
 */
esp_err_t esp_async_color_convert_uninstall(async_color_convert_handle_t conv_hdl);

/**
 * @brief Async color conversion request
 *
 * Coordinates and size are in pixels.
 *
 * The source and destination windows are:
 * - source: [src_x, src_x + copy_width) x [src_y, src_y + copy_height)
 * - destination: [dst_x, dst_x + copy_width) x [dst_y, dst_y + copy_height)
 *
 * Both windows must be fully inside their corresponding image bounds.
 *
 * Conversion rule is inferred from source and destination formats:
 * - If source and destination are the same format, it performs 2D copy only.
 */
typedef struct {
    const void *src_buffer;  /*!< Source picture base address */
    uint32_t src_stride;     /*!< Source picture row stride in pixels */
    uint32_t src_height;     /*!< Source picture height in pixels */
    uint32_t src_x;          /*!< Source window x offset in pixels */
    uint32_t src_y;          /*!< Source window y offset in pixels */

    void *dst_buffer;        /*!< Destination picture base address */
    uint32_t dst_stride;     /*!< Destination picture row stride in pixels */
    uint32_t dst_height;     /*!< Destination picture height in pixels */
    uint32_t dst_x;          /*!< Destination window x offset in pixels */
    uint32_t dst_y;          /*!< Destination window y offset in pixels */

    uint32_t copy_width;     /*!< Conversion window width in pixels */
    uint32_t copy_height;    /*!< Conversion window height in pixels */

    esp_color_fourcc_t src_color_format;   /*!< Source pixel format */
    esp_color_fourcc_t dst_color_format;   /*!< Destination pixel format */
    color_conv_std_rgb_yuv_t color_conv_std; /*!< RGB/YUV conversion standard for RGB888<->UYVY422 */
} async_color_convert_request_t;

/**
 * @brief Submit an asynchronous 2D color conversion request
 *
 * The request is enqueued and completed later in DMA2D interrupt context.
 * The callback can be NULL if no completion notification is needed.
 *
 * @param[in] conv_hdl Driver handle returned by :cpp:func:`esp_async_color_convert_install_dma2d`
 * @param[in] request Color conversion request
 * @param[in] cb_isr ISR callback invoked on conversion completion, can be NULL
 * @param[in] cb_args User context passed to @p cb_isr
 *
 * @return
 *      - ESP_OK: Request accepted
 *      - ESP_ERR_INVALID_ARG: Invalid argument or invalid request fields
 *      - ESP_ERR_INVALID_STATE: No free internal transaction slot (queue full)
 *      - others: Error from lower-level DMA2D driver
 */
esp_err_t esp_async_color_convert(async_color_convert_handle_t conv_hdl,
                                  const async_color_convert_request_t *request,
                                  async_color_convert_isr_cb_t cb_isr,
                                  void *cb_args);

/**
 * @brief Blocking 2D color conversion API built on async request path
 *
 * @note This API must not be called from ISR context.
 *
 * @param[in] conv_hdl Driver handle returned by :cpp:func:`esp_async_color_convert_install_dma2d`
 * @param[in] request Color conversion request
 * @param[in] timeout_ms Timeout in milliseconds. Currently only ``-1`` is supported, which waits forever.
 *
 * @return
 *      - ESP_OK: Conversion completed successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument, unsupported timeout, or invalid request fields
 *      - ESP_ERR_INVALID_STATE: Called from ISR context, or queue unavailable
 *      - others: Error from lower-level DMA2D driver
 */
esp_err_t esp_color_convert_blocking(async_color_convert_handle_t conv_hdl,
                                     const async_color_convert_request_t *request,
                                     int32_t timeout_ms);

#ifdef __cplusplus
}
#endif
