/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "ulp_lp_core_lp_adc_shared.h"

volatile int adc_raw[8];

int main(void)
{
    while (1) {
        for (int i = 0; i < 8; i++) {
            lp_core_lp_adc_read_channel_raw(ADC_UNIT_1, i, (int *)&adc_raw[i]);
        }
    }

    return 0;
}
