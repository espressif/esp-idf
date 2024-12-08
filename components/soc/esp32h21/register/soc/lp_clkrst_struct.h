/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of clkrst_lp_clk_conf register
 *  need_des
 */
typedef union {
    struct {
        /** clkrst_slow_clk_sel : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t clkrst_slow_clk_sel:2;
        /** clkrst_fast_clk_sel : R/W; bitpos: [3:2]; default: 1;
         *  need_des
         */
        uint32_t clkrst_fast_clk_sel:2;
        /** clkrst_lp_peri_div_num : R/W; bitpos: [11:4]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_peri_div_num:8;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_clkrst_lp_clk_conf_reg_t;

/** Type of clkrst_lp_clk_po_en register
 *  need_des
 */
typedef union {
    struct {
        /** clkrst_aon_slow_oen : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t clkrst_aon_slow_oen:1;
        /** clkrst_aon_fast_oen : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t clkrst_aon_fast_oen:1;
        /** clkrst_sosc_oen : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t clkrst_sosc_oen:1;
        /** clkrst_fosc_oen : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t clkrst_fosc_oen:1;
        /** clkrst_osc32k_oen : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t clkrst_osc32k_oen:1;
        /** clkrst_xtal32k_oen : R/W; bitpos: [5]; default: 1;
         *  need_des
         */
        uint32_t clkrst_xtal32k_oen:1;
        /** clkrst_core_efuse_oen : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t clkrst_core_efuse_oen:1;
        /** clkrst_slow_oen : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t clkrst_slow_oen:1;
        /** clkrst_fast_oen : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t clkrst_fast_oen:1;
        /** clkrst_rng_oen : R/W; bitpos: [9]; default: 1;
         *  need_des
         */
        uint32_t clkrst_rng_oen:1;
        /** clkrst_lpbus_oen : R/W; bitpos: [10]; default: 1;
         *  need_des
         */
        uint32_t clkrst_lpbus_oen:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_clkrst_lp_clk_po_en_reg_t;

/** Type of clkrst_lp_clk_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clkrst_fast_ori_gate : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clkrst_fast_ori_gate:1;
    };
    uint32_t val;
} lp_clkrst_lp_clk_en_reg_t;

/** Type of clkrst_lp_rst_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** clkrst_aon_efuse_core_reset_en : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t clkrst_aon_efuse_core_reset_en:1;
        /** clkrst_lp_timer_reset_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_timer_reset_en:1;
        /** clkrst_wdt_reset_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t clkrst_wdt_reset_en:1;
        /** clkrst_ana_peri_reset_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clkrst_ana_peri_reset_en:1;
    };
    uint32_t val;
} lp_clkrst_lp_rst_en_reg_t;

/** Type of clkrst_reset_cause register
 *  need_des
 */
typedef union {
    struct {
        /** clkrst_reset_cause : RO; bitpos: [4:0]; default: 0;
         *  need_des
         */
        uint32_t clkrst_reset_cause:5;
        /** clkrst_core0_reset_flag : RO; bitpos: [5]; default: 1;
         *  need_des
         */
        uint32_t clkrst_core0_reset_flag:1;
        uint32_t reserved_6:23;
        /** clkrst_core0_reset_cause_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t clkrst_core0_reset_cause_clr:1;
        /** clkrst_core0_reset_flag_set : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t clkrst_core0_reset_flag_set:1;
        /** clkrst_core0_reset_flag_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clkrst_core0_reset_flag_clr:1;
    };
    uint32_t val;
} lp_clkrst_reset_cause_reg_t;

/** Type of clkrst_cpu_reset register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** clkrst_rtc_wdt_cpu_reset_length : R/W; bitpos: [24:22]; default: 1;
         *  need_des
         */
        uint32_t clkrst_rtc_wdt_cpu_reset_length:3;
        /** clkrst_rtc_wdt_cpu_reset_en : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t clkrst_rtc_wdt_cpu_reset_en:1;
        /** clkrst_cpu_stall_wait : R/W; bitpos: [30:26]; default: 1;
         *  need_des
         */
        uint32_t clkrst_cpu_stall_wait:5;
        /** clkrst_cpu_stall_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clkrst_cpu_stall_en:1;
    };
    uint32_t val;
} lp_clkrst_cpu_reset_reg_t;

/** Type of clkrst_fosc_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** clkrst_fosc_dfreq : R/W; bitpos: [31:22]; default: 600;
         *  need_des
         */
        uint32_t clkrst_fosc_dfreq:10;
    };
    uint32_t val;
} lp_clkrst_fosc_cntl_reg_t;

/** Type of clkrst_rc32k_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** clkrst_rc32k_dfreq : R/W; bitpos: [31:22]; default: 650;
         *  need_des
         */
        uint32_t clkrst_rc32k_dfreq:10;
    };
    uint32_t val;
} lp_clkrst_rc32k_cntl_reg_t;

/** Type of clkrst_clk_to_hp register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** clkrst_icg_hp_xtal32k : R/W; bitpos: [28]; default: 1;
         *  need_des
         */
        uint32_t clkrst_icg_hp_xtal32k:1;
        /** clkrst_icg_hp_sosc : R/W; bitpos: [29]; default: 1;
         *  need_des
         */
        uint32_t clkrst_icg_hp_sosc:1;
        /** clkrst_icg_hp_osc32k : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t clkrst_icg_hp_osc32k:1;
        /** clkrst_icg_hp_fosc : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t clkrst_icg_hp_fosc:1;
    };
    uint32_t val;
} lp_clkrst_clk_to_hp_reg_t;

/** Type of clkrst_lpmem_force register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clkrst_lpmem_clk_force_on : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lpmem_clk_force_on:1;
    };
    uint32_t val;
} lp_clkrst_lpmem_force_reg_t;

/** Type of clkrst_lpperi register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** clkrst_lp_bletimer_div_num : R/W; bitpos: [23:12]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_bletimer_div_num:12;
        /** clkrst_lp_bletimer_32k_sel : R/W; bitpos: [25:24]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_bletimer_32k_sel:2;
        /** clkrst_lp_sel_osc_slow : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_sel_osc_slow:1;
        /** clkrst_lp_sel_osc_fast : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_sel_osc_fast:1;
        /** clkrst_lp_sel_xtal : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_sel_xtal:1;
        /** clkrst_lp_sel_xtal32k : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_sel_xtal32k:1;
        /** clkrst_lp_i2c_clk_sel : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_i2c_clk_sel:1;
        /** clkrst_lp_uart_clk_sel : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clkrst_lp_uart_clk_sel:1;
    };
    uint32_t val;
} lp_clkrst_lpperi_reg_t;

/** Type of clkrst_xtal32k register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** clkrst_rtc_sel_power_xtal32k : R/W; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t clkrst_rtc_sel_power_xtal32k:1;
        /** clkrst_dres_xtal32k : R/W; bitpos: [24:22]; default: 3;
         *  need_des
         */
        uint32_t clkrst_dres_xtal32k:3;
        /** clkrst_dgm_xtal32k : R/W; bitpos: [27:25]; default: 3;
         *  need_des
         */
        uint32_t clkrst_dgm_xtal32k:3;
        /** clkrst_dbuf_xtal32k : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t clkrst_dbuf_xtal32k:1;
        /** clkrst_dac_xtal32k : R/W; bitpos: [31:29]; default: 3;
         *  need_des
         */
        uint32_t clkrst_dac_xtal32k:3;
    };
    uint32_t val;
} lp_clkrst_xtal32k_reg_t;

