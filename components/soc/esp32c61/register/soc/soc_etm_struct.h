/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
        /** ch_enabled0 : R/WTC/WS; bitpos: [0]; default: 0;
         *  Represents channel0 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled0:1;
        /** ch_enabled1 : R/WTC/WS; bitpos: [1]; default: 0;
         *  Represents channel1 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled1:1;
        /** ch_enabled2 : R/WTC/WS; bitpos: [2]; default: 0;
         *  Represents channel2 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled2:1;
        /** ch_enabled3 : R/WTC/WS; bitpos: [3]; default: 0;
         *  Represents channel3 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled3:1;
        /** ch_enabled4 : R/WTC/WS; bitpos: [4]; default: 0;
         *  Represents channel4 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled4:1;
        /** ch_enabled5 : R/WTC/WS; bitpos: [5]; default: 0;
         *  Represents channel5 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled5:1;
        /** ch_enabled6 : R/WTC/WS; bitpos: [6]; default: 0;
         *  Represents channel6 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled6:1;
        /** ch_enabled7 : R/WTC/WS; bitpos: [7]; default: 0;
         *  Represents channel7 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled7:1;
        /** ch_enabled8 : R/WTC/WS; bitpos: [8]; default: 0;
         *  Represents channel8 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled8:1;
        /** ch_enabled9 : R/WTC/WS; bitpos: [9]; default: 0;
         *  Represents channel9 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled9:1;
        /** ch_enabled10 : R/WTC/WS; bitpos: [10]; default: 0;
         *  Represents channel10 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled10:1;
        /** ch_enabled11 : R/WTC/WS; bitpos: [11]; default: 0;
         *  Represents channel11 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled11:1;
        /** ch_enabled12 : R/WTC/WS; bitpos: [12]; default: 0;
         *  Represents channel12 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled12:1;
        /** ch_enabled13 : R/WTC/WS; bitpos: [13]; default: 0;
         *  Represents channel13 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled13:1;
        /** ch_enabled14 : R/WTC/WS; bitpos: [14]; default: 0;
         *  Represents channel14 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled14:1;
        /** ch_enabled15 : R/WTC/WS; bitpos: [15]; default: 0;
         *  Represents channel15 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled15:1;
        /** ch_enabled16 : R/WTC/WS; bitpos: [16]; default: 0;
         *  Represents channel16 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled16:1;
        /** ch_enabled17 : R/WTC/WS; bitpos: [17]; default: 0;
         *  Represents channel17 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled17:1;
        /** ch_enabled18 : R/WTC/WS; bitpos: [18]; default: 0;
         *  Represents channel18 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled18:1;
        /** ch_enabled19 : R/WTC/WS; bitpos: [19]; default: 0;
         *  Represents channel19 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled19:1;
        /** ch_enabled20 : R/WTC/WS; bitpos: [20]; default: 0;
         *  Represents channel20 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled20:1;
        /** ch_enabled21 : R/WTC/WS; bitpos: [21]; default: 0;
         *  Represents channel21 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled21:1;
        /** ch_enabled22 : R/WTC/WS; bitpos: [22]; default: 0;
         *  Represents channel22 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled22:1;
        /** ch_enabled23 : R/WTC/WS; bitpos: [23]; default: 0;
         *  Represents channel23 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled23:1;
        /** ch_enabled24 : R/WTC/WS; bitpos: [24]; default: 0;
         *  Represents channel24 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled24:1;
        /** ch_enabled25 : R/WTC/WS; bitpos: [25]; default: 0;
         *  Represents channel25 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled25:1;
        /** ch_enabled26 : R/WTC/WS; bitpos: [26]; default: 0;
         *  Represents channel26 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled26:1;
        /** ch_enabled27 : R/WTC/WS; bitpos: [27]; default: 0;
         *  Represents channel27 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled27:1;
        /** ch_enabled28 : R/WTC/WS; bitpos: [28]; default: 0;
         *  Represents channel28 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled28:1;
        /** ch_enabled29 : R/WTC/WS; bitpos: [29]; default: 0;
         *  Represents channel29 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled29:1;
        /** ch_enabled30 : R/WTC/WS; bitpos: [30]; default: 0;
         *  Represents channel30 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled30:1;
        /** ch_enabled31 : R/WTC/WS; bitpos: [31]; default: 0;
         *  Represents channel31 enable status.
         *  0: Disable
         *  1: Enable
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
        /** ch_enabled32 : R/WTC/WS; bitpos: [0]; default: 0;
         *  Represents channel32 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled32:1;
        /** ch_enabled33 : R/WTC/WS; bitpos: [1]; default: 0;
         *  Represents channel33 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled33:1;
        /** ch_enabled34 : R/WTC/WS; bitpos: [2]; default: 0;
         *  Represents channel34 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled34:1;
        /** ch_enabled35 : R/WTC/WS; bitpos: [3]; default: 0;
         *  Represents channel35 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled35:1;
        /** ch_enabled36 : R/WTC/WS; bitpos: [4]; default: 0;
         *  Represents channel36 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled36:1;
        /** ch_enabled37 : R/WTC/WS; bitpos: [5]; default: 0;
         *  Represents channel37 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled37:1;
        /** ch_enabled38 : R/WTC/WS; bitpos: [6]; default: 0;
         *  Represents channel38 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled38:1;
        /** ch_enabled39 : R/WTC/WS; bitpos: [7]; default: 0;
         *  Represents channel39 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled39:1;
        /** ch_enabled40 : R/WTC/WS; bitpos: [8]; default: 0;
         *  Represents channel40 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled40:1;
        /** ch_enabled41 : R/WTC/WS; bitpos: [9]; default: 0;
         *  Represents channel41 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled41:1;
        /** ch_enabled42 : R/WTC/WS; bitpos: [10]; default: 0;
         *  Represents channel42 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled42:1;
        /** ch_enabled43 : R/WTC/WS; bitpos: [11]; default: 0;
         *  Represents channel43 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled43:1;
        /** ch_enabled44 : R/WTC/WS; bitpos: [12]; default: 0;
         *  Represents channel44 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled44:1;
        /** ch_enabled45 : R/WTC/WS; bitpos: [13]; default: 0;
         *  Represents channel45 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled45:1;
        /** ch_enabled46 : R/WTC/WS; bitpos: [14]; default: 0;
         *  Represents channel46 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled46:1;
        /** ch_enabled47 : R/WTC/WS; bitpos: [15]; default: 0;
         *  Represents channel47 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled47:1;
        /** ch_enabled48 : R/WTC/WS; bitpos: [16]; default: 0;
         *  Represents channel48 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled48:1;
        /** ch_enabled49 : R/WTC/WS; bitpos: [17]; default: 0;
         *  Represents channel49 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_reg_t;

/** Type of evt_st0 register
 *  Event trigger status register
 */
