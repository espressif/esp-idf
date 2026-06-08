/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <sys/prctl.h>

#include "sys/param.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_timer_impl.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "esp_timer_impl";

/*
 * Linux host backend for esp_timer.
 *
 * This implementation emulates a hardware alarm using Linux timerfd.
 *
 * The esp_timer time domain is:
 *
 *     esp_time = CLOCK_MONOTONIC + s_time_offset_us
 *
 * Therefore, when esp_timer asks us to arm an alarm at deadline T,
 * the corresponding CLOCK_MONOTONIC absolute deadline is:
 *
 *     mono_deadline = T - s_time_offset_us
 *
 * The alarm is programmed as an absolute CLOCK_MONOTONIC timerfd deadline
 * using timerfd_settime(..., TFD_TIMER_ABSTIME, ...).
 *
 * A dedicated pthread waits on:
 *
 *     - timerfd: alarm expiration
 *     - eventfd: shutdown notification
 *
 * When timerfd expires, the thread notifies the common esp_timer task with
 * xTaskNotifyGive(), preserving the common esp_timer callback flow.
 *
 * timerfd expiration -> alarm pthread wakeup -> xTaskNotifyGive() -> esp_timer task execution
 */

/* Alarm values to generate interrupt on match */
extern uint64_t timestamp_id[2];

/* Dedicated alarm thread used as the Linux "hardware timer". */
static pthread_t s_alarm_thread;
static bool s_alarm_thread_created;

/* Linux timer/event descriptors. */
static int s_timer_fd = -1;
static int s_shutdown_fd = -1;

/* Adjustable offset between CLOCK_MONOTONIC and esp_timer time. */
static _Atomic int64_t s_time_offset_us;

/* -------------------------------------------------------------------------- */
/* Time base                                                                  */
/* -------------------------------------------------------------------------- */

static int64_t get_monotonic_time_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000LL + ts.tv_nsec / 1000LL;
}

uint64_t esp_timer_impl_get_counter_reg(void)
{
    return (uint64_t) esp_timer_impl_get_time();
}

int64_t esp_timer_impl_get_time(void)
{
    return get_monotonic_time_us() + atomic_load_explicit(&s_time_offset_us, memory_order_relaxed);
}

int64_t esp_timer_get_time(void)
{
    return esp_timer_impl_get_time();
}

/* -------------------------------------------------------------------------- */
/* timerfd helpers                                                            */
/* -------------------------------------------------------------------------- */

static void us_to_timespec_abs(int64_t us, struct timespec *ts)
{
    if (us < 0) {
        us = 0;
    }

    ts->tv_sec = us / 1000000LL;
    ts->tv_nsec = (us % 1000000LL) * 1000LL;
}

static int64_t deadline_to_monotonic_us(uint64_t deadline_us)
{
    int64_t offset_us = atomic_load_explicit(&s_time_offset_us, memory_order_relaxed);
    return (int64_t)deadline_us - offset_us;
}

static esp_err_t program_timerfd(uint64_t deadline_us)
{
    struct itimerspec its = { 0 };

    if (s_timer_fd < 0) {
        return ESP_ERR_INVALID_STATE;
    }

    if (deadline_us == UINT64_MAX) {
        /*
         * Disarm timerfd.
         * For timerfd_settime(), zero it_value disarms the timer.
         */
        if (timerfd_settime(s_timer_fd, TFD_TIMER_ABSTIME, &its, NULL) != 0) {
            ESP_LOGE(TAG, "timerfd disarm failed: %s", strerror(errno));
            return ESP_FAIL;
        }

        return ESP_OK;
    }

    int64_t mono_deadline_us = deadline_to_monotonic_us(deadline_us);
    int64_t now_mono_us = get_monotonic_time_us();

    if (mono_deadline_us <= now_mono_us) {
        // Using now + 1 us avoids zero it_value, because zero disarms timerfd.
        mono_deadline_us = now_mono_us + 1;
    }

    us_to_timespec_abs(mono_deadline_us, &its.it_value);
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timerfd_settime(s_timer_fd, TFD_TIMER_ABSTIME, &its, NULL) != 0) {
        ESP_LOGE(TAG, "timerfd_settime failed: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

/* -------------------------------------------------------------------------- */
/* Alarm thread                                                               */
/* -------------------------------------------------------------------------- */

static void *alarm_thread_func(void *arg)
{
    (void)arg;

#ifdef PR_SET_TIMERSLACK
    // Set timer slack to 1 ns for this thread.
    // Linux coalesces nearby wakeups to save power; slack controls the allowed delay.
    // Default is 50 us. Setting it to 1 ns minimises timerfd wakeup jitter.
    prctl(PR_SET_TIMERSLACK, 1, 0, 0, 0);
#endif

    struct pollfd fds[2] = {
        {
            .fd = s_timer_fd,
            .events = POLLIN,
            .revents = 0,
        },
        {
            .fd = s_shutdown_fd,
            .events = POLLIN,
            .revents = 0,
        },
    };

    while (true) {
        int ret = poll(fds, 2, -1);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }

            ESP_LOGE(TAG, "alarm poll failed: %s", strerror(errno));
            continue;
        }

        if (fds[1].revents & POLLIN) {
            uint64_t val;
            ssize_t n = read(s_shutdown_fd, &val, sizeof(val));
            (void)n;
            break;
        }

        if (fds[0].revents & POLLIN) {
            uint64_t expirations;
            ssize_t n = read(s_timer_fd, &expirations, sizeof(expirations));
            if (n != sizeof(expirations)) {
                if (n < 0 && (errno == EINTR || errno == EAGAIN)) {
                    continue;
                }

                if (n < 0) {
                    ESP_LOGE(TAG, "timerfd read failed: %s", strerror(errno));
                } else {
                    ESP_LOGE(TAG, "timerfd short read: %zd", n);
                }
                continue;
            }

            TaskHandle_t timer_task = esp_timer_impl_get_timer_task_handle();
            if (timer_task != NULL) {
                xTaskNotifyGive(timer_task);
            }
        }
    }

    return NULL;
}

