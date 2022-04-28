// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#pragma once

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_openthread_types.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread network interface glue.
 *
 * @param[in] config    The platform configuration.
 *
 * @return
 *      - glue pointer on success
 *      - NULL on failure
 *
 */
void *esp_openthread_netif_glue_init(const esp_openthread_platform_config_t *config);

/**
 * @brief This function deinitializes the OpenThread network interface glue.
 *
 */
void esp_openthread_netif_glue_deinit(void);

/**
 * @brief This function acquires the OpenThread netif.
 *
 * @return
 *      The OpenThread netif or NULL if not initialzied.
 *
 */
esp_netif_t *esp_openthread_get_netif(void);

#ifdef __cplusplus
}
#endif
