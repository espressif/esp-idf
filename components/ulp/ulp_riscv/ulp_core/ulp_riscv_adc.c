/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_riscv_adc_ulp_core.h"
#include "hal/adc_ll.h"

int32_t ulp_riscv_adc_read_channel(adc_unit_t adc_n, int channel)
{
    uint32_t event = (adc_n == ADC_UNIT_1) ? ADC_LL_EVENT_ADC1_ONESHOT_DONE : ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    adc_oneshot_ll_clear_event(event);
    adc_oneshot_ll_disable_all_unit();
    adc_oneshot_ll_enable(adc_n);
    adc_oneshot_ll_set_channel(adc_n, channel);

    adc_oneshot_ll_start(adc_n);
    while (adc_oneshot_ll_get_event(event) != true) {
        ;
    }
    int32_t out_raw = adc_oneshot_ll_get_raw_result(adc_n);
    if (adc_oneshot_ll_raw_check_valid(adc_n, out_raw) == false) {
        return -1;
    }

    //HW workaround: when enabling periph clock, this should be false
    adc_oneshot_ll_disable_all_unit();

    return out_raw;
}
