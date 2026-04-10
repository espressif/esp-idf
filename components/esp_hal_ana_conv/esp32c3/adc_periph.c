/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/adc_periph.h"

/* Store IO number corresponding to the ADC channel number. */
const int adc_channel_io_map[2][5] = {
    /* ADC1 */
    {
        ADC1_CHANNEL_0_GPIO_NUM, ADC1_CHANNEL_1_GPIO_NUM, ADC1_CHANNEL_2_GPIO_NUM, ADC1_CHANNEL_3_GPIO_NUM, ADC1_CHANNEL_4_GPIO_NUM
    },
    /* ADC2 */
    {
        ADC2_CHANNEL_0_GPIO_NUM, -1, -1, -1, -1
    }
};
