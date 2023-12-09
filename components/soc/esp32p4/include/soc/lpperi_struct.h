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
/** Type of clk_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** ck_en_rng : R/W; bitpos: [16]; default: 1;
         *  need_des
         */
        uint32_t ck_en_rng:1;
        /** ck_en_lp_tsens : R/W; bitpos: [17]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_tsens:1;
        /** ck_en_lp_pms : R/W; bitpos: [18]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_pms:1;
        /** ck_en_lp_efuse : R/W; bitpos: [19]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_efuse:1;
        /** ck_en_lp_iomux : R/W; bitpos: [20]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_iomux:1;
        /** ck_en_lp_touch : R/W; bitpos: [21]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_touch:1;
        /** ck_en_lp_spi : R/W; bitpos: [22]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_spi:1;
        /** ck_en_lp_adc : R/W; bitpos: [23]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_adc:1;
        /** ck_en_lp_i2s_tx : R/W; bitpos: [24]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_i2s_tx:1;
        /** ck_en_lp_i2s_rx : R/W; bitpos: [25]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_i2s_rx:1;
        /** ck_en_lp_i2s : R/W; bitpos: [26]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_i2s:1;
        /** ck_en_lp_i2cmst : R/W; bitpos: [27]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_i2cmst:1;
        /** ck_en_lp_i2c : R/W; bitpos: [28]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_i2c:1;
        /** ck_en_lp_uart : R/W; bitpos: [29]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_uart:1;
        /** ck_en_lp_intr : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t ck_en_lp_intr:1;
        /** ck_en_lp_core : R/W; bitpos: [31]; default: 0;
         *  write 1 to force on lp_core clk
         */
        uint32_t ck_en_lp_core:1;
    };
    uint32_t val;
} lpperi_clk_en_reg_t;

/** Type of core_clk_sel register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** lp_i2s_tx_clk_sel : R/W; bitpos: [25:24]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_tx_clk_sel:2;
        /** lp_i2s_rx_clk_sel : R/W; bitpos: [27:26]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_rx_clk_sel:2;
        /** lp_i2c_clk_sel : R/W; bitpos: [29:28]; default: 0;
         *  need_des
         */
        uint32_t lp_i2c_clk_sel:2;
        /** lp_uart_clk_sel : R/W; bitpos: [31:30]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_clk_sel:2;
    };
    uint32_t val;
} lpperi_core_clk_sel_reg_t;

/** Type of reset_en register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:18;
        /** rst_en_lp_tsens : R/W; bitpos: [18]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_tsens:1;
        /** rst_en_lp_pms : R/W; bitpos: [19]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_pms:1;
        /** rst_en_lp_efuse : R/W; bitpos: [20]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_efuse:1;
        /** rst_en_lp_iomux : R/W; bitpos: [21]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_iomux:1;
        /** rst_en_lp_touch : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_touch:1;
        /** rst_en_lp_spi : R/W; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_spi:1;
        /** rst_en_lp_adc : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_adc:1;
        /** rst_en_lp_i2s : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_i2s:1;
        /** rst_en_lp_i2cmst : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_i2cmst:1;
        /** rst_en_lp_i2c : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_i2c:1;
        /** rst_en_lp_uart : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_uart:1;
        /** rst_en_lp_intr : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_intr:1;
        /** rst_en_lp_rom : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_rom:1;
        /** rst_en_lp_core : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t rst_en_lp_core:1;
    };
    uint32_t val;
} lpperi_reset_en_reg_t;

/** Type of cpu register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lpcore_dbgm_unavailable : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t lpcore_dbgm_unavailable:1;
    };
    uint32_t val;
} lpperi_cpu_reg_t;

/** Type of mem_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** lp_uart_wakeup_flag_clr : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_wakeup_flag_clr:1;
        /** lp_uart_wakeup_flag : R/WTC/SS; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_wakeup_flag:1;
        uint32_t reserved_2:27;
        /** lp_uart_wakeup_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_wakeup_en:1;
        /** lp_uart_mem_force_pd : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_mem_force_pd:1;
        /** lp_uart_mem_force_pu : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t lp_uart_mem_force_pu:1;
    };
    uint32_t val;
} lpperi_mem_ctrl_reg_t;

/** Type of adc_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** sar2_clk_force_on : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t sar2_clk_force_on:1;
        /** sar1_clk_force_on : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t sar1_clk_force_on:1;
        /** lpadc_func_div_num : R/W; bitpos: [15:8]; default: 4;
         *  need_des
         */
        uint32_t lpadc_func_div_num:8;
        /** lpadc_sar2_div_num : R/W; bitpos: [23:16]; default: 4;
         *  need_des
         */
        uint32_t lpadc_sar2_div_num:8;
        /** lpadc_sar1_div_num : R/W; bitpos: [31:24]; default: 4;
         *  need_des
         */
        uint32_t lpadc_sar1_div_num:8;
    };
    uint32_t val;
} lpperi_adc_ctrl_reg_t;

