// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
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
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#include "soc/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Power management config for ESP32
 *
 * Pass a pointer to this structure as an argument to esp_pm_configure function.
 */
typedef struct {
    int max_freq_mhz;   /*!< Maximum CPU frequency, in MHz */
    int min_freq_mhz;   /*!< Minimum CPU frequency to use when no locks are taken, in MHz */
    bool light_sleep_enable;        /*!< Enter light sleep when no locks are taken */
} esp_pm_config_esp32s2_t;


#ifdef __cplusplus
}
#endif
