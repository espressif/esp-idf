/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
#include "esp_log_private.h"

static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void esp_log_impl_lock(void)
{
    assert(pthread_mutex_lock(&mutex1) == 0);
}

bool esp_log_impl_lock_timeout(void)
{
    esp_log_impl_lock();
    return true;
}

void esp_log_impl_unlock(void)
{
    assert(pthread_mutex_unlock(&mutex1) == 0);
}

uint32_t esp_log_timestamp(void)
{
    struct timespec current_time;
    int result = clock_gettime(CLOCK_MONOTONIC, &current_time);
    assert(result == 0);
    uint32_t milliseconds = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
    return milliseconds;
}
