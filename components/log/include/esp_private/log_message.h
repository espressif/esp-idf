/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdarg.h>
#include "esp_log_config.h"
#include "esp_log_level.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure to hold log message details.
 */
typedef struct {
    esp_log_config_t config; /**< Log configuration */
    const char *tag;         /**< Log tag */
    const char *format;      /**< Log format string */
    uint64_t timestamp;      /**< Log timestamp */
    const char *arg_types;   /**< Log argument types */
    va_list args;            /**< Log arguments */
} esp_log_msg_t;

#ifdef __cplusplus
}
#endif
