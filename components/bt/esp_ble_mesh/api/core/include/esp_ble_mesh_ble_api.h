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

#ifndef _ESP_BLE_MESH_BLE_API_H_
#define _ESP_BLE_MESH_BLE_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief         This function is called to start BLE advertising with the corresponding data
 *                and parameters while BLE Mesh is working at the same time.
 *
 * @note          1. When this function is called, the BLE advertising packet will be posted to
 *                the BLE mesh adv queue in the mesh stack and waited to be sent.
 *                2. In the BLE advertising parameters, the "duration" means the time used for
 *                sending the BLE advertising packet each time, it shall not be smaller than the
 *                advertising interval. When the packet is sent successfully, it will be posted
 *                to the adv queue again after the "period" time if the "count" is bigger than 0.
 *                The "count" means how many durations the packet will be sent after it is sent
 *                successfully for the first time. And if the "count" is set to 0xFFFF, which
 *                means the packet will be sent infinitely.
 *                3. The "priority" means the priority of BLE advertising packet compared with
 *                BLE Mesh packets. Currently two options (i.e. low/high) are provided. If the
 *                "priority" is high, the BLE advertising packet will be posted to the front of
 *                adv queue. Otherwise it will be posted to the back of adv queue.
 *
 * @param[in]     param: Pointer to the BLE advertising parameters
 * @param[in]     data:  Pointer to the BLE advertising data and scan response data
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_start_ble_advertising(const esp_ble_mesh_ble_adv_param_t *param,
                                             const esp_ble_mesh_ble_adv_data_t *data);

/**
 * @brief         This function is called to stop BLE advertising with the corresponding index.
 *
 * @param[in]     index: Index of BLE advertising
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_stop_ble_advertising(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_BLE_API_H_ */
