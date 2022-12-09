// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "driver/adc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    Digital controller setting
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

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/

/**
 * @brief Read Hall Sensor
 *
 * @note When the power switch of SARADC1, SARADC2, HALL sensor and AMP sensor is turned on,
 *       the input of GPIO36 and GPIO39 will be pulled down for about 80ns.
 *       When enabling power for any of these peripherals, ignore input from GPIO36 and GPIO39.
 *       Please refer to section 3.11 of 'ECO_and_Workarounds_for_Bugs_in_ESP32' for the description of this issue.
 *
 * @note The Hall Sensor uses channels 0 and 3 of ADC1. Do not configure
 * these channels for use as ADC channels.
 *
 * @note The ADC1 module must be enabled by calling
 *       adc1_config_width() before calling hall_sensor_read(). ADC1
 *       should be configured for 12 bit readings, as the hall sensor
 *       readings are low values and do not cover the full range of the
 *       ADC.
 *
 * @return The hall sensor reading.
 */
int hall_sensor_read(void);

#ifdef __cplusplus
}
#endif