/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_NETWORKING_API_H_
#define _ESP_BLE_MESH_NETWORKING_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 *                Node: Before calling this API, the op_pair_size and op_pair variables within
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
 * @brief         De-initialize the user-defined client model.
 *
 * @note          This function shall be invoked before esp_ble_mesh_deinit() is called.
 *
 * @param[in]     model: Pointer of the Client model.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_client_model_deinit(esp_ble_mesh_model_t *model);

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
                                             esp_ble_mesh_msg_ctx_t *ctx,
                                             uint32_t opcode,
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
                                             esp_ble_mesh_msg_ctx_t *ctx,
                                             uint32_t opcode,
                                             uint16_t length, uint8_t *data,
                                             int32_t msg_timeout, bool need_rsp,
                                             esp_ble_mesh_dev_role_t device_role);

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
 *                needs to be re-provisioned. The API function esp_ble_mesh_node_prov_enable()
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
esp_err_t esp_ble_mesh_provisioner_set_node_name(uint16_t index, const char *name);

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
const char *esp_ble_mesh_provisioner_get_node_name(uint16_t index);

/**
 * @brief        This function is called to get the node (provisioned device) index.
 *
 * @param[in]    name: Name of the node (end by '\0').
 *
 * @return       Node index on success, or an invalid value (0xFFFF) on failure.
 *
 */
uint16_t esp_ble_mesh_provisioner_get_node_index(const char *name);

/**
 * @brief        This function is called to store the Composition Data of the node.
 *
 * @param[in]    unicast_addr: Element address of the node
 * @param[in]    data:         Pointer of Composition Data
 * @param[in]    length:       Length of Composition Data
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_store_node_comp_data(uint16_t unicast_addr,
                                                        uint8_t *data, uint16_t length);

/**
 * @brief        This function is called to get the provisioned node information
 *               with the node device uuid.
 *
 * @param[in]    uuid: Device UUID of the node
 *
 * @return       Pointer of the node info struct or NULL on failure.
 *
 */
esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_uuid(const uint8_t uuid[16]);

/**
 * @brief        This function is called to get the provisioned node information
 *               with the node unicast address.
 *
 * @param[in]    unicast_addr: Unicast address of the node
 *
 * @return       Pointer of the node info struct or NULL on failure.
 *
 */
esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_addr(uint16_t unicast_addr);

/**
 * @brief        This function is called to get the provisioned node information
 *               with the node name.
 *
 * @param[in]    name: Name of the node (end by '\0').
 *
 * @return       Pointer of the node info struct or NULL on failure.
 *
 */
esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_name(const char *name);

/**
 * @brief         This function is called by Provisioner to get provisioned node count.
 *
 * @return        Number of the provisioned nodes.
 *
 */
uint16_t esp_ble_mesh_provisioner_get_prov_node_count(void);

/**
 * @brief         This function is called by Provisioner to get the entry of the node table.
 *
 * @note          After invoking the function to get the entry of nodes, users can use the "for"
 *                loop combined with the macro CONFIG_BLE_MESH_MAX_PROV_NODES to get each node's
 *                information. Before trying to read the node's information, users need to check
 *                if the node exists, i.e. if the *(esp_ble_mesh_node_t **node) is NULL.
 *                For example:
 *                ```
 *                const esp_ble_mesh_node_t **entry = esp_ble_mesh_provisioner_get_node_table_entry();
 *                for (int i = 0; i < CONFIG_BLE_MESH_MAX_PROV_NODES; i++) {
 *                    const esp_ble_mesh_node_t *node = entry[i];
 *                    if (node) {
 *                        ......
 *                    }
 *                }
 *                ```
 *
 * @return        Pointer to the start of the node table.
 *
 */
const esp_ble_mesh_node_t **esp_ble_mesh_provisioner_get_node_table_entry(void);

