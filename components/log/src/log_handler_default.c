/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/log_handler.h"
#include "esp_log_write.h"
#include "esp_private/log_format.h"

void esp_log_output_default(esp_log_msg_t *message)
{
#if ESP_LOG_VERSION == 1
    extern vprintf_like_t esp_log_vprint_func;
    vprintf_like_t vprintf_func = __atomic_load_n(&esp_log_vprint_func, __ATOMIC_SEQ_CST);
    vprintf_func(message->format, message->args);
#elif ESP_LOG_MODE_BINARY_EN
    esp_log_format_binary(message);
#else
    esp_log_format(message);
#endif
}
