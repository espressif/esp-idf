// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/soc.h"
#include "soc/syscon_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "soc/rtc_io_struct.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/adc_channel.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Store IO number corresponding to the ADC channel number.
 *
 * @value
 *      - >=0 : GPIO number index.
 *      - -1  : Not support.
 */
extern const int adc_channel_io_map[SOC_ADC_PERIPH_NUM][SOC_ADC_MAX_CHANNEL_NUM];

#ifdef __cplusplus
}
#endif