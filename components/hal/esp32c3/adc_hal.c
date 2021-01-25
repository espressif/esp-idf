// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for ADC (ESP32-C3 specific part)

#include <string.h>
#include "soc/soc_caps.h"
#include "hal/adc_hal.h"
#include "hal/adc_types.h"
#include "soc/soc.h"

//Currently we don't have context for the ADC HAL. So HAL variables are temporarily put here. But
//please don't follow this code. Create a context for your own HAL!

static bool s_filter_enabled[SOC_ADC_DIGI_FILTER_NUM] = {};
static adc_digi_filter_t s_filter[SOC_ADC_DIGI_FILTER_NUM] = {};

static bool s_monitor_enabled[SOC_ADC_DIGI_MONITOR_NUM] = {};
static adc_digi_monitor_t s_monitor_config[SOC_ADC_DIGI_MONITOR_NUM] = {};

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
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
    adc_hal_deinit();
}

void adc_hal_digi_controller_config(const adc_digi_config_t *cfg)
{
    //only one pattern table is supported on C3, but LL still needs one argument.
    const int pattern_both = 0;

    /* If enable digtal controller, adc xpd should always on. */
    adc_ll_set_power_manage(ADC_POWER_SW_ON);
    if (cfg->adc_pattern_len) {
        adc_ll_digi_clear_pattern_table(pattern_both);
        adc_ll_digi_set_pattern_table_len(pattern_both, cfg->adc_pattern_len);
        for (uint32_t i = 0; i < cfg->adc_pattern_len; i++) {
            adc_ll_digi_set_pattern_table(pattern_both, i, cfg->adc_pattern[i]);
        }
    }

    adc_ll_set_controller(pattern_both, ADC_CTRL_DIG);
    if (cfg->conv_limit_en) {
        adc_ll_digi_set_convert_limit_num(cfg->conv_limit_num);
        adc_ll_digi_convert_limit_enable();
    } else {
        adc_ll_digi_convert_limit_disable();
    }

    //clock
    uint32_t interval = APB_CLK_FREQ / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_A_DEFAULT / ADC_LL_CLKM_DIV_B_DEFAULT + 1) / 2 / cfg->sample_freq_hz;
    adc_ll_digi_set_trigger_interval(interval);
    adc_hal_digi_clk_config();
}

void adc_hal_digi_clk_config(void)
{
    //Here we set the clock divider factor to make the digital clock to 5M Hz
    adc_ll_digi_controller_clk_div(ADC_LL_CLKM_DIV_NUM_DEFAULT, ADC_LL_CLKM_DIV_B_DEFAULT, ADC_LL_CLKM_DIV_A_DEFAULT);
    adc_ll_digi_controller_clk_enable(0);
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

static void filter_update(adc_digi_filter_idx_t idx)
{
    //ESP32-C3 has no enable bit, the filter will be enabled when the filter channel is configured
    if (s_filter_enabled[idx]) {
        adc_ll_digi_filter_set_factor(idx, &s_filter[idx]);
    } else {
        adc_ll_digi_filter_disable(idx);
    }
}

/**
 * Set adc digital controller filter factor.
 *
 * @param idx ADC filter unit.
 * @param filter Filter config. Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
void adc_hal_digi_filter_set_factor(adc_digi_filter_idx_t idx, adc_digi_filter_t *filter)
{
    s_filter[idx] = *filter;
    filter_update(idx);
}

/**
 * Get adc digital controller filter factor.
 *
 * @param adc_n ADC unit.
 * @param factor Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
void adc_hal_digi_filter_get_factor(adc_digi_filter_idx_t idx, adc_digi_filter_t *filter)
{
    *filter = s_filter[idx];
}

void adc_hal_digi_filter_enable(adc_digi_filter_idx_t filter_idx, bool enable)
{
    s_filter_enabled[filter_idx] = enable;
    filter_update(filter_idx);
}

static void update_monitor(adc_digi_monitor_idx_t idx)
{
    //ESP32-C3 has no enable bit, the monitor will be enabled when the monitor channel is configured
    if (s_monitor_enabled[idx]) {
        adc_ll_digi_monitor_set_mode(idx, &s_monitor_config[idx]);
    } else {
        adc_ll_digi_monitor_disable(idx);
    }
}

void adc_hal_digi_monitor_config(adc_digi_monitor_idx_t idx, adc_digi_monitor_t *config)
{
    s_monitor_config[idx] = *config;
    update_monitor(idx);
}

void adc_hal_digi_monitor_enable(adc_digi_monitor_idx_t mon_idx, bool enable)
{
    s_monitor_enabled[mon_idx] = enable;
    update_monitor(mon_idx);
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
 * @param config Refer to `adc_arbiter_t`.
 */
void adc_hal_arbiter_config(adc_arbiter_t *config)
{
    adc_ll_set_arbiter_work_mode(config->mode);
    adc_ll_set_arbiter_priority(config->rtc_pri, config->dig_pri, config->pwdet_pri);
}
