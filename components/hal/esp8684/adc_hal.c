/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for ADC (ESP-8684 specific part)

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
}

/**
 * - Set ADC digital controller clock division factor. The clock is divided from `APLL` or `APB` clock.
 *   Expression: controller_clk = APLL/APB * (div_num  + div_a / div_b + 1).
 * - Enable clock and select clock source for ADC digital controller.
 */
static void adc_hal_digi_clk_config(void)
{
    //Here we set the clock divider factor to make the digital clock to 5M Hz
    adc_ll_digi_controller_clk_div(ADC_LL_CLKM_DIV_NUM_DEFAULT, ADC_LL_CLKM_DIV_B_DEFAULT, ADC_LL_CLKM_DIV_A_DEFAULT);
    adc_ll_digi_controller_clk_enable(0);
}

void adc_hal_digi_controller_config(const adc_digi_config_t *cfg)
{
    //only one pattern table is supported on C3, but LL still needs one argument.
    const int pattern_both = 0;

    if (cfg->adc_pattern_len) {
        adc_ll_digi_clear_pattern_table(pattern_both);
        adc_ll_digi_set_pattern_table_len(pattern_both, cfg->adc_pattern_len);
        for (uint32_t i = 0; i < cfg->adc_pattern_len; i++) {
            adc_ll_digi_set_pattern_table(pattern_both, i, cfg->adc_pattern[i]);
        }
    }

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

static void filter_update(adc_digi_filter_idx_t idx)
{
    //ESP8684 has no enable bit, the filter will be enabled when the filter channel is configured
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
    //ESP8684 has no enable bit, the monitor will be enabled when the monitor channel is configured
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
