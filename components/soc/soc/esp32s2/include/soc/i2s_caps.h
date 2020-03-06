// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#define APLL_MIN_FREQ                     (250000000)
#define APLL_MAX_FREQ                     (500000000)
#define APLL_I2S_MIN_RATE                 (10675) //in Hz, I2S Clock rate limited by hardware
#define I2S_AD_BCK_FACTOR                 (2)
#define I2S_PDM_BCK_FACTOR                (64)
#define I2S_MAX_BUFFER_SIZE               (4 * 1024 * 1024) //the maximum RAM can be allocated
#define I2S_BASE_CLK                      (2*APB_CLK_FREQ)

// ESP32-S2 have 2 I2S
#define SOC_I2S_NUM            (1)

#define SOC_I2S_SUPPORTS_PDM       (0) // ESP32-S2 don't support PDM
#define SOC_I2S_SUPPORTS_DMA_EQUAL (1) // ESP32-S2 need dma equal
#define SOC_I2S_SUPPORTS_ADC_DAC   (0) // ESP32-S2 don't support ADC and DAC
