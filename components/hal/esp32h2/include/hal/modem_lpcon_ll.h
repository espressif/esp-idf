/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H2 MODEM LPCON register operations

#pragma once

#include <stdlib.h>
#include "soc/soc.h"
#include "hal/assert.h"
#include "modem/modem_lpcon_struct.h"
#include "hal/modem_clock_types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_test_clk(modem_lpcon_dev_t *hw, bool en)
{
    hw->test_conf.clk_en = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_coex_lpclk_slow_osc(modem_lpcon_dev_t *hw, bool en)
{
    hw->coex_lp_clk_conf.clk_coex_lp_sel_osc_slow = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_coex_lpclk_fast_osc(modem_lpcon_dev_t *hw, bool en)
{
    hw->coex_lp_clk_conf.clk_coex_lp_sel_osc_fast = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_coex_lpclk_main_xtal(modem_lpcon_dev_t *hw, bool en)
{
    hw->coex_lp_clk_conf.clk_coex_lp_sel_xtal = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_coex_lpclk_32k_xtal(modem_lpcon_dev_t *hw, bool en)
{
    hw->coex_lp_clk_conf.clk_coex_lp_sel_xtal32k = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_set_coex_lpclk_divisor_value(modem_lpcon_dev_t *hw, uint32_t value)
{
    hw->coex_lp_clk_conf.clk_coex_lp_div_num = value;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_coex_lpclk_divisor_value(modem_lpcon_dev_t *hw)
{
    return hw->coex_lp_clk_conf.clk_coex_lp_div_num;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_coex_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_coex_en = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_i2c_master_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_i2c_mst_en = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_fe_mem_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_fe_mem_en = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_coex_force_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf_force_on.clk_coex_fo = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_i2c_master_force_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf_force_on.clk_i2c_mst_fo = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_fe_mem_force_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf_force_on.clk_fe_mem_fo = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_reset_coex(modem_lpcon_dev_t *hw)
{
    hw->rst_conf.rst_coex = 1;
    hw->rst_conf.rst_coex = 0;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_reset_i2c_master(modem_lpcon_dev_t *hw)
{
    hw->rst_conf.rst_i2c_mst = 1;
    hw->rst_conf.rst_i2c_mst = 0;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_reset_all(modem_lpcon_dev_t *hw)
{
    hw->rst_conf.val = 0xf;
    hw->rst_conf.val = 0;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_date(modem_lpcon_dev_t *hw)
{
    return hw->date.val;
}

#ifdef __cplusplus
}
#endif
