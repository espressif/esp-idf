/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of hp_active_dig_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** hp_active_vdd_spi_pd_en : R/W; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t hp_active_vdd_spi_pd_en:1;
        /** hp_active_hp_mem_dslp : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t hp_active_hp_mem_dslp:1;
        /** hp_active_pd_hp_mem_pd_en : R/W; bitpos: [26:23]; default: 0;
         *  need_des
         */
        uint32_t hp_active_pd_hp_mem_pd_en:4;
        /** hp_active_pd_hp_wifi_pd_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_active_pd_hp_wifi_pd_en:1;
        uint32_t reserved_28:1;
        /** hp_active_pd_hp_cpu_pd_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_active_pd_hp_cpu_pd_en:1;
        /** hp_active_pd_hp_aon_pd_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_active_pd_hp_aon_pd_en:1;
        /** hp_active_pd_top_pd_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_active_pd_top_pd_en:1;
    };
    uint32_t val;
} pmu_hp_active_dig_power_reg_t;

/** Type of hp_active_icg_hp_func register
 *  need_des
 */
typedef union {
    struct {
        /** hp_active_dig_icg_func_en : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t hp_active_dig_icg_func_en:32;
    };
    uint32_t val;
} pmu_hp_active_icg_hp_func_reg_t;

/** Type of hp_active_icg_hp_apb register
 *  need_des
 */
typedef union {
    struct {
        /** hp_active_dig_icg_apb_en : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t hp_active_dig_icg_apb_en:32;
    };
    uint32_t val;
} pmu_hp_active_icg_hp_apb_reg_t;

/** Type of hp_active_icg_modem register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** hp_active_dig_icg_modem_code : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t hp_active_dig_icg_modem_code:2;
    };
    uint32_t val;
} pmu_hp_active_icg_modem_reg_t;

/** Type of hp_active_hp_sys_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** hp_active_uart_wakeup_en : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t hp_active_uart_wakeup_en:1;
        /** hp_active_lp_pad_hold_all : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t hp_active_lp_pad_hold_all:1;
        /** hp_active_hp_pad_hold_all : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_active_hp_pad_hold_all:1;
        /** hp_active_dig_pad_slp_sel : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_active_dig_pad_slp_sel:1;
        /** hp_active_dig_pause_wdt : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_active_dig_pause_wdt:1;
        /** hp_active_dig_cpu_stall : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_active_dig_cpu_stall:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} pmu_hp_active_hp_sys_cntl_reg_t;

/** Type of hp_active_hp_ck_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** hp_active_i2c_iso_en : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_active_i2c_iso_en:1;
        /** hp_active_i2c_retention : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_active_i2c_retention:1;
        /** hp_active_xpd_bb_i2c : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_active_xpd_bb_i2c:1;
        /** hp_active_xpd_bbpll_i2c : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_active_xpd_bbpll_i2c:1;
        /** hp_active_xpd_bbpll : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_active_xpd_bbpll:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} pmu_hp_active_hp_ck_power_reg_t;

/** Type of hp_active_bias register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** hp_active_xpd_bias : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t hp_active_xpd_bias:1;
        /** hp_active_dbg_atten : R/W; bitpos: [29:26]; default: 0;
         *  need_des
         */
        uint32_t hp_active_dbg_atten:4;
        /** hp_active_pd_cur : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_active_pd_cur:1;
        /** hp_active_bias_sleep : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_active_bias_sleep:1;
    };
    uint32_t val;
} pmu_hp_active_bias_reg_t;

/** Type of hp_active_backup register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** hp_sleep2active_backup_modem_clk_code : R/W; bitpos: [5:4]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2active_backup_modem_clk_code:2;
        /** hp_modem2active_backup_modem_clk_code : R/W; bitpos: [7:6]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2active_backup_modem_clk_code:2;
        uint32_t reserved_8:2;
        /** hp_active_retention_mode : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t hp_active_retention_mode:1;
        /** hp_sleep2active_retention_en : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2active_retention_en:1;
        /** hp_modem2active_retention_en : R/W; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2active_retention_en:1;
        uint32_t reserved_13:1;
        /** hp_sleep2active_backup_clk_sel : R/W; bitpos: [15:14]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2active_backup_clk_sel:2;
        /** hp_modem2active_backup_clk_sel : R/W; bitpos: [17:16]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2active_backup_clk_sel:2;
        uint32_t reserved_18:2;
        /** hp_sleep2active_backup_mode : R/W; bitpos: [22:20]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2active_backup_mode:3;
        /** hp_modem2active_backup_mode : R/W; bitpos: [25:23]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2active_backup_mode:3;
        uint32_t reserved_26:3;
        /** hp_sleep2active_backup_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2active_backup_en:1;
        /** hp_modem2active_backup_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2active_backup_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} pmu_hp_active_backup_reg_t;

/** Type of hp_active_backup_clk register
 *  need_des
 */
typedef union {
    struct {
        /** hp_active_backup_icg_func_en : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t hp_active_backup_icg_func_en:32;
    };
    uint32_t val;
} pmu_hp_active_backup_clk_reg_t;

/** Type of hp_active_sysclk register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** hp_active_dig_sys_clk_no_div : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_active_dig_sys_clk_no_div:1;
        /** hp_active_icg_sys_clock_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_active_icg_sys_clock_en:1;
        /** hp_active_sys_clk_slp_sel : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_active_sys_clk_slp_sel:1;
        /** hp_active_icg_slp_sel : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_active_icg_slp_sel:1;
        /** hp_active_dig_sys_clk_sel : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t hp_active_dig_sys_clk_sel:2;
    };
    uint32_t val;
} pmu_hp_active_sysclk_reg_t;

/** Type of hp_active_hp_regulator0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** lp_dbias_vol : RO; bitpos: [8:4]; default: 24;
         *  need_des
         */
        uint32_t lp_dbias_vol:5;
        /** hp_dbias_vol : RO; bitpos: [13:9]; default: 24;
         *  need_des
         */
        uint32_t hp_dbias_vol:5;
        /** dig_regulator0_dbias_sel : R/W; bitpos: [14]; default: 1;
         *  need_des
         */
        uint32_t dig_regulator0_dbias_sel:1;
        /** dig_dbias_init : WT; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t dig_dbias_init:1;
        /** hp_active_hp_regulator_slp_mem_xpd : R/W; bitpos: [16]; default: 1;
         *  need_des
         */
        uint32_t hp_active_hp_regulator_slp_mem_xpd:1;
        /** hp_active_hp_regulator_slp_logic_xpd : R/W; bitpos: [17]; default: 1;
         *  need_des
         */
        uint32_t hp_active_hp_regulator_slp_logic_xpd:1;
        /** hp_active_hp_regulator_xpd : R/W; bitpos: [18]; default: 1;
         *  need_des
         */
        uint32_t hp_active_hp_regulator_xpd:1;
        /** hp_active_hp_regulator_slp_mem_dbias : R/W; bitpos: [22:19]; default: 12;
         *  need_des
         */
        uint32_t hp_active_hp_regulator_slp_mem_dbias:4;
        /** hp_active_hp_regulator_slp_logic_dbias : R/W; bitpos: [26:23]; default: 12;
         *  need_des
         */
        uint32_t hp_active_hp_regulator_slp_logic_dbias:4;
        /** hp_active_hp_regulator_dbias : R/W; bitpos: [31:27]; default: 24;
         *  need_des
         */
        uint32_t hp_active_hp_regulator_dbias:5;
    };
    uint32_t val;
} pmu_hp_active_hp_regulator0_reg_t;

/** Type of hp_active_hp_regulator1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** hp_active_hp_regulator_drv_b : R/W; bitpos: [31:8]; default: 0;
         *  need_des
         */
        uint32_t hp_active_hp_regulator_drv_b:24;
    };
    uint32_t val;
} pmu_hp_active_hp_regulator1_reg_t;

/** Type of hp_active_xtal register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_active_xpd_xtal : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t hp_active_xpd_xtal:1;
    };
    uint32_t val;
} pmu_hp_active_xtal_reg_t;

/** Type of hp_modem_dig_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** hp_modem_vdd_spi_pd_en : R/W; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_vdd_spi_pd_en:1;
        /** hp_modem_hp_mem_dslp : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_hp_mem_dslp:1;
        /** hp_modem_pd_hp_mem_pd_en : R/W; bitpos: [26:23]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_pd_hp_mem_pd_en:4;
        /** hp_modem_pd_hp_wifi_pd_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_pd_hp_wifi_pd_en:1;
        uint32_t reserved_28:1;
        /** hp_modem_pd_hp_cpu_pd_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_pd_hp_cpu_pd_en:1;
        /** hp_modem_pd_hp_aon_pd_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_pd_hp_aon_pd_en:1;
        /** hp_modem_pd_top_pd_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_pd_top_pd_en:1;
    };
    uint32_t val;
} pmu_hp_modem_dig_power_reg_t;

/** Type of hp_modem_icg_hp_func register
 *  need_des
 */
typedef union {
    struct {
        /** hp_modem_dig_icg_func_en : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t hp_modem_dig_icg_func_en:32;
    };
    uint32_t val;
} pmu_hp_modem_icg_hp_func_reg_t;

/** Type of hp_modem_icg_hp_apb register
 *  need_des
 */
typedef union {
    struct {
        /** hp_modem_dig_icg_apb_en : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t hp_modem_dig_icg_apb_en:32;
    };
    uint32_t val;
} pmu_hp_modem_icg_hp_apb_reg_t;

/** Type of hp_modem_icg_modem register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** hp_modem_dig_icg_modem_code : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_dig_icg_modem_code:2;
    };
    uint32_t val;
} pmu_hp_modem_icg_modem_reg_t;

/** Type of hp_modem_hp_sys_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** hp_modem_uart_wakeup_en : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_uart_wakeup_en:1;
        /** hp_modem_lp_pad_hold_all : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_lp_pad_hold_all:1;
        /** hp_modem_hp_pad_hold_all : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_hp_pad_hold_all:1;
        /** hp_modem_dig_pad_slp_sel : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_dig_pad_slp_sel:1;
        /** hp_modem_dig_pause_wdt : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_dig_pause_wdt:1;
        /** hp_modem_dig_cpu_stall : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_dig_cpu_stall:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} pmu_hp_modem_hp_sys_cntl_reg_t;

/** Type of hp_modem_hp_ck_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** hp_modem_i2c_iso_en : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_i2c_iso_en:1;
        /** hp_modem_i2c_retention : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_i2c_retention:1;
        /** hp_modem_xpd_bb_i2c : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_xpd_bb_i2c:1;
        /** hp_modem_xpd_bbpll_i2c : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_xpd_bbpll_i2c:1;
        /** hp_modem_xpd_bbpll : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_xpd_bbpll:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} pmu_hp_modem_hp_ck_power_reg_t;

/** Type of hp_modem_bias register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** hp_modem_xpd_bias : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_xpd_bias:1;
        /** hp_modem_dbg_atten : R/W; bitpos: [29:26]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_dbg_atten:4;
        /** hp_modem_pd_cur : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_pd_cur:1;
        /** hp_modem_bias_sleep : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_bias_sleep:1;
    };
    uint32_t val;
} pmu_hp_modem_bias_reg_t;

/** Type of hp_modem_backup register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** hp_sleep2modem_backup_modem_clk_code : R/W; bitpos: [5:4]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2modem_backup_modem_clk_code:2;
        uint32_t reserved_6:4;
        /** hp_modem_retention_mode : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_retention_mode:1;
        /** hp_sleep2modem_retention_en : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2modem_retention_en:1;
        uint32_t reserved_12:2;
        /** hp_sleep2modem_backup_clk_sel : R/W; bitpos: [15:14]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2modem_backup_clk_sel:2;
        uint32_t reserved_16:4;
        /** hp_sleep2modem_backup_mode : R/W; bitpos: [22:20]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2modem_backup_mode:3;
        uint32_t reserved_23:6;
        /** hp_sleep2modem_backup_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep2modem_backup_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} pmu_hp_modem_backup_reg_t;

/** Type of hp_modem_backup_clk register
 *  need_des
 */
typedef union {
    struct {
        /** hp_modem_backup_icg_func_en : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_backup_icg_func_en:32;
    };
    uint32_t val;
} pmu_hp_modem_backup_clk_reg_t;

/** Type of hp_modem_sysclk register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** hp_modem_dig_sys_clk_no_div : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_dig_sys_clk_no_div:1;
        /** hp_modem_icg_sys_clock_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_icg_sys_clock_en:1;
        /** hp_modem_sys_clk_slp_sel : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_sys_clk_slp_sel:1;
        /** hp_modem_icg_slp_sel : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_icg_slp_sel:1;
        /** hp_modem_dig_sys_clk_sel : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_dig_sys_clk_sel:2;
    };
    uint32_t val;
} pmu_hp_modem_sysclk_reg_t;

/** Type of hp_modem_hp_regulator0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** hp_modem_hp_regulator_slp_mem_xpd : R/W; bitpos: [16]; default: 1;
         *  need_des
         */
        uint32_t hp_modem_hp_regulator_slp_mem_xpd:1;
        /** hp_modem_hp_regulator_slp_logic_xpd : R/W; bitpos: [17]; default: 1;
         *  need_des
         */
        uint32_t hp_modem_hp_regulator_slp_logic_xpd:1;
        /** hp_modem_hp_regulator_xpd : R/W; bitpos: [18]; default: 1;
         *  need_des
         */
        uint32_t hp_modem_hp_regulator_xpd:1;
        /** hp_modem_hp_regulator_slp_mem_dbias : R/W; bitpos: [22:19]; default: 12;
         *  need_des
         */
        uint32_t hp_modem_hp_regulator_slp_mem_dbias:4;
        /** hp_modem_hp_regulator_slp_logic_dbias : R/W; bitpos: [26:23]; default: 12;
         *  need_des
         */
        uint32_t hp_modem_hp_regulator_slp_logic_dbias:4;
        /** hp_modem_hp_regulator_dbias : R/W; bitpos: [31:27]; default: 24;
         *  need_des
         */
        uint32_t hp_modem_hp_regulator_dbias:5;
    };
    uint32_t val;
} pmu_hp_modem_hp_regulator0_reg_t;

/** Type of hp_modem_hp_regulator1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** hp_modem_hp_regulator_drv_b : R/W; bitpos: [31:8]; default: 0;
         *  need_des
         */
        uint32_t hp_modem_hp_regulator_drv_b:24;
    };
    uint32_t val;
} pmu_hp_modem_hp_regulator1_reg_t;

/** Type of hp_modem_xtal register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_modem_xpd_xtal : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t hp_modem_xpd_xtal:1;
    };
    uint32_t val;
} pmu_hp_modem_xtal_reg_t;

/** Type of hp_sleep_dig_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** hp_sleep_vdd_spi_pd_en : R/W; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_vdd_spi_pd_en:1;
        /** hp_sleep_hp_mem_dslp : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_hp_mem_dslp:1;
        /** hp_sleep_pd_hp_mem_pd_en : R/W; bitpos: [26:23]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_hp_mem_pd_en:4;
        /** hp_sleep_pd_hp_wifi_pd_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_hp_wifi_pd_en:1;
        uint32_t reserved_28:1;
        /** hp_sleep_pd_hp_cpu_pd_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_hp_cpu_pd_en:1;
        /** hp_sleep_pd_hp_aon_pd_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_hp_aon_pd_en:1;
        /** hp_sleep_pd_top_pd_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_top_pd_en:1;
    };
    uint32_t val;
} pmu_hp_sleep_dig_power_reg_t;

/** Type of hp_sleep_icg_hp_func register
 *  need_des
 */
typedef union {
    struct {
        /** hp_sleep_dig_icg_func_en : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t hp_sleep_dig_icg_func_en:32;
    };
    uint32_t val;
} pmu_hp_sleep_icg_hp_func_reg_t;

/** Type of hp_sleep_icg_hp_apb register
 *  need_des
 */
typedef union {
    struct {
        /** hp_sleep_dig_icg_apb_en : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t hp_sleep_dig_icg_apb_en:32;
    };
    uint32_t val;
} pmu_hp_sleep_icg_hp_apb_reg_t;

/** Type of hp_sleep_icg_modem register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** hp_sleep_dig_icg_modem_code : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_dig_icg_modem_code:2;
    };
    uint32_t val;
} pmu_hp_sleep_icg_modem_reg_t;

/** Type of hp_sleep_hp_sys_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** hp_sleep_uart_wakeup_en : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_uart_wakeup_en:1;
        /** hp_sleep_lp_pad_hold_all : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_lp_pad_hold_all:1;
        /** hp_sleep_hp_pad_hold_all : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_hp_pad_hold_all:1;
        /** hp_sleep_dig_pad_slp_sel : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_dig_pad_slp_sel:1;
        /** hp_sleep_dig_pause_wdt : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_dig_pause_wdt:1;
        /** hp_sleep_dig_cpu_stall : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_dig_cpu_stall:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} pmu_hp_sleep_hp_sys_cntl_reg_t;

/** Type of hp_sleep_hp_ck_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** hp_sleep_i2c_iso_en : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_i2c_iso_en:1;
        /** hp_sleep_i2c_retention : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_i2c_retention:1;
        /** hp_sleep_xpd_bb_i2c : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_xpd_bb_i2c:1;
        /** hp_sleep_xpd_bbpll_i2c : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_xpd_bbpll_i2c:1;
        /** hp_sleep_xpd_bbpll : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_xpd_bbpll:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} pmu_hp_sleep_hp_ck_power_reg_t;

/** Type of hp_sleep_bias register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** hp_sleep_xpd_bias : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_xpd_bias:1;
        /** hp_sleep_dbg_atten : R/W; bitpos: [29:26]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_dbg_atten:4;
        /** hp_sleep_pd_cur : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_cur:1;
        /** hp_sleep_bias_sleep : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_bias_sleep:1;
    };
    uint32_t val;
} pmu_hp_sleep_bias_reg_t;

/** Type of hp_sleep_backup register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** hp_modem2sleep_backup_modem_clk_code : R/W; bitpos: [7:6]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2sleep_backup_modem_clk_code:2;
        /** hp_active2sleep_backup_modem_clk_code : R/W; bitpos: [9:8]; default: 0;
         *  need_des
         */
        uint32_t hp_active2sleep_backup_modem_clk_code:2;
        /** hp_sleep_retention_mode : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_retention_mode:1;
        uint32_t reserved_11:1;
        /** hp_modem2sleep_retention_en : R/W; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2sleep_retention_en:1;
        /** hp_active2sleep_retention_en : R/W; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t hp_active2sleep_retention_en:1;
        uint32_t reserved_14:2;
        /** hp_modem2sleep_backup_clk_sel : R/W; bitpos: [17:16]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2sleep_backup_clk_sel:2;
        /** hp_active2sleep_backup_clk_sel : R/W; bitpos: [19:18]; default: 0;
         *  need_des
         */
        uint32_t hp_active2sleep_backup_clk_sel:2;
        uint32_t reserved_20:3;
        /** hp_modem2sleep_backup_mode : R/W; bitpos: [25:23]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2sleep_backup_mode:3;
        /** hp_active2sleep_backup_mode : R/W; bitpos: [28:26]; default: 0;
         *  need_des
         */
        uint32_t hp_active2sleep_backup_mode:3;
        uint32_t reserved_29:1;
        /** hp_modem2sleep_backup_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_modem2sleep_backup_en:1;
        /** hp_active2sleep_backup_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_active2sleep_backup_en:1;
    };
    uint32_t val;
} pmu_hp_sleep_backup_reg_t;

/** Type of hp_sleep_backup_clk register
 *  need_des
 */
typedef union {
    struct {
        /** hp_sleep_backup_icg_func_en : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_backup_icg_func_en:32;
    };
    uint32_t val;
} pmu_hp_sleep_backup_clk_reg_t;

/** Type of hp_sleep_sysclk register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** hp_sleep_dig_sys_clk_no_div : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_dig_sys_clk_no_div:1;
        /** hp_sleep_icg_sys_clock_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_icg_sys_clock_en:1;
        /** hp_sleep_sys_clk_slp_sel : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_sys_clk_slp_sel:1;
        /** hp_sleep_icg_slp_sel : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_icg_slp_sel:1;
        /** hp_sleep_dig_sys_clk_sel : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_dig_sys_clk_sel:2;
    };
    uint32_t val;
} pmu_hp_sleep_sysclk_reg_t;

/** Type of hp_sleep_hp_regulator0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** hp_sleep_hp_regulator_slp_mem_xpd : R/W; bitpos: [16]; default: 1;
         *  need_des
         */
        uint32_t hp_sleep_hp_regulator_slp_mem_xpd:1;
        /** hp_sleep_hp_regulator_slp_logic_xpd : R/W; bitpos: [17]; default: 1;
         *  need_des
         */
        uint32_t hp_sleep_hp_regulator_slp_logic_xpd:1;
        /** hp_sleep_hp_regulator_xpd : R/W; bitpos: [18]; default: 1;
         *  need_des
         */
        uint32_t hp_sleep_hp_regulator_xpd:1;
        /** hp_sleep_hp_regulator_slp_mem_dbias : R/W; bitpos: [22:19]; default: 12;
         *  need_des
         */
        uint32_t hp_sleep_hp_regulator_slp_mem_dbias:4;
        /** hp_sleep_hp_regulator_slp_logic_dbias : R/W; bitpos: [26:23]; default: 12;
         *  need_des
         */
        uint32_t hp_sleep_hp_regulator_slp_logic_dbias:4;
        /** hp_sleep_hp_regulator_dbias : R/W; bitpos: [31:27]; default: 24;
         *  need_des
         */
        uint32_t hp_sleep_hp_regulator_dbias:5;
    };
    uint32_t val;
} pmu_hp_sleep_hp_regulator0_reg_t;

/** Type of hp_sleep_hp_regulator1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** hp_sleep_hp_regulator_drv_b : R/W; bitpos: [31:8]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_hp_regulator_drv_b:24;
    };
    uint32_t val;
} pmu_hp_sleep_hp_regulator1_reg_t;

/** Type of hp_sleep_xtal register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** hp_sleep_xpd_xtal : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t hp_sleep_xpd_xtal:1;
    };
    uint32_t val;
} pmu_hp_sleep_xtal_reg_t;

/** Type of hp_sleep_lp_regulator0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** hp_sleep_lp_regulator_slp_xpd : R/W; bitpos: [21]; default: 1;
         *  need_des
         */
        uint32_t hp_sleep_lp_regulator_slp_xpd:1;
        /** hp_sleep_lp_regulator_xpd : R/W; bitpos: [22]; default: 1;
         *  need_des
         */
        uint32_t hp_sleep_lp_regulator_xpd:1;
        /** hp_sleep_lp_regulator_slp_dbias : R/W; bitpos: [26:23]; default: 12;
         *  need_des
         */
        uint32_t hp_sleep_lp_regulator_slp_dbias:4;
        /** hp_sleep_lp_regulator_dbias : R/W; bitpos: [31:27]; default: 24;
         *  need_des
         */
        uint32_t hp_sleep_lp_regulator_dbias:5;
    };
    uint32_t val;
} pmu_hp_sleep_lp_regulator0_reg_t;

/** Type of hp_sleep_lp_regulator1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** hp_sleep_lp_regulator_drv_b : R/W; bitpos: [31:28]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_lp_regulator_drv_b:4;
    };
    uint32_t val;
} pmu_hp_sleep_lp_regulator1_reg_t;

/** Type of hp_sleep_lp_dig_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** hp_sleep_lp_mem_dslp : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_lp_mem_dslp:1;
        /** hp_sleep_pd_lp_peri_pd_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_lp_peri_pd_en:1;
    };
    uint32_t val;
} pmu_hp_sleep_lp_dig_power_reg_t;

/** Type of hp_sleep_lp_ck_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** hp_sleep_xpd_xtal32k : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_xpd_xtal32k:1;
        /** hp_sleep_xpd_rc32k : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_xpd_rc32k:1;
        /** hp_sleep_xpd_fosc_clk : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t hp_sleep_xpd_fosc_clk:1;
        /** hp_sleep_pd_osc_clk : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sleep_pd_osc_clk:1;
    };
    uint32_t val;
} pmu_hp_sleep_lp_ck_power_reg_t;

/** Type of lp_sleep_lp_regulator0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** lp_sleep_lp_regulator_slp_xpd : R/W; bitpos: [21]; default: 1;
         *  need_des
         */
        uint32_t lp_sleep_lp_regulator_slp_xpd:1;
        /** lp_sleep_lp_regulator_xpd : R/W; bitpos: [22]; default: 1;
         *  need_des
         */
        uint32_t lp_sleep_lp_regulator_xpd:1;
        /** lp_sleep_lp_regulator_slp_dbias : R/W; bitpos: [26:23]; default: 12;
         *  need_des
         */
        uint32_t lp_sleep_lp_regulator_slp_dbias:4;
        /** lp_sleep_lp_regulator_dbias : R/W; bitpos: [31:27]; default: 24;
         *  need_des
         */
        uint32_t lp_sleep_lp_regulator_dbias:5;
    };
    uint32_t val;
} pmu_lp_sleep_lp_regulator0_reg_t;

/** Type of lp_sleep_lp_regulator1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** lp_sleep_lp_regulator_drv_b : R/W; bitpos: [31:28]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_lp_regulator_drv_b:4;
    };
    uint32_t val;
} pmu_lp_sleep_lp_regulator1_reg_t;

/** Type of lp_sleep_xtal register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_sleep_xpd_xtal : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t lp_sleep_xpd_xtal:1;
    };
    uint32_t val;
} pmu_lp_sleep_xtal_reg_t;

/** Type of lp_sleep_lp_dig_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_sleep_lp_mem_dslp : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_lp_mem_dslp:1;
        /** lp_sleep_pd_lp_peri_pd_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_pd_lp_peri_pd_en:1;
    };
    uint32_t val;
} pmu_lp_sleep_lp_dig_power_reg_t;

/** Type of lp_sleep_lp_ck_power register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** lp_sleep_xpd_xtal32k : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_xpd_xtal32k:1;
        /** lp_sleep_xpd_rc32k : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_xpd_rc32k:1;
        /** lp_sleep_xpd_fosc_clk : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_sleep_xpd_fosc_clk:1;
        /** lp_sleep_pd_osc_clk : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_pd_osc_clk:1;
    };
    uint32_t val;
} pmu_lp_sleep_lp_ck_power_reg_t;

/** Type of lp_sleep_bias register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** lp_sleep_xpd_bias : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_xpd_bias:1;
        /** lp_sleep_dbg_atten : R/W; bitpos: [29:26]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_dbg_atten:4;
        /** lp_sleep_pd_cur : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_pd_cur:1;
        /** lp_sleep_bias_sleep : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_sleep_bias_sleep:1;
    };
    uint32_t val;
} pmu_lp_sleep_bias_reg_t;

/** Type of imm_hp_ck_power register
 *  need_des
 */
typedef union {
    struct {
        /** tie_low_global_bbpll_icg : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t tie_low_global_bbpll_icg:1;
        /** tie_low_global_xtal_icg : WT; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t tie_low_global_xtal_icg:1;
        /** tie_low_i2c_retention : WT; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t tie_low_i2c_retention:1;
        /** tie_low_xpd_bb_i2c : WT; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t tie_low_xpd_bb_i2c:1;
        /** tie_low_xpd_bbpll_i2c : WT; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t tie_low_xpd_bbpll_i2c:1;
        /** tie_low_xpd_bbpll : WT; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t tie_low_xpd_bbpll:1;
        /** tie_low_xpd_xtal : WT; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t tie_low_xpd_xtal:1;
        uint32_t reserved_7:18;
        /** tie_high_global_bbpll_icg : WT; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t tie_high_global_bbpll_icg:1;
        /** tie_high_global_xtal_icg : WT; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t tie_high_global_xtal_icg:1;
        /** tie_high_i2c_retention : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t tie_high_i2c_retention:1;
        /** tie_high_xpd_bb_i2c : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tie_high_xpd_bb_i2c:1;
        /** tie_high_xpd_bbpll_i2c : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t tie_high_xpd_bbpll_i2c:1;
        /** tie_high_xpd_bbpll : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t tie_high_xpd_bbpll:1;
        /** tie_high_xpd_xtal : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tie_high_xpd_xtal:1;
    };
    uint32_t val;
} pmu_imm_hp_ck_power_reg_t;

/** Type of imm_sleep_sysclk register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** update_dig_icg_switch : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t update_dig_icg_switch:1;
        /** tie_low_icg_slp_sel : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t tie_low_icg_slp_sel:1;
        /** tie_high_icg_slp_sel : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t tie_high_icg_slp_sel:1;
        /** update_dig_sys_clk_sel : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t update_dig_sys_clk_sel:1;
    };
    uint32_t val;
} pmu_imm_sleep_sysclk_reg_t;

/** Type of imm_hp_func_icg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** update_dig_icg_func_en : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t update_dig_icg_func_en:1;
    };
    uint32_t val;
} pmu_imm_hp_func_icg_reg_t;

/** Type of imm_hp_apb_icg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** update_dig_icg_apb_en : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t update_dig_icg_apb_en:1;
    };
    uint32_t val;
} pmu_imm_hp_apb_icg_reg_t;

/** Type of imm_modem_icg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** update_dig_icg_modem_en : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t update_dig_icg_modem_en:1;
    };
    uint32_t val;
} pmu_imm_modem_icg_reg_t;

/** Type of imm_lp_icg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** tie_low_lp_rootclk_sel : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t tie_low_lp_rootclk_sel:1;
        /** tie_high_lp_rootclk_sel : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tie_high_lp_rootclk_sel:1;
    };
    uint32_t val;
} pmu_imm_lp_icg_reg_t;

/** Type of imm_pad_hold_all register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tie_high_lp_pad_hold_all : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tie_high_lp_pad_hold_all:1;
        /** tie_low_lp_pad_hold_all : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t tie_low_lp_pad_hold_all:1;
        /** tie_high_hp_pad_hold_all : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t tie_high_hp_pad_hold_all:1;
        /** tie_low_hp_pad_hold_all : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tie_low_hp_pad_hold_all:1;
    };
    uint32_t val;
} pmu_imm_pad_hold_all_reg_t;

/** Type of imm_i2c_iso register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** tie_high_i2c_iso_en : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t tie_high_i2c_iso_en:1;
        /** tie_low_i2c_iso_en : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tie_low_i2c_iso_en:1;
    };
    uint32_t val;
} pmu_imm_i2c_iso_reg_t;

/** Type of power_wait_timer0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** dg_hp_powerdown_timer : R/W; bitpos: [13:5]; default: 255;
         *  need_des
         */
        uint32_t dg_hp_powerdown_timer:9;
        /** dg_hp_powerup_timer : R/W; bitpos: [22:14]; default: 255;
         *  need_des
         */
        uint32_t dg_hp_powerup_timer:9;
        /** dg_hp_wait_timer : R/W; bitpos: [31:23]; default: 255;
         *  need_des
         */
        uint32_t dg_hp_wait_timer:9;
    };
    uint32_t val;
} pmu_power_wait_timer0_reg_t;

/** Type of power_wait_timer1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:9;
        /** dg_lp_powerdown_timer : R/W; bitpos: [15:9]; default: 255;
         *  need_des
         */
        uint32_t dg_lp_powerdown_timer:7;
        /** dg_lp_powerup_timer : R/W; bitpos: [22:16]; default: 255;
         *  need_des
         */
        uint32_t dg_lp_powerup_timer:7;
        /** dg_lp_wait_timer : R/W; bitpos: [31:23]; default: 255;
         *  need_des
         */
        uint32_t dg_lp_wait_timer:9;
    };
    uint32_t val;
} pmu_power_wait_timer1_reg_t;

/** Type of power_pd_top_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** force_top_reset : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t force_top_reset:1;
        /** force_top_iso : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t force_top_iso:1;
        /** force_top_pu : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t force_top_pu:1;
        /** force_top_no_reset : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t force_top_no_reset:1;
        /** force_top_no_iso : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t force_top_no_iso:1;
        /** force_top_pd : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t force_top_pd:1;
        /** pd_top_mask : R/W; bitpos: [10:6]; default: 0;
         *  need_des
         */
        uint32_t pd_top_mask:5;
        uint32_t reserved_11:16;
        /** pd_top_pd_mask : R/W; bitpos: [31:27]; default: 0;
         *  need_des
         */
        uint32_t pd_top_pd_mask:5;
    };
    uint32_t val;
} pmu_power_pd_top_cntl_reg_t;

