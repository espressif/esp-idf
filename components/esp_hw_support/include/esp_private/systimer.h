/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"

#if CONFIG_ESP_TIMER_IMPL_TG0_LAC
/* Selects which Timer Group peripheral to use */
#define LACT_MODULE     0

/* Desired number of timer ticks per microsecond.
 * This value should be small enough so that all possible APB frequencies
 * could be divided by it without remainder.
 * On the other hand, the smaller this value is, the longer we need to wait
 * after setting UPDATE_REG before the timer value can be read.
 * If LACT_TICKS_PER_US == 1, then we need to wait up to 1 microsecond, which
 * makes esp_timer_impl_get_time function take too much time.
 * The value LACT_TICKS_PER_US == 2 allows for most of the APB frequencies, and
 * allows reading the counter quickly enough.
 */
#define LACT_TICKS_PER_US    2
#endif

// we assign the systimer resources statically
#define SYSTIMER_COUNTER_ESPTIMER    0 // Counter used by esptimer, to generate the system level wall clock
#define SYSTIMER_COUNTER_OS_TICK     1 // Counter used by RTOS porting layer, to generate the OS tick
#define SYSTIMER_ALARM_OS_TICK_CORE0 0 // Alarm used by OS tick, dedicated for core 0
#define SYSTIMER_ALARM_OS_TICK_CORE1 1 // Alarm used by OS tick, dedicated for core 1
#define SYSTIMER_ALARM_ESPTIMER      2 // Alarm used by esptimer

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert ticks to microseconds
 *
 * @param ticks ticks to convert
 * @return microseconds
 */
uint64_t systimer_ticks_to_us(uint64_t ticks) __attribute__((const));

/**
 * @brief Convert microseconds to ticks
 *
 * @param us microseconds to convert
 * @return ticks
 */
uint64_t systimer_us_to_ticks(uint64_t us) __attribute__((const));

#ifdef __cplusplus
}
#endif
