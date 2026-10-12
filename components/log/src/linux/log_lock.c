/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <pthread.h>
#include <assert.h>
#include "esp_private/log_lock.h"

static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void esp_log_impl_lock(void)
{
    int result = pthread_mutex_lock(&mutex1);
    assert(result == 0);
    (void) result;
}

bool esp_log_impl_lock_timeout(void)
{
    esp_log_impl_lock();
    return true;
}

void esp_log_impl_unlock(void)
{
    int result = pthread_mutex_unlock(&mutex1);
    assert(result == 0);
    (void) result;
}
