/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * ets_timer module implements a set of legacy timer APIs which are
 * used by the WiFi driver. This is done on top of the newer esp_timer APIs.
 * Applications should not use ets_timer functions, as they may change without
 * notice.
 */

#include <string.h>
#include "esp_types.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "esp_timer_impl.h"

// for ETSTimer type
#include "rom/ets_sys.h"

/* We abuse 'timer_arg' field of ETSTimer structure to hold a pointer to esp_timer */
#define ESP_TIMER(p_ets_timer) ((esp_timer_handle_t) (p_ets_timer)->timer_arg)

/* We abuse 'timer_expire' field of ETSTimer structure to hold a magic value
 * signifying that the contents of the timer was zeroed out.
 */
#define TIMER_INITIALIZED_FIELD(p_ets_timer) ((p_ets_timer)->timer_expire)
#define TIMER_INITIALIZED_VAL 0x12121212

static ESP_TIMER_IRAM_ATTR bool timer_initialized(ETSTimer *ptimer)
{
    return TIMER_INITIALIZED_FIELD(ptimer) == TIMER_INITIALIZED_VAL;
}

void ets_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
{
    if (!timer_initialized(ptimer)) {
        memset(ptimer, 0, sizeof(*ptimer));
        TIMER_INITIALIZED_FIELD(ptimer) = TIMER_INITIALIZED_VAL;
    }

    if (ESP_TIMER(ptimer) == NULL) {
        const esp_timer_create_args_t create_args = {
            .callback = pfunction,
            .arg = parg,
            .name = "ETSTimer",
            .dispatch_method = ESP_TIMER_TASK
        };

        ESP_ERROR_CHECK(esp_timer_create(&create_args, (esp_timer_handle_t*) & (ptimer->timer_arg)));
    }
}

void ESP_TIMER_IRAM_ATTR ets_timer_arm_us(ETSTimer *ptimer, uint32_t time_us, bool repeat_flag)
{
    assert(timer_initialized(ptimer));
    esp_timer_stop(ESP_TIMER(ptimer));  // no error check
    if (!repeat_flag) {
        ESP_ERROR_CHECK(esp_timer_start_once(ESP_TIMER(ptimer), time_us));
    } else {
        ESP_ERROR_CHECK(esp_timer_start_periodic(ESP_TIMER(ptimer), time_us));
    }
}

void ESP_TIMER_IRAM_ATTR ets_timer_arm(ETSTimer *ptimer, uint32_t time_ms, bool repeat_flag)
{
    uint64_t time_us = 1000LL * (uint64_t) time_ms;
    assert(timer_initialized(ptimer));
    esp_timer_stop(ESP_TIMER(ptimer));  // no error check
    if (!repeat_flag) {
        ESP_ERROR_CHECK(esp_timer_start_once(ESP_TIMER(ptimer), time_us));
    } else {
        ESP_ERROR_CHECK(esp_timer_start_periodic(ESP_TIMER(ptimer), time_us));
    }
}

void ets_timer_done(ETSTimer *ptimer)
{
    if (timer_initialized(ptimer)) {
        esp_timer_delete(ESP_TIMER(ptimer));
        ptimer->timer_arg = NULL;
        TIMER_INITIALIZED_FIELD(ptimer) = 0;
    }
}

void ESP_TIMER_IRAM_ATTR ets_timer_disarm(ETSTimer *ptimer)
{
    if (timer_initialized(ptimer)) {
        esp_timer_stop(ESP_TIMER(ptimer));
    }
}

void ets_timer_init(void)
{

}

void ets_timer_deinit(void)
{

}

void os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg) __attribute__((alias("ets_timer_setfn")));
void os_timer_disarm(ETSTimer *ptimer) __attribute__((alias("ets_timer_disarm")));
void os_timer_arm_us(ETSTimer *ptimer, uint32_t u_seconds, bool repeat_flag) __attribute__((alias("ets_timer_arm_us")));
void os_timer_arm(ETSTimer *ptimer, uint32_t milliseconds, bool repeat_flag) __attribute__((alias("ets_timer_arm")));
void os_timer_done(ETSTimer *ptimer) __attribute__((alias("ets_timer_done")));
