// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "hal/adc_hal.h"

void adc_hal_init(void)
{
    adc_ll_set_power_manage(ADC_POWER_BY_FSM);
    // Set internal FSM wait time, fixed value.
    adc_ll_dig_set_fsm_time(SOC_ADC_FSM_RSTB_WAIT_DEFAULT, SOC_ADC_FSM_START_WAIT_DEFAULT,
                            SOC_ADC_FSM_STANDBY_WAIT_DEFAULT);
    adc_ll_dig_set_sample_cycle(ADC_FSM_SAMPLE_CYCLE_DEFAULT);
    adc_ll_output_invert(ADC_NUM_1, SOC_ADC1_DATA_INVERT_DEFAULT);
    adc_ll_output_invert(ADC_NUM_2, SOC_ADC2_DATA_INVERT_DEFAULT);
}

void adc_hal_dig_controller_config(const adc_hal_dig_config_t *cfg)
{
    /* If enable digtal controller, adc xpd should always on. */
    adc_ll_set_power_manage(ADC_POWER_SW_ON);
    adc_ll_set_clk_div(cfg->clk_div);
    /* Single channel mode or multi channel mode. */
    adc_ll_dig_set_convert_mode(cfg->conv_mode);
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_1) {
        adc_ll_set_controller(ADC_NUM_1, ADC_CTRL_DIG);
        adc_ll_set_pattern_table_len(ADC_NUM_1, cfg->adc1_pattern_len);
        for (int i = 0; i < cfg->adc1_pattern_len; i++) {
            adc_ll_set_pattern_table(ADC_NUM_1, i, cfg->adc1_pattern[i]);
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        adc_ll_set_controller(ADC_NUM_2, ADC_CTRL_DIG);
        adc_ll_set_pattern_table_len(ADC_NUM_2, cfg->adc2_pattern_len);
        for (int i = 0; i < cfg->adc2_pattern_len; i++) {
            adc_ll_set_pattern_table(ADC_NUM_2, i, cfg->adc2_pattern[i]);
        }
    }
    adc_ll_dig_set_output_format(cfg->format);
    if (cfg->conv_limit_en) {
        adc_ll_dig_set_convert_limit_num(cfg->conv_limit_num);
        adc_ll_dig_convert_limit_enable();
    } else {
        adc_ll_dig_convert_limit_disable();
    }
    adc_ll_dig_set_data_source(ADC_I2S_DATA_SRC_ADC);
}

int adc_hal_convert(adc_ll_num_t adc_n, int channel)
{
    adc_ll_rtc_enable_channel(adc_n, channel);
    adc_ll_rtc_start_convert(adc_n, channel);
    while (adc_ll_rtc_convert_is_done(adc_n) != true);
    return adc_ll_rtc_get_convert_value(adc_n);
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
    Sens_Vp0 = adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_0 );
    Sens_Vn0 = adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_3 );
    adc_ll_hall_phase_enable();
    Sens_Vp1 = adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_0 );
    Sens_Vn1 = adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_3 );
    hall_value = (Sens_Vp1 - Sens_Vp0) - (Sens_Vn1 - Sens_Vn0);
    return hall_value;
}