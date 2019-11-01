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

#ifndef _ESP_BLE_MESH_PROVISIONING_API_H_
#define _ESP_BLE_MESH_PROVISIONING_API_H_

#include "esp_ble_mesh_defs.h"

/** @brief: event, event code of provisioning events; param, parameters of provisioning events */
typedef void (* esp_ble_mesh_prov_cb_t)(esp_ble_mesh_prov_cb_event_t event,
                                        esp_ble_mesh_prov_cb_param_t *param);

/**
 * @brief         Register BLE Mesh provisioning callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_prov_callback(esp_ble_mesh_prov_cb_t callback);

/**
 * @brief         Check if a device has been provisioned.
 *
 * @return        TRUE if the device is provisioned, FALSE if the device is unprovisioned.
 *
 */
bool esp_ble_mesh_node_is_provisioned(void);

/**
 * @brief         Enable specific provisioning bearers to get the device ready for provisioning.
 *
 * @note          PB-ADV: send unprovisioned device beacon.
 *                PB-GATT: send connectable advertising packets.
 *
 * @param         bearers: Bit-wise OR of provisioning bearers.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_prov_enable(esp_ble_mesh_prov_bearer_t bearers);

/**
 * @brief         Disable specific provisioning bearers to make a device inaccessible for provisioning.
 *
 * @param         bearers: Bit-wise OR of provisioning bearers.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_prov_disable(esp_ble_mesh_prov_bearer_t bearers);

/**
 * @brief        Unprovisioned device set own oob public key & private key pair.
 *
 * @param[in]    pub_key_x:   Unprovisioned device's Public Key X
 * @param[in]    pub_key_y:   Unprovisioned device's Public Key Y
 * @param[in]    private_key: Unprovisioned device's Private Key
 *
 * @return       ESP_OK on success or error code otherwise.
 */
esp_err_t esp_ble_mesh_node_set_oob_pub_key(uint8_t pub_key_x[32], uint8_t pub_key_y[32],
        uint8_t private_key[32]);

/**
 * @brief        Provide provisioning input OOB number.
 *
 * @note         This is intended to be called if the user has received ESP_BLE_MESH_NODE_PROV_INPUT_EVT
 *               with ESP_BLE_MESH_ENTER_NUMBER as the action.
 *
 * @param[in]    number: Number input by device.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_input_number(uint32_t number);

/**
 * @brief        Provide provisioning input OOB string.
 *
 * @note         This is intended to be called if the user has received ESP_BLE_MESH_NODE_PROV_INPUT_EVT
 *               with ESP_BLE_MESH_ENTER_STRING as the action.
 *
 * @param[in]    string: String input by device.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_input_string(const char *string);

/**
 * @brief        Using this function, an unprovisioned device can set its own device name,
 *               which will be broadcasted in its advertising data.
 *
 * @param[in]    name: Unprovisioned device name
 *
 * @note         This API applicable to PB-GATT mode only by setting the name to the scan response data,
 *               it doesn't apply to PB-ADV mode.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_set_unprovisioned_device_name(const char *name);

/**
 * @brief        Provisioner inputs unprovisioned device's oob public key.
 *
 * @param[in]    link_idx:   The provisioning link index
 * @param[in]    pub_key_x:  Unprovisioned device's Public Key X
 * @param[in]    pub_key_y:  Unprovisioned device's Public Key Y
 *
 * @return       ESP_OK on success or error code otherwise.
 */
esp_err_t esp_ble_mesh_provisioner_read_oob_pub_key(uint8_t link_idx, uint8_t pub_key_x[32],
        uint8_t pub_key_y[32]);

