// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define APLL_MIN_FREQ                     (250000000)
#define APLL_MAX_FREQ                     (500000000)
#define APLL_I2S_MIN_RATE                 (10675) //in Hz, I2S Clock rate limited by hardware
#define I2S_AD_BCK_FACTOR                 (2)
#define I2S_PDM_BCK_FACTOR                (64)
#define I2S_MAX_BUFFER_SIZE               (4 * 1024 * 1024) //the maximum RAM can be allocated
#define I2S_BASE_CLK                      (2*APB_CLK_FREQ)

// ESP32 have 2 I2S
#define I2S_NUM_0             (0) /*!< I2S port 0 */
#define I2S_NUM_1             (1) /*!< I2S port 0 */
#define I2S_NUM_MAX           (2) /*!< I2S port max */
#define SOC_I2S_NUM           (I2S_NUM_MAX)

#define SOC_I2S_SUPPORT_PDM     (1) //ESP32 support PDM

#ifdef __cplusplus
}
#endif
