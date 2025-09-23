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
        /** timer_sel_chn : R/W; bitpos: [1:0]; default: 0;
         *  Configures which timer is channel n selected.
         *  0: Select timer0
         *  1: Select timer1
         *  2: Select timer2
         *  3: Select timer3
         */
        uint32_t timer_sel_chn:2;
        /** sig_out_en_chn : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable signal output on channel n.
         *  0: Signal output disable
         *  1: Signal output enable
         */
        uint32_t sig_out_en_chn:1;
        /** idle_lv_chn : R/W; bitpos: [3]; default: 0;
         *  Configures the output value when channel n is inactive. Valid only when
         *  LEDC_SIG_OUT_EN_CHn is 0.
         *  0: Output level is low
         *  1: Output level is high
         */
        uint32_t idle_lv_chn:1;
        /** para_up_chn : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to update LEDC_HPOINT_CHn, LEDC_DUTY_START_CHn,
         *  LEDC_SIG_OUT_EN_CHn, LEDC_TIMER_SEL_CHn, LEDC_DUTY_NUM_CHn, LEDC_DUTY_CYCLE_CHn,
         *  LEDC_DUTY_SCALE_CHn, LEDC_DUTY_INC_CHn, and LEDC_OVF_CNT_EN_CHn fields for channel
         *  n, and will be automatically cleared by hardware.
         *  0: Invalid. No effect
         *  1: Update
         */
        uint32_t para_up_chn:1;
        /** ovf_num_chn : R/W; bitpos: [14:5]; default: 0;
         *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CHn_INT interrupt
         *  will be triggered when channel n overflows for (LEDC_OVF_NUM_CHn + 1) times.
         */
        uint32_t ovf_num_chn:10;
        /** ovf_cnt_en_chn : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable the ovf_cnt of channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ovf_cnt_en_chn:1;
        /** ovf_cnt_reset_chn : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to reset the  ovf_cnt of channel n.
         *  0: Invalid. No effect
         *  1: Reset the ovf_cnt
         */
        uint32_t ovf_cnt_reset_chn:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} ledc_chn_conf0_reg_t;

/** Type of chn_hpoint register
 *  High point register for channel n
 */
