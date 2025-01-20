/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stdio.h>
#include "esp_private/log_print.h"
#include "esp_log_config.h"
#include "sdkconfig.h"

void esp_log_printf(esp_log_config_t config, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    esp_log_vprintf(config, format, args);
    va_end(args);
}
