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

/** Group: Configure Register */
/** Type of saradc_ctrl register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_saradc_start_force : R/W; bitpos: [0]; default: 0;
         *  select software enable saradc sample
         */
        uint32_t saradc_saradc_start_force:1;
        /** saradc_saradc_start : R/W; bitpos: [1]; default: 0;
         *  software enable saradc sample
         */
        uint32_t saradc_saradc_start:1;
        uint32_t reserved_2:4;
        /** saradc_saradc_sar_clk_gated : R/W; bitpos: [6]; default: 1;
         *  SAR clock gated
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
        /** saradc_saradc2_pwdet_drv : R/W; bitpos: [29]; default: 0;
         *  enable saradc2 power detect driven func.
         */
        uint32_t saradc_saradc2_pwdet_drv:1;
        /** saradc_saradc_wait_arb_cycle : R/W; bitpos: [31:30]; default: 1;
         *  wait arbit signal stable after sar_done
         */
        uint32_t saradc_saradc_wait_arb_cycle:2;
    };
    uint32_t val;
} apb_saradc_ctrl_reg_t;

/** Type of saradc_ctrl2 register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_saradc_meas_num_limit : R/W; bitpos: [0]; default: 0;
         *  enable max meas num
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
 *  digital saradc configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** saradc_apb_saradc_filter_factor1 : R/W; bitpos: [28:26]; default: 0;
         *  Factor of saradc filter1
         */
        uint32_t saradc_apb_saradc_filter_factor1:3;
        /** saradc_apb_saradc_filter_factor0 : R/W; bitpos: [31:29]; default: 0;
         *  Factor of saradc filter0
         */
        uint32_t saradc_apb_saradc_filter_factor0:3;
    };
    uint32_t val;
} apb_saradc_filter_ctrl1_reg_t;

/** Type of saradc_fsm_wait register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_saradc_xpd_wait : R/W; bitpos: [7:0]; default: 8;
         *  saradc_xpd_wait
         */
        uint32_t saradc_saradc_xpd_wait:8;
        /** saradc_saradc_rstb_wait : R/W; bitpos: [15:8]; default: 8;
         *  saradc_rstb_wait
         */
        uint32_t saradc_saradc_rstb_wait:8;
        /** saradc_saradc_standby_wait : R/W; bitpos: [23:16]; default: 255;
         *  saradc_standby_wait
         */
        uint32_t saradc_saradc_standby_wait:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} apb_saradc_fsm_wait_reg_t;

/** Type of saradc_sar1_status register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_saradc_sar1_status : RO; bitpos: [31:0]; default: 536870912;
         *  saradc1 status about data and channel
         */
        uint32_t saradc_saradc_sar1_status:32;
    };
    uint32_t val;
} apb_saradc_sar1_status_reg_t;

/** Type of saradc_sar2_status register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_saradc_sar2_status : RO; bitpos: [31:0]; default: 536870912;
         *  saradc2 status about data and channel
         */
        uint32_t saradc_saradc_sar2_status:32;
    };
    uint32_t val;
} apb_saradc_sar2_status_reg_t;

/** Type of saradc_sar_patt_tab1 register
 *  digital saradc configure register
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
 *  digital saradc configure register
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
 *  digital saradc configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** saradc_saradc_onetime_atten : R/W; bitpos: [24:23]; default: 0;
         *  configure onetime atten
         */
        uint32_t saradc_saradc_onetime_atten:2;
        /** saradc_saradc_onetime_channel : R/W; bitpos: [28:25]; default: 13;
         *  configure onetime channel
         */
        uint32_t saradc_saradc_onetime_channel:4;
        /** saradc_saradc_onetime_start : R/W; bitpos: [29]; default: 0;
         *  trigger adc onetime sample
         */
        uint32_t saradc_saradc_onetime_start:1;
        /** saradc_saradc2_onetime_sample : R/W; bitpos: [30]; default: 0;
         *  enable adc2 onetime sample
         */
        uint32_t saradc_saradc2_onetime_sample:1;
        /** saradc_saradc1_onetime_sample : R/W; bitpos: [31]; default: 0;
         *  enable adc1 onetime sample
         */
        uint32_t saradc_saradc1_onetime_sample:1;
    };
    uint32_t val;
} apb_saradc_onetime_sample_reg_t;

