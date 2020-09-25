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
#ifndef _SOC_TIMG_STRUCT_H_
#define _SOC_TIMG_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    struct {
        union {
            struct {
                uint32_t reserved0:       9;
                uint32_t use_xtal:        1;
                uint32_t alarm_en:        1;
                uint32_t level_int_en:    1;
                uint32_t edge_int_en:     1;
                uint32_t divider:        16;
                uint32_t autoreload:      1;
                uint32_t increase:        1;
                uint32_t enable:          1;
            };
            uint32_t val;
        } config;
        uint32_t cnt_low;                                      /**/
        uint32_t cnt_high;                                      /**/
        union {
            struct {
                uint32_t reserved0: 31;
                uint32_t update:     1;
            };
            uint32_t val;
        } update;
        uint32_t alarm_low;                                 /**/
        uint32_t alarm_high;                                 /**/
        uint32_t load_low;                                  /**/
        uint32_t load_high;                                  /**/
        uint32_t reload;                                    /**/
    } hw_timer[2];
    union {
        struct {
            uint32_t reserved0:           12;
            uint32_t appcpu_reset_en:      1;
            uint32_t procpu_reset_en:      1;
            uint32_t flashboot_mod_en:     1;
            uint32_t sys_reset_length:     3;
            uint32_t cpu_reset_length:     3;
            uint32_t level_int_en:         1;
            uint32_t edge_int_en:          1;
            uint32_t stg3:                 2;
            uint32_t stg2:                 2;
            uint32_t stg1:                 2;
            uint32_t stg0:                 2;
            uint32_t en:                   1;
        };
        uint32_t val;
    } wdt_config0;
    union {
        struct {
            uint32_t reserved0:       16;
            uint32_t clk_prescale:    16;
        };
        uint32_t val;
    } wdt_config1;
    uint32_t wdt_config2;                                /**/
    uint32_t wdt_config3;                                /**/
    uint32_t wdt_config4;                                /**/
    uint32_t wdt_config5;                                /**/
    uint32_t wdt_feed;                                   /**/
    uint32_t wdt_wprotect;                               /**/
    union {
        struct {
            uint32_t reserved0:             12;
            uint32_t start_cycling:          1;
            uint32_t clk_sel:                2;
            uint32_t rdy:                    1;
            uint32_t max:                   15;
            uint32_t start:                  1;
        };
        uint32_t val;
    } rtc_cali_cfg;
    union {
        struct {
            uint32_t cycling_data_vld:          1;
            uint32_t reserved1:                 6;
            uint32_t value:                    25;
        };
        uint32_t val;
    } rtc_cali_cfg1;
    union {
        struct {
            uint32_t reserved0:         6;
            uint32_t use_reftick:       1;
            uint32_t rtc_only:          1;
            uint32_t cpst_en:           1;
            uint32_t lac_en:            1;
            uint32_t alarm_en:          1;
            uint32_t level_int_en:      1;
            uint32_t edge_int_en:       1;
            uint32_t divider:          16;
            uint32_t autoreload:        1;
            uint32_t increase:          1;
            uint32_t en:                1;
        };
        uint32_t val;
    } lactconfig;
    union {
        struct {
            uint32_t reserved0:         6;
            uint32_t step_len:         26;
        };
        uint32_t val;
    } lactrtc;
    uint32_t lactlo;                                    /**/
    uint32_t lacthi;                                    /**/
    uint32_t lactupdate;                                /**/
    uint32_t lactalarmlo;                               /**/
    uint32_t lactalarmhi;                               /**/
    uint32_t lactloadlo;                                /**/
    uint32_t lactloadhi;                                /**/
    uint32_t lactload;                                  /**/
    union {
        struct {
            uint32_t t0:           1;
            uint32_t t1:           1;
            uint32_t wdt:          1;
            uint32_t lact:         1;
            uint32_t reserved4:   28;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t t0:           1;
            uint32_t t1:           1;
            uint32_t wdt:          1;
            uint32_t lact:         1;
            uint32_t reserved4:   28;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t t0:          1;
            uint32_t t1:          1;
            uint32_t wdt:         1;
            uint32_t lact:        1;
            uint32_t reserved4:  28;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t t0:           1;
            uint32_t t1:           1;
            uint32_t wdt:          1;
            uint32_t lact:         1;
            uint32_t reserved4:   28;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t timeout:                  1;       /*timeout indicator*/
            uint32_t reserved1:                2;
            uint32_t timeout_rst_cnt:          4;       /*Cycles that release calibration timeout reset*/
            uint32_t timeout_thres:           25;       /*timeout if cali value counts over threshold*/
        };
        uint32_t val;
    } rtc_cali_cfg2;
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
    union {
        struct {
            uint32_t date:        28;
            uint32_t reserved28:   4;
        };
        uint32_t val;
    } timg_date;
    union {
        struct {
            uint32_t reserved0: 31;
            uint32_t en:         1;
        };
        uint32_t val;
    } clk;
} timg_dev_t;
extern timg_dev_t TIMERG0;
extern timg_dev_t TIMERG1;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_TIMG_STRUCT_H_ */
