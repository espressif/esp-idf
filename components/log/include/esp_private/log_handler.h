/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_log_message.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Linked only when esp_log_set_handler() is used. Not an application override API. */
void esp_log_handler_process(esp_log_msg_t *message);
#if ESP_LOG_VERSION == 1
void esp_log_handler_process_v1(esp_log_config_t config, const char *tag, const char *format, va_list args);
#endif

/* Standard output without re-entering the handler or repeating level filtering. */
void esp_log_output_default(esp_log_msg_t *message);

#ifdef __cplusplus
}
#endif
