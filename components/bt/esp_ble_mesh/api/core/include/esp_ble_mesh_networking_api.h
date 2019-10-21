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

#ifndef _ESP_BLE_MESH_NETWORKING_API_H_
#define _ESP_BLE_MESH_NETWORKING_API_H_

#include "esp_ble_mesh_defs.h"

/** @brief: event, event code of user-defined model events; param, parameters of user-defined model events */
typedef void (* esp_ble_mesh_model_cb_t)(esp_ble_mesh_model_cb_event_t event,
        esp_ble_mesh_model_cb_param_t *param);

/**
 * @brief         Register BLE Mesh callback for user-defined models' operations.
 *                This callback can report the following events generated for the user-defined models:
 *                - Call back the messages received by user-defined client and server models to the
 *                  application layer;
 *                - If users call esp_ble_mesh_server/client_model_send, this callback notifies the
 *                  application layer of the send_complete event;
 *                - If user-defined client model sends a message that requires response, and the response
 *                  message is received after the timer expires, the response message will be reported
 *                  to the application layer as published by a peer device;
 *                - If the user-defined client model fails to receive the response message during a specified
 *                  period of time, a timeout event will be reported to the application layer.
 *
 * @note          The client models (i.e. Config Client model, Health Client model, Generic
 *                Client models, Sensor Client model, Scene Client model and Lighting Client models)
 *                that have been realized internally have their specific register functions.
 *                For example, esp_ble_mesh_register_config_client_callback is the register
 *                function for Config Client Model.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb_t callback);

/**
 * @brief        Add the message opcode to the beginning of the model message
 *               before sending or publishing the model message.
 *
 * @note         This API is only used to set the opcode of the message.
 *
 * @param[in]    data: Pointer to the message data.
 * @param[in]    opcode: The message opcode.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_model_msg_opcode_init(uint8_t *data, uint32_t opcode);

/**
 * @brief         Initialize the user-defined client model. All user-defined client models
 *                shall call this function to initialize the client model internal data.
 *                Node: Before calling this API, the op_pair_size and op_pair variabled within
 *                      the user_data(defined using esp_ble_mesh_client_t_) of the client model
 *                      need to be initialized.
 *
 * @param[in]     model: BLE Mesh Client model to which the message belongs.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_client_model_init(esp_ble_mesh_model_t *model);

/**
 * @brief         Send server model messages(such as server model status messages).
 *
 * @param[in]     model: BLE Mesh Server Model to which the message belongs.
 * @param[in]     ctx:   Message context, includes keys, TTL, etc.
 * @param[in]     opcode: Message opcode.
 * @param[in]     length: Message length (exclude the message opcode).
 * @param[in]     data: Parameters of Access Payload (exclude the message opcode) to be sent.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_server_model_send_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx, uint32_t opcode,
        uint16_t length, uint8_t *data);

/**
 * @brief         Send client model message (such as model get, set, etc).
 *
 * @param[in]     model: BLE Mesh Client Model to which the message belongs.
 * @param[in]     ctx:   Message context, includes keys, TTL, etc.
 * @param[in]     opcode: Message opcode.
 * @param[in]     length: Message length (exclude the message opcode).
 * @param[in]     data: Parameters of the Access Payload (exclude the message opcode) to be sent.
 * @param[in]     msg_timeout: Time to get response to the message (in milliseconds).
 * @param[in]     need_rsp: TRUE if the opcode requires the peer device to reply, FALSE otherwise.
 * @param[in]     device_role: Role of the device (Node/Provisioner) that sends the message.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_client_model_send_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx, uint32_t opcode,
        uint16_t length, uint8_t *data, int32_t msg_timeout,
        bool need_rsp, esp_ble_mesh_dev_role_t device_role);

/**
 * @brief         Send a model publication message.
 *
 * @note          Before calling this function, the user needs to ensure that the model
 *                publication message (@ref esp_ble_mesh_model_pub_t.msg) contains a valid
 *                message to be sent. And if users want to update the publishing message,
 *                this API should be called in ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT
 *                with the message updated.
 *
 *
 * @param[in]    model: Mesh (client) Model publishing the message.
 * @param[in]    opcode: Message opcode.
 * @param[in]    length: Message length (exclude the message opcode).
 * @param[in]    data: Parameters of the Access Payload (exclude the message opcode) to be sent.
 * @param[in]    device_role: Role of the device (node/provisioner) publishing the message of the type esp_ble_mesh_dev_role_t.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_model_publish(esp_ble_mesh_model_t *model, uint32_t opcode,
                                     uint16_t length, uint8_t *data,
                                     esp_ble_mesh_dev_role_t device_role);

/**
 * @brief        Update a server model state value. If the model publication
 *               state is set properly (e.g. publish address is set to a valid
 *               address), it will publish corresponding status message.
 *
 * @note         Currently this API is used to update bound state value, not
 *               for all server model states.
 *
 * @param[in]    model: Server model which is going to update the state.
 * @param[in]    type:  Server model state type.
 * @param[in]    value: Server model state value.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_server_model_update_state(esp_ble_mesh_model_t *model,
        esp_ble_mesh_server_state_type_t type,
        esp_ble_mesh_server_state_value_t *value);

/**
 * @brief         Reset the provisioning procedure of the local BLE Mesh node.
 *
 * @note          All provisioning information in this node will be deleted and the node
 *                needs to be reprovisioned. The API function esp_ble_mesh_node_prov_enable()
 *                needs to be called to start a new provisioning procedure.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_local_reset(void);

/**
 * @brief        This function is called to set the node (provisioned device) name.
 *
 * @param[in]    index: Index of the node in the node queue.
 * @param[in]    name: Name (end by '\0') to be set for the node.
 *
 * @note         index is obtained from the parameters of ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_node_name(int index, const char *name);

/**
 * @brief        This function is called to get the node (provisioned device) name.
 *
 * @param[in]    index: Index of the node in the node queue.
 *
 * @note         index is obtained from the parameters of ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT.
 *
 * @return       Node name on success, or NULL on failure.
 *
 */
