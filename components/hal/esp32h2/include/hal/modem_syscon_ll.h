/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H2 MODEM SYSCON register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "hal/assert.h"
#include "modem/modem_syscon_struct.h"
#include "hal/modem_clock_types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_test_clk(modem_syscon_dev_t *hw, bool en)
{
    hw->test_conf.clk_en = en;
}


__attribute__((always_inline))
static inline void modem_syscon_ll_enable_data_dump_mux_clock(modem_syscon_dev_t *hw, bool en)
{
    // ESP32H2 Not Support
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_etm_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_etm_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_ieee802154_apb_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_zb_apb_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_ieee802154_mac_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_zb_mac_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_modem_sec_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_modem_sec_en = en;
    hw->clk_conf.clk_modem_sec_ecb_en = en;
    hw->clk_conf.clk_modem_sec_ccm_en = en;
    hw->clk_conf.clk_modem_sec_bah_en = en;
    hw->clk_conf.clk_modem_sec_apb_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_ble_timer_apb(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_ble_timer_apb_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_ble_timer_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_ble_timer_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_data_dump_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf.clk_data_dump_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_etm_force_clock(modem_syscon_dev_t *hw)
{
    hw->clk_conf_force_on.clk_etm_fo = 1;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_modem_sec_force_clock(modem_syscon_dev_t *hw)
{
    hw->clk_conf_force_on.clk_modem_sec_fo = 1;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_ble_timer_force_clock(modem_syscon_dev_t *hw)
{
    hw->clk_conf_force_on.clk_ble_timer_fo = 1;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_data_dump_force_clock(modem_syscon_dev_t *hw)
{
    hw->clk_conf_force_on.clk_data_dump_fo = 1;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_fe(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_fe = 1;
    hw->modem_rst_conf.rst_fe = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_btmac_apb(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_btmac_apb = 1;
    hw->modem_rst_conf.rst_btmac_apb = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_btmac(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_btmac = 1;
    hw->modem_rst_conf.rst_btmac = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_btbb_apb(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_btbb_apb = 1;
    hw->modem_rst_conf.rst_btbb_apb = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_btbb(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_btbb = 1;
    hw->modem_rst_conf.rst_btbb = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_etm(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_etm = 1;
    hw->modem_rst_conf.rst_etm = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_zbmac(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_zbmac = 1;
    hw->modem_rst_conf.rst_zbmac = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_modem_sec(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_modem_ecb = 1;
    hw->modem_rst_conf.rst_modem_ccm = 1;
    hw->modem_rst_conf.rst_modem_bah = 1;
    hw->modem_rst_conf.rst_modem_sec = 1;
    hw->modem_rst_conf.rst_modem_ecb = 0;
    hw->modem_rst_conf.rst_modem_ccm = 0;
    hw->modem_rst_conf.rst_modem_bah = 0;
    hw->modem_rst_conf.rst_modem_sec = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_ble_timer(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_ble_timer = 1;
    hw->modem_rst_conf.rst_ble_timer = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_data_dump(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.rst_data_dump = 1;
    hw->modem_rst_conf.rst_data_dump = 0;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_reset_all(modem_syscon_dev_t *hw)
{
    hw->modem_rst_conf.val = 0xffffffff;
    hw->modem_rst_conf.val = 0;
}


__attribute__((always_inline))
static inline void modem_syscon_ll_clk_conf1_configure(modem_syscon_dev_t *hw, bool en, uint32_t mask)
{
    if(en){
        hw->clk_conf1.val = hw->clk_conf1.val | mask;
    } else {
        hw->clk_conf1.val = hw->clk_conf1.val & ~mask;
    }
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_fe_16m_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1.clk_fe_16m_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_fe_32m_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1.clk_fe_32m_en = en;
}


__attribute__((always_inline))
static inline void modem_syscon_ll_enable_fe_sdm_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1.clk_fe_sdm_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_fe_adc_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1.clk_fe_adc_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_fe_apb_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1.clk_fe_apb_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_bt_apb_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1.clk_bt_apb_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_bt_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1.clk_bt_en = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_fe_force_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1_force_on.clk_fe_fo = en;
}

__attribute__((always_inline))
static inline void modem_syscon_ll_enable_bt_force_clock(modem_syscon_dev_t *hw, bool en)
{
    hw->clk_conf1_force_on.clk_bt_fo = en;
}

__attribute__((always_inline))
static inline uint32_t modem_syscon_ll_get_date(modem_syscon_dev_t *hw)
{
    return hw->date.val;
}

#ifdef __cplusplus
}
#endif
