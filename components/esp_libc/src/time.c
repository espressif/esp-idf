/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <reent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/times.h>

#include "esp_system.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_private/system_internal.h"

#include "soc/rtc.h"

#include "esp_time_impl.h"
#include "esp_libc_timekeeping.h"

#include "sdkconfig.h"
#include "esp_libc_clock.h"

#if !CONFIG_ESP_TIME_FUNCS_USE_NONE
#define IMPL_NEWLIB_TIME_FUNCS 1
#endif

#if IMPL_NEWLIB_TIME_FUNCS
extern const esp_libc_clock_desc_t _esp_libc_clock_array_start[];
extern const esp_libc_clock_desc_t _esp_libc_clock_array_end[];

// time functions are implemented -- they should not be weak
#define WEAK_UNLESS_TIMEFUNC_IMPL

/* Built-in CLOCK_REALTIME: thin wrappers calling esp_libc_timekeeping (µs) API; unit conversion here. */
static int realtime_gettime(struct timespec *tp, void *ctx)
{
    (void)ctx;
    uint64_t microseconds = esp_libc_timekeeping_get_realtime_us();
    tp->tv_sec = microseconds / 1000000;
    tp->tv_nsec = (microseconds % 1000000) * 1000L;
    return 0;
}

static int realtime_settime(const struct timespec *tp, void *ctx)
{
    (void)ctx;
    uint64_t us = ((uint64_t) tp->tv_sec) * 1000000LL + (tp->tv_nsec / 1000L);
    esp_libc_timekeeping_set_realtime_us(us);
    return 0;
}

static int realtime_adjtime(struct timex *buf, void *ctx)
{
    (void)ctx;
    bool offset_in_ns = (buf->modes & ADJ_NANO) == ADJ_NANO;
    if (buf->modes & ADJ_OFFSET) {
        if (buf->modes == ADJ_OFFSET_SS_READ) {
            // ADJ_OFFSET_SS_READ is always in microseconds
            buf->offset = (long)esp_libc_timekeeping_adjtime_get_remaining_us();
            return 0;
        }
        int64_t offset_us = offset_in_ns ? (int64_t)buf->offset / 1000 : (int64_t)buf->offset;
        int64_t prev_us;
        if (esp_libc_timekeeping_adjtime_apply(offset_us, &prev_us) != 0) {
            return -1;
        }
        buf->offset = offset_in_ns ? (long)(prev_us * 1000) : (long)prev_us;
        return 0;
    }
    if (buf->modes & ADJ_FREQUENCY) {
        errno = EOPNOTSUPP;
        return -1;
    }
    // if no known mode specified, just populate the timex structure with the current state
    buf->offset = (long)esp_libc_timekeeping_adjtime_get_remaining_us();
    buf->offset = offset_in_ns ? buf->offset * 1000 : buf->offset;
    buf->freq = 0;
    return 0;
}

static int realtime_getres(struct timespec *res, void *ctx)
{
    (void)ctx;
    res->tv_sec = 0;
    res->tv_nsec = esp_system_get_time_resolution();
    return 0;
}

/* Built-in CLOCK_MONOTONIC (time since boot) */
static int monotonic_gettime(struct timespec *tp, void *ctx)
{
    (void)ctx;
    uint64_t monotonic_time_us = esp_time_impl_get_time();
    tp->tv_sec = monotonic_time_us / 1000000LL;
    tp->tv_nsec = (monotonic_time_us % 1000000LL) * 1000L;
    return 0;
}

static int monotonic_getres(struct timespec *res, void *ctx)
{
    (void)ctx;
    res->tv_sec = 0;
    res->tv_nsec = esp_system_get_time_resolution();
    return 0;
}

static const esp_libc_clock_ops_t s_realtime_ops = {
    .gettime = realtime_gettime,
    .settime = realtime_settime,
    .adjtime = realtime_adjtime,
    .getres = realtime_getres
};

static const esp_libc_clock_ops_t s_monotonic_ops = {
    .gettime = monotonic_gettime,
    .settime = NULL,
    .adjtime = NULL,
    .getres = monotonic_getres
};

ESP_LIBC_CLOCK_REGISTER(_realtime, CLOCK_REALTIME, s_realtime_ops, NULL);
ESP_LIBC_CLOCK_REGISTER(_monotonic, CLOCK_MONOTONIC, s_monotonic_ops, NULL);

static void esp_clock_registry_check_duplicates(void)
{
    for (const esp_libc_clock_desc_t *it = _esp_libc_clock_array_start;
            it < _esp_libc_clock_array_end; ++it) {
        for (const esp_libc_clock_desc_t *jt = it + 1;
                jt < _esp_libc_clock_array_end; ++jt) {
            assert(it->clk_id != jt->clk_id);
        }
    }
}

static const esp_libc_clock_desc_t *esp_clock_find_entry(clockid_t clk_id)
{
    for (const esp_libc_clock_desc_t *it = _esp_libc_clock_array_start;
            it < _esp_libc_clock_array_end; ++it) {
        if (it->clk_id == clk_id) {
            return it;
        }
    }
    return NULL;
}
#else
// no time functions are actually implemented -- allow users to override them
#define WEAK_UNLESS_TIMEFUNC_IMPL __attribute__((weak))
#endif // IMPL_NEWLIB_TIME_FUNCS