/** Type of lp_i2s_rxclk_div_num register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** lp_i2s_rx_clkm_div_num : R/W; bitpos: [31:24]; default: 2;
         *  need_des
         */
        uint32_t lp_i2s_rx_clkm_div_num:8;
    };
    uint32_t val;
} lpperi_lp_i2s_rxclk_div_num_reg_t;

/** Type of lp_i2s_rxclk_div_xyz register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** lp_i2s_rx_clkm_div_yn1 : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_rx_clkm_div_yn1:1;
        /** lp_i2s_rx_clkm_div_z : R/W; bitpos: [13:5]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_rx_clkm_div_z:9;
        /** lp_i2s_rx_clkm_div_y : R/W; bitpos: [22:14]; default: 1;
         *  need_des
         */
        uint32_t lp_i2s_rx_clkm_div_y:9;
        /** lp_i2s_rx_clkm_div_x : R/W; bitpos: [31:23]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_rx_clkm_div_x:9;
    };
    uint32_t val;
} lpperi_lp_i2s_rxclk_div_xyz_reg_t;

/** Type of lp_i2s_txclk_div_num register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** lp_i2s_tx_clkm_div_num : R/W; bitpos: [31:24]; default: 2;
         *  need_des
         */
        uint32_t lp_i2s_tx_clkm_div_num:8;
    };
    uint32_t val;
} lpperi_lp_i2s_txclk_div_num_reg_t;

/** Type of lp_i2s_txclk_div_xyz register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** lp_i2s_tx_clkm_div_yn1 : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_tx_clkm_div_yn1:1;
        /** lp_i2s_tx_clkm_div_z : R/W; bitpos: [13:5]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_tx_clkm_div_z:9;
        /** lp_i2s_tx_clkm_div_y : R/W; bitpos: [22:14]; default: 1;
         *  need_des
         */
        uint32_t lp_i2s_tx_clkm_div_y:9;
        /** lp_i2s_tx_clkm_div_x : R/W; bitpos: [31:23]; default: 0;
         *  need_des
         */
        uint32_t lp_i2s_tx_clkm_div_x:9;
    };
    uint32_t val;
} lpperi_lp_i2s_txclk_div_xyz_reg_t;


/** Group: Version register */
/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lpperi_date_reg_t;


typedef struct lpperi_dev_t {
    volatile lpperi_clk_en_reg_t clk_en;
    volatile lpperi_core_clk_sel_reg_t core_clk_sel;
    volatile lpperi_reset_en_reg_t reset_en;
    volatile lpperi_cpu_reg_t cpu;
    uint32_t reserved_010[6];
    volatile lpperi_mem_ctrl_reg_t mem_ctrl;
    volatile lpperi_adc_ctrl_reg_t adc_ctrl;
    volatile lpperi_lp_i2s_rxclk_div_num_reg_t lp_i2s_rxclk_div_num;
    volatile lpperi_lp_i2s_rxclk_div_xyz_reg_t lp_i2s_rxclk_div_xyz;
    volatile lpperi_lp_i2s_txclk_div_num_reg_t lp_i2s_txclk_div_num;
    volatile lpperi_lp_i2s_txclk_div_xyz_reg_t lp_i2s_txclk_div_xyz;
    uint32_t reserved_040[239];
    volatile lpperi_date_reg_t date;
} lpperi_dev_t;

extern lpperi_dev_t LPPERI;

#ifndef __cplusplus
_Static_assert(sizeof(lpperi_dev_t) == 0x400, "Invalid size of lpperi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
