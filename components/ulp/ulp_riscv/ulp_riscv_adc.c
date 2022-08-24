/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_riscv/ulp_riscv_adc_ulp_core.h"
#include "hal/adc_ll.h"

int32_t ulp_riscv_adc_read_channel(adc_unit_t adc_n, int channel)
{
    adc_ll_rtc_enable_channel(adc_n, channel);

    adc_ll_rtc_start_convert(adc_n, channel);
    while (adc_ll_rtc_convert_is_done(adc_n) != true) {
        ;
    }
    int32_t out_raw = adc_ll_rtc_get_convert_value(adc_n);

    return out_raw;
}
