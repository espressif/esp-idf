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

#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_openthread.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initializes the border router features of OpenThread.
 *
 * @note Calling this function will make the device behave as an OpenThread
 *       border router. Kconfig option CONFIG_OPENTHREAD_BORDER_ROUTER is required.
 *
 * @param[in]  backbone_netif   The backbone network interface (WiFi or ethernet)
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if feature not supported
 *      - ESP_ERR_INVALID_STATE if already initialized
 *      - ESP_FIAL on other failures
 *
 */
esp_err_t esp_openthread_border_router_init(esp_netif_t *backbone_netif);

/**
 * @brief   Deinitializes the border router features of OpenThread.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if not initialized
 *      - ESP_FIAL on other failures
 *
 */
esp_err_t esp_openthread_border_router_deinit(void);

/**
 * @brief   Gets the backbone interface of OpenThread border router.
 *
 * @return
 *      The backbone interface or NULL if border router not initialized.
 *
 */
esp_netif_t *esp_openthread_get_backbone_netif(void);

#ifdef __cplusplus
}
#endif
