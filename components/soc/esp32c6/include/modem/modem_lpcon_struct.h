/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    struct {
        uint32_t clk_en:1;
        uint32_t clk_debug_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} modem_lpcon_test_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_lp_timer_sel_osc_slow:1;
        uint32_t clk_lp_timer_sel_osc_fast:1;
        uint32_t clk_lp_timer_sel_xtal:1;
        uint32_t clk_lp_timer_sel_xtal32k:1;
        uint32_t clk_lp_timer_div_num:12;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} modem_lpcon_lp_timer_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_coex_lp_sel_osc_slow:1;
        uint32_t clk_coex_lp_sel_osc_fast:1;
        uint32_t clk_coex_lp_sel_xtal:1;
        uint32_t clk_coex_lp_sel_xtal32k:1;
        uint32_t clk_coex_lp_div_num:12;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} modem_lpcon_coex_lp_clk_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_wifipwr_lp_sel_osc_slow:1;
        uint32_t clk_wifipwr_lp_sel_osc_fast:1;
        uint32_t clk_wifipwr_lp_sel_xtal:1;
        uint32_t clk_wifipwr_lp_sel_xtal32k:1;
        uint32_t clk_wifipwr_lp_div_num:12;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} modem_lpcon_wifi_lp_clk_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_i2c_mst_sel_160m:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} modem_lpcon_i2c_mst_clk_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_modem_32k_sel:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} modem_lpcon_modem_32k_clk_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_wifipwr_en:1;
        uint32_t clk_coex_en:1;
        uint32_t clk_i2c_mst_en:1;
        uint32_t clk_lp_timer_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} modem_lpcon_clk_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_wifipwr_fo:1;
        uint32_t clk_coex_fo:1;
        uint32_t clk_i2c_mst_fo:1;
        uint32_t clk_lp_timer_fo:1;
        uint32_t clk_bcmem_fo:1;
        uint32_t clk_i2c_mst_mem_fo:1;
        uint32_t clk_chan_freq_mem_fo:1;
        uint32_t clk_pbus_mem_fo:1;
        uint32_t clk_agc_mem_fo:1;
        uint32_t clk_dc_mem_fo:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} modem_lpcon_clk_conf_force_on_reg_t;

typedef union {
    struct {
        uint32_t reserved_0:16;
        uint32_t clk_wifipwr_st_map:4;
        uint32_t clk_coex_st_map:4;
        uint32_t clk_i2c_mst_st_map:4;
        uint32_t clk_lp_apb_st_map:4;
    };
    uint32_t val;
} modem_lpcon_clk_conf_power_st_reg_t;

typedef union {
    struct {
        uint32_t rst_wifipwr:1;
        uint32_t rst_coex:1;
        uint32_t rst_i2c_mst:1;
        uint32_t rst_lp_timer:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} modem_lpcon_rst_conf_reg_t;

typedef union {
    struct {
        uint32_t dc_mem_force_pu:1;
        uint32_t dc_mem_force_pd:1;
        uint32_t agc_mem_force_pu:1;
        uint32_t agc_mem_force_pd:1;
        uint32_t pbus_mem_force_pu:1;
        uint32_t pbus_mem_force_pd:1;
        uint32_t bc_mem_force_pu:1;
        uint32_t bc_mem_force_pd:1;
        uint32_t i2c_mst_mem_force_pu:1;
        uint32_t i2c_mst_mem_force_pd:1;
        uint32_t chan_freq_mem_force_pu:1;
        uint32_t chan_freq_mem_force_pd:1;
        uint32_t modem_pwr_mem_wp:3;
        uint32_t modem_pwr_mem_wa:3;
        uint32_t modem_pwr_mem_ra:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} modem_lpcon_mem_conf_reg_t;

typedef union {
    struct {
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} modem_lpcon_date_reg_t;


typedef struct {
    volatile modem_lpcon_test_conf_reg_t test_conf;
    volatile modem_lpcon_lp_timer_conf_reg_t lp_timer_conf;
    volatile modem_lpcon_coex_lp_clk_conf_reg_t coex_lp_clk_conf;
    volatile modem_lpcon_wifi_lp_clk_conf_reg_t wifi_lp_clk_conf;
    volatile modem_lpcon_i2c_mst_clk_conf_reg_t i2c_mst_clk_conf;
    volatile modem_lpcon_modem_32k_clk_conf_reg_t modem_32k_clk_conf;
    volatile modem_lpcon_clk_conf_reg_t clk_conf;
    volatile modem_lpcon_clk_conf_force_on_reg_t clk_conf_force_on;
    volatile modem_lpcon_clk_conf_power_st_reg_t clk_conf_power_st;
    volatile modem_lpcon_rst_conf_reg_t rst_conf;
    volatile modem_lpcon_mem_conf_reg_t mem_conf;
    volatile modem_lpcon_date_reg_t date;
} modem_lpcon_dev_t;

extern modem_lpcon_dev_t MODEM_LPCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_lpcon_dev_t) == 0x30, "Invalid size of modem_lpcon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
