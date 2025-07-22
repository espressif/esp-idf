/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
typedef union {
    struct {
        uint32_t clk_en: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} modem_lpcon_test_conf_t;

typedef union {
    struct {
        uint32_t clk_lp_timer_sel_osc_slow: 1;
        uint32_t clk_lp_timer_sel_osc_fast: 1;
        uint32_t clk_lp_timer_sel_xtal: 1;
        uint32_t clk_lp_timer_sel_xtal32k: 1;
        uint32_t clk_lp_timer_div_num: 12;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} modem_lpcon_lp_timer_conf_t;

typedef union {
    struct {
        uint32_t clk_coex_lp_sel_osc_slow: 1;
        uint32_t clk_coex_lp_sel_osc_fast: 1;
        uint32_t clk_coex_lp_sel_xtal: 1;
        uint32_t clk_coex_lp_sel_xtal32k: 1;
        uint32_t clk_coex_lp_div_num: 12;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} modem_lpcon_coex_lp_clk_conf_t;

typedef union {
    struct {
        uint32_t clk_wifipwr_lp_sel_osc_slow: 1;
        uint32_t clk_wifipwr_lp_sel_osc_fast: 1;
        uint32_t clk_wifipwr_lp_sel_xtal: 1;
        uint32_t clk_wifipwr_lp_sel_xtal32k: 1;
        uint32_t clk_wifipwr_lp_div_num: 12;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} modem_lpcon_wifi_lp_clk_conf_t;

typedef union {
    struct {
        uint32_t clk_modem_aon_force: 2;
        uint32_t modem_pwr_clk_src_fo: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} modem_lpcon_modem_src_clk_conf_t;

typedef union {
    struct {
        uint32_t clk_modem_32k_sel: 2;
        uint32_t reserved_2: 30;
    };
    uint32_t val;
} modem_lpcon_modem_32k_clk_conf_t;

typedef union {
    struct {
        uint32_t clk_wifipwr_en: 1;
        uint32_t clk_coex_en: 1;
        uint32_t clk_i2c_mst_en: 1;
        uint32_t clk_lp_timer_en: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} modem_lpcon_clk_conf_t;

typedef union {
    struct {
        uint32_t clk_wifipwr_fo: 1;
        uint32_t clk_coex_fo: 1;
        uint32_t clk_i2c_mst_fo: 1;
        uint32_t clk_lp_timer_fo: 1;
        uint32_t clk_fe_mem_fo: 1;
        uint32_t reserved_5: 27;
    };
    uint32_t val;
} modem_lpcon_clk_conf_force_on_t;

typedef union {
    struct {
        uint32_t reserved_0: 16;
        uint32_t clk_wifipwr_st_map: 4;
        uint32_t clk_coex_st_map: 4;
        uint32_t clk_i2c_mst_st_map: 4;
        uint32_t clk_lp_apb_st_map: 4;
    };
    uint32_t val;
} modem_lpcon_clk_conf_power_st_t;

typedef union {
    struct {
        uint32_t rst_wifipwr: 1;
        uint32_t rst_coex: 1;
        uint32_t rst_i2c_mst: 1;
        uint32_t rst_lp_timer: 1;
        uint32_t rst_dcmem: 1;
        uint32_t reserved_5: 27;
    };
    uint32_t val;
} modem_lpcon_rst_conf_t;

typedef union {
    struct {
        uint32_t modem_pwr_tick_target: 6;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} modem_lpcon_tick_conf_t;

typedef union {
    struct {
        uint32_t dc_mem_mode: 3;
        uint32_t dc_mem_force: 1;
        uint32_t agc_mem_mode: 3;
        uint32_t agc_mem_force: 1;
        uint32_t pbus_mem_mode: 3;
        uint32_t pbus_mem_force: 1;
        uint32_t bc_mem_mode: 3;
        uint32_t bc_mem_force: 1;
        uint32_t i2c_mst_mem_mode: 3;
        uint32_t i2c_mst_mem_force: 1;
        uint32_t chan_freq_mem_mode: 3;
        uint32_t chan_freq_mem_force: 1;
        uint32_t reserved_24: 8;
    };
    uint32_t val;
} modem_lpcon_mem_conf_t;

typedef union {
    struct {
        uint32_t modem_pwr_rf1_aux_ctrl: 32;
    };
    uint32_t val;
} modem_lpcon_mem_rf1_aux_ctrl_t;

typedef union {
    struct {
        uint32_t modem_pwr_rf2_aux_ctrl: 32;
    };
    uint32_t val;
} modem_lpcon_mem_rf2_aux_ctrl_t;

typedef union {
    struct {
        uint32_t chan_freq_mem_en: 1;
        uint32_t pbus_mem_en: 1;
        uint32_t agc_mem_en: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} modem_lpcon_apb_mem_sel_t;

typedef union {
    struct {
        uint32_t _dcmem_valid_0: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_0_t;

typedef union {
    struct {
        uint32_t _dcmem_valid_1: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_1_t;

typedef union {
    struct {
        uint32_t _dcmem_valid_2: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_2_t;

typedef union {
    struct {
        uint32_t _dcmem_valid_3: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_3_t;

typedef union {
    struct {
        uint32_t date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} modem_lpcon_date_t;

typedef struct {
    volatile modem_lpcon_test_conf_t test_conf;
    volatile modem_lpcon_lp_timer_conf_t lp_timer_conf;
    volatile modem_lpcon_coex_lp_clk_conf_t coex_lp_clk_conf;
    volatile modem_lpcon_wifi_lp_clk_conf_t wifi_lp_clk_conf;
    volatile modem_lpcon_modem_src_clk_conf_t modem_src_clk_conf;
    volatile modem_lpcon_modem_32k_clk_conf_t modem_32k_clk_conf;
    volatile modem_lpcon_clk_conf_t clk_conf;
    volatile modem_lpcon_clk_conf_force_on_t clk_conf_force_on;
    volatile modem_lpcon_clk_conf_power_st_t clk_conf_power_st;
    volatile modem_lpcon_rst_conf_t rst_conf;
    volatile modem_lpcon_tick_conf_t tick_conf;
    volatile modem_lpcon_mem_conf_t mem_conf;
    volatile modem_lpcon_mem_rf1_aux_ctrl_t mem_rf1_aux_ctrl;
    volatile modem_lpcon_mem_rf2_aux_ctrl_t mem_rf2_aux_ctrl;
    volatile modem_lpcon_apb_mem_sel_t apb_mem_sel;
    volatile modem_lpcon_dcmem_valid_0_t dcmem_valid_0;
    volatile modem_lpcon_dcmem_valid_1_t dcmem_valid_1;
    volatile modem_lpcon_dcmem_valid_2_t dcmem_valid_2;
    volatile modem_lpcon_dcmem_valid_3_t dcmem_valid_3;
    volatile modem_lpcon_date_t date;
} modem_lpcon_dev_t;

extern modem_lpcon_dev_t MODEM_LPCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_lpcon_dev_t) == 0x50, "Invalid size of modem_lpcon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
