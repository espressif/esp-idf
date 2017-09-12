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
#include "driver/adc.h"

/** @cond */
#define ADC_CAL_GAIN_SCALE    16
#define ADC_CAL_OFFSET_SCALE  10

#define ADC_CAL_IDEAL_V_REF 1100 //In mV
#define ADC_CAL_LOW_V_REF 1000
#define ADC_CAL_HIGH_V_REF 1200
#define ADC_CAL_MIN 0
#define ADC_CAL_MAX 4095
/** @endcond */

/**
 * @brief Structure storing Lookup Table
 *
 * The Lookup Tables (LUT) of a given attenuation contains 33 equally spaced
 * points. The Gain and Offset curves are used to find the appopriate gain and
 * offset factor given a reference voltage v_ref.
 *
 * @note A seperate LUT is provided for each attenuation and are defined in
 * esp_adc_cal_lookup_tables.c
 */
typedef struct {
    uint32_t gain_m;       /**<Gradient of Gain Curve */
    uint32_t gain_c;       /**<Offset of Gain Curve */
    uint32_t offset_m;      /**<Gradient of Offset Curve */
    uint32_t offset_c;      /**<Offset of Offset Curve */
    uint32_t bit_shift;     /**<Bit shift used find corresponding LUT points
                                given an ADC reading*/
    uint32_t voltage[];     /**<Array of voltages in mV representing the
                                ADC-Voltage curve */
} esp_adc_cal_lookup_table_t;

/**
 * @brief Structure storing ADC characteristics of given v_ref
 *
 * The ADC Characteristics structure stores the gain and offset factors of an
 * ESP32 module's ADC. These factors are calculated using the reference voltage,
 * and the Gain and Offset curves provided in the lookup tables.
 *
 * @note Call esp_adc_cal_get_characteristics() to initialize the structure
 *
 */
typedef struct {
    uint32_t v_ref;         /**<Reference Voltage of current ESP32 Module in mV*/
    uint32_t gain;          /**<Scaling factor used to correct LUT voltages to
                                current v_ref. Bit shifted by << ADC_CAL_GAIN_SCALE
                                for uint32 arithmetic */
    uint32_t offset;        /**<Offset in mV used to correct LUT Voltages to current v_ref */
    uint32_t ideal_offset;  /**<Offset in mV at the ideal reference voltage */
    adc_bits_width_t bit_width;     /**<Bit width of ADC e.g. ADC_WIDTH_12Bit */
    const esp_adc_cal_lookup_table_t *table;    /**<Pointer to LUT */
} esp_adc_cal_characteristics_t;

extern const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_0;  /**<LUT for atten0 */
extern const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_1;  /**<LUT for atten1 */
extern const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_2;  /**<LUT for atten2 */
extern const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_3;  /**<LUT for atten3 */

/**
 * @brief Calculate characteristics of ADC
 *
 * This function will calculate the gain and offset factors based on the
 * reference voltage parameter and the Gain and Offset curve provided in the LUT.
 *
 * @note reference voltage of the ADCs can be routed to GPIO using
 * adc2_vref_to_gpio() from the ADC driver
 *
 * @note The LUT members have been bit shifted by ADC_CAL_GAIN_SCALE or
 * ADC_CAL_OFFSET_SCALE to make them uint32_t compatible. This bit shifting will
 * accounted for in this function
 *
 * @param[in]   v_ref       true reference voltage of the ADC in mV (1000 to 1200mV). Nominal
 *                          value for reference voltage is 1100mV.
 * @param[in]   atten       attenuation setting used to select the corresponding lookup table
 * @param[in]   bit_width   bit width of ADC
 * @param[out]  chars       pointer to structure used to store ADC characteristics of module
 */
void esp_adc_cal_get_characteristics(uint32_t v_ref,
                                     adc_atten_t atten,
                                     adc_bits_width_t bit_width,
                                     esp_adc_cal_characteristics_t *chars);

/**
 * @brief   Convert raw ADC reading to voltage in mV
 *
 * This function converts a raw ADC reading to a voltage in mV. This conversion
 * is based on the ADC's characteristics. The raw ADC reading is referenced
 * against the LUT (pointed to inside characteristics struct) to obtain a voltage.
 * Gain and offset factors are then applied to the voltage in order to obtain
 * the final result.
 *
 * @param[in]   adc     ADC reading (different bit widths will be handled)
 * @param[in]   chars   pointer to structure containing ADC characteristics of
 *                      the module. Structure also contains pointer to the
 *                      corresponding LUT
 *
 * @return              Calculated voltage in mV
 *
 * @note characteristics structure must be initialized using
 * esp_adc_cal_get_characteristics() before this function is used
 */
uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc,
                                    const esp_adc_cal_characteristics_t *chars);

/**
 * @brief   Reads ADC1 and returns voltage in mV
 *
 * This function reads the ADC1 using adc1_get_raw() to obtain a raw ADC
 * reading. The reading is then converted into a voltage value using
 * esp_adc_cal_raw_to_voltage().
 *
 * @param[in]   channel     Channel of ADC1 to measure
 * @param[in]   chars       Pointer to ADC characteristics struct
 *
 * @return      voltage     Calculated voltage in mV
 *
 * @note    ADC must be initialized using adc1_config_width() and
 *          adc1_config_channel_atten() before this function is used
 *
 * @note    characteristics structure must be initialized using
 *          esp_adc_cal_get_characteristics() before this function is used
 */
uint32_t adc1_to_voltage(adc1_channel_t channel, const esp_adc_cal_characteristics_t *chars);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_ADC_CAL_H__ */
