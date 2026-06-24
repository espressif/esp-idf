/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <time.h>

void ref_clock_init(void)
{

}

void ref_clock_deinit(void)
{

}

uint64_t ref_clock_get(void)
{
    struct timespec current_time;
    assert(clock_gettime(CLOCK_MONOTONIC, &current_time) == 0);

    uint64_t ref_ticks = current_time.tv_sec * 1000000;
    ref_ticks += (current_time.tv_nsec / 1000);
    return ref_ticks;
}
