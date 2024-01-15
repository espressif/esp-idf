/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char** names;
    const int* pins;
#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
    const int *adc_channels;
#endif
} pin_configuration_t;


void check_sd_card_pins(pin_configuration_t *config, const int pin_count);

#ifdef __cplusplus
}
#endif
