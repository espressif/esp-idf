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

/** Group: Configuration Register */
/** Type of test_conf register
 *  No description
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  No description
         */
        uint32_t clk_en: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} modem_lpcon_test_conf_reg_t;

/** Type of coex_lp_clk_conf register
 *  No description
 */
typedef union {
    struct {
        /** clk_coex_lp_sel_osc_slow : R/W; bitpos: [0]; default: 0;
         *  No description
         */
        uint32_t clk_coex_lp_sel_osc_slow: 1;
        /** clk_coex_lp_sel_osc_fast : R/W; bitpos: [1]; default: 0;
         *  No description
         */
        uint32_t clk_coex_lp_sel_osc_fast: 1;
        /** clk_coex_lp_sel_xtal : R/W; bitpos: [2]; default: 0;
         *  No description
         */
        uint32_t clk_coex_lp_sel_xtal: 1;
        /** clk_coex_lp_sel_xtal32k : R/W; bitpos: [3]; default: 0;
         *  No description
         */
        uint32_t clk_coex_lp_sel_xtal32k: 1;
        /** clk_coex_lp_div_num : R/W; bitpos: [15:4]; default: 0;
         *  No description
         */
        uint32_t clk_coex_lp_div_num: 12;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} modem_lpcon_coex_lp_clk_conf_reg_t;

/** Type of clk_conf register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 1;
        /** clk_coex_en : R/W; bitpos: [1]; default: 0;
         *  No description
         */
        uint32_t clk_coex_en: 1;
        /** clk_i2c_mst_en : R/W; bitpos: [2]; default: 0;
         *  No description
         */
        uint32_t clk_i2c_mst_en: 1;
        uint32_t reserved_3: 2;
        /** clk_fe_mem_en : R/W; bitpos: [5]; default: 0;
         *  No description
         */
        uint32_t clk_fe_mem_en: 1;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} modem_lpcon_clk_conf_reg_t;

/** Type of clk_conf_force_on register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 1;
        /** clk_coex_fo : R/W; bitpos: [1]; default: 0;
         *  No description
         */
        uint32_t clk_coex_fo: 1;
        /** clk_i2c_mst_fo : R/W; bitpos: [2]; default: 0;
         *  No description
         */
        uint32_t clk_i2c_mst_fo: 1;
        uint32_t reserved_3: 2;
        /** clk_fe_mem_fo : R/W; bitpos: [5]; default: 0;
         *  No description
         */
        uint32_t clk_fe_mem_fo: 1;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} modem_lpcon_clk_conf_force_on_reg_t;

/** Type of tick_conf register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 6;
        /** pwr_tick_target : RO; bitpos: [31:6]; default: 0;
         *  No description
         */
        uint32_t pwr_tick_target: 26;
    };
    uint32_t val;
} modem_lpcon_tick_conf_reg_t;

/** Type of rst_conf register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 1;
        /** rst_coex : WO; bitpos: [1]; default: 0;
         *  No description
         */
        uint32_t rst_coex: 1;
        /** rst_i2c_mst : WO; bitpos: [2]; default: 0;
         *  No description
         */
        uint32_t rst_i2c_mst: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} modem_lpcon_rst_conf_reg_t;

/** Type of mem_conf register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 2;
        /** agc_mem_force_pu : R/W; bitpos: [2]; default: 1;
         *  No description
         */
        uint32_t agc_mem_force_pu: 1;
        /** agc_mem_force_pd : R/W; bitpos: [3]; default: 0;
         *  No description
         */
        uint32_t agc_mem_force_pd: 1;
        /** pbus_mem_force_pu : R/W; bitpos: [4]; default: 1;
         *  No description
         */
        uint32_t pbus_mem_force_pu: 1;
        /** pbus_mem_force_pd : R/W; bitpos: [5]; default: 0;
         *  No description
         */
        uint32_t pbus_mem_force_pd: 1;
        uint32_t reserved_6: 2;
        /** i2c_mst_mem_force_pu : R/W; bitpos: [8]; default: 0;
         *  No description
         */
        uint32_t i2c_mst_mem_force_pu: 1;
        /** i2c_mst_mem_force_pd : R/W; bitpos: [9]; default: 0;
         *  No description
         */
        uint32_t i2c_mst_mem_force_pd: 1;
        /** chan_freq_mem_force_pu : R/W; bitpos: [10]; default: 0;
         *  No description
         */
        uint32_t chan_freq_mem_force_pu: 1;
        /** chan_freq_mem_force_pd : R/W; bitpos: [11]; default: 0;
         *  No description
         */
        uint32_t chan_freq_mem_force_pd: 1;
        /** modem_pwr_mem_wp : R/W; bitpos: [14:12]; default: 0;
         *  No description
         */
        uint32_t modem_pwr_mem_wp: 3;
        /** modem_pwr_mem_wa : R/W; bitpos: [17:15]; default: 5;
         *  No description
         */
        uint32_t modem_pwr_mem_wa: 3;
        /** modem_pwr_mem_ra : R/W; bitpos: [19:18]; default: 0;
         *  No description
         */
        uint32_t modem_pwr_mem_ra: 2;
        /** modem_pwr_mem_rm : R/W; bitpos: [23:20]; default: 2;
         *  No description
         */
        uint32_t modem_pwr_mem_rm: 4;
        uint32_t reserved_24: 8;
    };
    uint32_t val;
} modem_lpcon_mem_conf_reg_t;

/** Group: Version Register */
/** Type of date register
 *  No description
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35689088;
         *  No description
         */
        uint32_t date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} modem_lpcon_date_reg_t;

typedef struct {
    volatile modem_lpcon_test_conf_reg_t test_conf;
    volatile modem_lpcon_coex_lp_clk_conf_reg_t coex_lp_clk_conf;
    volatile modem_lpcon_clk_conf_reg_t clk_conf;
    volatile modem_lpcon_clk_conf_force_on_reg_t clk_conf_force_on;
    volatile modem_lpcon_tick_conf_reg_t tick_conf;
    volatile modem_lpcon_rst_conf_reg_t rst_conf;
    volatile modem_lpcon_mem_conf_reg_t mem_conf;
    volatile modem_lpcon_date_reg_t date;
} modem_lpcon_dev_t;

extern modem_lpcon_dev_t MODEM_LPCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_lpcon_dev_t) == 0x20, "Invalid size of modem_lpcon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
