// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _BTC_BLE_MESH_PROV_H_
#define _BTC_BLE_MESH_PROV_H_

#include "btc/btc_task.h"
#include "esp_bt_defs.h"

#include "mesh_access.h"
#include "mesh_buf.h"
#include "mesh_main.h"
#include "esp_ble_mesh_defs.h"

#if CONFIG_BT_MESH

typedef enum {
    BTC_BLE_MESH_ACT_APP_REGISTER = 0,
    BTC_BLE_MESH_ACT_PROV_ENABLE,
    BTC_BLE_MESH_ACT_PROV_DISABLE,
    BTC_BLE_MESH_ACT_NODE_RESET,
    BTC_BLE_MESH_ACT_INPUT_NUMBER,
    BTC_BLE_MESH_ACT_INPUT_STR,
    BTC_BLE_MESH_ACT_SET_DEVICE_NAME,
    BTC_BLE_MESH_ACT_PROXY_IDENTITY_ENABLE,
    BTC_BLE_MESH_ACT_PROXY_GATT_ENABLE,
    BTC_BLE_MESH_ACT_PROXY_GATT_DISABLE,
#if (CONFIG_BT_MESH_PROVISIONER)
    BTC_BLE_MESH_ACT_PROVISIONER_INPUT_STR,
    BTC_BLE_MESH_ACT_PROVISIONER_INPUT_NUM,
    BTC_BLE_MESH_ACT_PROVISIONER_ENABLE,
    BTC_BLE_MESH_ACT_PROVISIONER_DISABLE,
    BTC_BLE_MESH_ACT_PROVISIONER_DEV_ADD,
    BTC_BLE_MESH_ACT_PROVISIONER_DEV_DEL,
    BTC_BLE_MESH_ACT_PROVISIONER_SET_DEV_UUID_MATCH,
    BTC_BLE_MESH_ACT_PROVISIONER_SET_PROV_DATA_INFO,
    BTC_BLE_MESH_ACT_PROVISIONER_SET_NODE_NAME,
    BTC_BLE_MESH_ACT_PROVISIONER_SET_LOCAL_APP_KEY,
    BTC_BLE_MESH_ACT_PROVISIONER_BIND_LOCAL_MOD_APP,
    BTC_BLE_MESH_ACT_PROVISIONER_ADD_LOCAL_NET_KEY,
#endif /* CONFIG_BT_MESH_PROVISIONER */
} btc_ble_mesh_prov_act_t;

typedef enum {
    BTC_BLE_MESH_ACT_MODEL_PUBLISH,
    BTC_BLE_MESH_ACT_SERVER_MODEL_SEND,
    BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND
} btc_ble_mesh_model_act_t;

typedef struct {
    /* Message handler for the opcode */
    void (*const func)(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf);
    /** @brief Output of a number is requested.
     *
     *  This callback notifies the application that it should
     *  output the given number using the given action.
     *
     *  @param act Action for outputting the number.
     *  @param num Number to be outputted.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int (*output_number)(bt_mesh_output_action_t act, u32_t num);

    /** @brief Output of a string is requested.
     *
     *  This callback notifies the application that it should
     *  display the given string to the user.
     *
     *  @param str String to be displayed.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int (*output_string)(const char *str);

    /** @brief Input is requested.
     *
     *  This callback notifies the application that it should
     *  request input from the user using the given action. The
     *  requested input will either be a string or a number, and
     *  the application needs to consequently call the
     *  bt_mesh_input_string() or bt_mesh_input_number() functions
     *  once the data has been acquired from the user.
     *
     *  @param act Action for inputting data.
     *  @param num Maximum size of the inputted data.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int (*input)(bt_mesh_input_action_t act, u8_t size);

    /** @brief Provisioning link has been opened.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void (*link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning link has been closed.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void (*link_close)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning is complete.
     *
     *  This callback notifies the application that provisioning has
     *  been successfully completed, and that the local node has been
     *  assigned the specified NetKeyIndex and primary element address.
     *
     *  @param net_idx NetKeyIndex given during provisioning.
     *  @param addr Primary element address.
     */
    void (*complete)(u16_t net_idx, u16_t addr);

    /** @brief Node has been reset.
     *
     *  This callback notifies the application that the local node
     *  has been reset and needs to be reprovisioned. The node will
     *  not automatically advertise as unprovisioned, rather the
     *  bt_mesh_prov_enable() API needs to be called to enable
     *  unprovisioned advertising on one or more provisioning bearers.
     */
    void (*reset)(void);
} btc_mesh_cb_t;

typedef void (*const func_cb)(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf);

typedef int (*output_number_cb)(bt_mesh_output_action_t act, u32_t num);

typedef int (*output_string_cb)(const char *str);

typedef int (*input_cb)(bt_mesh_input_action_t act, u8_t size);

typedef void (*link_open_cb)(bt_mesh_prov_bearer_t bearer);

typedef void (*link_close_cb)(bt_mesh_prov_bearer_t bearer);

typedef void (*complete_cb)(u16_t net_idx, u16_t addr);