/* -------------------------------------------------------------------------- */
/* Alarm programming API                                                      */
/* -------------------------------------------------------------------------- */

void esp_timer_impl_set_alarm_id(uint64_t timestamp_us, unsigned alarm_id)
{
    esp_timer_impl_lock();

    if (alarm_id < 2) {
        timestamp_id[alarm_id] = timestamp_us;
    } else {
        ESP_LOGE(TAG, "Invalid alarm_id: %u", alarm_id);
        esp_timer_impl_unlock();
        return;
    }

    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);
    program_timerfd(min_alarm_us);

    esp_timer_impl_unlock();
}

void esp_timer_impl_set(uint64_t new_us)
{
    esp_timer_impl_lock();

    atomic_store_explicit(&s_time_offset_us, (int64_t)new_us - get_monotonic_time_us(), memory_order_relaxed);

    // Offset changed, so the same esp_timer deadline now maps to a different
    // CLOCK_MONOTONIC absolute deadline. Reprogram timerfd.
    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);
    program_timerfd(min_alarm_us);

    esp_timer_impl_unlock();
}

void esp_timer_impl_advance(int64_t time_diff_us)
{
    esp_timer_impl_lock();

    atomic_fetch_add_explicit(&s_time_offset_us, time_diff_us, memory_order_relaxed);

    // Offset changed, so reprogram the host timer.
    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);
    program_timerfd(min_alarm_us);

    esp_timer_impl_unlock();
}

void esp_timer_private_set(uint64_t new_us)
{
    esp_timer_impl_set(new_us);
}

void esp_timer_private_advance(int64_t time_diff_us)
{
    esp_timer_impl_advance(time_diff_us);
}

/* -------------------------------------------------------------------------- */
/* Init/deinit                                                                */
/* -------------------------------------------------------------------------- */

esp_err_t esp_timer_impl_early_init(void)
{
    // No initialization required to call esp_timer_impl_get_time().
    return ESP_OK;
}

esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler)
{
    (void)alarm_handler;

    timestamp_id[0] = UINT64_MAX;
    timestamp_id[1] = UINT64_MAX;

    atomic_store_explicit(&s_time_offset_us, 0, memory_order_relaxed);

    s_timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    if (s_timer_fd < 0) {
        ESP_LOGE(TAG, "timerfd_create failed: %s", strerror(errno));
        return ESP_FAIL;
    }

    s_shutdown_fd = eventfd(0, EFD_CLOEXEC);
    if (s_shutdown_fd < 0) {
        ESP_LOGE(TAG, "eventfd failed: %s", strerror(errno));
        close(s_timer_fd);
        s_timer_fd = -1;
        return ESP_FAIL;
    }

    int err = pthread_create(&s_alarm_thread, NULL, alarm_thread_func, NULL);
    if (err != 0) {
        ESP_LOGE(TAG, "Failed to create alarm thread: %s", strerror(err));

        close(s_shutdown_fd);
        close(s_timer_fd);

        s_shutdown_fd = -1;
        s_timer_fd = -1;

        return ESP_FAIL;
    }

    s_alarm_thread_created = true;

    ESP_LOGI(TAG, "esp_timer initialized successfully");
    return ESP_OK;
}

void esp_timer_impl_deinit(void)
{
    if (!s_alarm_thread_created) {
        return;
    }

    // Disarm timer first, then wake alarm thread through eventfd.
    if (s_timer_fd >= 0) {
        struct itimerspec its = { 0 };
        (void)timerfd_settime(s_timer_fd, TFD_TIMER_ABSTIME, &its, NULL);
    }

    if (s_shutdown_fd >= 0) {
        uint64_t one = 1;
        ssize_t n = write(s_shutdown_fd, &one, sizeof(one));
        (void)n;
    }

    pthread_join(s_alarm_thread, NULL);

    if (s_shutdown_fd >= 0) {
        close(s_shutdown_fd);
        s_shutdown_fd = -1;
    }

    if (s_timer_fd >= 0) {
        close(s_timer_fd);
        s_timer_fd = -1;
    }

    s_alarm_thread_created = false;
}

uint64_t esp_timer_impl_get_alarm_reg(void)
{
    esp_timer_impl_lock();
    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);
    esp_timer_impl_unlock();

    return min_alarm_us;
}
