/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/adc_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    adc_channel_t channel;  // ADC channel
    adc_atten_t atten;      // ADC channel attenuation
    adc_bits_width_t width; // ADC bit width, only used for ADC unit 1
} ulp_riscv_adc_cfg_t;      // ULP Riscv ADC configuration parameters

/**
 * @brief Initialize and calibrate the ADC1 for use by ULP RISCV
 *
 * @param cfg           Configuration parameters
 * @return esp_err_t    ESP_OK for successful.
 */
esp_err_t ulp_riscv_adc_init(const ulp_riscv_adc_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
