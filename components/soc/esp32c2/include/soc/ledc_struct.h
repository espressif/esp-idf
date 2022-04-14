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

/** Group: Configuration Register */
/** Type of chn_conf0 register
 *  Configuration register 0 for channel n
 */
typedef union {
    struct {
        /** timer_sel : R/W; bitpos: [1:0]; default: 0;
         *  This field is used to select one of timers for channel n.
         *
         *  0: select timer0; 1: select timer1; 2: select timer2; 3: select timer3
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
        /** duty_scale : R/W; bitpos: [9:0]; default: 0;
         *  This register is used to configure the changing step scale of duty on channel n.
         */
        uint32_t duty_scale:10;
        /** duty_cycle : R/W; bitpos: [19:10]; default: 0;
         *  The duty will change every LEDC_DUTY_CYCLE_CHn on channel n.
         */
        uint32_t duty_cycle:10;
        /** duty_num : R/W; bitpos: [29:20]; default: 0;
         *  This register is used to control the number of times the duty cycle will be changed.
         */
        uint32_t duty_num:10;
        /** duty_inc : R/W; bitpos: [30]; default: 1;
         *  This register is used to increase or decrease the duty of output signal on channel
         *  n. 1: Increase; 0: Decrease.
         */
        uint32_t duty_inc:1;
        /** duty_start : R/W/SC; bitpos: [31]; default: 0;
         *  Other configured fields in LEDC_CHn_CONF1_REG will start to take effect when this
         *  bit is set to 1.
         */
        uint32_t duty_start:1;
    };
    uint32_t val;
} ledc_chn_conf1_reg_t;

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
        uint32_t reserved_2:29;
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
        /** hpoint : R/W; bitpos: [13:0]; default: 0;
         *  The output value changes to high when the selected timers has reached the value
         *  specified by this register.
         */
        uint32_t hpoint:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} ledc_chn_hpoint_reg_t;


/** Group: Duty Cycle Register */
/** Type of chn_duty register
 *  Initial duty cycle for channel n
 */
