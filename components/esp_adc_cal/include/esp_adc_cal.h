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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "driver/adc.h"
#include "esp_adc_cal_constants.h"

/**
 * @brief ADC characterization mode
 */
typedef enum {
    ESP_ADC_CAL_MODE_LIN = 0,               /**< Characterize the ADC as a linear curve*/
    ESP_ADC_CAL_MODE_LUT= 1,                /**< Characterize the ADC using a lookup table*/
} esp_adc_cal_mode_t;

/**
 * @brief Type of calibration value used in characterization
 */
typedef enum {
    ESP_ADC_CAL_VAL_EFUSE_VREF = 0,         /**< Characterization based on reference voltage stored in eFuse*/
    ESP_ADC_CAL_VAL_EFUSE_TP = 1,           /**< Characterization based on Two Point values stored in eFuse*/
    ESP_ADC_CAL_VAL_DEFAULT_VREF = 2,       /**< Characterization based on default reference voltage*/
} esp_adc_cal_value_t;

/**
 * @brief Structure storing Lookup Table
 *
 * A Lookup Table (LUT) of a given ADC and attenuation contains two curves
 * mapping ADC readings to a voltage in mV. Each curve contains 33 equally spaced
 * points separated by a step size of 128. The low_vref_curve represents the ADC
 * voltage curve of a module with a reference voltage of 1000mV, whilst the
 * high_vref_curve represents a reference voltage of 1200mV.
 *
 * @note Separate LUTs are provided for each ADC at each attenuation
 */
typedef struct {
    uint32_t low_vref_curve[33];        /**< Voltage curve at a reference voltage of 1000mV*/
    uint32_t high_vref_curve[33];       /**< Voltage curve at a reference voltage of 1200mV*/
} esp_adc_cal_lookup_table_t;

/**
 * @brief Structure storing characteristics of an ADC
 *
 * @note Call esp_adc_cal_get_characteristics() to initialize the structure
 */
typedef struct {
    esp_adc_cal_mode_t mode;                            /**< Characterization mode*/
    adc_unit_t adc_num;                                 /**< ADC number*/
    union {
        struct {
            uint32_t coeff_a;                           /**< 1st order coefficient of linear characteristics curve*/
            uint32_t coeff_b;                           /**< 0th order coefficient of linear characteristics curve*/
        } linear_chars;
        struct {
            uint32_t vref;                              /**< Reference voltage*/
            const esp_adc_cal_lookup_table_t *table;    /**< Pointer to lookup table*/
        } lut_chars;
    };
} esp_adc_cal_characteristics_t;


/**
 * @brief Checks if ADC calibration values are stored in eFuse
 *
 * This function checks if ADC reference voltage or Two Point calibration voltages
 * have been burned to the eFuse of the current ESP32
 *
 * @param   value_type  Type of calibration value (ESP_ADC_CAL_VAL_EFUSE_VREF or ESP_ADC_CAL_VAL_EFUSE_TP)
 *
 * @return
 *      - ESP_OK: The calibration mode is supported in eFuse
 *      - ESP_ERR_NOT_SUPPORTED: Error, eFuse values are not burned
 *      - ESP_ERR_INVALID_ARG: Error, invalid argument (ESP_ADC_CAL_VAL_DEFAULT_VREF)
 */
esp_err_t esp_adc_cal_check_efuse(esp_adc_cal_value_t value_type);

