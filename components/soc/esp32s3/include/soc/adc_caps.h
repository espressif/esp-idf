// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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

#define SOC_ADC_PERIPH_NUM              (2)
#define SOC_ADC_PATT_LEN_MAX            (16)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM) (10)
#define SOC_ADC_MAX_CHANNEL_NUM         (10)
#define SOC_ADC_MAX_BITWIDTH            (13)


/**
 * Check if adc support digital controller (DMA) mode.
 * @value
 *      - 1 : support;
 *      - 0 : not support;
 */
#define SOC_ADC_SUPPORT_DMA_MODE(PERIPH_NUM) ((PERIPH_NUM==0)? 1: 1)
#define SOC_ADC_SUPPORT_RTC_CTRL        1
