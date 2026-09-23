/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_trace_types.h"

/* forward declarations */
typedef struct esp_trace_encoder esp_trace_encoder_t;
typedef struct esp_trace_transport esp_trace_transport_t;

/**
 * @brief Encoder Virtual Table
 *
 * Defines the interface for trace encoders (libraries).
 *
 * @warning Runtime callbacks (write, flush, take_lock, give_lock, panic_handler)
 *          must not call FreeRTOS / IDF APIs that themselves trigger trace hooks
 *          (e.g. vTaskDelay, xQueue*, xSemaphore*) — doing so re-enters the
 *          tracing path and can deadlock on the encoder lock or crash in ISR
 *          context.
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

    /** @brief Resume trace event emission */
    esp_err_t (*start)(esp_trace_encoder_t *enc);

    /** @brief Pause trace event emission */
    esp_err_t (*stop)(esp_trace_encoder_t *enc);

    /** @brief Flush pending trace data through the encoder */
    esp_err_t (*flush)(esp_trace_encoder_t *enc);

    /**
     * @brief Take encoder lock.
     *        Callers should pass ESP_TRACE_TMO_INFINITE unless they explicitly
     *        check the return value — pairing a failed take with give_lock()
     *        causes a spinlock owner-mismatch assert.
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

    /*
     * Optional function-trace callbacks. Leave NULL if unsupported.
     *
     * Capability contract: an encoder supports function tracing when both
     * function_enter and function_exit are set. The runtime checks these once at
     * start and returns ESP_ERR_NOT_SUPPORTED otherwise. Callbacks receive raw
     * addresses. The backend stays free to choose its own payload encoding.
     */

    /**
     * @brief Send a function entry event
     * @param enc Encoder instance
     * @param func Start address of the entered function
     * @param call_site Return address in the caller
     */
    void (*function_enter)(esp_trace_encoder_t *enc, void *func, void *call_site);

    /**
     * @brief Send a function exit event
     * @param enc Encoder instance
     * @param func Start address of the exited function
     * @param call_site Return address in the caller
     */
    void (*function_exit)(esp_trace_encoder_t *enc, void *func, void *call_site);

} esp_trace_encoder_vtable_t;

/**
 * @brief Encoder structure, represents an active trace library instance.
 */
struct esp_trace_encoder {
    const esp_trace_encoder_vtable_t *vt;  ///< Pointer to encoder vtable
    esp_trace_transport_t *tp;             ///< Pointer to transport instance
    void *ctx;                             ///< Encoder specific context
};

/**
 * @brief Report a change in the encoder's recording state.
 *
 * Encoders whose recording is controlled by the host (e.g. SystemView start/stop
 * over JTAG or UART) call this so dependent features such as function tracing can
 * follow the actual recording state.
 *
 * @param active true if the encoder is now recording, false otherwise.
 */
void esp_trace_notify_recording_state(bool active);

#ifdef __cplusplus
}
#endif
