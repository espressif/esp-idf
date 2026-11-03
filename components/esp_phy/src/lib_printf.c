/*
 * SPDX-FileCopyrightText: 2016-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file lib_printf.c
 *
 * This file contains library-specific printf functions
 * used by WiFi libraries in the `lib` directory.
 * These function are used to catch any output which gets printed
 * by libraries, and redirect it to ESP_LOG macros.
 *
 * Eventually WiFi libraries will use ESP_LOG functions internally
 * and these definitions will be removed.
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_attr.h"

#define VPRINTF_STACK_BUFFER_SIZE 80

static int lib_printf(const char* tag, const char* format, va_list arg)
{
    char temp[VPRINTF_STACK_BUFFER_SIZE];
    /* vsnprintf() appends a terminating '\0' when the size argument is greater than 0.
     * len returns the length required for the complete formatted output, which may exceed sizeof(temp). */
    int len = vsnprintf(temp, sizeof(temp), format, arg);
    if (len < 0) {
        len = 0;
    }
    if ((size_t)len >= sizeof(temp)) {
        len = sizeof(temp) - 1;
    }
    int i;
    for (i = len - 1; i >= 0; --i) {
        if (temp[i] != '\n' && temp[i] != '\r' && temp[i] != ' ') {
            break;
        }
        temp[i] = 0;
    }
    if (i >= 0) {
        ESP_LOGI(tag, "%s", temp);
    }
    return len;
}

int phy_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("phy", format, arg);
    va_end(arg);
    return res;
}


int rtc_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("rtc", format, arg);
    va_end(arg);
    return res;
}
