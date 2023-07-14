/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C6 MODEM LPCON register operations

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
static inline void modem_lpcon_ll_enable_ble_rtc_timer_slow_osc(modem_lpcon_dev_t *hw, bool en)
{
    hw->lp_timer_conf.clk_lp_timer_sel_osc_slow = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_ble_rtc_timer_fast_osc(modem_lpcon_dev_t *hw, bool en)
{
    hw->lp_timer_conf.clk_lp_timer_sel_osc_fast = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_ble_rtc_timer_main_xtal(modem_lpcon_dev_t *hw, bool en)
{
    hw->lp_timer_conf.clk_lp_timer_sel_xtal = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_ble_rtc_timer_32k_xtal(modem_lpcon_dev_t *hw, bool en)
{
    hw->lp_timer_conf.clk_lp_timer_sel_xtal32k = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_set_ble_rtc_timer_divisor_value(modem_lpcon_dev_t *hw, uint32_t value)
{
    hw->lp_timer_conf.clk_lp_timer_div_num = value;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_ble_rtc_timer_divisor_value(modem_lpcon_dev_t *hw)
{
    return hw->lp_timer_conf.clk_lp_timer_div_num;
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
static inline void modem_lpcon_ll_enable_wifi_lpclk_slow_osc(modem_lpcon_dev_t *hw, bool en)
{
    hw->wifi_lp_clk_conf.clk_wifipwr_lp_sel_osc_slow = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_wifi_lpclk_fast_osc(modem_lpcon_dev_t *hw, bool en)
{
    hw->wifi_lp_clk_conf.clk_wifipwr_lp_sel_osc_fast = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_wifi_lpclk_main_xtal(modem_lpcon_dev_t *hw, bool en)
{
    hw->wifi_lp_clk_conf.clk_wifipwr_lp_sel_xtal = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_wifi_lpclk_32k_xtal(modem_lpcon_dev_t *hw, bool en)
{
    hw->wifi_lp_clk_conf.clk_wifipwr_lp_sel_xtal32k = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_set_wifi_lpclk_divisor_value(modem_lpcon_dev_t *hw, uint32_t value)
{
    hw->wifi_lp_clk_conf.clk_wifipwr_lp_div_num = value;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_wifi_lpclk_divisor_value(modem_lpcon_dev_t *hw)
{
    return hw->wifi_lp_clk_conf.clk_wifipwr_lp_div_num;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_i2c_master_160m_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->i2c_mst_clk_conf.clk_i2c_mst_sel_160m = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_select_modem_32k_clock_source(modem_lpcon_dev_t *hw, uint32_t src)
{
    hw->modem_32k_clk_conf.clk_modem_32k_sel = src;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_wifipwr_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_wifipwr_en = en;
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
static inline void modem_lpcon_ll_enable_ble_rtc_timer_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_lp_timer_en = en;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_wifipwr_force_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf_force_on.clk_wifipwr_fo = en;
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
static inline void modem_lpcon_ll_enable_ble_rtc_timer_force_clock(modem_lpcon_dev_t *hw, bool en)
{
    hw->clk_conf_force_on.clk_lp_timer_fo = en;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_wifipwr_icg_bitmap(modem_lpcon_dev_t *hw)
{
    return hw->clk_conf_power_st.clk_wifipwr_st_map;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_set_wifipwr_icg_bitmap(modem_lpcon_dev_t *hw, uint32_t bitmap)
{
    hw->clk_conf_power_st.clk_wifipwr_st_map = bitmap;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_coex_icg_bitmap(modem_lpcon_dev_t *hw)
{
    return hw->clk_conf_power_st.clk_coex_st_map;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_set_coex_icg_bitmap(modem_lpcon_dev_t *hw, uint32_t bitmap)
{
    hw->clk_conf_power_st.clk_coex_st_map = bitmap;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_i2c_master_icg_bitmap(modem_lpcon_dev_t *hw)
{
    return hw->clk_conf_power_st.clk_i2c_mst_st_map;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_set_i2c_master_icg_bitmap(modem_lpcon_dev_t *hw, uint32_t bitmap)
{
    hw->clk_conf_power_st.clk_i2c_mst_st_map = bitmap;
}

__attribute__((always_inline))
static inline uint32_t modem_lpcon_ll_get_lp_apb_icg_bitmap(modem_lpcon_dev_t *hw)
{
    return hw->clk_conf_power_st.clk_lp_apb_st_map;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_set_lp_apb_icg_bitmap(modem_lpcon_dev_t *hw, uint32_t bitmap)
{
    hw->clk_conf_power_st.clk_lp_apb_st_map = bitmap;
}

__attribute__((always_inline))
static inline void modem_lpcon_ll_reset_wifipwr(modem_lpcon_dev_t *hw)
{
    hw->rst_conf.rst_wifipwr = 1;
    hw->rst_conf.rst_wifipwr = 0;
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
static inline void modem_lpcon_ll_reset_ble_rtc_timer(modem_lpcon_dev_t *hw)
{
    hw->rst_conf.rst_lp_timer = 1;
    hw->rst_conf.rst_lp_timer = 0;
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
