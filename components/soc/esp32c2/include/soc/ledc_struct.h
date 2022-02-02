/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_LEDC_STRUCT_H_
#define _SOC_LEDC_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct ledc_dev_s {
    struct {
        struct {
            union {
                struct {
                    uint32_t timer_sel                     :    2;  /*This field is used to select one of timers for channel $n.; ; 0: select timer0; 1: select timer1; 2: select timer2; 3: select timer3*/
                    uint32_t sig_out_en                    :    1;  /*Set this bit to enable signal output on channel $n.*/
                    uint32_t idle_lv                       :    1;  /*This bit is used to control the output value when channel $n is inactive (when LEDC_SIG_OUT_EN_CH$n is 0).*/
                    uint32_t low_speed_update              :    1;  /*This bit is used to update LEDC_HPOINT_CH$n, LEDC_DUTY_START_CH$n, LEDC_SIG_OUT_EN_CH$n, LEDC_TIMER_SEL_CH$n, LEDC_DUTY_NUM_CH$n, LEDC_DUTY_CYCLE_CH$n, LEDC_DUTY_SCALE_CH$n, LEDC_DUTY_INC_CH$n, and LEDC_OVF_CNT_EN_CH$n fields for channel $n, and will be automatically cleared by hardware.*/
                    uint32_t ovf_num                       :    10;  /*This register is used to configure the maximum times of overflow minus 1.; ; The LEDC_OVF_CNT_CH$n_INT interrupt will be triggered when channel $n overflows for (LEDC_OVF_NUM_CH$n + 1) times.*/
                    uint32_t ovf_cnt_en                    :    1;  /*This bit is used to enable the ovf_cnt of channel $n.*/
                    uint32_t ovf_cnt_rst                   :    1;  /*Set this bit to reset the ovf_cnt of channel $n.*/
                    uint32_t reserved17                    :    15;  /*Reserved*/
                };
                uint32_t val;
            } conf0;
            union {
                struct {
                    uint32_t hpoint                        :    14;  /*The output value changes to high when the selected timers has reached the value specified by this register.*/
                    uint32_t reserved14                    :    18;  /*Reserved*/
                };
                uint32_t val;
            } hpoint;
            union {
                struct {
                    uint32_t duty                          :    19;  /*This register is used to change the output duty by controlling the Lpoint.; ; The output value turns to low when the selected timers has reached the Lpoint.*/
                    uint32_t reserved19                    :    13;  /*Reserved*/
                };
                uint32_t val;
            } duty;
            union {
                struct {
                    uint32_t duty_scale                    :    10;  /*This register is used to configure the changing step scale of duty on channel $n.*/
                    uint32_t duty_cycle                    :    10;  /*The duty will change every LEDC_DUTY_CYCLE_CH$n on channel $n.*/
                    uint32_t duty_num                      :    10;  /*This register is used to control the number of times the duty cycle will be changed.*/
                    uint32_t duty_inc                      :    1;  /*This register is used to increase or decrease the duty of output signal on channel $n. 1: Increase; 0: Decrease.*/
                    uint32_t duty_start                    :    1;  /*Other configured fields in LEDC_CH$n_CONF1_REG will start to take effect when this bit is set to 1.*/
                };
                uint32_t val;
            } conf1;
            union {
                struct {
                    uint32_t duty_read                     :    19;  /*This register stores the current duty of output signal on channel $n.*/
                    uint32_t reserved19                    :    13;  /*Reserved*/
                };
                uint32_t val;
            } duty_rd;
        } channel[6];
    } channel_group[1];
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    struct {
        struct {
            union {
                struct {
                    uint32_t duty_resolution               :    4;  /*This register is used to control the range of the counter in timer $x.*/
                    uint32_t clock_divider                 :    18;  /*This register is used to configure the divisor for the divider in timer $x.; ; The least significant eight bits represent the fractional part.*/
                    uint32_t pause                         :    1;  /*This bit is used to suspend the counter in timer $x.*/
                    uint32_t rst                           :    1;  /*This bit is used to reset timer $x. The counter will show 0 after reset.*/
                    uint32_t reserved24                    :    1;  /*Reserved*/
                    uint32_t low_speed_update              :    1;  /*Set this bit to update LEDC_CLK_DIV_TIMER$x and LEDC_TIMER$x_DUTY_RES.*/
                    uint32_t reserved26                    :    6;  /*Reserved*/
                };
                uint32_t val;
            } conf;
            union {
                struct {
                    uint32_t timer_cnt                     :    14;  /*This register stores the current counter value of timer $x.*/
                    uint32_t reserved14                    :    18;  /*Reserved*/
                };
                uint32_t val;
            } value;
        } timer[4];
    } timer_group[1];
    union {
        struct {
            uint32_t lstimer0_ovf                  :    1;  /*Triggered when the timer$x has reached its maximum counter value.*/
            uint32_t lstimer1_ovf                  :    1;  /*Triggered when the timer$x has reached its maximum counter value.*/
            uint32_t lstimer2_ovf                  :    1;  /*Triggered when the timer$x has reached its maximum counter value.*/
            uint32_t lstimer3_ovf                  :    1;  /*Triggered when the timer$x has reached its maximum counter value.*/
            uint32_t duty_chng_end_ch0             :    1;  /*Interrupt raw bit for channel $n. Triggered when the gradual change of duty has finished.*/
            uint32_t duty_chng_end_ch1             :    1;  /*Interrupt raw bit for channel $n. Triggered when the gradual change of duty has finished.*/
            uint32_t duty_chng_end_ch2             :    1;  /*Interrupt raw bit for channel $n. Triggered when the gradual change of duty has finished.*/
            uint32_t duty_chng_end_ch3             :    1;  /*Interrupt raw bit for channel $n. Triggered when the gradual change of duty has finished.*/
            uint32_t duty_chng_end_ch4             :    1;  /*Interrupt raw bit for channel $n. Triggered when the gradual change of duty has finished.*/
            uint32_t duty_chng_end_ch5             :    1;  /*Interrupt raw bit for channel $n. Triggered when the gradual change of duty has finished.*/
            uint32_t ovf_cnt_ch0                   :    1;  /*Interrupt raw bit for channel $n. Triggered when the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH$n.*/
            uint32_t ovf_cnt_ch1                   :    1;  /*Interrupt raw bit for channel $n. Triggered when the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH$n.*/
            uint32_t ovf_cnt_ch2                   :    1;  /*Interrupt raw bit for channel $n. Triggered when the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH$n.*/
            uint32_t ovf_cnt_ch3                   :    1;  /*Interrupt raw bit for channel $n. Triggered when the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH$n.*/
            uint32_t ovf_cnt_ch4                   :    1;  /*Interrupt raw bit for channel $n. Triggered when the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH$n.*/
            uint32_t ovf_cnt_ch5                   :    1;  /*Interrupt raw bit for channel $n. Triggered when the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH$n.*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t lstimer0_ovf                  :    1;  /*This is the masked interrupt status bit for the LEDC_TIMER$x_OVF_INT interrupt when LEDC_TIMER$x_OVF_INT_ENA is set to 1.*/
            uint32_t lstimer1_ovf                  :    1;  /*This is the masked interrupt status bit for the LEDC_TIMER$x_OVF_INT interrupt when LEDC_TIMER$x_OVF_INT_ENA is set to 1.*/
            uint32_t lstimer2_ovf                  :    1;  /*This is the masked interrupt status bit for the LEDC_TIMER$x_OVF_INT interrupt when LEDC_TIMER$x_OVF_INT_ENA is set to 1.*/
            uint32_t lstimer3_ovf                  :    1;  /*This is the masked interrupt status bit for the LEDC_TIMER$x_OVF_INT interrupt when LEDC_TIMER$x_OVF_INT_ENA is set to 1.*/
            uint32_t duty_chng_end_ch0             :    1;  /*This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt when LEDC_DUTY_CHNG_END_CH$n_INT_ENAIS set to 1.*/
            uint32_t duty_chng_end_ch1             :    1;  /*This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt when LEDC_DUTY_CHNG_END_CH$n_INT_ENAIS set to 1.*/
            uint32_t duty_chng_end_ch2             :    1;  /*This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt when LEDC_DUTY_CHNG_END_CH$n_INT_ENAIS set to 1.*/
            uint32_t duty_chng_end_ch3             :    1;  /*This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt when LEDC_DUTY_CHNG_END_CH$n_INT_ENAIS set to 1.*/
            uint32_t duty_chng_end_ch4             :    1;  /*This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt when LEDC_DUTY_CHNG_END_CH$n_INT_ENAIS set to 1.*/
            uint32_t duty_chng_end_ch5             :    1;  /*This is the masked interrupt status bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt when LEDC_DUTY_CHNG_END_CH$n_INT_ENAIS set to 1.*/
            uint32_t ovf_cnt_ch0                   :    1;  /*This is the masked interrupt status bit for the LEDC_OVF_CNT_CH$n_INT interrupt when LEDC_OVF_CNT_CH$n_INT_ENA is set to 1.*/
            uint32_t ovf_cnt_ch1                   :    1;  /*This is the masked interrupt status bit for the LEDC_OVF_CNT_CH$n_INT interrupt when LEDC_OVF_CNT_CH$n_INT_ENA is set to 1.*/
            uint32_t ovf_cnt_ch2                   :    1;  /*This is the masked interrupt status bit for the LEDC_OVF_CNT_CH$n_INT interrupt when LEDC_OVF_CNT_CH$n_INT_ENA is set to 1.*/
            uint32_t ovf_cnt_ch3                   :    1;  /*This is the masked interrupt status bit for the LEDC_OVF_CNT_CH$n_INT interrupt when LEDC_OVF_CNT_CH$n_INT_ENA is set to 1.*/
            uint32_t ovf_cnt_ch4                   :    1;  /*This is the masked interrupt status bit for the LEDC_OVF_CNT_CH$n_INT interrupt when LEDC_OVF_CNT_CH$n_INT_ENA is set to 1.*/
            uint32_t ovf_cnt_ch5                   :    1;  /*This is the masked interrupt status bit for the LEDC_OVF_CNT_CH$n_INT interrupt when LEDC_OVF_CNT_CH$n_INT_ENA is set to 1.*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t lstimer0_ovf                  :    1;  /*The interrupt enable bit for the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t lstimer1_ovf                  :    1;  /*The interrupt enable bit for the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t lstimer2_ovf                  :    1;  /*The interrupt enable bit for the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t lstimer3_ovf                  :    1;  /*The interrupt enable bit for the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t duty_chng_end_ch0             :    1;  /*The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch1             :    1;  /*The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch2             :    1;  /*The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch3             :    1;  /*The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch4             :    1;  /*The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch5             :    1;  /*The interrupt enable bit for the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch0                   :    1;  /*The interrupt enable bit for the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch1                   :    1;  /*The interrupt enable bit for the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch2                   :    1;  /*The interrupt enable bit for the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch3                   :    1;  /*The interrupt enable bit for the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch4                   :    1;  /*The interrupt enable bit for the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch5                   :    1;  /*The interrupt enable bit for the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t lstimer0_ovf                  :    1;  /*Set this bit to clear the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t lstimer1_ovf                  :    1;  /*Set this bit to clear the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t lstimer2_ovf                  :    1;  /*Set this bit to clear the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t lstimer3_ovf                  :    1;  /*Set this bit to clear the LEDC_TIMER$x_OVF_INT interrupt.*/
            uint32_t duty_chng_end_ch0             :    1;  /*Set this bit to clear the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch1             :    1;  /*Set this bit to clear the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch2             :    1;  /*Set this bit to clear the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch3             :    1;  /*Set this bit to clear the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch4             :    1;  /*Set this bit to clear the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t duty_chng_end_ch5             :    1;  /*Set this bit to clear the LEDC_DUTY_CHNG_END_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch0                   :    1;  /*Set this bit to clear the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch1                   :    1;  /*Set this bit to clear the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch2                   :    1;  /*Set this bit to clear the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch3                   :    1;  /*Set this bit to clear the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch4                   :    1;  /*Set this bit to clear the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t ovf_cnt_ch5                   :    1;  /*Set this bit to clear the LEDC_OVF_CNT_CH$n_INT interrupt.*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t apb_clk_sel                   :    2;  /*This bit is used to select clock source for the 4 timers .; ; 2'd1: APB_CLK 2'd2: RTC8M_CLK 2'd3: XTAL_CLK*/
            uint32_t reserved2                     :    29;  /*Reserved*/
            uint32_t clk_en                        :    1;  /*This bit is used to control clock.; ; 1'b1: Force clock on for register. 1'h0: Support clock only when application writes registers.*/
        };
        uint32_t val;
    } conf;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t date;
} ledc_dev_t;
extern ledc_dev_t LEDC;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_LEDC_STRUCT_H_ */
