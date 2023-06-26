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
/** Type of lp_cali_timer register
 *  need_des
 */
typedef union {
    struct {
        /** timer_target : R/W; bitpos: [29:0]; default: 4095;
         *  need_des
         */
        uint32_t timer_target:30;
        /** timer_stop : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t timer_stop:1;
        /** timer_start : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_start:1;
    };
    uint32_t val;
} rtclockcali_lp_cali_timer_reg_t;

/** Type of dfreq_high_limit_slow register
 *  RTC slow clock dfreq high limit.
 */
typedef union {
    struct {
        /** coarse_limit_diff_slow : R/W; bitpos: [7:0]; default: 16;
         *  When rtc_cali_value upper/lower than reg_high/low_limit +/-
         *  reg_coarse_limit_diff,the step of dfreq,will use reg_coarse_step.
         */
        uint32_t coarse_limit_diff_slow:8;
        /** high_limit_slow : R/W; bitpos: [31:8]; default: 267;
         *  when rtc_cali_value upper than reg_high_limit,frequency of osc will  increase .
         */
        uint32_t high_limit_slow:24;
    };
    uint32_t val;
} rtclockcali_dfreq_high_limit_slow_reg_t;

/** Type of dfreq_low_limit_slow register
 *  RTC slow clock dfreq low limit.
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** low_limit_slow : R/W; bitpos: [31:8]; default: 266;
         *  when rtc_cali_value lower than reg_low_limit,frequency of osc will  decrease .
         */
        uint32_t low_limit_slow:24;
    };
    uint32_t val;
} rtclockcali_dfreq_low_limit_slow_reg_t;

/** Type of dfreq_high_limit_fast register
 *  RTC fast clock dfreq high limit.
 */
typedef union {
    struct {
        /** coarse_limit_diff_fast : R/W; bitpos: [7:0]; default: 16;
         *  When rtc_cali_value upper/lower than reg_high/low_limit +/-
         *  reg_coarse_limit_diff,the step of dfreq,will use reg_coarse_step.
         */
        uint32_t coarse_limit_diff_fast:8;
        /** high_limit_fast : R/W; bitpos: [31:8]; default: 267;
         *  when rtc_cali_value upper than reg_high_limit,frequency of osc will  increase .
         */
        uint32_t high_limit_fast:24;
    };
    uint32_t val;
} rtclockcali_dfreq_high_limit_fast_reg_t;

/** Type of dfreq_low_limit_fast register
 *  RTC fast clock dfreq low limit.
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** low_limit_fast : R/W; bitpos: [31:8]; default: 266;
         *  when rtc_cali_value lower than reg_low_limit,frequency of osc will  decrease .
         */
        uint32_t low_limit_fast:24;
    };
    uint32_t val;
} rtclockcali_dfreq_low_limit_fast_reg_t;

/** Type of dfreq_conf2 register
 *  RTC DFREQ CONF2
 */
typedef union {
    struct {
        /** dreq_update : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t dreq_update:1;
        uint32_t reserved_1:1;
        /** dreq_init_32k : WT; bitpos: [2]; default: 0;
         *  Initialize the vaule of 32K OSC dfreq setting.
         */
        uint32_t dreq_init_32k:1;
        /** dreq_init_fosc : WT; bitpos: [3]; default: 0;
         *  Initialize the vaule of FOSC  dfreq setting.
         */
        uint32_t dreq_init_fosc:1;
        /** dreq_init_sosc : WT; bitpos: [4]; default: 0;
         *  Initialize the vaule of SOSC  dfreq setting.
         */
        uint32_t dreq_init_sosc:1;
        /** rc32k_dfreq_sel : R/W; bitpos: [5]; default: 0;
         *  1:Frequency of 32k controlled by calibration module.0:Frequency of 32k controlled
         *  by register from system-register bank
         */
        uint32_t rc32k_dfreq_sel:1;
        /** fosc_dfreq_sel : R/W; bitpos: [6]; default: 0;
         *  1:Frequency of FOSC controlled by calibration module.0:Frequency of FOSC controlled
         *  by register from system-register bank
         */
        uint32_t fosc_dfreq_sel:1;
        /** sosc_dfreq_sel : R/W; bitpos: [7]; default: 0;
         *  1:Frequency of SOSC controlled by calibration module.0:Frequency of SOSC controlled
         *  by register from system-register bank
         */
        uint32_t sosc_dfreq_sel:1;
        /** fine_step : R/W; bitpos: [15:8]; default: 1;
         *  Frequncy fine step.
         */
        uint32_t fine_step:8;
        /** coarse_step_fast : R/W; bitpos: [23:16]; default: 8;
         *  Frequncy coarse step,use to decrease calibration time.
         */
        uint32_t coarse_step_fast:8;
        /** coarse_step_slow : R/W; bitpos: [31:24]; default: 8;
         *  Frequncy coarse step,use to decrease calibration time.
         */
        uint32_t coarse_step_slow:8;
    };
    uint32_t val;
} rtclockcali_dfreq_conf2_reg_t;

