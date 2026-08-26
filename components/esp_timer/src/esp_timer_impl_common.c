/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_timer_impl.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_task.h"
#include "esp_attr.h"

/* Spinlock used to protect access to the hardware registers. */
portMUX_TYPE s_time_update_lock = portMUX_INITIALIZER_UNLOCKED;

/* Alarm values to generate interrupt on match
 * [0] - for ESP_TIMER_TASK alarms,
 * [1] - for ESP_TIMER_ISR alarms.
*/
uint64_t timestamp_id[2] = { UINT64_MAX, UINT64_MAX };

void esp_timer_impl_lock(void)
{
    portENTER_CRITICAL(&s_time_update_lock);
}

void esp_timer_impl_unlock(void)
{
    portEXIT_CRITICAL(&s_time_update_lock);
}

#ifndef CONFIG_IDF_TARGET_LINUX
void esp_timer_private_lock(void) __attribute__((alias("esp_timer_impl_lock")));
void esp_timer_private_unlock(void) __attribute__((alias("esp_timer_impl_unlock")));
#else // CONFIG_IDF_TARGET_LINUX
// Avoid using __attribute(alias) here since linux target builds on MacOS fail to compile.
void esp_timer_private_lock(void)
{
    esp_timer_impl_lock();
}

void esp_timer_private_unlock(void)
{
    esp_timer_impl_unlock();
}
#endif // CONFIG_IDF_TARGET_LINUX

void ESP_TIMER_IRAM_ATTR esp_timer_impl_set_alarm(uint64_t timestamp)
{
    esp_timer_impl_set_alarm_id(timestamp, 0);
}

#ifdef CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
/* Reprogram the hardware alarm to the nearest pending deadline,
 * i.e. MIN(timestamp_id[0], timestamp_id[1]). The timestamp_id[] cache is not modified.
 * Must be called with s_time_update_lock held. */
static inline void ESP_TIMER_IRAM_ATTR esp_timer_impl_rearm_alarm(void)
{
    esp_timer_impl_set_alarm_id(timestamp_id[ESP_TIMER_TASK], ESP_TIMER_TASK);
}

uint32_t ESP_TIMER_IRAM_ATTR esp_timer_impl_claim_due_alarms(void)
{
    portENTER_CRITICAL_ISR(&s_time_update_lock);
    uint64_t now = esp_timer_impl_get_time();
    uint32_t due_mask = 0;
    for (unsigned alarm_id = 0; alarm_id < sizeof(timestamp_id) / sizeof(timestamp_id[0]); ++alarm_id) {
        if (timestamp_id[alarm_id] <= now) {
            due_mask |= 1U << alarm_id;
            timestamp_id[alarm_id] = UINT64_MAX;
        }
    }
    esp_timer_impl_rearm_alarm();
    portEXIT_CRITICAL_ISR(&s_time_update_lock);
    return due_mask;
}
#endif

/* FIXME: This value is safe for 80MHz APB frequency, should be modified to depend on clock frequency. */
uint64_t ESP_TIMER_IRAM_ATTR esp_timer_impl_get_min_period_us(void)
{
    return 50;
}
