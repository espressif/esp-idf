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

#include "soc/soc_caps.h"
#include "hal/cpu_hal.h"
#include "hal/soc_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_CPU_CORES_NUM > 1
// Utility functions for multicore targets
#define __SOC_HAL_PERFORM_ON_OTHER_CORES(action)    { \
                                                        for (uint32_t i = 0, cur = cpu_hal_get_core_id(); i < SOC_CPU_CORES_NUM; i++) { \
                                                            if (i != cur) { \
                                                                action(i); \
                                                            } \
                                                        } \
                                                    }

#define SOC_HAL_STALL_OTHER_CORES()                 __SOC_HAL_PERFORM_ON_OTHER_CORES(soc_hal_stall_core);
#define SOC_HAL_UNSTALL_OTHER_CORES()               __SOC_HAL_PERFORM_ON_OTHER_CORES(soc_hal_unstall_core);
#define SOC_HAL_RESET_OTHER_CORES()                 __SOC_HAL_PERFORM_ON_OTHER_CORES(soc_hal_reset_core);

/**
 * Stall the specified CPU core.
 *
 * @note Has no effect if the core is already stalled - does not return an
 * ESP_ERR_INVALID_STATE.
 *
 * @param core core to stall [0..SOC_CPU_CORES_NUM - 1]
 */
void soc_hal_stall_core(int core);

/**
 * Unstall the specified CPU core.
 *
 * @note Has no effect if the core is already unstalled - does not return an
 * ESP_ERR_INVALID_STATE.
 *
 * @param core core to unstall [0..SOC_CPU_CORES_NUM - 1]
 */
void soc_hal_unstall_core(int core);

#endif // SOC_CPU_CORES_NUM > 1

/**
 * Reset the specified core.
 *
 * @param core core to reset [0..SOC_CPU_CORES_NUM - 1]
 */
#define soc_hal_reset_core(core)         soc_ll_reset_core((core))

#ifdef __cplusplus
}
#endif