/**
 * @brief        Provide provisioning input OOB string.
 *
 *               This is intended to be called after the esp_ble_mesh_prov_t prov_input_num
 *               callback has been called with ESP_BLE_MESH_ENTER_STRING as the action.
 *
 * @param[in]    string:   String input by Provisioner.
 * @param[in]    link_idx: The provisioning link index.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_input_string(const char *string, uint8_t link_idx);

/**
 * @brief        Provide provisioning input OOB number.
 *
 *               This is intended to be called after the esp_ble_mesh_prov_t prov_input_num
 *               callback has been called with ESP_BLE_MESH_ENTER_NUMBER as the action.
 *
 * @param[in]    number:   Number input by Provisioner.
 * @param[in]    link_idx: The provisioning link index.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_input_number(uint32_t number, uint8_t link_idx);

/**
 * @brief        Enable one or more provisioning bearers.
 *
 * @param[in]    bearers: Bit-wise OR of provisioning bearers.
 *
 * @note         PB-ADV: Enable BLE scan.
 *               PB-GATT: Initialize corresponding BLE Mesh Proxy info.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_prov_enable(esp_ble_mesh_prov_bearer_t bearers);

/**
 * @brief        Disable one or more provisioning bearers.
 *
 * @param[in]    bearers: Bit-wise OR of provisioning bearers.
 *
 * @note         PB-ADV: Disable BLE scan.
 *               PB-GATT: Break any existing BLE Mesh Provisioning connections.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_prov_disable(esp_ble_mesh_prov_bearer_t bearers);

/**
 * @brief        Add unprovisioned device info to the unprov_dev queue.
 *
 * @param[in]    add_dev: Pointer to a struct containing the device information
 * @param[in]    flags: Flags indicate several operations on the device information
 *                      - Remove device information from queue after device has been provisioned (BIT0)
 *                      - Start provisioning immediately after device is added to queue (BIT1)
 *                      - Device can be removed if device queue is full (BIT2)
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 * @note:        1. Currently address type only supports public address and static random address.
 *               2. If device UUID and/or device address as well as address type already exist in the
 *                  device queue, but the bearer is different from the existing one, add operation
 *                  will also be successful and it will update the provision bearer supported by
 *                  the device.
 *               3. For example, if the Provisioner wants to add an unprovisioned device info before
 *                  receiving its unprovisioned device beacon or Mesh Provisioning advertising packets,
 *                  the Provisioner can use this API to add the device info with each one or both of
 *                  device UUID and device address added. When the Provisioner gets the device's
 *                  advertising packets, it will start provisioning the device internally.
 *                  - In this situation, the Provisioner can set bearers with each one or both of
 *                    ESP_BLE_MESH_PROV_ADV and ESP_BLE_MESH_PROV_GATT enabled, and cannot set flags
 *                    with ADD_DEV_START_PROV_NOW_FLAG enabled.
 *               4. Another example is when the Provisioner receives the unprovisioned device's beacon or
 *                  Mesh Provisioning advertising packets, the advertising packets will be reported on to
 *                  the application layer using the callback registered by the function
 *                  esp_ble_mesh_register_prov_callback. And in the callback, the Provisioner
 *                  can call this API to start provisioning the device.
 *                  - If the Provisioner uses PB-ADV to provision, either one or both of device UUID and
 *                    device address can be added, bearers shall be set with ESP_BLE_MESH_PROV_ADV
 *                    enabled and the flags shall be set with ADD_DEV_START_PROV_NOW_FLAG enabled.
 *                  - If the Provisioner uses PB-GATT to provision, both the device UUID and device
 *                    address need to be added, bearers shall be set with ESP_BLE_MESH_PROV_GATT enabled,
 *                    and the flags shall be set with ADD_DEV_START_PROV_NOW_FLAG enabled.
 *                  - If the Provisioner just wants to store the unprovisioned device info when receiving
 *                    its advertising packets and start to provision it the next time (e.g. after receiving
 *                    its advertising packets again), then it can add the device info with either one or both
 *                    of device UUID and device address included. Bearers can be set with either one or both
 *                    of ESP_BLE_MESH_PROV_ADV and ESP_BLE_MESH_PROV_GATT enabled (recommend to enable the
 *                    bearer which will receive its advertising packets, because if the other bearer is
 *                    enabled, the Provisioner is not aware if the device supports the bearer), and flags
 *                    cannot be set with ADD_DEV_START_PROV_NOW_FLAG enabled.
 *                  - Note: ESP_BLE_MESH_PROV_ADV, ESP_BLE_MESH_PROV_GATT and ADD_DEV_START_PROV_NOW_FLAG
 *                          can not be enabled at the same time.
 *
 */
esp_err_t esp_ble_mesh_provisioner_add_unprov_dev(esp_ble_mesh_unprov_dev_add_t *add_dev,
        esp_ble_mesh_dev_add_flag_t flags);

/**
 * @brief        Delete device from queue, reset current provisioning link and reset the node.
 *
 * @note         If the device is in the queue, remove it from the queue; if the device is being
 *               provisioned, terminate the provisioning procedure; if the device has already
 *               been provisioned, reset the device. And either one of the addr or device UUID
 *               can be input.
 *
 * @param[in]    del_dev: Pointer to a struct containing the device information.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_dev(esp_ble_mesh_device_delete_t *del_dev);

/**
 * @brief        Callback for Provisioner that received advertising packets from unprovisioned devices which are
 *               not in the unprovisioned device queue.
 *
 *               Report on the unprovisioned device beacon and mesh provisioning service adv data to application.
 *
 * @param[in]    addr: Pointer to the unprovisioned device address.
 * @param[in]    addr_type: Unprovisioned device address type.
 * @param[in]    adv_type: Adv packet type(ADV_IND or ADV_NONCONN_IND).
 * @param[in]    dev_uuid: Unprovisioned device UUID pointer.
 * @param[in]    oob_info: OOB information of the unprovisioned device.
 * @param[in]    bearer: Adv packet received from PB-GATT or PB-ADV bearer.
 *
 */
typedef void (*esp_ble_mesh_prov_adv_cb_t)(const esp_ble_mesh_bd_addr_t addr, const esp_ble_mesh_addr_type_t addr_type,
        const uint8_t adv_type, const uint8_t *dev_uuid,
        uint16_t oob_info, esp_ble_mesh_prov_bearer_t bearer);

/**
 * @brief         This function is called by Provisioner to set the part of the device UUID
 *                to be compared before starting to provision.
 *
 * @param[in]     match_val: Value to be compared with the part of the device UUID.
 * @param[in]     match_len: Length of the compared match value.
 * @param[in]     offset: Offset of the device UUID to be compared (based on zero).
 * @param[in]     prov_after_match: Flag used to indicate whether provisioner should start to provision
 *                                  the device immediately if the part of the UUID matches.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_dev_uuid_match(const uint8_t *match_val, uint8_t match_len,
        uint8_t offset, bool prov_after_match);

/**
 * @brief         This function is called by Provisioner to set provisioning data information
 *                before starting to provision.
 *
 * @param[in]     prov_data_info: Pointer to a struct containing net_idx or flags or iv_index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_prov_data_info(esp_ble_mesh_prov_data_info_t *prov_data_info);

/**
 * @brief         This function is called to set provisioning data information before starting
 *                fast provisioning.
 *
 * @param[in]     fast_prov_info: Pointer to a struct containing unicast address range, net_idx, etc.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_set_fast_prov_info(esp_ble_mesh_fast_prov_info_t *fast_prov_info);

/**
 * @brief         This function is called to start/suspend/exit fast provisioning.
 *
 * @param[in]     action: fast provisioning action (i.e. enter, suspend, exit).
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_set_fast_prov_action(esp_ble_mesh_fast_prov_action_t action);

#endif /* _ESP_BLE_MESH_PROVISIONING_API_H_ */
