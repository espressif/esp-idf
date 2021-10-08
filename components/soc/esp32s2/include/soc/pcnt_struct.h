// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_PCNT_STRUCT_H_
#define _SOC_PCNT_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct pcnt_dev_s {
    struct {
        union {
            struct {
                uint32_t filter_thres:     10;
                uint32_t filter_en:         1;
                uint32_t thr_zero_en:       1;
                uint32_t thr_h_lim_en:      1;
                uint32_t thr_l_lim_en:      1;
                uint32_t thr_thres0_en:     1;
                uint32_t thr_thres1_en:     1;
                uint32_t ch0_neg_mode:      2;
                uint32_t ch0_pos_mode:      2;
                uint32_t ch0_hctrl_mode:    2;
                uint32_t ch0_lctrl_mode:    2;
                uint32_t ch1_neg_mode:      2;
                uint32_t ch1_pos_mode:      2;
                uint32_t ch1_hctrl_mode:    2;
                uint32_t ch1_lctrl_mode:    2;
            };
            uint32_t val;
        } conf0;
        union {
            struct {
                uint32_t cnt_thres0:   16;
                uint32_t cnt_thres1:   16;
            };
            uint32_t val;
        } conf1;
        union {
            struct {
                uint32_t cnt_h_lim:   16;
                uint32_t cnt_l_lim:   16;
            };
            uint32_t val;
        } conf2;
    } conf_unit[4];
    union {
        struct {
            uint32_t cnt_val:     16;
            uint32_t reserved16:  16;
        };
        uint32_t val;
    } cnt_unit[4];
    union {
        struct {
            uint32_t cnt_thr_event_u0:         1;
            uint32_t cnt_thr_event_u1:         1;
            uint32_t cnt_thr_event_u2:         1;
            uint32_t cnt_thr_event_u3:         1;
            uint32_t reserved4:               28;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t cnt_thr_event_u0:        1;
            uint32_t cnt_thr_event_u1:        1;
            uint32_t cnt_thr_event_u2:        1;
            uint32_t cnt_thr_event_u3:        1;
            uint32_t reserved4:              28;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t cnt_thr_event_u0:         1;
            uint32_t cnt_thr_event_u1:         1;
            uint32_t cnt_thr_event_u2:         1;
            uint32_t cnt_thr_event_u3:         1;
            uint32_t reserved4:               28;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t cnt_thr_event_u0:         1;
            uint32_t cnt_thr_event_u1:         1;
            uint32_t cnt_thr_event_u2:         1;
            uint32_t cnt_thr_event_u3:         1;
            uint32_t reserved4:               28;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t cnt_mode:              2;
            uint32_t thres1_lat:            1;
            uint32_t thres0_lat:            1;
            uint32_t l_lim_lat:             1;
            uint32_t h_lim_lat:             1;
            uint32_t zero_lat:              1;
            uint32_t reserved7:            25;
        };
        uint32_t val;
    } status_unit[4];
    union {
        struct {
            uint32_t cnt_rst_u0:       1;
            uint32_t cnt_pause_u0:     1;
            uint32_t cnt_rst_u1:       1;
            uint32_t cnt_pause_u1:     1;
            uint32_t cnt_rst_u2:       1;
            uint32_t cnt_pause_u2:     1;
            uint32_t cnt_rst_u3:       1;
            uint32_t cnt_pause_u3:     1;
            uint32_t reserved8:        8;
            uint32_t clk_en:           1;
            uint32_t reserved17:      15;
        };
        uint32_t val;
    } ctrl;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
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
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
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
    uint32_t date;                                  /**/
} pcnt_dev_t;
extern pcnt_dev_t PCNT;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_PCNT_STRUCT_H_ */
