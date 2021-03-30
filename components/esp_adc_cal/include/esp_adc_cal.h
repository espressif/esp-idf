// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_ADC_CAL_H__
#define __ESP_ADC_CAL_H__

#include <stdint.h>
#include "esp_err.h"
#include "driver/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of calibration value used in characterization
 */
typedef enum {
    ESP_ADC_CAL_VAL_EFUSE_VREF = 0,         /**< Characterization based on reference voltage stored in eFuse*/
    ESP_ADC_CAL_VAL_EFUSE_TP = 1,           /**< Characterization based on Two Point values stored in eFuse*/
    ESP_ADC_CAL_VAL_DEFAULT_VREF = 2,       /**< Characterization based on default reference voltage*/
    ESP_ADC_CAL_VAL_MAX,
    ESP_ADC_CAL_VAL_NOT_SUPPORTED = ESP_ADC_CAL_VAL_MAX,
} esp_adc_cal_value_t;

/**
 * @brief Structure storing characteristics of an ADC
 *
 * @note Call esp_adc_cal_characterize() to initialize the structure
 */
typedef struct {
    adc_unit_t adc_num;                     /**< ADC number*/
    adc_atten_t atten;                      /**< ADC attenuation*/
    adc_bits_width_t bit_width;             /**< ADC bit width */
    uint32_t coeff_a;                       /**< Gradient of ADC-Voltage curve*/
    uint32_t coeff_b;                       /**< Offset of ADC-Voltage curve*/
    uint32_t vref;                          /**< Vref used by lookup table*/
    const uint32_t *low_curve;              /**< Pointer to low Vref curve of lookup table (NULL if unused)*/
    const uint32_t *high_curve;             /**< Pointer to high Vref curve of lookup table (NULL if unused)*/
} esp_adc_cal_characteristics_t;

/**
 * @brief Checks if ADC calibration values are burned into eFuse
 *
 * This function checks if ADC reference voltage or Two Point values have been
 * burned to the eFuse of the current ESP32
 *
 * @param   value_type  Type of calibration value (ESP_ADC_CAL_VAL_EFUSE_VREF or ESP_ADC_CAL_VAL_EFUSE_TP)
 * @note in ESP32S2, only ESP_ADC_CAL_VAL_EFUSE_TP is supported. Some old ESP32S2s do not support this, either.
 * In which case you have to calibrate it manually, possibly by performing your own two-point calibration on the chip.
 *
 * @return
 *      - ESP_OK: The calibration mode is supported in eFuse
 *      - ESP_ERR_NOT_SUPPORTED: Error, eFuse values are not burned
 *      - ESP_ERR_INVALID_ARG: Error, invalid argument (ESP_ADC_CAL_VAL_DEFAULT_VREF)
 */
esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t value_type);

/**
 * @brief Characterize an ADC at a particular attenuation
 *
 * This function will characterize the ADC at a particular attenuation and generate
 * the ADC-Voltage curve in the form of [y = coeff_a * x + coeff_b].
 * Characterization can be based on Two Point values, eFuse Vref, or default Vref
 * and the calibration values will be prioritized in that order.
 *
 * @note
 * For ESP32, Two Point values and eFuse Vref calibration can be enabled/disabled using menuconfig.
 * For ESP32s2, only Two Point values calibration and only ADC_WIDTH_BIT_13 is supported. The parameter default_vref is unused.
 *
 *
 * @param[in]   adc_num         ADC to characterize (ADC_UNIT_1 or ADC_UNIT_2)
 * @param[in]   atten           Attenuation to characterize
 * @param[in]   bit_width       Bit width configuration of ADC
 * @param[in]   default_vref    Default ADC reference voltage in mV (Only in ESP32, used if eFuse values is not available)
 * @param[out]  chars           Pointer to empty structure used to store ADC characteristics
 *
 * @return
 *      - ESP_ADC_CAL_VAL_EFUSE_VREF: eFuse Vref used for characterization
 *      - ESP_ADC_CAL_VAL_EFUSE_TP: Two Point value used for characterization (only in Linear Mode)
 *      - ESP_ADC_CAL_VAL_DEFAULT_VREF: Default Vref used for characterization
 */
esp_adc_cal_value_t esp_adc_cal_characterize(adc_unit_t adc_num,
                                             adc_atten_t atten,
                                             adc_bits_width_t bit_width,
                                             uint32_t default_vref,
                                             esp_adc_cal_characteristics_t *chars);

/**
 * @brief   Convert an ADC reading to voltage in mV
 *
 * This function converts an ADC reading to a voltage in mV based on the ADC's
 * characteristics.
 *
 * @note    Characteristics structure must be initialized before this function
 *          is called (call esp_adc_cal_characterize())
 *
 * @param[in]   adc_reading     ADC reading
 * @param[in]   chars           Pointer to initialized structure containing ADC characteristics
 *
 * @return      Voltage in mV
 */
uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t *chars);

/**
 * @brief   Reads an ADC and converts the reading to a voltage in mV
 *
 * This function reads an ADC then converts the raw reading to a voltage in mV
 * based on the characteristics provided. The ADC that is read is also
 * determined by the characteristics.
 *
 * @note    The Characteristics structure must be initialized before this
 *          function is called (call esp_adc_cal_characterize())
 *
 * @param[in]   channel     ADC Channel to read
 * @param[in]   chars       Pointer to initialized ADC characteristics structure
 * @param[out]  voltage     Pointer to store converted voltage
 *
 * @return
 *      - ESP_OK: ADC read and converted to mV
 *      - ESP_ERR_TIMEOUT: Error, timed out attempting to read ADC
 *      - ESP_ERR_INVALID_ARG: Error due to invalid arguments
 */
esp_err_t esp_adc_cal_get_voltage(adc_channel_t channel, const esp_adc_cal_characteristics_t *chars, uint32_t *voltage);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_ADC_CAL_H__ */
