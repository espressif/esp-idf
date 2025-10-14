/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C5 LP_CLKRST & LP PERI register operations

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "soc/soc.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lpperi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline void _lp_clkrst_ll_enable_rng_clock(bool en)
{
    LPPERI.clk_en.rng_ck_en = en;
}
/// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_clkrst_ll_enable_rng_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _lp_clkrst_ll_enable_rng_clock(__VA_ARGS__)

__attribute__((always_inline))
static inline void _lp_clkrst_ll_enable_otp_dbg_clock(bool en)
{
    LPPERI.clk_en.otp_dbg_ck_en = en;
}
/// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_clkrst_ll_enable_otp_dbg_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _lp_clkrst_ll_enable_otp_dbg_clock(__VA_ARGS__)

__attribute__((always_inline))
static inline void _lp_clkrst_ll_enable_lp_ana_i2c_clock(bool en)
{
    LPPERI.clk_en.lp_ana_i2c_ck_en = en;
}
/// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_clkrst_ll_enable_lp_ana_i2c_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _lp_clkrst_ll_enable_lp_ana_i2c_clock(__VA_ARGS__)

__attribute__((always_inline))
static inline void _lp_clkrst_ll_enable_lp_ext_i2c_clock(bool en)
{
    LPPERI.clk_en.lp_ext_i2c_ck_en = en;
}
/// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_clkrst_ll_enable_lp_ext_i2c_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _lp_clkrst_ll_enable_lp_ext_i2c_clock(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
