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
#ifndef _SOC_RTC_IO_STRUCT_H_
#define _SOC_RTC_IO_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct rtc_io_dev_s {
    union {
        struct {
            uint32_t reserved0:        14;
            uint32_t data:18;              /*GPIO0~17 output value*/
        };
        uint32_t val;
    } out;
    union {
        struct {
            uint32_t reserved0:             14;
            uint32_t w1ts:18;         /*GPIO0~17 output value write 1 to set*/
        };
        uint32_t val;
    } out_w1ts;
    union {
        struct {
            uint32_t reserved0:             14;
            uint32_t w1tc:18;         /*GPIO0~17 output value write 1 to clear*/
        };
        uint32_t val;
    } out_w1tc;
    union {
        struct {
            uint32_t reserved0:      14;
            uint32_t enable:18;                /*GPIO0~17 output enable*/
        };
        uint32_t val;
    } enable;
    union {
        struct {
            uint32_t reserved0:           14;
            uint32_t w1ts:18;           /*GPIO0~17 output enable write 1 to set*/
        };
        uint32_t val;
    } enable_w1ts;
    union {
        struct {
            uint32_t reserved0:           14;
            uint32_t w1tc:18;           /*GPIO0~17 output enable write 1 to clear*/
        };
        uint32_t val;
    } enable_w1tc;
    union {
        struct {
            uint32_t reserved0:          14;
            uint32_t status:18;            /*GPIO0~17 interrupt status*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t reserved0:               14;
            uint32_t w1ts:18;       /*GPIO0~17 interrupt status write 1 to set*/
        };
        uint32_t val;
    } status_w1ts;
    union {
        struct {
            uint32_t reserved0:               14;
            uint32_t w1tc:18;       /*GPIO0~17 interrupt status write 1 to clear*/
        };
        uint32_t val;
    } status_w1tc;
    union {
        struct {
            uint32_t reserved0:       14;
            uint32_t in:18;               /*GPIO0~17 input value*/
        };
        uint32_t val;
    } in_val;
    union {
        struct {
            uint32_t reserved0:                   2;
            uint32_t pad_driver:    1;    /*if set to 0: normal output  if set to 1: open drain*/
            uint32_t reserved3:                   4;
            uint32_t int_type:      3;    /*if set to 0: GPIO interrupt disable  if set to 1: rising edge trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
            uint32_t wakeup_enable: 1;    /*GPIO wake up enable  only available in light sleep*/
            uint32_t reserved11:                 21;
        };
        uint32_t val;
    } pin[18];
    union {
        struct {
            uint32_t sel0:          5;
            uint32_t sel1:          5;
            uint32_t sel2:          5;
            uint32_t sel3:          5;
            uint32_t sel4:          5;
            uint32_t no_gating_12m: 1;
            uint32_t reserved26:              6;
        };
        uint32_t val;
    } debug_sel;
    uint32_t dig_pad_hold;                              /*select the digital pad hold value.*/
    union {
        struct {
            uint32_t reserved0: 30;
            uint32_t hall_phase: 1;                     /*Reverse phase of hall sensor*/
            uint32_t xpd_hall:   1;                     /*Power on hall sensor and connect to VP and VN*/
        };
        uint32_t val;
    } hall_sens;
    union {
        struct {
            uint32_t reserved0:      4;
            uint32_t sense4_fun_ie:  1;                 /*the input enable of the pad*/
            uint32_t sense4_slp_ie:  1;                 /*the input enable of the pad in sleep status*/
            uint32_t sense4_slp_sel: 1;                 /*the sleep status selection signal of the pad*/
            uint32_t sense4_fun_sel: 2;                 /*the functional selection signal of the pad*/
            uint32_t sense3_fun_ie:  1;                 /*the input enable of the pad*/
            uint32_t sense3_slp_ie:  1;                 /*the input enable of the pad in sleep status*/
            uint32_t sense3_slp_sel: 1;                 /*the sleep status selection signal of the pad*/
            uint32_t sense3_fun_sel: 2;                 /*the functional selection signal of the pad*/
            uint32_t sense2_fun_ie:  1;                 /*the input enable of the pad*/
            uint32_t sense2_slp_ie:  1;                 /*the input enable of the pad in sleep status*/
            uint32_t sense2_slp_sel: 1;                 /*the sleep status selection signal of the pad*/
            uint32_t sense2_fun_sel: 2;                 /*the functional selection signal of the pad*/
            uint32_t sense1_fun_ie:  1;                 /*the input enable of the pad*/
            uint32_t sense1_slp_ie:  1;                 /*the input enable of the pad in sleep status*/
            uint32_t sense1_slp_sel: 1;                 /*the sleep status selection signal of the pad*/
            uint32_t sense1_fun_sel: 2;                 /*the functional selection signal of the pad*/
            uint32_t sense4_mux_sel: 1;                 /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t sense3_mux_sel: 1;                 /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t sense2_mux_sel: 1;                 /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t sense1_mux_sel: 1;                 /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t sense4_hold:    1;                 /*hold the current value of the output when setting the hold to �1�*/
            uint32_t sense3_hold:    1;                 /*hold the current value of the output when setting the hold to �1�*/
            uint32_t sense2_hold:    1;                 /*hold the current value of the output when setting the hold to �1�*/
            uint32_t sense1_hold:    1;                 /*hold the current value of the output when setting the hold to �1�*/
        };
        uint32_t val;
    } sensor_pads;
    union {
        struct {
            uint32_t reserved0:   18;
            uint32_t adc2_fun_ie:  1;                   /*the input enable of the pad*/
            uint32_t adc2_slp_ie:  1;                   /*the input enable of the pad in sleep status*/
            uint32_t adc2_slp_sel: 1;                   /*the sleep status selection signal of the pad*/
            uint32_t adc2_fun_sel: 2;                   /*the functional selection signal of the pad*/
            uint32_t adc1_fun_ie:  1;                   /*the input enable of the pad*/
            uint32_t adc1_slp_ie:  1;                   /*the input enable of the pad in sleep status*/
            uint32_t adc1_slp_sel: 1;                   /*the sleep status selection signal of the pad*/
            uint32_t adc1_fun_sel: 2;                   /*the functional selection signal of the pad*/
            uint32_t adc2_mux_sel: 1;                   /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t adc1_mux_sel: 1;                   /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t adc2_hold:    1;                   /*hold the current value of the output when setting the hold to �1�*/
            uint32_t adc1_hold:    1;                   /*hold the current value of the output when setting the hold to �1�*/
        };
        uint32_t val;
    } adc_pad;
    union {
        struct {
            uint32_t reserved0:          10;
            uint32_t dac_xpd_force: 1;            /*Power on DAC1. Usually  we need to tristate PDAC1 if we power on the DAC  i.e. IE=0  OE=0  RDE=0  RUE=0*/
            uint32_t fun_ie:        1;            /*the input enable of the pad*/
            uint32_t slp_oe:        1;            /*the output enable of the pad in sleep status*/
            uint32_t slp_ie:        1;            /*the input enable of the pad in sleep status*/
            uint32_t slp_sel:       1;            /*the sleep status selection signal of the pad*/
            uint32_t fun_sel:       2;            /*the functional selection signal of the pad*/
            uint32_t mux_sel:       1;            /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t xpd_dac:       1;            /*Power on DAC1. Usually  we need to tristate PDAC1 if we power on the DAC  i.e. IE=0  OE=0  RDE=0  RUE=0*/
            uint32_t dac:           8;            /*PAD DAC1 control code.*/
            uint32_t rue:           1;            /*the pull up enable of the pad*/
            uint32_t rde:           1;            /*the pull down enable of the pad*/
            uint32_t hold:          1;            /*hold the current value of the output when setting the hold to �1�*/
            uint32_t drv:           2;            /*the driver strength of the pad*/
        };
        uint32_t val;
    } pad_dac[2];
    union {
        struct {
            uint32_t reserved0:      1;
            uint32_t dbias_xtal_32k: 2;                 /*32K XTAL self-bias reference control.*/
            uint32_t dres_xtal_32k:  2;                 /*32K XTAL resistor bias control.*/
            uint32_t x32p_fun_ie:    1;                 /*the input enable of the pad*/
            uint32_t x32p_slp_oe:    1;                 /*the output enable of the pad in sleep status*/
            uint32_t x32p_slp_ie:    1;                 /*the input enable of the pad in sleep status*/
            uint32_t x32p_slp_sel:   1;                 /*the sleep status selection signal of the pad*/
            uint32_t x32p_fun_sel:   2;                 /*the functional selection signal of the pad*/
            uint32_t x32n_fun_ie:    1;                 /*the input enable of the pad*/
            uint32_t x32n_slp_oe:    1;                 /*the output enable of the pad in sleep status*/
            uint32_t x32n_slp_ie:    1;                 /*the input enable of the pad in sleep status*/
            uint32_t x32n_slp_sel:   1;                 /*the sleep status selection signal of the pad*/
            uint32_t x32n_fun_sel:   2;                 /*the functional selection signal of the pad*/
            uint32_t x32p_mux_sel:   1;                 /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t x32n_mux_sel:   1;                 /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t xpd_xtal_32k:   1;                 /*Power up 32kHz crystal oscillator*/
            uint32_t dac_xtal_32k:   2;                 /*32K XTAL bias current DAC.*/
            uint32_t x32p_rue:       1;                 /*the pull up enable of the pad*/
            uint32_t x32p_rde:       1;                 /*the pull down enable of the pad*/
            uint32_t x32p_hold:      1;                 /*hold the current value of the output when setting the hold to �1�*/
            uint32_t x32p_drv:       2;                 /*the driver strength of the pad*/
            uint32_t x32n_rue:       1;                 /*the pull up enable of the pad*/
            uint32_t x32n_rde:       1;                 /*the pull down enable of the pad*/
            uint32_t x32n_hold:      1;                 /*hold the current value of the output when setting the hold to �1�*/
            uint32_t x32n_drv:       2;                 /*the driver strength of the pad*/
        };
        uint32_t val;
    } xtal_32k_pad;
    union {
        struct {
            uint32_t reserved0:     23;
            uint32_t dcur:     2;                 /*touch sensor bias current. Should have option to tie with BIAS_SLEEP(When BIAS_SLEEP  this setting is available*/
            uint32_t drange:   2;                 /*touch sensor saw wave voltage range.*/
            uint32_t drefl:    2;                 /*touch sensor saw wave bottom voltage.*/
            uint32_t drefh:    2;                 /*touch sensor saw wave top voltage.*/
            uint32_t xpd_bias: 1;                 /*touch sensor bias power on.*/
        };
        uint32_t val;
    } touch_cfg;
    union {
        struct {
            uint32_t reserved0:         12;
            uint32_t to_gpio: 1;             /*connect the rtc pad input to digital pad input �0� is availbale GPIO4*/
            uint32_t fun_ie:  1;             /*the input enable of the pad*/
            uint32_t slp_oe:  1;             /*the output enable of the pad in sleep status*/
            uint32_t slp_ie:  1;             /*the input enable of the pad in sleep status*/
            uint32_t slp_sel: 1;             /*the sleep status selection signal of the pad*/
            uint32_t fun_sel: 2;             /*the functional selection signal of the pad*/
            uint32_t mux_sel: 1;             /*�1� select the digital function  �0�slection the rtc function*/
            uint32_t xpd:     1;             /*touch sensor power on.*/
            uint32_t tie_opt: 1;             /*default touch sensor tie option. 0: tie low  1: tie high.*/
            uint32_t start:   1;             /*start touch sensor.*/
            uint32_t dac:     3;             /*touch sensor slope control. 3-bit for each touch panel  default 100.*/
            uint32_t reserved26:         1;
            uint32_t rue:     1;             /*the pull up enable of the pad*/
            uint32_t rde:     1;             /*the pull down enable of the pad*/
            uint32_t drv:     2;             /*the driver strength of the pad*/
            uint32_t hold:    1;             /*hold the current value of the output when setting the hold to �1�*/
        };
        uint32_t val;
    } touch_pad[10];
    union {
        struct {
            uint32_t reserved0:      27;
            uint32_t sel: 5;                /*select the wakeup source �0� select GPIO0 �1� select GPIO2 ...�17� select GPIO17*/
        };
        uint32_t val;
    } ext_wakeup0;
    union {
        struct {
            uint32_t reserved0:      27;
            uint32_t sel: 5;                /*select the external xtl power source �0� select GPIO0 �1� select GPIO2 ...�17� select GPIO17*/
        };
        uint32_t val;
    } xtl_ext_ctr;
    union {
        struct {
            uint32_t reserved0:        23;
            uint32_t debug_bit_sel: 5;
            uint32_t scl_sel:   2;              /*�0� using TOUCH_PAD[0] as i2c clk �1� using TOUCH_PAD[2] as i2c clk*/
            uint32_t sda_sel:   2;              /*�0� using TOUCH_PAD[1] as i2c sda �1� using TOUCH_PAD[3] as i2c sda*/
        };
        uint32_t val;
    } sar_i2c_io;
    union {
        struct {
            uint32_t date:      28;                     /*date*/
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
