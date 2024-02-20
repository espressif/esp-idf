/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/timer_types.h"
#include "soc/timer_group_struct.h"
#include "soc/pcr_struct.h"
// TODO: [ESP32C5] IDF-8693
// #include "soc/soc_etm_source.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get timer group register base address with giving group number
#define TIMER_LL_GET_HW(group_id) ((group_id == 0) ? (&TIMERG0) : (&TIMERG1))
#define TIMER_LL_EVENT_ALARM(timer_id) (1 << (timer_id))

// TODO: [ESP32C5] IDF-8693

/**
 * @brief Enable the bus clock for timer group module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void timer_ll_enable_bus_clock(int group_id, bool enable)
{
    // TODO: [ESP32C5] IDF-8705
    // if (group_id == 0) {
    //     PCR.timergroup0_conf.tg0_clk_en = enable;
    // } else {
    //     PCR.timergroup1_conf.tg1_clk_en = enable;
    // }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define timer_ll_enable_bus_clock(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; timer_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the timer group module
 *
 * @note  After reset the register, the "flash boot protection" will be enabled again.
 *        FLash boot protection is not used anymore after system boot up.
 *        This function will disable it by default in order to prevent the system from being reset unexpectedly.
 *
 * @param group_id Group ID
 */
static inline void timer_ll_reset_register(int group_id)
{
    // TODO: [ESP32C5] IDF-8705
    // if (group_id == 0) {
    //     PCR.timergroup0_conf.tg0_rst_en = 1;
    //     PCR.timergroup0_conf.tg0_rst_en = 0;
    //     TIMERG0.wdtconfig0.wdt_flashboot_mod_en = 0;
    // } else {
    //     PCR.timergroup1_conf.tg1_rst_en = 1;
    //     PCR.timergroup1_conf.tg1_rst_en = 0;
    //     TIMERG1.wdtconfig0.wdt_flashboot_mod_en = 0;
    // }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define timer_ll_reset_register(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; timer_ll_reset_register(__VA_ARGS__)


#ifdef __cplusplus
}
#endif
