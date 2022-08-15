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

/** Group: Configuration Register */
/** Type of chn_conf0 register
 *  Configuration register 0 for channel n
 */
typedef union {
    struct {
        /** timer_sel : R/W; bitpos: [1:0]; default: 0;
         *  This field is used to select one of timers for channel n.
         *
         *  0: select timer0, 1: select timer1, 2: select timer2, 3: select timer3
         */
        uint32_t timer_sel:2;
        /** sig_out_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable signal output on channel n.
         */
        uint32_t sig_out_en:1;
        /** idle_lv : R/W; bitpos: [3]; default: 0;
         *  This bit is used to control the output value when channel n is inactive (when
         *  LEDC_SIG_OUT_EN_CHn is 0).
         */
        uint32_t idle_lv:1;
        /** para_up : WT; bitpos: [4]; default: 0;
         *  This bit is used to update LEDC_HPOINT_CHn, LEDC_DUTY_START_CHn,
         *  LEDC_SIG_OUT_EN_CHn, LEDC_TIMER_SEL_CHn, LEDC_DUTY_NUM_CHn, LEDC_DUTY_CYCLE_CHn,
         *  LEDC_DUTY_SCALE_CHn, LEDC_DUTY_INC_CHn, and LEDC_OVF_CNT_EN_CHn fields for channel
         *  n, and will be automatically cleared by hardware.
         */
        uint32_t para_up:1;
        /** ovf_num : R/W; bitpos: [14:5]; default: 0;
         *  This register is used to configure the maximum times of overflow minus 1.
         *
         *  The LEDC_OVF_CNT_CHn_INT interrupt will be triggered when channel n overflows for
         *  (LEDC_OVF_NUM_CHn + 1) times.
         */
        uint32_t ovf_num:10;
        /** ovf_cnt_en : R/W; bitpos: [15]; default: 0;
         *  This bit is used to enable the ovf_cnt of channel n.
         */
        uint32_t ovf_cnt_en:1;
        /** ovf_cnt_reset : WT; bitpos: [16]; default: 0;
         *  Set this bit to reset the ovf_cnt of channel n.
         */
        uint32_t ovf_cnt_reset:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} ledc_chn_conf0_reg_t;

/** Type of chn_conf1 register
 *  Configuration register 1 for channel n
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** duty_start : R/W/SC; bitpos: [31]; default: 0;
         *  Other configured fields in LEDC_CHn_CONF1_REG will start to take effect when this
         *  bit is set to 1.
         */
        uint32_t duty_start:1;
    };
    uint32_t val;
} ledc_chn_conf1_reg_t;

/** Type of evt_task_en0 register
 *  Ledc event task enable bit register0.
 */
