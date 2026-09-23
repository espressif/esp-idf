/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_log_message.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Process a log message that passed level filtering.
 *
 * Return true to let the library print the message after your callback returns.
 * Return false to skip that output, for example if you already handled the message.
 *
 * To pass the message to a previous handler, use
 * `return previous.fn(message, previous.ctx);` when previous.fn is not NULL.
 *
 * The callback runs in the calling task and must support concurrent calls.
 * Logs emitted inside it bypass interception and use standard output.
 * Early, DRAM and bootloader logs bypass interception, as do constrained logs
 * in V2. In V1, use ordinary logs only in normal execution contexts.
 *
 * @param message Message valid during this call; see esp_log_msg_t for argument handling.
 * @param ctx Context supplied to esp_log_set_handler().
 * @return true to continue standard output, false to stop processing.
 */
typedef bool (*esp_log_handler_fn_t)(esp_log_msg_t *message, void *ctx);

/** @brief Saved callback and context for forwarding or restoring log handling. */
typedef struct {
    esp_log_handler_fn_t fn; /**< Callback, or NULL if none was installed. */
    void *ctx;              /**< Application-owned callback context. */
} esp_log_handler_t;

/**
 * @brief Set the application-wide message handler for Log V1 or V2.
 *
 * Use a handler to save, redirect or suppress selected logs. Pass NULL to restore
 * standard output. Call from normal task context; unavailable in bootloader and
 * non-OS builds.
 *
 * Initialize ctx before installing the handler, which may run immediately.
 * Replacing a handler does not wait for its pending calls to finish. Keep its
 * context and resources valid until all calls and any saved wrappers are done
 * using them.
 *
 * To wrap an existing handler, store previous in the new context and forward
 * messages explicitly. It is filled before the new handler can run; use storage
 * that no other task or callback is accessing. Restore wrappers in reverse order.
 *
 * @param handler Callback, or NULL to disable interception.
 * @param ctx Application-owned context, or NULL; ignored when handler is NULL.
 * @param[out] previous Replaced callback and context, or NULL if not needed.
 * @return ESP_OK.
 */
esp_err_t esp_log_set_handler(esp_log_handler_fn_t handler, void *ctx, esp_log_handler_t *previous);

#ifdef __cplusplus
}
#endif
