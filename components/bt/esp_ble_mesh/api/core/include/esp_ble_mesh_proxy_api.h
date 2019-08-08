// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP_BLE_MESH_PROXY_API_H_
#define _ESP_BLE_MESH_PROXY_API_H_

#include "esp_ble_mesh_defs.h"

/**
 * @brief   Enable advertising with Node Identity.
 *
 * @note    This API requires that GATT Proxy support be enabled. Once called,
 *          each subnet starts advertising using Node Identity for the next 60
 *          seconds, and after 60s Network ID will be advertised.
 *          Under normal conditions, the BLE Mesh Proxy Node Identity and
 *          Network ID advertising will be enabled automatically by BLE Mesh
 *          stack after the device is provisioned.
 *
 * @return  ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_identity_enable(void);

/**
 * @brief   Enable BLE Mesh GATT Proxy Service.
 *
 * @return  ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_gatt_enable(void);

/**
 * @brief   Disconnect the BLE Mesh GATT Proxy connection if there is any, and
 *          disable the BLE Mesh GATT Proxy Service.
 *
 * @return  ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_gatt_disable(void);

#endif /* _ESP_BLE_MESH_PROXY_API_H_ */

