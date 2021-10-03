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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for PCNT.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#include "soc/pcnt_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    pcnt_dev_t *dev; /*!< PCNT peripheral register base address */
} pcnt_hal_context_t;

/**
 * @brief Init the PCNT hal and set the PCNT to the default configuration.
 * @note This function should be called first before other hal layer function is called.
 *
 * @param hal Context of the HAL layer
 * @param group_id PCNT group ID
 */
void pcnt_hal_init(pcnt_hal_context_t *hal, int group_id);

#ifdef __cplusplus
}
#endif
