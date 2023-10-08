/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "driver/adc_types_legacy.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
/**
 * @brief Type of calibration value used in characterization
 */
typedef enum {
    ESP_ADC_CAL_VAL_EFUSE_VREF = 0,         /**< Characterization based on reference voltage stored in eFuse*/
    ESP_ADC_CAL_VAL_EFUSE_TP = 1,           /**< Characterization based on Two Point values stored in eFuse*/
    ESP_ADC_CAL_VAL_DEFAULT_VREF = 2,       /**< Characterization based on default reference voltage*/
    ESP_ADC_CAL_VAL_EFUSE_TP_FIT = 3,       /**< Characterization based on Two Point values and fitting curve coefficients stored in eFuse */
    ESP_ADC_CAL_VAL_MAX,
    ESP_ADC_CAL_VAL_NOT_SUPPORTED = ESP_ADC_CAL_VAL_MAX,
} esp_adc_cal_value_t;

/**
 * @brief Structure storing characteristics of an ADC
 *
 * @note Call esp_adc_cal_characterize() to initialize the structure
 */
typedef struct {
    adc_unit_t adc_num;                     /**< ADC unit*/
    adc_atten_t atten;                      /**< ADC attenuation*/
    adc_bits_width_t bit_width;             /**< ADC bit width */
    uint32_t coeff_a;                       /**< Gradient of ADC-Voltage curve*/
    uint32_t coeff_b;                       /**< Offset of ADC-Voltage curve*/
    uint32_t vref;                          /**< Vref used by lookup table*/
    const uint32_t *low_curve;              /**< Pointer to low Vref curve of lookup table (NULL if unused)*/
    const uint32_t *high_curve;             /**< Pointer to high Vref curve of lookup table (NULL if unused)*/
    uint8_t version;                        /**< ADC Calibration */
} esp_adc_cal_characteristics_t;
#endif  //#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3

#ifdef __cplusplus
}
#endif
