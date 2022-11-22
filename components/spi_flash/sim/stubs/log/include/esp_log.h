/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is a STUB FILE HEADER used when compiling ESP-IDF to run tests on the host system.
 * The header file used normally for ESP-IDF has the same name but is located elsewhere.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define heap_caps_malloc(a, b)  NULL
#define MALLOC_CAP_INTERNAL     0
#define MALLOC_CAP_8BIT         0

#define LOG_LOCAL_LEVEL         CONFIG_LOG_DEFAULT_LEVEL

typedef enum {
    ESP_LOG_NONE,       /*!< No log output */
    ESP_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    ESP_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    ESP_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    ESP_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ESP_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} esp_log_level_t;

#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_RESET_COLOR

#undef ESP_STATIC_ASSERT
#define ESP_STATIC_ASSERT(cond, message)

uint32_t esp_log_timestamp(void);
void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...) __attribute__ ((format (printf, 3, 4)));

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%d) %s: " format LOG_RESET_COLOR "\n"

#define ESP_LOGE( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_ERROR)   { esp_log_write(ESP_LOG_ERROR,   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#define ESP_LOGW( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_WARN)    { esp_log_write(ESP_LOG_WARN,    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#define ESP_LOGI( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO)    { esp_log_write(ESP_LOG_INFO,    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#define ESP_LOGD( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG)   { esp_log_write(ESP_LOG_DEBUG,   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#define ESP_LOGV( tag, format, ... )  if (LOG_LOCAL_LEVEL >= ESP_LOG_VERBOSE) { esp_log_write(ESP_LOG_VERBOSE, tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#define esp_flash_encryption_enabled() false

#ifdef __cplusplus
}
#endif
