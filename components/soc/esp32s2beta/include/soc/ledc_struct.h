// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
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
                    uint32_t reserved18: 13;
                    uint32_t clk_en:     1;              /*This bit is clock gating control signal. when software configure LED_PWM internal registers  it controls the register clock.*/
                };
                uint32_t val;
            } conf0;
            union {
                struct {
                    uint32_t hpoint:     14;             /*The output value changes to high when htimerx(x=[0 3]) selected by high speed channel has reached reg_hpoint_hsch0[19:0]*/
                    uint32_t reserved14: 16;
                };
                uint32_t val;
            } hpoint;
            union {
                struct {
                    uint32_t duty:      19;              /*The register is used to control output duty. When hstimerx(x=[0 3]) chosen by high speed channel  has reached reg_lpoint_hsch0 the output signal changes to low. reg_lpoint_hsch0=(reg_hpoint_hsch0[19:0]+reg_duty_hsch0[24:4])          (1)  reg_lpoint_hsch0=(reg_hpoint_hsch0[19:0]+reg_duty_hsch0[24:4] +1)     (2)  The least four bits in this register represent the decimal part and determines when to choose (1) or (2)*/
                    uint32_t reserved19: 11;
                };
                uint32_t val;
            } duty;
            union {
                struct {
                    uint32_t duty_scale:10;              /*This register controls the increase or decrease step scale for high speed channel.*/
                    uint32_t duty_cycle:10;              /*This register is used to increase or decrease the duty every reg_duty_cycle_hsch0 cycles for high speed channel.*/
                    uint32_t duty_num:  10;              /*This register is used to control the number of increased or decreased times for high speed channel.*/
                    uint32_t duty_inc:   1;              /*This register is used to increase the duty of output signal or decrease the duty of output signal for high speed channel.*/
                    uint32_t duty_start: 1;              /*When reg_duty_num_hsch0 reg_duty_cycle_hsch0 and reg_duty_scale_hsch0 has been configured. these register won't take effect until set reg_duty_start_hsch0. this bit is automatically cleared by hardware.*/
                };
                uint32_t val;
            } conf1;
            union {
                struct {
                    uint32_t duty_read: 19;              /*This register represents the current duty of the output signal for high speed channel.*/
                    uint32_t reserved19: 11;
                };
                uint32_t val;
            } duty_rd;
        } channel[8];
    } channel_group[1];                                /*two channel groups : 0: high-speed channels; 1: low-speed channels*/
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
                    uint32_t timer_cnt:  14;               /*software can read this register to get the current counter value in high speed timer*/
                    uint32_t reserved14: 16;
                };
                uint32_t val;
            } value;
        } timer[4];
    } timer_group[1];                                    /*two channel groups : 0: high-speed channels; 1: low-speed channels*/
    union {
        struct {
            uint32_t lstimer0_ovf:        1;           /*The interrupt raw bit for low speed channel0  counter overflow.*/
            uint32_t lstimer1_ovf:        1;           /*The interrupt raw bit for low speed channel1  counter overflow.*/
            uint32_t lstimer2_ovf:        1;           /*The interrupt raw bit for low speed channel2  counter overflow.*/
            uint32_t lstimer3_ovf:        1;           /*The interrupt raw bit for low speed channel3  counter overflow.*/
            uint32_t duty_chng_end_lsch0: 1;           /*The interrupt raw bit for low speed channel 0 duty change done.*/
            uint32_t duty_chng_end_lsch1: 1;           /*The interrupt raw bit for low speed channel 1 duty change done.*/
            uint32_t duty_chng_end_lsch2: 1;           /*The interrupt raw bit for low speed channel 2 duty change done.*/
            uint32_t duty_chng_end_lsch3: 1;           /*The interrupt raw bit for low speed channel 3 duty change done.*/
            uint32_t duty_chng_end_lsch4: 1;           /*The interrupt raw bit for low speed channel 4 duty change done.*/
            uint32_t duty_chng_end_lsch5: 1;           /*The interrupt raw bit for low speed channel 5 duty change done.*/
            uint32_t duty_chng_end_lsch6: 1;           /*The interrupt raw bit for low speed channel 6 duty change done.*/
            uint32_t duty_chng_end_lsch7: 1;           /*The interrupt raw bit for low speed channel 7 duty change done.*/
            uint32_t ovf_cnt_lsch0:       1;
            uint32_t ovf_cnt_lsch1:       1;
            uint32_t ovf_cnt_lsch2:       1;
            uint32_t ovf_cnt_lsch3:       1;
            uint32_t ovf_cnt_lsch4:       1;
            uint32_t ovf_cnt_lsch5:       1;
            uint32_t ovf_cnt_lsch6:       1;
            uint32_t ovf_cnt_lsch7:       1;
            uint32_t reserved20:          12;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t lstimer0_ovf:        1;
            uint32_t lstimer1_ovf:        1;
            uint32_t lstimer2_ovf:        1;
            uint32_t lstimer3_ovf:        1;
            uint32_t duty_chng_end_lsch0: 1;
            uint32_t duty_chng_end_lsch1: 1;
            uint32_t duty_chng_end_lsch2: 1;
            uint32_t duty_chng_end_lsch3: 1;
            uint32_t duty_chng_end_lsch4: 1;
            uint32_t duty_chng_end_lsch5: 1;
            uint32_t duty_chng_end_lsch6: 1;
            uint32_t duty_chng_end_lsch7: 1;
            uint32_t ovf_cnt_lsch0:       1;
            uint32_t ovf_cnt_lsch1:       1;
            uint32_t ovf_cnt_lsch2:       1;
            uint32_t ovf_cnt_lsch3:       1;
            uint32_t ovf_cnt_lsch4:       1;
            uint32_t ovf_cnt_lsch5:       1;
            uint32_t ovf_cnt_lsch6:       1;
            uint32_t ovf_cnt_lsch7:       1;
            uint32_t reserved20:          12;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t lstimer0_ovf:        1;
            uint32_t lstimer1_ovf:        1;
            uint32_t lstimer2_ovf:        1;
            uint32_t lstimer3_ovf:        1;
            uint32_t duty_chng_end_lsch0: 1;
            uint32_t duty_chng_end_lsch1: 1;
            uint32_t duty_chng_end_lsch2: 1;
            uint32_t duty_chng_end_lsch3: 1;
            uint32_t duty_chng_end_lsch4: 1;
            uint32_t duty_chng_end_lsch5: 1;
            uint32_t duty_chng_end_lsch6: 1;
            uint32_t duty_chng_end_lsch7: 1;
            uint32_t ovf_cnt_lsch0:       1;
            uint32_t ovf_cnt_lsch1:       1;
            uint32_t ovf_cnt_lsch2:       1;
            uint32_t ovf_cnt_lsch3:       1;
            uint32_t ovf_cnt_lsch4:       1;
            uint32_t ovf_cnt_lsch5:       1;
            uint32_t ovf_cnt_lsch6:       1;
            uint32_t ovf_cnt_lsch7:       1;
            uint32_t reserved20:          12;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t lstimer0_ovf:        1;
            uint32_t lstimer1_ovf:        1;
            uint32_t lstimer2_ovf:        1;
            uint32_t lstimer3_ovf:        1;
            uint32_t duty_chng_end_lsch0: 1;
            uint32_t duty_chng_end_lsch1: 1;
            uint32_t duty_chng_end_lsch2: 1;
            uint32_t duty_chng_end_lsch3: 1;
            uint32_t duty_chng_end_lsch4: 1;
            uint32_t duty_chng_end_lsch5: 1;
            uint32_t duty_chng_end_lsch6: 1;
            uint32_t duty_chng_end_lsch7: 1;
            uint32_t ovf_cnt_lsch0:       1;
            uint32_t ovf_cnt_lsch1:       1;
            uint32_t ovf_cnt_lsch2:       1;
            uint32_t ovf_cnt_lsch3:       1;
            uint32_t ovf_cnt_lsch4:       1;
            uint32_t ovf_cnt_lsch5:       1;
            uint32_t ovf_cnt_lsch6:       1;
            uint32_t ovf_cnt_lsch7:       1;
            uint32_t reserved20:          12;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t apb_clk_sel: 2;
            uint32_t reserved2:  30;
        };
        uint32_t val;
    } conf;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    uint32_t date;                                     /*This register represents the version .*/
} ledc_dev_t;
extern ledc_dev_t LEDC;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_LEDC_STRUCT_H_ */
