/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct ledc_dev_s {
    struct {
        struct {
            union {
                struct {
                    uint32_t timer_sel:  2;              /*There are four high speed timers  the two bits are used to select one of them for high speed channel.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
                    uint32_t sig_out_en: 1;              /*This is the output enable control bit for high speed channel*/
                    uint32_t idle_lv:    1;              /*This bit is used to control the output value when high speed channel is off.*/
                    uint32_t low_speed_update: 1;        /*This bit is only useful for low speed timer channels, reserved for high speed timers*/
                    uint32_t ovf_num:   10;
                    uint32_t ovf_cnt_en: 1;
                    uint32_t ovf_cnt_rst: 1;
                    uint32_t ovf_cnt_rst_st: 1;
                    uint32_t reserved18: 14;
                };
                uint32_t val;
            } conf0;
            union {
                struct {
                    uint32_t hpoint:      14;
                    uint32_t reserved14:  18;
                };
                uint32_t val;
            } hpoint;
            union {
                struct {
                    uint32_t duty:      19;
                    uint32_t reserved19:13;
                };
                uint32_t val;
            } duty;
            union {
                struct {
                    uint32_t duty_scale:      10;
                    uint32_t duty_cycle:      10;
                    uint32_t duty_num:        10;
                    uint32_t duty_inc:         1;
                    uint32_t duty_start:       1;
                };
                uint32_t val;
            } conf1;
            union {
                struct {
                    uint32_t duty_read: 19;
                    uint32_t reserved19:13;
                };
                uint32_t val;
            } duty_rd;
        } channel[8];
    } channel_group[1];  /* single channel group, low speed mode only */
    struct {
        struct {
            union {
                struct {
                    uint32_t duty_resolution:   4;
                    uint32_t clock_divider:     18;
                    uint32_t pause:             1;
                    uint32_t rst:               1;
                    uint32_t tick_sel:          1;
                    uint32_t low_speed_update:  1;
                    uint32_t reserved26:        6;
                };
                uint32_t val;
            } conf;
            union {
                struct {
                    uint32_t timer_cnt:   14;
                    uint32_t reserved14:  18;
                };
                uint32_t val;
            } value;
        } timer[4];
    } timer_group[1];  /* single channel group, low speed mode only */
    union {
        struct {
            uint32_t lstimer0_ovf:                1;
            uint32_t lstimer1_ovf:                1;
            uint32_t lstimer2_ovf:                1;
            uint32_t lstimer3_ovf:                1;
            uint32_t duty_chng_end_lsch0:         1;
            uint32_t duty_chng_end_lsch1:         1;
            uint32_t duty_chng_end_lsch2:         1;
            uint32_t duty_chng_end_lsch3:         1;
            uint32_t duty_chng_end_lsch4:         1;
            uint32_t duty_chng_end_lsch5:         1;
            uint32_t duty_chng_end_lsch6:         1;
            uint32_t duty_chng_end_lsch7:         1;
            uint32_t ovf_cnt_lsch0:               1;
            uint32_t ovf_cnt_lsch1:               1;
            uint32_t ovf_cnt_lsch2:               1;
            uint32_t ovf_cnt_lsch3:               1;
            uint32_t ovf_cnt_lsch4:               1;
            uint32_t ovf_cnt_lsch5:               1;
            uint32_t ovf_cnt_lsch6:               1;
            uint32_t ovf_cnt_lsch7:               1;
            uint32_t reserved20:                 12;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t lstimer0_ovf:               1;
            uint32_t lstimer1_ovf:               1;
            uint32_t lstimer2_ovf:               1;
            uint32_t lstimer3_ovf:               1;
            uint32_t duty_chng_end_lsch0:        1;
            uint32_t duty_chng_end_lsch1:        1;
            uint32_t duty_chng_end_lsch2:        1;
            uint32_t duty_chng_end_lsch3:        1;
            uint32_t duty_chng_end_lsch4:        1;
            uint32_t duty_chng_end_lsch5:        1;
            uint32_t duty_chng_end_lsch6:        1;
            uint32_t duty_chng_end_lsch7:        1;
            uint32_t ovf_cnt_lsch0:              1;
            uint32_t ovf_cnt_lsch1:              1;
            uint32_t ovf_cnt_lsch2:              1;
            uint32_t ovf_cnt_lsch3:              1;
            uint32_t ovf_cnt_lsch4:              1;
            uint32_t ovf_cnt_lsch5:              1;
            uint32_t ovf_cnt_lsch6:              1;
            uint32_t ovf_cnt_lsch7:              1;
            uint32_t reserved20:                12;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t lstimer0_ovf:                1;
            uint32_t lstimer1_ovf:                1;
            uint32_t lstimer2_ovf:                1;
            uint32_t lstimer3_ovf:                1;
            uint32_t duty_chng_end_lsch0:         1;
            uint32_t duty_chng_end_lsch1:         1;
            uint32_t duty_chng_end_lsch2:         1;
            uint32_t duty_chng_end_lsch3:         1;
            uint32_t duty_chng_end_lsch4:         1;
            uint32_t duty_chng_end_lsch5:         1;
            uint32_t duty_chng_end_lsch6:         1;
            uint32_t duty_chng_end_lsch7:         1;
            uint32_t ovf_cnt_lsch0:               1;
            uint32_t ovf_cnt_lsch1:               1;
            uint32_t ovf_cnt_lsch2:               1;
            uint32_t ovf_cnt_lsch3:               1;
            uint32_t ovf_cnt_lsch4:               1;
            uint32_t ovf_cnt_lsch5:               1;
            uint32_t ovf_cnt_lsch6:               1;
            uint32_t ovf_cnt_lsch7:               1;
            uint32_t reserved20:                 12;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t lstimer0_ovf:                1;
            uint32_t lstimer1_ovf:                1;
            uint32_t lstimer2_ovf:                1;
            uint32_t lstimer3_ovf:                1;
            uint32_t duty_chng_end_lsch0:         1;
            uint32_t duty_chng_end_lsch1:         1;
            uint32_t duty_chng_end_lsch2:         1;
            uint32_t duty_chng_end_lsch3:         1;
            uint32_t duty_chng_end_lsch4:         1;
            uint32_t duty_chng_end_lsch5:         1;
            uint32_t duty_chng_end_lsch6:         1;
            uint32_t duty_chng_end_lsch7:         1;
            uint32_t ovf_cnt_lsch0:               1;
            uint32_t ovf_cnt_lsch1:               1;
            uint32_t ovf_cnt_lsch2:               1;
            uint32_t ovf_cnt_lsch3:               1;
            uint32_t ovf_cnt_lsch4:               1;
            uint32_t ovf_cnt_lsch5:               1;
            uint32_t ovf_cnt_lsch6:               1;
            uint32_t ovf_cnt_lsch7:               1;
            uint32_t reserved20:                 12;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t apb_clk_sel: 2; // 0:invalid; 1:80MHz APB clock; 2:8MHz RTC clock; 3:XTAL clock
            uint32_t reserved2:  29;
            uint32_t clk_en:      1; /*This bit is clock gating control signal. when software configure LED_PWM internal registers  it controls the register clock.*/
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
    uint32_t date;                                     /**/
} ledc_dev_t;
extern ledc_dev_t LEDC;
#ifdef __cplusplus
}
#endif
