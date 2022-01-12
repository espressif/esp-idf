// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for ADC (common part)

#include "hal/adc_hal.h"
#include "hal/adc_types.h"

void adc_hal_digi_init(void)
{
    adc_hal_init();
    adc_ll_digi_set_clk_div(SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT);
}

void adc_hal_digi_deinit(void)
{
    adc_ll_digi_clear_pattern_table(ADC_NUM_1);
    adc_ll_digi_clear_pattern_table(ADC_NUM_2);
}

void adc_hal_digi_controller_config(const adc_digi_config_t *cfg)
{
    /* Single channel mode or multi channel mode. */
    adc_ll_digi_set_convert_mode(cfg->conv_mode);
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_1) {
        adc_ll_set_controller(ADC_NUM_1, ADC_CTRL_DIG);
        if (cfg->adc1_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_1);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_1, cfg->adc1_pattern_len);
            for (int i = 0; i < cfg->adc1_pattern_len; i++) {
                adc_ll_digi_set_pattern_table(ADC_NUM_1, i, cfg->adc1_pattern[i]);
            }
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        adc_ll_set_controller(ADC_NUM_2, ADC_CTRL_DIG);
        if (cfg->adc2_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_2);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_2, cfg->adc2_pattern_len);
            for (int i = 0; i < cfg->adc2_pattern_len; i++) {
                adc_ll_digi_set_pattern_table(ADC_NUM_2, i, cfg->adc2_pattern[i]);
            }
        }
    }
    adc_ll_digi_set_output_format(cfg->format);
    if (cfg->conv_limit_en) {
        adc_ll_digi_set_convert_limit_num(cfg->conv_limit_num);
        adc_ll_digi_convert_limit_enable();
    } else {
        adc_ll_digi_convert_limit_disable();
    }
    adc_ll_digi_set_data_source(ADC_I2S_DATA_SRC_ADC);
}

int adc_hal_hall_convert(void)
{
    int Sens_Vp0;
    int Sens_Vn0;
    int Sens_Vp1;
    int Sens_Vn1;
    int hall_value;
    // convert for 4 times with different phase and outputs
    adc_ll_hall_phase_disable();      // hall phase
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_0, &Sens_Vp0 );
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_3, &Sens_Vn0 );
    adc_ll_hall_phase_enable();
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_0, &Sens_Vp1 );
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_3, &Sens_Vn1 );
    hall_value = (Sens_Vp1 - Sens_Vp0) - (Sens_Vn1 - Sens_Vn0);
    return hall_value;
}