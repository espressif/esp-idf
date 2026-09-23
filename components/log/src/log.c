/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "esp_log_config.h"
#include "esp_log_level.h"
#include "esp_log_timestamp.h"
#include "esp_private/log_level.h"
#include "esp_private/log_timestamp.h"
#include "esp_private/log_lock.h"
#include "esp_private/log_util.h"
#include "esp_private/log_print.h"
#include "esp_log_message.h"
#include "esp_private/log_format.h"
#include "esp_log_write.h"
#include "esp_rom_sys.h"
#include "sdkconfig.h"

#if !BOOTLOADER_BUILD && !defined(NON_OS_BUILD)
/* A weak reference does not pull the optional handler state into ordinary apps. */
#if ESP_LOG_VERSION == 1
extern void esp_log_handler_process_v1(esp_log_config_t config, const char *tag, const char *format, va_list args) __attribute__((weak));
#else
extern void esp_log_handler_process(esp_log_msg_t *message) __attribute__((weak));
#endif
#endif

static __attribute__((unused)) bool is_level_loggable(esp_log_config_t config)
{
#if BOOTLOADER_BUILD
    return (config.opts.log_level != ESP_LOG_NONE) && (esp_log_get_default_level() >= config.opts.log_level);
#else
    return (config.opts.log_level != ESP_LOG_NONE)
#if CONFIG_LOG_MASTER_LEVEL
           && (esp_log_get_level_master() >= config.opts.log_level)
#endif // CONFIG_LOG_MASTER_LEVEL
           && ((config.opts.constrained_env) ? (esp_log_get_default_level() >= config.opts.log_level) : true);
#endif
}

void __attribute__((optimize("-O3"))) esp_log_va(esp_log_config_t config, const char *tag, const char *format, va_list args)
{
#if ESP_LOG_VERSION == 1
    if (config.opts.log_level != ESP_LOG_NONE && esp_log_is_tag_loggable(config.opts.log_level, tag)) {
#if !BOOTLOADER_BUILD && !defined(NON_OS_BUILD)
        if (esp_log_handler_process_v1 != NULL) {
            esp_log_handler_process_v1(config, tag, format, args);
            return;
        }
#endif
        extern vprintf_like_t esp_log_vprint_func;
        esp_log_vprint_func(format, args);
    }
#else // ESP_LOG_VERSION == 2
    if (is_level_loggable(config)) {
        __attribute__((unused)) uint64_t timestamp = 0;
        bool timestamp_valid = false;
        config.opts.dis_timestamp |= !ESP_LOG_SUPPORT_TIMESTAMP;
        config.opts.constrained_env = ESP_LOG_CONSTRAINED_ENV || config.opts.constrained_env || esp_log_util_is_constrained();
        if (!config.opts.dis_timestamp && ((!ESP_LOG_CONSTRAINED_ENV && config.opts.require_formatting) || ESP_LOG_CONSTRAINED_ENV)) {
            timestamp = esp_log_timestamp64(config.opts.constrained_env);
            timestamp_valid = true;
        }
#if !ESP_LOG_CONSTRAINED_ENV
        if (!config.opts.constrained_env && tag != NULL && !esp_log_is_tag_loggable(config.opts.log_level, tag)) {
            return;
        }
#endif
        esp_log_msg_t message = {
            .config = config,
            .tag = tag,
            .format = format,
            .timestamp = timestamp,
            .timestamp_valid = timestamp_valid,
            .arg_types = NULL,
        };
        va_copy(message.args, args);
#if ESP_LOG_MODE_BINARY_EN
        if (config.opts.binary_mode) {
            message.arg_types = va_arg(message.args, const char *);
        }
#endif // ESP_LOG_MODE_BINARY_EN
#if !BOOTLOADER_BUILD && !defined(NON_OS_BUILD)
        if (!config.opts.constrained_env && esp_log_handler_process != NULL) {
            esp_log_handler_process(&message);
        } else
#endif
        {
#if ESP_LOG_MODE_BINARY_EN
            esp_log_format_binary(&message);
#else
            esp_log_format(&message);
#endif
        }
        va_end(message.args);
    }
#endif // ESP_LOG_VERSION == 2
}

void __attribute__((optimize("-O3"))) esp_log(esp_log_config_t config, const char* tag, const char* format, ...)
{
#if ESP_LOG_VERSION == 1
    if (1) {
#else // ESP_LOG_VERSION == 2
    if (is_level_loggable(config)) {
#endif
        va_list args = {0};
        va_start(args, format);
        esp_log_va(config, tag, format, args);
        va_end(args);
    }
}
