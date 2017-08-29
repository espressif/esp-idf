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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    union {
        struct {
            volatile uint32_t pre_div:       10;
            volatile uint32_t clk_320m_en:    1;
            volatile uint32_t clk_en:         1;
            volatile uint32_t rst_tick:       1;
            volatile uint32_t quick_clk_chng: 1;
            volatile uint32_t reserved14:    18;
        };
        volatile uint32_t val;
    }clk_conf;
    union {
        struct {
            volatile uint32_t xtal_tick:     8;
            volatile uint32_t reserved8:    24;
        };
        volatile uint32_t val;
    }xtal_tick_conf;
    union {
        struct {
            volatile uint32_t pll_tick:     8;
            volatile uint32_t reserved8:   24;
        };
        volatile uint32_t val;
    }pll_tick_conf;
    union {
        struct {
            volatile uint32_t ck8m_tick:     8;
            volatile uint32_t reserved8:    24;
        };
        volatile uint32_t val;
    }ck8m_tick_conf;
    union {
        struct {
            volatile uint32_t start_force:       1;
            volatile uint32_t start:             1;
            volatile uint32_t sar2_mux:          1;       /*1: SAR ADC2 is controlled by DIG ADC2 CTRL  0: SAR ADC2 is controlled by PWDET CTRL*/
            volatile uint32_t work_mode:         2;       /*0: single mode  1: double mode  2: alternate mode*/
            volatile uint32_t sar_sel:           1;       /*0: SAR1  1: SAR2  only work for single SAR mode*/
            volatile uint32_t sar_clk_gated:     1;
            volatile uint32_t sar_clk_div:       8;       /*SAR clock divider*/
            volatile uint32_t sar1_patt_len:     4;       /*0 ~ 15 means length 1 ~ 16*/
            volatile uint32_t sar2_patt_len:     4;       /*0 ~ 15 means length 1 ~ 16*/
            volatile uint32_t sar1_patt_p_clear: 1;       /*clear the pointer of pattern table for DIG ADC1 CTRL*/
            volatile uint32_t sar2_patt_p_clear: 1;       /*clear the pointer of pattern table for DIG ADC2 CTRL*/
            volatile uint32_t data_sar_sel:      1;       /*1: sar_sel will be coded by the MSB of the 16-bit output data  in this case the resolution should not be larger than 11 bits.*/
            volatile uint32_t data_to_i2s:       1;       /*1: I2S input data is from SAR ADC (for DMA)  0: I2S input data is from GPIO matrix*/
            volatile uint32_t reserved27:        5;
        };
        volatile uint32_t val;
    }saradc_ctrl;
    union {
        struct {
            volatile uint32_t meas_num_limit: 1;
            volatile uint32_t max_meas_num:   8;          /*max conversion number*/
            volatile uint32_t sar1_inv:       1;          /*1: data to DIG ADC1 CTRL is inverted  otherwise not*/
            volatile uint32_t sar2_inv:       1;          /*1: data to DIG ADC2 CTRL is inverted  otherwise not*/
            volatile uint32_t reserved11:    21;
        };
        volatile uint32_t val;
    }saradc_ctrl2;
    union {
        struct {
            volatile uint32_t rstb_wait:    8;
            volatile uint32_t standby_wait: 8;
            volatile uint32_t start_wait:   8;
            volatile uint32_t sample_cycle: 8;            /*sample cycles*/
        };
        volatile uint32_t val;
    }saradc_fsm;
    volatile uint32_t saradc_sar1_patt_tab1;                 /*item 0 ~ 3 for pattern table 1 (each item one byte)*/
    volatile uint32_t saradc_sar1_patt_tab2;                 /*Item 4 ~ 7 for pattern table 1 (each item one byte)*/
    volatile uint32_t saradc_sar1_patt_tab3;                 /*Item 8 ~ 11 for pattern table 1 (each item one byte)*/
    volatile uint32_t saradc_sar1_patt_tab4;                 /*Item 12 ~ 15 for pattern table 1 (each item one byte)*/
    volatile uint32_t saradc_sar2_patt_tab1;                 /*item 0 ~ 3 for pattern table 2 (each item one byte)*/
    volatile uint32_t saradc_sar2_patt_tab2;                 /*Item 4 ~ 7 for pattern table 2 (each item one byte)*/
    volatile uint32_t saradc_sar2_patt_tab3;                 /*Item 8 ~ 11 for pattern table 2 (each item one byte)*/
    volatile uint32_t saradc_sar2_patt_tab4;                 /*Item 12 ~ 15 for pattern table 2 (each item one byte)*/
    union {
        struct {
            volatile uint32_t apll_tick:     8;
            volatile uint32_t reserved8:    24;
        };
        volatile uint32_t val;
    }apll_tick_conf;
    volatile uint32_t reserved_40;
    volatile uint32_t reserved_44;
    volatile uint32_t reserved_48;
    volatile uint32_t reserved_4c;
    volatile uint32_t reserved_50;
    volatile uint32_t reserved_54;
    volatile uint32_t reserved_58;
    volatile uint32_t reserved_5c;
    volatile uint32_t reserved_60;
    volatile uint32_t reserved_64;
    volatile uint32_t reserved_68;
    volatile uint32_t reserved_6c;
    volatile uint32_t reserved_70;
    volatile uint32_t reserved_74;
    volatile uint32_t reserved_78;
    volatile uint32_t date;                                      /**/
} syscon_dev_t;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_SYSCON_STRUCT_H_ */