/** Type of cali_en register
 *  Configure register.
 */
typedef union {
    struct {
        /** cali_en_32k : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t cali_en_32k:1;
        /** cali_en_fosc : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t cali_en_fosc:1;
        /** cali_en_sosc : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t cali_en_sosc:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} rtclockcali_cali_en_reg_t;

/** Type of dfreq_value register
 *  Configure register.
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** dreq_32k : RO; bitpos: [11:2]; default: 172;
         *  The value of dfreq num of 32k.
         */
        uint32_t dreq_32k:10;
        /** dreq_fosc : RO; bitpos: [21:12]; default: 172;
         *  The value of dfreq num of FOSC.
         */
        uint32_t dreq_fosc:10;
        /** dreq_sosc : RO; bitpos: [31:22]; default: 172;
         *  The value of dfreq num of SOSC.
         */
        uint32_t dreq_sosc:10;
    };
    uint32_t val;
} rtclockcali_dfreq_value_reg_t;

/** Type of bypass register
 *  Configure register.
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** hp_sleep_autocali : R/W; bitpos: [30]; default: 0;
         *  1:Chip begin to calibrating,when into hp_sleep.0:Disable this function.
         */
        uint32_t hp_sleep_autocali:1;
        /** lp_sleep_autocali : R/W; bitpos: [31]; default: 0;
         *  1:Chip begin to calibrating,when into lp_sleep.0:Disable this function.
         */
        uint32_t lp_sleep_autocali:1;
    };
    uint32_t val;
} rtclockcali_bypass_reg_t;

/** Type of int_raw register
 *  Configure register.
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** xtal_timeout_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  Indicate the xtal timeout  once happend .
         */
        uint32_t xtal_timeout_int_raw:1;
        /** cali_timeout_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  Indicate the calibration timeout  once happend .
         */
        uint32_t cali_timeout_int_raw:1;
        /** cali_done_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  Indicate the finish of once calibration .
         */
        uint32_t cali_done_int_raw:1;
    };
    uint32_t val;
} rtclockcali_int_raw_reg_t;

/** Type of int_st register
 *  Interrupt state register.
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** xtal_timeout_int_st : RO; bitpos: [29]; default: 0;
         *  Interrupt state register.
         */
        uint32_t xtal_timeout_int_st:1;
        /** cali_timeout_int_st : RO; bitpos: [30]; default: 0;
         *  Interrupt state register.
         */
        uint32_t cali_timeout_int_st:1;
        /** cali_done_int_st : RO; bitpos: [31]; default: 0;
         *  Interrupt state register.
         */
        uint32_t cali_done_int_st:1;
    };
    uint32_t val;
} rtclockcali_int_st_reg_t;

/** Type of int_ena register
 *  Configure register.
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** xtal_timeout_int_ena : R/W; bitpos: [29]; default: 0;
         *  Interrupt enable signal.
         */
        uint32_t xtal_timeout_int_ena:1;
        /** cali_timeout_int_ena : R/W; bitpos: [30]; default: 0;
         *  Interrupt enable signal.
         */
        uint32_t cali_timeout_int_ena:1;
        /** cali_done_int_ena : R/W; bitpos: [31]; default: 0;
         *  Interrupt enable signal.
         */
        uint32_t cali_done_int_ena:1;
    };
    uint32_t val;
} rtclockcali_int_ena_reg_t;

/** Type of int_clr register
 *  Configure register.
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** xtal_timeout_int_clr : WT; bitpos: [29]; default: 0;
         *  interrupt clear signal.
         */
        uint32_t xtal_timeout_int_clr:1;
        /** cali_timeout_int_clr : WT; bitpos: [30]; default: 0;
         *  interrupt clear signal.
         */
        uint32_t cali_timeout_int_clr:1;
        /** cali_done_int_clr : WT; bitpos: [31]; default: 0;
         *  interrupt clear signal.
         */
        uint32_t cali_done_int_clr:1;
    };
    uint32_t val;
} rtclockcali_int_clr_reg_t;

