/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H2 LP CLKRST & LP PERI register operations

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
static inline void lp_clkrst_ll_enable_ble_rtc_timer_slow_osc(lp_clkrst_dev_t *hw, bool en)
{
    hw->lpperi.lp_sel_osc_slow = en;
}

__attribute__((always_inline))
static inline void lp_clkrst_ll_enable_ble_rtc_timer_fast_osc(lp_clkrst_dev_t *hw, bool en)
{
    hw->lpperi.lp_sel_osc_fast = en;
}

__attribute__((always_inline))
static inline void lp_clkrst_ll_enable_ble_rtc_timer_main_xtal(lp_clkrst_dev_t *hw, bool en)
{
    hw->lpperi.lp_sel_xtal = en;
}

__attribute__((always_inline))
static inline void lp_clkrst_ll_enable_ble_rtc_timer_32k_xtal(lp_clkrst_dev_t *hw, bool en)
{
    hw->lpperi.lp_sel_xtal32k = en;
}

__attribute__((always_inline))
static inline void lp_clkrst_ll_set_ble_rtc_timer_divisor_value(lp_clkrst_dev_t *hw, uint32_t value)
{
    hw->lpperi.lp_bletimer_div_num = value;
}

__attribute__((always_inline))
static inline uint32_t lp_clkrst_ll_get_ble_rtc_timer_divisor_value(lp_clkrst_dev_t *hw)
{
    return hw->lpperi.lp_bletimer_div_num;
}

__attribute__((always_inline))
static inline void lp_clkrst_ll_select_modem_32k_clock_source(lp_clkrst_dev_t *hw, uint32_t src)
{
    hw->lpperi.lp_bletimer_32k_sel = src;
}

__attribute__((always_inline))
static inline void _lp_clkrst_ll_enable_rng_clock(bool en)
{
    LPPERI.clk_en.rng_ck_en = en;
}

/// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_clkrst_ll_enable_rng_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _lp_clkrst_ll_enable_rng_clock(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