typedef union {
    struct {
        /** evt_duty_chng_end_ch0_en : R/W; bitpos: [0]; default: 0;
         *  Ledc ch0 duty change end event enable register, write 1 to enable this event.
         */
        uint32_t evt_duty_chng_end_ch0_en:1;
        /** evt_duty_chng_end_ch1_en : R/W; bitpos: [1]; default: 0;
         *  Ledc ch1 duty change end event enable register, write 1 to enable this event.
         */
        uint32_t evt_duty_chng_end_ch1_en:1;
        /** evt_duty_chng_end_ch2_en : R/W; bitpos: [2]; default: 0;
         *  Ledc ch2 duty change end event enable register, write 1 to enable this event.
         */
        uint32_t evt_duty_chng_end_ch2_en:1;
        /** evt_duty_chng_end_ch3_en : R/W; bitpos: [3]; default: 0;
         *  Ledc ch3 duty change end event enable register, write 1 to enable this event.
         */
        uint32_t evt_duty_chng_end_ch3_en:1;
        /** evt_duty_chng_end_ch4_en : R/W; bitpos: [4]; default: 0;
         *  Ledc ch4 duty change end event enable register, write 1 to enable this event.
         */
        uint32_t evt_duty_chng_end_ch4_en:1;
        /** evt_duty_chng_end_ch5_en : R/W; bitpos: [5]; default: 0;
         *  Ledc ch5 duty change end event enable register, write 1 to enable this event.
         */
        uint32_t evt_duty_chng_end_ch5_en:1;
        uint32_t reserved_6:2;
        /** evt_ovf_cnt_pls_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Ledc ch0 overflow count pulse event enable register, write 1 to enable this event.
         */
        uint32_t evt_ovf_cnt_pls_ch0_en:1;
        /** evt_ovf_cnt_pls_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Ledc ch1 overflow count pulse event enable register, write 1 to enable this event.
         */
        uint32_t evt_ovf_cnt_pls_ch1_en:1;
        /** evt_ovf_cnt_pls_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Ledc ch2 overflow count pulse event enable register, write 1 to enable this event.
         */
        uint32_t evt_ovf_cnt_pls_ch2_en:1;
        /** evt_ovf_cnt_pls_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Ledc ch3 overflow count pulse event enable register, write 1 to enable this event.
         */
        uint32_t evt_ovf_cnt_pls_ch3_en:1;
        /** evt_ovf_cnt_pls_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Ledc ch4 overflow count pulse event enable register, write 1 to enable this event.
         */
        uint32_t evt_ovf_cnt_pls_ch4_en:1;
        /** evt_ovf_cnt_pls_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Ledc ch5 overflow count pulse event enable register, write 1 to enable this event.
         */
        uint32_t evt_ovf_cnt_pls_ch5_en:1;
        uint32_t reserved_14:2;
        /** evt_time_ovf_timer0_en : R/W; bitpos: [16]; default: 0;
         *  Ledc timer0 overflow event enable register, write 1 to enable this event.
         */
        uint32_t evt_time_ovf_timer0_en:1;
        /** evt_time_ovf_timer1_en : R/W; bitpos: [17]; default: 0;
         *  Ledc timer1 overflow event enable register, write 1 to enable this event.
         */
        uint32_t evt_time_ovf_timer1_en:1;
        /** evt_time_ovf_timer2_en : R/W; bitpos: [18]; default: 0;
         *  Ledc timer2 overflow event enable register, write 1 to enable this event.
         */
        uint32_t evt_time_ovf_timer2_en:1;
        /** evt_time_ovf_timer3_en : R/W; bitpos: [19]; default: 0;
         *  Ledc timer3 overflow event enable register, write 1 to enable this event.
         */
        uint32_t evt_time_ovf_timer3_en:1;
        /** evt_time0_cmp_en : R/W; bitpos: [20]; default: 0;
         *  Ledc timer0 compare event enable register, write 1 to enable this event.
         */
        uint32_t evt_time0_cmp_en:1;
        /** evt_time1_cmp_en : R/W; bitpos: [21]; default: 0;
         *  Ledc timer1 compare event enable register, write 1 to enable this event.
         */
        uint32_t evt_time1_cmp_en:1;
        /** evt_time2_cmp_en : R/W; bitpos: [22]; default: 0;
         *  Ledc timer2 compare event enable register, write 1 to enable this event.
         */
        uint32_t evt_time2_cmp_en:1;
        /** evt_time3_cmp_en : R/W; bitpos: [23]; default: 0;
         *  Ledc timer3 compare event enable register, write 1 to enable this event.
         */
        uint32_t evt_time3_cmp_en:1;
        /** task_duty_scale_update_ch0_en : R/W; bitpos: [24]; default: 0;
         *  Ledc ch0 duty scale update task enable register, write 1 to enable this task.
         */
        uint32_t task_duty_scale_update_ch0_en:1;
        /** task_duty_scale_update_ch1_en : R/W; bitpos: [25]; default: 0;
         *  Ledc ch1 duty scale update task enable register, write 1 to enable this task.
         */
        uint32_t task_duty_scale_update_ch1_en:1;
        /** task_duty_scale_update_ch2_en : R/W; bitpos: [26]; default: 0;
         *  Ledc ch2 duty scale update task enable register, write 1 to enable this task.
         */
        uint32_t task_duty_scale_update_ch2_en:1;
        /** task_duty_scale_update_ch3_en : R/W; bitpos: [27]; default: 0;
         *  Ledc ch3 duty scale update task enable register, write 1 to enable this task.
         */
        uint32_t task_duty_scale_update_ch3_en:1;
        /** task_duty_scale_update_ch4_en : R/W; bitpos: [28]; default: 0;
         *  Ledc ch4 duty scale update task enable register, write 1 to enable this task.
         */
        uint32_t task_duty_scale_update_ch4_en:1;
        /** task_duty_scale_update_ch5_en : R/W; bitpos: [29]; default: 0;
         *  Ledc ch5 duty scale update task enable register, write 1 to enable this task.
         */
        uint32_t task_duty_scale_update_ch5_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} ledc_evt_task_en0_reg_t;

/** Type of evt_task_en1 register
 *  Ledc event task enable bit register1.
 */
typedef union {
    struct {
        /** task_timer0_res_update_en : R/W; bitpos: [0]; default: 0;
         *  Ledc timer0 res update task enable register, write 1 to enable this task.
         */
        uint32_t task_timer0_res_update_en:1;
        /** task_timer1_res_update_en : R/W; bitpos: [1]; default: 0;
         *  Ledc timer1 res update task enable register, write 1 to enable this task.
         */
        uint32_t task_timer1_res_update_en:1;
        /** task_timer2_res_update_en : R/W; bitpos: [2]; default: 0;
         *  Ledc timer2 res update task enable register, write 1 to enable this task.
         */
        uint32_t task_timer2_res_update_en:1;
        /** task_timer3_res_update_en : R/W; bitpos: [3]; default: 0;
         *  Ledc timer3 res update task enable register, write 1 to enable this task.
         */
        uint32_t task_timer3_res_update_en:1;
        /** task_timer0_cap_en : R/W; bitpos: [4]; default: 0;
         *  Ledc timer0 capture task enable register, write 1 to enable this task.
         */
        uint32_t task_timer0_cap_en:1;
        /** task_timer1_cap_en : R/W; bitpos: [5]; default: 0;
         *  Ledc timer1 capture task enable register, write 1 to enable this task.
         */
        uint32_t task_timer1_cap_en:1;
        /** task_timer2_cap_en : R/W; bitpos: [6]; default: 0;
         *  Ledc timer2 capture task enable register, write 1 to enable this task.
         */
        uint32_t task_timer2_cap_en:1;
        /** task_timer3_cap_en : R/W; bitpos: [7]; default: 0;
         *  Ledc timer3 capture task enable register, write 1 to enable this task.
         */
        uint32_t task_timer3_cap_en:1;
        /** task_sig_out_dis_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Ledc ch0 signal out disable task enable register, write 1 to enable this task.
         */
        uint32_t task_sig_out_dis_ch0_en:1;
        /** task_sig_out_dis_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Ledc ch1 signal out disable task enable register, write 1 to enable this task.
         */
        uint32_t task_sig_out_dis_ch1_en:1;
        /** task_sig_out_dis_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Ledc ch2 signal out disable task enable register, write 1 to enable this task.
         */
        uint32_t task_sig_out_dis_ch2_en:1;
        /** task_sig_out_dis_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Ledc ch3 signal out disable task enable register, write 1 to enable this task.
         */
        uint32_t task_sig_out_dis_ch3_en:1;
        /** task_sig_out_dis_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Ledc ch4 signal out disable task enable register, write 1 to enable this task.
         */
        uint32_t task_sig_out_dis_ch4_en:1;
        /** task_sig_out_dis_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Ledc ch5 signal out disable task enable register, write 1 to enable this task.
         */
        uint32_t task_sig_out_dis_ch5_en:1;
        uint32_t reserved_14:2;
        /** task_ovf_cnt_rst_ch0_en : R/W; bitpos: [16]; default: 0;
         *  Ledc ch0 overflow count reset task enable register, write 1 to enable this task.
         */
        uint32_t task_ovf_cnt_rst_ch0_en:1;
        /** task_ovf_cnt_rst_ch1_en : R/W; bitpos: [17]; default: 0;
         *  Ledc ch1 overflow count reset task enable register, write 1 to enable this task.
         */
        uint32_t task_ovf_cnt_rst_ch1_en:1;
        /** task_ovf_cnt_rst_ch2_en : R/W; bitpos: [18]; default: 0;
         *  Ledc ch2 overflow count reset task enable register, write 1 to enable this task.
         */
        uint32_t task_ovf_cnt_rst_ch2_en:1;
        /** task_ovf_cnt_rst_ch3_en : R/W; bitpos: [19]; default: 0;
         *  Ledc ch3 overflow count reset task enable register, write 1 to enable this task.
         */
        uint32_t task_ovf_cnt_rst_ch3_en:1;
        /** task_ovf_cnt_rst_ch4_en : R/W; bitpos: [20]; default: 0;
         *  Ledc ch4 overflow count reset task enable register, write 1 to enable this task.
         */
        uint32_t task_ovf_cnt_rst_ch4_en:1;
        /** task_ovf_cnt_rst_ch5_en : R/W; bitpos: [21]; default: 0;
         *  Ledc ch5 overflow count reset task enable register, write 1 to enable this task.
         */
        uint32_t task_ovf_cnt_rst_ch5_en:1;
        uint32_t reserved_22:2;
        /** task_timer0_rst_en : R/W; bitpos: [24]; default: 0;
         *  Ledc timer0 reset task enable register, write 1 to enable this task.
         */
        uint32_t task_timer0_rst_en:1;
        /** task_timer1_rst_en : R/W; bitpos: [25]; default: 0;
         *  Ledc timer1 reset task enable register, write 1 to enable this task.
         */
        uint32_t task_timer1_rst_en:1;
        /** task_timer2_rst_en : R/W; bitpos: [26]; default: 0;
         *  Ledc timer2 reset task enable register, write 1 to enable this task.
         */
        uint32_t task_timer2_rst_en:1;
        /** task_timer3_rst_en : R/W; bitpos: [27]; default: 0;
         *  Ledc timer3 reset task enable register, write 1 to enable this task.
         */
        uint32_t task_timer3_rst_en:1;
        /** task_timer0_pause_resume_en : R/W; bitpos: [28]; default: 0;
         *  Ledc timer0 pause resume task enable register, write 1 to enable this task.
         */
        uint32_t task_timer0_pause_resume_en:1;
        /** task_timer1_pause_resume_en : R/W; bitpos: [29]; default: 0;
         *  Ledc timer1 pause resume task enable register, write 1 to enable this task.
         */
        uint32_t task_timer1_pause_resume_en:1;
        /** task_timer2_pause_resume_en : R/W; bitpos: [30]; default: 0;
         *  Ledc timer2 pause resume task enable register, write 1 to enable this task.
         */
        uint32_t task_timer2_pause_resume_en:1;
        /** task_timer3_pause_resume_en : R/W; bitpos: [31]; default: 0;
         *  Ledc timer3 pause resume task enable register, write 1 to enable this task.
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
         *  Ledc ch0 gamma restart task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_restart_ch0_en:1;
        /** task_gamma_restart_ch1_en : R/W; bitpos: [1]; default: 0;
         *  Ledc ch1 gamma restart task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_restart_ch1_en:1;
        /** task_gamma_restart_ch2_en : R/W; bitpos: [2]; default: 0;
         *  Ledc ch2 gamma restart task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_restart_ch2_en:1;
        /** task_gamma_restart_ch3_en : R/W; bitpos: [3]; default: 0;
         *  Ledc ch3 gamma restart task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_restart_ch3_en:1;
        /** task_gamma_restart_ch4_en : R/W; bitpos: [4]; default: 0;
         *  Ledc ch4 gamma restart task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_restart_ch4_en:1;
        /** task_gamma_restart_ch5_en : R/W; bitpos: [5]; default: 0;
         *  Ledc ch5 gamma restart task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_restart_ch5_en:1;
        uint32_t reserved_6:2;
        /** task_gamma_pause_ch0_en : R/W; bitpos: [8]; default: 0;
         *  Ledc ch0 gamma pause task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_pause_ch0_en:1;
        /** task_gamma_pause_ch1_en : R/W; bitpos: [9]; default: 0;
         *  Ledc ch1 gamma pause task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_pause_ch1_en:1;
        /** task_gamma_pause_ch2_en : R/W; bitpos: [10]; default: 0;
         *  Ledc ch2 gamma pause task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_pause_ch2_en:1;
        /** task_gamma_pause_ch3_en : R/W; bitpos: [11]; default: 0;
         *  Ledc ch3 gamma pause task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_pause_ch3_en:1;
        /** task_gamma_pause_ch4_en : R/W; bitpos: [12]; default: 0;
         *  Ledc ch4 gamma pause task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_pause_ch4_en:1;
        /** task_gamma_pause_ch5_en : R/W; bitpos: [13]; default: 0;
         *  Ledc ch5 gamma pause task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_pause_ch5_en:1;
        uint32_t reserved_14:2;
        /** task_gamma_resume_ch0_en : R/W; bitpos: [16]; default: 0;
         *  Ledc ch0 gamma resume task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_resume_ch0_en:1;
        /** task_gamma_resume_ch1_en : R/W; bitpos: [17]; default: 0;
         *  Ledc ch1 gamma resume task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_resume_ch1_en:1;
        /** task_gamma_resume_ch2_en : R/W; bitpos: [18]; default: 0;
         *  Ledc ch2 gamma resume task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_resume_ch2_en:1;
        /** task_gamma_resume_ch3_en : R/W; bitpos: [19]; default: 0;
         *  Ledc ch3 gamma resume task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_resume_ch3_en:1;
        /** task_gamma_resume_ch4_en : R/W; bitpos: [20]; default: 0;
         *  Ledc ch4 gamma resume task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_resume_ch4_en:1;
        /** task_gamma_resume_ch5_en : R/W; bitpos: [21]; default: 0;
         *  Ledc ch5 gamma resume task enable register, write 1 to enable this task.
         */
        uint32_t task_gamma_resume_ch5_en:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} ledc_evt_task_en2_reg_t;

/** Type of timern_cmp register
 *  Ledc timern compare value register.
 */
typedef union {
    struct {
        /** cmp : R/W; bitpos: [19:0]; default: 0;
         *  This register stores ledc timern compare value.
         */
        uint32_t cmp:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timern_cmp_reg_t;

/** Type of timern_cnt_cap register
 *  Ledc timern count value capture register.
 */
typedef union {
    struct {
        /** cnt_cap : RO; bitpos: [19:0]; default: 0;
         *  This register stores ledc timern count value.
         */
        uint32_t cnt_cap:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timern_cnt_cap_reg_t;

/** Type of conf register
 *  Global ledc configuration register
 */
typedef union {
    struct {
        /** apb_clk_sel : R/W; bitpos: [1:0]; default: 0;
         *  This bit is used to select clock source for the 4 timers .
         *
         *  2'd1: APB_CLK 2'd2: RTC8M_CLK 2'd3: XTAL_CLK
         */
        uint32_t apb_clk_sel:2;
        /** gamma_ram_clk_en_ch0 : R/W; bitpos: [2]; default: 0;
         *  This bit is used to control clock.
         *
         *  1'b1: Force clock on for gamma ram. 1'h0: Support clock only when application
         *  writes or read gamma ram.
         */
        uint32_t gamma_ram_clk_en_ch0:1;
        /** gamma_ram_clk_en_ch1 : R/W; bitpos: [3]; default: 0;
         *  This bit is used to control clock.
         *
         *  1'b1: Force clock on for gamma ram. 1'h0: Support clock only when application
         *  writes or read gamma ram.
         */
        uint32_t gamma_ram_clk_en_ch1:1;
        /** gamma_ram_clk_en_ch2 : R/W; bitpos: [4]; default: 0;
         *  This bit is used to control clock.
         *
         *  1'b1: Force clock on for gamma ram. 1'h0: Support clock only when application
         *  writes or read gamma ram.
         */
        uint32_t gamma_ram_clk_en_ch2:1;
        /** gamma_ram_clk_en_ch3 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to control clock.
         *
         *  1'b1: Force clock on for gamma ram. 1'h0: Support clock only when application
         *  writes or read gamma ram.
         */
        uint32_t gamma_ram_clk_en_ch3:1;
        /** gamma_ram_clk_en_ch4 : R/W; bitpos: [6]; default: 0;
         *  This bit is used to control clock.
         *
         *  1'b1: Force clock on for gamma ram. 1'h0: Support clock only when application
         *  writes or read gamma ram.
         */
        uint32_t gamma_ram_clk_en_ch4:1;
        /** gamma_ram_clk_en_ch5 : R/W; bitpos: [7]; default: 0;
         *  This bit is used to control clock.
         *
         *  1'b1: Force clock on for gamma ram. 1'h0: Support clock only when application
         *  writes or read gamma ram.
         */
        uint32_t gamma_ram_clk_en_ch5:1;
        uint32_t reserved_8:23;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  This bit is used to control clock.
         *
         *  1'b1: Force clock on for register. 1'h0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} ledc_conf_reg_t;


/** Group: Hpoint Register */
/** Type of chn_hpoint register
 *  High point register for channel n
 */
typedef union {
    struct {
        /** hpoint : R/W; bitpos: [19:0]; default: 0;
         *  The output value changes to high when the selected timers has reached the value
         *  specified by this register.
         */
        uint32_t hpoint:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_chn_hpoint_reg_t;


/** Group: Duty Cycle Register */
/** Type of chn_duty register
 *  Initial duty cycle for channel n
 */
typedef union {
    struct {
        /** duty : R/W; bitpos: [24:0]; default: 0;
         *  This register is used to change the output duty by controlling the Lpoint.
         *
         *  The output value turns to low when the selected timers has reached the Lpoint.
         */
        uint32_t duty:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} ledc_chn_duty_reg_t;

/** Type of chn_duty_r register
 *  Current duty cycle for channel n
 */
typedef union {
    struct {
        /** duty_r : RO; bitpos: [24:0]; default: 0;
         *  This register stores the current duty of output signal on channel n.
         */
        uint32_t duty_r:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} ledc_chn_duty_r_reg_t;


/** Group: Timer Register */
/** Type of timern_conf register
 *  Timer n configuration
 */
typedef union {
    struct {
        /** duty_res : R/W; bitpos: [4:0]; default: 0;
         *  This register is used to control the range of the counter in timer n.
         */
        uint32_t duty_res:5;
        /** clk_div : R/W; bitpos: [22:5]; default: 0;
         *  This register is used to configure the divisor for the divider in timer n.
         *
         *  The least significant eight bits represent the fractional part.
         */
        uint32_t clk_div:18;
        /** pause : R/W; bitpos: [23]; default: 0;
         *  This bit is used to suspend the counter in timer n.
         */
        uint32_t pause:1;
        /** rst : R/W; bitpos: [24]; default: 1;
         *  This bit is used to reset timer n. The counter will show 0 after reset.
         */
        uint32_t rst:1;
        /** tick_sel : R/W; bitpos: [25]; default: 0;
         *  This bit is used to select clock for timer n. When this bit is set to 1
         *  LEDC_APB_CLK_SEL[1:0] should be 1, otherwise the timer clock may be not accurate.
         *
         *  1'h0: SLOW_CLK 1'h1: REF_TICK
         */
        uint32_t tick_sel:1;
        /** para_up : WT; bitpos: [26]; default: 0;
         *  Set this bit to update LEDC_CLK_DIV_TIMERn and LEDC_TIMERn_DUTY_RES.
         */
        uint32_t para_up:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ledc_timerx_conf_reg_t;

/** Type of timern_value register
 *  Timer n current counter value
 */
typedef union {
    struct {
        /** timer_cnt : RO; bitpos: [19:0]; default: 0;
         *  This register stores the current counter value of timer n.
         */
        uint32_t timer_cnt:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} ledc_timerx_value_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** timer0_ovf_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  Triggered when the timer0 has reached its maximum counter value.
         */
        uint32_t timer0_ovf_int_raw:1;
        /** timer1_ovf_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  Triggered when the timer1 has reached its maximum counter value.
         */
        uint32_t timer1_ovf_int_raw:1;
        /** timer2_ovf_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  Triggered when the timer2 has reached its maximum counter value.
         */
        uint32_t timer2_ovf_int_raw:1;
        /** timer3_ovf_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  Triggered when the timer3 has reached its maximum counter value.
         */
        uint32_t timer3_ovf_int_raw:1;
        /** duty_chng_end_ch0_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  Interrupt raw bit for channel 0. Triggered when the gradual change of duty has
         *  finished.
         */
        uint32_t duty_chng_end_ch0_int_raw:1;
        /** duty_chng_end_ch1_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  Interrupt raw bit for channel 1. Triggered when the gradual change of duty has
         *  finished.
         */
        uint32_t duty_chng_end_ch1_int_raw:1;
        /** duty_chng_end_ch2_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  Interrupt raw bit for channel 2. Triggered when the gradual change of duty has
         *  finished.
         */
        uint32_t duty_chng_end_ch2_int_raw:1;
        /** duty_chng_end_ch3_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  Interrupt raw bit for channel 3. Triggered when the gradual change of duty has
         *  finished.
         */
        uint32_t duty_chng_end_ch3_int_raw:1;
        /** duty_chng_end_ch4_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  Interrupt raw bit for channel 4. Triggered when the gradual change of duty has
         *  finished.
         */
        uint32_t duty_chng_end_ch4_int_raw:1;
        /** duty_chng_end_ch5_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  Interrupt raw bit for channel 5. Triggered when the gradual change of duty has
         *  finished.
         */
        uint32_t duty_chng_end_ch5_int_raw:1;
        uint32_t reserved_10:2;
        /** ovf_cnt_ch0_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  Interrupt raw bit for channel 0. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH0.
         */
        uint32_t ovf_cnt_ch0_int_raw:1;
        /** ovf_cnt_ch1_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  Interrupt raw bit for channel 1. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH1.
         */
        uint32_t ovf_cnt_ch1_int_raw:1;
        /** ovf_cnt_ch2_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  Interrupt raw bit for channel 2. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH2.
         */
        uint32_t ovf_cnt_ch2_int_raw:1;
        /** ovf_cnt_ch3_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  Interrupt raw bit for channel 3. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH3.
         */
        uint32_t ovf_cnt_ch3_int_raw:1;
        /** ovf_cnt_ch4_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  Interrupt raw bit for channel 4. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH4.
         */
        uint32_t ovf_cnt_ch4_int_raw:1;
        /** ovf_cnt_ch5_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  Interrupt raw bit for channel 5. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH5.
         */
        uint32_t ovf_cnt_ch5_int_raw:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} ledc_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** timer0_ovf_int_st : RO; bitpos: [0]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_TIMER0_OVF_INT interrupt when
         *  LEDC_TIMER0_OVF_INT_ENA is set to 1.
         */
        uint32_t timer0_ovf_int_st:1;
        /** timer1_ovf_int_st : RO; bitpos: [1]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_TIMER1_OVF_INT interrupt when
         *  LEDC_TIMER1_OVF_INT_ENA is set to 1.
         */
        uint32_t timer1_ovf_int_st:1;
        /** timer2_ovf_int_st : RO; bitpos: [2]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_TIMER2_OVF_INT interrupt when
         *  LEDC_TIMER2_OVF_INT_ENA is set to 1.
         */
        uint32_t timer2_ovf_int_st:1;
        /** timer3_ovf_int_st : RO; bitpos: [3]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_TIMER3_OVF_INT interrupt when
         *  LEDC_TIMER3_OVF_INT_ENA is set to 1.
         */
        uint32_t timer3_ovf_int_st:1;
        /** duty_chng_end_ch0_int_st : RO; bitpos: [4]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH0_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH0_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch0_int_st:1;
        /** duty_chng_end_ch1_int_st : RO; bitpos: [5]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH1_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH1_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch1_int_st:1;
        /** duty_chng_end_ch2_int_st : RO; bitpos: [6]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH2_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH2_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch2_int_st:1;
        /** duty_chng_end_ch3_int_st : RO; bitpos: [7]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH3_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH3_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch3_int_st:1;
        /** duty_chng_end_ch4_int_st : RO; bitpos: [8]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH4_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH4_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch4_int_st:1;
        /** duty_chng_end_ch5_int_st : RO; bitpos: [9]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH5_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH5_INT_ENA is set to 1.
         */
        uint32_t duty_chng_end_ch5_int_st:1;
        uint32_t reserved_10:2;
        /** ovf_cnt_ch0_int_st : RO; bitpos: [12]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH0_INT interrupt when
         *  LEDC_OVF_CNT_CH0_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch0_int_st:1;
        /** ovf_cnt_ch1_int_st : RO; bitpos: [13]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH1_INT interrupt when
         *  LEDC_OVF_CNT_CH1_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch1_int_st:1;
        /** ovf_cnt_ch2_int_st : RO; bitpos: [14]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH2_INT interrupt when
         *  LEDC_OVF_CNT_CH2_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch2_int_st:1;
        /** ovf_cnt_ch3_int_st : RO; bitpos: [15]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH3_INT interrupt when
         *  LEDC_OVF_CNT_CH3_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch3_int_st:1;
        /** ovf_cnt_ch4_int_st : RO; bitpos: [16]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH4_INT interrupt when
         *  LEDC_OVF_CNT_CH4_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch4_int_st:1;
        /** ovf_cnt_ch5_int_st : RO; bitpos: [17]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH5_INT interrupt when
         *  LEDC_OVF_CNT_CH5_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch5_int_st:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} ledc_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** timer0_ovf_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the LEDC_TIMER0_OVF_INT interrupt.
         */
        uint32_t timer0_ovf_int_ena:1;
        /** timer1_ovf_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the LEDC_TIMER1_OVF_INT interrupt.
         */
        uint32_t timer1_ovf_int_ena:1;
        /** timer2_ovf_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the LEDC_TIMER2_OVF_INT interrupt.
         */
        uint32_t timer2_ovf_int_ena:1;
        /** timer3_ovf_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the LEDC_TIMER3_OVF_INT interrupt.
         */
        uint32_t timer3_ovf_int_ena:1;
        /** duty_chng_end_ch0_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH0_INT interrupt.
         */
        uint32_t duty_chng_end_ch0_int_ena:1;
        /** duty_chng_end_ch1_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH1_INT interrupt.
         */
        uint32_t duty_chng_end_ch1_int_ena:1;
        /** duty_chng_end_ch2_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH2_INT interrupt.
         */
        uint32_t duty_chng_end_ch2_int_ena:1;
        /** duty_chng_end_ch3_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH3_INT interrupt.
         */
        uint32_t duty_chng_end_ch3_int_ena:1;
        /** duty_chng_end_ch4_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH4_INT interrupt.
         */
        uint32_t duty_chng_end_ch4_int_ena:1;
        /** duty_chng_end_ch5_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH5_INT interrupt.
         */
        uint32_t duty_chng_end_ch5_int_ena:1;
        uint32_t reserved_10:2;
        /** ovf_cnt_ch0_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH0_INT interrupt.
         */
        uint32_t ovf_cnt_ch0_int_ena:1;
        /** ovf_cnt_ch1_int_ena : R/W; bitpos: [13]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH1_INT interrupt.
         */
        uint32_t ovf_cnt_ch1_int_ena:1;
        /** ovf_cnt_ch2_int_ena : R/W; bitpos: [14]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH2_INT interrupt.
         */
        uint32_t ovf_cnt_ch2_int_ena:1;
        /** ovf_cnt_ch3_int_ena : R/W; bitpos: [15]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH3_INT interrupt.
         */
        uint32_t ovf_cnt_ch3_int_ena:1;
        /** ovf_cnt_ch4_int_ena : R/W; bitpos: [16]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH4_INT interrupt.
         */
        uint32_t ovf_cnt_ch4_int_ena:1;
        /** ovf_cnt_ch5_int_ena : R/W; bitpos: [17]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH5_INT interrupt.
         */
        uint32_t ovf_cnt_ch5_int_ena:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} ledc_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** timer0_ovf_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the LEDC_TIMER0_OVF_INT interrupt.
         */
        uint32_t timer0_ovf_int_clr:1;
        /** timer1_ovf_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the LEDC_TIMER1_OVF_INT interrupt.
         */
        uint32_t timer1_ovf_int_clr:1;
        /** timer2_ovf_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the LEDC_TIMER2_OVF_INT interrupt.
         */
        uint32_t timer2_ovf_int_clr:1;
        /** timer3_ovf_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the LEDC_TIMER3_OVF_INT interrupt.
         */
        uint32_t timer3_ovf_int_clr:1;
        /** duty_chng_end_ch0_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the LEDC_DUTY_CHNG_END_CH0_INT interrupt.
         */
        uint32_t duty_chng_end_ch0_int_clr:1;
        /** duty_chng_end_ch1_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the LEDC_DUTY_CHNG_END_CH1_INT interrupt.
         */
        uint32_t duty_chng_end_ch1_int_clr:1;
        /** duty_chng_end_ch2_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the LEDC_DUTY_CHNG_END_CH2_INT interrupt.
         */
        uint32_t duty_chng_end_ch2_int_clr:1;
        /** duty_chng_end_ch3_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the LEDC_DUTY_CHNG_END_CH3_INT interrupt.
         */
        uint32_t duty_chng_end_ch3_int_clr:1;
        /** duty_chng_end_ch4_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the LEDC_DUTY_CHNG_END_CH4_INT interrupt.
         */
        uint32_t duty_chng_end_ch4_int_clr:1;
        /** duty_chng_end_ch5_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the LEDC_DUTY_CHNG_END_CH5_INT interrupt.
         */
        uint32_t duty_chng_end_ch5_int_clr:1;
        uint32_t reserved_10:2;
        /** ovf_cnt_ch0_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH0_INT interrupt.
         */
        uint32_t ovf_cnt_ch0_int_clr:1;
        /** ovf_cnt_ch1_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH1_INT interrupt.
         */
        uint32_t ovf_cnt_ch1_int_clr:1;
        /** ovf_cnt_ch2_int_clr : WT; bitpos: [14]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH2_INT interrupt.
         */
        uint32_t ovf_cnt_ch2_int_clr:1;
        /** ovf_cnt_ch3_int_clr : WT; bitpos: [15]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH3_INT interrupt.
         */
        uint32_t ovf_cnt_ch3_int_clr:1;
        /** ovf_cnt_ch4_int_clr : WT; bitpos: [16]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH4_INT interrupt.
         */
        uint32_t ovf_cnt_ch4_int_clr:1;
        /** ovf_cnt_ch5_int_clr : WT; bitpos: [17]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH5_INT interrupt.
         */
        uint32_t ovf_cnt_ch5_int_clr:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} ledc_int_clr_reg_t;


/** Group: Gamma RAM Register */
/** Type of chn_gamma_wr register
 *  Ledc chn gamma ram write register.
 */
typedef union {
    struct {
        /** gamma_duty_inc : R/W; bitpos: [0]; default: 0;
         *  Ledc chn gamma duty inc of current ram write address.This register is used to
         *  increase or decrease the duty of output signal on channel n.
         *
         *  1: Increase  0: Decrease.
         */
        uint32_t gamma_duty_inc:1;
        /** gamma_duty_cycle : R/W; bitpos: [10:1]; default: 0;
         *  Ledc chn gamma duty cycle of current ram write address.The duty will change every
         *  LEDC_CHn_GAMMA_DUTY_CYCLE on channel n.
         */
        uint32_t gamma_duty_cycle:10;
        /** gamma_scale : R/W; bitpos: [20:11]; default: 0;
         *  Ledc chn gamma scale of current ram write address.This register is used to
         *  configure the changing step scale of duty on channel n.
         */
        uint32_t gamma_scale:10;
        /** gamma_duty_num : R/W; bitpos: [30:21]; default: 0;
         *  Ledc chn gamma duty num of current ram write address.This register is used to
         *  control the number of times the duty cycle will be changed.
         */
        uint32_t gamma_duty_num:10;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} ledc_chn_gamma_wr_reg_t;

/** Type of chn_gamma_wr_addr register
 *  Ledc chn gamma ram write address register.
 */
typedef union {
    struct {
        /** gamma_wr_addr : R/W; bitpos: [3:0]; default: 0;
         *  Ledc chn gamma ram write address.
         */
        uint32_t gamma_wr_addr:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ledc_chn_gamma_wr_addr_reg_t;

/** Type of chn_gamma_rd_addr register
 *  Ledc chn gamma ram read address register.
 */
typedef union {
    struct {
        /** gamma_rd_addr : R/W; bitpos: [3:0]; default: 0;
         *  Ledc chn gamma ram read address.
         */
        uint32_t gamma_rd_addr:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ledc_chn_gamma_rd_addr_reg_t;

/** Type of chn_gamma_rd_data register
 *  Ledc chn gamma ram read data register.
 */
typedef union {
    struct {
        /** gamma_rd_data : RO; bitpos: [30:0]; default: 0;
         *  Ledc chn gamma ram read data.
         */
        uint32_t gamma_rd_data:31;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} ledc_chn_gamma_rd_data_reg_t;


/** Group: Gamma Config Register */
/** Type of chn_gamma_conf register
 *  Ledc chn gamma config register.
 */
typedef union {
    struct {
        /** gamma_entry_num : R/W; bitpos: [4:0]; default: 0;
         *  Ledc chn gamma entry num.
         */
        uint32_t gamma_entry_num:5;
        /** gamma_pause : WT; bitpos: [5]; default: 0;
         *  Ledc chn gamma pause, write 1 to pause.
         */
        uint32_t gamma_pause:1;
        /** gamma_resume : WT; bitpos: [6]; default: 0;
         *  Ledc chn gamma resume, write 1 to resume.
         */
        uint32_t gamma_resume:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ledc_chn_gamma_conf_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** ledc_date : R/W; bitpos: [27:0]; default: 34672976;
         *  This is the version control register.
         */
        uint32_t ledc_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ledc_date_reg_t;

typedef struct {
    volatile ledc_chn_conf0_reg_t conf0;
    volatile ledc_chn_hpoint_reg_t hpoint;
    volatile ledc_chn_duty_reg_t duty;
    volatile ledc_chn_conf1_reg_t conf1;
    volatile ledc_chn_duty_r_reg_t duty_rd;
} ledc_chn_reg_t;

typedef struct {
    volatile ledc_chn_reg_t channel[6];
} ledc_ch_group_reg_t;

typedef struct {
    volatile ledc_timerx_conf_reg_t conf;
    volatile ledc_timerx_value_reg_t value;
} ledc_timerx_reg_t;

typedef struct {
    volatile ledc_timerx_reg_t timer[4];
} ledc_timer_group_reg_t;

typedef struct {
    volatile ledc_chn_gamma_wr_reg_t wr;
    volatile ledc_chn_gamma_wr_addr_reg_t wr_addr;
    volatile ledc_chn_gamma_rd_addr_reg_t rd_addr;
    volatile ledc_chn_gamma_rd_data_reg_t rd_data;
} ledc_chn_gamma_reg_t;

typedef struct {
    volatile ledc_chn_gamma_reg_t channel[6];
} ledc_ch_gamma_group_reg_t;

typedef struct {
    volatile ledc_chn_gamma_conf_reg_t gamma_conf[6];
} ledc_ch_gamma_conf_group_reg_t;

typedef struct {
    volatile ledc_timern_cmp_reg_t cmp[4];
} ledc_timer_cmp_group_reg_t;

typedef struct {
    volatile ledc_timern_cnt_cap_reg_t cnt_cap[4];
} ledc_timer_cnt_cap_group_reg_t;

typedef struct ledc_dev_t {
    volatile ledc_ch_group_reg_t channel_group[1];
    uint32_t reserved_078[10];
    volatile ledc_timer_group_reg_t timer_group[1];
    volatile ledc_int_raw_reg_t int_raw;
    volatile ledc_int_st_reg_t int_st;
    volatile ledc_int_ena_reg_t int_ena;
    volatile ledc_int_clr_reg_t int_clr;
    uint32_t reserved_0d0[12];
    volatile ledc_ch_gamma_group_reg_t channel_gamma_group[1];
    uint32_t reserved_160[8];
    volatile ledc_ch_gamma_conf_group_reg_t channel_gamma_conf_group[1];
    uint32_t reserved_198[2];
    volatile ledc_evt_task_en0_reg_t evt_task_en0;
    volatile ledc_evt_task_en1_reg_t evt_task_en1;
    volatile ledc_evt_task_en2_reg_t evt_task_en2;
    uint32_t reserved_1ac;
    volatile ledc_timer_cmp_group_reg_t timer_cmp_group[1];
    volatile ledc_timer_cnt_cap_group_reg_t timer_cnt_cap_group[1];
    uint32_t reserved_1d0[8];
    volatile ledc_conf_reg_t conf;
    uint32_t reserved_1f4[2];
    volatile ledc_date_reg_t date;
} ledc_dev_t;

extern ledc_dev_t LEDC;

#ifndef __cplusplus
_Static_assert(sizeof(ledc_dev_t) == 0x200, "Invalid size of ledc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