/** Type of timeout register
 *  Configure register.
 */
typedef union {
    struct {
        /** timeout_target : R/W; bitpos: [29:0]; default: 0;
         *  use to setting max calibration time .
         */
        uint32_t timeout_target:30;
        uint32_t reserved_30:1;
        /** timeout_en : R/W; bitpos: [31]; default: 0;
         *  use to enable calibration time-out function ,the calibration force stopping,when
         *  timeout.
         */
        uint32_t timeout_en:1;
    };
    uint32_t val;
} rtclockcali_timeout_reg_t;

/** Type of xtal_timeout register
 *  Configure register.
 */
typedef union {
    struct {
        uint32_t reserved_0:14;
        /** xtal_timeout_cnt_target : R/W; bitpos: [29:14]; default: 65535;
         *  use to setting max xtal monitor time .
         */
        uint32_t xtal_timeout_cnt_target:16;
        /** xtal_timeout_cnt_stop : WT; bitpos: [30]; default: 0;
         *  use to stop XTAL time-out function ,timeout happened when xtal invalid.
         */
        uint32_t xtal_timeout_cnt_stop:1;
        /** xtal_timeout_cnt_start : WT; bitpos: [31]; default: 0;
         *  use to start XTAL time-out function ,timeout happened when xtal invalid.
         */
        uint32_t xtal_timeout_cnt_start:1;
    };
    uint32_t val;
} rtclockcali_xtal_timeout_reg_t;

/** Type of date register
 *  Configure register.
 */
typedef union {
    struct {
        /** rtclockcali_date : R/W; bitpos: [30:0]; default: 35660384;
         *  need_des
         */
        uint32_t rtclockcali_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} rtclockcali_date_reg_t;


/** Group: RTC CALI Control and configuration registers */
/** Type of rtccalicfg_slow register
 *  RTC calibration configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** rtc_cali_start_cycling_slow : R/W; bitpos: [12]; default: 1;
         *  0: one-shot frequency calculation,1: periodic frequency calculation,
         */
        uint32_t rtc_cali_start_cycling_slow:1;
        /** rtc_cali_clk_sel_slow : R/W; bitpos: [14:13]; default: 0;
         *  0:rtc slow clock. 1:clk_8m, 2:xtal_32k.
         */
        uint32_t rtc_cali_clk_sel_slow:2;
        /** rtc_cali_rdy_slow : RO; bitpos: [15]; default: 0;
         *  indicate one-shot frequency calculation is done.
         */
        uint32_t rtc_cali_rdy_slow:1;
        /** rtc_cali_max_slow : R/W; bitpos: [30:16]; default: 1;
         *  Configure the time to calculate RTC slow clock's frequency.
         */
        uint32_t rtc_cali_max_slow:15;
        /** rtc_cali_start_slow : R/W; bitpos: [31]; default: 0;
         *  Set this bit to start one-shot frequency calculation.
         */
        uint32_t rtc_cali_start_slow:1;
    };
    uint32_t val;
} rtclockcali_rtccalicfg_slow_reg_t;

/** Type of rtccalicfg_fast register
 *  RTC calibration configure register
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** fosc_div_num : R/W; bitpos: [11:4]; default: 0;
         *  fosc clock divider number
         */
        uint32_t fosc_div_num:8;
        /** rtc_cali_start_cycling_fast : R/W; bitpos: [12]; default: 1;
         *  0: one-shot frequency calculation,1: periodic frequency calculation,
         */
        uint32_t rtc_cali_start_cycling_fast:1;
        /** rtc_cali_clk_sel_fast : R/W; bitpos: [14:13]; default: 0;
         *  0:rtc slow clock. 1:clk_8m, 2:xtal_32k.
         */
        uint32_t rtc_cali_clk_sel_fast:2;
        /** rtc_cali_rdy_fast : RO; bitpos: [15]; default: 0;
         *  indicate one-shot frequency calculation is done.
         */
        uint32_t rtc_cali_rdy_fast:1;
        /** rtc_cali_max_fast : R/W; bitpos: [30:16]; default: 1;
         *  Configure the time to calculate RTC slow clock's frequency.
         */
        uint32_t rtc_cali_max_fast:15;
        /** rtc_cali_start_fast : R/W; bitpos: [31]; default: 0;
         *  Set this bit to start one-shot frequency calculation.
         */
        uint32_t rtc_cali_start_fast:1;
    };
    uint32_t val;
} rtclockcali_rtccalicfg_fast_reg_t;

