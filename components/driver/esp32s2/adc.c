/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_log.h"
#include "sys/lock.h"
#include "freertos/FreeRTOS.h"
#include "hal/adc_types.h"
#include "hal/adc_ll.h"

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

/**
 * Config monitor of adc digital controller.
 *
 * @note The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
 * @param adc_n ADC unit.
 * @param config Refer to ``adc_digi_monitor_t``.
 */
static void adc_digi_monitor_config(adc_ll_num_t adc_n, adc_digi_monitor_t *config)
{
    adc_ll_digi_monitor_set_mode(adc_n, config->mode);
    adc_ll_digi_monitor_set_thres(adc_n, config->threshold);
}

/*************************************/
/* Digital controller filter setting */
/*************************************/

esp_err_t adc_digi_filter_reset(adc_digi_filter_idx_t idx)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        adc_ll_digi_filter_reset(ADC_NUM_1);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        adc_ll_digi_filter_reset(ADC_NUM_2);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_set_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        adc_ll_digi_filter_set_factor(ADC_NUM_1, config->mode);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        adc_ll_digi_filter_set_factor(ADC_NUM_2, config->mode);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_get_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        config->adc_unit = ADC_UNIT_1;
        config->channel = ADC_CHANNEL_MAX;
        adc_ll_digi_filter_get_factor(ADC_NUM_1, &config->mode);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        config->adc_unit = ADC_UNIT_2;
        config->channel = ADC_CHANNEL_MAX;
        adc_ll_digi_filter_get_factor(ADC_NUM_2, &config->mode);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_filter_enable(adc_digi_filter_idx_t idx, bool enable)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_FILTER_IDX0) {
        adc_ll_digi_filter_enable(ADC_NUM_1, enable);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        adc_ll_digi_filter_enable(ADC_NUM_2, enable);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * @brief Get the filtered data of adc digital controller filter. For debug.
 *        The data after each measurement and filtering is updated to the DMA by the digital controller. But it can also be obtained manually through this API.
 *
 * @note For ESP32S2, The filter will filter all the enabled channel data of the each ADC unit at the same time.
 * @param idx Filter index.
 * @return Filtered data. if <0, the read data invalid.
 */
int adc_digi_filter_read_data(adc_digi_filter_idx_t idx)
{
    if (idx == ADC_DIGI_FILTER_IDX0) {
        return adc_ll_digi_filter_read_data(ADC_NUM_1);
    } else if (idx == ADC_DIGI_FILTER_IDX1) {
        return adc_ll_digi_filter_read_data(ADC_NUM_2);
    } else {
        return -1;
    }
}

/**************************************/
/* Digital controller monitor setting */
/**************************************/

esp_err_t adc_digi_monitor_set_config(adc_digi_monitor_idx_t idx, adc_digi_monitor_t *config)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_MONITOR_IDX0) {
        adc_digi_monitor_config(ADC_NUM_1, config);
    } else if (idx == ADC_DIGI_MONITOR_IDX1) {
        adc_digi_monitor_config(ADC_NUM_2, config);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t adc_digi_monitor_enable(adc_digi_monitor_idx_t idx, bool enable)
{
    ADC_ENTER_CRITICAL();
    if (idx == ADC_DIGI_MONITOR_IDX0) {
        adc_ll_digi_monitor_enable(ADC_NUM_1, enable);
    } else if (idx == ADC_DIGI_MONITOR_IDX1) {
        adc_ll_digi_monitor_enable(ADC_NUM_2, enable);
    }
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}
