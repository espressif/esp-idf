/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * ADC is shared by multiple components, including:
 * - esp_phy
 * - esp_wifi
 * - driver
 *
 * However, usages of above components are different.
 * Therefore, we put the common used parts into `esp_hw_support`, including:
 * - adc power maintenance
 * - adc hw calibration settings
 * - adc locks, to prevent concurrently using adc hw
 */

#pragma once
#include "esp_err.h"
#include "hal/adc_types.h"
#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
            ADC Hardware Calibration
---------------------------------------------------------------*/
/**
 * @brief Calculate the ADC HW calibration code. (Based on the pre-stored efuse or actual calibration)
 *
 * @param adc_n ADC unit to calibrate
 * @param atten Attenuation to use
 */
void adc_calc_hw_calibration_code(adc_unit_t adc_n, adc_atten_t atten);

/**
 * @brief Set the ADC HW calibration code.
 *
 * @param adc_n ADC unit to calibrate
 * @param atten Attenuation to use
 */
void adc_set_hw_calibration_code(adc_unit_t adc_n, adc_atten_t atten);

#if SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED
/**
 * @brief Load the channel compensation of the ADC HW calibration from eFuse to a static array
 *
 * @param adc_n ADC unit to compensation
 * @param chan  ADC channel to compensation
 * @param atten Attenuation to use
 */
void adc_load_hw_calibration_chan_compens(adc_unit_t adc_n, adc_channel_t chan, adc_atten_t atten);

/**
 * @brief Get the channel compensation of the ADC HW calibration from the static array
 *        that have been loaded from eFuse
 *
 * @param adc_n ADC unit to compensation
 * @param chan  ADC channel to compensation
 * @param atten Attenuation to use
 * @return The channel compensation
 */
int adc_get_hw_calibration_chan_compens(adc_unit_t adc_n, adc_channel_t chan, adc_atten_t atten);
#endif  // SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED
#endif //#if SOC_ADC_CALIBRATION_V1_SUPPORTED


/*---------------------------------------------------------------
            ADC Cross Peripheral Locks
---------------------------------------------------------------*/
/**
 * @brief Acquire ADC lock by unit
 *
 * The lock acquiring sequence will be: ADC1, ADC2, ...
 *
 * @note If any of the locks are taken, this API will wait until the lock is successfully acquired.
 *
 * @param[in] adc_unit    ADC unit ID
 *
 * @return
 *        - ESP_OK: On success
 */
esp_err_t adc_lock_acquire(adc_unit_t adc_unit);

/**
 * @brief Release ADC lock by unit
 *
 * The lock releasing sequence will be: ..., ADC2, ADC1
 *
 * @param[in] adc_unit    ADC unit ID
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_STATE: The lock(s) isn't acquired yet
 */
esp_err_t adc_lock_release(adc_unit_t adc_unit);

/**
 * @brief Try to acquire ADC lock by unit
 *
 * The lock acquiring sequence will be: ADC1, ADC2, ...
 *
 * @note If any of the locks are taken, this API will return immediately with an error `ESP_ERR_TIMEOUT`
 *
 * @param[in] adc_unit    ADC unit ID
 *
 * @return
 *        - ESP_OK:          On success
 *        - ESP_ERR_TIMEOUT: Lock(s) is taken already
 */
esp_err_t adc_lock_try_acquire(adc_unit_t adc_unit);

/**
 * @brief For WIFI module to claim the usage of ADC2.
 *
 * Other tasks will be forbidden to use ADC2 between ``adc2_wifi_acquire`` and ``adc2_wifi_release``.
 * The WIFI module may have to wait for a short time for the current conversion (if exist) to finish.
 *
 * @return
 *        - ESP_OK success
 *        - ESP_ERR_TIMEOUT reserved for future use. Currently the function will wait until success.
 */
esp_err_t adc2_wifi_acquire(void);

/**
 * @brief For WIFI module to let other tasks use the ADC2 when WIFI is not work.
 *
 * Other tasks will be forbidden to use ADC2 between ``adc2_wifi_acquire`` and ``adc2_wifi_release``.
 * Call this function to release the occupation of ADC2 by WIFI.
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_STATE: The lock(s) isn't acquired yet
 */
esp_err_t adc2_wifi_release(void);


#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
/**
 * @brief This API help ADC2 calibration constructor be linked.
 *
 * @note  This is a private function, Don't call `adc2_cal_include` in user code.
 */
void adc2_cal_include(void);
#else
/**
 * @brief There's no calibration involved on this chip.
 *
 * @note  This is a private function, Don't call `adc2_cal_include` in user code.
 */
#define adc2_cal_include()
#endif //CONFIG_IDF_TARGET_*

/*------------------------------------------------------------------------------
* For those who use APB_SARADC periph
*----------------------------------------------------------------------------*/
/**
 * @brief Claim the usage of the APB_SARADC periph
 *
 * Reference count inside
 */
void adc_apb_periph_claim(void);

/**
 * @brief Free the usage of the APB_SARADC periph
 *
 * Reference count inside
 */
void adc_apb_periph_free(void);


#ifdef __cplusplus
}
#endif
