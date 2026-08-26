/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#if CONFIG_IDF_TARGET_LINUX
#include <sys/time.h>
#include <time.h>
#else
#include "esp_timer.h"
#endif

uint64_t esp_tls_get_platform_time(void)
{
#if CONFIG_IDF_TARGET_LINUX
    // Use gettimeofday for Linux/MacOS, Ideally esp_timer should be used but it is not implemented for Linux/MacOS.
    struct timeval time = {};
    gettimeofday(&time, NULL);
    uint64_t curr_time = ((uint64_t)time.tv_sec * 1000000) + (time.tv_usec);
    return curr_time;
#else
    // For all other esp targets use esp_timer
    return esp_timer_get_time();
#endif
}
