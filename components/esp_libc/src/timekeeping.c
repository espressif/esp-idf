/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if !CONFIG_ESP_TIME_FUNCS_USE_NONE
#define IMPL_NEWLIB_TIME_FUNCS 1
#endif

#if IMPL_NEWLIB_TIME_FUNCS

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/lock.h>

#include "esp_time_impl.h"
#include "esp_libc_timekeeping.h"

/* Slew state for realtime adjtime (all in microseconds). */
static uint64_t s_adjtime_start_us;
static int64_t s_adjtime_total_correction_us;
static _lock_t s_adjtime_lock;

#define ADJTIME_CORRECTION_FACTOR 6

static uint64_t esp_libc_timekeeping_adjust_boot_time(void)
{
    uint64_t boot_time = esp_time_impl_get_boot_time();
    if ((boot_time == 0) || (esp_time_impl_get_time_since_boot() < s_adjtime_start_us)) {
        s_adjtime_start_us = 0;
    }
    if (s_adjtime_start_us > 0) {
        uint64_t since_boot = esp_time_impl_get_time_since_boot();
        // If to call this function once per second, then (since_boot - s_adjtime_start_us) will be 1_000_000 (1 second),
        // and the correction will be equal to (1_000_000us >> 6) = 15_625 us.
        // The minimum possible correction step can be (64us >> 6) = 1us.
        // Example: if the time error is 1 second, then it will be compensate for 1 sec / 0,015625 = 64 seconds.
        int64_t correction = (since_boot >> ADJTIME_CORRECTION_FACTOR) - (s_adjtime_start_us >> ADJTIME_CORRECTION_FACTOR);
        if (correction > 0) {
            s_adjtime_start_us = since_boot;
            if (s_adjtime_total_correction_us < 0) {
                if ((s_adjtime_total_correction_us + correction) >= 0) {
                    boot_time = boot_time + s_adjtime_total_correction_us;
                    s_adjtime_start_us = 0;
                } else {
                    s_adjtime_total_correction_us += correction;
                    boot_time -= correction;
                }
            } else {
                if ((s_adjtime_total_correction_us - correction) <= 0) {
                    boot_time = boot_time + s_adjtime_total_correction_us;
                    s_adjtime_start_us = 0;
                } else {
                    s_adjtime_total_correction_us -= correction;
                    boot_time += correction;
                }
            }
            esp_time_impl_set_boot_time(boot_time);
        }
    }
    return boot_time;
}

static uint64_t esp_libc_timekeeping_get_adjusted_boot_time(void)
{
    _lock_acquire(&s_adjtime_lock);
    uint64_t adjust_time = esp_libc_timekeeping_adjust_boot_time();
    _lock_release(&s_adjtime_lock);
    return adjust_time;
}

static void esp_libc_timekeeping_adjtime_corr_stop(void)
{
    _lock_acquire(&s_adjtime_lock);
    if (s_adjtime_start_us != 0) {
        esp_libc_timekeeping_adjust_boot_time();
        s_adjtime_start_us = 0;
    }
    _lock_release(&s_adjtime_lock);
}

uint64_t esp_libc_timekeeping_get_realtime_us(void)
{
    return esp_libc_timekeeping_get_adjusted_boot_time() + esp_time_impl_get_time_since_boot();
}

void esp_libc_timekeeping_set_realtime_us(uint64_t us)
{
    esp_libc_timekeeping_adjtime_corr_stop();
    uint64_t since_boot = esp_time_impl_get_time_since_boot();
    esp_time_impl_set_boot_time(us - since_boot);
}

int64_t esp_libc_timekeeping_adjtime_get_remaining_us(void)
{
    int64_t remaining_us;
    _lock_acquire(&s_adjtime_lock);
    esp_libc_timekeeping_adjust_boot_time();
    remaining_us = (s_adjtime_start_us != 0) ? s_adjtime_total_correction_us : 0;
    _lock_release(&s_adjtime_lock);
    return remaining_us;
}

int esp_libc_timekeeping_adjtime_apply(int64_t offset_us, int64_t *prev_remaining_us)
{
    if (prev_remaining_us == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (llabs(offset_us) > ((INT_MAX / 1000000L) - 1L) * 1000000L) {
        errno = EINVAL;
        return -1;
    }
    /*
    * If a new correction is started while one is already in progress,
    * the earlier tuning is stopped but the already completed part is
    * not canceled. The previous remaining correction is returned.
    */
    _lock_acquire(&s_adjtime_lock);
    *prev_remaining_us = (s_adjtime_start_us != 0) ? s_adjtime_total_correction_us : 0;
    esp_libc_timekeeping_adjust_boot_time();
    s_adjtime_start_us = esp_time_impl_get_time_since_boot();
    s_adjtime_total_correction_us = offset_us;
    _lock_release(&s_adjtime_lock);
    return 0;
}

#endif /* IMPL_NEWLIB_TIME_FUNCS */
