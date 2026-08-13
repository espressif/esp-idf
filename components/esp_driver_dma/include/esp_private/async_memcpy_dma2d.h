/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN ANY APPLICATIONS
// DMA2D async 2D memcpy is a private helper built on async color convert
// for same-format window copy (e.g. LCD frame buffer blit).

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
 * @brief Opaque handle of DMA2D async 2D memcpy driver instance
 *
 * @note Internally this is a thin wrapper over async color convert with
 *       source/destination formats forced to the same value (copy-only path).
 */
typedef struct async_memcpy_dma2d_t *async_memcpy_dma2d_handle_t;

/**
 * @brief DMA2D async 2D memcpy event data
 */
typedef struct {
} async_memcpy_dma2d_event_data_t;

/**
 * @brief DMA2D async 2D memcpy ISR callback
 *
 * @note This callback runs in ISR context.
 *
 * @param[in] mcp Driver handle that produced this event
 * @param[in] edata Event data for the completed request
 * @param[in] cb_args User context passed to :cpp:func:`esp_async_memcpy_dma2d`
 *
 * @return
 *      - true: a higher-priority task was woken and a yield is requested
 *      - false: no yield request
 */
typedef bool (*async_memcpy_dma2d_isr_cb_t)(async_memcpy_dma2d_handle_t mcp,
                                            async_memcpy_dma2d_event_data_t *edata,
                                            void *cb_args);

/**
 * @brief DMA2D async 2D memcpy driver configuration
 */
typedef struct {
    uint32_t backlog;        /*!< Number of in-flight/pending requests. 0 means driver default. */
    size_t dma_burst_size;   /*!< DMA burst length in bytes. 0 means driver default. */
    uint32_t intr_priority;  /*!< Interrupt priority. 0 means default low/medium priority. */
} async_memcpy_dma2d_config_t;

/**
 * @brief DMA2D async 2D memcpy transaction descriptor
 *
 * Coordinates and size are in pixels.
 *
 * The source and destination windows are:
 * - source: [src_x, src_x + copy_width) x [src_y, src_y + copy_height)
 * - destination: [dst_x, dst_x + copy_width) x [dst_y, dst_y + copy_height)
 *
 * Both windows must be fully inside their corresponding image bounds.
 *
 * This API only performs same-format 2D window copy (no color conversion).
 */
typedef struct {
    const void *src_buffer;        /*!< Source picture base address */
    uint32_t src_stride;           /*!< Source picture row stride in pixels */
    uint32_t src_height;           /*!< Source picture height in pixels */
    uint32_t src_x;                /*!< Source window x offset in pixels */
    uint32_t src_y;                /*!< Source window y offset in pixels */

    void *dst_buffer;              /*!< Destination picture base address */
    uint32_t dst_stride;           /*!< Destination picture row stride in pixels */
    uint32_t dst_height;           /*!< Destination picture height in pixels */
    uint32_t dst_x;                /*!< Destination window x offset in pixels */
    uint32_t dst_y;                /*!< Destination window y offset in pixels */

    uint32_t copy_width;           /*!< Copy window width in pixels */
    uint32_t copy_height;          /*!< Copy window height in pixels */

    esp_color_fourcc_t pixel_format; /*!< Pixel format of both source and destination */
} async_memcpy_dma2d_trans_desc_t;

/**
 * @brief Install DMA2D async 2D memcpy driver
 *
 * This is a thin wrapper over :cpp:func:`esp_async_color_convert_install_dma2d`.
 * 2D window copy is implemented as a same-format color-convert request.
 *
 * @param[in] config Driver configuration
 * @param[out] ret_hdl Returned driver handle
 *
 * @return
 *      - ESP_OK: Driver installed successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_ERR_NO_MEM: Out of memory
 *      - ESP_ERR_NOT_FOUND: Required DMA2D resource is unavailable
 *      - others: Error from lower-level driver
 */
esp_err_t esp_async_memcpy_install_dma2d(const async_memcpy_dma2d_config_t *config,
                                         async_memcpy_dma2d_handle_t *ret_hdl);

/**
 * @brief Uninstall DMA2D async 2D memcpy driver
 *
 * @param[in] mcp Driver handle returned by :cpp:func:`esp_async_memcpy_install_dma2d`
 *
 * @return
 *      - ESP_OK: Driver uninstalled successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_ERR_INVALID_STATE: There are pending requests in the queue
 */
esp_err_t esp_async_memcpy_uninstall_dma2d(async_memcpy_dma2d_handle_t mcp);

/**
 * @brief Submit an asynchronous 2D memory copy request via DMA2D
 *
 * The request is enqueued and completed later in DMA2D interrupt context.
 * The callback can be NULL if no completion notification is needed.
 *
 * @param[in] mcp Driver handle returned by :cpp:func:`esp_async_memcpy_install_dma2d`
 * @param[in] trans 2D memcpy transaction descriptor
 * @param[in] cb_isr ISR callback invoked on copy completion, can be NULL
 * @param[in] cb_args User context passed to @p cb_isr
 *
 * @return
 *      - ESP_OK: Request accepted
 *      - ESP_ERR_INVALID_ARG: Invalid argument or invalid request fields
 *      - ESP_ERR_INVALID_STATE: No free internal transaction slot (queue full)
 *      - others: Error from lower-level driver
 */
esp_err_t esp_async_memcpy_dma2d(async_memcpy_dma2d_handle_t mcp,
                                 const async_memcpy_dma2d_trans_desc_t *trans,
                                 async_memcpy_dma2d_isr_cb_t cb_isr,
                                 void *cb_args);

#ifdef __cplusplus
}
#endif
