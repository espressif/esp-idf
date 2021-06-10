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

#pragma once

#define SOC_I2S_APLL_MIN_FREQ                     (250000000)
#define SOC_I2S_APLL_MAX_FREQ                     (500000000)
#define SOC_I2S_APLL_MIN_RATE                 (10675) //in Hz, I2S Clock rate limited by hardware
#define SOC_I2S_MAX_BUFFER_SIZE               (4 * 1024 * 1024) //the maximum RAM can be allocated

#define SOC_I2S_NUM            (1)
