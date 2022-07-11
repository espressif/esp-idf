/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/systimer_struct.h"
#include "hal/systimer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    systimer_dev_t *dev;
} systimer_hal_context_t;

/**
 * @brief initialize systimer in HAL layer
 */
void systimer_hal_init(systimer_hal_context_t *hal);

/**
 * @brief enable systimer counter
 */
void systimer_hal_enable_counter(systimer_hal_context_t *hal, uint32_t counter_id);

/**
 * @brief get current counter value
 */
uint64_t systimer_hal_get_counter_value(systimer_hal_context_t *hal, uint32_t counter_id);

/**
 * @brief get current time (in microseconds)
 */
uint64_t systimer_hal_get_time(systimer_hal_context_t *hal, uint32_t counter_id);

/*
 * @brief set alarm target value (used in one-shot mode)
 */
void systimer_hal_set_alarm_target(systimer_hal_context_t *hal, uint32_t alarm_id, uint64_t target);

/**
 * @brief set alarm period value (used in period mode)
 */
void systimer_hal_set_alarm_period(systimer_hal_context_t *hal, uint32_t alarm_id, uint32_t period);

/**
 * @brief get alarm time
 */
uint64_t systimer_hal_get_alarm_value(systimer_hal_context_t *hal, uint32_t alarm_id);

/**
 * @brief enable alarm interrupt
 */
void systimer_hal_enable_alarm_int(systimer_hal_context_t *hal, uint32_t alarm_id);

/**
 * @brief select alarm mode
 */
void systimer_hal_select_alarm_mode(systimer_hal_context_t *hal, uint32_t alarm_id, systimer_alarm_mode_t mode);

/**
 * @brief update systimer step when apb clock gets changed
 */
void systimer_hal_on_apb_freq_update(systimer_hal_context_t *hal, uint32_t apb_ticks_per_us);

/**
 * @brief move systimer counter value forward or backward
 */
void systimer_hal_counter_value_advance(systimer_hal_context_t *hal, uint32_t counter_id, int64_t time_us);

/**
 * @brief  connect alarm unit to selected counter
 */
void systimer_hal_connect_alarm_counter(systimer_hal_context_t *hal, uint32_t alarm_id, uint32_t counter_id);

/**
 * @brief  set if a counter should be stalled when CPU is halted by the debugger
 */
void systimer_hal_counter_can_stall_by_cpu(systimer_hal_context_t *hal, uint32_t counter_id, uint32_t cpu_id, bool can);

#if !SOC_SYSTIMER_HAS_FIXED_TICKS_PER_US
/**
 * @brief set increase steps for systimer counter on different clock source
 */
void systimer_hal_set_steps_per_tick(systimer_hal_context_t *hal, int clock_source, uint32_t steps);
#endif

#ifdef __cplusplus
}
#endif
