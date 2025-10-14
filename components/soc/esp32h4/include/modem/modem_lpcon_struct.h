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
/* Type of test_conf register*/
typedef union {
    struct {
        /* clk_en : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_en: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} modem_lpcon_test_conf_reg_t;

/* Type of lp_timer_conf register*/
typedef union {
    struct {
        /* clk_lp_timer_sel_osc_slow : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_lp_timer_sel_osc_slow: 1;
        /* clk_lp_timer_sel_osc_fast : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_lp_timer_sel_osc_fast: 1;
        /* clk_lp_timer_sel_xtal : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_lp_timer_sel_xtal: 1;
        /* clk_lp_timer_sel_xtal32k : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_lp_timer_sel_xtal32k: 1;
        /* clk_lp_timer_div_num : R/W; bitpos: [15:4]; default: 0;*/
        uint32_t clk_lp_timer_div_num: 12;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} modem_lpcon_lp_timer_conf_reg_t;

/* Type of coex_lp_clk_conf register*/
typedef union {
    struct {
        /* clk_coex_lp_sel_osc_slow : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_coex_lp_sel_osc_slow: 1;
        /* clk_coex_lp_sel_osc_fast : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_coex_lp_sel_osc_fast: 1;
        /* clk_coex_lp_sel_xtal : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_coex_lp_sel_xtal: 1;
        /* clk_coex_lp_sel_xtal32k : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_coex_lp_sel_xtal32k: 1;
        /* clk_coex_lp_div_num : R/W; bitpos: [15:4]; default: 0;*/
        uint32_t clk_coex_lp_div_num: 12;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} modem_lpcon_coex_lp_clk_conf_reg_t;

/* Type of wifi_lp_clk_conf register*/
typedef union {
    struct {
        /* clk_wifipwr_lp_sel_osc_slow : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_wifipwr_lp_sel_osc_slow: 1;
        /* clk_wifipwr_lp_sel_osc_fast : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_wifipwr_lp_sel_osc_fast: 1;
        /* clk_wifipwr_lp_sel_xtal : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_wifipwr_lp_sel_xtal: 1;
        /* clk_wifipwr_lp_sel_xtal32k : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_wifipwr_lp_sel_xtal32k: 1;
        /* clk_wifipwr_lp_div_num : R/W; bitpos: [15:4]; default: 0;*/
        uint32_t clk_wifipwr_lp_div_num: 12;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} modem_lpcon_wifi_lp_clk_conf_reg_t;

/* Type of modem_src_clk_conf register*/
typedef union {
    struct {
        /* clk_modem_aon_force : R/W; bitpos: [1:0]; default: 0;*/
        uint32_t clk_modem_aon_force: 2;
        /* modem_pwr_32k_fo : R/W; bitpos: [2]; default: 0;*/
        uint32_t modem_pwr_32k_fo: 1;
        /* modem_pwr_fosc_fo : R/W; bitpos: [3]; default: 0;*/
        uint32_t modem_pwr_fosc_fo: 1;
        /* modem_pwr_sosc_fo : R/W; bitpos: [4]; default: 0;*/
        uint32_t modem_pwr_sosc_fo: 1;
        /* modem_pwr_xtal_fo : R/W; bitpos: [5]; default: 0;*/
        uint32_t modem_pwr_xtal_fo: 1;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} modem_lpcon_modem_src_clk_conf_reg_t;

/* Type of modem_32k_clk_conf register*/
typedef union {
    struct {
        /* clk_modem_32k_sel : R/W; bitpos: [1:0]; default: 0;*/
        uint32_t clk_modem_32k_sel: 2;
        uint32_t reserved_2: 30;
    };
    uint32_t val;
} modem_lpcon_modem_32k_clk_conf_reg_t;

/* Type of clk_conf register*/
typedef union {
    struct {
        /* clk_wifipwr_en : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_wifipwr_en: 1;
        /* clk_coex_en : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_coex_en: 1;
        /* clk_i2c_mst_en : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_i2c_mst_en: 1;
        /* clk_lp_timer_en : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_lp_timer_en: 1;
        /* clk_ana_xtal_en : R/W; bitpos: [4]; default: 1;*/
        uint32_t clk_ana_xtal_en: 1;
        uint32_t reserved_5: 27;
    };
    uint32_t val;
} modem_lpcon_clk_conf_reg_t;

/* Type of clk_conf_force_on register*/
typedef union {
    struct {
        /* clk_wifipwr_fo : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_wifipwr_fo: 1;
        /* clk_coex_fo : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_coex_fo: 1;
        /* clk_i2c_mst_fo : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_i2c_mst_fo: 1;
        /* clk_lp_timer_fo : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_lp_timer_fo: 1;
        /* clk_agc_mem_fo : R/W; bitpos: [4]; default: 0;*/
        uint32_t clk_agc_mem_fo: 1;
        /* clk_pbus_mem_fo : R/W; bitpos: [5]; default: 0;*/
        uint32_t clk_pbus_mem_fo: 1;
        /* clk_chan_freq_mem_fo : R/W; bitpos: [6]; default: 0;*/
        uint32_t clk_chan_freq_mem_fo: 1;
        /* clk_agc_dcmem_fo : R/W; bitpos: [7]; default: 0;*/
        uint32_t clk_agc_dcmem_fo: 1;
        /* clk_bcmem_fo : R/W; bitpos: [8]; default: 0;*/
        uint32_t clk_bcmem_fo: 1;
        /* clk_i2c_mst_mem_fo : R/W; bitpos: [9]; default: 0;*/
        uint32_t clk_i2c_mst_mem_fo: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} modem_lpcon_clk_conf_force_on_reg_t;

/* Type of clk_conf_power_st register*/
typedef union {
    struct {
        uint32_t reserved_0: 16;
        /* clk_wifipwr_st_map : R/W; bitpos: [19:16]; default: 0;*/
        uint32_t clk_wifipwr_st_map: 4;
        /* clk_coex_st_map : R/W; bitpos: [23:20]; default: 0;*/
        uint32_t clk_coex_st_map: 4;
        /* clk_i2c_mst_st_map : R/W; bitpos: [27:24]; default: 0;*/
        uint32_t clk_i2c_mst_st_map: 4;
        /* clk_lp_apb_st_map : R/W; bitpos: [31:28]; default: 0;*/
        uint32_t clk_lp_apb_st_map: 4;
    };
    uint32_t val;
} modem_lpcon_clk_conf_power_st_reg_t;

/* Type of rst_conf register*/
typedef union {
    struct {
        /* rst_wifipwr : WO; bitpos: [0]; default: 0;*/
        uint32_t rst_wifipwr: 1;
        /* rst_coex : WO; bitpos: [1]; default: 0;*/
        uint32_t rst_coex: 1;
        /* rst_i2c_mst : WO; bitpos: [2]; default: 0;*/
        uint32_t rst_i2c_mst: 1;
        /* rst_lp_timer : WO; bitpos: [3]; default: 0;*/
        uint32_t rst_lp_timer: 1;
        /* rst_dcmem : WO; bitpos: [4]; default: 0;*/
        uint32_t rst_dcmem: 1;
        /* rst_modem_power_ctrl : WO; bitpos: [5]; default: 0;*/
        uint32_t rst_modem_power_ctrl: 1;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} modem_lpcon_rst_conf_reg_t;

/* Type of tick_conf register*/
typedef union {
    struct {
        /* modem_pwr_tick_target : R/W; bitpos: [5:0]; default: 39;*/
        uint32_t modem_pwr_tick_target: 6;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} modem_lpcon_tick_conf_reg_t;

/* Type of mem_conf register*/
typedef union {
    struct {
        /* dc_mem_mode : R/W; bitpos: [2:0]; default: 0;*/
        uint32_t dc_mem_mode: 3;
        /* dc_mem_force : R/W; bitpos: [3]; default: 1;*/
        uint32_t dc_mem_force: 1;
        /* agc_mem_mode : R/W; bitpos: [6:4]; default: 0;*/
        uint32_t agc_mem_mode: 3;
        /* agc_mem_force : R/W; bitpos: [7]; default: 1;*/
        uint32_t agc_mem_force: 1;
        /* pbus_mem_mode : R/W; bitpos: [10:8]; default: 0;*/
        uint32_t pbus_mem_mode: 3;
        /* pbus_mem_force : R/W; bitpos: [11]; default: 1;*/
        uint32_t pbus_mem_force: 1;
        /* bc_mem_mode : R/W; bitpos: [14:12]; default: 0;*/
        uint32_t bc_mem_mode: 3;
        /* bc_mem_force : R/W; bitpos: [15]; default: 1;*/
        uint32_t bc_mem_force: 1;
        /* i2c_mst_mem_mode : R/W; bitpos: [18:16]; default: 0;*/
        uint32_t i2c_mst_mem_mode: 3;
        /* i2c_mst_mem_force : R/W; bitpos: [19]; default: 1;*/
        uint32_t i2c_mst_mem_force: 1;
        /* chan_freq_mem_mode : R/W; bitpos: [22:20]; default: 0;*/
        uint32_t chan_freq_mem_mode: 3;
        /* chan_freq_mem_force : R/W; bitpos: [23]; default: 1;*/
        uint32_t chan_freq_mem_force: 1;
        uint32_t reserved_24: 8;
    };
    uint32_t val;
} modem_lpcon_mem_conf_reg_t;

/* Type of mem_rf1_aux_ctrl register*/
typedef union {
    struct {
        /* modem_pwr_rf1_aux_ctrl : R/W; bitpos: [31:0]; default: 10320;*/
        uint32_t modem_pwr_rf1_aux_ctrl: 32;
    };
    uint32_t val;
} modem_lpcon_mem_rf1_aux_ctrl_reg_t;

/* Type of mem_rf2_aux_ctrl register*/
typedef union {
    struct {
        /* modem_pwr_rf2_aux_ctrl : R/W; bitpos: [31:0]; default: 0;*/
        uint32_t modem_pwr_rf2_aux_ctrl: 32;
    };
    uint32_t val;
} modem_lpcon_mem_rf2_aux_ctrl_reg_t;

/* Type of apb_mem_sel register*/
typedef union {
    struct {
        /* chan_freq_mem_en : R/W; bitpos: [0]; default: 0;*/
        uint32_t chan_freq_mem_en: 1;
        /* pbus_mem_en : R/W; bitpos: [1]; default: 0;*/
        uint32_t pbus_mem_en: 1;
        /* agc_mem_en : R/W; bitpos: [2]; default: 0;*/
        uint32_t agc_mem_en: 1;
        /* pwr_mem_base : R/W; bitpos: [18:3]; default: 0;*/
        uint32_t pwr_mem_base: 16;
        uint32_t reserved_19: 13;
    };
    uint32_t val;
} modem_lpcon_apb_mem_sel_reg_t;

/* Type of dcmem_valid_0 register*/
typedef union {
    struct {
        /* dcmem_valid_0 : RO; bitpos: [31:0]; default: 0;*/
        uint32_t dcmem_valid_0: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_0_reg_t;

/* Type of dcmem_valid_1 register*/
typedef union {
    struct {
        /* dcmem_valid_1 : RO; bitpos: [31:0]; default: 0;*/
        uint32_t dcmem_valid_1: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_1_reg_t;

/* Type of dcmem_valid_2 register*/
typedef union {
    struct {
        /* dcmem_valid_2 : RO; bitpos: [31:0]; default: 0;*/
        uint32_t dcmem_valid_2: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_2_reg_t;

/* Type of dcmem_valid_3 register*/
typedef union {
    struct {
        /* dcmem_valid_3 : RO; bitpos: [31:0]; default: 0;*/
        uint32_t dcmem_valid_3: 32;
    };
    uint32_t val;
} modem_lpcon_dcmem_valid_3_reg_t;

/* Type of date register*/
typedef union {
    struct {
        /* date : R/W; bitpos: [27:0]; default: 37823024;*/
        uint32_t date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} modem_lpcon_date_reg_t;

typedef struct {
    volatile modem_lpcon_test_conf_reg_t test_conf;
    volatile modem_lpcon_lp_timer_conf_reg_t lp_timer_conf;
    volatile modem_lpcon_coex_lp_clk_conf_reg_t coex_lp_clk_conf;
    volatile modem_lpcon_wifi_lp_clk_conf_reg_t wifi_lp_clk_conf;
    volatile modem_lpcon_modem_src_clk_conf_reg_t modem_src_clk_conf;
    volatile modem_lpcon_modem_32k_clk_conf_reg_t modem_32k_clk_conf;
    volatile modem_lpcon_clk_conf_reg_t clk_conf;
    volatile modem_lpcon_clk_conf_force_on_reg_t clk_conf_force_on;
    volatile modem_lpcon_clk_conf_power_st_reg_t clk_conf_power_st;
    volatile modem_lpcon_rst_conf_reg_t rst_conf;
    volatile modem_lpcon_tick_conf_reg_t tick_conf;
    volatile modem_lpcon_mem_conf_reg_t mem_conf;
    volatile modem_lpcon_mem_rf1_aux_ctrl_reg_t mem_rf1_aux_ctrl;
    volatile modem_lpcon_mem_rf2_aux_ctrl_reg_t mem_rf2_aux_ctrl;
    volatile modem_lpcon_apb_mem_sel_reg_t apb_mem_sel;
    volatile modem_lpcon_dcmem_valid_0_reg_t dcmem_valid_0;
    volatile modem_lpcon_dcmem_valid_1_reg_t dcmem_valid_1;
    volatile modem_lpcon_dcmem_valid_2_reg_t dcmem_valid_2;
    volatile modem_lpcon_dcmem_valid_3_reg_t dcmem_valid_3;
    volatile modem_lpcon_date_reg_t date;
} modem_lpcon_dev_t;

extern modem_lpcon_dev_t MODEM_LPCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_lpcon_dev_t) == 0x50, "Invalid size of modem_lpcon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