/** Type of saradc_arb_ctrl register
 *  digital saradc configure register
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
} apb_saradc_arb_ctrl_reg_t;

/** Type of saradc_filter_ctrl0 register
 *  digital saradc configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:18;
        /** saradc_apb_saradc_filter_channel1 : R/W; bitpos: [21:18]; default: 13;
         *  configure filter1 to adc channel
         */
        uint32_t saradc_apb_saradc_filter_channel1:4;
        /** saradc_apb_saradc_filter_channel0 : R/W; bitpos: [25:22]; default: 13;
         *  configure filter0 to adc channel
         */
        uint32_t saradc_apb_saradc_filter_channel0:4;
        uint32_t reserved_26:5;
        /** saradc_apb_saradc_filter_reset : R/W; bitpos: [31]; default: 0;
         *  enable apb_adc1_filter
         */
        uint32_t saradc_apb_saradc_filter_reset:1;
    };
    uint32_t val;
} apb_saradc_filter_ctrl0_reg_t;

/** Type of saradc_sar1data_status register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_apb_saradc1_data : RO; bitpos: [16:0]; default: 0;
         *  saradc1 data
         */
        uint32_t saradc_apb_saradc1_data:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} apb_saradc_sar1data_status_reg_t;

/** Type of saradc_sar2data_status register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_apb_saradc2_data : RO; bitpos: [16:0]; default: 0;
         *  saradc2 data
         */
        uint32_t saradc_apb_saradc2_data:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} apb_saradc_sar2data_status_reg_t;

/** Type of saradc_thres0_ctrl register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_apb_saradc_thres0_channel : R/W; bitpos: [3:0]; default: 13;
         *  configure thres0 to adc channel
         */
        uint32_t saradc_apb_saradc_thres0_channel:4;
        uint32_t reserved_4:1;
        /** saradc_apb_saradc_thres0_high : R/W; bitpos: [17:5]; default: 8191;
         *  saradc thres0 monitor thres
         */
        uint32_t saradc_apb_saradc_thres0_high:13;
        /** saradc_apb_saradc_thres0_low : R/W; bitpos: [30:18]; default: 0;
         *  saradc thres0 monitor thres
         */
        uint32_t saradc_apb_saradc_thres0_low:13;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} apb_saradc_thres0_ctrl_reg_t;

/** Type of saradc_thres1_ctrl register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_apb_saradc_thres1_channel : R/W; bitpos: [3:0]; default: 13;
         *  configure thres1 to adc channel
         */
        uint32_t saradc_apb_saradc_thres1_channel:4;
        uint32_t reserved_4:1;
        /** saradc_apb_saradc_thres1_high : R/W; bitpos: [17:5]; default: 8191;
         *  saradc thres1 monitor thres
         */
        uint32_t saradc_apb_saradc_thres1_high:13;
        /** saradc_apb_saradc_thres1_low : R/W; bitpos: [30:18]; default: 0;
         *  saradc thres1 monitor thres
         */
        uint32_t saradc_apb_saradc_thres1_low:13;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} apb_saradc_thres1_ctrl_reg_t;

/** Type of saradc_thres_ctrl register
 *  digital saradc configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** saradc_apb_saradc_thres_all_en : R/W; bitpos: [27]; default: 0;
         *  enable thres to all channel
         */
        uint32_t saradc_apb_saradc_thres_all_en:1;
        uint32_t reserved_28:2;
        /** saradc_apb_saradc_thres1_en : R/W; bitpos: [30]; default: 0;
         *  enable thres1
         */
        uint32_t saradc_apb_saradc_thres1_en:1;
        /** saradc_apb_saradc_thres0_en : R/W; bitpos: [31]; default: 0;
         *  enable thres0
         */
        uint32_t saradc_apb_saradc_thres0_en:1;
    };
    uint32_t val;
} apb_saradc_thres_ctrl_reg_t;

