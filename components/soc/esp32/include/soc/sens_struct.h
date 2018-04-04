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
            uint32_t sar1_clk_div:      8;
            uint32_t sar1_sample_cycle: 8;
            uint32_t sar1_sample_bit:   2;
            uint32_t sar1_clk_gated:    1;
            uint32_t sar1_sample_num:   8;
            uint32_t sar1_dig_force:    1;                /*1: ADC1 is controlled by the digital controller  0: RTC controller*/
            uint32_t sar1_data_inv:     1;
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
    uint32_t ulp_cp_sleep_cyc0;                          /**/
    uint32_t ulp_cp_sleep_cyc1;                          /**/
    uint32_t ulp_cp_sleep_cyc2;                          /**/
    uint32_t ulp_cp_sleep_cyc3;                          /**/
    uint32_t ulp_cp_sleep_cyc4;                          /**/
    union {
        struct {
            uint32_t sar1_bit_width:         2;
            uint32_t sar2_bit_width:         2;
            uint32_t sar2_en_test:           1;
            uint32_t sar2_pwdet_cct:         3;
            uint32_t ulp_cp_force_start_top: 1;
            uint32_t ulp_cp_start_top:       1;
            uint32_t sarclk_en:              1;
            uint32_t pc_init:               11;
            uint32_t sar2_stop:              1;
            uint32_t sar1_stop:              1;
            uint32_t sar2_pwdet_en:          1;
            uint32_t reserved25:             7;
        };
        uint32_t val;
    } sar_start_force;
    union {
        struct {
            uint32_t mem_wr_addr_init:    11;
            uint32_t mem_wr_addr_size:    11;
            uint32_t rtc_mem_wr_offst_clr: 1;
            uint32_t reserved23:           9;
        };
        uint32_t val;
    } sar_mem_wr_ctrl;
    uint32_t sar_atten1;                                  /**/
    uint32_t sar_atten2;                                  /**/
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
            uint32_t tsens_out:       8;
            uint32_t tsens_rdy_out:   1;
            uint32_t reserved31:      1;
        };
        uint32_t val;
    } sar_slave_addr3;
    union {
        struct {
            uint32_t i2c_slave_addr7:11;
            uint32_t i2c_slave_addr6:11;
            uint32_t i2c_rdata:       8;
            uint32_t i2c_done:        1;
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
            uint32_t tsens_in_inv:         1;
            uint32_t tsens_clk_div:        8;
            uint32_t tsens_power_up:       1;
            uint32_t tsens_power_up_force: 1;
            uint32_t tsens_dump_out:       1;
            uint32_t reserved27:           5;
        };
        uint32_t val;
    } sar_tctrl;
    union {
        struct {
            uint32_t sar_i2c_ctrl:       28;
            uint32_t sar_i2c_start:       1;
            uint32_t sar_i2c_start_force: 1;
            uint32_t reserved30:          2;
        };
        uint32_t val;
    } sar_i2c_ctrl;
    union {
        struct {
            uint32_t meas1_data_sar:   16;
            uint32_t meas1_done_sar:    1;
            uint32_t meas1_start_sar:   1;
            uint32_t meas1_start_force: 1;                /*1: ADC1 is controlled by the digital or RTC controller  0: Ulp coprocessor*/
            uint32_t sar1_en_pad:      12;
            uint32_t sar1_en_pad_force: 1;                /*1: Data ports are controlled by the digital or RTC controller  0: Ulp coprocessor*/
        };
        uint32_t val;
    } sar_meas_start1;
    union {
        struct {
            uint32_t touch_meas_delay:16;
            uint32_t touch_xpd_wait:   8;
            uint32_t touch_out_sel:    1;
            uint32_t touch_out_1en:    1;
            uint32_t xpd_hall_force:   1;                 /*1: Power of hall sensor is controlled by the digital or RTC controller  0: Ulp coprocessor*/
            uint32_t hall_phase_force: 1;                 /*1: Phase of hall sensor is controlled by the digital or RTC controller  0: Ulp coprocessor*/
            uint32_t reserved28:       4;
        };
        uint32_t val;
    } sar_touch_ctrl1;
    union {
        struct {
            uint32_t l_thresh: 16;
            uint32_t h_thresh: 16;
        };
        uint32_t val;
    } touch_thresh[5];
    union {
        struct {
            uint32_t l_val: 16;
            uint32_t h_val: 16;
        };
        uint32_t val;
    } touch_meas[5];
    union {
        struct {
            uint32_t touch_meas_en:     10;
            uint32_t touch_meas_done:    1;
            uint32_t touch_start_fsm_en: 1;
            uint32_t touch_start_en:     1;
            uint32_t touch_start_force:  1;
            uint32_t touch_sleep_cycles:16;
            uint32_t touch_meas_en_clr:  1;
            uint32_t reserved31:         1;
        };
        uint32_t val;
    } sar_touch_ctrl2;
    uint32_t reserved_88;
    union {
        struct {
            uint32_t touch_pad_worken:10;
            uint32_t touch_pad_outen2:10;
            uint32_t touch_pad_outen1:10;
            uint32_t reserved30:       2;
        };
        uint32_t val;
    } sar_touch_enable;
    union {
        struct {
            uint32_t sar2_clk_div:      8;
            uint32_t sar2_sample_cycle: 8;
            uint32_t sar2_sample_bit:   2;
            uint32_t sar2_clk_gated:    1;
            uint32_t sar2_sample_num:   8;
            uint32_t sar2_pwdet_force:  1;                /*1: ADC2 is controlled by PWDET  0: digital or RTC controller*/
            uint32_t sar2_dig_force:    1;                /*1: ADC2 is controlled by the digital controller  0: RTC controller*/
            uint32_t sar2_data_inv:     1;
            uint32_t reserved30:        2;
        };
        uint32_t val;
    } sar_read_ctrl2;
    union {
        struct {
            uint32_t meas2_data_sar:   16;
            uint32_t meas2_done_sar:    1;
            uint32_t meas2_start_sar:   1;
            uint32_t meas2_start_force: 1;                /*1: ADC2 is controlled by the digital or RTC controller  0: Ulp coprocessor*/
            uint32_t sar2_en_pad:      12;
            uint32_t sar2_en_pad_force: 1;                /*1: Data ports are controlled by the digital or RTC controller  0: Ulp coprocessor*/
        };
        uint32_t val;
    } sar_meas_start2;
    union {
        struct {
            uint32_t sw_fstep:          16;
            uint32_t sw_tone_en:         1;
            uint32_t debug_bit_sel:      5;
            uint32_t dac_dig_force:      1;
            uint32_t dac_clk_force_low:  1;
            uint32_t dac_clk_force_high: 1;
            uint32_t dac_clk_inv:        1;
            uint32_t reserved26:         6;
        };
        uint32_t val;
    } sar_dac_ctrl1;
    union {
        struct {
            uint32_t dac_dc1:    8;
            uint32_t dac_dc2:    8;
            uint32_t dac_scale1: 2;
            uint32_t dac_scale2: 2;
            uint32_t dac_inv1:   2;
            uint32_t dac_inv2:   2;
            uint32_t dac_cw_en1: 1;
            uint32_t dac_cw_en2: 1;
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
