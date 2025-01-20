/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include "esp_log_config.h"
#include "esp_log_level.h"
#include "esp_log_color.h"
#include "esp_log_timestamp.h"
#include "esp_private/log_level.h"
#include "esp_private/log_timestamp.h"
#include "esp_private/log_lock.h"
#include "esp_private/log_util.h"
#include "esp_private/log_print.h"
#include "sdkconfig.h"

static __attribute__((unused)) const char s_lvl_name[ESP_LOG_MAX] = {
    '\0', // NONE
    'E',  // ERROR
    'W',  // WARNING
    'I',  // INFO
    'D',  // DEBUG
    'V',  // VERBOSE
};

static __attribute__((unused)) const char s_lvl_color[ESP_LOG_MAX][8] = {
    "\0",                                               // NONE
    LOG_ANSI_COLOR_REGULAR(LOG_ANSI_COLOR_RED)"\0",     // ERROR
    LOG_ANSI_COLOR_REGULAR(LOG_ANSI_COLOR_YELLOW)"\0",  // WARNING
    LOG_ANSI_COLOR_REGULAR(LOG_ANSI_COLOR_GREEN)"\0",   // INFO
    "\0",                                               // DEBUG
    "\0",                                               // VERBOSE
};

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
        extern vprintf_like_t esp_log_vprint_func;
        esp_log_vprint_func(format, args);
    }
#else // ESP_LOG_VERSION == 2
    if (is_level_loggable(config)) {
        __attribute__((unused)) uint64_t timestamp = 0;
        config.opts.dis_timestamp |= !ESP_LOG_SUPPORT_TIMESTAMP;
        config.opts.constrained_env = ESP_LOG_CONSTRAINED_ENV || config.opts.constrained_env || esp_log_util_is_constrained();
        if (!config.opts.dis_timestamp && ((!ESP_LOG_CONSTRAINED_ENV && config.opts.require_formatting) || ESP_LOG_CONSTRAINED_ENV)) {
            timestamp = esp_log_timestamp64(config.opts.constrained_env);
        }
#if !ESP_LOG_CONSTRAINED_ENV
        if (!config.opts.constrained_env && tag != NULL && !esp_log_is_tag_loggable(config.opts.log_level, tag)) {
            return;
        }
#endif
        // formatting log
        if (config.opts.require_formatting) { // 1. print "<color_start><level_name> <(time)> <tag>: "
#if !ESP_LOG_CONSTRAINED_ENV
            if (!config.opts.constrained_env) {
                // flockfile&funlockfile are used here to prevent other threads
                // from writing to the same stream simultaneously using printf-like functions.
                // Below is formatting log, there are multiple calls to vprintf to log a single message.
                flockfile(stdout);
            }
#endif
            config.opts.dis_color = !ESP_LOG_SUPPORT_COLOR || config.opts.dis_color || (s_lvl_color[config.opts.log_level][0] == '\0');
            char timestamp_buffer[32] = { 0 };
            if (!config.opts.dis_timestamp) {
                esp_log_timestamp_str(config.opts.constrained_env, timestamp, timestamp_buffer);
            }
            esp_log_printf(config, "%s%c %s%s%s%s%s",
                           (!config.opts.dis_color) ? s_lvl_color[config.opts.log_level] : "",
                           s_lvl_name[config.opts.log_level],
                           (!config.opts.dis_timestamp) ? "(" : "",
                           timestamp_buffer,
                           (!config.opts.dis_timestamp) ? ") " : "",
                           (tag) ? tag : "",
                           (tag) ? ": " : "");
        }

        esp_log_vprintf(config, format, args); // 2. print user message

        if (config.opts.require_formatting) { // 3. print "<color_end><\n>"
            esp_log_printf(config, "%s", (config.opts.dis_color) ? "\n" : LOG_RESET_COLOR"\n");
#if !ESP_LOG_CONSTRAINED_ENV
            if (!config.opts.constrained_env) {
                funlockfile(stdout);
            }
#endif
        }
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
        va_list args;
        va_start(args, format);
        esp_log_va(config, tag, format, args);
        va_end(args);
    }
}
