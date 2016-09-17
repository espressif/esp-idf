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
typedef struct {
    struct{
        union {
            struct {
                volatile uint32_t reserved0:   10;
                volatile uint32_t alarm_en:     1;             /*When set  alarm is enabled*/
                volatile uint32_t level_int_en: 1;             /*When set  level type interrupt will be generated during alarm*/
                volatile uint32_t edge_int_en:  1;             /*When set  edge type interrupt will be generated during alarm*/
                volatile uint32_t divider:     16;             /*Timer clock (T0/1_clk) pre-scale value.*/
                volatile uint32_t autoreload:   1;             /*When set  timer 0/1 auto-reload at alarming is enabled*/
                volatile uint32_t increase:     1;             /*When set  timer 0/1 time-base counter increment. When cleared timer 0 time-base counter decrement.*/
                volatile uint32_t enable:       1;             /*When set  timer 0/1 time-base counter is enabled*/
            };
            volatile uint32_t val;
        }config;
        volatile uint32_t timer_cnt_low;                       /*Register to store timer 0/1 time-base counter current value lower 32 bits.*/
        volatile uint32_t timer_cnt_high;                      /*Register to store timer 0 time-base counter current value higher 32 bits.*/
        volatile uint32_t timer_update;                        /*Write any value will trigger a timer 0 time-base counter value update (timer 0 current value will be stored in registers above)*/
        volatile uint32_t timer_alarm_low;                     /*Timer 0 time-base counter value lower 32 bits that will trigger the alarm*/
        volatile uint32_t timer_alarm_high;                    /*Timer 0 time-base counter value higher 32 bits that will trigger the alarm*/
        volatile uint32_t timer_load_low;                      /*Lower 32 bits of the value that will load into timer 0 time-base counter*/
        volatile uint32_t timer_load_high;                     /*higher 32 bits of the value that will load into timer 0 time-base counter*/
        volatile uint32_t timer_reload;                        /*Write any value will trigger timer 0 time-base counter reload*/
    }hw_timer[2];
    union {
        struct {
            volatile uint32_t reserved0:           14;
            volatile uint32_t wdt_flashboot_mod_en: 1;         /*When set  flash boot protection is enabled*/
            volatile uint32_t wdt_sys_reset_length: 3;         /*length of system reset selection. 0: 100ns  1: 200ns  2: 300ns  3: 400ns  4: 500ns  5: 800ns  6: 1.6us  7: 3.2us*/
            volatile uint32_t wdt_cpu_reset_length: 3;         /*length of CPU reset selection. 0: 100ns  1: 200ns  2: 300ns  3: 400ns  4: 500ns  5: 800ns  6: 1.6us  7: 3.2us*/
            volatile uint32_t wdt_level_int_en:     1;         /*When set  level type interrupt generation is enabled*/
            volatile uint32_t wdt_edge_int_en:      1;         /*When set  edge type interrupt generation is enabled*/
            volatile uint32_t wdt_stg3:             2;         /*Stage 3 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            volatile uint32_t wdt_stg2:             2;         /*Stage 2 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            volatile uint32_t wdt_stg1:             2;         /*Stage 1 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            volatile uint32_t wdt_stg0:             2;         /*Stage 0 configuration. 0: off  1: interrupt  2: reset CPU  3: reset system*/
            volatile uint32_t wdt_en:               1;         /*When set  SWDT is enabled*/
        };
        volatile uint32_t val;
    }wdt_config0;
    union {
        struct {
            volatile uint32_t reserved0:       16;
            volatile uint32_t wdt_clk_prescale:16;             /*SWDT clock prescale value. Period = 12.5ns * value stored in this register*/
        };
        volatile uint32_t val;
    }wdt_config1;
    volatile uint32_t wdt_config2;                             /*Stage 0 timeout value in SWDT clock cycles*/
    volatile uint32_t wdt_config3;                             /*Stage 1 timeout value in SWDT clock cycles*/
    volatile uint32_t wdt_config4;                             /*Stage 2 timeout value in SWDT clock cycles*/
    volatile uint32_t wdt_config5;                             /*Stage 3 timeout value in SWDT clock cycles*/
    volatile uint32_t wdt_feed;                                /*Write any value will feed SWDT*/
    volatile uint32_t wdt_wprotect;                            /*If change its value from default  then write protection is on.*/
    union {
        struct {
            volatile uint32_t reserved0:             12;
            volatile uint32_t rtc_cali_start_cycling: 1;
            volatile uint32_t rtc_cali_clk_sel:       2;
            volatile uint32_t rtc_cali_rdy:           1;
            volatile uint32_t rtc_cali_max:          15;
            volatile uint32_t rtc_cali_start:         1;
        };
        volatile uint32_t val;
    }rtc_cali_cfg;
    union {
        struct {
            volatile uint32_t reserved0:      7;
            volatile uint32_t rtc_cali_value:25;
        };
        volatile uint32_t val;
    }rtc_cali_cfg1;
    union {
        struct {
            volatile uint32_t reserved0:         7;
            volatile uint32_t lact_rtc_only:     1;
            volatile uint32_t lact_cpst_en:      1;
            volatile uint32_t lact_lac_en:       1;
            volatile uint32_t lact_alarm_en:     1;
            volatile uint32_t lact_level_int_en: 1;
            volatile uint32_t lact_edge_int_en:  1;
            volatile uint32_t lact_divider:     16;
            volatile uint32_t lact_autoreload:   1;
            volatile uint32_t lact_increase:     1;
            volatile uint32_t lact_en:           1;
        };
        volatile uint32_t val;
    }lactconfig;
    union {
        struct {
            volatile uint32_t reserved0:         6;
            volatile uint32_t lact_rtc_step_len:26;
        };
        volatile uint32_t val;
    }lactrtc;
    volatile uint32_t lactlo;                                  /**/
    volatile uint32_t lacthi;                                  /**/
    volatile uint32_t lactupdate;                              /**/
    volatile uint32_t lactalarmlo;                             /**/
    volatile uint32_t lactalarmhi;                             /**/
    volatile uint32_t lactloadlo;                              /**/
    volatile uint32_t lactloadhi;                              /**/
    volatile uint32_t lactload;                                /**/
    union {
        struct {
            volatile uint32_t t0_int_ena:   1;                 /*interrupt when timer0 alarm*/
            volatile uint32_t t1_int_ena:   1;                 /*interrupt when timer1 alarm*/
            volatile uint32_t wdt_int_ena:  1;                 /*Interrupt when an interrupt stage timeout*/
            volatile uint32_t lact_int_ena: 1;
            volatile uint32_t reserved4:   28;
        };
        volatile uint32_t val;
    }int_ena_timers;
    union {
        struct {
            volatile uint32_t t0_int_raw:   1;                 /*interrupt when timer0 alarm*/
            volatile uint32_t t1_int_raw:   1;                 /*interrupt when timer1 alarm*/
            volatile uint32_t wdt_int_raw:  1;                 /*Interrupt when an interrupt stage timeout*/
            volatile uint32_t lact_int_raw: 1;
            volatile uint32_t reserved4:   28;
        };
        volatile uint32_t val;
    }int_raw_timers;
    union {
        struct {
            volatile uint32_t t0_int_st:   1;                  /*interrupt when timer0 alarm*/
            volatile uint32_t t1_int_st:   1;                  /*interrupt when timer1 alarm*/
            volatile uint32_t wdt_int_st:  1;                  /*Interrupt when an interrupt stage timeout*/
            volatile uint32_t lact_int_st: 1;
            volatile uint32_t reserved4:  28;
        };
        volatile uint32_t val;
    }int_st_timers;
    union {
        struct {
            volatile uint32_t t0_int_clr:   1;                 /*interrupt when timer0 alarm*/
            volatile uint32_t t1_int_clr:   1;                 /*interrupt when timer1 alarm*/
            volatile uint32_t wdt_int_clr:  1;                 /*Interrupt when an interrupt stage timeout*/
            volatile uint32_t lact_int_clr: 1;
            volatile uint32_t reserved4:   28;
        };
        volatile uint32_t val;
    }int_clr_timers;
    volatile uint32_t reserved_a8;
    volatile uint32_t reserved_ac;
    volatile uint32_t reserved_b0;
    volatile uint32_t reserved_b4;
    volatile uint32_t reserved_b8;
    volatile uint32_t reserved_bc;
    volatile uint32_t reserved_c0;
    volatile uint32_t reserved_c4;
    volatile uint32_t reserved_c8;
    volatile uint32_t reserved_cc;
    volatile uint32_t reserved_d0;
    volatile uint32_t reserved_d4;
    volatile uint32_t reserved_d8;
    volatile uint32_t reserved_dc;
    volatile uint32_t reserved_e0;
    volatile uint32_t reserved_e4;
    volatile uint32_t reserved_e8;
    volatile uint32_t reserved_ec;
    volatile uint32_t reserved_f0;
    volatile uint32_t reserved_f4;
    union {
        struct {
            volatile uint32_t date:28;                         /*Version of this regfile*/
            volatile uint32_t reserved28:   4;
        };
        volatile uint32_t val;
    }timg_date;
    union {
        struct {
            volatile uint32_t reserved0: 31;
            volatile uint32_t clk_en:     1;                   /*Force clock enable for this regfile*/
        };
        volatile uint32_t val;
    }clk;
} timg_dev_t;
extern volatile timg_dev_t TIMERG0;
extern volatile timg_dev_t TIMERG1;
#endif  /* _SOC_TIMG_STRUCT_H_ */