/** Type of saradc_int_ena register
 *  digital saradc int register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** saradc_apb_saradc_tsens_int_ena : R/W; bitpos: [25]; default: 0;
         *  tsens low  interrupt enable
         */
        uint32_t saradc_apb_saradc_tsens_int_ena:1;
        /** saradc_apb_saradc_thres1_low_int_ena : R/W; bitpos: [26]; default: 0;
         *  saradc thres1 low  interrupt enable
         */
        uint32_t saradc_apb_saradc_thres1_low_int_ena:1;
        /** saradc_apb_saradc_thres0_low_int_ena : R/W; bitpos: [27]; default: 0;
         *  saradc thres0 low interrupt enable
         */
        uint32_t saradc_apb_saradc_thres0_low_int_ena:1;
        /** saradc_apb_saradc_thres1_high_int_ena : R/W; bitpos: [28]; default: 0;
         *  saradc thres1 high interrupt enable
         */
        uint32_t saradc_apb_saradc_thres1_high_int_ena:1;
        /** saradc_apb_saradc_thres0_high_int_ena : R/W; bitpos: [29]; default: 0;
         *  saradc thres0 high interrupt enable
         */
        uint32_t saradc_apb_saradc_thres0_high_int_ena:1;
        /** saradc_apb_saradc2_done_int_ena : R/W; bitpos: [30]; default: 0;
         *  saradc2 done interrupt enable
         */
        uint32_t saradc_apb_saradc2_done_int_ena:1;
        /** saradc_apb_saradc1_done_int_ena : R/W; bitpos: [31]; default: 0;
         *  saradc1 done interrupt enable
         */
        uint32_t saradc_apb_saradc1_done_int_ena:1;
    };
    uint32_t val;
} apb_saradc_int_ena_reg_t;

/** Type of saradc_int_raw register
 *  digital saradc int register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** saradc_apb_saradc_tsens_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  saradc tsens  interrupt raw
         */
        uint32_t saradc_apb_saradc_tsens_int_raw:1;
        /** saradc_apb_saradc_thres1_low_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  saradc thres1 low  interrupt raw
         */
        uint32_t saradc_apb_saradc_thres1_low_int_raw:1;
        /** saradc_apb_saradc_thres0_low_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  saradc thres0 low interrupt raw
         */
        uint32_t saradc_apb_saradc_thres0_low_int_raw:1;
        /** saradc_apb_saradc_thres1_high_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  saradc thres1 high interrupt raw
         */
        uint32_t saradc_apb_saradc_thres1_high_int_raw:1;
        /** saradc_apb_saradc_thres0_high_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  saradc thres0 high interrupt raw
         */
        uint32_t saradc_apb_saradc_thres0_high_int_raw:1;
        /** saradc_apb_saradc2_done_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  saradc2 done interrupt raw
         */
        uint32_t saradc_apb_saradc2_done_int_raw:1;
        /** saradc_apb_saradc1_done_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  saradc1 done interrupt raw
         */
        uint32_t saradc_apb_saradc1_done_int_raw:1;
    };
    uint32_t val;
} apb_saradc_int_raw_reg_t;

/** Type of saradc_int_st register
 *  digital saradc int register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** saradc_apb_saradc_tsens_int_st : RO; bitpos: [25]; default: 0;
         *  saradc tsens  interrupt state
         */
        uint32_t saradc_apb_saradc_tsens_int_st:1;
        /** saradc_apb_saradc_thres1_low_int_st : RO; bitpos: [26]; default: 0;
         *  saradc thres1 low  interrupt state
         */
        uint32_t saradc_apb_saradc_thres1_low_int_st:1;
        /** saradc_apb_saradc_thres0_low_int_st : RO; bitpos: [27]; default: 0;
         *  saradc thres0 low interrupt state
         */
        uint32_t saradc_apb_saradc_thres0_low_int_st:1;
        /** saradc_apb_saradc_thres1_high_int_st : RO; bitpos: [28]; default: 0;
         *  saradc thres1 high interrupt state
         */
        uint32_t saradc_apb_saradc_thres1_high_int_st:1;
        /** saradc_apb_saradc_thres0_high_int_st : RO; bitpos: [29]; default: 0;
         *  saradc thres0 high interrupt state
         */
        uint32_t saradc_apb_saradc_thres0_high_int_st:1;
        /** saradc_apb_saradc2_done_int_st : RO; bitpos: [30]; default: 0;
         *  saradc2 done interrupt state
         */
        uint32_t saradc_apb_saradc2_done_int_st:1;
        /** saradc_apb_saradc1_done_int_st : RO; bitpos: [31]; default: 0;
         *  saradc1 done interrupt state
         */
        uint32_t saradc_apb_saradc1_done_int_st:1;
    };
    uint32_t val;
} apb_saradc_int_st_reg_t;

