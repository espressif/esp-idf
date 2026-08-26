/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdarg.h>
#include "esp_log_level.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*vprintf_like_t)(const char *, va_list);

/**
 * @brief Set function used to output log entries
 *
 * By default, log output goes to UART0. This function can be used to redirect log
 * output to some other destination, such as file or network. Returns the original
 * log handler, which may be necessary to return output to the previous destination.
 *
 * @note Please note that function callback here must be re-entrant as it can be
 * invoked in parallel from multiple tasks context.
 *
 * @param func new Function used for output. Must have same signature as vprintf.
 *
 * @return func old Function used for output.
 */
vprintf_like_t esp_log_set_vprintf(vprintf_like_t func);

/**
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * ESP_LOGE, ESP_LOGW, ESP_LOGI, ESP_LOGD, ESP_LOGV macros.
 *
 * This function or these macros should not be used from an interrupt.
 *
 * This function does not add any formatting elements such as color, timestamp, or tag.
 * It checks the level and tag level. If logging is allowed then it outputs it as is.
 *
 * @param level     Log level of the message.
 * @param tag       It is used to check whether logging is enabled for that tag (depends on CONFIG_LOG_TAG_LEVEL_IMPL).
 * @param format    The format string for the log message. It has to be fully formatted, no additional formatting items will be added.
 * @param ...       Optional arguments to be formatted according to the format string.
 */
void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...) __attribute__((format(printf, 3, 4)));

/**
 * @brief Write message into the log, va_list variant
 * @see esp_log_write()
 *
 * This function is provided to ease integration toward other logging framework,
 * so that esp_log can be used as a log sink.
 *
 * This function does not add any formatting elements such as color, timestamp, or tag.
 * It checks the level and tag level. If logging is allowed then it outputs it as is.
 *
 * @param level     Log level of the message.
 * @param tag       It is used to check whether logging is enabled for that tag (depends on CONFIG_LOG_TAG_LEVEL_IMPL).
 * @param format    The format string for the log message. It has to be fully formatted, no additional formatting items will be added.
 * @param args      List of arguments.
 */
void esp_log_writev(esp_log_level_t level, const char* tag, const char* format, va_list args);

#ifdef __cplusplus
}
#endif
