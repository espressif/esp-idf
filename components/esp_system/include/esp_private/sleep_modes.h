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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_SYSTEM_PM_POWER_DOWN_CPU
/**
 * @brief CPU Power down low-level initialize
 *
 * @param enable  enable or disable CPU power down during light sleep
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough retention memory
 */
esp_err_t esp_sleep_cpu_pd_low_init(bool enable);
#endif

#ifdef __cplusplus
}
#endif