/** Type of saradc_int_clr register
 *  digital saradc int register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** saradc_apb_saradc_tsens_int_clr : WT; bitpos: [25]; default: 0;
         *  saradc tsens  interrupt clear
         */
        uint32_t saradc_apb_saradc_tsens_int_clr:1;
        /** saradc_apb_saradc_thres1_low_int_clr : WT; bitpos: [26]; default: 0;
         *  saradc thres1 low  interrupt clear
         */
        uint32_t saradc_apb_saradc_thres1_low_int_clr:1;
        /** saradc_apb_saradc_thres0_low_int_clr : WT; bitpos: [27]; default: 0;
         *  saradc thres0 low interrupt clear
         */
        uint32_t saradc_apb_saradc_thres0_low_int_clr:1;
        /** saradc_apb_saradc_thres1_high_int_clr : WT; bitpos: [28]; default: 0;
         *  saradc thres1 high interrupt clear
         */
        uint32_t saradc_apb_saradc_thres1_high_int_clr:1;
        /** saradc_apb_saradc_thres0_high_int_clr : WT; bitpos: [29]; default: 0;
         *  saradc thres0 high interrupt clear
         */
        uint32_t saradc_apb_saradc_thres0_high_int_clr:1;
        /** saradc_apb_saradc2_done_int_clr : WT; bitpos: [30]; default: 0;
         *  saradc2 done interrupt clear
         */
        uint32_t saradc_apb_saradc2_done_int_clr:1;
        /** saradc_apb_saradc1_done_int_clr : WT; bitpos: [31]; default: 0;
         *  saradc1 done interrupt clear
         */
        uint32_t saradc_apb_saradc1_done_int_clr:1;
    };
    uint32_t val;
} apb_saradc_int_clr_reg_t;

/** Type of saradc_dma_conf register
 *  digital saradc configure register
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

/** Type of saradc_clkm_conf register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_clkm_div_num : R/W; bitpos: [7:0]; default: 4;
         *  Integral I2S clock divider value
         */
        uint32_t saradc_clkm_div_num:8;
        /** saradc_clkm_div_b : R/W; bitpos: [13:8]; default: 0;
         *  Fractional clock divider numerator value
         */
        uint32_t saradc_clkm_div_b:6;
        /** saradc_clkm_div_a : R/W; bitpos: [19:14]; default: 0;
         *  Fractional clock divider denominator value
         */
        uint32_t saradc_clkm_div_a:6;
        /** saradc_clk_en : R/W; bitpos: [20]; default: 0;
         *  reg clk en
         */
        uint32_t saradc_clk_en:1;
        /** saradc_clk_sel : R/W; bitpos: [22:21]; default: 0;
         *  Set this bit to enable clk_apll
         */
        uint32_t saradc_clk_sel:2;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} apb_saradc_clkm_conf_reg_t;

/** Type of saradc_apb_tsens_ctrl register
 *  digital tsens configure register
 */
