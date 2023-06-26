/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP-RISC-V example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP-RISC-V  coprocessor
*/

#include <stdint.h>
#include "ulp_riscv_utils.h"
#include "ulp_riscv_adc_ulp_core.h"

#include "example_config.h"

uint32_t adc_threshold = EXAMPLE_ADC_TRESHOLD;
int32_t wakeup_result;

int main (void)
{
    int32_t last_result = ulp_riscv_adc_read_channel(EXAMPLE_ADC_UNIT, EXAMPLE_ADC_CHANNEL);

    if (last_result > adc_threshold) {
        wakeup_result = last_result;
        ulp_riscv_wakeup_main_processor();
    }

    return 0;
}
