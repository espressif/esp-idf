// Copyright 2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
        ESP_EARLY_LOGI(tag, "%s", temp);
    }
    va_end(arg);
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
    // librtc.a printf temporary disabled due to UART baud rate switching bug.
    return 0;
}

int wpa_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("wpa", format, arg);
    va_end(arg);
    return res;
}

int wpa2_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("wpa2", format, arg);
    va_end(arg);
    return res;
}

int wps_printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = lib_printf("wps", format, arg);
    va_end(arg);
    return res;
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