/**
 * @brief Characterize an ADC at a particular attenuation under Linear or LUT Mode
 *
 * This function will generate the characteristics curve of one of the ADCs at a
 * particular attenuation. This characteristics curve will be stored in a
 * characteristics structure. Linear Mode will be characterize the ADC-Voltage
 * curve as a linear curve. LUT Mode will characterize the ADC-Voltage curve
 * using a lookup table. Calibration values in eFuse will be used to generate
 * the characteristics curve if available, and vref_default will be used if they
 * are not.
 *
 * @note This function will abort if there are no available options for
 *       characterization (characterization modes and calibration value types
 *       can be enabled and disabled in menuconfig)
 *
 * @param[in]   adc_num         ADC to characterize (ADC_UNIT_1 or ADC_UNIT_2)
 * @param[in]   atten           Attenuation to characterize
 * @param[in]   mode            Characterization mode (Linear or LUT)
 * @param[in]   vref_default    Default ADC reference voltage in mV (used if eFuse is not available)
 * @param[out]  chars           Pointer to empty structure used to store ADC characteristics
 *
 * @return
 *      - ESP_ADC_CAL_VAL_EFUSE_VREF: eFuse Vref used for characterization
 *      - ESP_ADC_CAL_VAL_EFUSE_TP: Two Point value used for characterization (only in Linear Mode)
 *      - ESP_ADC_CAL_VAL_DEFAULT_VREF: Default Vref used for characterization
 */
esp_adc_cal_value_t esp_adc_cal_characterize(adc_unit_t adc_num,
                                             adc_atten_t atten,
                                             esp_adc_cal_mode_t mode,
                                             uint32_t vref_default,
                                             esp_adc_cal_characteristics_t *chars);


/**
 * @brief   Convert an ADC reading to voltage in mV
 *
 * This function converts a an ADC reading to a voltage in mV based on the
 * ADC characteristics curve provided.
 *
 * @param[in]   adc_reading     ADC reading
 * @param[in]   bit_width       Bit width of the ADC reading
 * @param[in]   chars           Pointer to initialized structure containing ADC characteristics curve
 *
 * @return      Voltage in mV
 *
 * @note characteristics structure must be initialized first using esp_adc_cal_characterize()
 */
uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, adc_bits_width_t bit_width, const esp_adc_cal_characteristics_t *chars);

/**
 * @brief   Reads an ADC and returns a voltage in mV
 *
 * This function reads an ADC then converts the raw reading to a voltage in mV
 * using the characteristics curve provided. The ADC that is read is also
 * determined by the characteristics.
 *
 * @param[in]   channel     ADC Channel to read
 * @param[in]   bit_width   Bit width of ADC reading (must be same as ADC configuration)
 * @param[in]   chars       Pointer to initialized ADC characteristics structure
 * @param[out]  voltage     Pointer to store converted voltage
 *
 * @return
 *      - ESP_OK: ADC read and converted
 *      - ESP_ERR_TIMEOUT: Error, could not read ADC
 *      - ESP_ERR_INVALID_ARG: Error due to invalid arguments
 *
 * @note    The ADC must be initialized before calling this function. The
 *          bit_width parameter must be the same as the bit width used in
 *          configuration
 *
 * @note    Characteristics structure must be initialized using before calling
 *          this function
 */
esp_err_t adc_to_voltage(adc_channel_t channel, adc_bits_width_t bit_width, const esp_adc_cal_characteristics_t *chars, uint32_t *voltage);

/* -------------------------- Deprecated API ------------------------------- */

/** @cond */    //Doxygen command to hide deprecated function from API Reference
/**
 * @deprecated  ADC1 characterization function. Deprecated in order to accommodate
 *              new characterization functions. Use esp_adc_cal_characterize() instead
 */
void esp_adc_cal_get_characteristics(uint32_t vref, adc_atten_t atten, adc_bits_width_t bit_width, esp_adc_cal_characteristics_t *chars) __attribute__((deprecated));

/*
 * @deprecated  This function reads ADC1 and returns the corrected voltage. This
 *              has been deprecated in order to accommodate ADC2 support and new
 *              ADC calibration methods. Use the new function adc_to_voltage() instead
 */
uint32_t adc1_to_voltage(adc1_channel_t channel, const esp_adc_cal_characteristics_t *chars) __attribute__((deprecated));

/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_ADC_CAL_H__ */
