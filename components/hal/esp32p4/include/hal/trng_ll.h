/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/trng_struct.h"
#include "hal/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable or disable TRNG clock
 *
 * @param enable true to enable, false to disable
 */
static inline void trng_ll_enable_clock(bool enable)
{
    LP_TRNG.date.clk_en = enable;
}

/**
 * @brief Reset TRNG module
 */
static inline void trng_ll_reset(void)
{
    LP_TRNG.rstn.rstn = 0;
    LP_TRNG.rstn.rstn = 1;
}

/**
 * @brief Enable or disable TRNG sampling
 *
 * @param enable true to enable, false to disable
 */
static inline void trng_ll_enable_sample(bool enable)
{
    LP_TRNG.cfg.sample_enable = enable;
}

/**
 * @brief Set TRNG timer prescaler
 *
 * @param prescaler Timer prescaler value (0-255)
 */
static inline void trng_ll_set_timer_prescaler(uint8_t prescaler)
{
    LP_TRNG.cfg.timer_pscale = prescaler;
}

/**
 * @brief Enable or disable TRNG timer XOR
 *
 * @param enable true to enable, false to disable
 */
static inline void trng_ll_enable_timer(bool enable)
{
    LP_TRNG.cfg.timer_en = enable;
}

/**
 * @brief Get TRNG sample count
 *
 * @return Current sample count (0-255)
 */
static inline uint8_t trng_ll_get_sample_count(void)
{
    return (uint8_t)LP_TRNG.cfg.sample_cnt;
}

/**
 * @brief Read random data from TRNG
 *
 * @return 32-bit random data
 */
static inline uint32_t trng_ll_read_data(void)
{
    return LP_TRNG.data.data;
}

/**
 * @brief Enable TRNG module
 *
 * TODO: unify in trng_hal.c
 */
static inline void trng_ll_enable(void)
{
    trng_ll_enable_clock(true);
    trng_ll_reset();
    trng_ll_enable_timer(true);
    trng_ll_enable_sample(true);
}

/**
 * @brief Disable TRNG module
 *
 * TODO: unify in trng_hal.c
 */
static inline void trng_ll_disable(void)
{
    trng_ll_enable_sample(false);
    trng_ll_enable_timer(false);
    trng_ll_enable_clock(false);
}


#ifdef __cplusplus
}
#endif
