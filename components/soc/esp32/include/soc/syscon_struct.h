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
#ifndef _SOC_SYSCON_STRUCT_H_
#define _SOC_SYSCON_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t pre_div:       10;
            uint32_t clk_320m_en:    1;
            uint32_t clk_en:         1;
            uint32_t rst_tick:       1;
            uint32_t quick_clk_chng: 1;
            uint32_t reserved14:    18;
        };
        uint32_t val;
    }clk_conf;
    union {
        struct {
            uint32_t xtal_tick:     8;
            uint32_t reserved8:    24;
        };
        uint32_t val;
    }xtal_tick_conf;
    union {
        struct {
            uint32_t pll_tick:     8;
            uint32_t reserved8:   24;
        };
        uint32_t val;
    }pll_tick_conf;
    union {
        struct {
            uint32_t ck8m_tick:     8;
            uint32_t reserved8:    24;
        };
        uint32_t val;
    }ck8m_tick_conf;
    union {
        struct {
            uint32_t start_force:       1;
            uint32_t start:             1;
            uint32_t sar2_mux:          1;       /*1: SAR ADC2 is controlled by DIG ADC2 CTRL  0: SAR ADC2 is controlled by PWDET CTRL*/
            uint32_t work_mode:         2;       /*0: single mode  1: double mode  2: alternate mode*/
            uint32_t sar_sel:           1;       /*0: SAR1  1: SAR2  only work for single SAR mode*/
            uint32_t sar_clk_gated:     1;
            uint32_t sar_clk_div:       8;       /*SAR clock divider*/
            uint32_t sar1_patt_len:     4;       /*0 ~ 15 means length 1 ~ 16*/
            uint32_t sar2_patt_len:     4;       /*0 ~ 15 means length 1 ~ 16*/
            uint32_t sar1_patt_p_clear: 1;       /*clear the pointer of pattern table for DIG ADC1 CTRL*/
            uint32_t sar2_patt_p_clear: 1;       /*clear the pointer of pattern table for DIG ADC2 CTRL*/
            uint32_t data_sar_sel:      1;       /*1: sar_sel will be coded by the MSB of the 16-bit output data  in this case the resolution should not be larger than 11 bits.*/
            uint32_t data_to_i2s:       1;       /*1: I2S input data is from SAR ADC (for DMA)  0: I2S input data is from GPIO matrix*/
            uint32_t reserved27:        5;
        };
        uint32_t val;
    }saradc_ctrl;
    union {
        struct {
            uint32_t meas_num_limit: 1;
            uint32_t max_meas_num:   8;          /*max conversion number*/
            uint32_t sar1_inv:       1;          /*1: data to DIG ADC1 CTRL is inverted  otherwise not*/
            uint32_t sar2_inv:       1;          /*1: data to DIG ADC2 CTRL is inverted  otherwise not*/
            uint32_t reserved11:    21;
        };
        uint32_t val;
    }saradc_ctrl2;
    union {
        struct {
            uint32_t rstb_wait:    8;
            uint32_t standby_wait: 8;
            uint32_t start_wait:   8;
            uint32_t sample_cycle: 8;            /*sample cycles*/
        };
        uint32_t val;
    }saradc_fsm;
    uint32_t saradc_sar1_patt_tab[4];                 /*item 0 ~ 3 for ADC1 pattern table*/
    uint32_t saradc_sar2_patt_tab[4];                 /*item 0 ~ 3 for ADC2 pattern table*/
    union {
        struct {
            uint32_t apll_tick:     8;
            uint32_t reserved8:    24;
        };
        uint32_t val;
    }apll_tick_conf;
    uint32_t reserved_40;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t date;                                      /**/
} syscon_dev_t;

#ifdef __cplusplus
}
#endif
extern syscon_dev_t SYSCON;
#endif  /* _SOC_SYSCON_STRUCT_H_ */
