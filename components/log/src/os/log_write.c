/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "esp_log_write.h"
#include "esp_private/log_lock.h"
#include "esp_log_level.h"
#include "esp_log_config.h"
#include "esp_log.h"

vprintf_like_t esp_log_vprint_func = &vprintf;

vprintf_like_t esp_log_set_vprintf(vprintf_like_t func)
{
    /* This builtin, as described by Intel, is not a traditional
     * test-and-set operation, but rather an atomic exchange operation. It
     * writes value into *ptr, and returns the previous contents of *ptr.
     */
    return __atomic_exchange_n(&esp_log_vprint_func, func, __ATOMIC_SEQ_CST);
}

void esp_log_writev(esp_log_level_t level,
                    const char *tag,
                    const char *format,
                    va_list args)
{
    esp_log_va(ESP_LOG_CONFIG_INIT(level), tag, format, args);
}

void esp_log_write(esp_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    va_list list;
    va_start(list, format);
    esp_log_va(ESP_LOG_CONFIG_INIT(level), tag, format, list);
    va_end(list);
}
