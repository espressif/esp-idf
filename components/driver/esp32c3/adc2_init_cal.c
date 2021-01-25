// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
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

/* This file is used to get `adc2_init_code_calibration` executed before the APP when the ADC2 is used by Wi-Fi or other drivers.
The linker will link constructor (adc2_init_code_calibration) only when any sections inside the same file (adc2_cal_include) is used.
Don't put any other code into this file. */

#include "adc2_wifi_private.h"
#include "hal/adc_hal.h"
#include "esp_private/adc_cali.h"

/**
 * @brief Set initial code to ADC2 after calibration. ADC2 RTC and ADC2 PWDET controller share the initial code.
 *        This API be called in before `app_main()`.
 */
static __attribute__((constructor)) void adc2_init_code_calibration(void)
{
    const adc_ll_num_t adc_n = ADC_NUM_2;
    const adc_atten_t atten = ADC_ATTEN_DB_11;
    const adc_channel_t channel = 0;
    adc_cal_offset(adc_n, channel, atten);
}

/** Don't call `adc2_cal_include` in user code. */
void adc2_cal_include(void)
{
    /* When this empty function is called, the `adc2_init_code_calibration` constructor will be linked and executed before the app.*/
}