const char *esp_ble_mesh_provisioner_get_node_name(int index);

/**
 * @brief        This function is called to get the node (provisioned device) index.
 *
 * @param[in]    name: Name of the node (end by '\0').
 *
 * @return       Node index on success, or (negative) error code from errno.h on failure.
 *
 */
int esp_ble_mesh_provisioner_get_node_index(const char *name);

/**
 * @brief         This function is called to set the app key for the local BLE Mesh stack.
 *
 * @param[in]     app_key: The app key to be set for the local BLE Mesh stack.
 * @param[in]     net_idx: The network key index.
 * @param[in]     app_idx: The app key index.
 *
 * @note          app_key: If set to NULL, app_key will be generated internally.
 *                net_idx: Should be an existing one.
 *                app_idx: If it is going to be generated internally, it should be set to
 *                         0xFFFF, and the new app_idx will be reported via an event.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_add_local_app_key(const uint8_t app_key[16], uint16_t net_idx, uint16_t app_idx);

/**
 * @brief         This function is called by Provisioner to get the local app key value.
 *
 * @param[in]     net_idx: Network key index.
 * @param[in]     app_idx: Application key index.
 *
 * @return        App key on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_provisioner_get_local_app_key(uint16_t net_idx, uint16_t app_idx);

/**
 * @brief         This function is called by Provisioner to bind own model with proper app key.
 *
 * @param[in]     element_addr: Provisioner local element address
 * @param[in]     app_idx: Provisioner local appkey index
 * @param[in]     model_id: Provisioner local model id
 * @param[in]     company_id: Provisioner local company id
 *
 * @note          company_id: If going to bind app_key with local vendor model, company_id
 *                            should be set to 0xFFFF.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_bind_app_key_to_local_model(uint16_t element_addr, uint16_t app_idx,
        uint16_t model_id, uint16_t company_id);

/**
 * @brief         This function is called by Provisioner to add local network key.
 *
 * @param[in]     net_key: The network key to be added to the Provisioner local BLE Mesh stack.
 * @param[in]     net_idx: The network key index.
 *
 * @note          net_key: If set to NULL, net_key will be generated internally.
 *                net_idx: If it is going to be generated internally, it should be set to
 *                         0xFFFF, and the new net_idx will be reported via an event.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_add_local_net_key(const uint8_t net_key[16], uint16_t net_idx);

/**
 * @brief         This function is called by Provisioner to get the local network key value.
 *
 * @param[in]     net_idx: Network key index.
 *
 * @return        Network key on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_provisioner_get_local_net_key(uint16_t net_idx);

/**
 * @brief         This function is called to get fast provisioning application key.
 *
 * @param[in]     net_idx: Network key index.
 * @param[in]     app_idx: Application key index.
 *
 * @return        Application key on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_get_fast_prov_app_key(uint16_t net_idx, uint16_t app_idx);

#endif /* _ESP_BLE_MESH_NETWORKING_API_H_ */
