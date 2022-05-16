/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for ADC (common part)

#include "hal/adc_hal.h"
#include "hal/adc_hal_conf.h"
#include "hal/adc_types.h"


int adc_hal_hall_convert(void)
{
    int Sens_Vp0;
    int Sens_Vn0;
    int Sens_Vp1;
    int Sens_Vn1;
    int hall_value;
    // convert for 4 times with different phase and outputs
    adc_ll_hall_phase_disable();      // hall phase
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_0, &Sens_Vp0 );
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_3, &Sens_Vn0 );
    adc_ll_hall_phase_enable();
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_0, &Sens_Vp1 );
    adc_hal_convert( ADC_NUM_1, ADC_CHANNEL_3, &Sens_Vn1 );
    hall_value = (Sens_Vp1 - Sens_Vp0) - (Sens_Vn1 - Sens_Vn0);
    return hall_value;
}
