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

// The HAL layer for ADC (esp32s2 specific part)

#include "sdkconfig.h"
#include "hal/adc_hal.h"
#include "hal/adc_types.h"

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

void adc_hal_digi_init(void)
{
    adc_hal_init();
    adc_ll_digi_set_clk_div(SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT);
}

void adc_hal_digi_deinit(void)
{
    adc_ll_digi_trigger_disable();   // boss
    adc_ll_digi_dma_disable();
    adc_ll_digi_clear_pattern_table(ADC_NUM_1);
    adc_ll_digi_clear_pattern_table(ADC_NUM_2);
    adc_ll_digi_filter_reset(ADC_NUM_1);
    adc_ll_digi_filter_reset(ADC_NUM_2);
    adc_ll_digi_reset();
    adc_ll_digi_controller_clk_disable();
}

static inline void adc_set_init_code(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    uint32_t cal_val = adc_hal_calibration(adc_n, channel, atten, true, false);
    adc_hal_set_calibration_param(adc_n, cal_val);
}

void adc_hal_digi_controller_config(const adc_digi_config_t *cfg)
{
    /* Single channel mode or multi channel mode. */
    adc_ll_digi_set_convert_mode(cfg->conv_mode);
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_1) {
        if (cfg->adc1_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_1);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_1, cfg->adc1_pattern_len);
            for (int i = 0; i < cfg->adc1_pattern_len; i++) {
                adc_ll_digi_set_pattern_table(ADC_NUM_1, i, cfg->adc1_pattern[i]);
                adc_set_init_code(ADC_NUM_1, cfg->adc1_pattern[i].channel, cfg->adc1_pattern[i].atten);
            }
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        if (cfg->adc2_pattern_len) {
            adc_ll_digi_clear_pattern_table(ADC_NUM_2);
            adc_ll_digi_set_pattern_table_len(ADC_NUM_2, cfg->adc2_pattern_len);
            for (int i = 0; i < cfg->adc2_pattern_len; i++) {
                adc_ll_digi_set_pattern_table(ADC_NUM_2, i, cfg->adc2_pattern[i]);
                adc_set_init_code(ADC_NUM_2, cfg->adc2_pattern[i].channel, cfg->adc2_pattern[i].atten);
            }
        }
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_1) {
        adc_ll_set_controller(ADC_NUM_1, ADC_CTRL_DIG);
    }
    if (cfg->conv_mode & ADC_CONV_SINGLE_UNIT_2) {
        adc_ll_set_controller(ADC_NUM_2, ADC_CTRL_DIG);
    }
    adc_ll_digi_set_output_format(cfg->format);
    if (cfg->conv_limit_en) {
        adc_ll_digi_set_convert_limit_num(cfg->conv_limit_num);
        adc_ll_digi_convert_limit_enable();
    } else {
        adc_ll_digi_convert_limit_disable();
    }

    adc_ll_digi_set_trigger_interval(cfg->interval);
    adc_hal_digi_clk_config(&cfg->dig_clk);
    adc_ll_digi_dma_set_eof_num(cfg->dma_eof_num);
}

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Enable clock and select clock source for ADC digital controller.
 * Expression: controller_clk = (`APLL` or `APB`) / (div_num + div_a / div_b + 1).
 *
 * @note ADC and DAC digital controller share the same frequency divider.
 *       Please set a reasonable frequency division factor to meet the sampling frequency of the ADC and the output frequency of the DAC.
 *
 * @param clk Refer to ``adc_digi_clk_t``.
 */
void adc_hal_digi_clk_config(const adc_digi_clk_t *clk)
{
    adc_ll_digi_controller_clk_div(clk->div_num, clk->div_b, clk->div_a);
    adc_ll_digi_controller_clk_enable(clk->use_apll);
}

/**
 * Enable digital controller to trigger the measurement.
 */
void adc_hal_digi_enable(void)
{
    adc_ll_digi_dma_enable();
    adc_ll_digi_trigger_enable();
}

/**
 * Disable digital controller to trigger the measurement.
 */
void adc_hal_digi_disable(void)
{
    adc_ll_digi_trigger_disable();
    adc_ll_digi_dma_disable();
}

/**
 * Config monitor of adc digital controller.
 *
 * @note The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
 * @param adc_n ADC unit.
 * @param config Refer to ``adc_digi_monitor_t``.
 */
void adc_hal_digi_monitor_config(adc_ll_num_t adc_n, adc_digi_monitor_t *config)
{
    adc_ll_digi_monitor_set_mode(adc_n, config->mode);
    adc_ll_digi_monitor_set_thres(adc_n, config->threshold);
}

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

