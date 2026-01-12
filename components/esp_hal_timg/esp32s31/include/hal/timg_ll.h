/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Attention: Timer Group has 3 independent functions: General Purpose Timer, Watchdog Timer and Clock calibration.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/timer_group_struct.h"
#include "soc/hp_sys_clkrst_struct.h"

#define TIMG_LL_GET(_attr) TIMG_LL_ ## _attr

// Number of Timer Group instances
#define TIMG_LL_INST_NUM 2

// Number of general purpose timers in each Timer Group
#define TIMG_LL_GPTIMERS_PER_INST 2

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
    if (group_id == 0) {
        HP_SYS_CLKRST.timergrp0_ctrl0.reg_timergrp0_apb_clk_en = enable;
    } else {
        HP_SYS_CLKRST.timergrp1_ctrl0.reg_timergrp1_apb_clk_en = enable;
    }
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
    if (group_id == 0) {
        HP_SYS_CLKRST.timergrp0_ctrl0.reg_timergrp0_rst_en = 1;
        HP_SYS_CLKRST.timergrp0_ctrl0.reg_timergrp0_rst_en = 0;
        TIMERG0.wdtconfig0.wdt_flashboot_mod_en = 0;
    } else {
        HP_SYS_CLKRST.timergrp1_ctrl0.reg_timergrp1_rst_en = 1;
        HP_SYS_CLKRST.timergrp1_ctrl0.reg_timergrp1_rst_en = 0;
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
