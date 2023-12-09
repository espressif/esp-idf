/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/systimer_types.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct systimer_dev_t *systimer_soc_handle_t;   // systimer SOC layer handle

// the definitions of the following functions are provided by esp_hw_support component, see esp_hw_support/port/${TARGET}/systimer.c
typedef uint64_t (*ticks_to_us_func_t)(uint64_t ticks); // prototype of function to convert ticks to microseconds
typedef uint64_t (*us_to_ticks_func_t)(uint64_t us);    // prototype of function to convert microseconds to ticks

/**
 * @brief systimer HAL context structure
 */
typedef struct {
    systimer_soc_handle_t dev;      /*!< systimer peripheral base address */
    ticks_to_us_func_t ticks_to_us; /*!< function to convert ticks to microseconds */
    us_to_ticks_func_t us_to_ticks; /*!< function to convert microseconds to ticks */
} systimer_hal_context_t;

/**
 * @brief systimer HAL configuration structure
 */
typedef struct {
    ticks_to_us_func_t ticks_to_us; /*!< function to convert ticks to microseconds */
    us_to_ticks_func_t us_to_ticks; /*!< function to convert microseconds to ticks */
} systimer_hal_tick_rate_ops_t;

/**
 * @brief Systimer clock source
 */
typedef soc_periph_systimer_clk_src_t systimer_clock_source_t;

/**
 * @brief initialize systimer in HAL layer
 */
void systimer_hal_init(systimer_hal_context_t *hal);

/**
 * @brief Deinitialize systimer HAL layer
 */
void systimer_hal_deinit(systimer_hal_context_t *hal);

/**
 * @brief Set tick rate operation functions
 */
void systimer_hal_set_tick_rate_ops(systimer_hal_context_t *hal, systimer_hal_tick_rate_ops_t *ops);

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

#if !SOC_SYSTIMER_FIXED_DIVIDER
/**
 * @brief set increase steps for systimer counter on different clock source
 */
void systimer_hal_set_steps_per_tick(systimer_hal_context_t *hal, int clock_source, uint32_t steps);
#endif

/**
 * @brief Get Systimer clock source
 */
systimer_clock_source_t systimer_hal_get_clock_source(systimer_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
