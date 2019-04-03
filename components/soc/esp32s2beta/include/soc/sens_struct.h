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
#ifndef _SOC_SENS_STRUCT_H_
#define _SOC_SENS_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t sar1_clk_div:      8;                /*clock divider*/
            uint32_t sar1_sample_cycle: 8;                /*sample cycles for SAR ADC1*/
            uint32_t sar1_sample_bit:   2;                /*00: for 9-bit width  01: for 10-bit width  10: for 11-bit width  11: for 12-bit width*/
            uint32_t sar1_clk_gated:    1;
            uint32_t sar1_sample_num:   8;
            uint32_t sar1_dig_force:    1;                /*1: SAR ADC1 controlled by DIG ADC1 CTRL  0: SAR ADC1 controlled by RTC ADC1 CTRL*/
            uint32_t sar1_data_inv:     1;                /*Invert SAR ADC1 data*/
            uint32_t reserved29:        3;
        };
        uint32_t val;
    } sar_read_ctrl;
    uint32_t sar_read_status1;                            /**/
    union {
        struct {
            uint32_t sar_amp_wait1:16;
            uint32_t sar_amp_wait2:16;
        };
        uint32_t val;
    } sar_meas_wait1;
    union {
        struct {
            uint32_t sar_amp_wait3: 16;
            uint32_t force_xpd_amp:  2;
            uint32_t force_xpd_sar:  2;
            uint32_t sar2_rstb_wait: 8;
            uint32_t reserved28:     4;
        };
        uint32_t val;
    } sar_meas_wait2;
    union {
        struct {
            uint32_t xpd_sar_amp_fsm:       4;
            uint32_t amp_rst_fb_fsm:        4;
            uint32_t amp_short_ref_fsm:     4;
            uint32_t amp_short_ref_gnd_fsm: 4;
            uint32_t xpd_sar_fsm:           4;
            uint32_t sar_rstb_fsm:          4;
            uint32_t sar2_xpd_wait:         8;
        };
        uint32_t val;
    } sar_meas_ctrl;
    uint32_t sar_read_status2;                            /**/
    uint32_t ulp_cp_sleep_cyc0;                           /*sleep cycles for ULP-coprocessor timer*/
    uint32_t ulp_cp_sleep_cyc1;                           /**/
    uint32_t ulp_cp_sleep_cyc2;                           /**/
    uint32_t ulp_cp_sleep_cyc3;                           /**/
    uint32_t ulp_cp_sleep_cyc4;                           /**/
    union {
        struct {
            uint32_t sar1_bit_width:         2;           /*00: 9 bit  01: 10 bits  10: 11bits  11: 12bits*/
            uint32_t sar2_bit_width:         2;           /*00: 9 bit  01: 10 bits  10: 11bits  11: 12bits*/
            uint32_t sar2_en_test:           1;           /*SAR2_EN_TEST  only active when reg_sar2_dig_force = 0*/
            uint32_t sar2_pwdet_cct:         3;           /*SAR2_PWDET_CCT  PA power detector capacitance tuning.*/
            uint32_t ulp_cp_force_start_top: 1;           /*1: ULP-coprocessor is started by SW  0: ULP-coprocessor is started by timer*/
            uint32_t ulp_cp_start_top:       1;           /*Write 1 to start ULP-coprocessor  only active when reg_ulp_cp_force_start_top = 1*/
            uint32_t sarclk_en:              1;
            uint32_t pc_init:               11;           /*initialized PC for ULP-coprocessor*/
            uint32_t sar2_stop:              1;           /*stop SAR ADC2 conversion*/
            uint32_t sar1_stop:              1;           /*stop SAR ADC1 conversion*/
            uint32_t sar2_pwdet_en:          1;           /*N/A*/
            uint32_t reserved25:             7;
        };
        uint32_t val;
    } sar_start_force;
    union {
        struct {
            uint32_t mem_wr_addr_init:    11;
            uint32_t mem_wr_addr_size:    11;
            uint32_t rtc_mem_wr_offst_clr: 1;
            uint32_t ulp_cp_clk_fo:        1;             /*ulp coprocessor clk force on*/
            uint32_t reserved24:           8;
        };
        uint32_t val;
    } sar_mem_wr_ctrl;
    uint32_t sar_atten1;                                  /*2-bit attenuation for each pad  11:1dB  10:6dB  01:3dB  00:0dB*/
    uint32_t sar_atten2;                                  /*2-bit attenuation for each pad  11:1dB  10:6dB  01:3dB  00:0dB*/
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
            uint32_t tsens_out:       8;                  /*temperature sensor data out*/
            uint32_t tsens_rdy_out:   1;                  /*indicate temperature sensor out ready*/
            uint32_t reserved31:      1;
        };
        uint32_t val;
    } sar_slave_addr3;
    union {
        struct {
            uint32_t i2c_slave_addr7:11;
            uint32_t i2c_slave_addr6:11;
            uint32_t i2c_rdata:       8;                  /*I2C read data*/
            uint32_t i2c_done:        1;                  /*indicate I2C done*/
            uint32_t reserved31:      1;
        };
        uint32_t val;
    } sar_slave_addr4;
    union {
        struct {
            uint32_t tsens_xpd_wait:      12;
            uint32_t tsens_xpd_force:      1;
            uint32_t tsens_clk_inv:        1;
            uint32_t tsens_clk_gated:      1;
            uint32_t tsens_in_inv:         1;             /*invert temperature sensor data*/
            uint32_t tsens_clk_div:        8;             /*temperature sensor clock divider*/
            uint32_t tsens_power_up:       1;             /*temperature sensor power up*/
            uint32_t tsens_power_up_force: 1;             /*1: dump out & power up controlled by SW  0: by FSM*/
            uint32_t tsens_dump_out:       1;             /*temperature sensor dump out  only active when reg_tsens_power_up_force = 1*/
            uint32_t tsens_dos:            4;             /*Temperature sensor calibration bits*/
            uint32_t tsens_force:          1;             /*1: select saradc_reg 0: select efuse*/
        };
        uint32_t val;
    } sar_tctrl;
    union {
        struct {
            uint32_t sar_i2c_ctrl:       28;              /*I2C control data  only active when reg_sar_i2c_start_force = 1*/
            uint32_t sar_i2c_start:       1;              /*start I2C  only active when reg_sar_i2c_start_force = 1*/
            uint32_t sar_i2c_start_force: 1;              /*1: I2C started by SW  0: I2C started by FSM*/
            uint32_t reserved30:          2;
        };
        uint32_t val;
    } sar_i2c_ctrl;
    union {
        struct {
            uint32_t meas1_data_sar:   16;                /*SAR ADC1 data*/
            uint32_t meas1_done_sar:    1;                /*SAR ADC1 conversion done indication*/
            uint32_t meas1_start_sar:   1;                /*SAR ADC1 controller (in RTC) starts conversion  only active when reg_meas1_start_force = 1*/
            uint32_t meas1_start_force: 1;                /*1: SAR ADC1 controller (in RTC) is started by SW  0: SAR ADC1 controller is started by ULP-coprocessor*/
            uint32_t sar1_en_pad:      12;                /*SAR ADC1 pad enable bitmap  only active when reg_sar1_en_pad_force = 1*/
            uint32_t sar1_en_pad_force: 1;                /*1: SAR ADC1 pad enable bitmap is controlled by SW  0: SAR ADC1 pad enable bitmap is controlled by ULP-coprocessor*/
        };
        uint32_t val;
    } sar_meas_start1;
    union {
        struct {
            uint32_t touch_meas_delay:16;                 /*the meas length (in 8MHz)*/
            uint32_t touch_xpd_wait:   8;                 /*the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD*/
            uint32_t touch_out_sel:    1;                 /*1: when the counter is greater then the threshold  the touch pad is considered as “touched”  0: when the counter is less than the threshold  the touch pad is considered as “touched”*/
            uint32_t touch_out_1en:    1;                 /*1: wakeup interrupt is generated if SET1 is “touched”  0: wakeup interrupt is generated only if SET1 & SET2 is both “touched”*/
            uint32_t xpd_hall_force:   1;                 /*1: XPD HALL is controlled by SW. 0: XPD HALL is controlled by FSM in ULP-coprocessor*/
            uint32_t hall_phase_force: 1;                 /*1: HALL PHASE is controlled by SW  0: HALL PHASE is controlled by FSM in ULP-coprocessor*/
            uint32_t reserved28:       4;
        };
        uint32_t val;
    } sar_touch_ctrl1;
    union {
        struct {
            uint32_t l_thresh:     16;                    /*the threshold for touch pad 1*/
            uint32_t h_thresh:     16;                    /*the threshold for touch pad 0*/
        };
        uint32_t val;
    } touch_thresh[5];
    union {
        struct {
            uint32_t l_val:          16;                  /*the counter for touch pad 1*/
            uint32_t h_val:          16;                  /*the counter for touch pad 0*/
        };
        uint32_t val;
    } touch_meas[5];
    union {
        struct {
            uint32_t touch_meas_en:     10;               /*10-bit register to indicate which pads are “touched”*/
            uint32_t touch_meas_done:    1;               /*fsm set 1 to indicate touch touch meas is done*/
            uint32_t touch_start_fsm_en: 1;               /*1: TOUCH_START & TOUCH_XPD is controlled by touch fsm  0: TOUCH_START & TOUCH_XPD is controlled by registers*/
            uint32_t touch_start_en:     1;               /*1: start touch fsm  valid when reg_touch_start_force is set*/
            uint32_t touch_start_force:  1;               /*1: to start touch fsm by SW  0: to start touch fsm by timer*/
            uint32_t touch_sleep_cycles:16;               /*sleep cycles for timer*/
            uint32_t touch_meas_en_clr:  1;               /*to clear reg_touch_meas_en*/
            uint32_t reserved31:         1;
        };
        uint32_t val;
    } sar_touch_ctrl2;
    uint32_t reserved_88;
    union {
        struct {
            uint32_t touch_pad_worken:10;                 /*Bitmap defining the working set during the measurement.*/
            uint32_t touch_pad_outen2:10;                 /*Bitmap defining SET2 for generating wakeup interrupt. SET2 is “touched” only if at least one of touch pad in SET2 is “touched”.*/
            uint32_t touch_pad_outen1:10;                 /*Bitmap defining SET1 for generating wakeup interrupt. SET1 is “touched” only if at least one of touch pad in SET1 is “touched”.*/
            uint32_t reserved30:       2;
        };
        uint32_t val;
    } sar_touch_enable;
    union {
        struct {
            uint32_t touch_meas_raw:10;                   /*touch sensor raw result*/
            uint32_t reserved10:    22;
        };
        uint32_t val;
    } sar_touch_ctrl3;
    union {
        struct {
            uint32_t sar2_clk_div:      8;                /*clock divider*/
            uint32_t sar2_sample_cycle: 8;                /*sample cycles for SAR ADC2*/
            uint32_t sar2_sample_bit:   2;                /*00: for 9-bit width  01: for 10-bit width  10: for 11-bit width  11: for 12-bit width*/
            uint32_t sar2_clk_gated:    1;
            uint32_t sar2_sample_num:   8;
            uint32_t sar2_pwdet_force:  1;
            uint32_t sar2_dig_force:    1;                /*1: SAR ADC2 controlled by DIG ADC2 CTRL or PWDET CTRL  0: SAR ADC2 controlled by RTC ADC2 CTRL*/
            uint32_t sar2_data_inv:     1;                /*Invert SAR ADC2 data*/
            uint32_t reserved30:        2;
        };
        uint32_t val;
    } sar_read_ctrl2;
    union {
        struct {
            uint32_t meas2_data_sar:   16;                /*SAR ADC2 data*/
            uint32_t meas2_done_sar:    1;                /*SAR ADC2 conversion done indication*/
            uint32_t meas2_start_sar:   1;                /*SAR ADC2 controller (in RTC) starts conversion  only active when reg_meas2_start_force = 1*/
            uint32_t meas2_start_force: 1;                /*1: SAR ADC2 controller (in RTC) is started by SW  0: SAR ADC2 controller is started by ULP-coprocessor*/
            uint32_t sar2_en_pad:      12;                /*SAR ADC2 pad enable bitmap  only active when reg_sar2_en_pad_force = 1*/
            uint32_t sar2_en_pad_force: 1;                /*1: SAR ADC2 pad enable bitmap is controlled by SW  0: SAR ADC2 pad enable bitmap is controlled by ULP-coprocessor*/
        };
        uint32_t val;
    } sar_meas_start2;
    union {
        struct {
            uint32_t sw_fstep:          16;               /*frequency step for CW generator  can be used to adjust the frequency*/
            uint32_t sw_tone_en:         1;               /*1: enable CW generator  0: disable CW generator*/
            uint32_t debug_bit_sel:      5;
            uint32_t dac_dig_force:      1;               /*1: DAC1 & DAC2 use DMA  0: DAC1 & DAC2 do not use DMA*/
            uint32_t dac_clk_force_low:  1;               /*1: force PDAC_CLK to low*/
            uint32_t dac_clk_force_high: 1;               /*1: force PDAC_CLK to high*/
            uint32_t dac_clk_inv:        1;               /*1: invert PDAC_CLK*/
            uint32_t reserved26:         6;
        };
        uint32_t val;
    } sar_dac_ctrl1;
    union {
        struct {
            uint32_t dac_dc1:    8;                       /*DC offset for DAC1 CW generator*/
            uint32_t dac_dc2:    8;                       /*DC offset for DAC2 CW generator*/
            uint32_t dac_scale1: 2;                       /*00: no scale  01: scale to 1/2  10: scale to 1/4  scale to 1/8*/
            uint32_t dac_scale2: 2;                       /*00: no scale  01: scale to 1/2  10: scale to 1/4  scale to 1/8*/
            uint32_t dac_inv1:   2;                       /*00: do not invert any bits  01: invert all bits  10: invert MSB  11: invert all bits except MSB*/
            uint32_t dac_inv2:   2;                       /*00: do not invert any bits  01: invert all bits  10: invert MSB  11: invert all bits except MSB*/
            uint32_t dac_cw_en1: 1;                       /*1: to select CW generator as source to PDAC1_DAC[7:0]  0: to select register reg_pdac1_dac[7:0] as source to PDAC1_DAC[7:0]*/
            uint32_t dac_cw_en2: 1;                       /*1: to select CW generator as source to PDAC2_DAC[7:0]  0: to select register reg_pdac2_dac[7:0] as source to PDAC2_DAC[7:0]*/
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } sar_dac_ctrl2;
    union {
        struct {
            uint32_t sar1_dac_xpd_fsm:           4;
            uint32_t sar1_dac_xpd_fsm_idle:      1;
            uint32_t xpd_sar_amp_fsm_idle:       1;
            uint32_t amp_rst_fb_fsm_idle:        1;
            uint32_t amp_short_ref_fsm_idle:     1;
            uint32_t amp_short_ref_gnd_fsm_idle: 1;
            uint32_t xpd_sar_fsm_idle:           1;
            uint32_t sar_rstb_fsm_idle:          1;
            uint32_t sar2_rstb_force:            2;
            uint32_t amp_rst_fb_force:           2;
            uint32_t amp_short_ref_force:        2;
            uint32_t amp_short_ref_gnd_force:    2;
            uint32_t reserved19:                13;
        };
        uint32_t val;
    } sar_meas_ctrl2;
    union {
        struct {
            uint32_t clk_fo:                  1;          /*cocpu clk force on*/
            uint32_t start_2_reset_dis:       6;          /*time from start cocpu to pull down reset*/
            uint32_t start_2_intr_en:         6;          /*time from start cocpu to give start interrupt*/
            uint32_t shut:                    1;          /*to shut cocpu*/
            uint32_t shut_2_clk_dis:          6;          /*time from shut cocpu to disable clk*/
            uint32_t shut_reset_en:           1;          /*to reset cocpu*/
            uint32_t sel:                     1;          /*1: old ULP 0: new riscV*/
            uint32_t done_force:              1;          /*1: select riscv done 0: select ulp done*/
            uint32_t done:                    1;          /*done signal used by riscv to control timer.*/
            uint32_t int_trigger:             1;          /*trigger cocpu register interrupt*/
            uint32_t clk_en:                  1;          /*check cocpu whether clk on*/
            uint32_t reset_n:                 1;          /*check cocpu whether in reset state*/
            uint32_t eoi:                     1;          /*check cocpu whether in interrupt state*/
            uint32_t trap:                    1;          /*check cocpu whether in trap state*/
            uint32_t reserved29:              3;
        };
        uint32_t val;
    } sar_cocpu_ctrl;
    union {
        struct {
            uint32_t saradc_int_ena:               1;
            uint32_t tsens_int_ena:                1;
            uint32_t start_int_ena:                1;
            uint32_t cocpu_int_ena:                1;
            uint32_t ebreak_int_ena:               1;             /*int enable entry*/
            uint32_t reserved5:            5;
            uint32_t saradc_int_clr:               1;
            uint32_t tsens_int_clr:                1;
            uint32_t start_int_clr:                1;
            uint32_t cocpu_int_clr:                1;
            uint32_t ebreak_int_clr:               1;             /*int clear entry*/
            uint32_t reserved15:           5;
            uint32_t saradc_int:           1;             /*int from saradc*/
            uint32_t tsens_int:            1;             /*int from tsens*/
            uint32_t start_int:            1;             /*int from start*/
            uint32_t cocpu_int:                  1;             /*int from register*/
            uint32_t ebreak_int:           1;             /*int from ebreak*/
            uint32_t reserved25:           7;
        };
        uint32_t val;
    } sar_cocpu_int;
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
    uint32_t sar_nouse;                                   /**/
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
