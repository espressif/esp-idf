/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_lp_adc_shared.h"
#include "ulp_lp_core_uart.h"

#define EXAMPLE_LP_ADC1_CHAN0   CONFIG_EXAMPLE_LP_ADC1_CHANNEL_0_SELECT
#define EXAMPLE_LP_ADC1_CHAN1   CONFIG_EXAMPLE_LP_ADC1_CHANNEL_1_SELECT
int main (void)
{

    int adc_raw_value[2];
    int adc_converted_value[2];

    lp_core_lp_adc_read_channel_raw(ADC_UNIT_1, EXAMPLE_LP_ADC1_CHAN0, &adc_raw_value[0]);
    lp_core_printf("lpadc1 chan0 raw value = %d\r\n", adc_raw_value[0]);
    lp_core_lp_adc_read_channel_converted(ADC_UNIT_1, EXAMPLE_LP_ADC1_CHAN0, &adc_converted_value[0]);
    lp_core_printf("lpadc1 chan0 converted value = %d mV\r\n", adc_converted_value[0]);

    lp_core_lp_adc_read_channel_raw(ADC_UNIT_1, EXAMPLE_LP_ADC1_CHAN1, &adc_raw_value[1]);
    lp_core_printf("lpadc1 chan1 raw value = %d\r\n", adc_raw_value[1]);
    lp_core_lp_adc_read_channel_converted(ADC_UNIT_1, EXAMPLE_LP_ADC1_CHAN1, &adc_converted_value[1]);
    lp_core_printf("lpadc1 chan1 converted value = %d mV\r\n", adc_converted_value[1]);

    lp_core_printf("\n");
    lp_core_uart_tx_flush(LP_UART_NUM_0);

    return 0;
}
