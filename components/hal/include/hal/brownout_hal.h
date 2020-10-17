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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"

typedef struct {
    uint8_t threshold;
    bool enabled;
    bool reset_enabled;
    bool flash_power_down;
    bool rf_power_down;
} brownout_hal_config_t;

void brownout_hal_config(const brownout_hal_config_t *cfg);

void brownout_hal_intr_enable(bool enable);

void brownout_hal_intr_clear(void);

#ifdef __cplusplus
}
#endif
