/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/soc.h"
#if SOC_LP_TIMER_SUPPORTED
#include "hal/lp_timer_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LP_TIMER_SUPPORTED
/*
 * @brief set alarm target value
 *
 * @param timer_id timer num of lp_timer, 0 or 1 for esp32c6 and esp32h2
 *
 * @param value when counter reaches alarm value, alarm event will be triggered
 */
void lp_timer_hal_set_alarm_target(uint8_t timer_id, uint64_t value);

/**
 * @brief get current counter value
 */
uint64_t lp_timer_hal_get_cycle_count(void);

/**
 * @brief clear alarm interrupt status
 */
void lp_timer_hal_clear_alarm_intr_status(void);

/**
 * @brief clear overflow interrupt status
 */
void lp_timer_hal_clear_overflow_intr_status(void);

#endif

#ifdef __cplusplus
}
#endif
