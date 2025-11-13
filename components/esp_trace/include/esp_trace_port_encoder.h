/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "esp_err.h"
#include "esp_trace_types.h"

/* forward declarations */
typedef struct esp_trace_encoder esp_trace_encoder_t;
typedef struct esp_trace_transport esp_trace_transport_t;

/**
 * @brief Encoder Virtual Table
 *
 * Defines the interface for trace encoders (libraries)
 */
typedef struct {
    /**
     * @brief Initialize encoder
     * @param enc Encoder instance
     * @param enc_cfg Encoder-specific configuration
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*init)(esp_trace_encoder_t *enc, const void *enc_cfg);

    /**
     * @brief Write trace data
     * @param enc Encoder instance
     * @param data Data buffer to write
     * @param size Size of data in bytes
     * @param tmo Timeout in microseconds
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t (*write)(esp_trace_encoder_t *enc, const void *data, size_t size, uint32_t tmo);

    /**
     * @brief Panic handler
     * @param enc Encoder instance
     * @param info Panic information
     */
    void (*panic_handler)(esp_trace_encoder_t *enc, const void *info);

    /**
     * @brief Take encoder lock
     * @param enc Encoder instance
     * @param tmo Timeout in microseconds
     * @return Lock state (for recursive locking) or 0 on failure
     */
    unsigned int (*take_lock)(esp_trace_encoder_t *enc, uint32_t tmo_us);

    /**
     * @brief Give encoder lock
     * @param enc Encoder instance
     * @param int_state The interrupt state
     */
    void (*give_lock)(esp_trace_encoder_t *enc, unsigned int_state);

} esp_trace_encoder_vtable_t;

/**
 * @brief Encoder structure, represents an active trace library instance.
 */
struct esp_trace_encoder {
    const esp_trace_encoder_vtable_t *vt;  ///< Pointer to encoder vtable
    esp_trace_transport_t *tp;             ///< Pointer to transport instance
    void *ctx;                             ///< Encoder specific context
};

#ifdef __cplusplus
}
#endif