/** Type of power_pd_hpaon_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** force_hp_aon_reset : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t force_hp_aon_reset:1;
        /** force_hp_aon_iso : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t force_hp_aon_iso:1;
        /** force_hp_aon_pu : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t force_hp_aon_pu:1;
        /** force_hp_aon_no_reset : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t force_hp_aon_no_reset:1;
        /** force_hp_aon_no_iso : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t force_hp_aon_no_iso:1;
        /** force_hp_aon_pd : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t force_hp_aon_pd:1;
        /** pd_hp_aon_mask : R/W; bitpos: [10:6]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_aon_mask:5;
        uint32_t reserved_11:16;
        /** pd_hp_aon_pd_mask : R/W; bitpos: [31:27]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_aon_pd_mask:5;
    };
    uint32_t val;
} pmu_power_pd_hpaon_cntl_reg_t;

/** Type of power_pd_hpcpu_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** force_hp_cpu_reset : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t force_hp_cpu_reset:1;
        /** force_hp_cpu_iso : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t force_hp_cpu_iso:1;
        /** force_hp_cpu_pu : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t force_hp_cpu_pu:1;
        /** force_hp_cpu_no_reset : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t force_hp_cpu_no_reset:1;
        /** force_hp_cpu_no_iso : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t force_hp_cpu_no_iso:1;
        /** force_hp_cpu_pd : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t force_hp_cpu_pd:1;
        /** pd_hp_cpu_mask : R/W; bitpos: [10:6]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_cpu_mask:5;
        uint32_t reserved_11:16;
        /** pd_hp_cpu_pd_mask : R/W; bitpos: [31:27]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_cpu_pd_mask:5;
    };
    uint32_t val;
} pmu_power_pd_hpcpu_cntl_reg_t;

/** Type of power_pd_hpwifi_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** force_hp_wifi_reset : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t force_hp_wifi_reset:1;
        /** force_hp_wifi_iso : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t force_hp_wifi_iso:1;
        /** force_hp_wifi_pu : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t force_hp_wifi_pu:1;
        /** force_hp_wifi_no_reset : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t force_hp_wifi_no_reset:1;
        /** force_hp_wifi_no_iso : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t force_hp_wifi_no_iso:1;
        /** force_hp_wifi_pd : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t force_hp_wifi_pd:1;
        /** pd_hp_wifi_mask : R/W; bitpos: [10:6]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_wifi_mask:5;
        uint32_t reserved_11:16;
        /** pd_hp_wifi_pd_mask : R/W; bitpos: [31:27]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_wifi_pd_mask:5;
    };
    uint32_t val;
} pmu_power_pd_hpwifi_cntl_reg_t;

/** Type of power_pd_lpperi_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** force_lp_peri_reset : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t force_lp_peri_reset:1;
        /** force_lp_peri_iso : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t force_lp_peri_iso:1;
        /** force_lp_peri_pu : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t force_lp_peri_pu:1;
        /** force_lp_peri_no_reset : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t force_lp_peri_no_reset:1;
        /** force_lp_peri_no_iso : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t force_lp_peri_no_iso:1;
        /** force_lp_peri_pd : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t force_lp_peri_pd:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} pmu_power_pd_lpperi_cntl_reg_t;

/** Type of power_pd_mem_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** force_hp_mem_iso : R/W; bitpos: [3:0]; default: 0;
         *  need_des
         */
        uint32_t force_hp_mem_iso:4;
        /** force_hp_mem_pd : R/W; bitpos: [7:4]; default: 0;
         *  need_des
         */
        uint32_t force_hp_mem_pd:4;
        uint32_t reserved_8:16;
        /** force_hp_mem_no_iso : R/W; bitpos: [27:24]; default: 15;
         *  need_des
         */
        uint32_t force_hp_mem_no_iso:4;
        /** force_hp_mem_pu : R/W; bitpos: [31:28]; default: 15;
         *  need_des
         */
        uint32_t force_hp_mem_pu:4;
    };
    uint32_t val;
} pmu_power_pd_mem_cntl_reg_t;