typedef union {
    struct {
        /** gpio_evt_ch0_rise_edge_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents GPIO_EVT_CH0_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch0_rise_edge_st:1;
        /** gpio_evt_ch1_rise_edge_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents GPIO_EVT_CH1_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch1_rise_edge_st:1;
        /** gpio_evt_ch2_rise_edge_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents GPIO_EVT_CH2_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch2_rise_edge_st:1;
        /** gpio_evt_ch3_rise_edge_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents GPIO_EVT_CH3_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch3_rise_edge_st:1;
        /** gpio_evt_ch4_rise_edge_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents GPIO_EVT_CH4_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch4_rise_edge_st:1;
        /** gpio_evt_ch5_rise_edge_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents GPIO_EVT_CH5_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch5_rise_edge_st:1;
        /** gpio_evt_ch6_rise_edge_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents GPIO_EVT_CH6_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch6_rise_edge_st:1;
        /** gpio_evt_ch7_rise_edge_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents GPIO_EVT_CH7_RISE_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch7_rise_edge_st:1;
        /** gpio_evt_ch0_fall_edge_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents GPIO_EVT_CH0_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch0_fall_edge_st:1;
        /** gpio_evt_ch1_fall_edge_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents GPIO_EVT_CH1_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch1_fall_edge_st:1;
        /** gpio_evt_ch2_fall_edge_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents GPIO_EVT_CH2_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch2_fall_edge_st:1;
        /** gpio_evt_ch3_fall_edge_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents GPIO_EVT_CH3_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch3_fall_edge_st:1;
        /** gpio_evt_ch4_fall_edge_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents GPIO_EVT_CH4_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch4_fall_edge_st:1;
        /** gpio_evt_ch5_fall_edge_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents GPIO_EVT_CH5_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch5_fall_edge_st:1;
        /** gpio_evt_ch6_fall_edge_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents GPIO_EVT_CH6_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch6_fall_edge_st:1;
        /** gpio_evt_ch7_fall_edge_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents GPIO_EVT_CH7_FALL_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch7_fall_edge_st:1;
        /** gpio_evt_ch0_any_edge_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents GPIO_EVT_CH0_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch0_any_edge_st:1;
        /** gpio_evt_ch1_any_edge_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents GPIO_EVT_CH1_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch1_any_edge_st:1;
        /** gpio_evt_ch2_any_edge_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents GPIO_EVT_CH2_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch2_any_edge_st:1;
        /** gpio_evt_ch3_any_edge_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents GPIO_EVT_CH3_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch3_any_edge_st:1;
        /** gpio_evt_ch4_any_edge_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents GPIO_EVT_CH4_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch4_any_edge_st:1;
        /** gpio_evt_ch5_any_edge_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents GPIO_EVT_CH5_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch5_any_edge_st:1;
        /** gpio_evt_ch6_any_edge_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents GPIO_EVT_CH6_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch6_any_edge_st:1;
        /** gpio_evt_ch7_any_edge_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents GPIO_EVT_CH7_ANY_EDGE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_ch7_any_edge_st:1;
        /** gpio_evt_zero_det_pos0_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents GPIO_EVT_ZERO_DET_POS0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_zero_det_pos0_st:1;
        /** gpio_evt_zero_det_neg0_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents GPIO_EVT_ZERO_DET_NEG0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_evt_zero_det_neg0_st:1;
        uint32_t reserved_26:2;
        /** ledc_evt_duty_chng_end_ch0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents LEDC_EVT_DUTY_CHNG_END_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch0_st:1;
        /** ledc_evt_duty_chng_end_ch1_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents LEDC_EVT_DUTY_CHNG_END_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch1_st:1;
        /** ledc_evt_duty_chng_end_ch2_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents LEDC_EVT_DUTY_CHNG_END_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch2_st:1;
        /** ledc_evt_duty_chng_end_ch3_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents LEDC_EVT_DUTY_CHNG_END_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch3_st:1;
    };
    uint32_t val;
} soc_etm_evt_st0_reg_t;

/** Type of evt_st1 register
 *  Event trigger status register
 */
typedef union {
    struct {
        /** ledc_evt_duty_chng_end_ch4_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents LEDC_EVT_DUTY_CHNG_END_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch4_st:1;
        /** ledc_evt_duty_chng_end_ch5_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents LEDC_EVT_DUTY_CHNG_END_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_duty_chng_end_ch5_st:1;
        /** ledc_evt_ovf_cnt_pls_ch0_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents LEDC_EVT_OVF_CNT_PLS_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch0_st:1;
        /** ledc_evt_ovf_cnt_pls_ch1_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents LEDC_EVT_OVF_CNT_PLS_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch1_st:1;
        /** ledc_evt_ovf_cnt_pls_ch2_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents LEDC_EVT_OVF_CNT_PLS_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch2_st:1;
        /** ledc_evt_ovf_cnt_pls_ch3_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents LEDC_EVT_OVF_CNT_PLS_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch3_st:1;
        /** ledc_evt_ovf_cnt_pls_ch4_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents LEDC_EVT_OVF_CNT_PLS_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch4_st:1;
        /** ledc_evt_ovf_cnt_pls_ch5_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents LEDC_EVT_OVF_CNT_PLS_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch5_st:1;
        /** ledc_evt_time_ovf_timer0_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents LEDC_EVT_TIME_OVF_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer0_st:1;
        /** ledc_evt_time_ovf_timer1_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents LEDC_EVT_TIME_OVF_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer1_st:1;
        /** ledc_evt_time_ovf_timer2_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents LEDC_EVT_TIME_OVF_TIMER2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer2_st:1;
        /** ledc_evt_time_ovf_timer3_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents LEDC_EVT_TIME_OVF_TIMER3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_time_ovf_timer3_st:1;
        /** ledc_evt_timer0_cmp_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents LEDC_EVT_TIMER0_CMP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_timer0_cmp_st:1;
        /** ledc_evt_timer1_cmp_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents LEDC_EVT_TIMER1_CMP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_timer1_cmp_st:1;
        /** ledc_evt_timer2_cmp_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents LEDC_EVT_TIMER2_CMP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_timer2_cmp_st:1;
        /** ledc_evt_timer3_cmp_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents LEDC_EVT_TIMER3_CMP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_evt_timer3_cmp_st:1;
        /** tg0_evt_cnt_cmp_timer0_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents TG0_EVT_CNT_CMP_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_evt_cnt_cmp_timer0_st:1;
        /** tg0_evt_cnt_cmp_timer1_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents TG0_EVT_CNT_CMP_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_evt_cnt_cmp_timer1_st:1;
        /** tg1_evt_cnt_cmp_timer0_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents TG1_EVT_CNT_CMP_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_evt_cnt_cmp_timer0_st:1;
        /** tg1_evt_cnt_cmp_timer1_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents TG1_EVT_CNT_CMP_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_evt_cnt_cmp_timer1_st:1;
        /** systimer_evt_cnt_cmp0_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents SYSTIMER_EVT_CNT_CMP0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t systimer_evt_cnt_cmp0_st:1;
        /** systimer_evt_cnt_cmp1_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents SYSTIMER_EVT_CNT_CMP1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t systimer_evt_cnt_cmp1_st:1;
        /** systimer_evt_cnt_cmp2_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents SYSTIMER_EVT_CNT_CMP2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t systimer_evt_cnt_cmp2_st:1;
        /** adc_evt_conv_cmplt0_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents ADC_EVT_CONV_CMPLT0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_conv_cmplt0_st:1;
        /** adc_evt_eq_above_thresh0_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents ADC_EVT_EQ_ABOVE_THRESH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_eq_above_thresh0_st:1;
        /** adc_evt_eq_above_thresh1_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents ADC_EVT_EQ_ABOVE_THRESH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_eq_above_thresh1_st:1;
        /** adc_evt_eq_below_thresh0_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents ADC_EVT_EQ_BELOW_THRESH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_eq_below_thresh0_st:1;
        /** adc_evt_eq_below_thresh1_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents ADC_EVT_EQ_BELOW_THRESH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_eq_below_thresh1_st:1;
        /** adc_evt_result_done0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents ADC_EVT_RESULT_DONE0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_result_done0_st:1;
        /** adc_evt_stopped0_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents ADC_EVT_STOPPED0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_stopped0_st:1;
        /** adc_evt_started0_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents ADC_EVT_STARTED0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_evt_started0_st:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} soc_etm_evt_st1_reg_t;

/** Type of evt_st2 register
 *  Event trigger status register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** tmpsnsr_evt_over_limit_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents TMPSNSR_EVT_OVER_LIMIT trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tmpsnsr_evt_over_limit_st:1;
        /** i2s0_evt_rx_done_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents I2S0_EVT_RX_DONE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_evt_rx_done_st:1;
        /** i2s0_evt_tx_done_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents I2S0_EVT_TX_DONE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_evt_tx_done_st:1;
        /** i2s0_evt_x_words_received_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents I2S0_EVT_X_WORDS_RECEIVED trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_evt_x_words_received_st:1;
        /** i2s0_evt_x_words_sent_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents I2S0_EVT_X_WORDS_SENT trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_evt_x_words_sent_st:1;
        uint32_t reserved_12:7;
        /** rtc_evt_tick_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents RTC_EVT_TICK trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t rtc_evt_tick_st:1;
        /** rtc_evt_ovf_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents RTC_EVT_OVF trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t rtc_evt_ovf_st:1;
        /** rtc_evt_cmp_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents RTC_EVT_CMP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t rtc_evt_cmp_st:1;
        /** gdma_ahb_evt_in_done_ch0_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_DONE_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_done_ch0_st:1;
        /** gdma_ahb_evt_in_done_ch1_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_DONE_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_done_ch1_st:1;
        uint32_t reserved_24:1;
        /** gdma_ahb_evt_in_suc_eof_ch0_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_SUC_EOF_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_suc_eof_ch0_st:1;
        /** gdma_ahb_evt_in_suc_eof_ch1_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_SUC_EOF_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_suc_eof_ch1_st:1;
        uint32_t reserved_27:1;
        /** gdma_ahb_evt_in_fifo_empty_ch0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_FIFO_EMPTY_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_fifo_empty_ch0_st:1;
        /** gdma_ahb_evt_in_fifo_empty_ch1_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_FIFO_EMPTY_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_fifo_empty_ch1_st:1;
        uint32_t reserved_30:1;
        /** gdma_ahb_evt_in_fifo_full_ch0_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_FIFO_FULL_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_fifo_full_ch0_st:1;
    };
    uint32_t val;
} soc_etm_evt_st2_reg_t;

/** Type of evt_st3 register
 *  Event trigger status register
 */
typedef union {
    struct {
        /** gdma_ahb_evt_in_fifo_full_ch1_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents GDMA_AHB_EVT_IN_FIFO_FULL_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_in_fifo_full_ch1_st:1;
        uint32_t reserved_1:1;
        /** gdma_ahb_evt_out_done_ch0_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_DONE_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_done_ch0_st:1;
        /** gdma_ahb_evt_out_done_ch1_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_DONE_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_done_ch1_st:1;
        uint32_t reserved_4:1;
        /** gdma_ahb_evt_out_eof_ch0_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_EOF_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_eof_ch0_st:1;
        /** gdma_ahb_evt_out_eof_ch1_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_EOF_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_eof_ch1_st:1;
        uint32_t reserved_7:1;
        /** gdma_ahb_evt_out_total_eof_ch0_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_TOTAL_EOF_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_total_eof_ch0_st:1;
        /** gdma_ahb_evt_out_total_eof_ch1_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_TOTAL_EOF_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_total_eof_ch1_st:1;
        uint32_t reserved_10:1;
        /** gdma_ahb_evt_out_fifo_empty_ch0_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_fifo_empty_ch0_st:1;
        /** gdma_ahb_evt_out_fifo_empty_ch1_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_fifo_empty_ch1_st:1;
        uint32_t reserved_13:1;
        /** gdma_ahb_evt_out_fifo_full_ch0_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_FIFO_FULL_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_fifo_full_ch0_st:1;
        /** gdma_ahb_evt_out_fifo_full_ch1_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents GDMA_AHB_EVT_OUT_FIFO_FULL_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_evt_out_fifo_full_ch1_st:1;
        uint32_t reserved_16:1;
        /** pmu_evt_sleep_weekup_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents PMU_EVT_SLEEP_WEEKUP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t pmu_evt_sleep_weekup_st:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_evt_st3_reg_t;

/** Type of task_st0 register
 *  Task trigger status register
 */
typedef union {
    struct {
        /** gpio_task_ch0_set_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents GPIO_TASK_CH0_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch0_set_st:1;
        /** gpio_task_ch1_set_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents GPIO_TASK_CH1_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch1_set_st:1;
        /** gpio_task_ch2_set_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents GPIO_TASK_CH2_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch2_set_st:1;
        /** gpio_task_ch3_set_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents GPIO_TASK_CH3_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch3_set_st:1;
        /** gpio_task_ch4_set_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents GPIO_TASK_CH4_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch4_set_st:1;
        /** gpio_task_ch5_set_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents GPIO_TASK_CH5_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch5_set_st:1;
        /** gpio_task_ch6_set_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents GPIO_TASK_CH6_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch6_set_st:1;
        /** gpio_task_ch7_set_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents GPIO_TASK_CH7_SET trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch7_set_st:1;
        /** gpio_task_ch0_clear_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents GPIO_TASK_CH0_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch0_clear_st:1;
        /** gpio_task_ch1_clear_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents GPIO_TASK_CH1_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch1_clear_st:1;
        /** gpio_task_ch2_clear_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents GPIO_TASK_CH2_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch2_clear_st:1;
        /** gpio_task_ch3_clear_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents GPIO_TASK_CH3_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch3_clear_st:1;
        /** gpio_task_ch4_clear_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents GPIO_TASK_CH4_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch4_clear_st:1;
        /** gpio_task_ch5_clear_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents GPIO_TASK_CH5_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch5_clear_st:1;
        /** gpio_task_ch6_clear_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents GPIO_TASK_CH6_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch6_clear_st:1;
        /** gpio_task_ch7_clear_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents GPIO_TASK_CH7_CLEAR trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch7_clear_st:1;
        /** gpio_task_ch0_toggle_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents GPIO_TASK_CH0_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch0_toggle_st:1;
        /** gpio_task_ch1_toggle_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents GPIO_TASK_CH1_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch1_toggle_st:1;
        /** gpio_task_ch2_toggle_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents GPIO_TASK_CH2_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch2_toggle_st:1;
        /** gpio_task_ch3_toggle_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents GPIO_TASK_CH3_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch3_toggle_st:1;
        /** gpio_task_ch4_toggle_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents GPIO_TASK_CH4_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch4_toggle_st:1;
        /** gpio_task_ch5_toggle_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents GPIO_TASK_CH5_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch5_toggle_st:1;
        /** gpio_task_ch6_toggle_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents GPIO_TASK_CH6_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch6_toggle_st:1;
        /** gpio_task_ch7_toggle_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents GPIO_TASK_CH7_TOGGLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gpio_task_ch7_toggle_st:1;
        /** ledc_task_timer0_res_update_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents LEDC_TASK_TIMER0_RES_UPDATE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer0_res_update_st:1;
        /** ledc_task_timer1_res_update_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents LEDC_TASK_TIMER1_RES_UPDATE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer1_res_update_st:1;
        /** ledc_task_timer2_res_update_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents LEDC_TASK_TIMER2_RES_UPDATE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer2_res_update_st:1;
        /** ledc_task_timer3_res_update_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents LEDC_TASK_TIMER3_RES_UPDATE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer3_res_update_st:1;
        /** ledc_task_duty_scale_update_ch0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents LEDC_TASK_DUTY_SCALE_UPDATE_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch0_st:1;
        /** ledc_task_duty_scale_update_ch1_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents LEDC_TASK_DUTY_SCALE_UPDATE_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch1_st:1;
        /** ledc_task_duty_scale_update_ch2_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents LEDC_TASK_DUTY_SCALE_UPDATE_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch2_st:1;
        /** ledc_task_duty_scale_update_ch3_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents LEDC_TASK_DUTY_SCALE_UPDATE_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch3_st:1;
    };
    uint32_t val;
} soc_etm_task_st0_reg_t;

/** Type of task_st1 register
 *  Task trigger status register
 */
typedef union {
    struct {
        /** ledc_task_duty_scale_update_ch4_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents LEDC_TASK_DUTY_SCALE_UPDATE_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch4_st:1;
        /** ledc_task_duty_scale_update_ch5_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents LEDC_TASK_DUTY_SCALE_UPDATE_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_duty_scale_update_ch5_st:1;
        /** ledc_task_timer0_cap_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents LEDC_TASK_TIMER0_CAP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer0_cap_st:1;
        /** ledc_task_timer1_cap_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents LEDC_TASK_TIMER1_CAP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer1_cap_st:1;
        /** ledc_task_timer2_cap_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents LEDC_TASK_TIMER2_CAP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer2_cap_st:1;
        /** ledc_task_timer3_cap_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents LEDC_TASK_TIMER3_CAP trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer3_cap_st:1;
        /** ledc_task_sig_out_dis_ch0_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents LEDC_TASK_SIG_OUT_DIS_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch0_st:1;
        /** ledc_task_sig_out_dis_ch1_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents LEDC_TASK_SIG_OUT_DIS_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch1_st:1;
        /** ledc_task_sig_out_dis_ch2_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents LEDC_TASK_SIG_OUT_DIS_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch2_st:1;
        /** ledc_task_sig_out_dis_ch3_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents LEDC_TASK_SIG_OUT_DIS_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch3_st:1;
        /** ledc_task_sig_out_dis_ch4_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents LEDC_TASK_SIG_OUT_DIS_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch4_st:1;
        /** ledc_task_sig_out_dis_ch5_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents LEDC_TASK_SIG_OUT_DIS_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_sig_out_dis_ch5_st:1;
        /** ledc_task_ovf_cnt_rst_ch0_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents LEDC_TASK_OVF_CNT_RST_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch0_st:1;
        /** ledc_task_ovf_cnt_rst_ch1_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents LEDC_TASK_OVF_CNT_RST_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch1_st:1;
        /** ledc_task_ovf_cnt_rst_ch2_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents LEDC_TASK_OVF_CNT_RST_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch2_st:1;
        /** ledc_task_ovf_cnt_rst_ch3_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents LEDC_TASK_OVF_CNT_RST_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch3_st:1;
        /** ledc_task_ovf_cnt_rst_ch4_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents LEDC_TASK_OVF_CNT_RST_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch4_st:1;
        /** ledc_task_ovf_cnt_rst_ch5_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents LEDC_TASK_OVF_CNT_RST_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_ovf_cnt_rst_ch5_st:1;
        /** ledc_task_timer0_rst_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents LEDC_TASK_TIMER0_RST trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer0_rst_st:1;
        /** ledc_task_timer1_rst_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents LEDC_TASK_TIMER1_RST trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer1_rst_st:1;
        /** ledc_task_timer2_rst_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents LEDC_TASK_TIMER2_RST trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer2_rst_st:1;
        /** ledc_task_timer3_rst_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents LEDC_TASK_TIMER3_RST trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer3_rst_st:1;
        /** ledc_task_timer0_resume_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents LEDC_TASK_TIMER0_RESUME trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer0_resume_st:1;
        /** ledc_task_timer1_resume_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents LEDC_TASK_TIMER1_RESUME trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer1_resume_st:1;
        /** ledc_task_timer2_resume_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents LEDC_TASK_TIMER2_RESUME trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer2_resume_st:1;
        /** ledc_task_timer3_resume_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents LEDC_TASK_TIMER3_RESUME trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer3_resume_st:1;
        /** ledc_task_timer0_pause_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents LEDC_TASK_TIMER0_PAUSE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer0_pause_st:1;
        /** ledc_task_timer1_pause_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents LEDC_TASK_TIMER1_PAUSE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer1_pause_st:1;
        /** ledc_task_timer2_pause_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents LEDC_TASK_TIMER2_PAUSE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer2_pause_st:1;
        /** ledc_task_timer3_pause_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents LEDC_TASK_TIMER3_PAUSE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_timer3_pause_st:1;
        /** ledc_task_gamma_restart_ch0_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESTART_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_restart_ch0_st:1;
        /** ledc_task_gamma_restart_ch1_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESTART_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_restart_ch1_st:1;
    };
    uint32_t val;
} soc_etm_task_st1_reg_t;

/** Type of task_st2 register
 *  Task trigger status register
 */
typedef union {
    struct {
        /** ledc_task_gamma_restart_ch2_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESTART_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_restart_ch2_st:1;
        /** ledc_task_gamma_restart_ch3_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESTART_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_restart_ch3_st:1;
        /** ledc_task_gamma_restart_ch4_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESTART_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_restart_ch4_st:1;
        /** ledc_task_gamma_restart_ch5_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESTART_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_restart_ch5_st:1;
        /** ledc_task_gamma_pause_ch0_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents LEDC_TASK_GAMMA_PAUSE_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch0_st:1;
        /** ledc_task_gamma_pause_ch1_st : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents LEDC_TASK_GAMMA_PAUSE_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch1_st:1;
        /** ledc_task_gamma_pause_ch2_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents LEDC_TASK_GAMMA_PAUSE_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch2_st:1;
        /** ledc_task_gamma_pause_ch3_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents LEDC_TASK_GAMMA_PAUSE_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch3_st:1;
        /** ledc_task_gamma_pause_ch4_st : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents LEDC_TASK_GAMMA_PAUSE_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch4_st:1;
        /** ledc_task_gamma_pause_ch5_st : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents LEDC_TASK_GAMMA_PAUSE_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_pause_ch5_st:1;
        /** ledc_task_gamma_resume_ch0_st : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESUME_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_resume_ch0_st:1;
        /** ledc_task_gamma_resume_ch1_st : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESUME_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_resume_ch1_st:1;
        /** ledc_task_gamma_resume_ch2_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESUME_CH2 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_resume_ch2_st:1;
        /** ledc_task_gamma_resume_ch3_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESUME_CH3 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_resume_ch3_st:1;
        /** ledc_task_gamma_resume_ch4_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESUME_CH4 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_resume_ch4_st:1;
        /** ledc_task_gamma_resume_ch5_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents LEDC_TASK_GAMMA_RESUME_CH5 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t ledc_task_gamma_resume_ch5_st:1;
        /** tg0_task_cnt_start_timer0_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents TG0_TASK_CNT_START_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_start_timer0_st:1;
        /** tg0_task_alarm_start_timer0_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents TG0_TASK_ALARM_START_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_alarm_start_timer0_st:1;
        /** tg0_task_cnt_stop_timer0_st : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents TG0_TASK_CNT_STOP_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_stop_timer0_st:1;
        /** tg0_task_cnt_reload_timer0_st : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents TG0_TASK_CNT_RELOAD_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_reload_timer0_st:1;
        /** tg0_task_cnt_cap_timer0_st : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents TG0_TASK_CNT_CAP_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_cap_timer0_st:1;
        /** tg0_task_cnt_start_timer1_st : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents TG0_TASK_CNT_START_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_start_timer1_st:1;
        /** tg0_task_alarm_start_timer1_st : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents TG0_TASK_ALARM_START_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_alarm_start_timer1_st:1;
        /** tg0_task_cnt_stop_timer1_st : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents TG0_TASK_CNT_STOP_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_stop_timer1_st:1;
        /** tg0_task_cnt_reload_timer1_st : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents TG0_TASK_CNT_RELOAD_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_reload_timer1_st:1;
        /** tg0_task_cnt_cap_timer1_st : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents TG0_TASK_CNT_CAP_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg0_task_cnt_cap_timer1_st:1;
        /** tg1_task_cnt_start_timer0_st : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents TG1_TASK_CNT_START_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_start_timer0_st:1;
        /** tg1_task_alarm_start_timer0_st : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents TG1_TASK_ALARM_START_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_alarm_start_timer0_st:1;
        /** tg1_task_cnt_stop_timer0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents TG1_TASK_CNT_STOP_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_stop_timer0_st:1;
        /** tg1_task_cnt_reload_timer0_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents TG1_TASK_CNT_RELOAD_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_reload_timer0_st:1;
        /** tg1_task_cnt_cap_timer0_st : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents TG1_TASK_CNT_CAP_TIMER0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_cap_timer0_st:1;
        /** tg1_task_cnt_start_timer1_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents TG1_TASK_CNT_START_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_start_timer1_st:1;
    };
    uint32_t val;
} soc_etm_task_st2_reg_t;

/** Type of task_st3 register
 *  Task trigger status register
 */
typedef union {
    struct {
        /** tg1_task_alarm_start_timer1_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents TG1_TASK_ALARM_START_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_alarm_start_timer1_st:1;
        /** tg1_task_cnt_stop_timer1_st : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents TG1_TASK_CNT_STOP_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_stop_timer1_st:1;
        /** tg1_task_cnt_reload_timer1_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents TG1_TASK_CNT_RELOAD_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_reload_timer1_st:1;
        /** tg1_task_cnt_cap_timer1_st : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents TG1_TASK_CNT_CAP_TIMER1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tg1_task_cnt_cap_timer1_st:1;
        /** adc_task_sample0_st : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents ADC_TASK_SAMPLE0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_task_sample0_st:1;
        uint32_t reserved_5:1;
        /** adc_task_start0_st : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents ADC_TASK_START0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_task_start0_st:1;
        /** adc_task_stop0_st : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents ADC_TASK_STOP0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t adc_task_stop0_st:1;
        uint32_t reserved_8:4;
        /** tmpsnsr_task_start_sample_st : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents TMPSNSR_TASK_START_SAMPLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tmpsnsr_task_start_sample_st:1;
        /** tmpsnsr_task_stop_sample_st : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents TMPSNSR_TASK_STOP_SAMPLE trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t tmpsnsr_task_stop_sample_st:1;
        /** i2s0_task_start_rx_st : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents I2S0_TASK_START_RX trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_task_start_rx_st:1;
        /** i2s0_task_start_tx_st : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents I2S0_TASK_START_TX trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_task_start_tx_st:1;
        /** i2s0_task_stop_rx_st : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents I2S0_TASK_STOP_RX trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_task_stop_rx_st:1;
        /** i2s0_task_stop_tx_st : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents I2S0_TASK_STOP_TX trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t i2s0_task_stop_tx_st:1;
        uint32_t reserved_18:10;
        /** gdma_ahb_task_in_start_ch0_st : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents GDMA_AHB_TASK_IN_START_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_task_in_start_ch0_st:1;
        /** gdma_ahb_task_in_start_ch1_st : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents GDMA_AHB_TASK_IN_START_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_task_in_start_ch1_st:1;
        uint32_t reserved_30:1;
        /** gdma_ahb_task_out_start_ch0_st : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents GDMA_AHB_TASK_OUT_START_CH0 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_task_out_start_ch0_st:1;
    };
    uint32_t val;
} soc_etm_task_st3_reg_t;

/** Type of task_st4 register
 *  Task trigger status register
 */
typedef union {
    struct {
        /** gdma_ahb_task_out_start_ch1_st : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents GDMA_AHB_TASK_OUT_START_CH1 trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t gdma_ahb_task_out_start_ch1_st:1;
        uint32_t reserved_1:1;
        /** pmu_task_sleep_req_st : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents PMU_TASK_SLEEP_REQ trigger status.
         *  0: Not triggered
         *  1: Triggered
         */
        uint32_t pmu_task_sleep_req_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} soc_etm_task_st4_reg_t;


/** Group: Configuration Register */
/** Type of ch_ena_ad0_set register
 *  Channel enable register
 */
typedef union {
    struct {
        /** ch_enable0 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable channel0.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable0:1;
        /** ch_enable1 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to enable channel1.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable1:1;
        /** ch_enable2 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to enable channel2.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable2:1;
        /** ch_enable3 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to enable channel3.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable3:1;
        /** ch_enable4 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to enable channel4.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable4:1;
        /** ch_enable5 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to enable channel5.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable5:1;
        /** ch_enable6 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to enable channel6.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable6:1;
        /** ch_enable7 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to enable channel7.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable7:1;
        /** ch_enable8 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to enable channel8.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable8:1;
        /** ch_enable9 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to enable channel9.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable9:1;
        /** ch_enable10 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to enable channel10.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable10:1;
        /** ch_enable11 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to enable channel11.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable11:1;
        /** ch_enable12 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to enable channel12.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable12:1;
        /** ch_enable13 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to enable channel13.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable13:1;
        /** ch_enable14 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to enable channel14.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable14:1;
        /** ch_enable15 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to enable channel15.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable15:1;
        /** ch_enable16 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to enable channel16.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable16:1;
        /** ch_enable17 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to enable channel17.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable17:1;
        /** ch_enable18 : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to enable channel18.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable18:1;
        /** ch_enable19 : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to enable channel19.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable19:1;
        /** ch_enable20 : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to enable channel20.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable20:1;
        /** ch_enable21 : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to enable channel21.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable21:1;
        /** ch_enable22 : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to enable channel22.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable22:1;
        /** ch_enable23 : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to enable channel23.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable23:1;
        /** ch_enable24 : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to enable channel24.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable24:1;
        /** ch_enable25 : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to enable channel25.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable25:1;
        /** ch_enable26 : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to enable channel26.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable26:1;
        /** ch_enable27 : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to enable channel27.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable27:1;
        /** ch_enable28 : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to enable channel28.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable28:1;
        /** ch_enable29 : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to enable channel29.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable29:1;
        /** ch_enable30 : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to enable channel30.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable30:1;
        /** ch_enable31 : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to enable channel31.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_set_reg_t;

/** Type of ch_ena_ad0_clr register
 *  Channel disable register
 */
typedef union {
    struct {
        /** ch_disable0 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to disable channel0.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable0:1;
        /** ch_disable1 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to disable channel1.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable1:1;
        /** ch_disable2 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to disable channel2.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable2:1;
        /** ch_disable3 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to disable channel3.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable3:1;
        /** ch_disable4 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to disable channel4.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable4:1;
        /** ch_disable5 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to disable channel5.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable5:1;
        /** ch_disable6 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to disable channel6.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable6:1;
        /** ch_disable7 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to disable channel7.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable7:1;
        /** ch_disable8 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to disable channel8.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable8:1;
        /** ch_disable9 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to disable channel9.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable9:1;
        /** ch_disable10 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to disable channel10.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable10:1;
        /** ch_disable11 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to disable channel11.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable11:1;
        /** ch_disable12 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to disable channel12.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable12:1;
        /** ch_disable13 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to disable channel13.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable13:1;
        /** ch_disable14 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to disable channel14.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable14:1;
        /** ch_disable15 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to disable channel15.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable15:1;
        /** ch_disable16 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to disable channel16.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable16:1;
        /** ch_disable17 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to disable channel17.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable17:1;
        /** ch_disable18 : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to disable channel18.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable18:1;
        /** ch_disable19 : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to disable channel19.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable19:1;
        /** ch_disable20 : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to disable channel20.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable20:1;
        /** ch_disable21 : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to disable channel21.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable21:1;
        /** ch_disable22 : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to disable channel22.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable22:1;
        /** ch_disable23 : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to disable channel23.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable23:1;
        /** ch_disable24 : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to disable channel24.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable24:1;
        /** ch_disable25 : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to disable channel25.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable25:1;
        /** ch_disable26 : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to disable channel26.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable26:1;
        /** ch_disable27 : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to disable channel27.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable27:1;
        /** ch_disable28 : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to disable channel28.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable28:1;
        /** ch_disable29 : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to disable channel29.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable29:1;
        /** ch_disable30 : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to disable channel30.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable30:1;
        /** ch_disable31 : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to disable channel31.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_clr_reg_t;

/** Type of ch_ena_ad1_set register
 *  Channel enable register
 */
typedef union {
    struct {
        /** ch_enable32 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable channel32.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable32:1;
        /** ch_enable33 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to enable channel33.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable33:1;
        /** ch_enable34 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to enable channel34.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable34:1;
        /** ch_enable35 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to enable channel35.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable35:1;
        /** ch_enable36 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to enable channel36.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable36:1;
        /** ch_enable37 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to enable channel37.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable37:1;
        /** ch_enable38 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to enable channel38.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable38:1;
        /** ch_enable39 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to enable channel39.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable39:1;
        /** ch_enable40 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to enable channel40.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable40:1;
        /** ch_enable41 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to enable channel41.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable41:1;
        /** ch_enable42 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to enable channel42.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable42:1;
        /** ch_enable43 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to enable channel43.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable43:1;
        /** ch_enable44 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to enable channel44.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable44:1;
        /** ch_enable45 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to enable channel45.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable45:1;
        /** ch_enable46 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to enable channel46.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable46:1;
        /** ch_enable47 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to enable channel47.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable47:1;
        /** ch_enable48 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to enable channel48.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable48:1;
        /** ch_enable49 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to enable channel49.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_set_reg_t;

/** Type of ch_ena_ad1_clr register
 *  Channel disable register
 */
typedef union {
    struct {
        /** ch_disable32 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to disable channel32.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable32:1;
        /** ch_disable33 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to disable channel33.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable33:1;
        /** ch_disable34 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to disable channel34.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable34:1;
        /** ch_disable35 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to disable channel35.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable35:1;
        /** ch_disable36 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to disable channel36.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable36:1;
        /** ch_disable37 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to disable channel37.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable37:1;
        /** ch_disable38 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to disable channel38.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable38:1;
        /** ch_disable39 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to disable channel39.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable39:1;
        /** ch_disable40 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to disable channel40.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable40:1;
        /** ch_disable41 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to disable channel41.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable41:1;
        /** ch_disable42 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to disable channel42.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable42:1;
        /** ch_disable43 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to disable channel43.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable43:1;
        /** ch_disable44 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to disable channel44.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable44:1;
        /** ch_disable45 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to disable channel45.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable45:1;
        /** ch_disable46 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to disable channel46.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable46:1;
        /** ch_disable47 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to disable channel47.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable47:1;
        /** ch_disable48 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to disable channel48.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable48:1;
        /** ch_disable49 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to disable channel49.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_clr_reg_t;

/** Type of chn_evt_id register
 *  Channeln event ID register
 */
typedef union {
    struct {
        /** chn_evt_id : R/W; bitpos: [6:0]; default: 0;
         *  Configures channeln event ID.
         */
        uint32_t chn_evt_id:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} soc_etm_chn_evt_id_reg_t;

/** Type of chn_task_id register
 *  Channeln task ID register
 */
typedef union {
    struct {
        /** chn_task_id : R/W; bitpos: [7:0]; default: 0;
         *  Configures channeln task ID.
         */
        uint32_t chn_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_chn_task_id_reg_t;

/** Type of evt_st0_clr register
 *  Event trigger status clear register
 */
typedef union {
    struct {
        /** gpio_evt_ch0_rise_edge_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH0_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch0_rise_edge_st_clr:1;
        /** gpio_evt_ch1_rise_edge_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH1_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch1_rise_edge_st_clr:1;
        /** gpio_evt_ch2_rise_edge_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH2_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch2_rise_edge_st_clr:1;
        /** gpio_evt_ch3_rise_edge_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH3_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch3_rise_edge_st_clr:1;
        /** gpio_evt_ch4_rise_edge_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH4_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch4_rise_edge_st_clr:1;
        /** gpio_evt_ch5_rise_edge_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH5_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch5_rise_edge_st_clr:1;
        /** gpio_evt_ch6_rise_edge_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH6_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch6_rise_edge_st_clr:1;
        /** gpio_evt_ch7_rise_edge_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH7_RISE_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch7_rise_edge_st_clr:1;
        /** gpio_evt_ch0_fall_edge_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH0_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch0_fall_edge_st_clr:1;
        /** gpio_evt_ch1_fall_edge_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH1_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch1_fall_edge_st_clr:1;
        /** gpio_evt_ch2_fall_edge_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH2_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch2_fall_edge_st_clr:1;
        /** gpio_evt_ch3_fall_edge_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH3_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch3_fall_edge_st_clr:1;
        /** gpio_evt_ch4_fall_edge_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH4_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch4_fall_edge_st_clr:1;
        /** gpio_evt_ch5_fall_edge_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH5_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch5_fall_edge_st_clr:1;
        /** gpio_evt_ch6_fall_edge_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH6_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch6_fall_edge_st_clr:1;
        /** gpio_evt_ch7_fall_edge_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH7_FALL_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch7_fall_edge_st_clr:1;
        /** gpio_evt_ch0_any_edge_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH0_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch0_any_edge_st_clr:1;
        /** gpio_evt_ch1_any_edge_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH1_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch1_any_edge_st_clr:1;
        /** gpio_evt_ch2_any_edge_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH2_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch2_any_edge_st_clr:1;
        /** gpio_evt_ch3_any_edge_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH3_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch3_any_edge_st_clr:1;
        /** gpio_evt_ch4_any_edge_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH4_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch4_any_edge_st_clr:1;
        /** gpio_evt_ch5_any_edge_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH5_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch5_any_edge_st_clr:1;
        /** gpio_evt_ch6_any_edge_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH6_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch6_any_edge_st_clr:1;
        /** gpio_evt_ch7_any_edge_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_CH7_ANY_EDGE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_ch7_any_edge_st_clr:1;
        /** gpio_evt_zero_det_pos0_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_ZERO_DET_POS0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_zero_det_pos0_st_clr:1;
        /** gpio_evt_zero_det_neg0_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear GPIO_EVT_ZERO_DET_NEG0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_evt_zero_det_neg0_st_clr:1;
        uint32_t reserved_26:2;
        /** ledc_evt_duty_chng_end_ch0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_DUTY_CHNG_END_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch0_st_clr:1;
        /** ledc_evt_duty_chng_end_ch1_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_DUTY_CHNG_END_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch1_st_clr:1;
        /** ledc_evt_duty_chng_end_ch2_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_DUTY_CHNG_END_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch2_st_clr:1;
        /** ledc_evt_duty_chng_end_ch3_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_DUTY_CHNG_END_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch3_st_clr:1;
    };
    uint32_t val;
} soc_etm_evt_st0_clr_reg_t;

/** Type of evt_st1_clr register
 *  Event trigger status clear register
 */
typedef union {
    struct {
        /** ledc_evt_duty_chng_end_ch4_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_DUTY_CHNG_END_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch4_st_clr:1;
        /** ledc_evt_duty_chng_end_ch5_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_DUTY_CHNG_END_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_duty_chng_end_ch5_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch0_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_OVF_CNT_PLS_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch0_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch1_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_OVF_CNT_PLS_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch1_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch2_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_OVF_CNT_PLS_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch2_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch3_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_OVF_CNT_PLS_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch3_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch4_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_OVF_CNT_PLS_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch4_st_clr:1;
        /** ledc_evt_ovf_cnt_pls_ch5_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_OVF_CNT_PLS_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_ovf_cnt_pls_ch5_st_clr:1;
        /** ledc_evt_time_ovf_timer0_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIME_OVF_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer0_st_clr:1;
        /** ledc_evt_time_ovf_timer1_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIME_OVF_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer1_st_clr:1;
        /** ledc_evt_time_ovf_timer2_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIME_OVF_TIMER2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer2_st_clr:1;
        /** ledc_evt_time_ovf_timer3_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIME_OVF_TIMER3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_time_ovf_timer3_st_clr:1;
        /** ledc_evt_timer0_cmp_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIMER0_CMP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_timer0_cmp_st_clr:1;
        /** ledc_evt_timer1_cmp_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIMER1_CMP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_timer1_cmp_st_clr:1;
        /** ledc_evt_timer2_cmp_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIMER2_CMP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_timer2_cmp_st_clr:1;
        /** ledc_evt_timer3_cmp_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear LEDC_EVT_TIMER3_CMP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_evt_timer3_cmp_st_clr:1;
        /** tg0_evt_cnt_cmp_timer0_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear TG0_EVT_CNT_CMP_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_evt_cnt_cmp_timer0_st_clr:1;
        /** tg0_evt_cnt_cmp_timer1_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear TG0_EVT_CNT_CMP_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_evt_cnt_cmp_timer1_st_clr:1;
        /** tg1_evt_cnt_cmp_timer0_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear TG1_EVT_CNT_CMP_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_evt_cnt_cmp_timer0_st_clr:1;
        /** tg1_evt_cnt_cmp_timer1_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear TG1_EVT_CNT_CMP_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_evt_cnt_cmp_timer1_st_clr:1;
        /** systimer_evt_cnt_cmp0_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear SYSTIMER_EVT_CNT_CMP0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t systimer_evt_cnt_cmp0_st_clr:1;
        /** systimer_evt_cnt_cmp1_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear SYSTIMER_EVT_CNT_CMP1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t systimer_evt_cnt_cmp1_st_clr:1;
        /** systimer_evt_cnt_cmp2_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear SYSTIMER_EVT_CNT_CMP2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t systimer_evt_cnt_cmp2_st_clr:1;
        /** adc_evt_conv_cmplt0_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear ADC_EVT_CONV_CMPLT0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_conv_cmplt0_st_clr:1;
        /** adc_evt_eq_above_thresh0_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear ADC_EVT_EQ_ABOVE_THRESH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_eq_above_thresh0_st_clr:1;
        /** adc_evt_eq_above_thresh1_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear ADC_EVT_EQ_ABOVE_THRESH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_eq_above_thresh1_st_clr:1;
        /** adc_evt_eq_below_thresh0_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear ADC_EVT_EQ_BELOW_THRESH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_eq_below_thresh0_st_clr:1;
        /** adc_evt_eq_below_thresh1_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear ADC_EVT_EQ_BELOW_THRESH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_eq_below_thresh1_st_clr:1;
        /** adc_evt_result_done0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear ADC_EVT_RESULT_DONE0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_result_done0_st_clr:1;
        /** adc_evt_stopped0_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear ADC_EVT_STOPPED0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_stopped0_st_clr:1;
        /** adc_evt_started0_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear ADC_EVT_STARTED0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_evt_started0_st_clr:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} soc_etm_evt_st1_clr_reg_t;

/** Type of evt_st2_clr register
 *  Event trigger status clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** tmpsnsr_evt_over_limit_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear TMPSNSR_EVT_OVER_LIMIT trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tmpsnsr_evt_over_limit_st_clr:1;
        /** i2s0_evt_rx_done_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear I2S0_EVT_RX_DONE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_evt_rx_done_st_clr:1;
        /** i2s0_evt_tx_done_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear I2S0_EVT_TX_DONE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_evt_tx_done_st_clr:1;
        /** i2s0_evt_x_words_received_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear I2S0_EVT_X_WORDS_RECEIVED trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_evt_x_words_received_st_clr:1;
        /** i2s0_evt_x_words_sent_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear I2S0_EVT_X_WORDS_SENT trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_evt_x_words_sent_st_clr:1;
        uint32_t reserved_12:7;
        /** rtc_evt_tick_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear RTC_EVT_TICK trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t rtc_evt_tick_st_clr:1;
        /** rtc_evt_ovf_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear RTC_EVT_OVF trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t rtc_evt_ovf_st_clr:1;
        /** rtc_evt_cmp_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear RTC_EVT_CMP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t rtc_evt_cmp_st_clr:1;
        /** gdma_ahb_evt_in_done_ch0_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_DONE_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_done_ch0_st_clr:1;
        /** gdma_ahb_evt_in_done_ch1_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_DONE_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_done_ch1_st_clr:1;
        uint32_t reserved_24:1;
        /** gdma_ahb_evt_in_suc_eof_ch0_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_SUC_EOF_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_suc_eof_ch0_st_clr:1;
        /** gdma_ahb_evt_in_suc_eof_ch1_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_SUC_EOF_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_suc_eof_ch1_st_clr:1;
        uint32_t reserved_27:1;
        /** gdma_ahb_evt_in_fifo_empty_ch0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_FIFO_EMPTY_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_fifo_empty_ch0_st_clr:1;
        /** gdma_ahb_evt_in_fifo_empty_ch1_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_FIFO_EMPTY_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_fifo_empty_ch1_st_clr:1;
        uint32_t reserved_30:1;
        /** gdma_ahb_evt_in_fifo_full_ch0_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_FIFO_FULL_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_fifo_full_ch0_st_clr:1;
    };
    uint32_t val;
} soc_etm_evt_st2_clr_reg_t;

/** Type of evt_st3_clr register
 *  Event trigger status clear register
 */
typedef union {
    struct {
        /** gdma_ahb_evt_in_fifo_full_ch1_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_IN_FIFO_FULL_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_in_fifo_full_ch1_st_clr:1;
        uint32_t reserved_1:1;
        /** gdma_ahb_evt_out_done_ch0_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_DONE_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_done_ch0_st_clr:1;
        /** gdma_ahb_evt_out_done_ch1_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_DONE_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_done_ch1_st_clr:1;
        uint32_t reserved_4:1;
        /** gdma_ahb_evt_out_eof_ch0_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_EOF_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_eof_ch0_st_clr:1;
        /** gdma_ahb_evt_out_eof_ch1_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_EOF_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_eof_ch1_st_clr:1;
        uint32_t reserved_7:1;
        /** gdma_ahb_evt_out_total_eof_ch0_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_TOTAL_EOF_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_total_eof_ch0_st_clr:1;
        /** gdma_ahb_evt_out_total_eof_ch1_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_TOTAL_EOF_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_total_eof_ch1_st_clr:1;
        uint32_t reserved_10:1;
        /** gdma_ahb_evt_out_fifo_empty_ch0_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_FIFO_EMPTY_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_fifo_empty_ch0_st_clr:1;
        /** gdma_ahb_evt_out_fifo_empty_ch1_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_FIFO_EMPTY_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_fifo_empty_ch1_st_clr:1;
        uint32_t reserved_13:1;
        /** gdma_ahb_evt_out_fifo_full_ch0_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_FIFO_FULL_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_fifo_full_ch0_st_clr:1;
        /** gdma_ahb_evt_out_fifo_full_ch1_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_EVT_OUT_FIFO_FULL_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_evt_out_fifo_full_ch1_st_clr:1;
        uint32_t reserved_16:1;
        /** pmu_evt_sleep_weekup_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear PMU_EVT_SLEEP_WEEKUP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t pmu_evt_sleep_weekup_st_clr:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_evt_st3_clr_reg_t;

/** Type of task_st0_clr register
 *  Task trigger status clear register
 */
typedef union {
    struct {
        /** gpio_task_ch0_set_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH0_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch0_set_st_clr:1;
        /** gpio_task_ch1_set_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH1_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch1_set_st_clr:1;
        /** gpio_task_ch2_set_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH2_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch2_set_st_clr:1;
        /** gpio_task_ch3_set_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH3_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch3_set_st_clr:1;
        /** gpio_task_ch4_set_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH4_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch4_set_st_clr:1;
        /** gpio_task_ch5_set_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH5_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch5_set_st_clr:1;
        /** gpio_task_ch6_set_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH6_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch6_set_st_clr:1;
        /** gpio_task_ch7_set_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH7_SET trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch7_set_st_clr:1;
        /** gpio_task_ch0_clear_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH0_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch0_clear_st_clr:1;
        /** gpio_task_ch1_clear_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH1_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch1_clear_st_clr:1;
        /** gpio_task_ch2_clear_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH2_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch2_clear_st_clr:1;
        /** gpio_task_ch3_clear_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH3_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch3_clear_st_clr:1;
        /** gpio_task_ch4_clear_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH4_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch4_clear_st_clr:1;
        /** gpio_task_ch5_clear_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH5_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch5_clear_st_clr:1;
        /** gpio_task_ch6_clear_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH6_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch6_clear_st_clr:1;
        /** gpio_task_ch7_clear_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH7_CLEAR trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch7_clear_st_clr:1;
        /** gpio_task_ch0_toggle_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH0_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch0_toggle_st_clr:1;
        /** gpio_task_ch1_toggle_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH1_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch1_toggle_st_clr:1;
        /** gpio_task_ch2_toggle_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH2_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch2_toggle_st_clr:1;
        /** gpio_task_ch3_toggle_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH3_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch3_toggle_st_clr:1;
        /** gpio_task_ch4_toggle_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH4_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch4_toggle_st_clr:1;
        /** gpio_task_ch5_toggle_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH5_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch5_toggle_st_clr:1;
        /** gpio_task_ch6_toggle_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH6_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch6_toggle_st_clr:1;
        /** gpio_task_ch7_toggle_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear GPIO_TASK_CH7_TOGGLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gpio_task_ch7_toggle_st_clr:1;
        /** ledc_task_timer0_res_update_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER0_RES_UPDATE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer0_res_update_st_clr:1;
        /** ledc_task_timer1_res_update_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER1_RES_UPDATE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer1_res_update_st_clr:1;
        /** ledc_task_timer2_res_update_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER2_RES_UPDATE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer2_res_update_st_clr:1;
        /** ledc_task_timer3_res_update_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER3_RES_UPDATE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer3_res_update_st_clr:1;
        /** ledc_task_duty_scale_update_ch0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_DUTY_SCALE_UPDATE_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch0_st_clr:1;
        /** ledc_task_duty_scale_update_ch1_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_DUTY_SCALE_UPDATE_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch1_st_clr:1;
        /** ledc_task_duty_scale_update_ch2_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_DUTY_SCALE_UPDATE_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch2_st_clr:1;
        /** ledc_task_duty_scale_update_ch3_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_DUTY_SCALE_UPDATE_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch3_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st0_clr_reg_t;

/** Type of task_st1_clr register
 *  Task trigger status clear register
 */
typedef union {
    struct {
        /** ledc_task_duty_scale_update_ch4_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_DUTY_SCALE_UPDATE_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch4_st_clr:1;
        /** ledc_task_duty_scale_update_ch5_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_DUTY_SCALE_UPDATE_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_duty_scale_update_ch5_st_clr:1;
        /** ledc_task_timer0_cap_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER0_CAP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer0_cap_st_clr:1;
        /** ledc_task_timer1_cap_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER1_CAP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer1_cap_st_clr:1;
        /** ledc_task_timer2_cap_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER2_CAP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer2_cap_st_clr:1;
        /** ledc_task_timer3_cap_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER3_CAP trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer3_cap_st_clr:1;
        /** ledc_task_sig_out_dis_ch0_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_SIG_OUT_DIS_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch0_st_clr:1;
        /** ledc_task_sig_out_dis_ch1_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_SIG_OUT_DIS_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch1_st_clr:1;
        /** ledc_task_sig_out_dis_ch2_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_SIG_OUT_DIS_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch2_st_clr:1;
        /** ledc_task_sig_out_dis_ch3_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_SIG_OUT_DIS_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch3_st_clr:1;
        /** ledc_task_sig_out_dis_ch4_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_SIG_OUT_DIS_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch4_st_clr:1;
        /** ledc_task_sig_out_dis_ch5_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_SIG_OUT_DIS_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_sig_out_dis_ch5_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch0_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_OVF_CNT_RST_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch0_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch1_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_OVF_CNT_RST_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch1_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch2_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_OVF_CNT_RST_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch2_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch3_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_OVF_CNT_RST_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch3_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch4_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_OVF_CNT_RST_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch4_st_clr:1;
        /** ledc_task_ovf_cnt_rst_ch5_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_OVF_CNT_RST_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_ovf_cnt_rst_ch5_st_clr:1;
        /** ledc_task_timer0_rst_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER0_RST trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer0_rst_st_clr:1;
        /** ledc_task_timer1_rst_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER1_RST trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer1_rst_st_clr:1;
        /** ledc_task_timer2_rst_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER2_RST trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer2_rst_st_clr:1;
        /** ledc_task_timer3_rst_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER3_RST trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer3_rst_st_clr:1;
        /** ledc_task_timer0_resume_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER0_RESUME trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer0_resume_st_clr:1;
        /** ledc_task_timer1_resume_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER1_RESUME trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer1_resume_st_clr:1;
        /** ledc_task_timer2_resume_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER2_RESUME trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer2_resume_st_clr:1;
        /** ledc_task_timer3_resume_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER3_RESUME trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer3_resume_st_clr:1;
        /** ledc_task_timer0_pause_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER0_PAUSE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer0_pause_st_clr:1;
        /** ledc_task_timer1_pause_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER1_PAUSE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer1_pause_st_clr:1;
        /** ledc_task_timer2_pause_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER2_PAUSE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer2_pause_st_clr:1;
        /** ledc_task_timer3_pause_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_TIMER3_PAUSE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_timer3_pause_st_clr:1;
        /** ledc_task_gamma_restart_ch0_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESTART_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch0_st_clr:1;
        /** ledc_task_gamma_restart_ch1_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESTART_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch1_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st1_clr_reg_t;

/** Type of task_st2_clr register
 *  Task trigger status clear register
 */
typedef union {
    struct {
        /** ledc_task_gamma_restart_ch2_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESTART_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch2_st_clr:1;
        /** ledc_task_gamma_restart_ch3_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESTART_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch3_st_clr:1;
        /** ledc_task_gamma_restart_ch4_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESTART_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch4_st_clr:1;
        /** ledc_task_gamma_restart_ch5_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESTART_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_restart_ch5_st_clr:1;
        /** ledc_task_gamma_pause_ch0_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_PAUSE_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch0_st_clr:1;
        /** ledc_task_gamma_pause_ch1_st_clr : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_PAUSE_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch1_st_clr:1;
        /** ledc_task_gamma_pause_ch2_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_PAUSE_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch2_st_clr:1;
        /** ledc_task_gamma_pause_ch3_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_PAUSE_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch3_st_clr:1;
        /** ledc_task_gamma_pause_ch4_st_clr : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_PAUSE_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch4_st_clr:1;
        /** ledc_task_gamma_pause_ch5_st_clr : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_PAUSE_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_pause_ch5_st_clr:1;
        /** ledc_task_gamma_resume_ch0_st_clr : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESUME_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch0_st_clr:1;
        /** ledc_task_gamma_resume_ch1_st_clr : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESUME_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch1_st_clr:1;
        /** ledc_task_gamma_resume_ch2_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESUME_CH2 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch2_st_clr:1;
        /** ledc_task_gamma_resume_ch3_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESUME_CH3 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch3_st_clr:1;
        /** ledc_task_gamma_resume_ch4_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESUME_CH4 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch4_st_clr:1;
        /** ledc_task_gamma_resume_ch5_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear LEDC_TASK_GAMMA_RESUME_CH5 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ledc_task_gamma_resume_ch5_st_clr:1;
        /** tg0_task_cnt_start_timer0_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_START_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_start_timer0_st_clr:1;
        /** tg0_task_alarm_start_timer0_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear TG0_TASK_ALARM_START_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_alarm_start_timer0_st_clr:1;
        /** tg0_task_cnt_stop_timer0_st_clr : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_STOP_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_stop_timer0_st_clr:1;
        /** tg0_task_cnt_reload_timer0_st_clr : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_RELOAD_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_reload_timer0_st_clr:1;
        /** tg0_task_cnt_cap_timer0_st_clr : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_CAP_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_cap_timer0_st_clr:1;
        /** tg0_task_cnt_start_timer1_st_clr : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_START_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_start_timer1_st_clr:1;
        /** tg0_task_alarm_start_timer1_st_clr : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to clear TG0_TASK_ALARM_START_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_alarm_start_timer1_st_clr:1;
        /** tg0_task_cnt_stop_timer1_st_clr : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_STOP_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_stop_timer1_st_clr:1;
        /** tg0_task_cnt_reload_timer1_st_clr : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_RELOAD_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_reload_timer1_st_clr:1;
        /** tg0_task_cnt_cap_timer1_st_clr : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to clear TG0_TASK_CNT_CAP_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg0_task_cnt_cap_timer1_st_clr:1;
        /** tg1_task_cnt_start_timer0_st_clr : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_START_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_start_timer0_st_clr:1;
        /** tg1_task_alarm_start_timer0_st_clr : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to clear TG1_TASK_ALARM_START_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_alarm_start_timer0_st_clr:1;
        /** tg1_task_cnt_stop_timer0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_STOP_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_stop_timer0_st_clr:1;
        /** tg1_task_cnt_reload_timer0_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_RELOAD_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_reload_timer0_st_clr:1;
        /** tg1_task_cnt_cap_timer0_st_clr : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_CAP_TIMER0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_cap_timer0_st_clr:1;
        /** tg1_task_cnt_start_timer1_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_START_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_start_timer1_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st2_clr_reg_t;

/** Type of task_st3_clr register
 *  Task trigger status clear register
 */
typedef union {
    struct {
        /** tg1_task_alarm_start_timer1_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear TG1_TASK_ALARM_START_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_alarm_start_timer1_st_clr:1;
        /** tg1_task_cnt_stop_timer1_st_clr : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_STOP_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_stop_timer1_st_clr:1;
        /** tg1_task_cnt_reload_timer1_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_RELOAD_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_reload_timer1_st_clr:1;
        /** tg1_task_cnt_cap_timer1_st_clr : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to clear TG1_TASK_CNT_CAP_TIMER1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tg1_task_cnt_cap_timer1_st_clr:1;
        /** adc_task_sample0_st_clr : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to clear ADC_TASK_SAMPLE0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_task_sample0_st_clr:1;
        uint32_t reserved_5:1;
        /** adc_task_start0_st_clr : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to clear ADC_TASK_START0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_task_start0_st_clr:1;
        /** adc_task_stop0_st_clr : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to clear ADC_TASK_STOP0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t adc_task_stop0_st_clr:1;
        uint32_t reserved_8:4;
        /** tmpsnsr_task_start_sample_st_clr : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to clear TMPSNSR_TASK_START_SAMPLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tmpsnsr_task_start_sample_st_clr:1;
        /** tmpsnsr_task_stop_sample_st_clr : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to clear TMPSNSR_TASK_STOP_SAMPLE trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t tmpsnsr_task_stop_sample_st_clr:1;
        /** i2s0_task_start_rx_st_clr : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to clear I2S0_TASK_START_RX trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_task_start_rx_st_clr:1;
        /** i2s0_task_start_tx_st_clr : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to clear I2S0_TASK_START_TX trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_task_start_tx_st_clr:1;
        /** i2s0_task_stop_rx_st_clr : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear I2S0_TASK_STOP_RX trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_task_stop_rx_st_clr:1;
        /** i2s0_task_stop_tx_st_clr : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to clear I2S0_TASK_STOP_TX trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t i2s0_task_stop_tx_st_clr:1;
        uint32_t reserved_18:10;
        /** gdma_ahb_task_in_start_ch0_st_clr : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_TASK_IN_START_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_task_in_start_ch0_st_clr:1;
        /** gdma_ahb_task_in_start_ch1_st_clr : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_TASK_IN_START_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_task_in_start_ch1_st_clr:1;
        uint32_t reserved_30:1;
        /** gdma_ahb_task_out_start_ch0_st_clr : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_TASK_OUT_START_CH0 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_task_out_start_ch0_st_clr:1;
    };
    uint32_t val;
} soc_etm_task_st3_clr_reg_t;

/** Type of task_st4_clr register
 *  Task trigger status clear register
 */
typedef union {
    struct {
        /** gdma_ahb_task_out_start_ch1_st_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear GDMA_AHB_TASK_OUT_START_CH1 trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t gdma_ahb_task_out_start_ch1_st_clr:1;
        uint32_t reserved_1:1;
        /** pmu_task_sleep_req_st_clr : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to clear PMU_TASK_SLEEP_REQ trigger status.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t pmu_task_sleep_req_st_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} soc_etm_task_st4_clr_reg_t;

/** Type of clk_en register
 *  ETM clock enable register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to open register clock gate.
         *  0: Open the clock gate only when application writes registers
         *  1: Force open the clock gate for register
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} soc_etm_clk_en_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36737361;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} soc_etm_date_reg_t;


typedef struct soc_etm_dev_t {
    volatile soc_etm_ch_ena_ad0_reg_t ch_ena_ad0;
    volatile soc_etm_ch_ena_ad0_set_reg_t ch_ena_ad0_set;
    volatile soc_etm_ch_ena_ad0_clr_reg_t ch_ena_ad0_clr;
    volatile soc_etm_ch_ena_ad1_reg_t ch_ena_ad1;
    volatile soc_etm_ch_ena_ad1_set_reg_t ch_ena_ad1_set;
    volatile soc_etm_ch_ena_ad1_clr_reg_t ch_ena_ad1_clr;
    volatile struct {
        soc_etm_chn_evt_id_reg_t eid;
        soc_etm_chn_task_id_reg_t tid;
    } channel[50];
    volatile soc_etm_evt_st0_reg_t evt_st0;
    volatile soc_etm_evt_st0_clr_reg_t evt_st0_clr;
    volatile soc_etm_evt_st1_reg_t evt_st1;
    volatile soc_etm_evt_st1_clr_reg_t evt_st1_clr;
    volatile soc_etm_evt_st2_reg_t evt_st2;
    volatile soc_etm_evt_st2_clr_reg_t evt_st2_clr;
    volatile soc_etm_evt_st3_reg_t evt_st3;
    volatile soc_etm_evt_st3_clr_reg_t evt_st3_clr;
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
_Static_assert(sizeof(soc_etm_dev_t) == 0x1f8, "Invalid size of soc_etm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
