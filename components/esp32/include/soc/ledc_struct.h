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
#ifndef _SOC_LEDC_STRUCT_H_
#define _SOC_LEDC_STRUCT_H_
typedef struct {
    struct{
        union {
            struct {
                volatile uint32_t timer_sel:  2;              /*There are four high speed timers  the two bits are used to select one of them for high speed channel.  2'b00: seletc hstimer0.   2'b01: select hstimer1.  2'b10: select hstimer2.    2'b11: select hstimer3.*/
                volatile uint32_t sig_out_en: 1;              /*This is the output enable control bit for high speed channel*/
                volatile uint32_t idle_lv:    1;              /*This bit is used to control the output value when high speed channel is off.*/
                volatile uint32_t reserved4: 27;
                volatile uint32_t clk_en:     1;              /*This bit is clock gating control signal. when software configure LED_PWM internal registers  it controls the register clock.*/
            };
            volatile uint32_t val;
        }conf0;
        union {
            struct {
                volatile uint32_t hpoint:     20;             /*The output value changes to high when htimerx(x=[0 3]) selected by high speed channel has reached reg_hpoint_hsch0[19:0]*/
                volatile uint32_t reserved20: 12;
            };
            volatile uint32_t val;
        }hpoint;
        union {
            struct {
                volatile uint32_t duty:      25;              /*The register is used to control output duty. When hstimerx(x=[0 3]) chosen by high speed channel  has reached reg_lpoint_hsch0 the output signal changes to low. reg_lpoint_hsch0=(reg_hpoint_hsch0[19:0]+reg_duty_hsch0[24:4])          (1)  reg_lpoint_hsch0=(reg_hpoint_hsch0[19:0]+reg_duty_hsch0[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
                volatile uint32_t reserved25: 7;
            };
            volatile uint32_t val;
        }duty;
        union {
            struct {
                volatile uint32_t duty_scale:10;              /*This register controls the increase or decrease step scale for high speed channel.*/
                volatile uint32_t duty_cycle:10;              /*This register is used to increase or decrease the duty every reg_duty_cycle_hsch0 cycles for high speed channel.*/
                volatile uint32_t duty_num:  10;              /*This register is used to control the number of increased or decreased times for high speed channel.*/
                volatile uint32_t duty_inc:   1;              /*This register is used to increase the duty of output signal or decrease the duty of output signal for high speed channel.*/
                volatile uint32_t duty_start: 1;              /*When reg_duty_num_hsch0 reg_duty_cycle_hsch0 and reg_duty_scale_hsch0 has been configured. these register won't take effect until set reg_duty_start_hsch0. this bit is automatically cleared by hardware.*/
            };
            volatile uint32_t val;
        }conf1;
        union {
            struct {
                volatile uint32_t duty_read: 25;              /*This register represents the current duty of the output signal for high speed channel.*/
                volatile uint32_t reserved25: 7;
            };
            volatile uint32_t val;
        }duty_rd;
    }high_speed_channel[8];
    struct{
        union {
            struct {
                volatile uint32_t timer_sel:  2;              /*There are four low speed timers  the two bits are used to select one of them for low speed channel.  2'b00: seletc lstimer0.   2'b01: select lstimer1.  2'b10: select lstimer2.    2'b11: select lstimer3.*/
                volatile uint32_t sig_out_en: 1;              /*This is the output enable control bit for low speed channel.*/
                volatile uint32_t idle_lv:    1;              /*This bit is used to control the output value when low speed channel is off.*/
                volatile uint32_t para_up:    1;              /*This bit is used to update register LEDC_LSCH0_HPOINT and LEDC_LSCH0_DUTY for low speed channel.*/
                volatile uint32_t reserved5: 27;
            };
            volatile uint32_t val;
        }conf0;
        union {
            struct {
                volatile uint32_t hpoint:     20;             /*The output value changes to high when lstimerx(x=[0 3]) selected by low speed channel has reached reg_hpoint_lsch0[19:0]*/
                volatile uint32_t reserved20: 12;
            };
            volatile uint32_t val;
        }hpoint;
        union {
            struct {
                volatile uint32_t duty:      25;              /*The register is used to control output duty. When lstimerx(x=[0 3]) choosed by low speed channel has reached reg_lpoint_lsch0 the output signal changes to low. reg_lpoint_lsch0=(reg_hpoint_lsch0[19:0]+reg_duty_lsch0[24:4])          (1)  reg_lpoint_lsch0=(reg_hpoint_lsch0[19:0]+reg_duty_lsch0[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
                volatile uint32_t reserved25: 7;
            };
            volatile uint32_t val;
        }duty;
        union {
            struct {
                volatile uint32_t duty_scale:10;              /*This register controls the increase or decrease step scale for low speed channel.*/
                volatile uint32_t duty_cycle:10;              /*This register is used to increase or decrease the duty every reg_duty_cycle_lsch0 cycles for low speed channel.*/
                volatile uint32_t duty_num:  10;              /*This register is used to control the num of increased or decreased times for low speed channel6.*/
                volatile uint32_t duty_inc:   1;              /*This register is used to increase the duty of output signal or decrease the duty of output signal for low speed channel6.*/
                volatile uint32_t duty_start: 1;              /*When reg_duty_num_hsch1 reg_duty_cycle_hsch1 and reg_duty_scale_hsch1 has been configured. these register won't take effect until set reg_duty_start_hsch1. this bit is automatically cleared by hardware.*/
            };
            volatile uint32_t val;
        }conf1;
        union {
            struct {
                volatile uint32_t duty_read: 25;              /*This register represents the current duty of the output signal for low speed channel.*/
                volatile uint32_t reserved25: 7;
            };
            volatile uint32_t val;
        }duty_r;
    }low_speed_channel[8];
    struct{
        union {
            struct {
                volatile uint32_t timer_lim:  5;               /*This register controls the range of the counter in high speed timer. the counter range is [0 2**reg_hstimer0_lim] the max bit width for counter is 20.*/
                volatile uint32_t div_num:   18;               /*This register is used to configure parameter for divider in high speed timer the least significant eight bits represent the decimal part.*/
                volatile uint32_t pause:      1;               /*This bit is used to pause the counter in high speed timer*/
                volatile uint32_t rst:        1;               /*This bit is used to reset high speed timer the counter will be 0 after reset.*/
                volatile uint32_t tick_sel:   1;               /*This bit is used to choose apb_clk or ref_tick for high speed timer. 1'b1:apb_clk  0:ref_tick*/
                volatile uint32_t reserved26: 6;
            };
            volatile uint32_t val;
        }conf;
        union {
            struct {
                volatile uint32_t timer_cnt:  20;               /*software can read this register to get the current counter value in high speed timer*/
                volatile uint32_t reserved20: 12;
            };
            volatile uint32_t val;
        }value;
    }high_speed_timer[4];
    struct{
        union {
            struct {
                volatile uint32_t timer_lim:    5;              /*This register controls the range of the counter in low speed timer. the counter range is [0 2**reg_lstimer0_lim] the max bit width for counter is 20.*/
                volatile uint32_t div_num:     18;              /*This register is used to configure parameter for divider in low speed timer the least significant eight bits represent the decimal part.*/
                volatile uint32_t pause:        1;              /*This bit is used to pause the counter in low speed timer.*/
                volatile uint32_t rst:          1;              /*This bit is used to reset low speed timer the counter will be 0 after reset.*/
                volatile uint32_t tick_sel:     1;              /*This bit is used to choose slow_clk or ref_tick for low speed timer. 1'b1:slow_clk  0:ref_tick*/
                volatile uint32_t param_update: 1;              /*Set this bit  to update  reg_div_num_lstime0 and  reg_lstimer0_lim.*/
                volatile uint32_t reserved27:   5;
            };
            volatile uint32_t val;
        }conf;
        union {
            struct {
                volatile uint32_t timer_cnt:   20;              /*software can read this register to get the current counter value in low speed timer.*/
                volatile uint32_t reserved20:  12;
            };
            volatile uint32_t val;
        }value;
    }low_speed_timer[4];
    union {
        struct {
            volatile uint32_t hstimer0_ovf_int_raw:        1;   /*The interrupt raw bit for high speed channel0  counter overflow.*/
            volatile uint32_t hstimer1_ovf_int_raw:        1;   /*The interrupt raw bit for high speed channel1  counter overflow.*/
            volatile uint32_t hstimer2_ovf_int_raw:        1;   /*The interrupt raw bit for high speed channel2  counter overflow.*/
            volatile uint32_t hstimer3_ovf_int_raw:        1;   /*The interrupt raw bit for high speed channel3  counter overflow.*/
            volatile uint32_t lstimer0_ovf_int_raw:        1;   /*The interrupt raw bit for low speed channel0  counter overflow.*/
            volatile uint32_t lstimer1_ovf_int_raw:        1;   /*The interrupt raw bit for low speed channel1  counter overflow.*/
            volatile uint32_t lstimer2_ovf_int_raw:        1;   /*The interrupt raw bit for low speed channel2  counter overflow.*/
            volatile uint32_t lstimer3_ovf_int_raw:        1;   /*The interrupt raw bit for low speed channel3  counter overflow.*/
            volatile uint32_t duty_chng_end_hsch0_int_raw: 1;   /*The interrupt raw bit for high speed channel 0 duty change done.*/
            volatile uint32_t duty_chng_end_hsch1_int_raw: 1;   /*The interrupt raw bit for high speed channel 1 duty change done.*/
            volatile uint32_t duty_chng_end_hsch2_int_raw: 1;   /*The interrupt raw bit for high speed channel 2 duty change done.*/
            volatile uint32_t duty_chng_end_hsch3_int_raw: 1;   /*The interrupt raw bit for high speed channel 3 duty change done.*/
            volatile uint32_t duty_chng_end_hsch4_int_raw: 1;   /*The interrupt raw bit for high speed channel 4 duty change done.*/
            volatile uint32_t duty_chng_end_hsch5_int_raw: 1;   /*The interrupt raw bit for high speed channel 5 duty change done.*/
            volatile uint32_t duty_chng_end_hsch6_int_raw: 1;   /*The interrupt raw bit for high speed channel 6 duty change done.*/
            volatile uint32_t duty_chng_end_hsch7_int_raw: 1;   /*The interrupt raw bit for high speed channel 7 duty change done.*/
            volatile uint32_t duty_chng_end_lsch0_int_raw: 1;   /*The interrupt raw bit for low speed channel 0 duty change done.*/
            volatile uint32_t duty_chng_end_lsch1_int_raw: 1;   /*The interrupt raw bit for low speed channel 1 duty change done.*/
            volatile uint32_t duty_chng_end_lsch2_int_raw: 1;   /*The interrupt raw bit for low speed channel 2 duty change done.*/
            volatile uint32_t duty_chng_end_lsch3_int_raw: 1;   /*The interrupt raw bit for low speed channel 3 duty change done.*/
            volatile uint32_t duty_chng_end_lsch4_int_raw: 1;   /*The interrupt raw bit for low speed channel 4 duty change done.*/
            volatile uint32_t duty_chng_end_lsch5_int_raw: 1;   /*The interrupt raw bit for low speed channel 5 duty change done.*/
            volatile uint32_t duty_chng_end_lsch6_int_raw: 1;   /*The interrupt raw bit for low speed channel 6 duty change done.*/
            volatile uint32_t duty_chng_end_lsch7_int_raw: 1;   /*The interrupt raw bit for low speed channel 7 duty change done.*/
            volatile uint32_t reserved24:                  8;
        };
        volatile uint32_t val;
    }int_raw;
    union {
        struct {
            volatile uint32_t hstimer0_ovf_int_st:        1;    /*The interrupt status bit for high speed channel0  counter overflow event.*/
            volatile uint32_t hstimer1_ovf_int_st:        1;    /*The interrupt status bit for high speed channel1  counter overflow event.*/
            volatile uint32_t hstimer2_ovf_int_st:        1;    /*The interrupt status bit for high speed channel2  counter overflow event.*/
            volatile uint32_t hstimer3_ovf_int_st:        1;    /*The interrupt status bit for high speed channel3  counter overflow event.*/
            volatile uint32_t lstimer0_ovf_int_st:        1;    /*The interrupt status bit for low speed channel0  counter overflow event.*/
            volatile uint32_t lstimer1_ovf_int_st:        1;    /*The interrupt status bit for low speed channel1  counter overflow event.*/
            volatile uint32_t lstimer2_ovf_int_st:        1;    /*The interrupt status bit for low speed channel2  counter overflow event.*/
            volatile uint32_t lstimer3_ovf_int_st:        1;    /*The interrupt status bit for low speed channel3  counter overflow event.*/
            volatile uint32_t duty_chng_end_hsch0_int_st: 1;    /*The interrupt status bit for high speed channel 0 duty change done event.*/
            volatile uint32_t duty_chng_end_hsch1_int_st: 1;    /*The interrupt status bit for high speed channel 1 duty change done event.*/
            volatile uint32_t duty_chng_end_hsch2_int_st: 1;    /*The interrupt status bit for high speed channel 2 duty change done event.*/
            volatile uint32_t duty_chng_end_hsch3_int_st: 1;    /*The interrupt status bit for high speed channel 3 duty change done event.*/
            volatile uint32_t duty_chng_end_hsch4_int_st: 1;    /*The interrupt status bit for high speed channel 4 duty change done event.*/
            volatile uint32_t duty_chng_end_hsch5_int_st: 1;    /*The interrupt status bit for high speed channel 5 duty change done event.*/
            volatile uint32_t duty_chng_end_hsch6_int_st: 1;    /*The interrupt status bit for high speed channel 6 duty change done event.*/
            volatile uint32_t duty_chng_end_hsch7_int_st: 1;    /*The interrupt status bit for high speed channel 7 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch0_int_st: 1;    /*The interrupt status bit for low speed channel 0 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch1_int_st: 1;    /*The interrupt status bit for low speed channel 1 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch2_int_st: 1;    /*The interrupt status bit for low speed channel 2 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch3_int_st: 1;    /*The interrupt status bit for low speed channel 3 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch4_int_st: 1;    /*The interrupt status bit for low speed channel 4 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch5_int_st: 1;    /*The interrupt status bit for low speed channel 5 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch6_int_st: 1;    /*The interrupt status bit for low speed channel 6 duty change done event.*/
            volatile uint32_t duty_chng_end_lsch7_int_st: 1;    /*The interrupt status bit for low speed channel 7 duty change done event*/
            volatile uint32_t reserved24:                 8;
        };
        volatile uint32_t val;
    }int_st;
    union {
        struct {
            volatile uint32_t hstimer0_ovf_int_ena:        1;   /*The interrupt enable bit for high speed channel0  counter overflow interrupt.*/
            volatile uint32_t hstimer1_ovf_int_ena:        1;   /*The interrupt enable bit for high speed channel1  counter overflow interrupt.*/
            volatile uint32_t hstimer2_ovf_int_ena:        1;   /*The interrupt enable bit for high speed channel2  counter overflow interrupt.*/
            volatile uint32_t hstimer3_ovf_int_ena:        1;   /*The interrupt enable bit for high speed channel3  counter overflow interrupt.*/
            volatile uint32_t lstimer0_ovf_int_ena:        1;   /*The interrupt enable bit for low speed channel0  counter overflow interrupt.*/
            volatile uint32_t lstimer1_ovf_int_ena:        1;   /*The interrupt enable bit for low speed channel1  counter overflow interrupt.*/
            volatile uint32_t lstimer2_ovf_int_ena:        1;   /*The interrupt enable bit for low speed channel2  counter overflow interrupt.*/
            volatile uint32_t lstimer3_ovf_int_ena:        1;   /*The interrupt enable bit for low speed channel3  counter overflow interrupt.*/
            volatile uint32_t duty_chng_end_hsch0_int_ena: 1;   /*The interrupt enable bit for high speed channel 0 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch1_int_ena: 1;   /*The interrupt enable bit for high speed channel 1 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch2_int_ena: 1;   /*The interrupt enable bit for high speed channel 2 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch3_int_ena: 1;   /*The interrupt enable bit for high speed channel 3 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch4_int_ena: 1;   /*The interrupt enable bit for high speed channel 4 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch5_int_ena: 1;   /*The interrupt enable bit for high speed channel 5 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch6_int_ena: 1;   /*The interrupt enable bit for high speed channel 6 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch7_int_ena: 1;   /*The interrupt enable bit for high speed channel 7 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch0_int_ena: 1;   /*The interrupt enable bit for low speed channel 0 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch1_int_ena: 1;   /*The interrupt enable bit for low speed channel 1 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch2_int_ena: 1;   /*The interrupt enable bit for low speed channel 2 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch3_int_ena: 1;   /*The interrupt enable bit for low speed channel 3 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch4_int_ena: 1;   /*The interrupt enable bit for low speed channel 4 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch5_int_ena: 1;   /*The interrupt enable bit for low speed channel 5 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch6_int_ena: 1;   /*The interrupt enable bit for low speed channel 6 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch7_int_ena: 1;   /*The interrupt enable bit for low speed channel 7 duty change done interrupt.*/
            volatile uint32_t reserved24:                  8;
        };
        volatile uint32_t val;
    }int_ena;
    union {
        struct {
            volatile uint32_t hstimer0_ovf_int_clr:        1;   /*Set this  bit to clear  high speed channel0  counter overflow interrupt.*/
            volatile uint32_t hstimer1_ovf_int_clr:        1;   /*Set this  bit to clear  high speed channel1  counter overflow interrupt.*/
            volatile uint32_t hstimer2_ovf_int_clr:        1;   /*Set this  bit to clear  high speed channel2  counter overflow interrupt.*/
            volatile uint32_t hstimer3_ovf_int_clr:        1;   /*Set this  bit to clear  high speed channel3  counter overflow interrupt.*/
            volatile uint32_t lstimer0_ovf_int_clr:        1;   /*Set this  bit to clear  low speed channel0  counter overflow interrupt.*/
            volatile uint32_t lstimer1_ovf_int_clr:        1;   /*Set this  bit to clear  low speed channel1  counter overflow interrupt.*/
            volatile uint32_t lstimer2_ovf_int_clr:        1;   /*Set this  bit to clear  low speed channel2  counter overflow interrupt.*/
            volatile uint32_t lstimer3_ovf_int_clr:        1;   /*Set this  bit to clear  low speed channel3  counter overflow interrupt.*/
            volatile uint32_t duty_chng_end_hsch0_int_clr: 1;   /*Set this  bit to clear  high speed channel 0 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch1_int_clr: 1;   /*Set this  bit to clear  high speed channel 1 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch2_int_clr: 1;   /*Set this  bit to clear  high speed channel 2 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch3_int_clr: 1;   /*Set this  bit to clear  high speed channel 3 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch4_int_clr: 1;   /*Set this  bit to clear  high speed channel 4 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch5_int_clr: 1;   /*Set this  bit to clear  high speed channel 5 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch6_int_clr: 1;   /*Set this  bit to clear  high speed channel 6 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_hsch7_int_clr: 1;   /*Set this  bit to clear  high speed channel 7 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch0_int_clr: 1;   /*Set this  bit to clear  low speed channel 0 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch1_int_clr: 1;   /*Set this  bit to clear  low speed channel 1 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch2_int_clr: 1;   /*Set this  bit to clear  low speed channel 2 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch3_int_clr: 1;   /*Set this  bit to clear  low speed channel 3 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch4_int_clr: 1;   /*Set this  bit to clear  low speed channel 4 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch5_int_clr: 1;   /*Set this  bit to clear  low speed channel 5 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch6_int_clr: 1;   /*Set this  bit to clear  low speed channel 6 duty change done interrupt.*/
            volatile uint32_t duty_chng_end_lsch7_int_clr: 1;   /*Set this  bit to clear  low speed channel 7 duty change done interrupt.*/
            volatile uint32_t reserved24:                  8;
        };
        volatile uint32_t val;
    }int_clr;
    union {
        struct {
            volatile uint32_t apb_clk_sel: 1;                   /*This bit is used to set the frequency of slow_clk. 1'b1:80mhz  1'b0:8mhz*/
            volatile uint32_t reserved1:  31;
        };
        volatile uint32_t val;
    }conf;
    volatile uint32_t reserved_194;
    volatile uint32_t reserved_198;
    volatile uint32_t reserved_19c;
    volatile uint32_t reserved_1a0;
    volatile uint32_t reserved_1a4;
    volatile uint32_t reserved_1a8;
    volatile uint32_t reserved_1ac;
    volatile uint32_t reserved_1b0;
    volatile uint32_t reserved_1b4;
    volatile uint32_t reserved_1b8;
    volatile uint32_t reserved_1bc;
    volatile uint32_t reserved_1c0;
    volatile uint32_t reserved_1c4;
    volatile uint32_t reserved_1c8;
    volatile uint32_t reserved_1cc;
    volatile uint32_t reserved_1d0;
    volatile uint32_t reserved_1d4;
    volatile uint32_t reserved_1d8;
    volatile uint32_t reserved_1dc;
    volatile uint32_t reserved_1e0;
    volatile uint32_t reserved_1e4;
    volatile uint32_t reserved_1e8;
    volatile uint32_t reserved_1ec;
    volatile uint32_t reserved_1f0;
    volatile uint32_t reserved_1f4;
    volatile uint32_t reserved_1f8;
    volatile uint32_t date;                                     /*This register represents the version .*/
} ledc_dev_t;
extern volatile ledc_dev_t LEDC;
#endif  /* _SOC_LEDC_STRUCT_H_ */
