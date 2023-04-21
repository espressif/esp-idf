/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdarg.h>
#include <sdkconfig.h>
#include "SEGGER_SYSVIEW_Int.h"
#include "freertos/FreeRTOS.h"

static portMUX_TYPE s_log_mutex = portMUX_INITIALIZER_UNLOCKED;

int esp_sysview_vprintf(const char * format, va_list args)
{
    static char log_buffer[SEGGER_SYSVIEW_MAX_STRING_LEN];

    portENTER_CRITICAL(&s_log_mutex);
    size_t len = vsnprintf(log_buffer, sizeof(log_buffer), format, args);
    if (len > sizeof(log_buffer) - 1) {
        log_buffer[sizeof(log_buffer) - 1] = 0;
    }
    SEGGER_SYSVIEW_Print(log_buffer);
    portEXIT_CRITICAL(&s_log_mutex);
    return len;
}
