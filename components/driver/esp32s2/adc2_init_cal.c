/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
