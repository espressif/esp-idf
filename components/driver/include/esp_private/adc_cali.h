// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
