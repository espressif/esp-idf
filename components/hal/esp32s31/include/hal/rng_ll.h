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
 * @brief Select the noise source for TRNG health tests
 *
 * @param source One-hot noise source selection
 */
static inline void rng_ll_set_noise_source(uint32_t source)
{
    REG_SET_FIELD(TRNG_CONF_REG, TRNG_NOISE_SOURCE_SEL, source);
}

/**
 * @brief Select the sampling-enable signal for TRNG health tests
 *
 * @param position One-hot sampling-enable selection
 */
static inline void rng_ll_set_noise_position(uint32_t position)
{
    REG_SET_FIELD(TRNG_CONF_REG, TRNG_NOISE_POS_SEL, position);
}

/**
 * @brief Configure repetition and adaptive proportion health-test thresholds
 *
 * @param repetition_cutoff Repetition count test cutoff
 * @param adaptive_cutoff Adaptive proportion test cutoff
 */
static inline void rng_ll_set_health_test_thresholds(uint32_t repetition_cutoff, uint32_t adaptive_cutoff)
{
    REG_SET_FIELD(TRNG_CONF_REG, TRNG_REPETITION_VALUE_C, repetition_cutoff);
    REG_SET_FIELD(TRNG_CONF_REG, TRNG_ADPATIVE_VALUE_C, adaptive_cutoff);
}

/**
 * @brief Configure the number of samples processed by the startup health test
 *
 * @param sample_limit Number of startup test samples
 */
static inline void rng_ll_set_startup_test_limit(uint32_t sample_limit)
{
    REG_SET_FIELD(TRNG_DEBUG_CONF_REG, TRNG_STARTUP_TEST_LIMIT, sample_limit);
}

/**
 * @brief Enable or disable standard 256-bit TRNG output mode
 *
 * @param enable true to enable standard output mode, false otherwise
 */
static inline void rng_ll_enable_random_output_mode(bool enable)
{
    if (enable) {
        REG_SET_BIT(TRNG_CONF_REG, TRNG_RANDOM_OUTPUT_MODE);
    } else {
        REG_CLR_BIT(TRNG_CONF_REG, TRNG_RANDOM_OUTPUT_MODE);
    }
}

/**
 * @brief Enable or bypass TRNG health tests
 *
 * @param enable true to enable health tests, false to bypass them
 */
static inline void rng_ll_enable_health_test(bool enable)
{
    if (enable) {
        REG_CLR_BIT(TRNG_DEBUG_CONF_REG, TRNG_HEALTH_TEST_BYPASS);
    } else {
        REG_SET_BIT(TRNG_DEBUG_CONF_REG, TRNG_HEALTH_TEST_BYPASS);
    }
}

/**
 * @brief Start the TRNG startup health test
 */
static inline void rng_ll_start_startup_test(void)
{
    REG_SET_BIT(TRNG_DEBUG_CONF_REG, TRNG_STARTUP_TEST_START);
}

/**
 * @brief Stop TRNG health tests
 */
static inline void rng_ll_stop_health_test(void)
{
    REG_SET_BIT(TRNG_DEBUG_CONF_REG, TRNG_HEALTH_TEST_END);
}

/**
 * @brief Enable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_enable(void)
{
    rng_ll_enable_bus_clock(true);
    rng_ll_enable_clock(true);
    rng_ll_reset();

    rng_ll_set_noise_source(BIT(4));
    rng_ll_set_noise_position(BIT(4));
    rng_ll_set_health_test_thresholds(0x1f, 0x12);
    rng_ll_set_startup_test_limit(1024);
    rng_ll_enable_health_test(true);
    rng_ll_enable_random_output_mode(true);
    rng_ll_enable_noise_crc(true);
    rng_ll_enable_sample(true);
    rng_ll_start_startup_test();
}

/**
 * @brief Disable RNG module
 *
 * TODO: unify in rng_hal.c
 */
static inline void rng_ll_disable(void)
{
    rng_ll_stop_health_test();
    rng_ll_enable_random_output_mode(false);
    rng_ll_enable_noise_crc(false);
    rng_ll_enable_sample(false);
    rng_ll_enable_clock(false);
    rng_ll_enable_bus_clock(false);
}

/**
 * @brief Check that the RNG is live: clocked and out of reset.
 *
 * @return True if the RNG is enabled and operational, false otherwise.
 */
static inline bool rng_ll_is_enabled(void)
{
    return LP_PERI_CLKRST.rng_ctrl.lp_rng_clk_en && !LP_PERI_CLKRST.rng_ctrl.lp_rng_rst_en;
}

#ifdef __cplusplus
}
#endif
