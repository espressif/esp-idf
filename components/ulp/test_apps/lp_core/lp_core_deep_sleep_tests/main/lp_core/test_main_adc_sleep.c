/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_lp_adc_shared.h"
#include "lp_sleep_payload.h"
#include "lp_sleep_finish.h"

volatile int32_t adc_sleep_ret[LP_SLEEP_ADC_CHANNELS];
volatile int32_t adc_sleep_raw[LP_SLEEP_ADC_CHANNELS];

int main(void)
{
    ulp_lp_core_delay_us(LP_SLEEP_SETTLE_US);

    for (int channel = 0; channel < LP_SLEEP_ADC_CHANNELS; channel++) {
        adc_sleep_ret[channel] = lp_core_lp_adc_read_channel_raw(
                                     ADC_UNIT_1, channel, (int *)&adc_sleep_raw[channel]);
    }

    lp_sleep_finish();
}
