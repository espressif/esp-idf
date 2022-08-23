/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "esp_types.h"
#include "esp_err.h"

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
     * @param[in]  arg        ///< ADC calibration scheme specific context
     * @param[in]  raw        ///< ADC raw data
     * @param[out] voltage    ///< Calibrated ADC voltage (in mV)
     *
     * @return
     *         - ESP_OK:                On success
     *         - ESP_ERR_INVALID_ARG:   Invalid argument
     *         - ESP_ERR_INVALID_STATE: Invalid state, scheme didn't registered
     */
    esp_err_t (*raw_to_voltage)(void *arg, int raw, int *voltage);

    /**
     * @brief ADC calibration specific contexts
     * Can be customized to difference calibration schemes
     */
    void *ctx;

};

#ifdef __cplusplus
}
#endif
