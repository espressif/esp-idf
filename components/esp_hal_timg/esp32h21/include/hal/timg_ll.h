/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Attention: Timer Group has 3 independent functions: General Purpose Timer, Watchdog Timer and Clock calibration.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/timer_group_struct.h"
#include "soc/pcr_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for timer group module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void _timg_ll_enable_bus_clock(int group_id, bool enable)
{
    PCR.timergroup[group_id].timergroup_conf.tg_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define timg_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _timg_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the timer group module
 *
 * @note  After reset the register, the "flash boot protection" will be enabled again.
 *        FLash boot protection is not used anymore after system boot up.
 *        This function will disable it by default in order to prevent the system from being reset unexpectedly.
 *
 * @param group_id Group ID
 */
static inline void _timg_ll_reset_register(int group_id)
{
    PCR.timergroup[group_id].timergroup_conf.tg_rst_en = 1;
    PCR.timergroup[group_id].timergroup_conf.tg_rst_en = 0;
    if (group_id == 0) {
        TIMERG0.wdtconfig0.wdt_flashboot_mod_en = 0;
    } else {
        TIMERG1.wdtconfig0.wdt_flashboot_mod_en = 0;
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define timg_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _timg_ll_reset_register(__VA_ARGS__); \
    } while(0)

#ifdef __cplusplus
}
#endif
