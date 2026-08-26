/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_adc/adc_cali.h"
#include "adc_cali_schemes.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
/*---------------------------------------------------------------
            Curve Fitting Calibration Scheme
---------------------------------------------------------------*/
typedef struct {
    adc_unit_t unit_id;         ///< ADC unit
    adc_channel_t chan;         ///< ADC channel, for chips with SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED, calibration can be per channel
    adc_atten_t atten;          ///< ADC attenuation
    adc_bitwidth_t bitwidth;    ///< ADC raw output bitwidth
} adc_cali_curve_fitting_config_t;

/**
 * @brief Create a Curve Fitting calibration scheme
 *
 * After creating, you'll get a handle to this scheme. Then you can use the driver APIS in `esp_adc/adc_cali.h` to do the
 * ADC calibration via the handle you get.
 *
 * @param[in]  config  Initial configurations
 * @param[out] handle  ADC calibration handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_NO_MEM:        No enough memory
 *        - ESP_ERR_NOT_SUPPORTED: Scheme required eFuse bits not burnt
 */
esp_err_t adc_cali_create_scheme_curve_fitting(const adc_cali_curve_fitting_config_t *config, adc_cali_handle_t *ret_handle);

/**
 * @brief Delete the Curve Fitting calibration scheme handle
 *
 * @param[in] handle ADC calibration handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 */
esp_err_t adc_cali_delete_scheme_curve_fitting(adc_cali_handle_t handle);
#endif  //  #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
/*---------------------------------------------------------------
            Line Fitting Calibration Scheme
---------------------------------------------------------------*/
/**
 * @brief Type of calibration value used in line fitting scheme characterization
 */
typedef enum {
    ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_VREF = 0,        ///< Characterization based on reference voltage stored in eFuse
    ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_TP = 1,          ///< Characterization based on Two Point values stored in eFuse
    ADC_CALI_LINE_FITTING_EFUSE_VAL_DEFAULT_VREF = 2,      ///< Characterization based on default reference voltage
} adc_cali_line_fitting_efuse_val_t;

typedef struct {
    adc_unit_t unit_id;         ///< ADC unit
    adc_atten_t atten;          ///< ADC attenuation
    adc_bitwidth_t bitwidth;    ///< ADC raw output bitwidth
#if CONFIG_IDF_TARGET_ESP32
    /**
     * @brief Default ADC reference voltage in mV.
     *
     * Use this when the ADC calibration value is `ADC_CALI_LINE_FITTING_EFUSE_VAL_DEFAULT_VREF`.
     * If others, driver will use the calibration code burnt in the eFuse for calibration.
     */
    uint32_t default_vref;
#endif
} adc_cali_line_fitting_config_t;

/**
 * @brief Create a Line Fitting calibration scheme
 *
 * After creating, you'll get a handle to this scheme. Then you can use the driver APIS in `esp_adc/adc_cali.h` to do the
 * ADC calibration via the handle you get.
 *
 * @param[in]  config  Initial configurations
 * @param[out] handle  ADC calibration handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_NO_MEM:        No enough memory
 *        - ESP_ERR_NOT_SUPPORTED: Scheme required eFuse bits not burnt
 */
esp_err_t adc_cali_create_scheme_line_fitting(const adc_cali_line_fitting_config_t *config, adc_cali_handle_t *ret_handle);

/**
 * @brief Delete the Line Fitting calibration scheme handle
 *
 * @param[in] handle ADC calibration handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 */
esp_err_t adc_cali_delete_scheme_line_fitting(adc_cali_handle_t handle);

#if CONFIG_IDF_TARGET_ESP32
/**
 * @brief Helper function to quickly check the ADC calibration code burnt on your eFuse
 *
 * @note If `cali_val` equals to `ESP_ADC_CALI_VAL_DEFAULT_VREF`, please set the `default_vref`
 *       when creating this scheme (See `ESP_ADC_CALI_SCHEME_VER_LINE_FITTING_init_t`)
 *
 * @param[out] cali_val See `esp_adc_cali_value_t`
 *
 * @return
 *         - ESP_OK:                On success
 *         - ESP_ERR_INVALID_ARG:   Invalid argument
 *         - ESP_ERR_NOT_SUPPORTED: Scheme required eFuse bits not burnt
 */
esp_err_t adc_cali_scheme_line_fitting_check_efuse(adc_cali_line_fitting_efuse_val_t *cali_val);
#endif  // CONFIG_IDF_TARGET_ESP32
#endif //  #if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED

#ifdef __cplusplus
}
#endif
