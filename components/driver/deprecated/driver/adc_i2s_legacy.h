/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"
#include "esp_err.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
/**
 * @brief ESP32 ADC DMA source selection.
 */
typedef enum {
    ADC_I2S_DATA_SRC_IO_SIG = 0, /*!< I2S data from GPIO matrix signal  */
    ADC_I2S_DATA_SRC_ADC = 1,    /*!< I2S data from ADC */
} adc_i2s_source_t;
#endif

#if CONFIG_IDF_TARGET_ESP32
/*---------------------------------------------------------------
            ESP32 Deprecated API
---------------------------------------------------------------*/
/**
 * @brief Set I2S data source
 * @param src I2S DMA data source, I2S DMA can get data from digital signals or from ADC.
 * @return
 *     - ESP_OK success
 */
esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src);

/**
 * @brief Initialize I2S ADC mode
 * @param adc_unit ADC unit index
 * @param channel ADC channel index
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel);
#endif

#ifdef __cplusplus
}
#endif
