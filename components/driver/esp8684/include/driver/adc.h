/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/adc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/*************************************/
/* Digital controller filter setting */
/*************************************/
/**
 * @brief Reset adc digital controller filter.
 *
 * @param idx Filter index.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_filter_reset(adc_digi_filter_idx_t idx);

/**
 * @brief Set adc digital controller filter configuration.
 *
 * @param idx Filter index.
 * @param config See ``adc_digi_filter_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_filter_set_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config);

/**
 * @brief Get adc digital controller filter configuration.
 *
 * @param idx Filter index.
 * @param config See ``adc_digi_filter_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_filter_get_config(adc_digi_filter_idx_t idx, adc_digi_filter_t *config);

/**
 * @brief Enable/disable adc digital controller filter.
 *        Filtering the ADC data to obtain smooth data at higher sampling rates.
 *
 * @param idx Filter index.
 * @param enable Enable/Disable filter.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_filter_enable(adc_digi_filter_idx_t idx, bool enable);

/**************************************/
/* Digital controller monitor setting */
/**************************************/

/**
 * @brief Config monitor of adc digital controller.
 *
 * @param idx Monitor index.
 * @param config See ``adc_digi_monitor_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_monitor_set_config(adc_digi_monitor_idx_t idx, adc_digi_monitor_t *config);

/**
 * @brief Enable/disable monitor of adc digital controller.
 *
 * @param idx Monitor index.
 * @param enable True or false enable monitor.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_monitor_enable(adc_digi_monitor_idx_t idx, bool enable);

#ifdef __cplusplus
}
#endif