typedef void (*reset_cb)(void);

typedef union {
    struct ble_mesh_reg_args {
        esp_ble_mesh_prov_t *prov;
        esp_ble_mesh_comp_t *comp;
        uint8_t mesh_if;
    } mesh_reg;
    struct ble_mesh_prov_enable_args {
        esp_ble_mesh_prov_bearer_t bearers;
    } mesh_prov_enable;
    struct ble_mesh_prov_disable_args {
        esp_ble_mesh_prov_bearer_t bearers;
    } mesh_prov_disable;
    struct ble_mesh_input_num_args {
        uint32_t num;
    } input_num;
    struct ble_mesh_input_str_args {
        char *str;
    } input_str;
    struct ble_mesh_set_device_name_args {
        char name[ESP_BLE_MESH_DEVICE_NAME_MAX_LEN];
    } set_device_name;
#if (CONFIG_BT_MESH_PROVISIONER)
    struct ble_mesh_provisioner_input_str_args {
        const char *str;
    } provisioner_input_str;
    struct ble_mesh_provisioner_input_num_args {
        uint32_t num;
    } provisioner_input_num;
    struct ble_mesh_provisioner_enable_args {
        esp_ble_mesh_prov_bearer_t bearers;
    } provisioner_enable;
    struct ble_mesh_provisioner_disable_args {
        esp_ble_mesh_prov_bearer_t bearers;
    } provisioner_disable;
    struct ble_mesh_provisioner_dev_add_args {
        esp_ble_mesh_unprov_dev_add_t add_dev;
        esp_ble_mesh_dev_add_flag_t   flags;
    } provisioner_dev_add;
    struct ble_mesh_provisioner_dev_del_args {
        esp_ble_mesh_device_delete_t del_dev;
    } provisioner_dev_del;
    struct ble_mesh_provisioner_set_dev_uuid_match_args {
        uint8_t offset;
        uint8_t match_len;
        uint8_t match_val[16];
        bool prov_after_match;
    } set_dev_uuid_match;
    struct ble_mesh_provisioner_set_prov_net_idx_args {
        esp_ble_mesh_prov_data_info_t prov_data;
    } set_prov_data_info;
    struct ble_mesh_provisioner_set_node_name_args {
        int index;
        char name[ESP_BLE_MESH_NODE_NAME_MAX_LEN];
    } set_node_name;
    struct ble_mesh_provisioner_add_local_app_key_args {
        uint8_t app_key[16];
        uint16_t net_idx;
        uint16_t app_idx;
    } add_local_app_key;
    struct ble_mesh_provisioner_bind_local_mod_app_args {
        uint16_t elem_addr;
        uint16_t model_id;
        uint16_t cid;
        uint16_t app_idx;
    } local_mod_app_bind;
    struct ble_mesh_provisioner_add_local_net_key_args {
        uint8_t net_key[16];
        uint16_t net_idx;
    } add_local_net_key;
#endif /* CONFIG_BT_MESH_PROVISIONER */
} btc_ble_mesh_prov_args_t;

typedef union {
    struct ble_mesh_model_publish_args {
        esp_ble_mesh_model_t *model;
        uint8_t device_role;
    } model_publish;
    struct ble_mesh_model_send_args {
        esp_ble_mesh_model_t *model;
        esp_ble_mesh_msg_ctx_t *ctx;
        uint32_t opcode;
        bool need_rsp;
        uint16_t length;
        uint8_t *data;
        uint8_t device_role;
        int32_t msg_timeout;
    } model_send;
} btc_ble_mesh_model_args_t;

void btc_ble_mesh_prov_arg_deep_free(btc_msg_t *msg);

void btc_ble_mesh_prov_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

int btc_ble_mesh_client_init(esp_ble_mesh_model_t *model);

int32_t btc_ble_mesh_model_pub_period_get(esp_ble_mesh_model_t *mod);

uint16_t btc_ble_mesh_get_primary_addr(void);

uint16_t *btc_ble_mesh_model_find_group(esp_ble_mesh_model_t *mod, uint16_t addr);

esp_ble_mesh_elem_t *btc_ble_mesh_elem_find(u16_t addr);

uint8_t btc_ble_mesh_elem_count(void);

esp_ble_mesh_model_t *btc_ble_mesh_model_find_vnd(esp_ble_mesh_elem_t *elem,
        uint16_t company, uint16_t id);

esp_ble_mesh_model_t *btc_ble_mesh_model_find(esp_ble_mesh_elem_t *elem,
        u16_t id);

const esp_ble_mesh_comp_t *btc_ble_mesh_comp_get(void);

const btc_mesh_cb_t *btc_mesh_prov_get_interface(void);

void btc_mesh_model_call_handler(btc_msg_t *msg);
void btc_mesh_model_cb_handler(btc_msg_t *msg);

void btc_mesh_prov_call_handler(btc_msg_t *msg);

void btc_mesh_prov_cb_handler(btc_msg_t *msg);

#endif /* _BTC_BLE_MESH_PROV_H_ */
#endif /* #if CONFIG_BT_MESH */

