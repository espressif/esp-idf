/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Attention: Timer Group has 3 independent functions: General Purpose Timer, Watchdog Timer and Clock calibration.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/timer_group_struct.h"
#include "soc/system_reg.h"

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
    uint32_t reg_val = READ_PERI_REG(DPORT_PERIP_CLK_EN0_REG);
    if (group_id == 0) {
        reg_val &= ~DPORT_TIMERGROUP_CLK_EN_M;
        reg_val |= enable << DPORT_TIMERGROUP_CLK_EN_S;
    } else {
        reg_val &= ~DPORT_TIMERGROUP1_CLK_EN_M;
        reg_val |= enable << DPORT_TIMERGROUP1_CLK_EN_S;
    }
    WRITE_PERI_REG(DPORT_PERIP_CLK_EN0_REG, reg_val);
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
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, DPORT_TIMERGROUP_RST_M);
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, 0);
        TIMERG0.wdtconfig0.wdt_flashboot_mod_en = 0;
    } else {
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, DPORT_TIMERGROUP1_RST_M);
        WRITE_PERI_REG(DPORT_PERIP_RST_EN0_REG, 0);
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
