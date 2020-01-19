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

#include <stdint.h>

#include "esp_err.h"

#include "hal/cpu_hal.h"
#include "hal/soc_hal.h"
#include "hal/soc_ll.h"
#include "soc/soc_caps.h"

#define CHECK_CORE(core)                            { if ((core) > SOC_CPU_CORES_NUM) return ESP_ERR_INVALID_ARG; }
#define PERFORM_ON_OTHER_CORES(action)              { \
                                                        for (int i = 0, cur = cpu_hal_get_core_id(); i < SOC_CPU_CORES_NUM; i++) { \
                                                            if (i != cur) { \
                                                                action(i); \
                                                            } \
                                                        } \
                                                    }

esp_err_t soc_hal_stall_core(int core)
{
    CHECK_CORE(core);

    if (core < 0) {
        PERFORM_ON_OTHER_CORES(soc_hal_stall_core);
    } else {
        soc_ll_stall_core(core);
    }

    return ESP_OK;
}

esp_err_t soc_hal_unstall_core(int core)
{
    CHECK_CORE(core);

    if (core < 0) {
        PERFORM_ON_OTHER_CORES(soc_hal_unstall_core);
    } else {
        soc_ll_unstall_core(core);
    }

    return ESP_OK;
}

esp_err_t soc_hal_reset_core(int core)
{
    CHECK_CORE(core);

    if (core < 0) {
        PERFORM_ON_OTHER_CORES(soc_hal_reset_core);
    } else {
        soc_ll_reset_core(core);
    }

    return ESP_OK;
}