/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_timer_impl.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_compiler.h"
#include "soc/periph_defs.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_clk.h"
#include "esp_private/systimer.h"
#include "esp_private/periph_ctrl.h"
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

/* Systimer HAL layer object */
static systimer_hal_context_t systimer_hal;

/* Spinlock used to protect access to the hardware registers. */
static portMUX_TYPE s_time_update_lock = portMUX_INITIALIZER_UNLOCKED;

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
    return systimer_hal_get_counter_value(&systimer_hal, SYSTIMER_LL_COUNTER_CLOCK);
}

int64_t IRAM_ATTR esp_timer_impl_get_time(void)
{
    // we hope the execution time of this function won't > 1us
    // thus, to save one function call, we didn't use the existing `systimer_hal_get_time`
    return systimer_hal.ticks_to_us(systimer_hal_get_counter_value(&systimer_hal, SYSTIMER_LL_COUNTER_CLOCK));
}

int64_t esp_timer_get_time(void) __attribute__((alias("esp_timer_impl_get_time")));

void IRAM_ATTR esp_timer_impl_set_alarm_id(uint64_t timestamp, unsigned alarm_id)
{
    static uint64_t timestamp_id[2] = { UINT64_MAX, UINT64_MAX };
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    timestamp_id[alarm_id] = timestamp;
    timestamp = MIN(timestamp_id[0], timestamp_id[1]);
    if (timestamp != UINT64_MAX) {
        systimer_hal_set_alarm_target(&systimer_hal, SYSTIMER_LL_ALARM_CLOCK, timestamp);
    }
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

void IRAM_ATTR esp_timer_impl_set_alarm(uint64_t timestamp)
{
    esp_timer_impl_set_alarm_id(timestamp, 0);
}

static void IRAM_ATTR timer_alarm_isr(void *arg)
{
    // clear the interrupt
    systimer_ll_clear_alarm_int(systimer_hal.dev, SYSTIMER_LL_ALARM_CLOCK);
    /* Call the upper layer handler */
    (*s_alarm_handler)(arg);
}

void IRAM_ATTR esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us)
{
#if !SOC_SYSTIMER_FIXED_DIVIDER
    systimer_hal_on_apb_freq_update(&systimer_hal, apb_ticks_per_us);
#endif
}

void esp_timer_impl_set(uint64_t new_us)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    systimer_counter_value_t new_count = {
        .val = systimer_hal.us_to_ticks(new_us),
    };
    systimer_ll_set_counter_value(systimer_hal.dev, SYSTIMER_LL_COUNTER_CLOCK, new_count.val);
    systimer_ll_apply_counter_value(systimer_hal.dev, SYSTIMER_LL_COUNTER_CLOCK);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

void esp_timer_impl_advance(int64_t time_diff_us)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    systimer_hal_counter_value_advance(&systimer_hal, SYSTIMER_LL_COUNTER_CLOCK, time_diff_us);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

esp_err_t esp_timer_impl_early_init(void)
{
    periph_module_enable(PERIPH_SYSTIMER_MODULE);
    systimer_hal_tick_rate_ops_t ops = {
        .ticks_to_us = systimer_ticks_to_us,
        .us_to_ticks = systimer_us_to_ticks,
    };
    systimer_hal_init(&systimer_hal);
    systimer_hal_set_tick_rate_ops(&systimer_hal, &ops);

#if !SOC_SYSTIMER_FIXED_DIVIDER
    assert(esp_clk_xtal_freq() == (40 * 1000000) &&
           "update the step for xtal to support other XTAL:APB frequency ratios");
    systimer_hal_set_steps_per_tick(&systimer_hal, 0, 2); // for xtal
    systimer_hal_set_steps_per_tick(&systimer_hal, 1, 1); // for pll
#endif

    systimer_hal_enable_counter(&systimer_hal, SYSTIMER_LL_COUNTER_CLOCK);
    systimer_hal_select_alarm_mode(&systimer_hal, SYSTIMER_LL_ALARM_CLOCK, SYSTIMER_ALARM_MODE_ONESHOT);
    systimer_hal_connect_alarm_counter(&systimer_hal, SYSTIMER_LL_ALARM_CLOCK, SYSTIMER_LL_COUNTER_CLOCK);

    return ESP_OK;
}

esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler)
{
    s_alarm_handler = alarm_handler;
    const int interrupt_lvl = (1 << CONFIG_ESP_TIMER_INTERRUPT_LEVEL) & ESP_INTR_FLAG_LEVELMASK;
#if SOC_SYSTIMER_INT_LEVEL
    int int_type = 0;
#else
    int int_type = ESP_INTR_FLAG_EDGE;
#endif // SOC_SYSTIMER_INT_LEVEL
    esp_err_t err = esp_intr_alloc(ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE,
                                   ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM | int_type | interrupt_lvl,
                                   &timer_alarm_isr, NULL, &s_timer_interrupt_handle);

    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "esp_intr_alloc failed (0x%x)", err);
        goto err_intr_alloc;
    }

    /* TODO: if SYSTIMER is used for anything else, access to SYSTIMER_INT_ENA_REG has to be
    * protected by a shared spinlock. Since this code runs as part of early startup, this
    * is practically not an issue.
    */
    systimer_hal_enable_alarm_int(&systimer_hal, SYSTIMER_LL_ALARM_CLOCK);

    err = esp_intr_enable(s_timer_interrupt_handle);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "esp_intr_enable failed (0x%x)", err);
        goto err_intr_en;
    }
    return ESP_OK;

err_intr_en:
    systimer_ll_enable_alarm(systimer_hal.dev, SYSTIMER_LL_ALARM_CLOCK, false);
    /* TODO: may need a spinlock, see the note related to SYSTIMER_INT_ENA_REG in systimer_hal_init */
    systimer_ll_enable_alarm_int(systimer_hal.dev, SYSTIMER_LL_ALARM_CLOCK, false);
    esp_intr_free(s_timer_interrupt_handle);
err_intr_alloc:
    s_alarm_handler = NULL;
    return err;
}

void esp_timer_impl_deinit(void)
{
    esp_intr_disable(s_timer_interrupt_handle);
    systimer_ll_enable_alarm(systimer_hal.dev, SYSTIMER_LL_ALARM_CLOCK, false);
    /* TODO: may need a spinlock, see the note related to SYSTIMER_INT_ENA_REG in systimer_hal_init */
    systimer_ll_enable_alarm_int(systimer_hal.dev, SYSTIMER_LL_ALARM_CLOCK, false);
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
    uint64_t val = systimer_hal_get_alarm_value(&systimer_hal, SYSTIMER_LL_ALARM_CLOCK);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
    return val;
}

void esp_timer_private_update_apb_freq(uint32_t apb_ticks_per_us) __attribute__((alias("esp_timer_impl_update_apb_freq")));
void esp_timer_private_set(uint64_t new_us) __attribute__((alias("esp_timer_impl_set")));
void esp_timer_private_advance(int64_t time_diff_us) __attribute__((alias("esp_timer_impl_advance")));
void esp_timer_private_lock(void) __attribute__((alias("esp_timer_impl_lock")));
void esp_timer_private_unlock(void) __attribute__((alias("esp_timer_impl_unlock")));
