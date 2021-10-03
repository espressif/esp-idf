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

/**
 * @brief Config ADC module arbiter.
 *        The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 *        the low priority controller will read the invalid ADC2 data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note Default priority: Wi-Fi > RTC > Digital;
 * @note In normal use, there is no need to call this interface to config arbiter.
 *
 * @param adc_unit ADC unit.
 * @param config Refer to `adc_arbiter_t`.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_NOT_SUPPORTED ADC unit not support arbiter.
 */
esp_err_t adc_arbiter_config(adc_unit_t adc_unit, adc_arbiter_t *config);

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * @brief Enable digital controller to trigger the measurement.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_start(void);

/**
 * @brief Disable digital controller to trigger the measurement.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_stop(void);

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
 * @note For ESP32S2, Filter IDX0/IDX1 can only be used to filter all enabled channels of ADC1/ADC2 unit at the same time.
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
 * @note For ESP32S2, Filter IDX0/IDX1 can only be used to filter all enabled channels of ADC1/ADC2 unit at the same time.
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
 * @note For ESP32S2, Filter IDX0/IDX1 can only be used to filter all enabled channels of ADC1/ADC2 unit at the same time.
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
 * @note For ESP32S2, The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
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
 * @note For ESP32S2, The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
 *
 * @param idx Monitor index.
 * @param enable True or false enable monitor.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_monitor_enable(adc_digi_monitor_idx_t idx, bool enable);

/**************************************/
/*   Digital controller intr setting  */
/**************************************/

/**
 * @brief Enable interrupt of adc digital controller by bitmask.
 *
 * @param adc_unit ADC unit.
 * @param intr_mask Interrupt bitmask. See ``adc_digi_intr_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_intr_enable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask);

/**
 * @brief Disable interrupt of adc digital controller by bitmask.
 *
 * @param adc_unit ADC unit.
 * @param intr_mask Interrupt bitmask. See ``adc_digi_intr_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_intr_disable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask);

/**
 * @brief Clear interrupt of adc digital controller by bitmask.
 *
 * @param adc_unit ADC unit.
 * @param intr_mask Interrupt bitmask. See ``adc_digi_intr_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_intr_clear(adc_unit_t adc_unit, adc_digi_intr_t intr_mask);

/**
 * @brief Get interrupt status mask of adc digital controller.
 *
 * @param adc_unit ADC unit.
 * @return
 *     - intr Interrupt bitmask, See ``adc_digi_intr_t``.
 */
uint32_t adc_digi_intr_get_status(adc_unit_t adc_unit);

/**
 * @brief Register ADC interrupt handler, the handler is an ISR.
 *        The handler will be attached to the same CPU core that this function is running on.
 *
 * @param fn Interrupt handler function.
 * @param arg Parameter for handler function
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_NOT_FOUND Can not find the interrupt that matches the flags.
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t adc_digi_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags);

/**
 * @brief Deregister ADC interrupt handler, the handler is an ISR.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG hander error.
 *     - ESP_FAIL ISR not be registered.
 */
esp_err_t adc_digi_isr_deregister(void);

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/

/*---------------------------------------------------------------
                    Deprecated API
---------------------------------------------------------------*/
/**
 * @brief Set I2S data source
 *
 * @param src I2S DMA data source, I2S DMA can get data from digital signals or from ADC.
 *
 * @deprecated The ESP32S2 don't use I2S DMA. Call ``adc_digi_controller_config`` instead.
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src) __attribute__((deprecated));

/**
 * @brief Initialize I2S ADC mode
 *
 * @param adc_unit ADC unit index
 * @param channel ADC channel index
 *
 * @deprecated The ESP32S2 don't use I2S DMA. Call ``adc_digi_controller_config`` instead.
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel) __attribute__((deprecated));

#ifdef __cplusplus
}
#endif
