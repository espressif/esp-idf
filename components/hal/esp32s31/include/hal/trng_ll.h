/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/trng_struct.h"
#include "soc/lp_peri_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable LP TRNG bus clock
 */
static inline void trng_ll_enable_bus_clock(bool enable)
{
    LP_PERI_CLKRST.rng_ctrl.lp_rng_clk_en = enable;
}

/**
 * @brief Reset LP TRNG bus clock
 *
 */
static inline void trng_ll_reset(void)
{
    LP_PERI_CLKRST.rng_ctrl.lp_rng_rst_en = 1;
    LP_PERI_CLKRST.rng_ctrl.lp_rng_rst_en = 0;
}

/**
 * @brief Enable or disable TRNG module clock
 *
 * @param enable true to enable, false to disable
 */
static inline void trng_ll_enable_clock(bool enable)
{
    LP_TRNG.date.clk_en = enable;
}

/**
 * @brief Enable or disable TRNG sampling
 *
 * @param enable true to enable, false to disable
 */
static inline void trng_ll_enable_sample(bool enable)
{
    LP_TRNG.conf.sample_enable = enable;
}

/**
 * @brief Enable or disable CRC RNG source
 *
 * @param enable true to enable CRC RNG, false to disable
 */
static inline void trng_ll_enable_noise_crc(bool enable)
{
    LP_TRNG.conf.noise_crc_en = enable;
}

/**
 * @brief Read random data from TRNG
 */
static inline uint32_t trng_ll_read_data(void)
{
    return LP_TRNG.crc_sync_data.sw_crc_random_data_sync;
}

/**
 * @brief Enable TRNG module
 *
 * TODO: unify in trng_hal.c
 */
static inline void trng_ll_enable(void)
{
    trng_ll_enable_bus_clock(true);
    trng_ll_reset();
    trng_ll_enable_clock(true);
    trng_ll_enable_sample(true);
    trng_ll_enable_noise_crc(true);
}

/**
 * @brief Disable TRNG module
 *
 * TODO: unify in trng_hal.c
 */
static inline void trng_ll_disable(void)
{
    trng_ll_enable_sample(false);
    trng_ll_enable_noise_crc(false);
    trng_ll_enable_clock(false);
    trng_ll_enable_bus_clock(false);
}

#ifdef __cplusplus
}
#endif
