/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_compiler.h"
#include "esp_log_handler.h"
#include "esp_private/log_handler.h"
#if CONFIG_IDF_TARGET_LINUX
#include <pthread.h>
static pthread_mutex_t s_handler_lock = PTHREAD_MUTEX_INITIALIZER;
#define HANDLER_ENTER_CRITICAL() pthread_mutex_lock(&s_handler_lock)
#define HANDLER_EXIT_CRITICAL() pthread_mutex_unlock(&s_handler_lock)
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static portMUX_TYPE s_handler_lock = portMUX_INITIALIZER_UNLOCKED;
#define HANDLER_ENTER_CRITICAL() portENTER_CRITICAL(&s_handler_lock)
#define HANDLER_EXIT_CRITICAL() portEXIT_CRITICAL(&s_handler_lock)
#endif

static esp_log_handler_t s_handler;
static _Thread_local bool s_handler_recursion_guard;

esp_err_t esp_log_set_handler(esp_log_handler_fn_t handler, void *ctx, esp_log_handler_t *previous)
{
    HANDLER_ENTER_CRITICAL();
    if (previous != NULL) {
        *previous = s_handler;
    }
    s_handler = (esp_log_handler_t) {
        .fn = handler, .ctx = handler ? ctx : NULL
    };
    HANDLER_EXIT_CRITICAL();
    return ESP_OK;
}

void esp_log_handler_process(esp_log_msg_t *message)
{
    // Keep handler synchronization independent of allocations under the tag-level lock.
    HANDLER_ENTER_CRITICAL();
    esp_log_handler_t handler = s_handler;
    HANDLER_EXIT_CRITICAL();

    if (handler.fn == NULL) {
        esp_log_output_default(message);
        return;
    }

    if (
#if !CONFIG_IDF_TARGET_LINUX
        // TLS may not be available before the scheduler starts.
        unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) ||
#endif
        s_handler_recursion_guard) {
        esp_log_output_default(message);
        return;
    }

    s_handler_recursion_guard = true;
    bool print = handler.fn(message, handler.ctx);
    s_handler_recursion_guard = false;
    if (print) {
        esp_log_output_default(message);
    }
}

#if ESP_LOG_VERSION == 1
void esp_log_handler_process_v1(esp_log_config_t config, const char *tag, const char *format, va_list args)
{
    esp_log_msg_t message = {
        .config = config,
        .tag = tag,
        .format = format,
    };
    va_copy(message.args, args);
    esp_log_handler_process(&message);
    va_end(message.args);
}
#endif
