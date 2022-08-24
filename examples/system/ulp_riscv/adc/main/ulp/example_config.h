/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "hal/adc_types.h"

#define EXAMPLE_ADC_CHANNEL     ADC_CHANNEL_0
#define EXAMPLE_ADC_ATTEN       ADC_ATTEN_DB_11
#define EXAMPLE_ADC_WIDTH       ADC_WIDTH_BIT_12

/* Set high threshold, approx. 1.75V*/
#define EXAMPLE_ADC_TRESHOLD    2000
