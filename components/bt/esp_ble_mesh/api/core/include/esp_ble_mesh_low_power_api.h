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

#ifndef _ESP_BLE_MESH_LOW_POWER_API_H_
#define _ESP_BLE_MESH_LOW_POWER_API_H_

#include "esp_ble_mesh_defs.h"

/**
 * @brief       Enable BLE Mesh device LPN functionality.
 *
 * @note        This API enables LPN functionality. Once called, the proper
 *              Friend Request will be sent.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_lpn_enable(void);

/**
 * @brief       Disable BLE Mesh device LPN functionality.
 *
 * @param[in]   force: when disabling LPN functionality, use this flag to indicate
 *                     whether directly clear corresponding information or just
 *                     send friend clear to disable it if friendship has already
 *                     been established.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_lpn_disable(bool force);

/**
 * @brief       LPN tries to poll messages from the Friend Node.
 *
 * @note        The Friend Poll message is sent by a Low Power node to ask the Friend
 *              node to send a message that it has stored for the Low Power node.
 *              Users can call this API to send Friend Poll message manually. If this
 *              API is not invoked, the bottom layer of the Low Power node will send
 *              Friend Poll before the PollTimeout timer expires.
 *              If the corresponding Friend Update is received and MD is set to 0,
 *              which means there are no messages for the Low Power node, then the
 *              Low Power node will stop scanning.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_lpn_poll(void);

#endif /* _ESP_BLE_MESH_LOW_POWER_API_H_ */