/**
 * Config ADC2 module arbiter.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC2 data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 * @note Default priority: Wi-Fi > RTC > Digital;
 *
 * @param config Refer to ``adc_arbiter_t``.
 */
void adc_hal_arbiter_config(adc_arbiter_t *config)
{
    adc_ll_set_arbiter_work_mode(config->mode);
    adc_ll_set_arbiter_priority(config->rtc_pri, config->dig_pri, config->pwdet_pri);
}

/*---------------------------------------------------------------
                    ADC calibration setting
---------------------------------------------------------------*/

#define ADC_HAL_CAL_OFFSET_RANGE (4096)
#define ADC_HAL_CAL_TIMES        (10)

static uint16_t s_adc_cali_param[ADC_NUM_MAX][ADC_ATTEN_MAX] = { {0}, {0} };

static uint32_t adc_hal_read_self_cal(adc_ll_num_t adc_n, int channel)
{
    adc_ll_rtc_start_convert(adc_n, channel);
    while (adc_ll_rtc_convert_is_done(adc_n) != true);
    return (uint32_t)adc_ll_rtc_get_convert_value(adc_n);
}

uint32_t adc_hal_calibration(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten, bool internal_gnd, bool force_cal)
{
#ifdef CONFIG_IDF_ENV_FPGA
    return 0;
#endif

    if (!force_cal) {
        if (s_adc_cali_param[adc_n][atten]) {
            return (uint32_t)s_adc_cali_param[adc_n][atten];
        }
    }

    uint32_t code_list[ADC_HAL_CAL_TIMES] = {0};
    uint32_t code_sum = 0;
    uint32_t code_h = 0;
    uint32_t code_l = 0;
    uint32_t chk_code = 0;
    uint32_t dout = 0;

    adc_hal_set_power_manage(ADC_POWER_SW_ON);
    if (adc_n == ADC_NUM_2) {
        adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
        adc_hal_arbiter_config(&config);
    }
    adc_hal_set_controller(adc_n, ADC_CTRL_RTC);    //Set controller

    // adc_hal_arbiter_config(adc_arbiter_t *config)
    adc_ll_calibration_prepare(adc_n, channel, internal_gnd);

    /* Enable/disable internal connect GND (for calibration). */
    if (internal_gnd) {
        adc_ll_rtc_disable_channel(adc_n, channel);
        adc_ll_set_atten(adc_n, 0, atten);  // Note: when disable all channel, HW auto select channel0 atten param.
    } else {
        adc_ll_rtc_enable_channel(adc_n, channel);
        adc_ll_set_atten(adc_n, channel, atten);
    }

    for (uint8_t rpt = 0 ; rpt < ADC_HAL_CAL_TIMES ; rpt ++) {
        code_h = ADC_HAL_CAL_OFFSET_RANGE;
        code_l = 0;
        chk_code = (code_h + code_l) / 2;
        adc_ll_set_calibration_param(adc_n, chk_code);
        dout = adc_hal_read_self_cal(adc_n, channel);
        while (code_h - code_l > 1) {
            if (dout == 0) {
                code_h = chk_code;
            } else {
                code_l = chk_code;
            }
            chk_code = (code_h + code_l) / 2;
            adc_ll_set_calibration_param(adc_n, chk_code);
            dout = adc_hal_read_self_cal(adc_n, channel);
            if ((code_h - code_l == 1)) {
                chk_code += 1;
                adc_ll_set_calibration_param(adc_n, chk_code);
                dout = adc_hal_read_self_cal(adc_n, channel);
            }
        }
        code_list[rpt] = chk_code;
        code_sum += chk_code;
    }
    code_l = code_list[0];
    code_h = code_list[0];
    for (uint8_t i = 0 ; i < ADC_HAL_CAL_TIMES ; i++) {
        if (code_l > code_list[i]) {
            code_l = code_list[i];
        }
        if (code_h < code_list[i]) {
            code_h = code_list[i];
        }
    }
    chk_code = code_h + code_l;
    dout = ((code_sum - chk_code) % (ADC_HAL_CAL_TIMES - 2) < 4)
           ? (code_sum - chk_code) / (ADC_HAL_CAL_TIMES - 2)
           : (code_sum - chk_code) / (ADC_HAL_CAL_TIMES - 2) + 1;

    adc_ll_set_calibration_param(adc_n, dout);
    adc_ll_calibration_finish(adc_n);
    s_adc_cali_param[adc_n][atten] = (uint16_t)dout;
    return dout;
}