typedef union {
    struct {
        /** duty : R/W; bitpos: [18:0]; default: 0;
         *  This register is used to change the output duty by controlling the Lpoint.
         *
         *  The output value turns to low when the selected timers has reached the Lpoint.
         */
        uint32_t duty:19;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} ledc_chn_duty_reg_t;

/** Type of chn_duty_r register
 *  Current duty cycle for channel n
 */
typedef union {
    struct {
        /** duty_r : RO; bitpos: [18:0]; default: 0;
         *  This register stores the current duty of output signal on channel n.
         */
        uint32_t duty_r:19;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} ledc_chn_duty_r_reg_t;


/** Group: Timer Register */
/** Type of timerx_conf register
 *  Timer x configuration
 */
typedef union {
    struct {
        /** duty_res : R/W; bitpos: [3:0]; default: 0;
         *  This register is used to control the range of the counter in timer x.
         */
        uint32_t duty_res:4;
        /** clk_div : R/W; bitpos: [21:4]; default: 0;
         *  This register is used to configure the divisor for the divider in timer x.
         *
         *  The least significant eight bits represent the fractional part.
         */
        uint32_t clk_div:18;
        /** pause : R/W; bitpos: [22]; default: 0;
         *  This bit is used to suspend the counter in timer x.
         */
        uint32_t pause:1;
        /** rst : R/W; bitpos: [23]; default: 1;
         *  This bit is used to reset timer x. The counter will show 0 after reset.
         */
        uint32_t rst:1;
        uint32_t reserved_24:1;
        /** para_up : WT; bitpos: [25]; default: 0;
         *  Set this bit to update LEDC_CLK_DIV_TIMERx and LEDC_TIMERx_DUTY_RES.
         */
        uint32_t para_up:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} ledc_timerx_conf_reg_t;

/** Type of timerx_value register
 *  Timer x current counter value
 */
typedef union {
    struct {
        /** timer_cnt : RO; bitpos: [13:0]; default: 0;
         *  This register stores the current counter value of timer x.
         */
        uint32_t timer_cnt:14;
        uint32_t reserved_14:18;
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
        /** ovf_cnt_ch0_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  Interrupt raw bit for channel 0. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH0.
         */
        uint32_t ovf_cnt_ch0_int_raw:1;
        /** ovf_cnt_ch1_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  Interrupt raw bit for channel 1. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH1.
         */
        uint32_t ovf_cnt_ch1_int_raw:1;
        /** ovf_cnt_ch2_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  Interrupt raw bit for channel 2. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH2.
         */
        uint32_t ovf_cnt_ch2_int_raw:1;
        /** ovf_cnt_ch3_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  Interrupt raw bit for channel 3. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH3.
         */
        uint32_t ovf_cnt_ch3_int_raw:1;
        /** ovf_cnt_ch4_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  Interrupt raw bit for channel 4. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH4.
         */
        uint32_t ovf_cnt_ch4_int_raw:1;
        /** ovf_cnt_ch5_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  Interrupt raw bit for channel 5. Triggered when the ovf_cnt has reached the value
         *  specified by LEDC_OVF_NUM_CH5.
         */
        uint32_t ovf_cnt_ch5_int_raw:1;
        uint32_t reserved_16:16;
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
         *  interrupt when LEDC_DUTY_CHNG_END_CH0_INT_ENAIS set to 1.
         */
        uint32_t duty_chng_end_ch0_int_st:1;
        /** duty_chng_end_ch1_int_st : RO; bitpos: [5]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH1_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH1_INT_ENAIS set to 1.
         */
        uint32_t duty_chng_end_ch1_int_st:1;
        /** duty_chng_end_ch2_int_st : RO; bitpos: [6]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH2_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH2_INT_ENAIS set to 1.
         */
        uint32_t duty_chng_end_ch2_int_st:1;
        /** duty_chng_end_ch3_int_st : RO; bitpos: [7]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH3_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH3_INT_ENAIS set to 1.
         */
        uint32_t duty_chng_end_ch3_int_st:1;
        /** duty_chng_end_ch4_int_st : RO; bitpos: [8]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH4_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH4_INT_ENAIS set to 1.
         */
        uint32_t duty_chng_end_ch4_int_st:1;
        /** duty_chng_end_ch5_int_st : RO; bitpos: [9]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH5_INT
         *  interrupt when LEDC_DUTY_CHNG_END_CH5_INT_ENAIS set to 1.
         */
        uint32_t duty_chng_end_ch5_int_st:1;
        /** ovf_cnt_ch0_int_st : RO; bitpos: [10]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH0_INT interrupt when
         *  LEDC_OVF_CNT_CH0_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch0_int_st:1;
        /** ovf_cnt_ch1_int_st : RO; bitpos: [11]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH1_INT interrupt when
         *  LEDC_OVF_CNT_CH1_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch1_int_st:1;
        /** ovf_cnt_ch2_int_st : RO; bitpos: [12]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH2_INT interrupt when
         *  LEDC_OVF_CNT_CH2_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch2_int_st:1;
        /** ovf_cnt_ch3_int_st : RO; bitpos: [13]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH3_INT interrupt when
         *  LEDC_OVF_CNT_CH3_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch3_int_st:1;
        /** ovf_cnt_ch4_int_st : RO; bitpos: [14]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH4_INT interrupt when
         *  LEDC_OVF_CNT_CH4_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch4_int_st:1;
        /** ovf_cnt_ch5_int_st : RO; bitpos: [15]; default: 0;
         *  This is the masked interrupt status bit for the LEDC_OVF_CNT_CH5_INT interrupt when
         *  LEDC_OVF_CNT_CH5_INT_ENA is set to 1.
         */
        uint32_t ovf_cnt_ch5_int_st:1;
        uint32_t reserved_16:16;
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
        /** ovf_cnt_ch0_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH0_INT interrupt.
         */
        uint32_t ovf_cnt_ch0_int_ena:1;
        /** ovf_cnt_ch1_int_ena : R/W; bitpos: [11]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH1_INT interrupt.
         */
        uint32_t ovf_cnt_ch1_int_ena:1;
        /** ovf_cnt_ch2_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH2_INT interrupt.
         */
        uint32_t ovf_cnt_ch2_int_ena:1;
        /** ovf_cnt_ch3_int_ena : R/W; bitpos: [13]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH3_INT interrupt.
         */
        uint32_t ovf_cnt_ch3_int_ena:1;
        /** ovf_cnt_ch4_int_ena : R/W; bitpos: [14]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH4_INT interrupt.
         */
        uint32_t ovf_cnt_ch4_int_ena:1;
        /** ovf_cnt_ch5_int_ena : R/W; bitpos: [15]; default: 0;
         *  The interrupt enable bit for the LEDC_OVF_CNT_CH5_INT interrupt.
         */
        uint32_t ovf_cnt_ch5_int_ena:1;
        uint32_t reserved_16:16;
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
        /** ovf_cnt_ch0_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH0_INT interrupt.
         */
        uint32_t ovf_cnt_ch0_int_clr:1;
        /** ovf_cnt_ch1_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH1_INT interrupt.
         */
        uint32_t ovf_cnt_ch1_int_clr:1;
        /** ovf_cnt_ch2_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH2_INT interrupt.
         */
        uint32_t ovf_cnt_ch2_int_clr:1;
        /** ovf_cnt_ch3_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH3_INT interrupt.
         */
        uint32_t ovf_cnt_ch3_int_clr:1;
        /** ovf_cnt_ch4_int_clr : WT; bitpos: [14]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH4_INT interrupt.
         */
        uint32_t ovf_cnt_ch4_int_clr:1;
        /** ovf_cnt_ch5_int_clr : WT; bitpos: [15]; default: 0;
         *  Set this bit to clear the LEDC_OVF_CNT_CH5_INT interrupt.
         */
        uint32_t ovf_cnt_ch5_int_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ledc_int_clr_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** ledc_date : R/W; bitpos: [31:0]; default: 419829504;
         *  This is the version control register.
         */
        uint32_t ledc_date:32;
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

typedef struct ledc_dev_t {
    volatile ledc_ch_group_reg_t channel_group[1];
    uint32_t reserved_078[10];
    volatile ledc_timer_group_reg_t timer_group[1];
    volatile ledc_int_raw_reg_t int_raw;
    volatile ledc_int_st_reg_t int_st;
    volatile ledc_int_ena_reg_t int_ena;
    volatile ledc_int_clr_reg_t int_clr;
    volatile ledc_conf_reg_t conf;
    uint32_t reserved_0d4[10];
    volatile ledc_date_reg_t date;
} ledc_dev_t;

extern ledc_dev_t LEDC;

#ifndef __cplusplus
_Static_assert(sizeof(ledc_dev_t) == 0x100, "Invalid size of ledc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