/** Type of clkrst_date register
 *  need_des
 */
typedef union {
    struct {
        /** clkrst_clkrst_date : R/W; bitpos: [30:0]; default: 37782064;
         *  need_des
         */
        uint32_t clkrst_clkrst_date:31;
        /** clkrst_clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clkrst_clk_en:1;
    };
    uint32_t val;
} lp_clkrst_date_reg_t;


typedef struct {
    volatile lp_clkrst_lp_clk_conf_reg_t lp_clk_conf;
    volatile lp_clkrst_lp_clk_po_en_reg_t lp_clk_po_en;
    volatile lp_clkrst_lp_clk_en_reg_t lp_clk_en;
    volatile lp_clkrst_lp_rst_en_reg_t lp_rst_en;
    volatile lp_clkrst_reset_cause_reg_t reset_cause;
    volatile lp_clkrst_cpu_reset_reg_t cpu_reset;
    volatile lp_clkrst_fosc_cntl_reg_t fosc_cntl;
    volatile lp_clkrst_rc32k_cntl_reg_t rc32k_cntl;
    volatile lp_clkrst_clk_to_hp_reg_t clk_to_hp;
    volatile lp_clkrst_lpmem_force_reg_t lpmem_force;
    volatile lp_clkrst_lpperi_reg_t lpperi;
    volatile lp_clkrst_xtal32k_reg_t xtal32k;
    uint32_t reserved_030[243];
    volatile lp_clkrst_date_reg_t date;
} lp_clkrst_dev_t;

extern lp_clkrst_dev_t LP_CLKRST;

#ifndef __cplusplus
_Static_assert(sizeof(lp_clkrst_dev_t) == 0x400, "Invalid size of lp_clkrst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
