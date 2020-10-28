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
#ifndef _SOC_SENS_STRUCT_H_
#define _SOC_SENS_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t sar1_clk_div:    8;                    /*clock divider*/
            uint32_t reserved8:      10;
            uint32_t sar1_clk_gated:  1;
            uint32_t sar1_sample_num: 8;
            uint32_t reserved27:      1;
            uint32_t sar1_data_inv:   1;                    /*Invert SAR ADC1 data*/
            uint32_t sar1_int_en:     1;                    /*enable saradc1 to send out interrupt*/
            uint32_t reserved30:      2;
        };
        uint32_t val;
    } sar_reader1_ctrl;
    uint32_t sar_reader1_status;                            /**/
    union {
        struct {
            uint32_t reserved0:              22;
            uint32_t rtc_saradc_reset:        1;
            uint32_t rtc_saradc_clkgate_en:   1;
            uint32_t force_xpd_amp:           2;
            uint32_t amp_rst_fb_force:        2;
            uint32_t amp_short_ref_force:     2;
            uint32_t amp_short_ref_gnd_force: 2;
        };
        uint32_t val;
    } sar_meas1_ctrl1;
    union {
        struct {
            uint32_t meas1_data_sar:   16;                  /*SAR ADC1 data*/
            uint32_t meas1_done_sar:    1;                  /*SAR ADC1 conversion done indication*/
            uint32_t meas1_start_sar:   1;                  /*SAR ADC1 controller (in RTC) starts conversion*/
            uint32_t meas1_start_force: 1;                  /*1: SAR ADC1 controller (in RTC) is started by SW*/
            uint32_t sar1_en_pad:      12;                  /*SAR ADC1 pad enable bitmap*/
            uint32_t sar1_en_pad_force: 1;                  /*1: SAR ADC1 pad enable bitmap is controlled by SW*/
        };
        uint32_t val;
    } sar_meas1_ctrl2;
    union {
        struct {
            uint32_t reserved0:     31;
            uint32_t sar1_dig_force: 1;                     /*1: SAR ADC1 controlled by DIG ADC1 CTRL*/
        };
        uint32_t val;
    } sar_meas1_mux;
    uint32_t sar_atten1;                                    /*2-bit attenuation for each pad*/
    union {
        struct {
            uint32_t sar_amp_wait1:16;
            uint32_t sar_amp_wait2:16;
        };
        uint32_t val;
    } sar_amp_ctrl1;
    union {
        struct {
            uint32_t sar1_dac_xpd_fsm_idle:      1;
            uint32_t xpd_sar_amp_fsm_idle:       1;
            uint32_t amp_rst_fb_fsm_idle:        1;
            uint32_t amp_short_ref_fsm_idle:     1;
            uint32_t amp_short_ref_gnd_fsm_idle: 1;
            uint32_t xpd_sar_fsm_idle:           1;
            uint32_t sar_rstb_fsm_idle:          1;
            uint32_t reserved7:                  9;
            uint32_t sar_amp_wait3:             16;
        };
        uint32_t val;
    } sar_amp_ctrl2;
    union {
        struct {
            uint32_t sar1_dac_xpd_fsm:      4;
            uint32_t xpd_sar_amp_fsm:       4;
            uint32_t amp_rst_fb_fsm:        4;
            uint32_t amp_short_ref_fsm:     4;
            uint32_t amp_short_ref_gnd_fsm: 4;
            uint32_t xpd_sar_fsm:           4;
            uint32_t sar_rstb_fsm:          4;
            uint32_t reserved28:            4;
        };
        uint32_t val;
    } sar_amp_ctrl3;
    union {
        struct {
            uint32_t sar2_clk_div:        8;                /*clock divider*/
            uint32_t reserved8:           8;
            uint32_t sar2_wait_arb_cycle: 2;                /*wait arbit stable after sar_done*/
            uint32_t sar2_clk_gated:      1;
            uint32_t sar2_sample_num:     8;
            uint32_t reserved27:          2;
            uint32_t sar2_data_inv:       1;                /*Invert SAR ADC2 data*/
            uint32_t sar2_int_en:         1;                /*enable saradc2 to send out interrupt*/
            uint32_t reserved31:          1;
        };
        uint32_t val;
    } sar_reader2_ctrl;
    uint32_t sar_reader2_status;                            /**/
    union {
        struct {
            uint32_t sar2_cntl_state:   3;                  /*saradc2_cntl_fsm*/
            uint32_t sar2_pwdet_cal_en: 1;                  /*rtc control pwdet enable*/
            uint32_t sar2_pkdet_cal_en: 1;                  /*rtc control pkdet enable*/
            uint32_t sar2_en_test:      1;                  /*SAR2_EN_TEST*/
            uint32_t sar2_rstb_force:   2;
            uint32_t sar2_standby_wait: 8;
            uint32_t sar2_rstb_wait:    8;
            uint32_t sar2_xpd_wait:     8;
        };
        uint32_t val;
    } sar_meas2_ctrl1;
    union {
        struct {
            uint32_t meas2_data_sar:   16;                  /*SAR ADC2 data*/
            uint32_t meas2_done_sar:    1;                  /*SAR ADC2 conversion done indication*/
            uint32_t meas2_start_sar:   1;                  /*SAR ADC2 controller (in RTC) starts conversion*/
            uint32_t meas2_start_force: 1;                  /*1: SAR ADC2 controller (in RTC) is started by SW*/
            uint32_t sar2_en_pad:      12;                  /*SAR ADC2 pad enable bitmap*/
            uint32_t sar2_en_pad_force: 1;                  /*1: SAR ADC2 pad enable bitmap is controlled by SW*/
        };
        uint32_t val;
    } sar_meas2_ctrl2;
    union {
        struct {
            uint32_t reserved0:     28;
            uint32_t sar2_pwdet_cct: 3;                     /*SAR2_PWDET_CCT*/
            uint32_t sar2_rtc_force: 1;                     /*in sleep  force to use rtc to control ADC*/
        };
        uint32_t val;
    } sar_meas2_mux;
    uint32_t sar_atten2;                                    /*2-bit attenuation for each pad*/
    union {
        struct {
            uint32_t reserved0:    29;
            uint32_t force_xpd_sar: 2;
            uint32_t sarclk_en:     1;
        };
        uint32_t val;
    } sar_power_xpd_sar;
    union {
        struct {
            uint32_t i2c_slave_addr1:   11;
            uint32_t i2c_slave_addr0:   11;
            uint32_t meas_status:        8;
            uint32_t reserved30:         2;
        };
        uint32_t val;
    } sar_slave_addr1;
    union {
        struct {
            uint32_t i2c_slave_addr3:11;
            uint32_t i2c_slave_addr2:11;
            uint32_t reserved22:     10;
        };
        uint32_t val;
    } sar_slave_addr2;
    union {
        struct {
            uint32_t i2c_slave_addr5:11;
            uint32_t i2c_slave_addr4:11;
            uint32_t reserved22:     10;
        };
        uint32_t val;
    } sar_slave_addr3;
    union {
        struct {
            uint32_t i2c_slave_addr7:11;
            uint32_t i2c_slave_addr6:11;
            uint32_t reserved22:     10;
        };
        uint32_t val;
    } sar_slave_addr4;
    union {
        struct {
            uint32_t tsens_out:            8;               /*temperature sensor data out*/
            uint32_t tsens_ready:          1;               /*indicate temperature sensor out ready*/
            uint32_t reserved9:            3;
            uint32_t tsens_int_en:         1;               /*enable temperature sensor to send out interrupt*/
            uint32_t tsens_in_inv:         1;               /*invert temperature sensor data*/
            uint32_t tsens_clk_div:        8;               /*temperature sensor clock divider*/
            uint32_t tsens_power_up:       1;               /*temperature sensor power up*/
            uint32_t tsens_power_up_force: 1;               /*1: dump out & power up controlled by SW*/
            uint32_t tsens_dump_out:       1;               /*temperature sensor dump out*/
            uint32_t reserved25:           7;
        };
        uint32_t val;
    } sar_tctrl;
    union {
        struct {
            uint32_t tsens_xpd_wait:  12;
            uint32_t tsens_xpd_force:  2;
            uint32_t tsens_clk_inv:    1;
            uint32_t tsens_clkgate_en: 1;                   /*temperature sensor clock enable*/
            uint32_t tsens_reset:      1;                   /*temperature sensor reset*/
            uint32_t reserved17:      15;
        };
        uint32_t val;
    } sar_tctrl2;
    union {
        struct {
            uint32_t sar_i2c_ctrl:       28;                /*I2C control data*/
            uint32_t sar_i2c_start:       1;                /*start I2C*/
            uint32_t sar_i2c_start_force: 1;                /*1: I2C started by SW*/
            uint32_t reserved30:          2;
        };
        uint32_t val;
    } sar_i2c_ctrl;
    union {
        struct {
            uint32_t touch_outen:        15;                /*touch controller output enable*/
            uint32_t touch_status_clr:    1;                /*clear all touch active status*/
            uint32_t touch_data_sel:      2;                /*3: smooth data 2: benchmark 1 0: raw_data*/
            uint32_t touch_denoise_end:   1;                /*touch_denoise_done*/
            uint32_t touch_unit_end:      1;                /*touch_unit_done*/
            uint32_t touch_approach_pad2: 4;                /*indicate which pad is approach pad2*/
            uint32_t touch_approach_pad1: 4;                /*indicate which pad is approach pad1*/
            uint32_t touch_approach_pad0: 4;                /*indicate which pad is approach pad0*/
        };
        uint32_t val;
    } sar_touch_conf;
    union {
        struct {
            uint32_t thresh:       22;                      /*Finger threshold for touch pad 1*/
            uint32_t reserved22:   10;
        };
        uint32_t val;
    } touch_thresh[14];
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
    union {
        struct {
            uint32_t touch_pad_active: 15;                  /*touch active status*/
            uint32_t touch_channel_clr:15;                  /*Clear touch channel*/
            uint32_t reserved30:        1;
            uint32_t touch_meas_done:   1;
        };
        uint32_t val;
    } sar_touch_chn_st;
    union {
        struct {
            uint32_t touch_denoise_data:22;                 /*the counter for touch pad 0*/
            uint32_t touch_scan_curr:    4;
            uint32_t reserved26:         6;
        };
        uint32_t val;
    } sar_touch_status0;
    union {
        struct {
            uint32_t touch_pad_data: 22;
            uint32_t reserved22:          7;
            uint32_t touch_pad_debounce:  3;
        };
        uint32_t val;
    } sar_touch_status[14];
    union {
        struct {
            uint32_t touch_slp_data:    22;
            uint32_t reserved22:         7;
            uint32_t touch_slp_debounce: 3;
        };
        uint32_t val;
    } sar_touch_slp_status;
    union {
        struct {
            uint32_t touch_approach_pad2_cnt: 8;
            uint32_t touch_approach_pad1_cnt: 8;
            uint32_t touch_approach_pad0_cnt: 8;
            uint32_t touch_slp_approach_cnt:  8;
        };
        uint32_t val;
    } sar_touch_appr_status;
    union {
        struct {
            uint32_t sw_fstep:          16;                 /*frequency step for CW generator*/
            uint32_t sw_tone_en:         1;                 /*1: enable CW generator*/
            uint32_t debug_bit_sel:      5;
            uint32_t dac_dig_force:      1;                 /*1: DAC1 & DAC2 use DMA*/
            uint32_t dac_clk_force_low:  1;                 /*1: force PDAC_CLK to low*/
            uint32_t dac_clk_force_high: 1;                 /*1: force PDAC_CLK to high*/
            uint32_t dac_clk_inv:        1;                 /*1: invert PDAC_CLK*/
            uint32_t dac_reset:          1;
            uint32_t dac_clkgate_en:     1;
            uint32_t reserved28:         4;
        };
        uint32_t val;
    } sar_dac_ctrl1;
    union {
        struct {
            uint32_t dac_dc1:    8;                         /*DC offset for DAC1 CW generator*/
            uint32_t dac_dc2:    8;                         /*DC offset for DAC2 CW generator*/
            uint32_t dac_scale1: 2;                         /*00: no scale*/
            uint32_t dac_scale2: 2;                         /*00: no scale*/
            uint32_t dac_inv1:   2;                         /*00: do not invert any bits*/
            uint32_t dac_inv2:   2;                         /*00: do not invert any bits*/
            uint32_t dac_cw_en1: 1;                         /*1: to select CW generator as source to PDAC1_DAC[7:0]*/
            uint32_t dac_cw_en2: 1;                         /*1: to select CW generator as source to PDAC2_DAC[7:0]*/
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } sar_dac_ctrl2;
    union {
        struct {
            uint32_t reserved0:        25;
            uint32_t dbg_trigger:       1;                  /*trigger cocpu debug registers*/
            uint32_t clk_en:            1;                  /*check cocpu whether clk on*/
            uint32_t reset_n:           1;                  /*check cocpu whether in reset state*/
            uint32_t eoi:               1;                  /*check cocpu whether in interrupt state*/
            uint32_t trap:              1;                  /*check cocpu whether in trap state*/
            uint32_t ebreak:            1;                  /*check cocpu whether in ebreak*/
            uint32_t reserved31:        1;
        };
        uint32_t val;
    } sar_cocpu_state;
    union {
        struct {
            uint32_t touch_done:                   1;       /*int from touch done*/
            uint32_t touch_inactive:               1;       /*int from touch inactive*/
            uint32_t touch_active:                 1;       /*int from touch active*/
            uint32_t saradc1:                      1;       /*int from saradc1*/
            uint32_t saradc2:                      1;       /*int from saradc2*/
            uint32_t tsens:                        1;       /*int from tsens*/
            uint32_t start:                        1;       /*int from start*/
            uint32_t sw:                           1;       /*int from software*/
            uint32_t swd:                          1;       /*int from super watch dog*/
            uint32_t reserved9:                   23;
        };
        uint32_t val;
    } sar_cocpu_int_raw;
    union {
        struct {
            uint32_t touch_done:                   1;
            uint32_t touch_inactive:               1;
            uint32_t touch_active:                 1;
            uint32_t saradc1:                      1;
            uint32_t saradc2:                      1;
            uint32_t tsens:                        1;
            uint32_t start:                        1;
            uint32_t sw:                           1;       /*cocpu int enable*/
            uint32_t swd:                          1;
            uint32_t reserved9:                   23;
        };
        uint32_t val;
    } sar_cocpu_int_ena;
    union {
        struct {
            uint32_t touch_done:                  1;
            uint32_t touch_inactive:              1;
            uint32_t touch_active:                1;
            uint32_t saradc1:                     1;
            uint32_t saradc2:                     1;
            uint32_t tsens:                       1;
            uint32_t start:                       1;
            uint32_t sw:                          1;        /*cocpu int status*/
            uint32_t swd:                         1;
            uint32_t reserved9:                  23;
        };
        uint32_t val;
    } sar_cocpu_int_st;
    union {
        struct {
            uint32_t touch_done:                   1;
            uint32_t touch_inactive:               1;
            uint32_t touch_active:                 1;
            uint32_t saradc1:                      1;
            uint32_t saradc2:                      1;
            uint32_t tsens:                        1;
            uint32_t start:                        1;
            uint32_t sw:                           1;       /*cocpu int clear*/
            uint32_t swd:                          1;
            uint32_t reserved9:                   23;
        };
        uint32_t val;
    } sar_cocpu_int_clr;
    union {
        struct {
            uint32_t pc:            13;                     /*cocpu Program counter*/
            uint32_t mem_vld:        1;                     /*cocpu mem valid output*/
            uint32_t mem_rdy:        1;                     /*cocpu mem ready input*/
            uint32_t mem_wen:        4;                     /*cocpu mem write enable output*/
            uint32_t mem_addr:      13;                     /*cocpu mem address output*/
        };
        uint32_t val;
    } sar_cocpu_debug;
    union {
        struct {
            uint32_t reserved0:       28;
            uint32_t xpd_hall:         1;                   /*Power on hall sensor and connect to VP and VN*/
            uint32_t xpd_hall_force:   1;                   /*1: XPD HALL is controlled by SW. 0: XPD HALL is controlled by FSM in ULP-coprocessor*/
            uint32_t hall_phase:       1;                   /*Reverse phase of hall sensor*/
            uint32_t hall_phase_force: 1;                   /*1: HALL PHASE is controlled by SW  0: HALL PHASE is controlled by FSM in ULP-coprocessor*/
        };
        uint32_t val;
    } sar_hall_ctrl;
    uint32_t sar_nouse;                                     /**/
    union {
        struct {
            uint32_t reserved0:        30;
            uint32_t iomux_reset:       1;
            uint32_t iomux_clk_gate_en: 1;
        };
        uint32_t val;
    } sar_io_mux_conf;
    union {
        struct {
            uint32_t sar_date:  28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } sardate;
} sens_dev_t;
extern sens_dev_t SENS;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_SENS_STRUCT_H_ */
