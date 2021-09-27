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
#ifndef _SOC_RTC_IO_STRUCT_H_
#define _SOC_RTC_IO_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct rtc_io_dev_s {
    union {
        struct {
            uint32_t reserved0:        10;
            uint32_t data:             22;              /*RTC GPIO 0 ~ 21 output data*/
        };
        uint32_t val;
    } out;
    union {
        struct {
            uint32_t reserved0:             10;
            uint32_t w1ts:                  22;         /*RTC GPIO 0 ~ 21 output data write 1 to set*/
        };
        uint32_t val;
    } out_w1ts;
    union {
        struct {
            uint32_t reserved0:             10;
            uint32_t w1tc:                  22;         /*RTC GPIO 0 ~ 21 output data write 1 to clear*/
        };
        uint32_t val;
    } out_w1tc;
    union {
        struct {
            uint32_t reserved0:      10;
            uint32_t enable:         22;                /*RTC GPIO 0 ~ 21 enable*/
        };
        uint32_t val;
    } enable;
    union {
        struct {
            uint32_t reserved0:           10;
            uint32_t w1ts:                22;           /*RTC GPIO 0 ~ 21 enable write 1 to set*/
        };
        uint32_t val;
    } enable_w1ts;
    union {
        struct {
            uint32_t reserved0:           10;
            uint32_t w1tc:                22;           /*RTC GPIO 0 ~ 21 enable write 1 to clear*/
        };
        uint32_t val;
    } enable_w1tc;
    union {
        struct {
            uint32_t reserved0:          10;
            uint32_t status:             22;            /*RTC GPIO 0 ~ 21 interrupt status*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t reserved0:               10;
            uint32_t w1ts:                    22;       /*RTC GPIO 0 ~ 21 interrupt status write 1 to set*/
        };
        uint32_t val;
    } status_w1ts;
    union {
        struct {
            uint32_t reserved0:               10;
            uint32_t w1tc:                    22;       /*RTC GPIO 0 ~ 21 interrupt status write 1 to clear*/
        };
        uint32_t val;
    } status_w1tc;
    union {
        struct {
            uint32_t reserved0:       10;
            uint32_t in:              22;               /*RTC GPIO input data*/
        };
        uint32_t val;
    } in_val;
    union {
        struct {
            uint32_t reserved0:                   2;
            uint32_t pad_driver:                  1;    /*if set to 0: normal output  if set to 1: open drain*/
            uint32_t reserved3:                   4;
            uint32_t int_type:                    3;    /*if set to 0: GPIO interrupt disable   if set to 1: rising edge trigger   if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger   if set to 5: high level trigger*/
            uint32_t wakeup_enable:               1;    /*RTC GPIO wakeup enable bit*/
            uint32_t reserved11:                 21;
        };
        uint32_t val;
    } pin[22];
    union {
        struct {
            uint32_t sel0:                    5;
            uint32_t sel1:                    5;
            uint32_t sel2:                    5;
            uint32_t sel3:                    5;
            uint32_t sel4:                    5;
            uint32_t no_gating_12m:           1;
            uint32_t reserved26:              6;
        };
        uint32_t val;
    } debug_sel;
    union {
        struct {
            uint32_t reserved0:         13;
            uint32_t fun_ie:             1;             /*input enable in work mode*/
            uint32_t slp_oe:             1;             /*output enable in sleep mode*/
            uint32_t slp_ie:             1;             /*input enable in sleep mode*/
            uint32_t slp_sel:            1;             /*1: enable sleep mode during sleep 0: no sleep mode*/
            uint32_t fun_sel:            2;             /*function sel*/
            uint32_t mux_sel:            1;             /*1: use RTC GPIO 0: use digital GPIO*/
            uint32_t xpd:                1;             /*TOUCH_XPD*/
            uint32_t tie_opt:            1;             /*TOUCH_TIE_OPT*/
            uint32_t start:              1;             /*TOUCH_START*/
            uint32_t dac:                3;             /*TOUCH_DAC*/
            uint32_t reserved26:         1;
            uint32_t rue:                1;             /*RUE*/
            uint32_t rde:                1;             /*RDE*/
            uint32_t drv:                2;             /*DRV*/
            uint32_t reserved31:         1;
        };
        uint32_t val;
    } touch_pad[15];
    union {
        struct {
            uint32_t reserved0:   13;
            uint32_t x32p_fun_ie:  1;                   /*input enable in work mode*/
            uint32_t x32p_slp_oe:  1;                   /*output enable in sleep mode*/
            uint32_t x32p_slp_ie:  1;                   /*input enable in sleep mode*/
            uint32_t x32p_slp_sel: 1;                   /*1: enable sleep mode during sleep 0: no sleep mode*/
            uint32_t x32p_fun_sel: 2;                   /*function sel*/
            uint32_t x32p_mux_sel: 1;                   /*1: use RTC GPIO 0: use digital GPIO*/
            uint32_t reserved20:   7;
            uint32_t x32p_rue:     1;                   /*RUE*/
            uint32_t x32p_rde:     1;                   /*RDE*/
            uint32_t x32p_drv:     2;                   /*DRV*/
            uint32_t reserved31:   1;
        };
        uint32_t val;
    } xtal_32p_pad;
    union {
        struct {
            uint32_t reserved0:   13;
            uint32_t x32n_fun_ie:  1;                   /*input enable in work mode*/
            uint32_t x32n_slp_oe:  1;                   /*output enable in sleep mode*/
            uint32_t x32n_slp_ie:  1;                   /*input enable in sleep mode*/
            uint32_t x32n_slp_sel: 1;                   /*1: enable sleep mode during sleep 0: no sleep mode*/
            uint32_t x32n_fun_sel: 2;                   /*function sel*/
            uint32_t x32n_mux_sel: 1;                   /*1: use RTC GPIO 0: use digital GPIO*/
            uint32_t reserved20:   7;
            uint32_t x32n_rue:     1;                   /*RUE*/
            uint32_t x32n_rde:     1;                   /*RDE*/
            uint32_t x32n_drv:     2;                   /*DRV*/
            uint32_t reserved31:   1;
        };
        uint32_t val;
    } xtal_32n_pad;
    union {
        struct {
            uint32_t reserved0:           3;
            uint32_t dac:                 8;            /*PDAC1_DAC*/
            uint32_t xpd_dac:             1;            /*PDAC1_XPD_DAC*/
            uint32_t dac_xpd_force:       1;            /*1: use reg_pdac1_xpd_dac to control PDAC1_XPD_DAC 0: use SAR ADC FSM to control PDAC1_XPD_DAC*/
            uint32_t fun_ie:              1;            /*input enable in work mode*/
            uint32_t slp_oe:              1;            /*output enable in sleep mode*/
            uint32_t slp_ie:              1;            /*input enable in sleep mode*/
            uint32_t slp_sel:             1;            /*1: enable sleep mode during sleep 0: no sleep mode*/
            uint32_t fun_sel:             2;            /*PDAC1 function sel*/
            uint32_t mux_sel:             1;            /*1: use RTC GPIO 0: use digital GPIO*/
            uint32_t reserved20:          7;
            uint32_t rue:                 1;            /*PDAC1_RUE*/
            uint32_t rde:                 1;            /*PDAC1_RDE*/
            uint32_t drv:                 2;            /*PDAC1_DRV*/
            uint32_t reserved31:          1;
        };
        uint32_t val;
    } pad_dac[2];
    union {
        struct {
            uint32_t reserved0:        13;
            uint32_t rtc_pad19_fun_ie:  1;              /*input enable in work mode*/
            uint32_t rtc_pad19_slp_oe:  1;              /*output enable in sleep mode*/
            uint32_t rtc_pad19_slp_ie:  1;              /*input enable in sleep mode*/
            uint32_t rtc_pad19_slp_sel: 1;              /*1: enable sleep mode during sleep 0: no sleep mode*/
            uint32_t rtc_pad19_fun_sel: 2;              /*function sel*/
            uint32_t rtc_pad19_mux_sel: 1;              /*1: use RTC GPIO 0: use digital GPIO*/
            uint32_t reserved20:        7;
            uint32_t rtc_pad19_rue:     1;              /*RUE*/
            uint32_t rtc_pad19_rde:     1;              /*RDE*/
            uint32_t rtc_pad19_drv:     2;              /*DRV*/
            uint32_t reserved31:        1;
        };
        uint32_t val;
    } rtc_pad19;
    union {
        struct {
            uint32_t reserved0:        13;
            uint32_t rtc_pad20_fun_ie:  1;              /*input enable in work mode*/
            uint32_t rtc_pad20_slp_oe:  1;              /*output enable in sleep mode*/
            uint32_t rtc_pad20_slp_ie:  1;              /*input enable in sleep mode*/
            uint32_t rtc_pad20_slp_sel: 1;              /*1: enable sleep mode during sleep 0: no sleep mode*/
            uint32_t rtc_pad20_fun_sel: 2;              /*function sel*/
            uint32_t rtc_pad20_mux_sel: 1;              /*1: use RTC GPIO 0: use digital GPIO*/
            uint32_t reserved20:        7;
            uint32_t rtc_pad20_rue:     1;              /*RUE*/
            uint32_t rtc_pad20_rde:     1;              /*RDE*/
            uint32_t rtc_pad20_drv:     2;              /*DRV*/
            uint32_t reserved31:        1;
        };
        uint32_t val;
    } rtc_pad20;
    union {
        struct {
            uint32_t reserved0:        13;
            uint32_t rtc_pad21_fun_ie:  1;              /*input enable in work mode*/
            uint32_t rtc_pad21_slp_oe:  1;              /*output enable in sleep mode*/
            uint32_t rtc_pad21_slp_ie:  1;              /*input enable in sleep mode*/
            uint32_t rtc_pad21_slp_sel: 1;              /*1: enable sleep mode during sleep 0: no sleep mode*/
            uint32_t rtc_pad21_fun_sel: 2;              /*function sel*/
            uint32_t rtc_pad21_mux_sel: 1;              /*1: use RTC GPIO 0: use digital GPIO*/
            uint32_t reserved20:        7;
            uint32_t rtc_pad21_rue:     1;              /*RUE*/
            uint32_t rtc_pad21_rde:     1;              /*RDE*/
            uint32_t rtc_pad21_drv:     2;              /*DRV*/
            uint32_t reserved31:        1;
        };
        uint32_t val;
    } rtc_pad21;
    union {
        struct {
            uint32_t reserved0:      27;
            uint32_t sel:             5;
        };
        uint32_t val;
    } ext_wakeup0;
    union {
        struct {
            uint32_t reserved0:      27;
            uint32_t sel:             5;                /*select RTC GPIO 0 ~ 17 to control XTAL*/
        };
        uint32_t val;
    } xtl_ext_ctr;
    union {
        struct {
            uint32_t reserved0:        23;
            uint32_t debug_bit_sel:     5;
            uint32_t scl_sel:           2;
            uint32_t sda_sel:           2;
        };
        uint32_t val;
    } sar_i2c_io;
    union {
        struct {
            uint32_t io_touch_bufsel:  4;               /*BUF_SEL when touch work without fsm*/
            uint32_t io_touch_bufmode: 1;               /*BUF_MODE when touch work without fsm*/
            uint32_t reserved5:       27;
        };
        uint32_t val;
    } touch_ctrl;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t reserved_fc;
    uint32_t reserved_100;
    uint32_t reserved_104;
    uint32_t reserved_108;
    uint32_t reserved_10c;
    uint32_t reserved_110;
    uint32_t reserved_114;
    uint32_t reserved_118;
    uint32_t reserved_11c;
    uint32_t reserved_120;
    uint32_t reserved_124;
    uint32_t reserved_128;
    uint32_t reserved_12c;
    uint32_t reserved_130;
    uint32_t reserved_134;
    uint32_t reserved_138;
    uint32_t reserved_13c;
    uint32_t reserved_140;
    uint32_t reserved_144;
    uint32_t reserved_148;
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
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
    union {
        struct {
            uint32_t date:      28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } date;
} rtc_io_dev_t;
extern rtc_io_dev_t RTCIO;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_RTC_IO_STRUCT_H_ */
