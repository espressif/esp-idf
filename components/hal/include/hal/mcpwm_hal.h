// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for MCPWM (common part)

#pragma once

#include "soc/mcpwm_struct.h"

typedef struct {
    int host_id; ///< Which MCPWM peripheral to use, 0-1.
} mcpwm_hal_init_config_t;

typedef struct {
    mcpwm_dev_t *dev; ///< Beginning address of the peripheral registers of a single MCPWM unit. Call `mcpwm_hal_init` to initialize it.
} mcpwm_hal_context_t;

/**
 * @brief Initialize the internal state of the HAL.
 *
 * @param hal Context of the HAL layer.
 * @param init_config Configuration for the HAL to be used only once.
 */
void mcpwm_hal_init(mcpwm_hal_context_t *hal, const mcpwm_hal_init_config_t *init_config);
