/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/lpperi_reg.h"
#include "soc/lpperi_struct.h"
#include "hal/lp_clkrst_ll.h"

//For ESP32C5, RNG needs to be reset and enabled again when wakeup from sleep
#define RNG_LL_NEEDS_RESET_WHEN_WAKEUP 1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read random data from RNG
 *
 * @return 32-bit random data
 */
static inline uint32_t rng_ll_read_data(void)
{
    return REG_READ(LPPERI_RNG_DATA_SYNC_REG);
}

/**
 * @brief Enable or disable RNG sampling.
 *
 * @param enable True to enable, False to disable
 */
static inline void rng_ll_enable_sample(bool enable)
{
    LPPERI.rng_cfg.rng_sample_enable = enable;
}

/**
 * @brief Enable or disable rng xor rtc timer.
 *
 * @param enable True to enable, False to disable
 */
static inline void rng_ll_enable_rtc_timer(bool enable)
{
    LPPERI.rng_cfg.rtc_timer_en = enable ? 0x3 : 0x0;
}

/**
 * @brief Enable or disable rng xor async rng timer.
 *
 * @param enable True to enable, False to disable
 */
static inline void rng_ll_enable_rng_timer(bool enable)
{
    LPPERI.rng_cfg.rng_timer_en = enable;
}

/**
 * @brief Reset RNG.
 */
static inline void rng_ll_reset(void)
{
    LPPERI.reset_en.lp_rng_reset_en = 1;
    LPPERI.reset_en.lp_rng_reset_en = 0;
}

/**
 * @brief Enable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_enable(void)
{
    _lp_clkrst_ll_enable_rng_clock(true);
    rng_ll_enable_sample(true);
    rng_ll_enable_rtc_timer(true);
    rng_ll_enable_rng_timer(true);
}

/**
 * @brief Disable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_disable(void)
{
    rng_ll_enable_sample(false);
    rng_ll_enable_rtc_timer(false);
    rng_ll_enable_rng_timer(false);
    _lp_clkrst_ll_enable_rng_clock(false);
}

#ifdef __cplusplus
}
#endif
