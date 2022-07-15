/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/adc_types.h"

/**@{*/
/**
 * Tie the ADC channel input to the middle (1.4V) by connecting both internal pullup and pulldown.
 *
 * `tie_middle`: 1.4v;
 * `tie_high`  : 3.3v;
 * `tie_low`   : 0v;
 *
 * @param adc_unit_t ADC unit.
 * @param adc_channel_t ADC channel.
 */
void adc_fake_tie_middle(adc_unit_t adc_unit, adc_channel_t channel);
void adc_fake_tie_high(adc_unit_t adc_unit, adc_channel_t channel);
void adc_fake_tie_low(adc_unit_t adc_unit, adc_channel_t channel);
void adc_io_normal(adc_unit_t adc_unit, adc_channel_t channel);
/**@}*/
