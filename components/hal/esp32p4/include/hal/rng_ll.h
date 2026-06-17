/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/config.h"
#include "soc/soc.h"
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
#include "soc/trng_reg.h"
#include "soc/trng_struct.h"
#else
#include "soc/lp_system_reg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t rng_ll_read_data(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    return REG_READ(RNG_DATA_REG);
#else
    return REG_READ(LP_SYSTEM_REG_RNG_DATA_REG);
#endif
}

/**
 * @brief Enable or disable RNG clock
 *
 * @param enable true to enable, false to disable
 */
static inline void rng_ll_enable_clock(bool enable)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    LP_TRNG.date.clk_en = enable;
#else
    (void) enable;
#endif
}

/**
 * @brief Reset RNG module
 */
static inline void rng_ll_reset(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    LP_TRNG.rstn.rstn = 0;
    LP_TRNG.rstn.rstn = 1;
#endif
}

/**
 * @brief Enable or disable RNG sampling
 *
 * @param enable true to enable, false to disable
 */
static inline void rng_ll_enable_sample(bool enable)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    LP_TRNG.cfg.sample_enable = enable;
#else
    (void) enable;
#endif
}

/**
 * @brief Set RNG timer prescaler
 *
 * @param prescaler Timer prescaler value (0-255)
 */
static inline void rng_ll_set_timer_prescaler(uint8_t prescaler)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    LP_TRNG.cfg.timer_pscale = prescaler;
#else
    (void) prescaler;
#endif
}

/**
 * @brief Enable or disable RNG timer XOR
 *
 * @param enable true to enable, false to disable
 */
static inline void rng_ll_enable_timer(bool enable)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    LP_TRNG.cfg.timer_en = enable;
#else
    (void) enable;
#endif
}

/**
 * @brief Get RNG sample count
 *
 * @return Current sample count (0-255)
 */
static inline uint8_t rng_ll_get_sample_count(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    return (uint8_t)LP_TRNG.cfg.sample_cnt;
#else
    return 0;
#endif
}

/**
 * @brief Enable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_enable(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    rng_ll_enable_clock(true);
    rng_ll_reset();
    rng_ll_enable_timer(true);
    rng_ll_enable_sample(true);
#endif
}

/**
 * @brief Disable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_disable(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    rng_ll_enable_sample(false);
    rng_ll_enable_timer(false);
    rng_ll_enable_clock(false);
#endif
}

#ifdef __cplusplus
}
#endif
