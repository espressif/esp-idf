/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "esp_types.h"
#include "esp_err.h"
#include "esp_adc/adc_cali.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct adc_cali_scheme_t adc_cali_scheme_t;

/**
 * @brief ADC Calibration Scheme Interface and Context
 */
struct adc_cali_scheme_t {

    /**
     * @brief Convert ADC raw data to calibrated voltage
     *
     * Optional. Schemes that need the ADC channel may leave this NULL.
     * Then `adc_cali_raw_to_voltage()` returns `ESP_ERR_NOT_SUPPORTED`.
     *
     * @param[in]  arg        ADC calibration scheme specific context
     * @param[in]  raw        ADC raw data
     * @param[out] voltage    Calibrated ADC voltage (in mV)
     *
     * @return
     *         - ESP_OK:                On success
     *         - ESP_ERR_INVALID_ARG:   Invalid argument
     *         - ESP_ERR_INVALID_STATE: Invalid state, scheme didn't registered
     *         - ESP_ERR_NOT_SUPPORTED: Scheme requires the channel-aware conversion API
     */
    esp_err_t (*raw_to_voltage)(void *arg, int raw, int *voltage);

    /**
     * @brief Convert ADC raw data to calibrated voltage, with channel info
     *
     * Optional. Schemes that need the channel to apply channel-dependent
     * corrections implement this. When NULL, `adc_cali_chan_raw_to_voltage()`
     * falls back to `raw_to_voltage` and ignores the channel.
     *
     * @param[in]  arg        ADC calibration scheme specific context
     * @param[in]  channel    ADC channel the raw data was sampled from
     * @param[in]  raw        ADC raw data
     * @param[out] voltage    Calibrated ADC voltage (in mV)
     *
     * @return
     *         - ESP_OK:                On success
     *         - ESP_ERR_INVALID_ARG:   Invalid argument
     *         - ESP_ERR_INVALID_STATE: Invalid state, scheme didn't registered
     */
    esp_err_t (*chan_raw_to_voltage)(void *arg, adc_channel_t channel, int raw, int *voltage);

    /**
     * @brief Delete ADC calibration scheme specific context
     *
     * @param[in] arg ADC calibration scheme specific context
     *
     * @return
     *         - ESP_OK:                On success
     *         - ESP_ERR_INVALID_ARG:   Invalid argument
     */
    esp_err_t (*del)(void *arg);

    /**
     * @brief ADC calibration specific contexts
     * Can be customized to difference calibration schemes
     */
    void *ctx;

};

/**
 * @brief Allocate an ADC calibration scheme wrapper (handle)
 *
 * The common layer owns the lifecycle of the `adc_cali_scheme_t` wrapper: it is
 * allocated here and released by `adc_cali_delete_scheme()`. A scheme
 * implementation only needs to allocate and free its own `ctx` (the latter via
 * the `del` callback).
 *
 * @param[in]  raw_to_voltage       Scheme-specific raw-to-voltage callback, or NULL
 *                                  when only the channel-aware callback is provided
 * @param[in]  chan_raw_to_voltage  Optional channel-aware callback, or NULL
 *                                  when `raw_to_voltage` is provided. At least one
 *                                  conversion callback is required.
 * @param[in]  del                  Scheme-specific context delete callback
 * @param[in]  ctx                  Scheme-specific context
 * @param[out] ret_handle           Output: created calibration scheme handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_NO_MEM:        Out of memory
 */
esp_err_t adc_cali_new_scheme(esp_err_t (*raw_to_voltage)(void *arg, int raw, int *voltage),
                              esp_err_t (*chan_raw_to_voltage)(void *arg, adc_channel_t channel, int raw, int *voltage),
                              esp_err_t (*del)(void *arg),
                              void *ctx,
                              adc_cali_scheme_t **ret_handle);

/**
 * @brief Delete an ADC calibration scheme handle
 *
 * Scheme implementations should expose a scheme-specific delete wrapper that
 * calls this function.
 *
 * @param[in] handle ADC calibration handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 */
esp_err_t adc_cali_delete_scheme(adc_cali_handle_t handle);

#ifdef __cplusplus
}
#endif
