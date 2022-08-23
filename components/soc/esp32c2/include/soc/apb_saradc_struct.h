/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of saradc_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** saradc_saradc_start_force : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc_start_force:1;
        /** saradc_saradc_start : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc_start:1;
        uint32_t reserved_2:4;
        /** saradc_saradc_sar_clk_gated : R/W; bitpos: [6]; default: 1;
         *  Need add description
         */
        uint32_t saradc_saradc_sar_clk_gated:1;
        /** saradc_saradc_sar_clk_div : R/W; bitpos: [14:7]; default: 4;
         *  SAR clock divider
         */
        uint32_t saradc_saradc_sar_clk_div:8;
        /** saradc_saradc_sar_patt_len : R/W; bitpos: [17:15]; default: 7;
         *  0 ~ 15 means length 1 ~ 16
         */
        uint32_t saradc_saradc_sar_patt_len:3;
        uint32_t reserved_18:5;
        /** saradc_saradc_sar_patt_p_clear : R/W; bitpos: [23]; default: 0;
         *  clear the pointer of pattern table for DIG ADC1 CTRL
         */
        uint32_t saradc_saradc_sar_patt_p_clear:1;
        uint32_t reserved_24:3;
        /** saradc_saradc_xpd_sar_force : R/W; bitpos: [28:27]; default: 0;
         *  force option to xpd sar blocks
         */
        uint32_t saradc_saradc_xpd_sar_force:2;
        uint32_t reserved_29:1;
        /** saradc_saradc_wait_arb_cycle : R/W; bitpos: [31:30]; default: 1;
         *  wait arbit signal stable after sar_done
         */
        uint32_t saradc_saradc_wait_arb_cycle:2;
    };
    uint32_t val;
} apb_saradc_ctrl_reg_t;

/** Type of saradc_ctrl2 register
 *  register description
 */
typedef union {
    struct {
        /** saradc_saradc_meas_num_limit : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc_meas_num_limit:1;
        /** saradc_saradc_max_meas_num : R/W; bitpos: [8:1]; default: 255;
         *  max conversion number
         */
        uint32_t saradc_saradc_max_meas_num:8;
        /** saradc_saradc_sar1_inv : R/W; bitpos: [9]; default: 0;
         *  1: data to DIG ADC1 CTRL is inverted, otherwise not
         */
        uint32_t saradc_saradc_sar1_inv:1;
        /** saradc_saradc_sar2_inv : R/W; bitpos: [10]; default: 0;
         *  1: data to DIG ADC2 CTRL is inverted, otherwise not
         */
        uint32_t saradc_saradc_sar2_inv:1;
        uint32_t reserved_11:1;
        /** saradc_saradc_timer_target : R/W; bitpos: [23:12]; default: 10;
         *  to set saradc timer target
         */
        uint32_t saradc_saradc_timer_target:12;
        /** saradc_saradc_timer_en : R/W; bitpos: [24]; default: 0;
         *  to enable saradc timer trigger
         */
        uint32_t saradc_saradc_timer_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} apb_saradc_ctrl2_reg_t;

/** Type of saradc_filter_ctrl1 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** saradc_filter_factor1 : R/W; bitpos: [28:26]; default: 0;
         *  Need add description
         */
        uint32_t saradc_filter_factor1:3;
        /** saradc_filter_factor0 : R/W; bitpos: [31:29]; default: 0;
         *  Need add description
         */
        uint32_t saradc_filter_factor0:3;
    };
    uint32_t val;
} apb_saradc_filter_ctrl1_reg_t;

/** Type of saradc_fsm_wait register
 *  register description
 */
