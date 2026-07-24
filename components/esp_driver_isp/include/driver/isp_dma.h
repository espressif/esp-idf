/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include "esp_err.h"
#include "driver/isp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Process one ISP DMA frame: feed the input buffer through the ISP and wait for completion
 *
 * @note The driver synchronizes cacheable buffers before and after DMA access. Input buffers use
 *       an unaligned cache write-back, while cacheable output buffers and their derived frame
 *       sizes must be aligned to the cache line size.
 * @note This function blocks until both input and output DMA channels finish. On timeout,
 *       the output buffer may still be owned by DMA and must not be accessed.
 *
 * @param[in] proc           Processor handle
 * @param[in] output_buffer  Destination buffer for ISP output
 * @param[in] input_buffer   Source input buffer for RAW frame data
 * @param[in] timeout_ms     Timeout in milliseconds for waiting transfer completion
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Invalid argument
 *         - ESP_ERR_TIMEOUT       Wait timeout
 */
esp_err_t esp_isp_dma_process_frame(isp_proc_handle_t proc, void *output_buffer, const void *input_buffer, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
