/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "esp_log_config.h"
#include "esp_log_level.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Message available to a log handler.
 *
 * V1 macros include the log prefix and newline in format/args; V2 provides
 * formatting metadata separately. Direct write calls use the supplied format.
 *
 * For deferred processing, copy or encode the required data before returning.
 * Read args through va_copy()/va_end() if another handler or standard output
 * will also use the message.
 */
typedef struct {
    esp_log_config_t config; /**< Configuration, including the severity level. */
    const char *tag;         /**< Tag, or NULL. */
    const char *format;      /**< Format string; in binary mode this may be an ELF-only address, not readable text. */
    uint64_t timestamp;      /**< Timestamp; meaningful only when timestamp_valid is true. */
    bool timestamp_valid;   /**< Whether timestamp is available; always false in V1. */
    const char *arg_types;   /**< Binary argument metadata, otherwise NULL. */
    va_list args;           /**< Arguments matching format. */
} esp_log_msg_t;

#ifdef __cplusplus
}
#endif
