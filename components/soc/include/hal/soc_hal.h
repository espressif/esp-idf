// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

/**
 * Stall the specified CPU core.
 *
 * @note Has no effect if the core is already stalled - does not return an
 * ESP_ERR_INVALID_STATE.
 *
 * @param core core to stall [0..SOC_CPU_CORES_NUM - 1]; if core < 0 is specified, all other cores are stalled
 *
 * @return ESP_ERR_INVALID_ARG core argument invalid
 * @return ESP_OK success
 */
esp_err_t soc_hal_stall_core(int core);

/**
 * Unstall the specified CPU core.
 *
 * @note Has no effect if the core is already unstalled - does not return an
 * ESP_ERR_INVALID_STATE.
 *
 * @param core core to unstall [0..SOC_CPU_CORES_NUM - 1]; if core < 0 is specified, all other cores are unstalled
 *
 * @return ESP_ERR_INVALID_ARG core argument invalid
 * @return ESP_OK success
 */
esp_err_t soc_hal_unstall_core(int core);

/**
 * Reset the specified core.
 *
 * @param core core to reset [0..SOC_CPU_CORES_NUM - 1]; if core < 0 is specified, all other cores are reset
 *
 * @return ESP_ERR_INVALID_ARG core argument invalid
 * @return ESP_OK success
 */
esp_err_t soc_hal_reset_core(int core);