/**
 * @brief        This function is called to delete the provisioned node information
 *               with the node device uuid.
 *
 * @param[in]    uuid: Device UUID of the node
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_node_with_uuid(const uint8_t uuid[16]);

/**
 * @brief        This function is called to delete the provisioned node information
 *               with the node unicast address.
 *
 * @param[in]    unicast_addr: Unicast address of the node
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_node_with_addr(uint16_t unicast_addr);

/**
 * @brief         This function is called to add a local AppKey for Provisioner.
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
esp_err_t esp_ble_mesh_provisioner_add_local_app_key(const uint8_t app_key[16],
                                                     uint16_t net_idx, uint16_t app_idx);

/**
 * @brief         This function is used to update a local AppKey for Provisioner.
 *
 * @param[in]     app_key: Value of the AppKey.
 * @param[in]     net_idx: Corresponding NetKey Index.
 * @param[in]     app_idx: The AppKey Index
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_update_local_app_key(const uint8_t app_key[16],
                                                        uint16_t net_idx, uint16_t app_idx);

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
 * @brief         This function is called by Provisioner to update a local network key.
 *
 * @param[in]     net_key: Value of the NetKey.
 * @param[in]     net_idx: The NetKey Index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_update_local_net_key(const uint8_t net_key[16], uint16_t net_idx);

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
 * @brief         This function is called by Provisioner to enable or disable receiving
 *                heartbeat messages.
 *
 * @note          If enabling receiving heartbeat message successfully, the filter will
 *                be an empty rejectlist by default, which means all heartbeat messages
 *                received by the Provisioner will be reported to the application layer.
 *
 * @param[in]     enable: Enable or disable receiving heartbeat messages.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_recv_heartbeat(bool enable);

/**
 * @brief         This function is called by Provisioner to set the heartbeat filter type.
 *
 * @note          1. If the filter type is not the same with the current value, then all the
 *                   filter entries will be cleaned.
 *                2. If the previous type is rejectlist, and changed to acceptlist, then the
 *                   filter will be an empty acceptlist, which means no heartbeat messages
 *                   will be reported. Users need to add SRC or DST into the filter entry,
 *                   then heartbeat messages from the SRC or to the DST will be reported.
 *
 * @param[in]     type: Heartbeat filter type (acceptlist or rejectlist).
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_heartbeat_filter_type(uint8_t type);

/**
 * @brief         This function is called by Provisioner to add or remove a heartbeat filter entry.
 *
 * @note          1. If the operation is "ADD", the "hb_src" can be set to the SRC (can only be a
 *                   unicast address) of heartbeat messages, and the "hb_dst" can be set to the
 *                   DST (unicast address or group address), at least one of them needs to be set.
 *                   - If only one of them is set, the filter entry will only use the configured
 *                     SRC or DST to filter heartbeat messages.
 *                   - If both of them are set, the SRC and DST will both be used to decide if a
 *                     heartbeat message will be handled.
 *                   - If SRC or DST already exists in some filter entry, then the corresponding
 *                     entry will be cleaned firstly, then a new entry will be allocated to store
 *                     the information.
 *                2. If the operation is "REMOVE", the "hb_src" can be set to the SRC (can only be
 *                   a unicast address) of heartbeat messages, and the "hb_dst" can be set to the
 *                   DST (unicast address or group address), at least one of them needs to be set.
 *                   - The filter entry with the same SRC or DST will be removed.
 *
 * @param[in]     op:   Add or REMOVE
 * @param[in]     info: Heartbeat filter entry information, including:
 *                      hb_src - Heartbeat source address;
 *                      hb_dst - Heartbeat destination address;
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_heartbeat_filter_info(uint8_t op, esp_ble_mesh_heartbeat_filter_info_t *info);

/**
 * @brief         This function is called by Provisioner to directly erase the mesh
 *                information from nvs namespace.
 *
 * @note          This function can be invoked when the mesh stack is not initialized
 *                or has been de-initialized.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_direct_erase_settings(void);

/**
 * @brief         This function is called by Provisioner to open a nvs namespace
 *                for storing mesh information.
 *
 * @note          Before open another nvs namespace, the previously opened nvs
 *                namespace must be closed firstly.
 *
 * @param[in]     index: Settings index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_open_settings_with_index(uint8_t index);

/**
 * @brief         This function is called by Provisioner to open a nvs namespace
 *                for storing mesh information.
 *
 * @note          Before open another nvs namespace, the previously opened nvs
 *                namespace must be closed firstly.
 *
 * @param[in]     uid: Settings user id.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_open_settings_with_uid(const char *uid);

/**
 * @brief         This function is called by Provisioner to close a nvs namespace
 *                which is opened previously for storing mesh information.
 *
 * @note          1. Before closing the nvs namespace, it must be open.
 *                2. When the function is invoked, the Provisioner functionality
 *                   will be disabled firstly, and:
 *                   a) If the "erase" flag is set to false, the mesh information
 *                      will be cleaned (e.g. removing NetKey, AppKey, nodes, etc)
 *                      from the mesh stack.
 *                   b) If the "erase" flag is set to true, the mesh information
 *                      stored in the nvs namespace will also be erased besides
 *                      been cleaned from the mesh stack.
 *                3. If Provisioner tries to work properly again, we can invoke the
 *                   open function to open a new nvs namespace or a previously added
 *                   one, and restore the mesh information from it if not erased.
 *                4. The working process shall be as following:
 *                   a) Open settings A
 *                   b) Start to provision and control nodes
 *                   c) Close settings A
 *                   d) Open settings B
 *                   e) Start to provision and control other nodes
 *                   f) Close settings B
 *                   g) ......
 *
 * @param[in]     index: Settings index.
 * @param[in]     erase: Indicate if erasing mesh information.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_close_settings_with_index(uint8_t index, bool erase);

/**
 * @brief         This function is called by Provisioner to close a nvs namespace
 *                which is opened previously for storing mesh information.
 *
 * @note          1. Before closing the nvs namespace, it must be open.
 *                2. When the function is invoked, the Provisioner functionality
 *                   will be disabled firstly, and:
 *                   a) If the "erase" flag is set to false, the mesh information
 *                      will be cleaned (e.g. removing NetKey, AppKey, nodes, etc)
 *                      from the mesh stack.
 *                   b) If the "erase" flag is set to true, the mesh information
 *                      stored in the nvs namespace will also be erased besides
 *                      been cleaned from the mesh stack.
 *                3. If Provisioner tries to work properly again, we can invoke the
 *                   open function to open a new nvs namespace or a previously added
 *                   one, and restore the mesh information from it if not erased.
 *                4. The working process shall be as following:
 *                   a) Open settings A
 *                   b) Start to provision and control nodes
 *                   c) Close settings A
 *                   d) Open settings B
 *                   e) Start to provision and control other nodes
 *                   f) Close settings B
 *                   g) ......
 *
 * @param[in]     uid: Settings user id.
 * @param[in]     erase: Indicate if erasing mesh information.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_close_settings_with_uid(const char *uid, bool erase);

/**
 * @brief         This function is called by Provisioner to erase the mesh information
 *                and settings user id from a nvs namespace.
 *
 * @note          When this function is called, the nvs namespace must not be open.
 *                This function is used to erase the mesh information and settings
 *                user id which are not used currently.
 *
 * @param[in]     index: Settings index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_settings_with_index(uint8_t index);

/**
 * @brief         This function is called by Provisioner to erase the mesh information
 *                and settings user id from a nvs namespace.
 *
 * @note          When this function is called, the nvs namespace must not be open.
 *                This function is used to erase the mesh information and settings
 *                user id which are not used currently.
 *
 * @param[in]     uid: Settings user id.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_settings_with_uid(const char *uid);

/**
 * @brief         This function is called by Provisioner to get settings user id.
 *
 * @param[in]     index: Settings index.
 *
 * @return        Setting user id on success or NULL on failure.
 *
 */
