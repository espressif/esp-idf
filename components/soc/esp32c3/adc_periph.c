// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/adc_periph.h"

/* Store IO number corresponding to the ADC channel number. */
const int adc_channel_io_map[SOC_ADC_PERIPH_NUM][SOC_ADC_MAX_CHANNEL_NUM] = {
    /* ADC1 */
    {
        ADC1_CHANNEL_0_GPIO_NUM, ADC1_CHANNEL_1_GPIO_NUM, ADC1_CHANNEL_2_GPIO_NUM, ADC1_CHANNEL_3_GPIO_NUM, ADC1_CHANNEL_4_GPIO_NUM
    },
    /* ADC2 */
    {
        ADC2_CHANNEL_0_GPIO_NUM, -1, -1, -1, -1
    }
};
