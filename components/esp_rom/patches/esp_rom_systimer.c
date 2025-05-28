/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stddef.h>
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"

#if CONFIG_HAL_SYSTIMER_USE_ROM_IMPL

#if CONFIG_IDF_TARGET_ESP32C2 && (CONFIG_ESP32C2_REV_MIN_FULL < 200)
void systimer_hal_init(systimer_hal_context_t *hal)
{
    hal->dev = &SYSTIMER;
    systimer_ll_enable_clock(hal->dev, true);
}

void systimer_hal_deinit(systimer_hal_context_t *hal)
{
    systimer_ll_enable_clock(hal->dev, false);
    hal->dev = NULL;
}

void systimer_hal_set_tick_rate_ops(systimer_hal_context_t *hal, systimer_hal_tick_rate_ops_t *ops)
{
    hal->ticks_to_us = ops->ticks_to_us;
    hal->us_to_ticks = ops->us_to_ticks;
}

uint64_t systimer_hal_get_time(systimer_hal_context_t *hal, uint32_t counter_id)
{
    return hal->ticks_to_us(systimer_hal_get_counter_value(hal, counter_id));
}

void systimer_hal_set_alarm_target(systimer_hal_context_t *hal, uint32_t alarm_id, uint64_t target)
{
    systimer_counter_value_t alarm = {
        .val = hal->us_to_ticks(target),
    };
    systimer_ll_enable_alarm(hal->dev, alarm_id, false);
    systimer_ll_set_alarm_target(hal->dev, alarm_id, alarm.val);
    systimer_ll_apply_alarm_value(hal->dev, alarm_id);
    systimer_ll_enable_alarm(hal->dev, alarm_id, true);
}

void systimer_hal_set_alarm_period(systimer_hal_context_t *hal, uint32_t alarm_id, uint32_t period)
{
    systimer_ll_enable_alarm(hal->dev, alarm_id, false);
    systimer_ll_set_alarm_period(hal->dev, alarm_id, hal->us_to_ticks(period));
    systimer_ll_apply_alarm_value(hal->dev, alarm_id);
    systimer_ll_enable_alarm(hal->dev, alarm_id, true);
}

void systimer_hal_counter_value_advance(systimer_hal_context_t *hal, uint32_t counter_id, int64_t time_us)
{
    systimer_counter_value_t new_count = {
        .val = systimer_hal_get_counter_value(hal, counter_id) + hal->us_to_ticks(time_us),
    };
    systimer_ll_set_counter_value(hal->dev, counter_id, new_count.val);
    systimer_ll_apply_counter_value(hal->dev, counter_id);
}
#endif // CONFIG_IDF_TARGET_ESP32C2 && (CONFIG_ESP32C2_REV_MIN_FULL < 200)

#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32H21 || CONFIG_IDF_TARGET_ESP32H4
void systimer_hal_init(systimer_hal_context_t *hal)
{
    hal->dev = &SYSTIMER;
    systimer_ll_enable_clock(hal->dev, true);
    systimer_ll_enable_etm(&SYSTIMER, true);
}

void systimer_hal_deinit(systimer_hal_context_t *hal)
{
    systimer_ll_enable_etm(&SYSTIMER, false);
    systimer_ll_enable_clock(hal->dev, false);
    hal->dev = NULL;
}
#endif // CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32H21 || CONFIG_IDF_TARGET_ESP32H4

#endif // CONFIG_HAL_SYSTIMER_USE_ROM_IMPL