typedef union {
    struct {
        /** hpoint_chn : R/W; bitpos: [19:0]; default: 0;
         *  Configures high point of signal output on channel n. The output value changes to
         *  high when the selected timers has reached the value specified by this register.
         */
        uint32_t hpoint_chn:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_chn_hpoint_reg_t;

/** Type of chn_duty register
 *  Initial duty cycle register for channel n
 */
typedef union {
    struct {
        /** duty_chn : R/W; bitpos: [24:0]; default: 0;
         *  Configures the duty of signal output on channel n.
         */
        uint32_t duty_chn:25;
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
        /** duty_start_chn : R/W/SC; bitpos: [31]; default: 0;
         *  Configures whether the duty cycle fading configurations take effect.
         *  0: Not take effect
         *  1: Take effect
         */
        uint32_t duty_start_chn:1;
    };
    uint32_t val;
} ledc_chn_conf1_reg_t;

/** Type of timern_conf register
 *  Timer n configuration register
 */
typedef union {
    struct {
        /** timern_duty_res : R/W; bitpos: [4:0]; default: 0;
         *  Configures the range of the counter in timer n.
         */
        uint32_t timern_duty_res:5;
        /** clk_div_timern : R/W; bitpos: [22:5]; default: 0;
         *  Configures the divisor for the divider in timer n.The least significant eight bits
         *  represent the fractional part.
         */
        uint32_t clk_div_timern:18;
        /** timern_pause : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to pause the counter in timer n.
         *  0: Normal
         *  1: Pause
         */
        uint32_t timern_pause:1;
        /** timern_rst : R/W; bitpos: [24]; default: 1;
         *  Configures whether or not to reset timer n. The counter will show 0 after reset.
         *  0: Not reset
         *  1: Reset
         */
        uint32_t timern_rst:1;
        uint32_t reserved_25:1;
        /** timern_para_up : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to update LEDC_CLK_DIV_TIMERn and LEDC_TIMERn_DUTY_RES.
         *  0: Invalid. No effect
         *  1: Update
         */
        uint32_t timern_para_up:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ledc_timern_conf_reg_t;

/** Type of chn_gamma_conf register
 *  Ledc chn gamma config register.
 */
typedef union {
    struct {
        /** chn_gamma_entry_num : R/W; bitpos: [4:0]; default: 0;
         *  Configures the number of duty cycle fading rages for LEDC chn.
         */
        uint32_t chn_gamma_entry_num:5;
        /** chn_gamma_pause : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to pause duty cycle fading of LEDC chn.
         *  0: Invalid. No effect
         *  1: Pause
         */
        uint32_t chn_gamma_pause:1;
        /** chn_gamma_resume : WT; bitpos: [6]; default: 0;
         *  Configures whether or nor to resume duty cycle fading of LEDC chn.
         *  0: Invalid. No effect
         *  1: Resume
         */
        uint32_t chn_gamma_resume:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ledc_chn_gamma_conf_reg_t;

/** Type of evt_task_en0 register
 *  Ledc event task enable bit register0.
 */
typedef union {
    struct {
        /** evt_duty_chng_end_ch0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable the ledc_ch0_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch0_en:1;
        /** evt_duty_chng_end_ch1_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable the ledc_ch1_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch1_en:1;
        /** evt_duty_chng_end_ch2_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable the ledc_ch2_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch2_en:1;
        /** evt_duty_chng_end_ch3_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable the ledc_ch3_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch3_en:1;
        /** evt_duty_chng_end_ch4_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable the ledc_ch4_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch4_en:1;
        /** evt_duty_chng_end_ch5_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable the ledc_ch5_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch5_en:1;
        /** evt_duty_chng_end_ch6_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable the ledc_ch6_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch6_en:1;
        /** evt_duty_chng_end_ch7_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable the ledc_ch7_duty_chng_end event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_duty_chng_end_ch7_en:1;
        /** evt_ovf_cnt_pls_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable the ledc_ch0_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch0_en:1;
        /** evt_ovf_cnt_pls_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable the ledc_ch1_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch1_en:1;
        /** evt_ovf_cnt_pls_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable the ledc_ch2_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch2_en:1;
        /** evt_ovf_cnt_pls_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable the ledc_ch3_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch3_en:1;
        /** evt_ovf_cnt_pls_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable the ledc_ch4_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch4_en:1;
        /** evt_ovf_cnt_pls_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable the ledc_ch5_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch5_en:1;
        /** evt_ovf_cnt_pls_ch6_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable the ledc_ch6_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch6_en:1;
        /** evt_ovf_cnt_pls_ch7_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable the ledc_ch7_ovf_cnt_pls event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_ovf_cnt_pls_ch7_en:1;
        /** evt_time_ovf_timer0_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable the ledc_timer0_ovf event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer0_en:1;
        /** evt_time_ovf_timer1_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable the ledc_timer1_ovf event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer1_en:1;
        /** evt_time_ovf_timer2_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable the ledc_timer2_ovf event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer2_en:1;
        /** evt_time_ovf_timer3_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable the ledc_timer3_ovf event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time_ovf_timer3_en:1;
        /** evt_time0_cmp_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable the ledc_timer0_cmp event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time0_cmp_en:1;
        /** evt_time1_cmp_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable the ledc_timer1_cmp event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time1_cmp_en:1;
        /** evt_time2_cmp_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable the ledc_timer2_cmp event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time2_cmp_en:1;
        /** evt_time3_cmp_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable the ledc_timer3_cmp event.
         *  0: Disable
         *  1: Enable
         */
        uint32_t evt_time3_cmp_en:1;
        /** task_duty_scale_update_ch0_en : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to enable the ledc_ch0_duty_scale_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch0_en:1;
        /** task_duty_scale_update_ch1_en : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not to enable the ledc_ch1_duty_scale_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch1_en:1;
        /** task_duty_scale_update_ch2_en : R/W; bitpos: [26]; default: 0;
         *  Configures whether or not to enable the ledc_ch2_duty_scale_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch2_en:1;
        /** task_duty_scale_update_ch3_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to enable the ledc_ch3_duty_scale_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch3_en:1;
        /** task_duty_scale_update_ch4_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable the ledc_ch4_duty_scale_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch4_en:1;
        /** task_duty_scale_update_ch5_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable the ledc_ch5_duty_scale_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch5_en:1;
        /** task_duty_scale_update_ch6_en : R/W; bitpos: [30]; default: 0;
         *  Configures whether or not to enable the ledc_ch6_duty_scale_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_duty_scale_update_ch6_en:1;
        /** task_duty_scale_update_ch7_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not to enable the ledc_ch7_duty_scale_update task.
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
         *  Configures whether or not to enable ledc_timer0_res_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_res_update_en:1;
        /** task_timer1_res_update_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable ledc_timer1_res_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_res_update_en:1;
        /** task_timer2_res_update_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable ledc_timer2_res_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_res_update_en:1;
        /** task_timer3_res_update_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable ledc_timer3_res_update task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer3_res_update_en:1;
        /** task_timer0_cap_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable ledc_timer0_cap task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_cap_en:1;
        /** task_timer1_cap_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable ledc_timer1_cap task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_cap_en:1;
        /** task_timer2_cap_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable ledc_timer2_cap task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_cap_en:1;
        /** task_timer3_cap_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable ledc_timer3_cap task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer3_cap_en:1;
        /** task_sig_out_dis_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable ledc_ch0_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch0_en:1;
        /** task_sig_out_dis_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable ledc_ch1_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch1_en:1;
        /** task_sig_out_dis_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable ledc_ch2_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch2_en:1;
        /** task_sig_out_dis_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable ledc_ch3_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch3_en:1;
        /** task_sig_out_dis_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable ledc_ch4_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch4_en:1;
        /** task_sig_out_dis_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable ledc_ch5_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch5_en:1;
        /** task_sig_out_dis_ch6_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable ledc_ch6_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch6_en:1;
        /** task_sig_out_dis_ch7_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable ledc_ch7_sig_out_dis task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_sig_out_dis_ch7_en:1;
        /** task_ovf_cnt_rst_ch0_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable ledc_ch0_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch0_en:1;
        /** task_ovf_cnt_rst_ch1_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable ledc_ch1_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch1_en:1;
        /** task_ovf_cnt_rst_ch2_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable ledc_ch2_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch2_en:1;
        /** task_ovf_cnt_rst_ch3_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable ledc_ch3_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch3_en:1;
        /** task_ovf_cnt_rst_ch4_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable ledc_ch4_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch4_en:1;
        /** task_ovf_cnt_rst_ch5_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable ledc_ch5_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch5_en:1;
        /** task_ovf_cnt_rst_ch6_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable ledc_ch6_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch6_en:1;
        /** task_ovf_cnt_rst_ch7_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable ledc_ch7_ovf_cnt_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_ovf_cnt_rst_ch7_en:1;
        /** task_timer0_rst_en : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to enable ledc_timer0_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_rst_en:1;
        /** task_timer1_rst_en : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not to enable ledc_timer1_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_rst_en:1;
        /** task_timer2_rst_en : R/W; bitpos: [26]; default: 0;
         *  Configures whether or not to enable ledc_timer2_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_rst_en:1;
        /** task_timer3_rst_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to enable ledc_timer3_rst task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer3_rst_en:1;
        /** task_timer0_pause_resume_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable ledc_timer0_pause_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer0_pause_resume_en:1;
        /** task_timer1_pause_resume_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable ledc_timer1_pause_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer1_pause_resume_en:1;
        /** task_timer2_pause_resume_en : R/W; bitpos: [30]; default: 0;
         *  Configures whether or not to enable ledc_timer2_pause_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_timer2_pause_resume_en:1;
        /** task_timer3_pause_resume_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not to enable ledc_timer3_pause_resume task.
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
        /** task_gamma_restart_ch0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable ledc_ch0_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch0_en:1;
        /** task_gamma_restart_ch1_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable ledc_ch1_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch1_en:1;
        /** task_gamma_restart_ch2_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable ledc_ch2_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch2_en:1;
        /** task_gamma_restart_ch3_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable ledc_ch3_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch3_en:1;
        /** task_gamma_restart_ch4_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable ledc_ch4_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch4_en:1;
        /** task_gamma_restart_ch5_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable ledc_ch5_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch5_en:1;
        /** task_gamma_restart_ch6_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable ledc_ch6_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch6_en:1;
        /** task_gamma_restart_ch7_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable ledc_ch7_gamma_restart task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_restart_ch7_en:1;
        /** task_gamma_pause_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable ledc_ch0_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch0_en:1;
        /** task_gamma_pause_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable ledc_ch1_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch1_en:1;
        /** task_gamma_pause_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable ledc_ch2_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch2_en:1;
        /** task_gamma_pause_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable ledc_ch3_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch3_en:1;
        /** task_gamma_pause_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable ledc_ch4_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch4_en:1;
        /** task_gamma_pause_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable ledc_ch5_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch5_en:1;
        /** task_gamma_pause_ch6_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable ledc_ch6_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch6_en:1;
        /** task_gamma_pause_ch7_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable ledc_ch7_gamma_pause task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_pause_ch7_en:1;
        /** task_gamma_resume_ch0_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable ledc_ch0_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch0_en:1;
        /** task_gamma_resume_ch1_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable ledc_ch1_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch1_en:1;
        /** task_gamma_resume_ch2_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable ledc_ch2_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch2_en:1;
        /** task_gamma_resume_ch3_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable ledc_ch3_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch3_en:1;
        /** task_gamma_resume_ch4_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable ledc_ch4_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch4_en:1;
        /** task_gamma_resume_ch5_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable ledc_ch5_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch5_en:1;
        /** task_gamma_resume_ch6_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable ledc_ch6_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch6_en:1;
        /** task_gamma_resume_ch7_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable ledc_ch7_gamma_resume task.
         *  0: Disable
         *  1: Enable
         */
        uint32_t task_gamma_resume_ch7_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} ledc_evt_task_en2_reg_t;

/** Type of timern_cmp register
 *  Ledc timern compare value register.
 */
typedef union {
    struct {
        /** timern_cmp : R/W; bitpos: [19:0]; default: 0;
         *  Configures the comparison value for LEDC timern.
         */
        uint32_t timern_cmp:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timern_cmp_reg_t;

/** Type of conf register
 *  LEDC global configuration register
 */
typedef union {
    struct {
        /** apb_clk_sel : R/W; bitpos: [1:0]; default: 0;
         *  Configures the clock source for the four timers.
         *  0: APB_CLK
         *  1: RC_FAST_CLK
         *  2: XTAL_CLK
         *  3: Invalid. No clock
         */
        uint32_t apb_clk_sel:2;
        /** gamma_ram_clk_en_ch0 : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to open LEDC ch0 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch0 gamma ram
         *  1: Force open the clock gate for LEDC ch0 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch0:1;
        /** gamma_ram_clk_en_ch1 : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to open LEDC ch1 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch1 gamma ram
         *  1: Force open the clock gate for LEDC ch1 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch1:1;
        /** gamma_ram_clk_en_ch2 : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to open LEDC ch2 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch2 gamma ram
         *  1: Force open the clock gate for LEDC ch2 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch2:1;
        /** gamma_ram_clk_en_ch3 : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to open LEDC ch3 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch3 gamma ram
         *  1: Force open the clock gate for LEDC ch3 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch3:1;
        /** gamma_ram_clk_en_ch4 : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to open LEDC ch4 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch4 gamma ram
         *  1: Force open the clock gate for LEDC ch4 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch4:1;
        /** gamma_ram_clk_en_ch5 : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to open LEDC ch5 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch5 gamma ram
         *  1: Force open the clock gate for LEDC ch5 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch5:1;
        /** gamma_ram_clk_en_ch6 : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to open LEDC ch6 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch6 gamma ram
         *  1: Force open the clock gate for LEDC ch6 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch6:1;
        /** gamma_ram_clk_en_ch7 : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to open LEDC ch7 gamma ram clock gate.
         *  0: Open the clock gate only when application writes or reads LEDC ch7 gamma ram
         *  1: Force open the clock gate for LEDC ch7 gamma ram
         */
        uint32_t gamma_ram_clk_en_ch7:1;
        uint32_t reserved_10:21;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not to open register clock gate.
         *  0: Open the clock gate only when application writes registers
         *  1: Force open the clock gate for register
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} ledc_conf_reg_t;


/** Group: Status Register */
/** Type of chn_duty_r register
 *  Current duty cycle register for channel n
 */
typedef union {
    struct {
        /** duty_chn_r : RO; bitpos: [24:0]; default: 0;
         *  Represents the current duty of output signal on channel n.
         */
        uint32_t duty_chn_r:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} ledc_chn_duty_r_reg_t;

/** Type of timern_value register
 *  Timer n current counter value register
 */
typedef union {
    struct {
        /** timern_cnt : RO; bitpos: [19:0]; default: 0;
         *  Represents the current counter value of timer n.
         */
        uint32_t timern_cnt:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timern_value_reg_t;

/** Type of timern_cnt_cap register
 *  Ledc timern captured count value register.
 */
typedef union {
    struct {
        /** timern_cnt_cap : RO; bitpos: [19:0]; default: 0;
         *  Represents the captured LEDC timern count value.
         */
        uint32_t timern_cnt_cap:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timern_cnt_cap_reg_t;


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
        /** ledc_date : R/W; bitpos: [27:0]; default: 37765152;
         *  Configures the version.
         */
        uint32_t ledc_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ledc_date_reg_t;


typedef struct {
    volatile ledc_chn_conf0_reg_t ch0_conf0;
    volatile ledc_chn_hpoint_reg_t ch0_hpoint;
    volatile ledc_chn_duty_reg_t ch0_duty;
    volatile ledc_chn_conf1_reg_t ch0_conf1;
    volatile ledc_chn_duty_r_reg_t ch0_duty_r;
    volatile ledc_chn_conf0_reg_t ch1_conf0;
    volatile ledc_chn_hpoint_reg_t ch1_hpoint;
    volatile ledc_chn_duty_reg_t ch1_duty;
    volatile ledc_chn_conf1_reg_t ch1_conf1;
    volatile ledc_chn_duty_r_reg_t ch1_duty_r;
    volatile ledc_chn_conf0_reg_t ch2_conf0;
    volatile ledc_chn_hpoint_reg_t ch2_hpoint;
    volatile ledc_chn_duty_reg_t ch2_duty;
    volatile ledc_chn_conf1_reg_t ch2_conf1;
    volatile ledc_chn_duty_r_reg_t ch2_duty_r;
    volatile ledc_chn_conf0_reg_t ch3_conf0;
    volatile ledc_chn_hpoint_reg_t ch3_hpoint;
    volatile ledc_chn_duty_reg_t ch3_duty;
    volatile ledc_chn_conf1_reg_t ch3_conf1;
    volatile ledc_chn_duty_r_reg_t ch3_duty_r;
    volatile ledc_chn_conf0_reg_t ch4_conf0;
    volatile ledc_chn_hpoint_reg_t ch4_hpoint;
    volatile ledc_chn_duty_reg_t ch4_duty;
    volatile ledc_chn_conf1_reg_t ch4_conf1;
    volatile ledc_chn_duty_r_reg_t ch4_duty_r;
    volatile ledc_chn_conf0_reg_t ch5_conf0;
    volatile ledc_chn_hpoint_reg_t ch5_hpoint;
    volatile ledc_chn_duty_reg_t ch5_duty;
    volatile ledc_chn_conf1_reg_t ch5_conf1;
    volatile ledc_chn_duty_r_reg_t ch5_duty_r;
    volatile ledc_chn_conf0_reg_t ch6_conf0;
    volatile ledc_chn_hpoint_reg_t ch6_hpoint;
    volatile ledc_chn_duty_reg_t ch6_duty;
    volatile ledc_chn_conf1_reg_t ch6_conf1;
    volatile ledc_chn_duty_r_reg_t ch6_duty_r;
    volatile ledc_chn_conf0_reg_t ch7_conf0;
    volatile ledc_chn_hpoint_reg_t ch7_hpoint;
    volatile ledc_chn_duty_reg_t ch7_duty;
    volatile ledc_chn_conf1_reg_t ch7_conf1;
    volatile ledc_chn_duty_r_reg_t ch7_duty_r;
    volatile ledc_timern_conf_reg_t timer0_conf;
    volatile ledc_timern_value_reg_t timer0_value;
    volatile ledc_timern_conf_reg_t timer1_conf;
    volatile ledc_timern_value_reg_t timer1_value;
    volatile ledc_timern_conf_reg_t timer2_conf;
    volatile ledc_timern_value_reg_t timer2_value;
    volatile ledc_timern_conf_reg_t timer3_conf;
    volatile ledc_timern_value_reg_t timer3_value;
    volatile ledc_int_raw_reg_t int_raw;
    volatile ledc_int_st_reg_t int_st;
    volatile ledc_int_ena_reg_t int_ena;
    volatile ledc_int_clr_reg_t int_clr;
    uint32_t reserved_0d0[12];
    volatile ledc_chn_gamma_conf_reg_t chn_gamma_conf[8];
    volatile ledc_evt_task_en0_reg_t evt_task_en0;
    volatile ledc_evt_task_en1_reg_t evt_task_en1;
    volatile ledc_evt_task_en2_reg_t evt_task_en2;
    uint32_t reserved_12c[5];
    volatile ledc_timern_cmp_reg_t timern_cmp[4];
    volatile ledc_timern_cnt_cap_reg_t timern_cnt_cap[4];
    uint32_t reserved_160[4];
    volatile ledc_conf_reg_t conf;
    volatile ledc_date_reg_t date;
} ledc_dev_t;

extern ledc_dev_t LEDC;

#ifndef __cplusplus
_Static_assert(sizeof(ledc_dev_t) == 0x178, "Invalid size of ledc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
