/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

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
