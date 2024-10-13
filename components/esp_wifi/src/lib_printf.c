/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_log.h"
#include "esp_attr.h"

#define VPRINTF_STACK_BUFFER_SIZE 80

static int lib_printf(const char* tag, const char* format, va_list arg)
{
    char temp[VPRINTF_STACK_BUFFER_SIZE];
    int len = vsnprintf(temp, sizeof(temp) - 1, format, arg);
    temp[sizeof(temp) - 1] = 0;
    int i;
    for (i = len - 1; i >= 0; --i) {
        if (temp[i] != '\n' && temp[i] != '\r' && temp[i] != ' ') {
            break;
        }
        temp[i] = 0;
    }
    if (i > 0) {
        ESP_LOGI(tag, "%s", temp);
    }
    va_end(arg);
    return len;
}

int pp_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("pp", format, arg);
    va_end(arg);
    return res;
}

int sc_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("smartconfig", format, arg);
    va_end(arg);
    return res;
}

int core_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("core", format, arg);
    va_end(arg);
    return res;
}

int net80211_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("net80211", format, arg);
    va_end(arg);
    return res;
}

int target_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("target", format, arg);
    va_end(arg);
    return res;
}

int wapi_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("wapi", format, arg);
    va_end(arg);
    return res;
}

int mesh_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("mesh", format, arg);
    va_end(arg);
    return res;
}
