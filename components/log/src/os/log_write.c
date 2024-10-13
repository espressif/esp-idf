/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_private/log_lock.h"
#include "esp_private/log_level.h"
#include "sdkconfig.h"

static vprintf_like_t s_log_print_func = &vprintf;

vprintf_like_t esp_log_set_vprintf(vprintf_like_t func)
{
    esp_log_impl_lock();
    vprintf_like_t orig_func = s_log_print_func;
    s_log_print_func = func;
    esp_log_impl_unlock();
    return orig_func;
}

void esp_log_writev(esp_log_level_t level,
                    const char *tag,
                    const char *format,
                    va_list args)
{
    esp_log_level_t level_for_tag = esp_log_level_get_timeout(tag);
    if (ESP_LOG_NONE != level_for_tag && level <= level_for_tag) {
        (*s_log_print_func)(format, args);
    }
}

void esp_log_write(esp_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    va_list list;
    va_start(list, format);
    esp_log_writev(level, tag, format, list);
    va_end(list);
}
