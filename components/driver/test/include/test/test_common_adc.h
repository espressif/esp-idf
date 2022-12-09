// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "driver/adc.h"

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
