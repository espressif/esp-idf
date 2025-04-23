/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_log_config.h"
#include "log_message.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Format log message.
 *
 * @param message Pointer to log message structure.
 */
void esp_log_format(esp_log_msg_t *message);

#if ESP_LOG_MODE_BINARY_EN
/**
 * @brief Format log message in binary mode.
 *
 * @param message Pointer to log message structure.
 */
void esp_log_format_binary(esp_log_msg_t *message);
#endif // ESP_LOG_MODE_BINARY_EN

#ifdef __cplusplus
}
#endif
