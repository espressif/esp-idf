/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is a STUB FILE used when compiling ESP-IDF to run tests on the host system.
 * The source file used normally for ESP-IDF has the same name but is located elsewhere.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "esp_log.h"

void esp_log_write(esp_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);
}

uint32_t esp_log_timestamp(void)
{
    return 0;
}