/** Type of rtccalicfg1_slow register
 *  RTC calibration configure1 register
 */
typedef union {
    struct {
        /** rtc_cali_cycling_data_vld_slow : RO; bitpos: [0]; default: 0;
         *  indicate periodic frequency calculation is done.
         */
        uint32_t rtc_cali_cycling_data_vld_slow:1;
        uint32_t reserved_1:6;
        /** rtc_cali_value_slow : RO; bitpos: [31:7]; default: 0;
         *  When one-shot or periodic frequency calculation is done, read this value to
         *  calculate RTC slow clock's frequency.
         */
        uint32_t rtc_cali_value_slow:25;
    };
    uint32_t val;
} rtclockcali_rtccalicfg1_slow_reg_t;

/** Type of rtccalicfg1_fast register
 *  RTC calibration configure1 register
 */
typedef union {
    struct {
        /** rtc_cali_cycling_data_vld_fast : RO; bitpos: [0]; default: 0;
         *  indicate periodic frequency calculation is done.
         */
        uint32_t rtc_cali_cycling_data_vld_fast:1;
        uint32_t reserved_1:6;
        /** rtc_cali_value_fast : RO; bitpos: [31:7]; default: 0;
         *  When one-shot or periodic frequency calculation is done, read this value to
         *  calculate RTC slow clock's frequency.
         */
        uint32_t rtc_cali_value_fast:25;
    };
    uint32_t val;
} rtclockcali_rtccalicfg1_fast_reg_t;

/** Type of rtccalicfg2 register
 *  Timer group calibration register
 */
typedef union {
    struct {
        /** rtc_cali_timeout : RO; bitpos: [0]; default: 0;
         *  RTC calibration timeout indicator
         */
        uint32_t rtc_cali_timeout:1;
        uint32_t reserved_1:2;
        /** rtc_cali_timeout_rst_cnt : R/W; bitpos: [6:3]; default: 3;
         *  Cycles that release calibration timeout reset
         */
        uint32_t rtc_cali_timeout_rst_cnt:4;
        /** rtc_cali_timeout_thres : R/W; bitpos: [31:7]; default: 33554431;
         *  Threshold value for the RTC calibration timer. If the calibration timer's value
         *  exceeds this threshold, a timeout is triggered.
         */
        uint32_t rtc_cali_timeout_thres:25;
    };
    uint32_t val;
} rtclockcali_rtccalicfg2_reg_t;


typedef struct {
    volatile rtclockcali_lp_cali_timer_reg_t lp_cali_timer;
    volatile rtclockcali_rtccalicfg_slow_reg_t rtccalicfg_slow;
    volatile rtclockcali_rtccalicfg_fast_reg_t rtccalicfg_fast;
    volatile rtclockcali_rtccalicfg1_slow_reg_t rtccalicfg1_slow;
    volatile rtclockcali_rtccalicfg1_fast_reg_t rtccalicfg1_fast;
    volatile rtclockcali_rtccalicfg2_reg_t rtccalicfg2;
    volatile rtclockcali_dfreq_high_limit_slow_reg_t dfreq_high_limit_slow;
    volatile rtclockcali_dfreq_low_limit_slow_reg_t dfreq_low_limit_slow;
    volatile rtclockcali_dfreq_high_limit_fast_reg_t dfreq_high_limit_fast;
    volatile rtclockcali_dfreq_low_limit_fast_reg_t dfreq_low_limit_fast;
    volatile rtclockcali_dfreq_conf2_reg_t dfreq_conf2;
    volatile rtclockcali_cali_en_reg_t cali_en;
    volatile rtclockcali_dfreq_value_reg_t dfreq_value;
    volatile rtclockcali_bypass_reg_t bypass;
    volatile rtclockcali_int_raw_reg_t int_raw;
    volatile rtclockcali_int_st_reg_t int_st;
    volatile rtclockcali_int_ena_reg_t int_ena;
    volatile rtclockcali_int_clr_reg_t int_clr;
    volatile rtclockcali_timeout_reg_t timeout;
    volatile rtclockcali_xtal_timeout_reg_t xtal_timeout;
    uint32_t reserved_050[235];
    volatile rtclockcali_date_reg_t date;
} rtclockcali_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(rtclockcali_dev_t) == 0x400, "Invalid size of rtclockcali_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
