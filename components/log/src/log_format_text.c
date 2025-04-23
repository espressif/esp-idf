/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/log_message.h"
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

void esp_log_format(esp_log_msg_t *message)
{
    if (message->config.opts.require_formatting) {
#if !ESP_LOG_CONSTRAINED_ENV
        if (!message->config.opts.constrained_env) {
#if CONFIG_LIBC_NEWLIB
            flockfile(stdout);
#endif
        }
#endif
        message->config.opts.dis_color |= !ESP_LOG_SUPPORT_COLOR || (s_lvl_color[message->config.opts.log_level][0] == '\0');
        char timestamp_buffer[32] = { 0 };
        if (!message->config.opts.dis_timestamp) {
            esp_log_timestamp_str(message->config.opts.constrained_env, message->timestamp, timestamp_buffer);
        }
        esp_log_printf(message->config, "%s%c %s%s%s%s%s",
                       (!message->config.opts.dis_color) ? s_lvl_color[message->config.opts.log_level] : "",
                       s_lvl_name[message->config.opts.log_level],
                       (!message->config.opts.dis_timestamp) ? "(" : "",
                       timestamp_buffer,
                       (!message->config.opts.dis_timestamp) ? ") " : "",
                       (message->tag) ? message->tag : "",
                       (message->tag) ? ": " : "");
    }

    esp_log_vprintf(message->config, message->format, message->args);

    if (message->config.opts.require_formatting) {
        esp_log_printf(message->config, "%s", (message->config.opts.dis_color) ? "\n" : LOG_RESET_COLOR"\n");
#if !ESP_LOG_CONSTRAINED_ENV
        if (!message->config.opts.constrained_env) {
#if CONFIG_LIBC_NEWLIB
            funlockfile(stdout);
#endif
        }
#endif
    }
}
