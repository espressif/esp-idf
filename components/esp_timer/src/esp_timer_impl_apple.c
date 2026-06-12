/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdatomic.h>
#include "sys/param.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_timer_impl.h"
#include "esp_timer.h"
#include "esp_log.h"

#include <dispatch/dispatch.h>

static const char *TAG = "esp_timer_impl";

/* Alarm values to generate interrupt on match */
extern uint64_t timestamp_id[2];

/* GCD queue and timer source used as "hardware timer" on macOS */
static dispatch_queue_t  s_queue        = NULL;
static dispatch_source_t s_timer_source = NULL;

/* Flag to track if timer is armed */
static volatile bool s_timer_armed = false;
static _Atomic int64_t s_time_offset_us;

/* -------------------------------------------------------------------------- */
/* Time base                                                                  */
/* -------------------------------------------------------------------------- */

uint64_t esp_timer_impl_get_counter_reg(void)
{
    return (uint64_t) esp_timer_impl_get_time();
}

static int64_t get_monotonic_time_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000LL + ts.tv_nsec / 1000LL;
}

int64_t esp_timer_impl_get_time(void)
{
    return get_monotonic_time_us() + atomic_load_explicit(&s_time_offset_us, memory_order_relaxed);
}

int64_t esp_timer_get_time(void)
{
    return esp_timer_impl_get_time();
}

static void timer_alarm_dispatch_handler(void *ctx)
{
    (void) ctx;
    TaskHandle_t timer_task = esp_timer_impl_get_timer_task_handle();
    if (timer_task != NULL) {
        xTaskNotifyGive(timer_task);
    }
}

static void setup_alarm(uint64_t alarm_us)
{
    int64_t now = esp_timer_impl_get_time();
    int64_t delta_ns = (alarm_us > now) ? (alarm_us - now) * 1000LL : 0;

    if (delta_ns < 50000LL) {
        delta_ns = 50000LL; /* Minimum 50us delay to avoid busy loop */
    }

    dispatch_time_t when = dispatch_time(DISPATCH_TIME_NOW, delta_ns);
    dispatch_source_set_timer(s_timer_source, when, 0, 0);

    s_timer_armed = true;
}

void esp_timer_impl_set_alarm_id(uint64_t timestamp_us, unsigned alarm_id)
{
    esp_timer_impl_lock();
    timestamp_id[alarm_id] = timestamp_us;
    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);

    if (min_alarm_us != UINT64_MAX) {
        setup_alarm(min_alarm_us);
    } else if (s_timer_armed) {
        dispatch_source_set_timer(s_timer_source, DISPATCH_TIME_FOREVER, 0, 0);
        s_timer_armed = false;
    }

    esp_timer_impl_unlock();
}

void esp_timer_impl_set(uint64_t new_us)
{
    esp_timer_impl_lock();
    atomic_store_explicit(&s_time_offset_us, (int64_t)new_us - get_monotonic_time_us(), memory_order_relaxed);
    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);
    if (min_alarm_us != UINT64_MAX) {
        setup_alarm(min_alarm_us);
    }
    esp_timer_impl_unlock();
}

void esp_timer_impl_advance(int64_t time_diff_us)
{
    esp_timer_impl_lock();
    atomic_fetch_add_explicit(&s_time_offset_us, time_diff_us, memory_order_relaxed);
    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);
    if (min_alarm_us != UINT64_MAX) {
        setup_alarm(min_alarm_us);
    }
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

esp_err_t esp_timer_impl_early_init(void)
{
    return ESP_OK;
}

esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler)
{
    (void) alarm_handler;

    timestamp_id[0] = UINT64_MAX;
    timestamp_id[1] = UINT64_MAX;
    atomic_store_explicit(&s_time_offset_us, 0, memory_order_relaxed);

    s_queue = dispatch_get_global_queue(QOS_CLASS_USER_INTERACTIVE, 0);
    if (s_queue == NULL) {
        ESP_LOGE(TAG, "Failed to get GCD queue");
        return ESP_FAIL;
    }

    s_timer_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, s_queue);
    if (s_timer_source == NULL) {
        ESP_LOGE(TAG, "Failed to create GCD timer source");
        return ESP_FAIL;
    }

    dispatch_set_context(s_timer_source, NULL);
    dispatch_source_set_event_handler_f(s_timer_source, timer_alarm_dispatch_handler);
    dispatch_resume(s_timer_source);

    ESP_LOGI(TAG, "esp_timer initialized successfully");
    return ESP_OK;
}

void esp_timer_impl_deinit(void)
{
    if (s_timer_source != NULL) {
        dispatch_source_cancel(s_timer_source);
        dispatch_release(s_timer_source);
        s_timer_source = NULL;
    }
}

uint64_t esp_timer_impl_get_alarm_reg(void)
{
    esp_timer_impl_lock();
    uint64_t min_alarm_us = MIN(timestamp_id[0], timestamp_id[1]);
    esp_timer_impl_unlock();
    return min_alarm_us;
}