const char *esp_ble_mesh_provisioner_get_settings_uid(uint8_t index);

/**
 * @brief         This function is called by Provisioner to get settings index.
 *
 * @param[in]     uid: Settings user id.
 *
 * @return        Settings index.
 *
 */
uint8_t esp_ble_mesh_provisioner_get_settings_index(const char *uid);

/**
 * @brief         This function is called by Provisioner to get the number of free
 *                settings user id.
 *
 * @return        Number of free settings user id.
 *
 */
uint8_t esp_ble_mesh_provisioner_get_free_settings_count(void);

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

#if CONFIG_BLE_MESH_CERT_BASED_PROV
/**
 * @brief         This function is called by provisioner to send provisioning records
 *                get message.
 *
 * @param[in]     link_idx:    The provisioning link index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_send_prov_records_get(uint16_t link_idx);

/**
 * @brief         This function is called by provisioner to send provisioning record
 *                request message.
 *
 * @param[in]     link_idx:    The provisioning link index.
 * @param[in]     record_id:   The record identity.
 * @param[in]     frag_offset: The starting offset of the fragment.
 * @param[in]     max_size:    The max record fragment size.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_send_prov_record_req(uint16_t link_idx, uint16_t record_id,
                                                        uint16_t frag_offset, uint16_t max_size);

/**
 * @brief         This function is called by provisioner to send provisioning invite
 *                message.
 *
 * @param[in]     link_idx:  The provisioning link index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_send_prov_invite(uint16_t link_idx);

/**
 * @brief         This function is called by provisioner to send link close
 *
 * @param[in]     link_idx:  The provisioning link index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_send_link_close(uint16_t link_idx);
#endif /* #if CONFIG_BLE_MESH_CERT_BASED_PROV */

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_NETWORKING_API_H_ */
