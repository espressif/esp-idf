// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

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

typedef volatile struct timg_dev_s {
    struct{
        union {
            struct {
                uint32_t reserved0:   10;
                uint32_t alarm_en:     1;             /*When set  alarm is enabled*/
                uint32_t level_int_en: 1;             /*When set  level type interrupt will be generated during alarm*/
                uint32_t edge_int_en:  1;             /*When set  edge type interrupt will be generated during alarm*/
                uint32_t divider:     16;             /*Timer clock (T0/1_clk) pre-scale value.*/
                uint32_t autoreload:   1;             /*When set  timer 0/1 auto-reload at alarming is enabled*/
                uint32_t increase:     1;             /*When set  timer 0/1 time-base counter increment. When cleared timer 0 time-base counter decrement.*/
                uint32_t enable:       1;             /*When set  timer 0/1 time-base counter is enabled*/
            };
            uint32_t val;
        } config;
        uint32_t cnt_low;                             /*Register to store timer 0/1 time-base counter current value lower 32 bits.*/
        uint32_t cnt_high;                            /*Register to store timer 0 time-base counter current value higher 32 bits.*/
        uint32_t update;                              /*Write any value will trigger a timer 0 time-base counter value update (timer 0 current value will be stored in registers above)*/
        uint32_t alarm_low;                           /*Timer 0 time-base counter value lower 32 bits that will trigger the alarm*/
        uint32_t alarm_high;                          /*Timer 0 time-base counter value higher 32 bits that will trigger the alarm*/
        uint32_t load_low;                            /*Lower 32 bits of the value that will load into timer 0 time-base counter*/
        uint32_t load_high;                           /*higher 32 bits of the value that will load into timer 0 time-base counter*/
        uint32_t reload;                              /*Write any value will trigger timer 0 time-base counter reload*/
    } hw_timer[2];
    union {
        struct {
            uint32_t reserved0:       14;
            uint32_t flashboot_mod_en: 1;             /*When set  flash boot protection is enabled*/
            uint32_t sys_reset_length: 3;             /*length of system reset selection. 0: 100ns  1: 200ns  2: 300ns  3: 400ns  4: 500ns  5: 800ns  6: 1.6us  7: 3.2us*/
            uint32_t cpu_reset_length: 3;             /*length of CPU reset selection. 0: 100ns  1: 200ns  2: 300ns  3: 400ns  4: 500ns  5: 800ns  6: 1.6us  7: 3.2us*/
            uint32_t level_int_en:     1;             /*When set  level type interrupt generation is enabled*/
            uint32_t edge_int_en:      1;             /*When set  edge type interrupt generation is enabled*/
            uint32_t stg3:             2;             /*Stage 3 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            uint32_t stg2:             2;             /*Stage 2 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            uint32_t stg1:             2;             /*Stage 1 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            uint32_t stg0:             2;             /*Stage 0 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            uint32_t en:               1;             /*When set  SWDT is enabled*/
        };
        uint32_t val;
    } wdt_config0;
    union {
        struct {
            uint32_t reserved0:       16;
            uint32_t clk_prescale:16;             /*SWDT clock prescale value. Period = 12.5ns * value stored in this register*/
        };
        uint32_t val;
    } wdt_config1;
    uint32_t wdt_config2;                             /*Stage 0 timeout value in SWDT clock cycles*/
    uint32_t wdt_config3;                             /*Stage 1 timeout value in SWDT clock cycles*/
    uint32_t wdt_config4;                             /*Stage 2 timeout value in SWDT clock cycles*/
    uint32_t wdt_config5;                             /*Stage 3 timeout value in SWDT clock cycles*/
    uint32_t wdt_feed;                                /*Write any value will feed SWDT*/
    uint32_t wdt_wprotect;                            /*If change its value from default  then write protection is on.*/
    union {
        struct {
            uint32_t reserved0:             12;
            uint32_t start_cycling: 1;
            uint32_t clk_sel:       2;
            uint32_t rdy:           1;
            uint32_t max:          15;
            uint32_t start:         1;
        };
        uint32_t val;
    } rtc_cali_cfg;
    union {
        struct {
            uint32_t reserved0:      7;
            uint32_t value:25;
        };
        uint32_t val;
    } rtc_cali_cfg1;
    union {
        struct {
            uint32_t reserved0:         7;
            uint32_t rtc_only:     1;
            uint32_t cpst_en:      1;
            uint32_t lac_en:       1;
            uint32_t alarm_en:     1;
            uint32_t level_int_en: 1;
            uint32_t edge_int_en:  1;
            uint32_t divider:     16;
            uint32_t autoreload:   1;
            uint32_t increase:     1;
            uint32_t en:           1;
        };
        uint32_t val;
    } lactconfig;
    union {
        struct {
            uint32_t reserved0:         6;
            uint32_t step_len:26;
        };
        uint32_t val;
    } lactrtc;
    uint32_t lactlo;                                  /**/
    uint32_t lacthi;                                  /**/
    uint32_t lactupdate;                              /**/
    uint32_t lactalarmlo;                             /**/
    uint32_t lactalarmhi;                             /**/
    uint32_t lactloadlo;                              /**/
    uint32_t lactloadhi;                              /**/
    uint32_t lactload;                                /**/
    union {
        struct {
            uint32_t t0:         1;                   /*interrupt when timer0 alarm*/
            uint32_t t1:         1;                   /*interrupt when timer1 alarm*/
            uint32_t wdt:        1;                   /*Interrupt when an interrupt stage timeout*/
            uint32_t lact:       1;
            uint32_t reserved4: 28;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t t0:        1;                    /*interrupt when timer0 alarm*/
            uint32_t t1:        1;                    /*interrupt when timer1 alarm*/
            uint32_t wdt:       1;                    /*Interrupt when an interrupt stage timeout*/
            uint32_t lact:      1;
            uint32_t reserved4:28;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t t0:         1;                   /*interrupt when timer0 alarm*/
            uint32_t t1:         1;                   /*interrupt when timer1 alarm*/
            uint32_t wdt:        1;                   /*Interrupt when an interrupt stage timeout*/
            uint32_t lact:       1;
            uint32_t reserved4: 28;
        };
        uint32_t val;
    } int_st_timers;
    union {
        struct {
            uint32_t t0:         1;                   /*interrupt when timer0 alarm*/
            uint32_t t1:         1;                   /*interrupt when timer1 alarm*/
            uint32_t wdt:        1;                   /*Interrupt when an interrupt stage timeout*/
            uint32_t lact:       1;
            uint32_t reserved4: 28;
        };
        uint32_t val;
    } int_clr_timers;
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
    union {
        struct {
            uint32_t date:28;                         /*Version of this regfile*/
            uint32_t reserved28:   4;
        };
        uint32_t val;
    } timg_date;
    union {
        struct {
            uint32_t reserved0: 31;
            uint32_t en:     1;                   /*Force clock enable for this regfile*/
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
