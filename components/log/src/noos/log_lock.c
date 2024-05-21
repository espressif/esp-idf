/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include "esp_private/log_lock.h"

static int s_lock = 0;

void esp_log_impl_lock(void)
{
    assert(s_lock == 0);
    s_lock = 1;
}

bool esp_log_impl_lock_timeout(void)
{
    esp_log_impl_lock();
    return true;
}

void esp_log_impl_unlock(void)
{
    assert(s_lock == 1);
    s_lock = 0;
}
