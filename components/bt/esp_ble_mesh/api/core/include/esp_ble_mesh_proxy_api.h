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

/**
 * @brief        Proxy Client creates a connection with the Proxy Server.
 *
 * @param[in]    addr:      Device address of the Proxy Server.
 * @param[in]    addr_type: Device address type(public or static random).
 * @param[in]    net_idx:   NetKey Index related with Network ID in the Mesh Proxy
 *                          advertising packet.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_client_connect(esp_ble_mesh_bd_addr_t addr,
        esp_ble_mesh_addr_type_t addr_type, uint16_t net_idx);

/**
 * @brief        Proxy Client terminates a connection with the Proxy Server.
 *
 * @param[in]    conn_handle: Proxy connection handle.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_client_disconnect(uint8_t conn_handle);

/**
 * @brief        Proxy Client sets the filter type of the Proxy Server.
 *
 * @param[in]    conn_handle: Proxy connection handle.
 * @param[in]    net_idx:     Corresponding NetKey Index.
 * @param[in]    filter_type: whitelist or blacklist.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_client_set_filter_type(uint8_t conn_handle,
        uint16_t net_idx, esp_ble_mesh_proxy_filter_type_t filter_type);

/**
 * @brief        Proxy Client adds address to the Proxy Server filter list.
 *
 * @param[in]    conn_handle: Proxy connection handle.
 * @param[in]    net_idx:     Corresponding NetKey Index.
 * @param[in]    addr:        Pointer to the filter address.
 * @param[in]    addr_num:    Number of the filter address.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_client_add_filter_addr(uint8_t conn_handle,
        uint16_t net_idx, uint16_t *addr, uint16_t addr_num);

/**
 * @brief        Proxy Client removes address from the Proxy Server filter list.
 *
 * @param[in]    conn_handle: Proxy connection handle.
 * @param[in]    net_idx:     Corresponding NetKey Index.
 * @param[in]    addr:        Pointer to the filter address.
 * @param[in]    addr_num:    Number of the filter address.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_proxy_client_remove_filter_addr(uint8_t conn_handle,
        uint16_t net_idx, uint16_t *addr, uint16_t addr_num);

#endif /* _ESP_BLE_MESH_PROXY_API_H_ */

