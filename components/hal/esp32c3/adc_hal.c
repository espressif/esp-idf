/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
