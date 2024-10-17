/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <time.h>
#include "esp_log_timestamp.h"

uint32_t esp_log_early_timestamp(void)
{
    struct timespec current_time;
    int result = clock_gettime(CLOCK_MONOTONIC, &current_time);
    assert(result == 0);
    uint32_t milliseconds = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
    return milliseconds;
}

// Avoid using __attribute(alias) here since linux target builds on MacOS fail to compile.
uint32_t esp_log_timestamp(void)
{
    return esp_log_early_timestamp();
}
