/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
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
                    uint32_t timer_sel:           2;
                    uint32_t sig_out_en:          1;
                    uint32_t idle_lv:             1;
                    uint32_t low_speed_update:    1;
                    uint32_t ovf_num:            10;
                    uint32_t ovf_cnt_en:          1;
                    uint32_t ovf_cnt_rst:         1;
                    uint32_t reserved17:         15;
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
                    uint32_t reserved19:  13;
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
                    uint32_t duty_resolution:   4;
                    uint32_t clock_divider:    18;
                    uint32_t pause:             1;
                    uint32_t rst:               1;
                    uint32_t reserved24:        1;
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
    } timer_group[1];
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
            uint32_t ovf_cnt_lsch0:               1;
            uint32_t ovf_cnt_lsch1:               1;
            uint32_t ovf_cnt_lsch2:               1;
            uint32_t ovf_cnt_lsch3:               1;
            uint32_t ovf_cnt_lsch4:               1;
            uint32_t ovf_cnt_lsch5:               1;
            uint32_t reserved16:                 16;
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
            uint32_t ovf_cnt_lsch0:              1;
            uint32_t ovf_cnt_lsch1:              1;
            uint32_t ovf_cnt_lsch2:              1;
            uint32_t ovf_cnt_lsch3:              1;
            uint32_t ovf_cnt_lsch4:              1;
            uint32_t ovf_cnt_lsch5:              1;
            uint32_t reserved16:                16;
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
            uint32_t ovf_cnt_lsch0:               1;
            uint32_t ovf_cnt_lsch1:               1;
            uint32_t ovf_cnt_lsch2:               1;
            uint32_t ovf_cnt_lsch3:               1;
            uint32_t ovf_cnt_lsch4:               1;
            uint32_t ovf_cnt_lsch5:               1;
            uint32_t reserved16:                 16;
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
            uint32_t ovf_cnt_lsch0:               1;
            uint32_t ovf_cnt_lsch1:               1;
            uint32_t ovf_cnt_lsch2:               1;
            uint32_t ovf_cnt_lsch3:               1;
            uint32_t ovf_cnt_lsch4:               1;
            uint32_t ovf_cnt_lsch5:               1;
            uint32_t reserved16:                 16;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t apb_clk_sel: 2;
            uint32_t reserved2:  29;
            uint32_t clk_en:      1;
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
    union {
        struct {
            uint32_t date:      32;
        };
        uint32_t val;
    } date;
} ledc_dev_t;
extern ledc_dev_t LEDC;
#ifdef __cplusplus
}
#endif
