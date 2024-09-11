/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"

/**
 * @brief LP ADC channel configurations
 */
typedef adc_oneshot_chan_cfg_t lp_core_lp_adc_chan_cfg_t;

/**
 * @brief Initialize the LP ADC
 *
 * @note We only support LP ADC1 and not LP ADC2 due to a HW issue.
 *
 * @param unit_id   LP ADC unit to initialize
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if the unit_id is invalid
 *         ESP_ERR_NOT_SUPPORTED if the API is not supported on the LP Core
 *         ESP_FAIL if the ADC unit failed to initialize
 */
esp_err_t lp_core_lp_adc_init(adc_unit_t unit_id);

/**
 * @brief Deinitialize the LP ADC
 *
 * @param unit_id   LP ADC unit to deinitialize
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if the unit_id is invalid
 *         ESP_ERR_NOT_SUPPORTED if the API is not supported on the LP Core
 *         ESP_FAIL if the ADC unit failed to deinitialize
 */
esp_err_t lp_core_lp_adc_deinit(adc_unit_t unit_id);

/**
 * @brief Configure an LP ADC channel
 *
 * @param unit_id       ADC unit to configure the channel for
 * @param channel       ADC channel to configure
 * @param chan_config   Configuration for the channel
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if the unit_id is invalid
 *         ESP_ERR_NOT_SUPPORTED if the API is not supported on the LP Core
 *         ESP_FAIL if the channel configuration fails
 */
esp_err_t lp_core_lp_adc_config_channel(adc_unit_t unit_id, adc_channel_t channel, const lp_core_lp_adc_chan_cfg_t *chan_config);

/**
 * @brief Read the raw ADC value from a channel
 *
 * @note The raw value is the 12-bit value read from the ADC.
 *       The value is between 0 and 4095. To convert this value
 *       to a voltage, use the formula:
 *       voltage = (raw_value * (1.1v / 4095)) * attenuation
 *
 *       Alternatively, use lp_core_lp_adc_read_channel_converted()
 *       to get the converted value.
 *
 * @param[in] unit_id   ADC unit to configure the channel for
 * @param[in] channel   ADC channel to configure
 * @param[in] adc_raw   Pointer to store the raw 12-bit ADC value
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if the unit_id is invalid
 *         ESP_FAIL if the read fails
 */
esp_err_t lp_core_lp_adc_read_channel_raw(adc_unit_t unit_id, adc_channel_t channel, int *adc_raw);

/**
 * @brief Read the converted ADC value in millivolts from a channel
 *
 * @note The API converts the measured voltage based on the
 *       internal reference voltage of 1.1v and the the attenuation
 *       factors. It uses the formula:
 *       voltage = (raw_value * (1.1v / 4095)) * attenuation
 *
 *       To avoid complex floating-point operations at runtime,
 *       the API converts the raw data to millivolts. Also, the
 *       conversion approximates the calculation when scaling
 *       the voltage by using pre-computed attenuation factors.
 *
 * @note The conversion approximates the measured voltage based on the
 *       internal reference voltage of 1.1v and the approximations of
 *       the attenuation factors.
 *
 * @param[in]  unit_id      ADC unit to configure the channel for
 * @param[in]  channel      ADC channel to configure
 * @param[out] voltage_mv   Pointer to store the converted ADC value in millivolts
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if the unit_id is invalid or voltage_mv is NULL
 *         ESP_FAIL if the read fails
 */
esp_err_t lp_core_lp_adc_read_channel_converted(adc_unit_t unit_id, adc_channel_t channel, int *voltage_mv);

#ifdef __cplusplus
}
#endif
