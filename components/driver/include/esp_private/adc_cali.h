/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Internal header for calibration, don't use in app

#include "sdkconfig.h"
#include "esp_err.h"
#include "hal/adc_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !CONFIG_IDF_TARGET_ESP32

/**
 * @brief Calibrate the offset of ADC. (Based on the pre-stored efuse or actual calibration)
 *
 * @param adc_n ADC unit to calibrate
 * @param channel Target channel if really do calibration
 * @param atten Attenuation to use
 * @return Always ESP_OK
 */
extern esp_err_t adc_cal_offset(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten);

#endif

#ifdef __cplusplus
}
#endif
