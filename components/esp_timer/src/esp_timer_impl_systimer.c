// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_timer_impl.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "soc/periph_defs.h"
#include "freertos/FreeRTOS.h"
#include "hal/systimer_ll.h"
#include "hal/systimer_types.h"
#include "hal/systimer_hal.h"

/**
 * @file esp_timer_systimer.c
 * @brief Implementation of esp_timer using systimer.
 *
 * This timer is a 64-bit up-counting timer, with a programmable compare value (called 'alarm' hereafter).
 * When the timer reaches compare value, interrupt is raised.
 * The timer can be configured to produce an edge interrupt.
 *
 * @note systimer counter0 and alarm2 are adopted to implemented esp_timer
 */

static const char *TAG = "esp_timer_systimer";

/* Interrupt handle returned by the interrupt allocator */
static intr_handle_t s_timer_interrupt_handle;

/* Function from the upper layer to be called when the interrupt happens.
 * Registered in esp_timer_impl_init.
 */
static intr_handler_t s_alarm_handler = NULL;

/* Spinlock used to protect access to the hardware registers. */
portMUX_TYPE s_time_update_lock = portMUX_INITIALIZER_UNLOCKED;

void esp_timer_impl_lock(void)
{
    portENTER_CRITICAL(&s_time_update_lock);
}

void esp_timer_impl_unlock(void)
{
    portEXIT_CRITICAL(&s_time_update_lock);
}

uint64_t IRAM_ATTR esp_timer_impl_get_counter_reg(void)
{
    return systimer_hal_get_counter_value(SYSTIMER_COUNTER_0);
}

int64_t IRAM_ATTR esp_timer_impl_get_time(void)
{
    if (s_alarm_handler == NULL) {
        return 0;
    }
    return systimer_hal_get_time(SYSTIMER_COUNTER_0);
}

int64_t esp_timer_get_time(void) __attribute__((alias("esp_timer_impl_get_time")));

void IRAM_ATTR esp_timer_impl_set_alarm(uint64_t timestamp)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    systimer_hal_set_alarm_target(SYSTIMER_ALARM_2, timestamp);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

static void IRAM_ATTR timer_alarm_isr(void *arg)
{
    // clear the interrupt
    systimer_ll_clear_alarm_int(SYSTIMER_ALARM_2);
    /* Call the upper layer handler */
    (*s_alarm_handler)(arg);
}

void IRAM_ATTR esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us)
{
    systimer_hal_on_apb_freq_update(apb_ticks_per_us);
}

void esp_timer_impl_advance(int64_t time_us)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    systimer_hal_counter_value_advance(SYSTIMER_COUNTER_0, time_us);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler)
{
    s_alarm_handler = alarm_handler;
#if SOC_SYSTIMER_INT_LEVEL
    int int_type = 0;
#else
    int int_type = ESP_INTR_FLAG_EDGE;
#endif // SOC_SYSTIMER_INT_LEVEL
    esp_err_t err = esp_intr_alloc(ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE,
                                   ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM | int_type,
                                   &timer_alarm_isr, NULL, &s_timer_interrupt_handle);

    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "esp_intr_alloc failed (%#x)", err);
        goto err_intr_alloc;
    }

    systimer_hal_init();
    systimer_hal_enable_counter(SYSTIMER_COUNTER_0);
    systimer_hal_select_alarm_mode(SYSTIMER_ALARM_2, SYSTIMER_ALARM_MODE_ONESHOT);
    systimer_hal_connect_alarm_counter(SYSTIMER_ALARM_2, SYSTIMER_COUNTER_0);

    /* TODO: if SYSTIMER is used for anything else, access to SYSTIMER_INT_ENA_REG has to be
    * protected by a shared spinlock. Since this code runs as part of early startup, this
    * is practically not an issue.
    */
    systimer_hal_enable_alarm_int(SYSTIMER_ALARM_2);

    err = esp_intr_enable(s_timer_interrupt_handle);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "esp_intr_enable failed (%#x)", err);
        goto err_intr_en;
    }
    return ESP_OK;

err_intr_en:
    systimer_ll_disable_alarm(SYSTIMER_ALARM_2);
    /* TODO: may need a spinlock, see the note related to SYSTIMER_INT_ENA_REG in systimer_hal_init */
    systimer_ll_disable_alarm_int(SYSTIMER_ALARM_2);
    esp_intr_free(s_timer_interrupt_handle);
err_intr_alloc:
    s_alarm_handler = NULL;
    return err;
}

void esp_timer_impl_deinit(void)
{
    esp_intr_disable(s_timer_interrupt_handle);
    systimer_ll_disable_alarm(SYSTIMER_ALARM_2);
    /* TODO: may need a spinlock, see the note related to SYSTIMER_INT_ENA_REG in systimer_hal_init */
    systimer_ll_disable_alarm_int(SYSTIMER_ALARM_2);
    esp_intr_free(s_timer_interrupt_handle);
    s_timer_interrupt_handle = NULL;
    s_alarm_handler = NULL;
}

uint64_t IRAM_ATTR esp_timer_impl_get_min_period_us(void)
{
    return 50;
}

uint64_t esp_timer_impl_get_alarm_reg(void)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    uint64_t val = systimer_hal_get_alarm_value(SYSTIMER_ALARM_2);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
    return val;
}

void esp_timer_private_update_apb_freq(uint32_t apb_ticks_per_us) __attribute__((alias("esp_timer_impl_update_apb_freq")));
void esp_timer_private_advance(int64_t time_us) __attribute__((alias("esp_timer_impl_advance")));
void esp_timer_private_lock(void) __attribute__((alias("esp_timer_impl_lock")));
void esp_timer_private_unlock(void) __attribute__((alias("esp_timer_impl_unlock")));
