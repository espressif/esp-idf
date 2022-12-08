/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <reent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/lock.h>

#include "esp_system.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_private/system_internal.h"

#include "soc/rtc.h"

#include "esp_time_impl.h"

#include "sdkconfig.h"

#if !CONFIG_ESP_TIME_FUNCS_USE_NONE
#define IMPL_NEWLIB_TIME_FUNCS 1
#endif

#if IMPL_NEWLIB_TIME_FUNCS
// stores the start time of the slew
static uint64_t s_adjtime_start_us;
// is how many microseconds total to slew
static int64_t  s_adjtime_total_correction_us;

static _lock_t s_time_lock;

// This function gradually changes boot_time to the correction value and immediately updates it.
static uint64_t adjust_boot_time(void)
{
    #define ADJTIME_CORRECTION_FACTOR 6

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


// Get the adjusted boot time.
static uint64_t get_adjusted_boot_time(void)
{
    _lock_acquire(&s_time_lock);
    uint64_t adjust_time = adjust_boot_time();
    _lock_release(&s_time_lock);
    return adjust_time;
}

// Applying the accumulated correction to base_time and stopping the smooth time adjustment.
static void adjtime_corr_stop (void)
{
    _lock_acquire(&s_time_lock);
    if (s_adjtime_start_us != 0){
        adjust_boot_time();
        s_adjtime_start_us = 0;
    }
    _lock_release(&s_time_lock);
}
#endif

int adjtime(const struct timeval *delta, struct timeval *outdelta)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if(outdelta != NULL){
        _lock_acquire(&s_time_lock);
        adjust_boot_time();
        if (s_adjtime_start_us != 0) {
            outdelta->tv_sec    = s_adjtime_total_correction_us / 1000000L;
            outdelta->tv_usec   = s_adjtime_total_correction_us % 1000000L;
        } else {
            outdelta->tv_sec    = 0;
            outdelta->tv_usec   = 0;
        }
        _lock_release(&s_time_lock);
    }
    if(delta != NULL){
        int64_t sec  = delta->tv_sec;
        int64_t usec = delta->tv_usec;
        if(llabs(sec) > ((INT_MAX / 1000000L) - 1L)) {
            errno = EINVAL;
            return -1;
        }
        /*
        * If adjusting the system clock by adjtime () is already done during the second call adjtime (),
        * and the delta of the second call is not NULL, the earlier tuning is stopped,
        * but the already completed part of the adjustment is not canceled.
        */
        _lock_acquire(&s_time_lock);
        // If correction is already in progress (s_adjtime_start_time_us != 0), then apply accumulated corrections.
        adjust_boot_time();
        s_adjtime_start_us = esp_time_impl_get_time_since_boot();
        s_adjtime_total_correction_us = sec * 1000000L + usec;
        _lock_release(&s_time_lock);
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

clock_t IRAM_ATTR _times_r(struct _reent *r, struct tms *ptms)
{
    clock_t t = xTaskGetTickCount() * (portTICK_PERIOD_MS * CLK_TCK / 1000);
    ptms->tms_cstime = 0;
    ptms->tms_cutime = 0;
    ptms->tms_stime = t;
    ptms->tms_utime = 0;
    struct timeval tv = {0, 0};
    _gettimeofday_r(r, &tv, NULL);
    return (clock_t) tv.tv_sec;
}

int IRAM_ATTR _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
    (void) tz;

#if IMPL_NEWLIB_TIME_FUNCS
    if (tv) {
        uint64_t microseconds = get_adjusted_boot_time() + esp_time_impl_get_time_since_boot();
        tv->tv_sec = microseconds / 1000000;
        tv->tv_usec = microseconds % 1000000;
    }
    return 0;
#else
    __errno_r(r) = ENOSYS;
    return -1;
#endif
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    (void) tz;
#if IMPL_NEWLIB_TIME_FUNCS
    if (tv) {
        adjtime_corr_stop();
        uint64_t now = ((uint64_t) tv->tv_sec) * 1000000LL + tv->tv_usec;
        uint64_t since_boot = esp_time_impl_get_time_since_boot();
        esp_time_impl_set_boot_time(now - since_boot);
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int usleep(useconds_t us)
{
    const int us_per_tick = portTICK_PERIOD_MS * 1000;
    if (us < us_per_tick) {
        esp_rom_delay_us((uint32_t) us);
    } else {
        /* since vTaskDelay(1) blocks for anywhere between 0 and portTICK_PERIOD_MS,
         * round up to compensate.
         */
        vTaskDelay((us + us_per_tick - 1) / us_per_tick);
    }
    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    usleep(seconds*1000000UL);
    return 0;
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }
    struct timeval tv;
    switch (clock_id) {
        case CLOCK_REALTIME:
            tv.tv_sec = tp->tv_sec;
            tv.tv_usec = tp->tv_nsec / 1000L;
            settimeofday(&tv, NULL);
            break;
        default:
            errno = EINVAL;
            return -1;
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int clock_gettime (clockid_t clock_id, struct timespec *tp)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }
    struct timeval tv;
    uint64_t monotonic_time_us = 0;
    switch (clock_id) {
        case CLOCK_REALTIME:
            _gettimeofday_r(NULL, &tv, NULL);
            tp->tv_sec = tv.tv_sec;
            tp->tv_nsec = tv.tv_usec * 1000L;
            break;
        case CLOCK_MONOTONIC:
            monotonic_time_us = esp_time_impl_get_time();
            tp->tv_sec = monotonic_time_us / 1000000LL;
            tp->tv_nsec = (monotonic_time_us % 1000000LL) * 1000L;
            break;
        default:
            errno = EINVAL;
            return -1;
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int clock_getres (clockid_t clock_id, struct timespec *res)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if (res == NULL) {
        errno = EINVAL;
        return -1;
    }

    res->tv_sec = 0;
    res->tv_nsec = esp_system_get_time_resolution();

    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

void esp_newlib_time_init(void)
{
    esp_time_impl_init();
}
