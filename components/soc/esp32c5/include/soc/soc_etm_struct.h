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

/** Group: Status register */
/** Type of ch_ena_ad0 register
 *  Channel enable status register
 */
typedef union {
    struct {
        /** ch_enabled0 : R/WTC/WTS; bitpos: [0]; default: 0;
         *  Represents ch0 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled0:1;
        /** ch_enabled1 : R/WTC/WTS; bitpos: [1]; default: 0;
         *  Represents ch1 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled1:1;
        /** ch_enabled2 : R/WTC/WTS; bitpos: [2]; default: 0;
         *  Represents ch2 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled2:1;
        /** ch_enabled3 : R/WTC/WTS; bitpos: [3]; default: 0;
         *  Represents ch3 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled3:1;
        /** ch_enabled4 : R/WTC/WTS; bitpos: [4]; default: 0;
         *  Represents ch4 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled4:1;
        /** ch_enabled5 : R/WTC/WTS; bitpos: [5]; default: 0;
         *  Represents ch5 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled5:1;
        /** ch_enabled6 : R/WTC/WTS; bitpos: [6]; default: 0;
         *  Represents ch6 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled6:1;
        /** ch_enabled7 : R/WTC/WTS; bitpos: [7]; default: 0;
         *  Represents ch7 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled7:1;
        /** ch_enabled8 : R/WTC/WTS; bitpos: [8]; default: 0;
         *  Represents ch8 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled8:1;
        /** ch_enabled9 : R/WTC/WTS; bitpos: [9]; default: 0;
         *  Represents ch9 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled9:1;
        /** ch_enabled10 : R/WTC/WTS; bitpos: [10]; default: 0;
         *  Represents ch10 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled10:1;
        /** ch_enabled11 : R/WTC/WTS; bitpos: [11]; default: 0;
         *  Represents ch11 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled11:1;
        /** ch_enabled12 : R/WTC/WTS; bitpos: [12]; default: 0;
         *  Represents ch12 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled12:1;
        /** ch_enabled13 : R/WTC/WTS; bitpos: [13]; default: 0;
         *  Represents ch13 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled13:1;
        /** ch_enabled14 : R/WTC/WTS; bitpos: [14]; default: 0;
         *  Represents ch14 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled14:1;
        /** ch_enabled15 : R/WTC/WTS; bitpos: [15]; default: 0;
         *  Represents ch15 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled15:1;
        /** ch_enabled16 : R/WTC/WTS; bitpos: [16]; default: 0;
         *  Represents ch16 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled16:1;
        /** ch_enabled17 : R/WTC/WTS; bitpos: [17]; default: 0;
         *  Represents ch17 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled17:1;
        /** ch_enabled18 : R/WTC/WTS; bitpos: [18]; default: 0;
         *  Represents ch18 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled18:1;
        /** ch_enabled19 : R/WTC/WTS; bitpos: [19]; default: 0;
         *  Represents ch19 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled19:1;
        /** ch_enabled20 : R/WTC/WTS; bitpos: [20]; default: 0;
         *  Represents ch20 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled20:1;
        /** ch_enabled21 : R/WTC/WTS; bitpos: [21]; default: 0;
         *  Represents ch21 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled21:1;
        /** ch_enabled22 : R/WTC/WTS; bitpos: [22]; default: 0;
         *  Represents ch22 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled22:1;
        /** ch_enabled23 : R/WTC/WTS; bitpos: [23]; default: 0;
         *  Represents ch23 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled23:1;
        /** ch_enabled24 : R/WTC/WTS; bitpos: [24]; default: 0;
         *  Represents ch24 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled24:1;
        /** ch_enabled25 : R/WTC/WTS; bitpos: [25]; default: 0;
         *  Represents ch25 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled25:1;
        /** ch_enabled26 : R/WTC/WTS; bitpos: [26]; default: 0;
         *  Represents ch26 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled26:1;
        /** ch_enabled27 : R/WTC/WTS; bitpos: [27]; default: 0;
         *  Represents ch27 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled27:1;
        /** ch_enabled28 : R/WTC/WTS; bitpos: [28]; default: 0;
         *  Represents ch28 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled28:1;
        /** ch_enabled29 : R/WTC/WTS; bitpos: [29]; default: 0;
         *  Represents ch29 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled29:1;
        /** ch_enabled30 : R/WTC/WTS; bitpos: [30]; default: 0;
         *  Represents ch30 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled30:1;
        /** ch_enabled31 : R/WTC/WTS; bitpos: [31]; default: 0;
         *  Represents ch31 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_reg_t;

/** Type of ch_ena_ad1 register
 *  Channel enable status register
 */
typedef union {
    struct {
        /** ch_enabled32 : R/WTC/WTS; bitpos: [0]; default: 0;
         *  Represents ch32 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled32:1;
        /** ch_enabled33 : R/WTC/WTS; bitpos: [1]; default: 0;
         *  Represents ch33 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled33:1;
        /** ch_enabled34 : R/WTC/WTS; bitpos: [2]; default: 0;
         *  Represents ch34 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled34:1;
        /** ch_enabled35 : R/WTC/WTS; bitpos: [3]; default: 0;
         *  Represents ch35 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled35:1;
        /** ch_enabled36 : R/WTC/WTS; bitpos: [4]; default: 0;
         *  Represents ch36 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled36:1;
        /** ch_enabled37 : R/WTC/WTS; bitpos: [5]; default: 0;
         *  Represents ch37 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled37:1;
        /** ch_enabled38 : R/WTC/WTS; bitpos: [6]; default: 0;
         *  Represents ch38 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled38:1;
        /** ch_enabled39 : R/WTC/WTS; bitpos: [7]; default: 0;
         *  Represents ch39 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled39:1;
        /** ch_enabled40 : R/WTC/WTS; bitpos: [8]; default: 0;
         *  Represents ch40 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled40:1;
        /** ch_enabled41 : R/WTC/WTS; bitpos: [9]; default: 0;
         *  Represents ch41 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled41:1;
        /** ch_enabled42 : R/WTC/WTS; bitpos: [10]; default: 0;
         *  Represents ch42 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled42:1;
        /** ch_enabled43 : R/WTC/WTS; bitpos: [11]; default: 0;
         *  Represents ch43 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled43:1;
        /** ch_enabled44 : R/WTC/WTS; bitpos: [12]; default: 0;
         *  Represents ch44 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled44:1;
        /** ch_enabled45 : R/WTC/WTS; bitpos: [13]; default: 0;
         *  Represents ch45 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled45:1;
        /** ch_enabled46 : R/WTC/WTS; bitpos: [14]; default: 0;
         *  Represents ch46 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled46:1;
        /** ch_enabled47 : R/WTC/WTS; bitpos: [15]; default: 0;
         *  Represents ch47 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled47:1;
        /** ch_enabled48 : R/WTC/WTS; bitpos: [16]; default: 0;
         *  Represents ch48 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled48:1;
        /** ch_enabled49 : R/WTC/WTS; bitpos: [17]; default: 0;
         *  Represents ch49 enable status.\\0: Disable\\1: Enable
         */
        uint32_t ch_enabled49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_reg_t;

/** Type of evt_st0 register
 *  Events trigger status register
 */
typedef union {
    struct {
        /** gpio_evt_ch0_rise_edge_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents GPIO_evt_ch0_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch0_rise_edge_st:1;
        /** gpio_evt_ch1_rise_edge_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents GPIO_evt_ch1_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch1_rise_edge_st:1;
        /** gpio_evt_ch2_rise_edge_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents GPIO_evt_ch2_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch2_rise_edge_st:1;
        /** gpio_evt_ch3_rise_edge_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents GPIO_evt_ch3_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch3_rise_edge_st:1;
        /** gpio_evt_ch4_rise_edge_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents GPIO_evt_ch4_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch4_rise_edge_st:1;
        /** gpio_evt_ch5_rise_edge_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents GPIO_evt_ch5_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch5_rise_edge_st:1;
        /** gpio_evt_ch6_rise_edge_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents GPIO_evt_ch6_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch6_rise_edge_st:1;
        /** gpio_evt_ch7_rise_edge_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents GPIO_evt_ch7_rise_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch7_rise_edge_st:1;
        /** gpio_evt_ch0_fall_edge_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents GPIO_evt_ch0_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch0_fall_edge_st:1;
        /** gpio_evt_ch1_fall_edge_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents GPIO_evt_ch1_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch1_fall_edge_st:1;
        /** gpio_evt_ch2_fall_edge_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents GPIO_evt_ch2_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch2_fall_edge_st:1;
        /** gpio_evt_ch3_fall_edge_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents GPIO_evt_ch3_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch3_fall_edge_st:1;
        /** gpio_evt_ch4_fall_edge_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents GPIO_evt_ch4_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch4_fall_edge_st:1;
        /** gpio_evt_ch5_fall_edge_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents GPIO_evt_ch5_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch5_fall_edge_st:1;
        /** gpio_evt_ch6_fall_edge_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents GPIO_evt_ch6_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch6_fall_edge_st:1;
        /** gpio_evt_ch7_fall_edge_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents GPIO_evt_ch7_fall_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch7_fall_edge_st:1;
        /** gpio_evt_ch0_any_edge_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents GPIO_evt_ch0_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch0_any_edge_st:1;
        /** gpio_evt_ch1_any_edge_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents GPIO_evt_ch1_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch1_any_edge_st:1;
        /** gpio_evt_ch2_any_edge_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents GPIO_evt_ch2_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch2_any_edge_st:1;
        /** gpio_evt_ch3_any_edge_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents GPIO_evt_ch3_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch3_any_edge_st:1;
        /** gpio_evt_ch4_any_edge_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents GPIO_evt_ch4_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch4_any_edge_st:1;
        /** gpio_evt_ch5_any_edge_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents GPIO_evt_ch5_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch5_any_edge_st:1;
        /** gpio_evt_ch6_any_edge_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents GPIO_evt_ch6_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch6_any_edge_st:1;
        /** gpio_evt_ch7_any_edge_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents GPIO_evt_ch7_any_edge trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_ch7_any_edge_st:1;
        /** gpio_evt_zero_det_pos0_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents GPIO_evt_zero_det_pos0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_zero_det_pos0_st:1;
        /** gpio_evt_zero_det_neg0_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents GPIO_evt_zero_det_neg0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_evt_zero_det_neg0_st:1;
        /** ledc_evt_duty_chng_end_ch0_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents LEDC_evt_duty_chng_end_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch0_st:1;
        /** ledc_evt_duty_chng_end_ch1_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents LEDC_evt_duty_chng_end_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch1_st:1;
        /** ledc_evt_duty_chng_end_ch2_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents LEDC_evt_duty_chng_end_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch2_st:1;
        /** ledc_evt_duty_chng_end_ch3_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents LEDC_evt_duty_chng_end_ch3 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch3_st:1;
        /** ledc_evt_duty_chng_end_ch4_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents LEDC_evt_duty_chng_end_ch4 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch4_st:1;
        /** ledc_evt_duty_chng_end_ch5_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents LEDC_evt_duty_chng_end_ch5 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch5_st:1;
    };
    uint32_t val;
} soc_etm_evt_st0_reg_t;

/** Type of evt_st1 register
 *  Events trigger status register
 */
typedef union {
    struct {
        /** ledc_evt_ovf_cnt_pls_ch0_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents LEDC_evt_ovf_cnt_pls_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch0_st:1;
        /** ledc_evt_ovf_cnt_pls_ch1_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents LEDC_evt_ovf_cnt_pls_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch1_st:1;
        /** ledc_evt_ovf_cnt_pls_ch2_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents LEDC_evt_ovf_cnt_pls_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch2_st:1;
        /** ledc_evt_ovf_cnt_pls_ch3_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents LEDC_evt_ovf_cnt_pls_ch3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch3_st:1;
        /** ledc_evt_ovf_cnt_pls_ch4_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents LEDC_evt_ovf_cnt_pls_ch4 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch4_st:1;
        /** ledc_evt_ovf_cnt_pls_ch5_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents LEDC_evt_ovf_cnt_pls_ch5 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch5_st:1;
        /** ledc_evt_time_ovf_timer0_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents LEDC_evt_time_ovf_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer0_st:1;
        /** ledc_evt_time_ovf_timer1_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents LEDC_evt_time_ovf_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer1_st:1;
        /** ledc_evt_time_ovf_timer2_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents LEDC_evt_time_ovf_timer2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer2_st:1;
        /** ledc_evt_time_ovf_timer3_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents LEDC_evt_time_ovf_timer3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer3_st:1;
        /** ledc_evt_timer0_cmp_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents LEDC_evt_timer0_cmp trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_timer0_cmp_st:1;
        /** ledc_evt_timer1_cmp_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents LEDC_evt_timer1_cmp trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_timer1_cmp_st:1;
        /** ledc_evt_timer2_cmp_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents LEDC_evt_timer2_cmp trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_timer2_cmp_st:1;
        /** ledc_evt_timer3_cmp_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents LEDC_evt_timer3_cmp trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_evt_timer3_cmp_st:1;
        /** tg0_evt_cnt_cmp_timer0_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents TG0_evt_cnt_cmp_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_evt_cnt_cmp_timer0_st:1;
        /** tg0_evt_cnt_cmp_timer1_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents TG0_evt_cnt_cmp_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_evt_cnt_cmp_timer1_st:1;
        /** tg1_evt_cnt_cmp_timer0_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents TG1_evt_cnt_cmp_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_evt_cnt_cmp_timer0_st:1;
        /** tg1_evt_cnt_cmp_timer1_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents TG1_evt_cnt_cmp_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_evt_cnt_cmp_timer1_st:1;
        /** systimer_evt_cnt_cmp0_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents SYSTIMER_evt_cnt_cmp0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t systimer_evt_cnt_cmp0_st:1;
        /** systimer_evt_cnt_cmp1_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents SYSTIMER_evt_cnt_cmp1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t systimer_evt_cnt_cmp1_st:1;
        /** systimer_evt_cnt_cmp2_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents SYSTIMER_evt_cnt_cmp2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t systimer_evt_cnt_cmp2_st:1;
        /** mcpwm0_evt_timer0_stop_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents MCPWM0_evt_timer0_stop trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer0_stop_st:1;
        /** mcpwm0_evt_timer1_stop_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents MCPWM0_evt_timer1_stop trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer1_stop_st:1;
        /** mcpwm0_evt_timer2_stop_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents MCPWM0_evt_timer2_stop trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer2_stop_st:1;
        /** mcpwm0_evt_timer0_tez_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents MCPWM0_evt_timer0_tez trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer0_tez_st:1;
        /** mcpwm0_evt_timer1_tez_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents MCPWM0_evt_timer1_tez trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer1_tez_st:1;
        /** mcpwm0_evt_timer2_tez_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents MCPWM0_evt_timer2_tez trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer2_tez_st:1;
        /** mcpwm0_evt_timer0_tep_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents MCPWM0_evt_timer0_tep trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer0_tep_st:1;
        /** mcpwm0_evt_timer1_tep_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents MCPWM0_evt_timer1_tep trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer1_tep_st:1;
        /** mcpwm0_evt_timer2_tep_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents MCPWM0_evt_timer2_tep trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_timer2_tep_st:1;
        /** mcpwm0_evt_op0_tea_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents MCPWM0_evt_op0_tea trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op0_tea_st:1;
        /** mcpwm0_evt_op1_tea_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents MCPWM0_evt_op1_tea trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op1_tea_st:1;
    };
    uint32_t val;
} soc_etm_evt_st1_reg_t;

/** Type of evt_st2 register
 *  Events trigger status register
 */
typedef union {
    struct {
        /** mcpwm0_evt_op2_tea_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents MCPWM0_evt_op2_tea trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op2_tea_st:1;
        /** mcpwm0_evt_op0_teb_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents MCPWM0_evt_op0_teb trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op0_teb_st:1;
        /** mcpwm0_evt_op1_teb_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents MCPWM0_evt_op1_teb trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op1_teb_st:1;
        /** mcpwm0_evt_op2_teb_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents MCPWM0_evt_op2_teb trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op2_teb_st:1;
        /** mcpwm0_evt_f0_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents MCPWM0_evt_f0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_f0_st:1;
        /** mcpwm0_evt_f1_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents MCPWM0_evt_f1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_f1_st:1;
        /** mcpwm0_evt_f2_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents MCPWM0_evt_f2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_f2_st:1;
        /** mcpwm0_evt_f0_clr_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents MCPWM0_evt_f0_clr trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_f0_clr_st:1;
        /** mcpwm0_evt_f1_clr_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents MCPWM0_evt_f1_clr trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_f1_clr_st:1;
        /** mcpwm0_evt_f2_clr_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents MCPWM0_evt_f2_clr trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_f2_clr_st:1;
        /** mcpwm0_evt_tz0_cbc_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents MCPWM0_evt_tz0_cbc trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_tz0_cbc_st:1;
        /** mcpwm0_evt_tz1_cbc_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents MCPWM0_evt_tz1_cbc trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_tz1_cbc_st:1;
        /** mcpwm0_evt_tz2_cbc_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents MCPWM0_evt_tz2_cbc trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_tz2_cbc_st:1;
        /** mcpwm0_evt_tz0_ost_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents MCPWM0_evt_tz0_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_tz0_ost_st:1;
        /** mcpwm0_evt_tz1_ost_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents MCPWM0_evt_tz1_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_tz1_ost_st:1;
        /** mcpwm0_evt_tz2_ost_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents MCPWM0_evt_tz2_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_tz2_ost_st:1;
        /** mcpwm0_evt_cap0_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents MCPWM0_evt_cap0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_cap0_st:1;
        /** mcpwm0_evt_cap1_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents MCPWM0_evt_cap1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_cap1_st:1;
        /** mcpwm0_evt_cap2_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents MCPWM0_evt_cap2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_cap2_st:1;
        /** mcpwm0_evt_op0_tee1_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents MCPWM0_evt_op0_tee1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op0_tee1_st:1;
        /** mcpwm0_evt_op1_tee1_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents MCPWM0_evt_op1_tee1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op1_tee1_st:1;
        /** mcpwm0_evt_op2_tee1_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents MCPWM0_evt_op2_tee1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op2_tee1_st:1;
        /** mcpwm0_evt_op0_tee2_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents MCPWM0_evt_op0_tee2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op0_tee2_st:1;
        /** mcpwm0_evt_op1_tee2_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents MCPWM0_evt_op1_tee2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op1_tee2_st:1;
        /** mcpwm0_evt_op2_tee2_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents MCPWM0_evt_op2_tee2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_evt_op2_tee2_st:1;
        /** adc_evt_conv_cmplt0_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents ADC_evt_conv_cmplt0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_conv_cmplt0_st:1;
        /** adc_evt_eq_above_thresh0_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents ADC_evt_eq_above_thresh0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_eq_above_thresh0_st:1;
        /** adc_evt_eq_above_thresh1_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents ADC_evt_eq_above_thresh1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_eq_above_thresh1_st:1;
        /** adc_evt_eq_below_thresh0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents ADC_evt_eq_below_thresh0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_eq_below_thresh0_st:1;
        /** adc_evt_eq_below_thresh1_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents ADC_evt_eq_below_thresh1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_eq_below_thresh1_st:1;
        /** adc_evt_result_done0_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents ADC_evt_result_done0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_result_done0_st:1;
        /** adc_evt_stopped0_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents ADC_evt_stopped0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_stopped0_st:1;
    };
    uint32_t val;
} soc_etm_evt_st2_reg_t;

/** Type of evt_st3 register
 *  Events trigger status register
 */
typedef union {
    struct {
        /** adc_evt_started0_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents ADC_evt_started0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_evt_started0_st:1;
        /** regdma_evt_done0_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents REGDMA_evt_done0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_done0_st:1;
        /** regdma_evt_done1_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents REGDMA_evt_done1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_done1_st:1;
        /** regdma_evt_done2_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents REGDMA_evt_done2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_done2_st:1;
        /** regdma_evt_done3_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents REGDMA_evt_done3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_done3_st:1;
        /** regdma_evt_err0_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents REGDMA_evt_err0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_err0_st:1;
        /** regdma_evt_err1_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents REGDMA_evt_err1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_err1_st:1;
        /** regdma_evt_err2_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents REGDMA_evt_err2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_err2_st:1;
        /** regdma_evt_err3_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents REGDMA_evt_err3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_evt_err3_st:1;
        /** tmpsnsr_evt_over_limit_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents TMPSNSR_evt_over_limit trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tmpsnsr_evt_over_limit_st:1;
        /** i2s0_evt_rx_done_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents I2S0_evt_rx_done trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_evt_rx_done_st:1;
        /** i2s0_evt_tx_done_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents I2S0_evt_tx_done trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_evt_tx_done_st:1;
        /** i2s0_evt_x_words_received_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents I2S0_evt_x_words_received trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_evt_x_words_received_st:1;
        /** i2s0_evt_x_words_sent_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents I2S0_evt_x_words_sent trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_evt_x_words_sent_st:1;
        /** ulp_evt_err_intr_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents ULP_evt_err_intr trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ulp_evt_err_intr_st:1;
        /** ulp_evt_halt_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents ULP_evt_halt trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ulp_evt_halt_st:1;
        /** ulp_evt_start_intr_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents ULP_evt_start_intr trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ulp_evt_start_intr_st:1;
        /** rtc_evt_tick_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents RTC_evt_tick trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t rtc_evt_tick_st:1;
        /** rtc_evt_ovf_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents RTC_evt_ovf trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t rtc_evt_ovf_st:1;
        /** rtc_evt_cmp_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents RTC_evt_cmp trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t rtc_evt_cmp_st:1;
        /** gdma_evt_in_done_ch0_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents GDMA_evt_in_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_done_ch0_st:1;
        /** gdma_evt_in_done_ch1_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents GDMA_evt_in_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_done_ch1_st:1;
        /** gdma_evt_in_done_ch2_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents GDMA_evt_in_done_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_done_ch2_st:1;
        /** gdma_evt_in_suc_eof_ch0_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents GDMA_evt_in_suc_eof_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_suc_eof_ch0_st:1;
        /** gdma_evt_in_suc_eof_ch1_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents GDMA_evt_in_suc_eof_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_suc_eof_ch1_st:1;
        /** gdma_evt_in_suc_eof_ch2_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents GDMA_evt_in_suc_eof_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_suc_eof_ch2_st:1;
        /** gdma_evt_in_fifo_empty_ch0_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents GDMA_evt_in_fifo_empty_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_in_fifo_empty_ch0_st:1;
        /** gdma_evt_in_fifo_empty_ch1_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents GDMA_evt_in_fifo_empty_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_in_fifo_empty_ch1_st:1;
        /** gdma_evt_in_fifo_empty_ch2_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents GDMA_evt_in_fifo_empty_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_in_fifo_empty_ch2_st:1;
        /** gdma_evt_in_fifo_full_ch0_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents GDMA_evt_in_fifo_full_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_fifo_full_ch0_st:1;
        /** gdma_evt_in_fifo_full_ch1_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents GDMA_evt_in_fifo_full_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_fifo_full_ch1_st:1;
        /** gdma_evt_in_fifo_full_ch2_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents GDMA_evt_in_fifo_full_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_in_fifo_full_ch2_st:1;
    };
    uint32_t val;
} soc_etm_evt_st3_reg_t;

/** Type of evt_st4 register
 *  Events trigger status register
 */
typedef union {
    struct {
        /** gdma_evt_out_done_ch0_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents GDMA_evt_out_done_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_out_done_ch0_st:1;
        /** gdma_evt_out_done_ch1_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents GDMA_evt_out_done_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_out_done_ch1_st:1;
        /** gdma_evt_out_done_ch2_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents GDMA_evt_out_done_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_out_done_ch2_st:1;
        /** gdma_evt_out_eof_ch0_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents GDMA_evt_out_eof_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_out_eof_ch0_st:1;
        /** gdma_evt_out_eof_ch1_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents GDMA_evt_out_eof_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_out_eof_ch1_st:1;
        /** gdma_evt_out_eof_ch2_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents GDMA_evt_out_eof_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_evt_out_eof_ch2_st:1;
        /** gdma_evt_out_total_eof_ch0_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents GDMA_evt_out_total_eof_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_total_eof_ch0_st:1;
        /** gdma_evt_out_total_eof_ch1_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents GDMA_evt_out_total_eof_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_total_eof_ch1_st:1;
        /** gdma_evt_out_total_eof_ch2_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents GDMA_evt_out_total_eof_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_total_eof_ch2_st:1;
        /** gdma_evt_out_fifo_empty_ch0_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents GDMA_evt_out_fifo_empty_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_fifo_empty_ch0_st:1;
        /** gdma_evt_out_fifo_empty_ch1_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents GDMA_evt_out_fifo_empty_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_fifo_empty_ch1_st:1;
        /** gdma_evt_out_fifo_empty_ch2_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents GDMA_evt_out_fifo_empty_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_fifo_empty_ch2_st:1;
        /** gdma_evt_out_fifo_full_ch0_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents GDMA_evt_out_fifo_full_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_fifo_full_ch0_st:1;
        /** gdma_evt_out_fifo_full_ch1_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents GDMA_evt_out_fifo_full_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_fifo_full_ch1_st:1;
        /** gdma_evt_out_fifo_full_ch2_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents GDMA_evt_out_fifo_full_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t gdma_evt_out_fifo_full_ch2_st:1;
        /** pmu_evt_sleep_weekup_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents PMU_evt_sleep_weekup trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t pmu_evt_sleep_weekup_st:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} soc_etm_evt_st4_reg_t;

/** Type of task_st0 register
 *  Tasks trigger status register
 */
typedef union {
    struct {
        /** gpio_task_ch0_set_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents GPIO_task_ch0_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch0_set_st:1;
        /** gpio_task_ch1_set_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents GPIO_task_ch1_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch1_set_st:1;
        /** gpio_task_ch2_set_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents GPIO_task_ch2_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch2_set_st:1;
        /** gpio_task_ch3_set_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents GPIO_task_ch3_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch3_set_st:1;
        /** gpio_task_ch4_set_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents GPIO_task_ch4_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch4_set_st:1;
        /** gpio_task_ch5_set_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents GPIO_task_ch5_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch5_set_st:1;
        /** gpio_task_ch6_set_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents GPIO_task_ch6_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch6_set_st:1;
        /** gpio_task_ch7_set_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents GPIO_task_ch7_set trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch7_set_st:1;
        /** gpio_task_ch0_clear_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents GPIO_task_ch0_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch0_clear_st:1;
        /** gpio_task_ch1_clear_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents GPIO_task_ch1_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch1_clear_st:1;
        /** gpio_task_ch2_clear_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents GPIO_task_ch2_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch2_clear_st:1;
        /** gpio_task_ch3_clear_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents GPIO_task_ch3_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch3_clear_st:1;
        /** gpio_task_ch4_clear_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents GPIO_task_ch4_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch4_clear_st:1;
        /** gpio_task_ch5_clear_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents GPIO_task_ch5_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch5_clear_st:1;
        /** gpio_task_ch6_clear_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents GPIO_task_ch6_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch6_clear_st:1;
        /** gpio_task_ch7_clear_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents GPIO_task_ch7_clear trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch7_clear_st:1;
        /** gpio_task_ch0_toggle_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents GPIO_task_ch0_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch0_toggle_st:1;
        /** gpio_task_ch1_toggle_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents GPIO_task_ch1_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch1_toggle_st:1;
        /** gpio_task_ch2_toggle_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents GPIO_task_ch2_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch2_toggle_st:1;
        /** gpio_task_ch3_toggle_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents GPIO_task_ch3_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch3_toggle_st:1;
        /** gpio_task_ch4_toggle_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents GPIO_task_ch4_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch4_toggle_st:1;
        /** gpio_task_ch5_toggle_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents GPIO_task_ch5_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch5_toggle_st:1;
        /** gpio_task_ch6_toggle_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents GPIO_task_ch6_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch6_toggle_st:1;
        /** gpio_task_ch7_toggle_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents GPIO_task_ch7_toggle trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gpio_task_ch7_toggle_st:1;
        /** ledc_task_timer0_res_update_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents LEDC_task_timer0_res_update trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_timer0_res_update_st:1;
        /** ledc_task_timer1_res_update_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents LEDC_task_timer1_res_update trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_timer1_res_update_st:1;
        /** ledc_task_timer2_res_update_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents LEDC_task_timer2_res_update trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_timer2_res_update_st:1;
        /** ledc_task_timer3_res_update_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents LEDC_task_timer3_res_update trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_timer3_res_update_st:1;
        /** ledc_task_duty_scale_update_ch0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents LEDC_task_duty_scale_update_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch0_st:1;
        /** ledc_task_duty_scale_update_ch1_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents LEDC_task_duty_scale_update_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch1_st:1;
        /** ledc_task_duty_scale_update_ch2_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents LEDC_task_duty_scale_update_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch2_st:1;
        /** ledc_task_duty_scale_update_ch3_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents LEDC_task_duty_scale_update_ch3 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch3_st:1;
    };
    uint32_t val;
} soc_etm_task_st0_reg_t;

/** Type of task_st1 register
 *  Tasks trigger status register
 */
typedef union {
    struct {
        /** ledc_task_duty_scale_update_ch4_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents LEDC_task_duty_scale_update_ch4 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch4_st:1;
        /** ledc_task_duty_scale_update_ch5_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents LEDC_task_duty_scale_update_ch5 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch5_st:1;
        /** ledc_task_timer0_cap_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents LEDC_task_timer0_cap trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer0_cap_st:1;
        /** ledc_task_timer1_cap_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents LEDC_task_timer1_cap trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer1_cap_st:1;
        /** ledc_task_timer2_cap_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents LEDC_task_timer2_cap trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer2_cap_st:1;
        /** ledc_task_timer3_cap_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents LEDC_task_timer3_cap trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer3_cap_st:1;
        /** ledc_task_sig_out_dis_ch0_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents LEDC_task_sig_out_dis_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch0_st:1;
        /** ledc_task_sig_out_dis_ch1_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents LEDC_task_sig_out_dis_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch1_st:1;
        /** ledc_task_sig_out_dis_ch2_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents LEDC_task_sig_out_dis_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch2_st:1;
        /** ledc_task_sig_out_dis_ch3_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents LEDC_task_sig_out_dis_ch3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch3_st:1;
        /** ledc_task_sig_out_dis_ch4_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents LEDC_task_sig_out_dis_ch4 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch4_st:1;
        /** ledc_task_sig_out_dis_ch5_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents LEDC_task_sig_out_dis_ch5 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch5_st:1;
        /** ledc_task_ovf_cnt_rst_ch0_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents LEDC_task_ovf_cnt_rst_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch0_st:1;
        /** ledc_task_ovf_cnt_rst_ch1_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents LEDC_task_ovf_cnt_rst_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch1_st:1;
        /** ledc_task_ovf_cnt_rst_ch2_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents LEDC_task_ovf_cnt_rst_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch2_st:1;
        /** ledc_task_ovf_cnt_rst_ch3_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents LEDC_task_ovf_cnt_rst_ch3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch3_st:1;
        /** ledc_task_ovf_cnt_rst_ch4_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents LEDC_task_ovf_cnt_rst_ch4 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch4_st:1;
        /** ledc_task_ovf_cnt_rst_ch5_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents LEDC_task_ovf_cnt_rst_ch5 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch5_st:1;
        /** ledc_task_timer0_rst_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents LEDC_task_timer0_rst trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer0_rst_st:1;
        /** ledc_task_timer1_rst_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents LEDC_task_timer1_rst trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer1_rst_st:1;
        /** ledc_task_timer2_rst_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents LEDC_task_timer2_rst trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer2_rst_st:1;
        /** ledc_task_timer3_rst_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents LEDC_task_timer3_rst trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer3_rst_st:1;
        /** ledc_task_timer0_resume_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents LEDC_task_timer0_resume trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer0_resume_st:1;
        /** ledc_task_timer1_resume_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents LEDC_task_timer1_resume trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer1_resume_st:1;
        /** ledc_task_timer2_resume_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents LEDC_task_timer2_resume trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer2_resume_st:1;
        /** ledc_task_timer3_resume_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents LEDC_task_timer3_resume trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer3_resume_st:1;
        /** ledc_task_timer0_pause_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents LEDC_task_timer0_pause trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer0_pause_st:1;
        /** ledc_task_timer1_pause_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents LEDC_task_timer1_pause trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer1_pause_st:1;
        /** ledc_task_timer2_pause_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents LEDC_task_timer2_pause trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer2_pause_st:1;
        /** ledc_task_timer3_pause_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents LEDC_task_timer3_pause trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_timer3_pause_st:1;
        /** ledc_task_gamma_restart_ch0_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents LEDC_task_gamma_restart_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_restart_ch0_st:1;
        /** ledc_task_gamma_restart_ch1_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents LEDC_task_gamma_restart_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_restart_ch1_st:1;
    };
    uint32_t val;
} soc_etm_task_st1_reg_t;

/** Type of task_st2 register
 *  Tasks trigger status register
 */
typedef union {
    struct {
        /** ledc_task_gamma_restart_ch2_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents LEDC_task_gamma_restart_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_restart_ch2_st:1;
        /** ledc_task_gamma_restart_ch3_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents LEDC_task_gamma_restart_ch3 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_restart_ch3_st:1;
        /** ledc_task_gamma_restart_ch4_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents LEDC_task_gamma_restart_ch4 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_restart_ch4_st:1;
        /** ledc_task_gamma_restart_ch5_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents LEDC_task_gamma_restart_ch5 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_restart_ch5_st:1;
        /** ledc_task_gamma_pause_ch0_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents LEDC_task_gamma_pause_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch0_st:1;
        /** ledc_task_gamma_pause_ch1_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents LEDC_task_gamma_pause_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch1_st:1;
        /** ledc_task_gamma_pause_ch2_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents LEDC_task_gamma_pause_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch2_st:1;
        /** ledc_task_gamma_pause_ch3_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents LEDC_task_gamma_pause_ch3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch3_st:1;
        /** ledc_task_gamma_pause_ch4_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents LEDC_task_gamma_pause_ch4 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch4_st:1;
        /** ledc_task_gamma_pause_ch5_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents LEDC_task_gamma_pause_ch5 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch5_st:1;
        /** ledc_task_gamma_resume_ch0_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents LEDC_task_gamma_resume_ch0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_resume_ch0_st:1;
        /** ledc_task_gamma_resume_ch1_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents LEDC_task_gamma_resume_ch1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_resume_ch1_st:1;
        /** ledc_task_gamma_resume_ch2_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents LEDC_task_gamma_resume_ch2 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_resume_ch2_st:1;
        /** ledc_task_gamma_resume_ch3_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents LEDC_task_gamma_resume_ch3 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_resume_ch3_st:1;
        /** ledc_task_gamma_resume_ch4_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents LEDC_task_gamma_resume_ch4 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_resume_ch4_st:1;
        /** ledc_task_gamma_resume_ch5_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents LEDC_task_gamma_resume_ch5 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t ledc_task_gamma_resume_ch5_st:1;
        /** tg0_task_cnt_start_timer0_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents TG0_task_cnt_start_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_task_cnt_start_timer0_st:1;
        /** tg0_task_alarm_start_timer0_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents TG0_task_alarm_start_timer0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg0_task_alarm_start_timer0_st:1;
        /** tg0_task_cnt_stop_timer0_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents TG0_task_cnt_stop_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_task_cnt_stop_timer0_st:1;
        /** tg0_task_cnt_reload_timer0_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents TG0_task_cnt_reload_timer0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg0_task_cnt_reload_timer0_st:1;
        /** tg0_task_cnt_cap_timer0_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents TG0_task_cnt_cap_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_task_cnt_cap_timer0_st:1;
        /** tg0_task_cnt_start_timer1_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents TG0_task_cnt_start_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_task_cnt_start_timer1_st:1;
        /** tg0_task_alarm_start_timer1_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents TG0_task_alarm_start_timer1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg0_task_alarm_start_timer1_st:1;
        /** tg0_task_cnt_stop_timer1_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents TG0_task_cnt_stop_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_task_cnt_stop_timer1_st:1;
        /** tg0_task_cnt_reload_timer1_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents TG0_task_cnt_reload_timer1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg0_task_cnt_reload_timer1_st:1;
        /** tg0_task_cnt_cap_timer1_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents TG0_task_cnt_cap_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg0_task_cnt_cap_timer1_st:1;
        /** tg1_task_cnt_start_timer0_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents TG1_task_cnt_start_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_task_cnt_start_timer0_st:1;
        /** tg1_task_alarm_start_timer0_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents TG1_task_alarm_start_timer0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg1_task_alarm_start_timer0_st:1;
        /** tg1_task_cnt_stop_timer0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents TG1_task_cnt_stop_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_task_cnt_stop_timer0_st:1;
        /** tg1_task_cnt_reload_timer0_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents TG1_task_cnt_reload_timer0 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg1_task_cnt_reload_timer0_st:1;
        /** tg1_task_cnt_cap_timer0_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents TG1_task_cnt_cap_timer0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_task_cnt_cap_timer0_st:1;
        /** tg1_task_cnt_start_timer1_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents TG1_task_cnt_start_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_task_cnt_start_timer1_st:1;
    };
    uint32_t val;
} soc_etm_task_st2_reg_t;

/** Type of task_st3 register
 *  Tasks trigger status register
 */
typedef union {
    struct {
        /** tg1_task_alarm_start_timer1_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents TG1_task_alarm_start_timer1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg1_task_alarm_start_timer1_st:1;
        /** tg1_task_cnt_stop_timer1_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents TG1_task_cnt_stop_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_task_cnt_stop_timer1_st:1;
        /** tg1_task_cnt_reload_timer1_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents TG1_task_cnt_reload_timer1 trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t tg1_task_cnt_reload_timer1_st:1;
        /** tg1_task_cnt_cap_timer1_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents TG1_task_cnt_cap_timer1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tg1_task_cnt_cap_timer1_st:1;
        /** mcpwm0_task_cmpr0_a_up_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents MCPWM0_task_cmpr0_a_up trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cmpr0_a_up_st:1;
        /** mcpwm0_task_cmpr1_a_up_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents MCPWM0_task_cmpr1_a_up trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cmpr1_a_up_st:1;
        /** mcpwm0_task_cmpr2_a_up_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents MCPWM0_task_cmpr2_a_up trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cmpr2_a_up_st:1;
        /** mcpwm0_task_cmpr0_b_up_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents MCPWM0_task_cmpr0_b_up trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cmpr0_b_up_st:1;
        /** mcpwm0_task_cmpr1_b_up_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents MCPWM0_task_cmpr1_b_up trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cmpr1_b_up_st:1;
        /** mcpwm0_task_cmpr2_b_up_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents MCPWM0_task_cmpr2_b_up trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cmpr2_b_up_st:1;
        /** mcpwm0_task_gen_stop_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents MCPWM0_task_gen_stop trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_gen_stop_st:1;
        /** mcpwm0_task_timer0_syn_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents MCPWM0_task_timer0_syn trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_timer0_syn_st:1;
        /** mcpwm0_task_timer1_syn_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents MCPWM0_task_timer1_syn trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_timer1_syn_st:1;
        /** mcpwm0_task_timer2_syn_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents MCPWM0_task_timer2_syn trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_timer2_syn_st:1;
        /** mcpwm0_task_timer0_period_up_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents MCPWM0_task_timer0_period_up trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t mcpwm0_task_timer0_period_up_st:1;
        /** mcpwm0_task_timer1_period_up_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents MCPWM0_task_timer1_period_up trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t mcpwm0_task_timer1_period_up_st:1;
        /** mcpwm0_task_timer2_period_up_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents MCPWM0_task_timer2_period_up trigger status.\\0: Not triggered\\1:
         *  Triggered
         */
        uint32_t mcpwm0_task_timer2_period_up_st:1;
        /** mcpwm0_task_tz0_ost_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents MCPWM0_task_tz0_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_tz0_ost_st:1;
        /** mcpwm0_task_tz1_ost_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents MCPWM0_task_tz1_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_tz1_ost_st:1;
        /** mcpwm0_task_tz2_ost_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents MCPWM0_task_tz2_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_tz2_ost_st:1;
        /** mcpwm0_task_clr0_ost_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents MCPWM0_task_clr0_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_clr0_ost_st:1;
        /** mcpwm0_task_clr1_ost_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents MCPWM0_task_clr1_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_clr1_ost_st:1;
        /** mcpwm0_task_clr2_ost_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents MCPWM0_task_clr2_ost trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_clr2_ost_st:1;
        /** mcpwm0_task_cap0_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents MCPWM0_task_cap0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cap0_st:1;
        /** mcpwm0_task_cap1_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents MCPWM0_task_cap1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cap1_st:1;
        /** mcpwm0_task_cap2_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents MCPWM0_task_cap2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t mcpwm0_task_cap2_st:1;
        /** adc_task_sample0_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents ADC_task_sample0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_task_sample0_st:1;
        /** adc_task_sample1_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents ADC_task_sample1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_task_sample1_st:1;
        /** adc_task_start0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents ADC_task_start0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_task_start0_st:1;
        /** adc_task_stop0_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents ADC_task_stop0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t adc_task_stop0_st:1;
        /** regdma_task_start0_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents REGDMA_task_start0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_task_start0_st:1;
        /** regdma_task_start1_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents REGDMA_task_start1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_task_start1_st:1;
    };
    uint32_t val;
} soc_etm_task_st3_reg_t;

/** Type of task_st4 register
 *  Tasks trigger status register
 */
typedef union {
    struct {
        /** regdma_task_start2_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents REGDMA_task_start2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_task_start2_st:1;
        /** regdma_task_start3_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents REGDMA_task_start3 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t regdma_task_start3_st:1;
        /** tmpsnsr_task_start_sample_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents TMPSNSR_task_start_sample trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tmpsnsr_task_start_sample_st:1;
        /** tmpsnsr_task_stop_sample_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents TMPSNSR_task_stop_sample trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t tmpsnsr_task_stop_sample_st:1;
        /** i2s0_task_start_rx_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents I2S0_task_start_rx trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_task_start_rx_st:1;
        /** i2s0_task_start_tx_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents I2S0_task_start_tx trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_task_start_tx_st:1;
        /** i2s0_task_stop_rx_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents I2S0_task_stop_rx trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_task_stop_rx_st:1;
        /** i2s0_task_stop_tx_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents I2S0_task_stop_tx trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t i2s0_task_stop_tx_st:1;
        /** ulp_task_wakeup_cpu_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents ULP_task_wakeup_cpu trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ulp_task_wakeup_cpu_st:1;
        /** ulp_task_int_cpu_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents ULP_task_int_cpu trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t ulp_task_int_cpu_st:1;
        /** rtc_task_start_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents RTC_task_start trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t rtc_task_start_st:1;
        /** rtc_task_stop_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents RTC_task_stop trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t rtc_task_stop_st:1;
        /** rtc_task_clr_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents RTC_task_clr trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t rtc_task_clr_st:1;
        /** rtc_task_triggerflw_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents RTC_task_triggerflw trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t rtc_task_triggerflw_st:1;
        /** gdma_task_in_start_ch0_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents GDMA_task_in_start_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_task_in_start_ch0_st:1;
        /** gdma_task_in_start_ch1_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents GDMA_task_in_start_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_task_in_start_ch1_st:1;
        /** gdma_task_in_start_ch2_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents GDMA_task_in_start_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_task_in_start_ch2_st:1;
        /** gdma_task_out_start_ch0_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents GDMA_task_out_start_ch0 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_task_out_start_ch0_st:1;
        /** gdma_task_out_start_ch1_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents GDMA_task_out_start_ch1 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_task_out_start_ch1_st:1;
        /** gdma_task_out_start_ch2_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents GDMA_task_out_start_ch2 trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t gdma_task_out_start_ch2_st:1;
        /** pmu_task_sleep_req_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents PMU_task_sleep_req trigger status.\\0: Not triggered\\1: Triggered
         */
        uint32_t pmu_task_sleep_req_st:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} soc_etm_task_st4_reg_t;


/** Group: Configuration Register */
/** Type of ch_ena_ad0_set register
 *  Channel enable set register
 */
typedef union {
    struct {
        /** ch_enable0 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable ch0.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable0:1;
        /** ch_enable1 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to enable ch1.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable1:1;
        /** ch_enable2 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to enable ch2.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable2:1;
        /** ch_enable3 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to enable ch3.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable3:1;
        /** ch_enable4 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to enable ch4.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable4:1;
        /** ch_enable5 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to enable ch5.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable5:1;
        /** ch_enable6 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to enable ch6.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable6:1;
        /** ch_enable7 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to enable ch7.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable7:1;
        /** ch_enable8 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to enable ch8.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable8:1;
        /** ch_enable9 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to enable ch9.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable9:1;
        /** ch_enable10 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to enable ch10.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable10:1;
        /** ch_enable11 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to enable ch11.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable11:1;
        /** ch_enable12 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to enable ch12.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable12:1;
        /** ch_enable13 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to enable ch13.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable13:1;
        /** ch_enable14 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to enable ch14.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable14:1;
        /** ch_enable15 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to enable ch15.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable15:1;
        /** ch_enable16 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to enable ch16.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable16:1;
        /** ch_enable17 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to enable ch17.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable17:1;
        /** ch_enable18 : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to enable ch18.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable18:1;
        /** ch_enable19 : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to enable ch19.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable19:1;
        /** ch_enable20 : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to enable ch20.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable20:1;
        /** ch_enable21 : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to enable ch21.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable21:1;
        /** ch_enable22 : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to enable ch22.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable22:1;
        /** ch_enable23 : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to enable ch23.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable23:1;
        /** ch_enable24 : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to enable ch24.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable24:1;
        /** ch_enable25 : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to enable ch25.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable25:1;
        /** ch_enable26 : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to enable ch26.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable26:1;
        /** ch_enable27 : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to enable ch27.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable27:1;
        /** ch_enable28 : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to enable ch28.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable28:1;
        /** ch_enable29 : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to enable ch29.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable29:1;
        /** ch_enable30 : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to enable ch30.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable30:1;
        /** ch_enable31 : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to enable ch31.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_set_reg_t;

/** Type of ch_ena_ad0_clr register
 *  Channel enable clear register
 */
typedef union {
    struct {
        /** ch_disable0 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear ch0 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable0:1;
        /** ch_disable1 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear ch1 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable1:1;
        /** ch_disable2 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear ch2 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable2:1;
        /** ch_disable3 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear ch3 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable3:1;
        /** ch_disable4 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear ch4 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable4:1;
        /** ch_disable5 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear ch5 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable5:1;
        /** ch_disable6 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear ch6 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable6:1;
        /** ch_disable7 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear ch7 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable7:1;
        /** ch_disable8 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear ch8 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable8:1;
        /** ch_disable9 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear ch9 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable9:1;
        /** ch_disable10 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear ch10 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable10:1;
        /** ch_disable11 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear ch11 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable11:1;
        /** ch_disable12 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear ch12 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable12:1;
        /** ch_disable13 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear ch13 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable13:1;
        /** ch_disable14 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear ch14 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable14:1;
        /** ch_disable15 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear ch15 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable15:1;
        /** ch_disable16 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear ch16 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable16:1;
        /** ch_disable17 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear ch17 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable17:1;
        /** ch_disable18 : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear ch18 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable18:1;
        /** ch_disable19 : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear ch19 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable19:1;
        /** ch_disable20 : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear ch20 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable20:1;
        /** ch_disable21 : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear ch21 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable21:1;
        /** ch_disable22 : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear ch22 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable22:1;
        /** ch_disable23 : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear ch23 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable23:1;
        /** ch_disable24 : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear ch24 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable24:1;
        /** ch_disable25 : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear ch25 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable25:1;
        /** ch_disable26 : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear ch26 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable26:1;
        /** ch_disable27 : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear ch27 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable27:1;
        /** ch_disable28 : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear ch28 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable28:1;
        /** ch_disable29 : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear ch29 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable29:1;
        /** ch_disable30 : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear ch30 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable30:1;
        /** ch_disable31 : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear ch31 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_clr_reg_t;

/** Type of ch_ena_ad1_set register
 *  Channel enable set register
 */
typedef union {
    struct {
        /** ch_enable32 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable ch32.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable32:1;
        /** ch_enable33 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to enable ch33.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable33:1;
        /** ch_enable34 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to enable ch34.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable34:1;
        /** ch_enable35 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to enable ch35.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable35:1;
        /** ch_enable36 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to enable ch36.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable36:1;
        /** ch_enable37 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to enable ch37.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable37:1;
        /** ch_enable38 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to enable ch38.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable38:1;
        /** ch_enable39 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to enable ch39.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable39:1;
        /** ch_enable40 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to enable ch40.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable40:1;
        /** ch_enable41 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to enable ch41.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable41:1;
        /** ch_enable42 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to enable ch42.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable42:1;
        /** ch_enable43 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to enable ch43.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable43:1;
        /** ch_enable44 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to enable ch44.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable44:1;
        /** ch_enable45 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to enable ch45.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable45:1;
        /** ch_enable46 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to enable ch46.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable46:1;
        /** ch_enable47 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to enable ch47.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable47:1;
        /** ch_enable48 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to enable ch48.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable48:1;
        /** ch_enable49 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to enable ch49.\\0: Invalid, No effect\\1: Enable
         */
        uint32_t ch_enable49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_set_reg_t;

/** Type of ch_ena_ad1_clr register
 *  Channel enable clear register
 */
typedef union {
    struct {
        /** ch_disable32 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear ch32 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable32:1;
        /** ch_disable33 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear ch33 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable33:1;
        /** ch_disable34 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear ch34 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable34:1;
        /** ch_disable35 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear ch35 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable35:1;
        /** ch_disable36 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear ch36 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable36:1;
        /** ch_disable37 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear ch37 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable37:1;
        /** ch_disable38 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear ch38 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable38:1;
        /** ch_disable39 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear ch39 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable39:1;
        /** ch_disable40 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear ch40 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable40:1;
        /** ch_disable41 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear ch41 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable41:1;
        /** ch_disable42 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear ch42 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable42:1;
        /** ch_disable43 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear ch43 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable43:1;
        /** ch_disable44 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear ch44 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable44:1;
        /** ch_disable45 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear ch45 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable45:1;
        /** ch_disable46 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear ch46 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable46:1;
        /** ch_disable47 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear ch47 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable47:1;
        /** ch_disable48 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear ch48 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable48:1;
        /** ch_disable49 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear ch49 enable.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ch_disable49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_clr_reg_t;

/** Type of chn_evt_id register
 *  Channeln event id register
 */
typedef union {
    struct {
        /** chn_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  Configures chn_evt_id
         */
        uint32_t chn_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_chn_evt_id_reg_t;

/** Type of chn_task_id register
 *  Channeln task id register
 */
typedef union {
    struct {
        /** chn_task_id : R/W; bitpos: [7:0]; default: 0;
         *  Configures chn_task_id
         */
        uint32_t chn_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_chn_task_id_reg_t;

/** Type of evt_st0_clr register
 *  Events trigger status clear register
 */
typedef union {
    struct {
        /** gpio_evt_ch0_rise_edge_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch0_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch0_rise_edge_st_clr:1;
        /** gpio_evt_ch1_rise_edge_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch1_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch1_rise_edge_st_clr:1;
        /** gpio_evt_ch2_rise_edge_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch2_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch2_rise_edge_st_clr:1;
        /** gpio_evt_ch3_rise_edge_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch3_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch3_rise_edge_st_clr:1;
        /** gpio_evt_ch4_rise_edge_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch4_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch4_rise_edge_st_clr:1;
        /** gpio_evt_ch5_rise_edge_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch5_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch5_rise_edge_st_clr:1;
        /** gpio_evt_ch6_rise_edge_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch6_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch6_rise_edge_st_clr:1;
        /** gpio_evt_ch7_rise_edge_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch7_rise_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch7_rise_edge_st_clr:1;
        /** gpio_evt_ch0_fall_edge_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch0_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch0_fall_edge_st_clr:1;
        /** gpio_evt_ch1_fall_edge_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch1_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch1_fall_edge_st_clr:1;
        /** gpio_evt_ch2_fall_edge_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch2_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch2_fall_edge_st_clr:1;
        /** gpio_evt_ch3_fall_edge_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch3_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch3_fall_edge_st_clr:1;
        /** gpio_evt_ch4_fall_edge_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch4_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch4_fall_edge_st_clr:1;
        /** gpio_evt_ch5_fall_edge_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch5_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch5_fall_edge_st_clr:1;
        /** gpio_evt_ch6_fall_edge_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch6_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch6_fall_edge_st_clr:1;
        /** gpio_evt_ch7_fall_edge_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch7_fall_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch7_fall_edge_st_clr:1;
        /** gpio_evt_ch0_any_edge_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch0_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch0_any_edge_st_clr:1;
        /** gpio_evt_ch1_any_edge_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch1_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch1_any_edge_st_clr:1;
        /** gpio_evt_ch2_any_edge_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch2_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch2_any_edge_st_clr:1;
        /** gpio_evt_ch3_any_edge_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch3_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch3_any_edge_st_clr:1;
        /** gpio_evt_ch4_any_edge_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch4_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch4_any_edge_st_clr:1;
        /** gpio_evt_ch5_any_edge_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch5_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch5_any_edge_st_clr:1;
        /** gpio_evt_ch6_any_edge_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch6_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch6_any_edge_st_clr:1;
        /** gpio_evt_ch7_any_edge_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear GPIO_evt_ch7_any_edge trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_ch7_any_edge_st_clr:1;
        /** gpio_evt_zero_det_pos0_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear GPIO_evt_zero_det_pos0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_zero_det_pos0_st_clr:1;
        /** gpio_evt_zero_det_neg0_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear GPIO_evt_zero_det_neg0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_evt_zero_det_neg0_st_clr:1;
        /** ledc_evt_duty_chng_end_ch0_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch0_st_clr:1;
        /** ledc_evt_duty_chng_end_ch1_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch1_st_clr:1;
        /** ledc_evt_duty_chng_end_ch2_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch2_st_clr:1;
        /** ledc_evt_duty_chng_end_ch3_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch3_st_clr:1;
        /** ledc_evt_duty_chng_end_ch4_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch4 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch4_st_clr:1;
        /** ledc_evt_duty_chng_end_ch5_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear LEDC_evt_duty_chng_end_ch5 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch5_st_clr:1;
    };
    uint32_t val;
} soc_etm_evt_st0_clr_reg_t;

/** Type of evt_st1_clr register
 *  Events trigger status clear register
 */
typedef union {
    struct {
        /** ledc_evt_ovf_cnt_pls_ch0_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch0_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch1_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch1_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch2_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch2_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch3_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch3_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch4_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch4 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch4_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch5_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear LEDC_evt_ovf_cnt_pls_ch5 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch5_st_clr:1;
        /** ledc_evt_time_ovf_timer0_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear LEDC_evt_time_ovf_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer0_st_clr:1;
        /** ledc_evt_time_ovf_timer1_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear LEDC_evt_time_ovf_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer1_st_clr:1;
        /** ledc_evt_time_ovf_timer2_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear LEDC_evt_time_ovf_timer2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer2_st_clr:1;
        /** ledc_evt_time_ovf_timer3_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear LEDC_evt_time_ovf_timer3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer3_st_clr:1;
        /** ledc_evt_timer0_cmp_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear LEDC_evt_timer0_cmp trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t ledc_evt_timer0_cmp_st_clr:1;
        /** ledc_evt_timer1_cmp_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear LEDC_evt_timer1_cmp trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t ledc_evt_timer1_cmp_st_clr:1;
        /** ledc_evt_timer2_cmp_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear LEDC_evt_timer2_cmp trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t ledc_evt_timer2_cmp_st_clr:1;
        /** ledc_evt_timer3_cmp_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear LEDC_evt_timer3_cmp trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t ledc_evt_timer3_cmp_st_clr:1;
        /** tg0_evt_cnt_cmp_timer0_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear TG0_evt_cnt_cmp_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_evt_cnt_cmp_timer0_st_clr:1;
        /** tg0_evt_cnt_cmp_timer1_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear TG0_evt_cnt_cmp_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_evt_cnt_cmp_timer1_st_clr:1;
        /** tg1_evt_cnt_cmp_timer0_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear TG1_evt_cnt_cmp_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_evt_cnt_cmp_timer0_st_clr:1;
        /** tg1_evt_cnt_cmp_timer1_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear TG1_evt_cnt_cmp_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_evt_cnt_cmp_timer1_st_clr:1;
        /** systimer_evt_cnt_cmp0_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear SYSTIMER_evt_cnt_cmp0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t systimer_evt_cnt_cmp0_st_clr:1;
        /** systimer_evt_cnt_cmp1_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear SYSTIMER_evt_cnt_cmp1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t systimer_evt_cnt_cmp1_st_clr:1;
        /** systimer_evt_cnt_cmp2_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear SYSTIMER_evt_cnt_cmp2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t systimer_evt_cnt_cmp2_st_clr:1;
        /** mcpwm0_evt_timer0_stop_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer0_stop trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer0_stop_st_clr:1;
        /** mcpwm0_evt_timer1_stop_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer1_stop trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer1_stop_st_clr:1;
        /** mcpwm0_evt_timer2_stop_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer2_stop trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer2_stop_st_clr:1;
        /** mcpwm0_evt_timer0_tez_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer0_tez trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer0_tez_st_clr:1;
        /** mcpwm0_evt_timer1_tez_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer1_tez trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer1_tez_st_clr:1;
        /** mcpwm0_evt_timer2_tez_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer2_tez trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer2_tez_st_clr:1;
        /** mcpwm0_evt_timer0_tep_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer0_tep trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer0_tep_st_clr:1;
        /** mcpwm0_evt_timer1_tep_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer1_tep trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer1_tep_st_clr:1;
        /** mcpwm0_evt_timer2_tep_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_timer2_tep trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_timer2_tep_st_clr:1;
        /** mcpwm0_evt_op0_tea_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op0_tea trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op0_tea_st_clr:1;
        /** mcpwm0_evt_op1_tea_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op1_tea trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op1_tea_st_clr:1;
    };
    uint32_t val;
} soc_etm_evt_st1_clr_reg_t;

/** Type of evt_st2_clr register
 *  Events trigger status clear register
 */
typedef union {
    struct {
        /** mcpwm0_evt_op2_tea_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op2_tea trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op2_tea_st_clr:1;
        /** mcpwm0_evt_op0_teb_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op0_teb trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op0_teb_st_clr:1;
        /** mcpwm0_evt_op1_teb_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op1_teb trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op1_teb_st_clr:1;
        /** mcpwm0_evt_op2_teb_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op2_teb trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op2_teb_st_clr:1;
        /** mcpwm0_evt_f0_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_f0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_evt_f0_st_clr:1;
        /** mcpwm0_evt_f1_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_f1 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_evt_f1_st_clr:1;
        /** mcpwm0_evt_f2_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_f2 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_evt_f2_st_clr:1;
        /** mcpwm0_evt_f0_clr_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_f0_clr trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_f0_clr_st_clr:1;
        /** mcpwm0_evt_f1_clr_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_f1_clr trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_f1_clr_st_clr:1;
        /** mcpwm0_evt_f2_clr_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_f2_clr trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_f2_clr_st_clr:1;
        /** mcpwm0_evt_tz0_cbc_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_tz0_cbc trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_tz0_cbc_st_clr:1;
        /** mcpwm0_evt_tz1_cbc_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_tz1_cbc trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_tz1_cbc_st_clr:1;
        /** mcpwm0_evt_tz2_cbc_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_tz2_cbc trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_tz2_cbc_st_clr:1;
        /** mcpwm0_evt_tz0_ost_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_tz0_ost trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_tz0_ost_st_clr:1;
        /** mcpwm0_evt_tz1_ost_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_tz1_ost trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_tz1_ost_st_clr:1;
        /** mcpwm0_evt_tz2_ost_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_tz2_ost trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_tz2_ost_st_clr:1;
        /** mcpwm0_evt_cap0_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_cap0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_evt_cap0_st_clr:1;
        /** mcpwm0_evt_cap1_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_cap1 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_evt_cap1_st_clr:1;
        /** mcpwm0_evt_cap2_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_cap2 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_evt_cap2_st_clr:1;
        /** mcpwm0_evt_op0_tee1_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op0_tee1 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op0_tee1_st_clr:1;
        /** mcpwm0_evt_op1_tee1_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op1_tee1 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op1_tee1_st_clr:1;
        /** mcpwm0_evt_op2_tee1_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op2_tee1 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op2_tee1_st_clr:1;
        /** mcpwm0_evt_op0_tee2_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op0_tee2 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op0_tee2_st_clr:1;
        /** mcpwm0_evt_op1_tee2_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op1_tee2 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op1_tee2_st_clr:1;
        /** mcpwm0_evt_op2_tee2_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear MCPWM0_evt_op2_tee2 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_evt_op2_tee2_st_clr:1;
        /** adc_evt_conv_cmplt0_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear ADC_evt_conv_cmplt0 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t adc_evt_conv_cmplt0_st_clr:1;
        /** adc_evt_eq_above_thresh0_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear ADC_evt_eq_above_thresh0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t adc_evt_eq_above_thresh0_st_clr:1;
        /** adc_evt_eq_above_thresh1_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear ADC_evt_eq_above_thresh1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t adc_evt_eq_above_thresh1_st_clr:1;
        /** adc_evt_eq_below_thresh0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear ADC_evt_eq_below_thresh0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t adc_evt_eq_below_thresh0_st_clr:1;
        /** adc_evt_eq_below_thresh1_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear ADC_evt_eq_below_thresh1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t adc_evt_eq_below_thresh1_st_clr:1;
        /** adc_evt_result_done0_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear ADC_evt_result_done0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t adc_evt_result_done0_st_clr:1;
        /** adc_evt_stopped0_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear ADC_evt_stopped0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t adc_evt_stopped0_st_clr:1;
    };
    uint32_t val;
} soc_etm_evt_st2_clr_reg_t;

/** Type of evt_st3_clr register
 *  Events trigger status clear register
 */
typedef union {
    struct {
        /** adc_evt_started0_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear ADC_evt_started0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t adc_evt_started0_st_clr:1;
        /** regdma_evt_done0_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_done0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_done0_st_clr:1;
        /** regdma_evt_done1_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_done1 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_done1_st_clr:1;
        /** regdma_evt_done2_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_done2 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_done2_st_clr:1;
        /** regdma_evt_done3_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_done3 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_done3_st_clr:1;
        /** regdma_evt_err0_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_err0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_err0_st_clr:1;
        /** regdma_evt_err1_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_err1 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_err1_st_clr:1;
        /** regdma_evt_err2_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_err2 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_err2_st_clr:1;
        /** regdma_evt_err3_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear REGDMA_evt_err3 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t regdma_evt_err3_st_clr:1;
        /** tmpsnsr_evt_over_limit_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear TMPSNSR_evt_over_limit trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tmpsnsr_evt_over_limit_st_clr:1;
        /** i2s0_evt_rx_done_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear I2S0_evt_rx_done trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t i2s0_evt_rx_done_st_clr:1;
        /** i2s0_evt_tx_done_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear I2S0_evt_tx_done trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t i2s0_evt_tx_done_st_clr:1;
        /** i2s0_evt_x_words_received_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear I2S0_evt_x_words_received trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t i2s0_evt_x_words_received_st_clr:1;
        /** i2s0_evt_x_words_sent_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear I2S0_evt_x_words_sent trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t i2s0_evt_x_words_sent_st_clr:1;
        /** ulp_evt_err_intr_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear ULP_evt_err_intr trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t ulp_evt_err_intr_st_clr:1;
        /** ulp_evt_halt_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear ULP_evt_halt trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t ulp_evt_halt_st_clr:1;
        /** ulp_evt_start_intr_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear ULP_evt_start_intr trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t ulp_evt_start_intr_st_clr:1;
        /** rtc_evt_tick_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear RTC_evt_tick trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t rtc_evt_tick_st_clr:1;
        /** rtc_evt_ovf_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear RTC_evt_ovf trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t rtc_evt_ovf_st_clr:1;
        /** rtc_evt_cmp_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear RTC_evt_cmp trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t rtc_evt_cmp_st_clr:1;
        /** gdma_evt_in_done_ch0_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_done_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_done_ch0_st_clr:1;
        /** gdma_evt_in_done_ch1_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_done_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_done_ch1_st_clr:1;
        /** gdma_evt_in_done_ch2_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_done_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_done_ch2_st_clr:1;
        /** gdma_evt_in_suc_eof_ch0_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_suc_eof_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_suc_eof_ch0_st_clr:1;
        /** gdma_evt_in_suc_eof_ch1_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_suc_eof_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_suc_eof_ch1_st_clr:1;
        /** gdma_evt_in_suc_eof_ch2_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_suc_eof_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_suc_eof_ch2_st_clr:1;
        /** gdma_evt_in_fifo_empty_ch0_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_fifo_empty_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_fifo_empty_ch0_st_clr:1;
        /** gdma_evt_in_fifo_empty_ch1_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_fifo_empty_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_fifo_empty_ch1_st_clr:1;
        /** gdma_evt_in_fifo_empty_ch2_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_fifo_empty_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_fifo_empty_ch2_st_clr:1;
        /** gdma_evt_in_fifo_full_ch0_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_fifo_full_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_fifo_full_ch0_st_clr:1;
        /** gdma_evt_in_fifo_full_ch1_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_fifo_full_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_fifo_full_ch1_st_clr:1;
        /** gdma_evt_in_fifo_full_ch2_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear GDMA_evt_in_fifo_full_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_in_fifo_full_ch2_st_clr:1;
    };
    uint32_t val;
} soc_etm_evt_st3_clr_reg_t;

/** Type of evt_st4_clr register
 *  Events trigger status clear register
 */
typedef union {
    struct {
        /** gdma_evt_out_done_ch0_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_done_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_done_ch0_st_clr:1;
        /** gdma_evt_out_done_ch1_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_done_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_done_ch1_st_clr:1;
        /** gdma_evt_out_done_ch2_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_done_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_done_ch2_st_clr:1;
        /** gdma_evt_out_eof_ch0_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_eof_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_eof_ch0_st_clr:1;
        /** gdma_evt_out_eof_ch1_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_eof_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_eof_ch1_st_clr:1;
        /** gdma_evt_out_eof_ch2_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_eof_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_eof_ch2_st_clr:1;
        /** gdma_evt_out_total_eof_ch0_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_total_eof_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_total_eof_ch0_st_clr:1;
        /** gdma_evt_out_total_eof_ch1_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_total_eof_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_total_eof_ch1_st_clr:1;
        /** gdma_evt_out_total_eof_ch2_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_total_eof_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_total_eof_ch2_st_clr:1;
        /** gdma_evt_out_fifo_empty_ch0_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_fifo_empty_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_fifo_empty_ch0_st_clr:1;
        /** gdma_evt_out_fifo_empty_ch1_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_fifo_empty_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_fifo_empty_ch1_st_clr:1;
        /** gdma_evt_out_fifo_empty_ch2_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_fifo_empty_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_fifo_empty_ch2_st_clr:1;
        /** gdma_evt_out_fifo_full_ch0_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_fifo_full_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_fifo_full_ch0_st_clr:1;
        /** gdma_evt_out_fifo_full_ch1_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_fifo_full_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_fifo_full_ch1_st_clr:1;
        /** gdma_evt_out_fifo_full_ch2_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear GDMA_evt_out_fifo_full_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_evt_out_fifo_full_ch2_st_clr:1;
        /** pmu_evt_sleep_weekup_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear PMU_evt_sleep_weekup trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t pmu_evt_sleep_weekup_st_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} soc_etm_evt_st4_clr_reg_t;

/** Type of task_st0_clr register
 *  Tasks trigger status clear register
 */
typedef union {
    struct {
        /** gpio_task_ch0_set_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch0_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch0_set_st_clr:1;
        /** gpio_task_ch1_set_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch1_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch1_set_st_clr:1;
        /** gpio_task_ch2_set_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch2_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch2_set_st_clr:1;
        /** gpio_task_ch3_set_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch3_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch3_set_st_clr:1;
        /** gpio_task_ch4_set_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch4_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch4_set_st_clr:1;
        /** gpio_task_ch5_set_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch5_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch5_set_st_clr:1;
        /** gpio_task_ch6_set_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch6_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch6_set_st_clr:1;
        /** gpio_task_ch7_set_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch7_set trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch7_set_st_clr:1;
        /** gpio_task_ch0_clear_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch0_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch0_clear_st_clr:1;
        /** gpio_task_ch1_clear_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch1_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch1_clear_st_clr:1;
        /** gpio_task_ch2_clear_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch2_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch2_clear_st_clr:1;
        /** gpio_task_ch3_clear_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch3_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch3_clear_st_clr:1;
        /** gpio_task_ch4_clear_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch4_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch4_clear_st_clr:1;
        /** gpio_task_ch5_clear_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch5_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch5_clear_st_clr:1;
        /** gpio_task_ch6_clear_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch6_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch6_clear_st_clr:1;
        /** gpio_task_ch7_clear_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch7_clear trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t gpio_task_ch7_clear_st_clr:1;
        /** gpio_task_ch0_toggle_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch0_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch0_toggle_st_clr:1;
        /** gpio_task_ch1_toggle_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch1_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch1_toggle_st_clr:1;
        /** gpio_task_ch2_toggle_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch2_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch2_toggle_st_clr:1;
        /** gpio_task_ch3_toggle_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch3_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch3_toggle_st_clr:1;
        /** gpio_task_ch4_toggle_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch4_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch4_toggle_st_clr:1;
        /** gpio_task_ch5_toggle_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch5_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch5_toggle_st_clr:1;
        /** gpio_task_ch6_toggle_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch6_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch6_toggle_st_clr:1;
        /** gpio_task_ch7_toggle_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear GPIO_task_ch7_toggle trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gpio_task_ch7_toggle_st_clr:1;
        /** ledc_task_timer0_res_update_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer0_res_update trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer0_res_update_st_clr:1;
        /** ledc_task_timer1_res_update_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer1_res_update trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer1_res_update_st_clr:1;
        /** ledc_task_timer2_res_update_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer2_res_update trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer2_res_update_st_clr:1;
        /** ledc_task_timer3_res_update_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer3_res_update trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer3_res_update_st_clr:1;
        /** ledc_task_duty_scale_update_ch0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear LEDC_task_duty_scale_update_ch0 trigger
         *  status.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch0_st_clr:1;
        /** ledc_task_duty_scale_update_ch1_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear LEDC_task_duty_scale_update_ch1 trigger
         *  status.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch1_st_clr:1;
        /** ledc_task_duty_scale_update_ch2_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear LEDC_task_duty_scale_update_ch2 trigger
         *  status.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch2_st_clr:1;
        /** ledc_task_duty_scale_update_ch3_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear LEDC_task_duty_scale_update_ch3 trigger
         *  status.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch3_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st0_clr_reg_t;

/** Type of task_st1_clr register
 *  Tasks trigger status clear register
 */
typedef union {
    struct {
        /** ledc_task_duty_scale_update_ch4_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear LEDC_task_duty_scale_update_ch4 trigger
         *  status.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch4_st_clr:1;
        /** ledc_task_duty_scale_update_ch5_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear LEDC_task_duty_scale_update_ch5 trigger
         *  status.\\0: Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch5_st_clr:1;
        /** ledc_task_timer0_cap_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer0_cap trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer0_cap_st_clr:1;
        /** ledc_task_timer1_cap_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer1_cap trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer1_cap_st_clr:1;
        /** ledc_task_timer2_cap_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer2_cap trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer2_cap_st_clr:1;
        /** ledc_task_timer3_cap_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer3_cap trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer3_cap_st_clr:1;
        /** ledc_task_sig_out_dis_ch0_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear LEDC_task_sig_out_dis_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch0_st_clr:1;
        /** ledc_task_sig_out_dis_ch1_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear LEDC_task_sig_out_dis_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch1_st_clr:1;
        /** ledc_task_sig_out_dis_ch2_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear LEDC_task_sig_out_dis_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch2_st_clr:1;
        /** ledc_task_sig_out_dis_ch3_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear LEDC_task_sig_out_dis_ch3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch3_st_clr:1;
        /** ledc_task_sig_out_dis_ch4_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear LEDC_task_sig_out_dis_ch4 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch4_st_clr:1;
        /** ledc_task_sig_out_dis_ch5_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear LEDC_task_sig_out_dis_ch5 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch5_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch0_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch0_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch1_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch1_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch2_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch2_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch3_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch3_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch4_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch4 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch4_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch5_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear LEDC_task_ovf_cnt_rst_ch5 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch5_st_clr:1;
        /** ledc_task_timer0_rst_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer0_rst trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer0_rst_st_clr:1;
        /** ledc_task_timer1_rst_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer1_rst trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer1_rst_st_clr:1;
        /** ledc_task_timer2_rst_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer2_rst trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer2_rst_st_clr:1;
        /** ledc_task_timer3_rst_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer3_rst trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer3_rst_st_clr:1;
        /** ledc_task_timer0_resume_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer0_resume trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer0_resume_st_clr:1;
        /** ledc_task_timer1_resume_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer1_resume trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer1_resume_st_clr:1;
        /** ledc_task_timer2_resume_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer2_resume trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer2_resume_st_clr:1;
        /** ledc_task_timer3_resume_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer3_resume trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer3_resume_st_clr:1;
        /** ledc_task_timer0_pause_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer0_pause trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer0_pause_st_clr:1;
        /** ledc_task_timer1_pause_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer1_pause trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer1_pause_st_clr:1;
        /** ledc_task_timer2_pause_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer2_pause trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer2_pause_st_clr:1;
        /** ledc_task_timer3_pause_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear LEDC_task_timer3_pause trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_timer3_pause_st_clr:1;
        /** ledc_task_gamma_restart_ch0_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_restart_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch0_st_clr:1;
        /** ledc_task_gamma_restart_ch1_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_restart_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch1_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st1_clr_reg_t;

/** Type of task_st2_clr register
 *  Tasks trigger status clear register
 */
typedef union {
    struct {
        /** ledc_task_gamma_restart_ch2_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_restart_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch2_st_clr:1;
        /** ledc_task_gamma_restart_ch3_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_restart_ch3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch3_st_clr:1;
        /** ledc_task_gamma_restart_ch4_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_restart_ch4 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch4_st_clr:1;
        /** ledc_task_gamma_restart_ch5_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_restart_ch5 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch5_st_clr:1;
        /** ledc_task_gamma_pause_ch0_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_pause_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch0_st_clr:1;
        /** ledc_task_gamma_pause_ch1_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_pause_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch1_st_clr:1;
        /** ledc_task_gamma_pause_ch2_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_pause_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch2_st_clr:1;
        /** ledc_task_gamma_pause_ch3_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_pause_ch3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch3_st_clr:1;
        /** ledc_task_gamma_pause_ch4_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_pause_ch4 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch4_st_clr:1;
        /** ledc_task_gamma_pause_ch5_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_pause_ch5 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch5_st_clr:1;
        /** ledc_task_gamma_resume_ch0_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_resume_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch0_st_clr:1;
        /** ledc_task_gamma_resume_ch1_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_resume_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch1_st_clr:1;
        /** ledc_task_gamma_resume_ch2_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_resume_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch2_st_clr:1;
        /** ledc_task_gamma_resume_ch3_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_resume_ch3 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch3_st_clr:1;
        /** ledc_task_gamma_resume_ch4_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_resume_ch4 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch4_st_clr:1;
        /** ledc_task_gamma_resume_ch5_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear LEDC_task_gamma_resume_ch5 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch5_st_clr:1;
        /** tg0_task_cnt_start_timer0_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_start_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_start_timer0_st_clr:1;
        /** tg0_task_alarm_start_timer0_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear TG0_task_alarm_start_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_alarm_start_timer0_st_clr:1;
        /** tg0_task_cnt_stop_timer0_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_stop_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_stop_timer0_st_clr:1;
        /** tg0_task_cnt_reload_timer0_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_reload_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_reload_timer0_st_clr:1;
        /** tg0_task_cnt_cap_timer0_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_cap_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_cap_timer0_st_clr:1;
        /** tg0_task_cnt_start_timer1_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_start_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_start_timer1_st_clr:1;
        /** tg0_task_alarm_start_timer1_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear TG0_task_alarm_start_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_alarm_start_timer1_st_clr:1;
        /** tg0_task_cnt_stop_timer1_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_stop_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_stop_timer1_st_clr:1;
        /** tg0_task_cnt_reload_timer1_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_reload_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_reload_timer1_st_clr:1;
        /** tg0_task_cnt_cap_timer1_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear TG0_task_cnt_cap_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg0_task_cnt_cap_timer1_st_clr:1;
        /** tg1_task_cnt_start_timer0_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_start_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_start_timer0_st_clr:1;
        /** tg1_task_alarm_start_timer0_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear TG1_task_alarm_start_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_alarm_start_timer0_st_clr:1;
        /** tg1_task_cnt_stop_timer0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_stop_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_stop_timer0_st_clr:1;
        /** tg1_task_cnt_reload_timer0_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_reload_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_reload_timer0_st_clr:1;
        /** tg1_task_cnt_cap_timer0_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_cap_timer0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_cap_timer0_st_clr:1;
        /** tg1_task_cnt_start_timer1_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_start_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_start_timer1_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st2_clr_reg_t;

/** Type of task_st3_clr register
 *  Tasks trigger status clear register
 */
typedef union {
    struct {
        /** tg1_task_alarm_start_timer1_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear TG1_task_alarm_start_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_alarm_start_timer1_st_clr:1;
        /** tg1_task_cnt_stop_timer1_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_stop_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_stop_timer1_st_clr:1;
        /** tg1_task_cnt_reload_timer1_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_reload_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_reload_timer1_st_clr:1;
        /** tg1_task_cnt_cap_timer1_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear TG1_task_cnt_cap_timer1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tg1_task_cnt_cap_timer1_st_clr:1;
        /** mcpwm0_task_cmpr0_a_up_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cmpr0_a_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_cmpr0_a_up_st_clr:1;
        /** mcpwm0_task_cmpr1_a_up_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cmpr1_a_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_cmpr1_a_up_st_clr:1;
        /** mcpwm0_task_cmpr2_a_up_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cmpr2_a_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_cmpr2_a_up_st_clr:1;
        /** mcpwm0_task_cmpr0_b_up_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cmpr0_b_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_cmpr0_b_up_st_clr:1;
        /** mcpwm0_task_cmpr1_b_up_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cmpr1_b_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_cmpr1_b_up_st_clr:1;
        /** mcpwm0_task_cmpr2_b_up_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cmpr2_b_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_cmpr2_b_up_st_clr:1;
        /** mcpwm0_task_gen_stop_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_gen_stop trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_gen_stop_st_clr:1;
        /** mcpwm0_task_timer0_syn_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_timer0_syn trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_timer0_syn_st_clr:1;
        /** mcpwm0_task_timer1_syn_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_timer1_syn trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_timer1_syn_st_clr:1;
        /** mcpwm0_task_timer2_syn_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_timer2_syn trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_timer2_syn_st_clr:1;
        /** mcpwm0_task_timer0_period_up_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_timer0_period_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_timer0_period_up_st_clr:1;
        /** mcpwm0_task_timer1_period_up_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_timer1_period_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_timer1_period_up_st_clr:1;
        /** mcpwm0_task_timer2_period_up_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_timer2_period_up trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_timer2_period_up_st_clr:1;
        /** mcpwm0_task_tz0_ost_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_tz0_ost trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_task_tz0_ost_st_clr:1;
        /** mcpwm0_task_tz1_ost_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_tz1_ost trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_task_tz1_ost_st_clr:1;
        /** mcpwm0_task_tz2_ost_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_tz2_ost trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t mcpwm0_task_tz2_ost_st_clr:1;
        /** mcpwm0_task_clr0_ost_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_clr0_ost trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_clr0_ost_st_clr:1;
        /** mcpwm0_task_clr1_ost_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_clr1_ost trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_clr1_ost_st_clr:1;
        /** mcpwm0_task_clr2_ost_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_clr2_ost trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t mcpwm0_task_clr2_ost_st_clr:1;
        /** mcpwm0_task_cap0_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cap0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_task_cap0_st_clr:1;
        /** mcpwm0_task_cap1_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cap1 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_task_cap1_st_clr:1;
        /** mcpwm0_task_cap2_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear MCPWM0_task_cap2 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t mcpwm0_task_cap2_st_clr:1;
        /** adc_task_sample0_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear ADC_task_sample0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t adc_task_sample0_st_clr:1;
        /** adc_task_sample1_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear ADC_task_sample1 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t adc_task_sample1_st_clr:1;
        /** adc_task_start0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear ADC_task_start0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t adc_task_start0_st_clr:1;
        /** adc_task_stop0_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear ADC_task_stop0 trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t adc_task_stop0_st_clr:1;
        /** regdma_task_start0_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear REGDMA_task_start0 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t regdma_task_start0_st_clr:1;
        /** regdma_task_start1_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear REGDMA_task_start1 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t regdma_task_start1_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st3_clr_reg_t;

/** Type of task_st4_clr register
 *  Tasks trigger status clear register
 */
typedef union {
    struct {
        /** regdma_task_start2_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear REGDMA_task_start2 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t regdma_task_start2_st_clr:1;
        /** regdma_task_start3_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear REGDMA_task_start3 trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t regdma_task_start3_st_clr:1;
        /** tmpsnsr_task_start_sample_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear TMPSNSR_task_start_sample trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tmpsnsr_task_start_sample_st_clr:1;
        /** tmpsnsr_task_stop_sample_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear TMPSNSR_task_stop_sample trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t tmpsnsr_task_stop_sample_st_clr:1;
        /** i2s0_task_start_rx_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear I2S0_task_start_rx trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t i2s0_task_start_rx_st_clr:1;
        /** i2s0_task_start_tx_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear I2S0_task_start_tx trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t i2s0_task_start_tx_st_clr:1;
        /** i2s0_task_stop_rx_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear I2S0_task_stop_rx trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t i2s0_task_stop_rx_st_clr:1;
        /** i2s0_task_stop_tx_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear I2S0_task_stop_tx trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t i2s0_task_stop_tx_st_clr:1;
        /** ulp_task_wakeup_cpu_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear ULP_task_wakeup_cpu trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t ulp_task_wakeup_cpu_st_clr:1;
        /** ulp_task_int_cpu_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear ULP_task_int_cpu trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t ulp_task_int_cpu_st_clr:1;
        /** rtc_task_start_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear RTC_task_start trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t rtc_task_start_st_clr:1;
        /** rtc_task_stop_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear RTC_task_stop trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t rtc_task_stop_st_clr:1;
        /** rtc_task_clr_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear RTC_task_clr trigger status.\\0: Invalid, No
         *  effect\\1: Clear
         */
        uint32_t rtc_task_clr_st_clr:1;
        /** rtc_task_triggerflw_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear RTC_task_triggerflw trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t rtc_task_triggerflw_st_clr:1;
        /** gdma_task_in_start_ch0_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear GDMA_task_in_start_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_task_in_start_ch0_st_clr:1;
        /** gdma_task_in_start_ch1_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear GDMA_task_in_start_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_task_in_start_ch1_st_clr:1;
        /** gdma_task_in_start_ch2_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear GDMA_task_in_start_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_task_in_start_ch2_st_clr:1;
        /** gdma_task_out_start_ch0_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear GDMA_task_out_start_ch0 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_task_out_start_ch0_st_clr:1;
        /** gdma_task_out_start_ch1_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear GDMA_task_out_start_ch1 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_task_out_start_ch1_st_clr:1;
        /** gdma_task_out_start_ch2_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear GDMA_task_out_start_ch2 trigger status.\\0:
         *  Invalid, No effect\\1: Clear
         */
        uint32_t gdma_task_out_start_ch2_st_clr:1;
        /** pmu_task_sleep_req_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear PMU_task_sleep_req trigger status.\\0: Invalid,
         *  No effect\\1: Clear
         */
        uint32_t pmu_task_sleep_req_st_clr:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} soc_etm_task_st4_clr_reg_t;

/** Type of clk_en register
 *  ETM clock enable register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to open register clock gate.\\0: Open the clock gate only
         *  when application writes registers\\1: Force open the clock gate for register
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} soc_etm_clk_en_reg_t;


/** Group: Version Register */
/** Type of date register
 *  ETM date register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36770433;
         *  Configures the version.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} soc_etm_date_reg_t;


typedef struct {
    volatile soc_etm_ch_ena_ad0_reg_t ch_ena_ad0;
    volatile soc_etm_ch_ena_ad0_set_reg_t ch_ena_ad0_set;
    volatile soc_etm_ch_ena_ad0_clr_reg_t ch_ena_ad0_clr;
    volatile soc_etm_ch_ena_ad1_reg_t ch_ena_ad1;
    volatile soc_etm_ch_ena_ad1_set_reg_t ch_ena_ad1_set;
    volatile soc_etm_ch_ena_ad1_clr_reg_t ch_ena_ad1_clr;
    volatile soc_etm_chn_evt_id_reg_t ch0_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch0_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch1_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch1_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch2_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch2_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch3_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch3_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch4_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch4_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch5_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch5_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch6_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch6_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch7_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch7_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch8_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch8_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch9_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch9_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch10_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch10_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch11_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch11_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch12_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch12_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch13_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch13_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch14_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch14_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch15_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch15_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch16_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch16_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch17_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch17_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch18_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch18_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch19_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch19_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch20_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch20_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch21_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch21_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch22_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch22_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch23_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch23_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch24_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch24_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch25_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch25_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch26_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch26_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch27_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch27_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch28_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch28_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch29_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch29_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch30_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch30_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch31_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch31_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch32_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch32_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch33_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch33_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch34_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch34_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch35_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch35_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch36_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch36_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch37_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch37_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch38_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch38_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch39_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch39_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch40_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch40_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch41_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch41_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch42_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch42_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch43_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch43_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch44_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch44_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch45_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch45_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch46_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch46_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch47_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch47_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch48_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch48_task_id;
    volatile soc_etm_chn_evt_id_reg_t ch49_evt_id;
    volatile soc_etm_chn_task_id_reg_t ch49_task_id;
    volatile soc_etm_evt_st0_reg_t evt_st0;
    volatile soc_etm_evt_st0_clr_reg_t evt_st0_clr;
    volatile soc_etm_evt_st1_reg_t evt_st1;
    volatile soc_etm_evt_st1_clr_reg_t evt_st1_clr;
    volatile soc_etm_evt_st2_reg_t evt_st2;
    volatile soc_etm_evt_st2_clr_reg_t evt_st2_clr;
    volatile soc_etm_evt_st3_reg_t evt_st3;
    volatile soc_etm_evt_st3_clr_reg_t evt_st3_clr;
    volatile soc_etm_evt_st4_reg_t evt_st4;
    volatile soc_etm_evt_st4_clr_reg_t evt_st4_clr;
    volatile soc_etm_task_st0_reg_t task_st0;
    volatile soc_etm_task_st0_clr_reg_t task_st0_clr;
    volatile soc_etm_task_st1_reg_t task_st1;
    volatile soc_etm_task_st1_clr_reg_t task_st1_clr;
    volatile soc_etm_task_st2_reg_t task_st2;
    volatile soc_etm_task_st2_clr_reg_t task_st2_clr;
    volatile soc_etm_task_st3_reg_t task_st3;
    volatile soc_etm_task_st3_clr_reg_t task_st3_clr;
    volatile soc_etm_task_st4_reg_t task_st4;
    volatile soc_etm_task_st4_clr_reg_t task_st4_clr;
    volatile soc_etm_clk_en_reg_t clk_en;
    volatile soc_etm_date_reg_t date;
} soc_etm_dev_t;

extern soc_etm_dev_t SOC_ETM;

#ifndef __cplusplus
_Static_assert(sizeof(soc_etm_dev_t) == 0x200, "Invalid size of soc_etm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