int clock_adjtime(clockid_t clk_id, struct timex *buf)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if (buf == NULL) {
        errno = EINVAL;
        return -1;
    }

    const esp_libc_clock_desc_t *entry = esp_clock_find_entry(clk_id);
    if (entry == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (entry->ops.adjtime != NULL) {
        return entry->ops.adjtime(buf, entry->ctx);
    }
    errno = EOPNOTSUPP;
    return -1;
#else
    errno = ENOSYS;
    return -1;
#endif
}

WEAK_UNLESS_TIMEFUNC_IMPL int adjtime(const struct timeval *delta, struct timeval *outdelta)
{
    struct timex tx = {0};

    if (delta != NULL) {
        tx.modes = ADJ_OFFSET_SINGLESHOT;
        tx.offset = delta->tv_sec * 1000000L + delta->tv_usec;
    } else {
        tx.modes = ADJ_OFFSET_SS_READ;
    }

    int ret = realtime_adjtime(&tx, NULL);

    if (ret == 0 && outdelta != NULL) {
        outdelta->tv_sec  = tx.offset / 1000000L;
        outdelta->tv_usec = tx.offset % 1000000L;
    }

    return ret;
}

WEAK_UNLESS_TIMEFUNC_IMPL int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    (void) tz;
#if IMPL_NEWLIB_TIME_FUNCS
    if (tv) {
        struct timespec ts;
        ts.tv_sec = tv->tv_sec;
        ts.tv_nsec = tv->tv_usec * 1000L;
        return realtime_settime(&ts, NULL);
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

clock_t _times_r(struct _reent *r, struct tms *ptms)
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

WEAK_UNLESS_TIMEFUNC_IMPL int _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
    (void) tz;

#if IMPL_NEWLIB_TIME_FUNCS
    if (tv) {
        struct timespec ts;
        realtime_gettime(&ts, NULL);
        tv->tv_sec = ts.tv_sec;
        tv->tv_usec = ts.tv_nsec / 1000L;
    }
    return 0;
#else
    __errno_r(r) = ENOSYS;
    return -1;
#endif
}

WEAK_UNLESS_TIMEFUNC_IMPL int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }
    const esp_libc_clock_desc_t *entry = esp_clock_find_entry(clock_id);
    if (entry == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (entry->ops.settime != NULL) {
        return entry->ops.settime(tp, entry->ctx);
    }
    errno = EOPNOTSUPP;
    return -1;
#else
    errno = ENOSYS;
    return -1;
#endif
}

WEAK_UNLESS_TIMEFUNC_IMPL int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    const esp_libc_clock_desc_t *entry = esp_clock_find_entry(clock_id);
    if (entry == NULL) {
        errno = EINVAL;
        return -1;
    }
    return entry->ops.gettime(tp, entry->ctx);
#else
    errno = ENOSYS;
    return -1;
#endif
}

WEAK_UNLESS_TIMEFUNC_IMPL int clock_getres(clockid_t clock_id, struct timespec *res)
{
#if IMPL_NEWLIB_TIME_FUNCS
    if (res == NULL) {
        errno = EINVAL;
        return -1;
    }
    const esp_libc_clock_desc_t *entry = esp_clock_find_entry(clock_id);
    if (entry == NULL) {
        errno = EINVAL;
        return -1;
    }
    return entry->ops.getres(res, entry->ctx);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int usleep(useconds_t us)
{
    /* Even at max tick rate, vTaskDelay can still delay for the max of the us argument,
       we just need to make sure the tick calculation does not overflow
    */
    const int64_t us_per_tick = portTICK_PERIOD_MS * 1000;
    if (us < us_per_tick) {
        esp_rom_delay_us((uint32_t) us);
    } else {
        /* vTaskDelay may return up to (n-1) tick periods due to the tick ISR
           being asynchronous to the call. We must sleep at least the specified
           time, or longer. Checking the monotonic clock allows making an
           additional call to vTaskDelay when needed to ensure minimal time is
           actually slept. Adding `us_per_tick - 1` prevents ever passing 0 to
           vTaskDelay().
        */
        uint64_t now_us = esp_time_impl_get_time();
        uint64_t target_us = now_us + us;
        do {
            vTaskDelay((((target_us - now_us) + us_per_tick - 1) / us_per_tick));
            now_us = esp_time_impl_get_time();
            /* It is possible that the time left until the target time is less
             * than a tick period. However, we let usleep() to sleep for an
             * entire tick period. This, could result in usleep() sleeping for
             * a longer time than the requested time but that does not violate
             * the spec of usleep(). Additionally, it allows FreeRTOS to schedule
             * other tasks while the current task is sleeping.
             */
        } while (now_us < target_us);
    }
    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    usleep(seconds * 1000000UL);
    return 0;
}

/* TODO IDF-11226 */
void esp_newlib_time_init(void) __attribute__((alias("esp_libc_time_init")));
void esp_libc_time_init(void)
{
#if IMPL_NEWLIB_TIME_FUNCS
    esp_clock_registry_check_duplicates();
#endif
    esp_set_time_from_rtc();
}