typedef union {
    struct {
        /** saradc_saradc_xpd_wait : R/W; bitpos: [7:0]; default: 8;
         *  Need add description
         */
        uint32_t saradc_saradc_xpd_wait:8;
        /** saradc_saradc_rstb_wait : R/W; bitpos: [15:8]; default: 8;
         *  Need add description
         */
        uint32_t saradc_saradc_rstb_wait:8;
        /** saradc_saradc_standby_wait : R/W; bitpos: [23:16]; default: 255;
         *  Need add description
         */
        uint32_t saradc_saradc_standby_wait:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} apb_saradc_fsm_wait_reg_t;

/** Type of saradc_sar1_status register
 *  register description
 */
typedef union {
    struct {
        /** saradc_saradc_sar1_status : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc_sar1_status:32;
    };
    uint32_t val;
} apb_saradc_sar1_status_reg_t;

/** Type of saradc_sar2_status register
 *  register description
 */
typedef union {
    struct {
        /** saradc_saradc_sar2_status : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc_sar2_status:32;
    };
    uint32_t val;
} apb_saradc_sar2_status_reg_t;

/** Type of saradc_sar_patt_tab1 register
 *  register description
 */
typedef union {
    struct {
        /** saradc_saradc_sar_patt_tab1 : R/W; bitpos: [23:0]; default: 16777215;
         *  item 0 ~ 3 for pattern table 1 (each item one byte)
         */
        uint32_t saradc_saradc_sar_patt_tab1:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} apb_saradc_sar_patt_tab1_reg_t;

/** Type of saradc_sar_patt_tab2 register
 *  register description
 */
typedef union {
    struct {
        /** saradc_saradc_sar_patt_tab2 : R/W; bitpos: [23:0]; default: 16777215;
         *  Item 4 ~ 7 for pattern table 1 (each item one byte)
         */
        uint32_t saradc_saradc_sar_patt_tab2:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} apb_saradc_sar_patt_tab2_reg_t;

/** Type of saradc_onetime_sample register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** saradc_saradc_onetime_atten : R/W; bitpos: [24:23]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc_onetime_atten:2;
        /** saradc_saradc_onetime_channel : R/W; bitpos: [28:25]; default: 13;
         *  Need add description
         */
        uint32_t saradc_saradc_onetime_channel:4;
        /** saradc_saradc_onetime_start : R/W; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc_onetime_start:1;
        /** saradc_saradc2_onetime_sample : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc2_onetime_sample:1;
        /** saradc_saradc1_onetime_sample : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t saradc_saradc1_onetime_sample:1;
    };
    uint32_t val;
} apb_saradc_onetime_sample_reg_t;

/** Type of saradc_apb_adc_arb_ctrl register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** saradc_adc_arb_apb_force : R/W; bitpos: [2]; default: 0;
         *  adc2 arbiter force to enableapb controller
         */
        uint32_t saradc_adc_arb_apb_force:1;
        /** saradc_adc_arb_rtc_force : R/W; bitpos: [3]; default: 0;
         *  adc2 arbiter force to enable rtc controller
         */
        uint32_t saradc_adc_arb_rtc_force:1;
        /** saradc_adc_arb_wifi_force : R/W; bitpos: [4]; default: 0;
         *  adc2 arbiter force to enable wifi controller
         */
        uint32_t saradc_adc_arb_wifi_force:1;
        /** saradc_adc_arb_grant_force : R/W; bitpos: [5]; default: 0;
         *  adc2 arbiter force grant
         */
        uint32_t saradc_adc_arb_grant_force:1;
        /** saradc_adc_arb_apb_priority : R/W; bitpos: [7:6]; default: 0;
         *  Set adc2 arbiterapb priority
         */
        uint32_t saradc_adc_arb_apb_priority:2;
        /** saradc_adc_arb_rtc_priority : R/W; bitpos: [9:8]; default: 1;
         *  Set adc2 arbiter rtc priority
         */
        uint32_t saradc_adc_arb_rtc_priority:2;
        /** saradc_adc_arb_wifi_priority : R/W; bitpos: [11:10]; default: 2;
         *  Set adc2 arbiter wifi priority
         */
        uint32_t saradc_adc_arb_wifi_priority:2;
        /** saradc_adc_arb_fix_priority : R/W; bitpos: [12]; default: 0;
         *  adc2 arbiter uses fixed priority
         */
        uint32_t saradc_adc_arb_fix_priority:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} apb_saradc_apb_adc_arb_ctrl_reg_t;

/** Type of saradc_filter_ctrl0 register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:18;
        /** saradc_filter_channel1 : R/W; bitpos: [21:18]; default: 13;
         *  Need add description
         */
        uint32_t saradc_filter_channel1:4;
        /** saradc_filter_channel0 : R/W; bitpos: [25:22]; default: 13;
         *  apb_adc1_filter_factor
         */
        uint32_t saradc_filter_channel0:4;
        uint32_t reserved_26:5;
        /** saradc_filter_reset : R/W; bitpos: [31]; default: 0;
         *  enable apb_adc1_filter
         */
        uint32_t saradc_filter_reset:1;
    };
    uint32_t val;
} apb_saradc_filter_ctrl0_reg_t;

/** Type of saradc1_data_status register
 *  register description
 */
typedef union {
    struct {
        /** saradc1_data : RO; bitpos: [16:0]; default: 0;
         *  Need add description
         */
        uint32_t saradc1_data:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} apb_saradc1_data_status_reg_t;

/** Type of saradc2_data_status register
 *  register description
 */
typedef union {
    struct {
        /** saradc2_data : RO; bitpos: [16:0]; default: 0;
         *  Need add description
         */
        uint32_t saradc2_data:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} apb_saradc2_data_status_reg_t;

/** Type of saradc_thres0_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** saradc_thres0_channel : R/W; bitpos: [3:0]; default: 13;
         *  Need add description
         */
        uint32_t saradc_thres0_channel:4;
        uint32_t reserved_4:1;
        /** saradc_thres0_high : R/W; bitpos: [17:5]; default: 8191;
         *  saradc1's thres0 monitor thres
         */
        uint32_t saradc_thres0_high:13;
        /** saradc_thres0_low : R/W; bitpos: [30:18]; default: 0;
         *  saradc1's thres0 monitor thres
         */
        uint32_t saradc_thres0_low:13;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} apb_saradc_thres0_ctrl_reg_t;

/** Type of saradc_thres1_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** saradc_thres1_channel : R/W; bitpos: [3:0]; default: 13;
         *  Need add description
         */
        uint32_t saradc_thres1_channel:4;
        uint32_t reserved_4:1;
        /** saradc_thres1_high : R/W; bitpos: [17:5]; default: 8191;
         *  saradc1's thres0 monitor thres
         */
        uint32_t saradc_thres1_high:13;
        /** saradc_thres1_low : R/W; bitpos: [30:18]; default: 0;
         *  saradc1's thres0 monitor thres
         */
        uint32_t saradc_thres1_low:13;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} apb_saradc_thres1_ctrl_reg_t;

/** Type of saradc_thres_ctrl register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** saradc_thres_all_en : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres_all_en:1;
        /** saradc_thres3_en : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres3_en:1;
        /** saradc_thres2_en : R/W; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres2_en:1;
        /** saradc_thres1_en : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_en:1;
        /** saradc_thres0_en : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_en:1;
    };
    uint32_t val;
} apb_saradc_thres_ctrl_reg_t;

/** Type of saradc_int_ena register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** saradc_thres1_low_int_ena : R/W; bitpos: [26]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_low_int_ena:1;
        /** saradc_thres0_low_int_ena : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_low_int_ena:1;
        /** saradc_thres1_high_int_ena : R/W; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_high_int_ena:1;
        /** saradc_thres0_high_int_ena : R/W; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_high_int_ena:1;
        /** saradc2_done_int_ena : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t saradc2_done_int_ena:1;
        /** saradc1_done_int_ena : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t saradc1_done_int_ena:1;
    };
    uint32_t val;
} apb_saradc_int_ena_reg_t;

/** Type of saradc_int_raw register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** saradc_thres1_low_int_raw : RO; bitpos: [26]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_low_int_raw:1;
        /** saradc_thres0_low_int_raw : RO; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_low_int_raw:1;
        /** saradc_thres1_high_int_raw : RO; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_high_int_raw:1;
        /** saradc_thres0_high_int_raw : RO; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_high_int_raw:1;
        /** saradc2_done_int_raw : RO; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t saradc2_done_int_raw:1;
        /** saradc1_done_int_raw : RO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t saradc1_done_int_raw:1;
    };
    uint32_t val;
} apb_saradc_int_raw_reg_t;

/** Type of saradc_int_st register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** saradc_thres1_low_int_st : RO; bitpos: [26]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_low_int_st:1;
        /** saradc_thres0_low_int_st : RO; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_low_int_st:1;
        /** saradc_thres1_high_int_st : RO; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_high_int_st:1;
        /** saradc_thres0_high_int_st : RO; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_high_int_st:1;
        /** saradc2_done_int_st : RO; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t saradc2_done_int_st:1;
        /** saradc1_done_int_st : RO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t saradc1_done_int_st:1;
    };
    uint32_t val;
} apb_saradc_int_st_reg_t;

/** Type of saradc_int_clr register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** saradc_thres1_low_int_clr : WO; bitpos: [26]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_low_int_clr:1;
        /** saradc_thres0_low_int_clr : WO; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_low_int_clr:1;
        /** saradc_thres1_high_int_clr : WO; bitpos: [28]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres1_high_int_clr:1;
        /** saradc_thres0_high_int_clr : WO; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t saradc_thres0_high_int_clr:1;
        /** saradc2_done_int_clr : WO; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t saradc2_done_int_clr:1;
        /** saradc1_done_int_clr : WO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t saradc1_done_int_clr:1;
    };
    uint32_t val;
} apb_saradc_int_clr_reg_t;

/** Type of saradc_dma_conf register
 *  register description
 */
typedef union {
    struct {
        /** saradc_apb_adc_eof_num : R/W; bitpos: [15:0]; default: 255;
         *  the dma_in_suc_eof gen when sample cnt = spi_eof_num
         */
        uint32_t saradc_apb_adc_eof_num:16;
        uint32_t reserved_16:14;
        /** saradc_apb_adc_reset_fsm : R/W; bitpos: [30]; default: 0;
         *  reset_apb_adc_state
         */
        uint32_t saradc_apb_adc_reset_fsm:1;
        /** saradc_apb_adc_trans : R/W; bitpos: [31]; default: 0;
         *  enable apb_adc use spi_dma
         */
        uint32_t saradc_apb_adc_trans:1;
    };
    uint32_t val;
} apb_saradc_dma_conf_reg_t;

/** Type of saradc_apb_adc_clkm_conf register
 *  register description
 */
typedef union {
    struct {
        /** saradc_reg_clkm_div_num : R/W; bitpos: [7:0]; default: 4;
         *  Integral I2S clock divider value
         */
        uint32_t saradc_reg_clkm_div_num:8;
        /** saradc_reg_clkm_div_b : R/W; bitpos: [13:8]; default: 0;
         *  Fractional clock divider numerator value
         */
        uint32_t saradc_reg_clkm_div_b:6;
        /** saradc_reg_clkm_div_a : R/W; bitpos: [19:14]; default: 0;
         *  Fractional clock divider denominator value
         */
        uint32_t saradc_reg_clkm_div_a:6;
        /** saradc_clk_en : R/W; bitpos: [20]; default: 0;
         *  Need add description
         */
        uint32_t saradc_clk_en:1;
        /** saradc_reg_clk_sel : R/W; bitpos: [22:21]; default: 0;
         *  Set this bit to enable clk_apll
         */
        uint32_t saradc_reg_clk_sel:2;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} apb_saradc_apb_adc_clkm_conf_reg_t;

/** Type of saradc_apb_tsens_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** saradc_reg_tsens_out : RO; bitpos: [7:0]; default: 0;
         *  Need add description
         */
        uint32_t saradc_reg_tsens_out:8;
        uint32_t reserved_8:5;
        /** saradc_reg_tsens_in_inv : R/W; bitpos: [13]; default: 0;
         *  Need add description
         */
        uint32_t saradc_reg_tsens_in_inv:1;
        /** saradc_reg_tsens_clk_div : R/W; bitpos: [21:14]; default: 6;
         *  Need add description
         */
        uint32_t saradc_reg_tsens_clk_div:8;
        /** saradc_reg_tsens_pu : R/W; bitpos: [22]; default: 0;
         *  Need add description
         */
        uint32_t saradc_reg_tsens_pu:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} apb_saradc_apb_tsens_ctrl_reg_t;

/** Type of saradc_apb_tsens_ctrl2 register
 *  register description
 */
typedef union {
    struct {
        /** saradc_reg_tsens_xpd_wait : R/W; bitpos: [11:0]; default: 2;
         *  Need add description
         */
        uint32_t saradc_reg_tsens_xpd_wait:12;
        /** saradc_reg_tsens_xpd_force : R/W; bitpos: [13:12]; default: 0;
         *  Need add description
         */
        uint32_t saradc_reg_tsens_xpd_force:2;
        /** saradc_reg_tsens_clk_inv : R/W; bitpos: [14]; default: 1;
         *  Need add description
         */
        uint32_t saradc_reg_tsens_clk_inv:1;
        /** saradc_tsens_clk_sel : R/W; bitpos: [15]; default: 0;
         *  Need add description
         */
        uint32_t saradc_tsens_clk_sel:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} apb_saradc_apb_tsens_ctrl2_reg_t;

/** Type of saradc_cali register
 *  register description
 */
typedef union {
    struct {
        /** saradc_cali_cfg : R/W; bitpos: [16:0]; default: 32768;
         *  Need add description
         */
        uint32_t saradc_cali_cfg:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} apb_saradc_cali_reg_t;

/** Type of saradc_apb_ctrl_date register
 *  register description
 */
typedef union {
    struct {
        /** saradc_date : R/W; bitpos: [31:0]; default: 34632208;
         *  Need add description
         */
        uint32_t saradc_date:32;
    };
    uint32_t val;
} apb_saradc_apb_ctrl_date_reg_t;


typedef struct {
    volatile apb_saradc_ctrl_reg_t saradc_ctrl;
    volatile apb_saradc_ctrl2_reg_t saradc_ctrl2;
    volatile apb_saradc_filter_ctrl1_reg_t saradc_filter_ctrl1;
    volatile apb_saradc_fsm_wait_reg_t saradc_fsm_wait;
    volatile apb_saradc_sar1_status_reg_t saradc_sar1_status;
    volatile apb_saradc_sar2_status_reg_t saradc_sar2_status;
    volatile apb_saradc_sar_patt_tab1_reg_t saradc_sar_patt_tab1;
    volatile apb_saradc_sar_patt_tab2_reg_t saradc_sar_patt_tab2;
    volatile apb_saradc_onetime_sample_reg_t saradc_onetime_sample;
    volatile apb_saradc_apb_adc_arb_ctrl_reg_t saradc_apb_adc_arb_ctrl;
    volatile apb_saradc_filter_ctrl0_reg_t saradc_filter_ctrl0;
    volatile apb_saradc1_data_status_reg_t saradc1_data_status;
    volatile apb_saradc2_data_status_reg_t saradc2_data_status;
    volatile apb_saradc_thres0_ctrl_reg_t saradc_thres0_ctrl;
    volatile apb_saradc_thres1_ctrl_reg_t saradc_thres1_ctrl;
    volatile apb_saradc_thres_ctrl_reg_t saradc_thres_ctrl;
    volatile apb_saradc_int_ena_reg_t saradc_int_ena;
    volatile apb_saradc_int_raw_reg_t saradc_int_raw;
    volatile apb_saradc_int_st_reg_t saradc_int_st;
    volatile apb_saradc_int_clr_reg_t saradc_int_clr;
    volatile apb_saradc_dma_conf_reg_t saradc_dma_conf;
    volatile apb_saradc_apb_adc_clkm_conf_reg_t saradc_apb_adc_clkm_conf;
    volatile apb_saradc_apb_tsens_ctrl_reg_t saradc_apb_tsens_ctrl;
    volatile apb_saradc_apb_tsens_ctrl2_reg_t saradc_apb_tsens_ctrl2;
    volatile apb_saradc_cali_reg_t saradc_cali;
    uint32_t reserved_064[230];
    volatile apb_saradc_apb_ctrl_date_reg_t saradc_apb_ctrl_date;
} apb_dev_t;

extern apb_dev_t APB_SARADC;

#ifndef __cplusplus
_Static_assert(sizeof(apb_dev_t) == 0x400, "Invalid size of apb_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