typedef union {
    struct {
        /** saradc_tsens_out : RO; bitpos: [7:0]; default: 128;
         *  temperature sensor data out
         */
        uint32_t saradc_tsens_out:8;
        uint32_t reserved_8:5;
        /** saradc_tsens_in_inv : R/W; bitpos: [13]; default: 0;
         *  invert temperature sensor data
         */
        uint32_t saradc_tsens_in_inv:1;
        /** saradc_tsens_clk_div : R/W; bitpos: [21:14]; default: 6;
         *  temperature sensor clock divider
         */
        uint32_t saradc_tsens_clk_div:8;
        /** saradc_tsens_pu : R/W; bitpos: [22]; default: 0;
         *  temperature sensor power up
         */
        uint32_t saradc_tsens_pu:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} apb_saradc_apb_tsens_ctrl_reg_t;

/** Type of saradc_tsens_ctrl2 register
 *  digital tsens configure register
 */
typedef union {
    struct {
        /** saradc_tsens_xpd_wait : R/W; bitpos: [11:0]; default: 2;
         *  the time that power up tsens need wait
         */
        uint32_t saradc_tsens_xpd_wait:12;
        /** saradc_tsens_xpd_force : R/W; bitpos: [13:12]; default: 0;
         *  force power up tsens
         */
        uint32_t saradc_tsens_xpd_force:2;
        /** saradc_tsens_clk_inv : R/W; bitpos: [14]; default: 1;
         *  inv tsens clk
         */
        uint32_t saradc_tsens_clk_inv:1;
        /** saradc_tsens_clk_sel : R/W; bitpos: [15]; default: 0;
         *  tsens clk select
         */
        uint32_t saradc_tsens_clk_sel:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} apb_saradc_tsens_ctrl2_reg_t;

/** Type of saradc_cali register
 *  digital saradc configure register
 */
typedef union {
    struct {
        /** saradc_apb_saradc_cali_cfg : R/W; bitpos: [16:0]; default: 32768;
         *  saradc cali factor
         */
        uint32_t saradc_apb_saradc_cali_cfg:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} apb_saradc_cali_reg_t;

/** Type of tsens_wake register
 *  digital tsens configure register
 */
typedef union {
    struct {
        /** saradc_wakeup_th_low : R/W; bitpos: [7:0]; default: 0;
         *  reg_wakeup_th_low
         */
        uint32_t saradc_wakeup_th_low:8;
        /** saradc_wakeup_th_high : R/W; bitpos: [15:8]; default: 255;
         *  reg_wakeup_th_high
         */
        uint32_t saradc_wakeup_th_high:8;
        /** saradc_wakeup_over_upper_th : RO; bitpos: [16]; default: 0;
         *  reg_wakeup_over_upper_th
         */
        uint32_t saradc_wakeup_over_upper_th:1;
        /** saradc_wakeup_mode : R/W; bitpos: [17]; default: 0;
         *  reg_wakeup_mode
         */
        uint32_t saradc_wakeup_mode:1;
        /** saradc_wakeup_en : R/W; bitpos: [18]; default: 0;
         *  reg_wakeup_en
         */
        uint32_t saradc_wakeup_en:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} apb_tsens_wake_reg_t;

/** Type of tsens_sample register
 *  digital tsens configure register
 */
typedef union {
    struct {
        /** saradc_tsens_sample_rate : R/W; bitpos: [15:0]; default: 20;
         *  HW sample rate
         */
        uint32_t saradc_tsens_sample_rate:16;
        /** saradc_tsens_sample_en : R/W; bitpos: [16]; default: 0;
         *  HW sample en
         */
        uint32_t saradc_tsens_sample_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} apb_tsens_sample_reg_t;

/** Type of saradc_ctrl_date register
 *  version
 */
typedef union {
    struct {
        /** saradc_date : R/W; bitpos: [31:0]; default: 35676736;
         *  version
         */
        uint32_t saradc_date:32;
    };
    uint32_t val;
} apb_saradc_ctrl_date_reg_t;


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
    volatile apb_saradc_arb_ctrl_reg_t saradc_arb_ctrl;
    volatile apb_saradc_filter_ctrl0_reg_t saradc_filter_ctrl0;
    volatile apb_saradc_sar1data_status_reg_t saradc_sar1data_status;
    volatile apb_saradc_sar2data_status_reg_t saradc_sar2data_status;
    volatile apb_saradc_thres0_ctrl_reg_t saradc_thres0_ctrl;
    volatile apb_saradc_thres1_ctrl_reg_t saradc_thres1_ctrl;
    volatile apb_saradc_thres_ctrl_reg_t saradc_thres_ctrl;
    volatile apb_saradc_int_ena_reg_t saradc_int_ena;
    volatile apb_saradc_int_raw_reg_t saradc_int_raw;
    volatile apb_saradc_int_st_reg_t saradc_int_st;
    volatile apb_saradc_int_clr_reg_t saradc_int_clr;
    volatile apb_saradc_dma_conf_reg_t saradc_dma_conf;
    volatile apb_saradc_clkm_conf_reg_t saradc_clkm_conf;
    volatile apb_saradc_apb_tsens_ctrl_reg_t saradc_apb_tsens_ctrl;
    volatile apb_saradc_tsens_ctrl2_reg_t saradc_tsens_ctrl2;
    volatile apb_saradc_cali_reg_t saradc_cali;
    volatile apb_tsens_wake_reg_t tsens_wake;
    volatile apb_tsens_sample_reg_t tsens_sample;
    uint32_t reserved_06c[228];
    volatile apb_saradc_ctrl_date_reg_t saradc_ctrl_date;
} apb_saradc_dev_t;

extern apb_saradc_dev_t APB_SARADC;

#ifndef __cplusplus
_Static_assert(sizeof(apb_saradc_dev_t) == 0x400, "Invalid size of apb_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