/** Type of power_pd_mem_mask register
 *  need_des
 */
typedef union {
    struct {
        /** pd_hp_mem2_pd_mask : R/W; bitpos: [4:0]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_mem2_pd_mask:5;
        /** pd_hp_mem1_pd_mask : R/W; bitpos: [9:5]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_mem1_pd_mask:5;
        /** pd_hp_mem0_pd_mask : R/W; bitpos: [14:10]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_mem0_pd_mask:5;
        uint32_t reserved_15:2;
        /** pd_hp_mem2_mask : R/W; bitpos: [21:17]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_mem2_mask:5;
        /** pd_hp_mem1_mask : R/W; bitpos: [26:22]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_mem1_mask:5;
        /** pd_hp_mem0_mask : R/W; bitpos: [31:27]; default: 0;
         *  need_des
         */
        uint32_t pd_hp_mem0_mask:5;
    };
    uint32_t val;
} pmu_power_pd_mem_mask_reg_t;

/** Type of power_hp_pad register
 *  need_des
 */
typedef union {
    struct {
        /** force_hp_pad_no_iso_all : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t force_hp_pad_no_iso_all:1;
        /** force_hp_pad_iso_all : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t force_hp_pad_iso_all:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pmu_power_hp_pad_reg_t;

/** Type of power_vdd_spi_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:18;
        /** vdd_spi_pwr_wait : R/W; bitpos: [28:18]; default: 255;
         *  need_des
         */
        uint32_t vdd_spi_pwr_wait:11;
        /** vdd_spi_pwr_sw : R/W; bitpos: [30:29]; default: 3;
         *  need_des
         */
        uint32_t vdd_spi_pwr_sw:2;
        /** vdd_spi_pwr_sel_sw : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t vdd_spi_pwr_sel_sw:1;
    };
    uint32_t val;
} pmu_power_vdd_spi_cntl_reg_t;

/** Type of power_ck_wait_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** wait_xtl_stable : R/W; bitpos: [15:0]; default: 256;
         *  need_des
         */
        uint32_t wait_xtl_stable:16;
        /** wait_pll_stable : R/W; bitpos: [31:16]; default: 256;
         *  need_des
         */
        uint32_t wait_pll_stable:16;
    };
    uint32_t val;
} pmu_power_ck_wait_cntl_reg_t;

