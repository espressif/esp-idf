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
/** Type of chn_conf0 register
 *  Configuration register 0 for channel n
 */
typedef union {
    struct {
        /** timer_sel : R/W; bitpos: [1:0]; default: 0;
         *  Configures which timer is channel n selected.
         *  0: Select timer0
         *  1: Select timer1
         *  2: Select timer2
         *  3: Select timer3
         */
        uint32_t timer_sel:2;
        /** sig_out_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable signal output on channel n.
         *  0: Signal output disable
         *  1: Signal output enable
         */
        uint32_t sig_out_en:1;
        /** idle_lv : R/W; bitpos: [3]; default: 0;
         *  Configures the output value when channel n is inactive. Valid only when
         *  LEDC_SIG_OUT_EN_CHn is 0.
         *  0: Output level is low
         *  1: Output level is high
         */
        uint32_t idle_lv:1;
        /** para_up : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to update LEDC_HPOINT_CHn, LEDC_DUTY_START_CHn,
         *  LEDC_SIG_OUT_EN_CHn, LEDC_TIMER_SEL_CHn, LEDC_OVF_CNT_EN_CHn fields and duty cycle
         *  range configuration for channel n, and will be automatically cleared by hardware.
         *  0: Invalid. No effect
         *  1: Update
         */
        uint32_t para_up:1;
        /** ovf_num : R/W; bitpos: [14:5]; default: 0;
         *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CHn_INT interrupt
         *  will be triggered when channel n overflows for (LEDC_OVF_NUM_CHn + 1) times.
         */
        uint32_t ovf_num:10;
        /** ovf_cnt_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable the ovf_cnt of channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ovf_cnt_en:1;
        /** ovf_cnt_reset : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to reset the  ovf_cnt of channel n.
         *  0: Invalid. No effect
         *  1: Reset the ovf_cnt
         */
        uint32_t ovf_cnt_reset:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} ledc_chn_conf0_reg_t;

/** Type of chn_hpoint register
 *  High point register for channel n
 */
typedef union {
    struct {
        /** hpoint : R/W; bitpos: [19:0]; default: 0;
         *  Configures high point of signal output on channel n. The output value changes to
         *  high when the selected timers has reached the value specified by this register.
         */
        uint32_t hpoint:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_chn_hpoint_reg_t;

/** Type of chn_duty register
 *  Initial duty cycle register for channel n
 */
typedef union {
    struct {
        /** duty : R/W; bitpos: [24:0]; default: 0;
         *  Configures the duty of signal output on channel n.
         */
        uint32_t duty:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} ledc_chn_duty_reg_t;

/** Type of chn_conf1 register
 *  Configuration register 1 for channel n
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** duty_start : R/W/SC; bitpos: [31]; default: 0;
         *  Configures whether the duty cycle fading configurations take effect.
         *  0: Not take effect
         *  1: Take effect
         */
        uint32_t duty_start:1;
    };
    uint32_t val;
} ledc_chn_conf1_reg_t;

/** Type of timerx_conf register
 *  Timer x configuration register
 */
typedef union {
    struct {
        /** duty_res : R/W; bitpos: [4:0]; default: 0;
         *  Configures the bit width of the counter in timer x. Valid values are 1 to 20.
         */
        uint32_t duty_res:5;
        /** clk_div : R/W; bitpos: [22:5]; default: 0;
         *  Configures the divisor for the divider in timer x.The least significant eight bits
         *  represent the fractional part.
         */
        uint32_t clk_div:18;
        /** pause : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to pause the counter in timer x.
         *  0: Normal
         *  1: Pause
         */
        uint32_t pause:1;
        /** rst : R/W; bitpos: [24]; default: 1;
         *  Configures whether or not to reset timer x. The counter will show 0 after reset.
         *  0: Not reset
         *  1: Reset
         */
        uint32_t rst:1;
        uint32_t reserved_25:1;
        /** para_up : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to update LEDC_CLK_DIV_TIMERn and LEDC_TIMERn_DUTY_RES.
         *  0: Invalid. No effect
         *  1: Update
         */
        uint32_t para_up:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ledc_timerx_conf_reg_t;

/** Type of chn_fade_conf register
 *  Ledc chn fade config register.
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** fade_pause : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to pause duty cycle fading of LEDC chn.
         *  0: Invalid. No effect
         *  1: Pause
         */
        uint32_t fade_pause:1;
        /** fade_resume : WT; bitpos: [6]; default: 0;
         *  Configures whether or nor to resume duty cycle fading of LEDC chn.
         *  0: Invalid. No effect
         *  1: Resume
         */
        uint32_t fade_resume:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ledc_chn_fade_conf_reg_t;

/** Type of evt_task_en0 register
 *  Ledc event task enable bit register0.
 */
typedef union {
    struct {
        /** evt_duty_chng_end_ch0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH0 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch0_en:1;
        /** evt_duty_chng_end_ch1_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH1 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch1_en:1;
        /** evt_duty_chng_end_ch2_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH2 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch2_en:1;
        /** evt_duty_chng_end_ch3_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH3 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch3_en:1;
        /** evt_duty_chng_end_ch4_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH4 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch4_en:1;
        /** evt_duty_chng_end_ch5_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH5 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch5_en:1;
        /** evt_duty_chng_end_ch6_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH6 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch6_en:1;
        /** evt_duty_chng_end_ch7_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH7 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch7_en:1;
        /** evt_ovf_cnt_pls_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH0 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch0_en:1;
        /** evt_ovf_cnt_pls_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH1 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch1_en:1;
        /** evt_ovf_cnt_pls_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH2 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch2_en:1;
        /** evt_ovf_cnt_pls_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH3 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch3_en:1;
        /** evt_ovf_cnt_pls_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH4 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch4_en:1;
        /** evt_ovf_cnt_pls_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH5 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch5_en:1;
        /** evt_ovf_cnt_pls_ch6_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH6 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch6_en:1;
        /** evt_ovf_cnt_pls_ch7_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH7 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch7_en:1;
        /** evt_time_ovf_timer0_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER0 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer0_en:1;
        /** evt_time_ovf_timer1_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER1 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer1_en:1;
        /** evt_time_ovf_timer2_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER2 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer2_en:1;
        /** evt_time_ovf_timer3_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER3 event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer3_en:1;
        /** evt_timer0_cmp_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIMER0_CMP event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_timer0_cmp_en:1;
        /** evt_timer1_cmp_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIMER1_CMP event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_timer1_cmp_en:1;
        /** evt_timer2_cmp_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIMER2_CMP event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_timer2_cmp_en:1;
        /** evt_timer3_cmp_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable the LEDC_EVT_TIMER3_CMP event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_timer3_cmp_en:1;
        /** task_duty_scale_update_ch0_en : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH0 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch0_en:1;
        /** task_duty_scale_update_ch1_en : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH1 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch1_en:1;
        /** task_duty_scale_update_ch2_en : R/W; bitpos: [26]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH2 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch2_en:1;
        /** task_duty_scale_update_ch3_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH3 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch3_en:1;
        /** task_duty_scale_update_ch4_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH4 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch4_en:1;
        /** task_duty_scale_update_ch5_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH5 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch5_en:1;
        /** task_duty_scale_update_ch6_en : R/W; bitpos: [30]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH6 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch6_en:1;
        /** task_duty_scale_update_ch7_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH7 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch7_en:1;
    };
    uint32_t val;
} ledc_evt_task_en0_reg_t;

/** Type of evt_task_en1 register
 *  Ledc event task enable bit register1.
 */
typedef union {
    struct {
        /** task_timer0_res_update_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER0_RES_UPDATE task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_res_update_en:1;
        /** task_timer1_res_update_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER1_RES_UPDATE task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_res_update_en:1;
        /** task_timer2_res_update_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER2_RES_UPDATE task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_res_update_en:1;
        /** task_timer3_res_update_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER3_RES_UPDATE task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer3_res_update_en:1;
        /** task_timer0_cap_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER0_CAP task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_cap_en:1;
        /** task_timer1_cap_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER1_CAP task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_cap_en:1;
        /** task_timer2_cap_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER2_CAP task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_cap_en:1;
        /** task_timer3_cap_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER3_CAP task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer3_cap_en:1;
        /** task_sig_out_dis_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH0 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch0_en:1;
        /** task_sig_out_dis_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH1 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch1_en:1;
        /** task_sig_out_dis_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH2 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch2_en:1;
        /** task_sig_out_dis_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH3 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch3_en:1;
        /** task_sig_out_dis_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH4 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch4_en:1;
        /** task_sig_out_dis_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH5 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch5_en:1;
        /** task_sig_out_dis_ch6_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH6 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch6_en:1;
        /** task_sig_out_dis_ch7_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH7 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch7_en:1;
        /** task_ovf_cnt_rst_ch0_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH0 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch0_en:1;
        /** task_ovf_cnt_rst_ch1_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH1 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch1_en:1;
        /** task_ovf_cnt_rst_ch2_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH2 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch2_en:1;
        /** task_ovf_cnt_rst_ch3_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH3 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch3_en:1;
        /** task_ovf_cnt_rst_ch4_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH4 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch4_en:1;
        /** task_ovf_cnt_rst_ch5_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH5 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch5_en:1;
        /** task_ovf_cnt_rst_ch6_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH6 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch6_en:1;
        /** task_ovf_cnt_rst_ch7_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH7 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch7_en:1;
        /** task_timer0_rst_en : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER0_RST task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_rst_en:1;
        /** task_timer1_rst_en : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER1_RST task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_rst_en:1;
        /** task_timer2_rst_en : R/W; bitpos: [26]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER2_RST task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_rst_en:1;
        /** task_timer3_rst_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER3_RST task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer3_rst_en:1;
        /** task_timer0_pause_resume_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER0_PAUSE and LEDC_TASK_TIMER0
         *  _RESUME task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_pause_resume_en:1;
        /** task_timer1_pause_resume_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER1_PAUSE and LEDC_TASK_TIMER1
         *  _RESUME task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_pause_resume_en:1;
        /** task_timer2_pause_resume_en : R/W; bitpos: [30]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER2_PAUSE and LEDC_TASK_TIMER2
         *  _RESUME task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_pause_resume_en:1;
        /** task_timer3_pause_resume_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_TIMER3_PAUSE and LEDC_TASK_TIMER3
         *  _RESUME task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer3_pause_resume_en:1;
    };
    uint32_t val;
} ledc_evt_task_en1_reg_t;

/** Type of evt_task_en2 register
 *  Ledc event task enable bit register2.
 */
typedef union {
    struct {
        /** task_fade_restart_ch0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH0 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch0_en:1;
        /** task_fade_restart_ch1_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH1 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch1_en:1;
        /** task_fade_restart_ch2_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH2 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch2_en:1;
        /** task_fade_restart_ch3_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH3 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch3_en:1;
        /** task_fade_restart_ch4_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH4 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch4_en:1;
        /** task_fade_restart_ch5_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH5 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch5_en:1;
        /** task_fade_restart_ch6_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH6 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch6_en:1;
        /** task_fade_restart_ch7_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH7 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_restart_ch7_en:1;
        /** task_fade_pause_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH0 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch0_en:1;
        /** task_fade_pause_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH1 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch1_en:1;
        /** task_fade_pause_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH2 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch2_en:1;
        /** task_fade_pause_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH3 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch3_en:1;
        /** task_fade_pause_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH4 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch4_en:1;
        /** task_fade_pause_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH5 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch5_en:1;
        /** task_fade_pause_ch6_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH6 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch6_en:1;
        /** task_fade_pause_ch7_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH7 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_pause_ch7_en:1;
        /** task_fade_resume_ch0_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH0 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch0_en:1;
        /** task_fade_resume_ch1_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH1 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch1_en:1;
        /** task_fade_resume_ch2_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH2 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch2_en:1;
        /** task_fade_resume_ch3_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH3 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch3_en:1;
        /** task_fade_resume_ch4_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH4 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch4_en:1;
        /** task_fade_resume_ch5_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH5 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch5_en:1;
        /** task_fade_resume_ch6_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH6 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch6_en:1;
        /** task_fade_resume_ch7_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH7 task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_fade_resume_ch7_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} ledc_evt_task_en2_reg_t;

/** Type of timerx_cmp register
 *  Ledc timer x compare value register.
 */
typedef union {
    struct {
        /** cmp : R/W; bitpos: [19:0]; default: 0;
         *  Configures the comparison value for LEDC timer x.
         */
        uint32_t cmp:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timerx_cmp_reg_t;

/** Type of conf register
 *  LEDC global configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not to open register clock gate.
         *  0: Open the clock gate only when application writes registers
         *  1: Force open the clock gate for register
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} ledc_conf_reg_t;

/** Type of ch_power_up_conf register
 *  LEDC channel power up configuration register
 */
typedef union {
    struct {
        /** ch0_power_up : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to power up ch0.
         *  0: power down
         *  1: power up
         */
        uint32_t ch0_power_up:1;
        /** ch1_power_up : R/W; bitpos: [1]; default: 1;
         *  Configures whether or not to power up ch1.
         *  0: power down
         *  1: power up
         */
        uint32_t ch1_power_up:1;
        /** ch2_power_up : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to power up ch2.
         *  0: power down
         *  1: power up
         */
        uint32_t ch2_power_up:1;
        /** ch3_power_up : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to power up ch3.
         *  0: power down
         *  1: power up
         */
        uint32_t ch3_power_up:1;
        /** ch4_power_up : R/W; bitpos: [4]; default: 1;
         *  Configures whether or not to power up ch4.
         *  0: power down
         *  1: power up
         */
        uint32_t ch4_power_up:1;
        /** ch5_power_up : R/W; bitpos: [5]; default: 1;
         *  Configures whether or not to power up ch5.
         *  0: power down
         *  1: power up
         */
        uint32_t ch5_power_up:1;
        /** ch6_power_up : R/W; bitpos: [6]; default: 1;
         *  Configures whether or not to power up ch6.
         *  0: power down
         *  1: power up
         */
        uint32_t ch6_power_up:1;
        /** ch7_power_up : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not to power up ch7.
         *  0: power down
         *  1: power up
         */
        uint32_t ch7_power_up:1;
        /** ch0_clk_gate_force_on : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to open ch0 clock gate.
         *  0: Open the clock gate only when ch0 is power up
         *  1: Force open the clock gate for ch0
         */
        uint32_t ch0_clk_gate_force_on:1;
        /** ch1_clk_gate_force_on : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to open ch1 clock gate.
         *  0: Open the clock gate only when ch1 is power up
         *  1: Force open the clock gate for ch1
         */
        uint32_t ch1_clk_gate_force_on:1;
        /** ch2_clk_gate_force_on : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to open ch2 clock gate.
         *  0: Open the clock gate only when ch2 is power up
         *  1: Force open the clock gate for ch2
         */
        uint32_t ch2_clk_gate_force_on:1;
        /** ch3_clk_gate_force_on : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to open ch3 clock gate.
         *  0: Open the clock gate only when ch3 is power up
         *  1: Force open the clock gate for ch3
         */
        uint32_t ch3_clk_gate_force_on:1;
        /** ch4_clk_gate_force_on : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to open ch4 clock gate.
         *  0: Open the clock gate only when ch4 is power up
         *  1: Force open the clock gate for ch4
         */
        uint32_t ch4_clk_gate_force_on:1;
        /** ch5_clk_gate_force_on : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to open ch5 clock gate.
         *  0: Open the clock gate only when ch5 is power up
         *  1: Force open the clock gate for ch5
         */
        uint32_t ch5_clk_gate_force_on:1;
        /** ch6_clk_gate_force_on : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to open ch6 clock gate.
         *  0: Open the clock gate only when ch6 is power up
         *  1: Force open the clock gate for ch6
         */
        uint32_t ch6_clk_gate_force_on:1;
        /** ch7_clk_gate_force_on : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to open ch7 clock gate.
         *  0: Open the clock gate only when ch7 is power up
         *  1: Force open the clock gate for ch7
         */
        uint32_t ch7_clk_gate_force_on:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ledc_ch_power_up_conf_reg_t;

/** Type of timer_power_up_conf register
 *  LEDC timer power up configuration register
 */
typedef union {
    struct {
        /** timer0_power_up : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to power up timer0.
         *  0: power down
         *  1: power up
         */
        uint32_t timer0_power_up:1;
        /** timer1_power_up : R/W; bitpos: [1]; default: 1;
         *  Configures whether or not to power up timer1.
         *  0: power down
         *  1: power up
         */
        uint32_t timer1_power_up:1;
        /** timer2_power_up : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to power up timer2.
         *  0: power down
         *  1: power up
         */
        uint32_t timer2_power_up:1;
        /** timer3_power_up : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to power up timer3.
         *  0: power down
         *  1: power up
         */
        uint32_t timer3_power_up:1;
        /** timer0_clk_gate_force_on : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to open timer0 clock gate.
         *  0: Open the clock gate only when timer0 is power up
         *  1: Force open the clock gate for timer0
         */
        uint32_t timer0_clk_gate_force_on:1;
        /** timer1_clk_gate_force_on : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to open timer1 clock gate.
         *  0: Open the clock gate only when timer1 is power up
         *  1: Force open the clock gate for timer1
         */
        uint32_t timer1_clk_gate_force_on:1;
        /** timer2_clk_gate_force_on : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to open timer2 clock gate.
         *  0: Open the clock gate only when timer2 is power up
         *  1: Force open the clock gate for timer2
         */
        uint32_t timer2_clk_gate_force_on:1;
        /** timer3_clk_gate_force_on : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to open timer3 clock gate.
         *  0: Open the clock gate only when timer3 is power up
         *  1: Force open the clock gate for timer3
         */
        uint32_t timer3_clk_gate_force_on:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ledc_timer_power_up_conf_reg_t;


/** Group: Status Register */
/** Type of chn_duty_r register
 *  Current duty cycle register for channel n
 */
typedef union {
    struct {
        /** duty_r : RO; bitpos: [24:0]; default: 0;
         *  Represents the current duty of output signal on channel n.
         */
        uint32_t duty_r:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} ledc_chn_duty_r_reg_t;

/** Type of timerx_value register
 *  Timer x current counter value register
 */
typedef union {
    struct {
        /** cnt : RO; bitpos: [19:0]; default: 0;
         *  Represents the current counter value of timer x.
         */
        uint32_t cnt:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timerx_value_reg_t;

/** Type of timerx_cnt_cap register
 *  Ledc timer x captured count value register.
 */
typedef union {
    struct {
        /** cnt_cap : RO; bitpos: [19:0]; default: 0;
         *  Represents the captured LEDC timer x count value.
         */
        uint32_t cnt_cap:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timerx_cnt_cap_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  Interrupt raw status register
 */
typedef union {
    struct {
        /** timer0_ovf_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_TIMER0_OVF_INT. Triggered when the
         *  timer0 has reached its maximum counter value.
         */
        uint32_t timer0_ovf_int_raw:1;
        /** timer1_ovf_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_TIMER1_OVF_INT. Triggered when the
         *  timer1 has reached its maximum counter value.
         */
        uint32_t timer1_ovf_int_raw:1;
        /** timer2_ovf_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_TIMER2_OVF_INT. Triggered when the
         *  timer2 has reached its maximum counter value.
         */
        uint32_t timer2_ovf_int_raw:1;
        /** timer3_ovf_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_TIMER3_OVF_INT. Triggered when the
         *  timer3 has reached its maximum counter value.
         */
        uint32_t timer3_ovf_int_raw:1;
        /** duty_chng_end_ch0_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH0_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch0_int_raw:1;
        /** duty_chng_end_ch1_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH1_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch1_int_raw:1;
        /** duty_chng_end_ch2_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH2_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch2_int_raw:1;
        /** duty_chng_end_ch3_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH3_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch3_int_raw:1;
        /** duty_chng_end_ch4_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH4_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch4_int_raw:1;
        /** duty_chng_end_ch5_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH5_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch5_int_raw:1;
        /** duty_chng_end_ch6_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH6_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch6_int_raw:1;
        /** duty_chng_end_ch7_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH7_INT. Triggered
         *  when the fading of duty has finished.
         */
        uint32_t duty_chng_end_ch7_int_raw:1;
        /** ovf_cnt_ch0_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH0_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH0.
         */
        uint32_t ovf_cnt_ch0_int_raw:1;
        /** ovf_cnt_ch1_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH1_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH1.
         */
        uint32_t ovf_cnt_ch1_int_raw:1;
        /** ovf_cnt_ch2_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH2_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH2.
         */
        uint32_t ovf_cnt_ch2_int_raw:1;
        /** ovf_cnt_ch3_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH3_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH3.
         */
        uint32_t ovf_cnt_ch3_int_raw:1;
        /** ovf_cnt_ch4_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH4_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH4.
         */
        uint32_t ovf_cnt_ch4_int_raw:1;
        /** ovf_cnt_ch5_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH5_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH5.
         */
        uint32_t ovf_cnt_ch5_int_raw:1;
        /** ovf_cnt_ch6_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH6_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH6.
         */
        uint32_t ovf_cnt_ch6_int_raw:1;
        /** ovf_cnt_ch7_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH7_INT. Triggered when
         *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH7.
         */
        uint32_t ovf_cnt_ch7_int_raw:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_int_raw_reg_t;

/** Type of int_st register
 *  Interrupt masked status register
 */
typedef union {
    struct {
        /** timer0_ovf_int_st : RO; bitpos: [0]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_TIMER0_OVF_INT. Valid only
         *  when LEDC_TIMER0_OVF_INT_ENA is set to 1.
         */
        uint32_t timer0_ovf_int_st:1;
        /** timer1_ovf_int_st : RO; bitpos: [1]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_TIMER1_OVF_INT. Valid only
         *  when LEDC_TIMER1_OVF_INT_ENA is set to 1.
         */
        uint32_t timer1_ovf_int_st:1;
        /** timer2_ovf_int_st : RO; bitpos: [2]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_TIMER2_OVF_INT. Valid only
         *  when LEDC_TIMER2_OVF_INT_ENA is set to 1.
         */
        uint32_t timer2_ovf_int_st:1;
        /** timer3_ovf_int_st : RO; bitpos: [3]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_TIMER3_OVF_INT. Valid only
         *  when LEDC_TIMER3_OVF_INT_ENA is set to 1.
         */
        uint32_t timer3_ovf_int_st:1;
        /** duty_chng_end_ch0_int_st : RO; bitpos: [4]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH0_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH0_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch0_int_st:1;
        /** duty_chng_end_ch1_int_st : RO; bitpos: [5]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH1_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH1_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch1_int_st:1;
        /** duty_chng_end_ch2_int_st : RO; bitpos: [6]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH2_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH2_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch2_int_st:1;
        /** duty_chng_end_ch3_int_st : RO; bitpos: [7]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH3_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH3_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch3_int_st:1;
        /** duty_chng_end_ch4_int_st : RO; bitpos: [8]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH4_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH4_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch4_int_st:1;
        /** duty_chng_end_ch5_int_st : RO; bitpos: [9]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH5_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH5_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch5_int_st:1;
        /** duty_chng_end_ch6_int_st : RO; bitpos: [10]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH6_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH6_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch6_int_st:1;
        /** duty_chng_end_ch7_int_st : RO; bitpos: [11]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH7_INT. Valid
         *  only when LEDC_DUTY_CHNG_END_CH7_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch7_int_st:1;
        /** ovf_cnt_ch0_int_st : RO; bitpos: [12]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH0_INT. Valid only
         *  when LEDC_OVF_CNT_CH0_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch0_int_st:1;
        /** ovf_cnt_ch1_int_st : RO; bitpos: [13]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH1_INT. Valid only
         *  when LEDC_OVF_CNT_CH1_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch1_int_st:1;
        /** ovf_cnt_ch2_int_st : RO; bitpos: [14]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH2_INT. Valid only
         *  when LEDC_OVF_CNT_CH2_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch2_int_st:1;
        /** ovf_cnt_ch3_int_st : RO; bitpos: [15]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH3_INT. Valid only
         *  when LEDC_OVF_CNT_CH3_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch3_int_st:1;
        /** ovf_cnt_ch4_int_st : RO; bitpos: [16]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH4_INT. Valid only
         *  when LEDC_OVF_CNT_CH4_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch4_int_st:1;
        /** ovf_cnt_ch5_int_st : RO; bitpos: [17]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH5_INT. Valid only
         *  when LEDC_OVF_CNT_CH5_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch5_int_st:1;
        /** ovf_cnt_ch6_int_st : RO; bitpos: [18]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH6_INT. Valid only
         *  when LEDC_OVF_CNT_CH6_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch6_int_st:1;
        /** ovf_cnt_ch7_int_st : RO; bitpos: [19]; default: 0;
         *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH7_INT. Valid only
         *  when LEDC_OVF_CNT_CH7_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch7_int_st:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable register
 */
typedef union {
    struct {
        /** timer0_ovf_int_ena : R/W; bitpos: [0]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_TIMER0_OVF_INT.
         */
        uint32_t timer0_ovf_int_ena:1;
        /** timer1_ovf_int_ena : R/W; bitpos: [1]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_TIMER1_OVF_INT.
         */
        uint32_t timer1_ovf_int_ena:1;
        /** timer2_ovf_int_ena : R/W; bitpos: [2]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_TIMER2_OVF_INT.
         */
        uint32_t timer2_ovf_int_ena:1;
        /** timer3_ovf_int_ena : R/W; bitpos: [3]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_TIMER3_OVF_INT.
         */
        uint32_t timer3_ovf_int_ena:1;
        /** duty_chng_end_ch0_int_ena : R/W; bitpos: [4]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH0_INT.
         */
        uint32_t duty_chng_end_ch0_int_ena:1;
        /** duty_chng_end_ch1_int_ena : R/W; bitpos: [5]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH1_INT.
         */
        uint32_t duty_chng_end_ch1_int_ena:1;
        /** duty_chng_end_ch2_int_ena : R/W; bitpos: [6]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH2_INT.
         */
        uint32_t duty_chng_end_ch2_int_ena:1;
        /** duty_chng_end_ch3_int_ena : R/W; bitpos: [7]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH3_INT.
         */
        uint32_t duty_chng_end_ch3_int_ena:1;
        /** duty_chng_end_ch4_int_ena : R/W; bitpos: [8]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH4_INT.
         */
        uint32_t duty_chng_end_ch4_int_ena:1;
        /** duty_chng_end_ch5_int_ena : R/W; bitpos: [9]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH5_INT.
         */
        uint32_t duty_chng_end_ch5_int_ena:1;
        /** duty_chng_end_ch6_int_ena : R/W; bitpos: [10]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH6_INT.
         */
        uint32_t duty_chng_end_ch6_int_ena:1;
        /** duty_chng_end_ch7_int_ena : R/W; bitpos: [11]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH7_INT.
         */
        uint32_t duty_chng_end_ch7_int_ena:1;
        /** ovf_cnt_ch0_int_ena : R/W; bitpos: [12]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH0_INT.
         */
        uint32_t ovf_cnt_ch0_int_ena:1;
        /** ovf_cnt_ch1_int_ena : R/W; bitpos: [13]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH1_INT.
         */
        uint32_t ovf_cnt_ch1_int_ena:1;
        /** ovf_cnt_ch2_int_ena : R/W; bitpos: [14]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH2_INT.
         */
        uint32_t ovf_cnt_ch2_int_ena:1;
        /** ovf_cnt_ch3_int_ena : R/W; bitpos: [15]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH3_INT.
         */
        uint32_t ovf_cnt_ch3_int_ena:1;
        /** ovf_cnt_ch4_int_ena : R/W; bitpos: [16]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH4_INT.
         */
        uint32_t ovf_cnt_ch4_int_ena:1;
        /** ovf_cnt_ch5_int_ena : R/W; bitpos: [17]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH5_INT.
         */
        uint32_t ovf_cnt_ch5_int_ena:1;
        /** ovf_cnt_ch6_int_ena : R/W; bitpos: [18]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH6_INT.
         */
        uint32_t ovf_cnt_ch6_int_ena:1;
        /** ovf_cnt_ch7_int_ena : R/W; bitpos: [19]; default: 0;
         *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH7_INT.
         */
        uint32_t ovf_cnt_ch7_int_ena:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear register
 */
typedef union {
    struct {
        /** timer0_ovf_int_clr : WT; bitpos: [0]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_TIMER0_OVF_INT.
         */
        uint32_t timer0_ovf_int_clr:1;
        /** timer1_ovf_int_clr : WT; bitpos: [1]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_TIMER1_OVF_INT.
         */
        uint32_t timer1_ovf_int_clr:1;
        /** timer2_ovf_int_clr : WT; bitpos: [2]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_TIMER2_OVF_INT.
         */
        uint32_t timer2_ovf_int_clr:1;
        /** timer3_ovf_int_clr : WT; bitpos: [3]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_TIMER3_OVF_INT.
         */
        uint32_t timer3_ovf_int_clr:1;
        /** duty_chng_end_ch0_int_clr : WT; bitpos: [4]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH0_INT.
         */
        uint32_t duty_chng_end_ch0_int_clr:1;
        /** duty_chng_end_ch1_int_clr : WT; bitpos: [5]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH1_INT.
         */
        uint32_t duty_chng_end_ch1_int_clr:1;
        /** duty_chng_end_ch2_int_clr : WT; bitpos: [6]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH2_INT.
         */
        uint32_t duty_chng_end_ch2_int_clr:1;
        /** duty_chng_end_ch3_int_clr : WT; bitpos: [7]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH3_INT.
         */
        uint32_t duty_chng_end_ch3_int_clr:1;
        /** duty_chng_end_ch4_int_clr : WT; bitpos: [8]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH4_INT.
         */
        uint32_t duty_chng_end_ch4_int_clr:1;
        /** duty_chng_end_ch5_int_clr : WT; bitpos: [9]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH5_INT.
         */
        uint32_t duty_chng_end_ch5_int_clr:1;
        /** duty_chng_end_ch6_int_clr : WT; bitpos: [10]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH6_INT.
         */
        uint32_t duty_chng_end_ch6_int_clr:1;
        /** duty_chng_end_ch7_int_clr : WT; bitpos: [11]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH7_INT.
         */
        uint32_t duty_chng_end_ch7_int_clr:1;
        /** ovf_cnt_ch0_int_clr : WT; bitpos: [12]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH0_INT.
         */
        uint32_t ovf_cnt_ch0_int_clr:1;
        /** ovf_cnt_ch1_int_clr : WT; bitpos: [13]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH1_INT.
         */
        uint32_t ovf_cnt_ch1_int_clr:1;
        /** ovf_cnt_ch2_int_clr : WT; bitpos: [14]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH2_INT.
         */
        uint32_t ovf_cnt_ch2_int_clr:1;
        /** ovf_cnt_ch3_int_clr : WT; bitpos: [15]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH3_INT.
         */
        uint32_t ovf_cnt_ch3_int_clr:1;
        /** ovf_cnt_ch4_int_clr : WT; bitpos: [16]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH4_INT.
         */
        uint32_t ovf_cnt_ch4_int_clr:1;
        /** ovf_cnt_ch5_int_clr : WT; bitpos: [17]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH5_INT.
         */
        uint32_t ovf_cnt_ch5_int_clr:1;
        /** ovf_cnt_ch6_int_clr : WT; bitpos: [18]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH6_INT.
         */
        uint32_t ovf_cnt_ch6_int_clr:1;
        /** ovf_cnt_ch7_int_clr : WT; bitpos: [19]; default: 0;
         *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH7_INT.
         */
        uint32_t ovf_cnt_ch7_int_clr:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_int_clr_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** ledc_date : R/W; bitpos: [27:0]; default: 37765488;
         *  Configures the version.
         */
        uint32_t ledc_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ledc_date_reg_t;


typedef struct {
    volatile ledc_chn_conf0_reg_t conf0;
    volatile ledc_chn_hpoint_reg_t hpoint;
    volatile ledc_chn_duty_reg_t duty_init;
    volatile ledc_chn_conf1_reg_t conf1;
    volatile ledc_chn_duty_r_reg_t duty_r;
} ledc_chn_reg_t;

typedef struct {
    volatile ledc_chn_reg_t channel[8];
} ledc_ch_group_reg_t;

typedef struct {
    volatile ledc_timerx_conf_reg_t conf;
    volatile ledc_timerx_value_reg_t value;
} ledc_timerx_reg_t;

typedef struct {
    volatile ledc_timerx_reg_t timer[4];
} ledc_timer_group_reg_t;

typedef struct {
    volatile ledc_chn_fade_conf_reg_t fade_conf[8];
} ledc_ch_fade_conf_group_reg_t;

typedef struct {
    volatile ledc_timerx_cmp_reg_t cmp[4];
} ledc_timer_cmp_group_reg_t;

typedef struct {
    volatile ledc_timerx_cnt_cap_reg_t cnt_cap[4];
} ledc_timer_cnt_cap_group_reg_t;

/**
 * Fade param register type
 */
typedef union {
    struct {
        uint32_t duty_inc       :1;
        uint32_t duty_cycle     :10;
        uint32_t scale          :10;
        uint32_t duty_num       :10;
        uint32_t reserved       :1;
    };
    uint32_t val;
} ledc_chn_fade_param_reg_t;

typedef struct {
    volatile ledc_chn_fade_param_reg_t fade_param;
    uint32_t reserved[15];
} ledc_chn_fade_param_t;

typedef struct {
    volatile ledc_chn_fade_param_t channel[8];
} ledc_ch_fade_param_group_t;

typedef struct ledc_dev_t {
    volatile ledc_ch_group_reg_t channel_group[1];
    volatile ledc_timer_group_reg_t timer_group[1];
    volatile ledc_int_raw_reg_t int_raw;
    volatile ledc_int_st_reg_t int_st;
    volatile ledc_int_ena_reg_t int_ena;
    volatile ledc_int_clr_reg_t int_clr;
    uint32_t reserved_0d0[12];
    volatile ledc_ch_fade_conf_group_reg_t channel_fade_conf_group[1];
    volatile ledc_evt_task_en0_reg_t evt_task_en0;
    volatile ledc_evt_task_en1_reg_t evt_task_en1;
    volatile ledc_evt_task_en2_reg_t evt_task_en2;
    uint32_t reserved_12c[5];
    volatile ledc_timer_cmp_group_reg_t timer_cmp_group[1];
    volatile ledc_timer_cnt_cap_group_reg_t timer_cnt_cap_group[1];
    uint32_t reserved_160[4];
    volatile ledc_conf_reg_t conf;
    volatile ledc_ch_power_up_conf_reg_t ch_power_up_conf;
    volatile ledc_timer_power_up_conf_reg_t timer_power_up_conf;
    volatile ledc_date_reg_t date;
    uint32_t reserved_180[160];
    volatile ledc_ch_fade_param_group_t channel_fade_param_group[1];
} ledc_dev_t;

extern ledc_dev_t LEDC;

#ifndef __cplusplus
_Static_assert(sizeof(ledc_dev_t) == 0x600, "Invalid size of ledc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
