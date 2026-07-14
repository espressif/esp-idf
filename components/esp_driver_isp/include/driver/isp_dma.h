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
 * @note Input buffer content should be ready before calling this function. If the buffers are in
 *       cacheable memory, the caller should synchronize them around DMA access. Buffer sizes are
 *       derived from the ISP processor resolution and pixel formats. Both buffers must be 8-byte
 *       aligned. This function blocks until both input and output DMA channels finish.
 *
 * @param[in] proc           Processor handle
 * @param[in] output_buffer  Destination buffer for ISP output (8-byte aligned)
 * @param[in] input_buffer   Source input buffer for RAW frame data (8-byte aligned)
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
