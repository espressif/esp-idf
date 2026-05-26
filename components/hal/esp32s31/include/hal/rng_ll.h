/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/trng_reg.h"
#include "soc/trng_struct.h"
#include "soc/lp_peri_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t rng_ll_read_data(void)
{
    return REG_READ(TRNG_CRC_SYNC_DATA_REG);
}

/**
 * @brief Enable LP RNG bus clock
 */
static inline void rng_ll_enable_bus_clock(bool enable)
{
    LP_PERI_CLKRST.rng_ctrl.lp_rng_clk_en = enable;
}

/**
 * @brief Reset LP RNG bus clock
 */
static inline void rng_ll_reset(void)
{
    LP_PERI_CLKRST.rng_ctrl.lp_rng_rst_en = 1;
    LP_PERI_CLKRST.rng_ctrl.lp_rng_rst_en = 0;
}

/**
 * @brief Enable or disable RNG module clock
 *
 * @param enable true to enable, false to disable
 */
static inline void rng_ll_enable_clock(bool enable)
{
    LP_TRNG.date.clk_en = enable;
}

/**
 * @brief Enable or disable RNG sampling
 *
 * @param enable true to enable, false to disable
 */
static inline void rng_ll_enable_sample(bool enable)
{
    LP_TRNG.conf.sample_enable = enable;
}

/**
 * @brief Enable or disable CRC RNG source
 *
 * @param enable true to enable CRC RNG, false to disable
 */
static inline void rng_ll_enable_noise_crc(bool enable)
{
    LP_TRNG.conf.noise_crc_en = enable;
}

/**
 * @brief Enable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_enable(void)
{
    rng_ll_enable_bus_clock(true);
    rng_ll_reset();
    rng_ll_enable_clock(true);
    rng_ll_enable_sample(true);
    rng_ll_enable_noise_crc(true);
}

/**
 * @brief Disable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_disable(void)
{
    rng_ll_enable_sample(false);
    rng_ll_enable_noise_crc(false);
    rng_ll_enable_clock(false);
    rng_ll_enable_bus_clock(false);
}

#ifdef __cplusplus
}
#endif