/** Type of slp_wakeup_cntl0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** sleep_req : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t sleep_req:1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl0_reg_t;

/** Type of slp_wakeup_cntl1 register
 *  need_des
 */
typedef union {
    struct {
        /** sleep_reject_ena : R/W; bitpos: [30:0]; default: 0;
         *  need_des
         */
        uint32_t sleep_reject_ena:31;
        /** slp_reject_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t slp_reject_en:1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl1_reg_t;

/** Type of slp_wakeup_cntl2 register
 *  need_des
 */
typedef union {
    struct {
        /** wakeup_ena : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t wakeup_ena:32;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl2_reg_t;

/** Type of slp_wakeup_cntl3 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_min_slp_val : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t lp_min_slp_val:8;
        /** hp_min_slp_val : R/W; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t hp_min_slp_val:8;
        /** sleep_prt_sel : R/W; bitpos: [17:16]; default: 0;
         *  need_des
         */
        uint32_t sleep_prt_sel:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl3_reg_t;

/** Type of slp_wakeup_cntl4 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** slp_reject_cause_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t slp_reject_cause_clr:1;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl4_reg_t;

/** Type of slp_wakeup_cntl5 register
 *  need_des
 */
typedef union {
    struct {
        /** modem_wait_target : R/W; bitpos: [19:0]; default: 128;
         *  need_des
         */
        uint32_t modem_wait_target:20;
        uint32_t reserved_20:4;
        /** lp_ana_wait_target : R/W; bitpos: [31:24]; default: 1;
         *  need_des
         */
        uint32_t lp_ana_wait_target:8;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl5_reg_t;

/** Type of slp_wakeup_cntl6 register
 *  need_des
 */
typedef union {
    struct {
        /** soc_wakeup_wait : R/W; bitpos: [19:0]; default: 128;
         *  need_des
         */
        uint32_t soc_wakeup_wait:20;
        uint32_t reserved_20:10;
        /** soc_wakeup_wait_cfg : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_wait_cfg:2;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl6_reg_t;

/** Type of slp_wakeup_cntl7 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** ana_wait_target : R/W; bitpos: [31:16]; default: 1;
         *  need_des
         */
        uint32_t ana_wait_target:16;
    };
    uint32_t val;
} pmu_slp_wakeup_cntl7_reg_t;

/** Type of slp_wakeup_status0 register
 *  need_des
 */
typedef union {
    struct {
        /** wakeup_cause : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t wakeup_cause:32;
    };
    uint32_t val;
} pmu_slp_wakeup_status0_reg_t;

/** Type of slp_wakeup_status1 register
 *  need_des
 */
typedef union {
    struct {
        /** reject_cause : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t reject_cause:32;
    };
    uint32_t val;
} pmu_slp_wakeup_status1_reg_t;

/** Type of hp_ck_poweron register
 *  need_des
 */
typedef union {
    struct {
        /** i2c_por_wait_target : R/W; bitpos: [7:0]; default: 50;
         *  need_des
         */
        uint32_t i2c_por_wait_target:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} pmu_hp_ck_poweron_reg_t;

/** Type of hp_ck_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** modify_icg_cntl_wait : R/W; bitpos: [7:0]; default: 10;
         *  need_des
         */
        uint32_t modify_icg_cntl_wait:8;
        /** switch_icg_cntl_wait : R/W; bitpos: [15:8]; default: 10;
         *  need_des
         */
        uint32_t switch_icg_cntl_wait:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} pmu_hp_ck_cntl_reg_t;

/** Type of por_status register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** por_done : RO; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t por_done:1;
    };
    uint32_t val;
} pmu_por_status_reg_t;

/** Type of rf_pwc register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** xpd_tc5g_i2c : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t xpd_tc5g_i2c:1;
        /** xpd_rx5g_i2c : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t xpd_rx5g_i2c:1;
        /** perif_i2c_rstb : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t perif_i2c_rstb:1;
        /** xpd_perif_i2c : R/W; bitpos: [27]; default: 1;
         *  need_des
         */
        uint32_t xpd_perif_i2c:1;
        /** xpd_txrf_i2c : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t xpd_txrf_i2c:1;
        /** xpd_rfrx_pbus : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t xpd_rfrx_pbus:1;
        /** xpd_ckgen_i2c : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t xpd_ckgen_i2c:1;
        /** xpd_pll_i2c : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t xpd_pll_i2c:1;
    };
    uint32_t val;
} pmu_rf_pwc_reg_t;

/** Type of backup_cfg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** backup_sys_clk_no_div : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t backup_sys_clk_no_div:1;
    };
    uint32_t val;
} pmu_backup_cfg_reg_t;

/** Type of int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** lp_cpu_exc_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_exc_int_raw:1;
        /** sdio_idle_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sdio_idle_int_raw:1;
        /** sw_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t sw_int_raw:1;
        /** soc_sleep_reject_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t soc_sleep_reject_int_raw:1;
        /** soc_wakeup_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_raw:1;
    };
    uint32_t val;
} pmu_int_raw_reg_t;

/** Type of hp_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** lp_cpu_exc_int_st : RO; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_exc_int_st:1;
        /** sdio_idle_int_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sdio_idle_int_st:1;
        /** sw_int_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t sw_int_st:1;
        /** soc_sleep_reject_int_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t soc_sleep_reject_int_st:1;
        /** soc_wakeup_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_st:1;
    };
    uint32_t val;
} pmu_hp_int_st_reg_t;

/** Type of hp_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** lp_cpu_exc_int_ena : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_exc_int_ena:1;
        /** sdio_idle_int_ena : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sdio_idle_int_ena:1;
        /** sw_int_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t sw_int_ena:1;
        /** soc_sleep_reject_int_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t soc_sleep_reject_int_ena:1;
        /** soc_wakeup_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_ena:1;
    };
    uint32_t val;
} pmu_hp_int_ena_reg_t;

/** Type of hp_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** lp_cpu_exc_int_clr : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_exc_int_clr:1;
        /** sdio_idle_int_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sdio_idle_int_clr:1;
        /** sw_int_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t sw_int_clr:1;
        /** soc_sleep_reject_int_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t soc_sleep_reject_int_clr:1;
        /** soc_wakeup_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_clr:1;
    };
    uint32_t val;
} pmu_hp_int_clr_reg_t;

/** Type of lp_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** lp_cpu_wakeup_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_wakeup_int_raw:1;
        /** modem_switch_active_end_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_end_int_raw:1;
        /** sleep_switch_active_end_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_end_int_raw:1;
        /** sleep_switch_modem_end_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_end_int_raw:1;
        /** modem_switch_sleep_end_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_end_int_raw:1;
        /** active_switch_sleep_end_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_end_int_raw:1;
        /** modem_switch_active_start_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_start_int_raw:1;
        /** sleep_switch_active_start_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_start_int_raw:1;
        /** sleep_switch_modem_start_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_start_int_raw:1;
        /** modem_switch_sleep_start_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_start_int_raw:1;
        /** active_switch_sleep_start_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_start_int_raw:1;
        /** hp_sw_trigger_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sw_trigger_int_raw:1;
    };
    uint32_t val;
} pmu_lp_int_raw_reg_t;

/** Type of lp_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** lp_cpu_wakeup_int_st : RO; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_wakeup_int_st:1;
        /** modem_switch_active_end_int_st : RO; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_end_int_st:1;
        /** sleep_switch_active_end_int_st : RO; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_end_int_st:1;
        /** sleep_switch_modem_end_int_st : RO; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_end_int_st:1;
        /** modem_switch_sleep_end_int_st : RO; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_end_int_st:1;
        /** active_switch_sleep_end_int_st : RO; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_end_int_st:1;
        /** modem_switch_active_start_int_st : RO; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_start_int_st:1;
        /** sleep_switch_active_start_int_st : RO; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_start_int_st:1;
        /** sleep_switch_modem_start_int_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_start_int_st:1;
        /** modem_switch_sleep_start_int_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_start_int_st:1;
        /** active_switch_sleep_start_int_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_start_int_st:1;
        /** hp_sw_trigger_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sw_trigger_int_st:1;
    };
    uint32_t val;
} pmu_lp_int_st_reg_t;

/** Type of lp_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** lp_cpu_wakeup_int_ena : R/W; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_wakeup_int_ena:1;
        /** modem_switch_active_end_int_ena : R/W; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_end_int_ena:1;
        /** sleep_switch_active_end_int_ena : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_end_int_ena:1;
        /** sleep_switch_modem_end_int_ena : R/W; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_end_int_ena:1;
        /** modem_switch_sleep_end_int_ena : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_end_int_ena:1;
        /** active_switch_sleep_end_int_ena : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_end_int_ena:1;
        /** modem_switch_active_start_int_ena : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_start_int_ena:1;
        /** sleep_switch_active_start_int_ena : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_start_int_ena:1;
        /** sleep_switch_modem_start_int_ena : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_start_int_ena:1;
        /** modem_switch_sleep_start_int_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_start_int_ena:1;
        /** active_switch_sleep_start_int_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_start_int_ena:1;
        /** hp_sw_trigger_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sw_trigger_int_ena:1;
    };
    uint32_t val;
} pmu_lp_int_ena_reg_t;

/** Type of lp_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** lp_cpu_wakeup_int_clr : WT; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_wakeup_int_clr:1;
        /** modem_switch_active_end_int_clr : WT; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_end_int_clr:1;
        /** sleep_switch_active_end_int_clr : WT; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_end_int_clr:1;
        /** sleep_switch_modem_end_int_clr : WT; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_end_int_clr:1;
        /** modem_switch_sleep_end_int_clr : WT; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_end_int_clr:1;
        /** active_switch_sleep_end_int_clr : WT; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_end_int_clr:1;
        /** modem_switch_active_start_int_clr : WT; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_active_start_int_clr:1;
        /** sleep_switch_active_start_int_clr : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_active_start_int_clr:1;
        /** sleep_switch_modem_start_int_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t sleep_switch_modem_start_int_clr:1;
        /** modem_switch_sleep_start_int_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t modem_switch_sleep_start_int_clr:1;
        /** active_switch_sleep_start_int_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t active_switch_sleep_start_int_clr:1;
        /** hp_sw_trigger_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_sw_trigger_int_clr:1;
    };
    uint32_t val;
} pmu_lp_int_clr_reg_t;

/** Type of lp_cpu_pwr0 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_cpu_waiti_rdy : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_waiti_rdy:1;
        /** lp_cpu_stall_rdy : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_stall_rdy:1;
        uint32_t reserved_2:16;
        /** lp_cpu_force_stall : R/W; bitpos: [18]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_force_stall:1;
        /** lp_cpu_slp_waiti_flag_en : R/W; bitpos: [19]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_slp_waiti_flag_en:1;
        /** lp_cpu_slp_stall_flag_en : R/W; bitpos: [20]; default: 1;
         *  need_des
         */
        uint32_t lp_cpu_slp_stall_flag_en:1;
        /** lp_cpu_slp_stall_wait : R/W; bitpos: [28:21]; default: 255;
         *  need_des
         */
        uint32_t lp_cpu_slp_stall_wait:8;
        /** lp_cpu_slp_stall_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_slp_stall_en:1;
        /** lp_cpu_slp_reset_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_slp_reset_en:1;
        /** lp_cpu_slp_bypass_intr_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_slp_bypass_intr_en:1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr0_reg_t;

/** Type of lp_cpu_pwr1 register
 *  need_des
 */
typedef union {
    struct {
        /** lp_cpu_wakeup_en : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_wakeup_en:16;
        uint32_t reserved_16:15;
        /** lp_cpu_sleep_req : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_cpu_sleep_req:1;
    };
    uint32_t val;
} pmu_lp_cpu_pwr1_reg_t;

/** Type of hp_lp_cpu_comm register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_trigger_hp : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_trigger_hp:1;
        /** hp_trigger_lp : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hp_trigger_lp:1;
    };
    uint32_t val;
} pmu_hp_lp_cpu_comm_reg_t;

/** Type of hp_regulator_cfg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** dig_regulator_en_cal : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t dig_regulator_en_cal:1;
    };
    uint32_t val;
} pmu_hp_regulator_cfg_reg_t;

/** Type of main_state register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:11;
        /** main_last_st_state : RO; bitpos: [17:11]; default: 1;
         *  need_des
         */
        uint32_t main_last_st_state:7;
        /** main_tar_st_state : RO; bitpos: [24:18]; default: 4;
         *  need_des
         */
        uint32_t main_tar_st_state:7;
        /** main_cur_st_state : RO; bitpos: [31:25]; default: 4;
         *  need_des
         */
        uint32_t main_cur_st_state:7;
    };
    uint32_t val;
} pmu_main_state_reg_t;

/** Type of pwr_state register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:13;
        /** backup_st_state : RO; bitpos: [17:13]; default: 1;
         *  need_des
         */
        uint32_t backup_st_state:5;
        /** lp_pwr_st_state : RO; bitpos: [22:18]; default: 0;
         *  need_des
         */
        uint32_t lp_pwr_st_state:5;
        /** hp_pwr_st_state : RO; bitpos: [31:23]; default: 1;
         *  need_des
         */
        uint32_t hp_pwr_st_state:9;
    };
    uint32_t val;
} pmu_pwr_state_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** pmu_date : R/W; bitpos: [30:0]; default: 35664432;
         *  need_des
         */
        uint32_t pmu_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} pmu_date_reg_t;


/** Group: status_register */
/** Type of clk_state0 register
 *  need_des
 */
typedef union {
    struct {
        /** stable_xpd_bbpll_state : RO; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t stable_xpd_bbpll_state:1;
        /** stable_xpd_xtal_state : RO; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t stable_xpd_xtal_state:1;
        uint32_t reserved_2:13;
        /** sys_clk_slp_sel_state : RO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t sys_clk_slp_sel_state:1;
        /** sys_clk_sel_state : RO; bitpos: [17:16]; default: 0;
         *  need_des
         */
        uint32_t sys_clk_sel_state:2;
        /** sys_clk_no_div_state : RO; bitpos: [18]; default: 0;
         *  need_des
         */
        uint32_t sys_clk_no_div_state:1;
        /** icg_sys_clk_en_state : RO; bitpos: [19]; default: 0;
         *  need_des
         */
        uint32_t icg_sys_clk_en_state:1;
        /** icg_modem_switch_state : RO; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t icg_modem_switch_state:1;
        /** icg_modem_code_state : RO; bitpos: [22:21]; default: 0;
         *  need_des
         */
        uint32_t icg_modem_code_state:2;
        /** icg_slp_sel_state : RO; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t icg_slp_sel_state:1;
        /** icg_global_xtal_state : RO; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t icg_global_xtal_state:1;
        /** icg_global_pll_state : RO; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t icg_global_pll_state:1;
        /** ana_i2c_iso_en_state : RO; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t ana_i2c_iso_en_state:1;
        /** ana_i2c_retention_state : RO; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t ana_i2c_retention_state:1;
        /** ana_xpd_bb_i2c_state : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t ana_xpd_bb_i2c_state:1;
        /** ana_xpd_bbpll_i2c_state : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t ana_xpd_bbpll_i2c_state:1;
        /** ana_xpd_bbpll_state : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t ana_xpd_bbpll_state:1;
        /** ana_xpd_xtal_state : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t ana_xpd_xtal_state:1;
    };
    uint32_t val;
} pmu_clk_state0_reg_t;

/** Type of clk_state1 register
 *  need_des
 */
typedef union {
    struct {
        /** icg_func_en_state : RO; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t icg_func_en_state:32;
    };
    uint32_t val;
} pmu_clk_state1_reg_t;

/** Type of clk_state2 register
 *  need_des
 */
typedef union {
    struct {
        /** icg_apb_en_state : RO; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t icg_apb_en_state:32;
    };
    uint32_t val;
} pmu_clk_state2_reg_t;

/** Type of vdd_spi_status register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** stable_vdd_spi_pwr_drv : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t stable_vdd_spi_pwr_drv:1;
    };
    uint32_t val;
} pmu_vdd_spi_status_reg_t;


typedef struct pmu_dev_t {
    volatile pmu_hp_active_dig_power_reg_t hp_active_dig_power;
    volatile pmu_hp_active_icg_hp_func_reg_t hp_active_icg_hp_func;
    volatile pmu_hp_active_icg_hp_apb_reg_t hp_active_icg_hp_apb;
    volatile pmu_hp_active_icg_modem_reg_t hp_active_icg_modem;
    volatile pmu_hp_active_hp_sys_cntl_reg_t hp_active_hp_sys_cntl;
    volatile pmu_hp_active_hp_ck_power_reg_t hp_active_hp_ck_power;
    volatile pmu_hp_active_bias_reg_t hp_active_bias;
    volatile pmu_hp_active_backup_reg_t hp_active_backup;
    volatile pmu_hp_active_backup_clk_reg_t hp_active_backup_clk;
    volatile pmu_hp_active_sysclk_reg_t hp_active_sysclk;
    volatile pmu_hp_active_hp_regulator0_reg_t hp_active_hp_regulator0;
    volatile pmu_hp_active_hp_regulator1_reg_t hp_active_hp_regulator1;
    volatile pmu_hp_active_xtal_reg_t hp_active_xtal;
    volatile pmu_hp_modem_dig_power_reg_t hp_modem_dig_power;
    volatile pmu_hp_modem_icg_hp_func_reg_t hp_modem_icg_hp_func;
    volatile pmu_hp_modem_icg_hp_apb_reg_t hp_modem_icg_hp_apb;
    volatile pmu_hp_modem_icg_modem_reg_t hp_modem_icg_modem;
    volatile pmu_hp_modem_hp_sys_cntl_reg_t hp_modem_hp_sys_cntl;
    volatile pmu_hp_modem_hp_ck_power_reg_t hp_modem_hp_ck_power;
    volatile pmu_hp_modem_bias_reg_t hp_modem_bias;
    volatile pmu_hp_modem_backup_reg_t hp_modem_backup;
    volatile pmu_hp_modem_backup_clk_reg_t hp_modem_backup_clk;
    volatile pmu_hp_modem_sysclk_reg_t hp_modem_sysclk;
    volatile pmu_hp_modem_hp_regulator0_reg_t hp_modem_hp_regulator0;
    volatile pmu_hp_modem_hp_regulator1_reg_t hp_modem_hp_regulator1;
    volatile pmu_hp_modem_xtal_reg_t hp_modem_xtal;
    volatile pmu_hp_sleep_dig_power_reg_t hp_sleep_dig_power;
    volatile pmu_hp_sleep_icg_hp_func_reg_t hp_sleep_icg_hp_func;
    volatile pmu_hp_sleep_icg_hp_apb_reg_t hp_sleep_icg_hp_apb;
    volatile pmu_hp_sleep_icg_modem_reg_t hp_sleep_icg_modem;
    volatile pmu_hp_sleep_hp_sys_cntl_reg_t hp_sleep_hp_sys_cntl;
    volatile pmu_hp_sleep_hp_ck_power_reg_t hp_sleep_hp_ck_power;
    volatile pmu_hp_sleep_bias_reg_t hp_sleep_bias;
    volatile pmu_hp_sleep_backup_reg_t hp_sleep_backup;
    volatile pmu_hp_sleep_backup_clk_reg_t hp_sleep_backup_clk;
    volatile pmu_hp_sleep_sysclk_reg_t hp_sleep_sysclk;
    volatile pmu_hp_sleep_hp_regulator0_reg_t hp_sleep_hp_regulator0;
    volatile pmu_hp_sleep_hp_regulator1_reg_t hp_sleep_hp_regulator1;
    volatile pmu_hp_sleep_xtal_reg_t hp_sleep_xtal;
    volatile pmu_hp_sleep_lp_regulator0_reg_t hp_sleep_lp_regulator0;
    volatile pmu_hp_sleep_lp_regulator1_reg_t hp_sleep_lp_regulator1;
    uint32_t reserved_0a4;
    volatile pmu_hp_sleep_lp_dig_power_reg_t hp_sleep_lp_dig_power;
    volatile pmu_hp_sleep_lp_ck_power_reg_t hp_sleep_lp_ck_power;
    uint32_t reserved_0b0;
    volatile pmu_lp_sleep_lp_regulator0_reg_t lp_sleep_lp_regulator0;
    volatile pmu_lp_sleep_lp_regulator1_reg_t lp_sleep_lp_regulator1;
    volatile pmu_lp_sleep_xtal_reg_t lp_sleep_xtal;
    volatile pmu_lp_sleep_lp_dig_power_reg_t lp_sleep_lp_dig_power;
    volatile pmu_lp_sleep_lp_ck_power_reg_t lp_sleep_lp_ck_power;
    volatile pmu_lp_sleep_bias_reg_t lp_sleep_bias;
    volatile pmu_imm_hp_ck_power_reg_t imm_hp_ck_power;
    volatile pmu_imm_sleep_sysclk_reg_t imm_sleep_sysclk;
    volatile pmu_imm_hp_func_icg_reg_t imm_hp_func_icg;
    volatile pmu_imm_hp_apb_icg_reg_t imm_hp_apb_icg;
    volatile pmu_imm_modem_icg_reg_t imm_modem_icg;
    volatile pmu_imm_lp_icg_reg_t imm_lp_icg;
    volatile pmu_imm_pad_hold_all_reg_t imm_pad_hold_all;
    volatile pmu_imm_i2c_iso_reg_t imm_i2c_iso;
    volatile pmu_power_wait_timer0_reg_t power_wait_timer0;
    volatile pmu_power_wait_timer1_reg_t power_wait_timer1;
    volatile pmu_power_pd_top_cntl_reg_t power_pd_top_cntl;
    volatile pmu_power_pd_hpaon_cntl_reg_t power_pd_hpaon_cntl;
    volatile pmu_power_pd_hpcpu_cntl_reg_t power_pd_hpcpu_cntl;
    uint32_t reserved_100;
    volatile pmu_power_pd_hpwifi_cntl_reg_t power_pd_hpwifi_cntl;
    volatile pmu_power_pd_lpperi_cntl_reg_t power_pd_lpperi_cntl;
    volatile pmu_power_pd_mem_cntl_reg_t power_pd_mem_cntl;
    volatile pmu_power_pd_mem_mask_reg_t power_pd_mem_mask;
    volatile pmu_power_hp_pad_reg_t power_hp_pad;
    volatile pmu_power_vdd_spi_cntl_reg_t power_vdd_spi_cntl;
    volatile pmu_power_ck_wait_cntl_reg_t power_ck_wait_cntl;
    volatile pmu_slp_wakeup_cntl0_reg_t slp_wakeup_cntl0;
    volatile pmu_slp_wakeup_cntl1_reg_t slp_wakeup_cntl1;
    volatile pmu_slp_wakeup_cntl2_reg_t slp_wakeup_cntl2;
    volatile pmu_slp_wakeup_cntl3_reg_t slp_wakeup_cntl3;
    volatile pmu_slp_wakeup_cntl4_reg_t slp_wakeup_cntl4;
    volatile pmu_slp_wakeup_cntl5_reg_t slp_wakeup_cntl5;
    volatile pmu_slp_wakeup_cntl6_reg_t slp_wakeup_cntl6;
    volatile pmu_slp_wakeup_cntl7_reg_t slp_wakeup_cntl7;
    volatile pmu_slp_wakeup_status0_reg_t slp_wakeup_status0;
    volatile pmu_slp_wakeup_status1_reg_t slp_wakeup_status1;
    volatile pmu_hp_ck_poweron_reg_t hp_ck_poweron;
    volatile pmu_hp_ck_cntl_reg_t hp_ck_cntl;
    volatile pmu_por_status_reg_t por_status;
    volatile pmu_rf_pwc_reg_t rf_pwc;
    volatile pmu_backup_cfg_reg_t backup_cfg;
    volatile pmu_int_raw_reg_t int_raw;
    volatile pmu_hp_int_st_reg_t hp_int_st;
    volatile pmu_hp_int_ena_reg_t hp_int_ena;
    volatile pmu_hp_int_clr_reg_t hp_int_clr;
    volatile pmu_lp_int_raw_reg_t lp_int_raw;
    volatile pmu_lp_int_st_reg_t lp_int_st;
    volatile pmu_lp_int_ena_reg_t lp_int_ena;
    volatile pmu_lp_int_clr_reg_t lp_int_clr;
    volatile pmu_lp_cpu_pwr0_reg_t lp_cpu_pwr0;
    volatile pmu_lp_cpu_pwr1_reg_t lp_cpu_pwr1;
    volatile pmu_hp_lp_cpu_comm_reg_t hp_lp_cpu_comm;
    volatile pmu_hp_regulator_cfg_reg_t hp_regulator_cfg;
    volatile pmu_main_state_reg_t main_state;
    volatile pmu_pwr_state_reg_t pwr_state;
    volatile pmu_clk_state0_reg_t clk_state0;
    volatile pmu_clk_state1_reg_t clk_state1;
    volatile pmu_clk_state2_reg_t clk_state2;
    volatile pmu_vdd_spi_status_reg_t vdd_spi_status;
    uint32_t reserved_1a4[150];
    volatile pmu_date_reg_t date;
} pmu_dev_t;

extern pmu_dev_t PMU;

#ifndef __cplusplus
_Static_assert(sizeof(pmu_dev_t) == 0x400, "Invalid size of pmu_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
