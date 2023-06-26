/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sets the next wakeup alarm
 *
 * @note This only sets the alarm for a single wakeup. For periodic wakeups you will
 *       have to call this function again after each wakeup to configure the next time.
 *
 * @note If ulp_lp_core_cfg_t.lp_timer_sleep_duration_us is set the ulp will automatically set
 *       the next wakeup time after returning from main and override this value.
 *
 * @param sleep_duration_us Time to next wakeup in microseconds
 */
void ulp_lp_core_lp_timer_set_wakeup_time(uint64_t sleep_duration_us);


/**
 * @brief Disables the lp timer alarm and clears any pending alarm interrupts
 *
 */
void ulp_lp_core_lp_timer_disable(void);

#ifdef __cplusplus
}
#endif
