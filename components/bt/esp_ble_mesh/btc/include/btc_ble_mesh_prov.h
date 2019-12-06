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

#ifndef _BTC_BLE_MESH_PROV_H_
#define _BTC_BLE_MESH_PROV_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "btc/btc_task.h"

#include "mesh_access.h"
#include "mesh_buf.h"
#include "mesh_main.h"
#include "provisioner_prov.h"
#include "esp_ble_mesh_defs.h"

typedef enum {
    BTC_BLE_MESH_ACT_MESH_INIT = 0,
    BTC_BLE_MESH_ACT_PROV_ENABLE,
    BTC_BLE_MESH_ACT_PROV_DISABLE,
    BTC_BLE_MESH_ACT_NODE_RESET,
    BTC_BLE_MESH_ACT_SET_OOB_PUB_KEY,
    BTC_BLE_MESH_ACT_INPUT_NUMBER,
    BTC_BLE_MESH_ACT_INPUT_STRING,
    BTC_BLE_MESH_ACT_SET_DEVICE_NAME,
    BTC_BLE_MESH_ACT_PROXY_IDENTITY_ENABLE,
    BTC_BLE_MESH_ACT_PROXY_GATT_ENABLE,
    BTC_BLE_MESH_ACT_PROXY_GATT_DISABLE,
    BTC_BLE_MESH_ACT_PROVISIONER_READ_OOB_PUB_KEY,
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
    BTC_BLE_MESH_ACT_SET_FAST_PROV_INFO,
    BTC_BLE_MESH_ACT_SET_FAST_PROV_ACTION,
    BTC_BLE_MESH_ACT_LPN_ENABLE,
    BTC_BLE_MESH_ACT_LPN_DISABLE,
    BTC_BLE_MESH_ACT_LPN_POLL,
    BTC_BLE_MESH_ACT_PROXY_CLIENT_CONNECT,
    BTC_BLE_MESH_ACT_PROXY_CLIENT_DISCONNECT,
    BTC_BLE_MESH_ACT_PROXY_CLIENT_SET_FILTER_TYPE,
    BTC_BLE_MESH_ACT_PROXY_CLIENT_ADD_FILTER_ADDR,
    BTC_BLE_MESH_ACT_PROXY_CLIENT_REMOVE_FILTER_ADDR,
} btc_ble_mesh_prov_act_t;

typedef enum {
    BTC_BLE_MESH_ACT_MODEL_PUBLISH,
    BTC_BLE_MESH_ACT_SERVER_MODEL_SEND,
    BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND,
    BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE,
} btc_ble_mesh_model_act_t;

typedef union {
    struct ble_mesh_init_args {
        esp_ble_mesh_prov_t *prov;
        esp_ble_mesh_comp_t *comp;
        SemaphoreHandle_t semaphore;
    } mesh_init;
    struct ble_mesh_node_prov_enable_args {
        esp_ble_mesh_prov_bearer_t bearers;
    } node_prov_enable;
    struct ble_mesh_node_prov_disable_args {
        esp_ble_mesh_prov_bearer_t bearers;
    } node_prov_disable;
    struct ble_mesh_set_oob_pub_key_args {
        uint8_t pub_key_x[32];
        uint8_t pub_key_y[32];
        uint8_t private_key[32];
    } set_oob_pub_key;
    struct ble_mesh_node_input_num_args {
        uint32_t number;
    } input_number;
    struct ble_mesh_node_input_str_args {
        char string[8];
    } input_string;
    struct ble_mesh_set_device_name_args {
        char name[ESP_BLE_MESH_DEVICE_NAME_MAX_LEN];
    } set_device_name;
    struct ble_mesh_provisioner_read_oob_pub_key_args {
        uint8_t link_idx;
        uint8_t pub_key_x[32];
        uint8_t pub_key_y[32];
    } provisioner_read_oob_pub_key;
    struct ble_mesh_provisioner_input_str_args {
        char string[8];
        uint8_t link_idx;
    } provisioner_input_str;
    struct ble_mesh_provisioner_input_num_args {
        uint32_t number;
        uint8_t  link_idx;
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
    struct ble_mesh_set_fast_prov_info_args {
        uint16_t unicast_min;
        uint16_t unicast_max;
        uint16_t net_idx;
        uint8_t  flags;
        uint32_t iv_index;
        uint8_t  offset;
        uint8_t  match_len;
        uint8_t  match_val[16];
    } set_fast_prov_info;
    struct ble_mesh_set_fast_prov_action_args {
        uint8_t action;
    } set_fast_prov_action;
    struct ble_mesh_lpn_enable_args {
        /* RFU */
    } lpn_enable;
    struct ble_mesh_lpn_disable_args {
        bool force;
    } lpn_disable;
    struct ble_mesh_lpn_poll_args {
        /* RFU */
    } lpn_poll;
    struct ble_mesh_proxy_client_connect_args {
        uint8_t  addr[6];
        uint8_t  addr_type;
        uint16_t net_idx;
    } proxy_client_connect;
    struct ble_mesh_proxy_client_disconnect_args {
        uint8_t conn_handle;
    } proxy_client_disconnect;
    struct ble_mesh_proxy_client_set_filter_type_args {
        uint8_t  conn_handle;
        uint16_t net_idx;
        uint8_t  filter_type;
    } proxy_client_set_filter_type;
    struct ble_mesh_proxy_client_add_filter_addr_args {
        uint8_t   conn_handle;
        uint16_t  net_idx;
        uint16_t  addr_num;
        uint16_t *addr;
    } proxy_client_add_filter_addr;
    struct ble_mesh_proxy_client_remove_filter_addr_args {
        uint8_t   conn_handle;
        uint16_t  net_idx;
        uint16_t  addr_num;
        uint16_t *addr;
    } proxy_client_remove_filter_addr;
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
    struct ble_mesh_server_model_update_state_args {
        esp_ble_mesh_model_t *model;
        esp_ble_mesh_server_state_type_t type;
        esp_ble_mesh_server_state_value_t *value;
    } model_update_state;
} btc_ble_mesh_model_args_t;

void btc_ble_mesh_prov_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_ble_mesh_model_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

int btc_ble_mesh_client_model_init(esp_ble_mesh_model_t *model);

int32_t btc_ble_mesh_model_pub_period_get(esp_ble_mesh_model_t *mod);

uint16_t btc_ble_mesh_get_primary_addr(void);

uint16_t *btc_ble_mesh_model_find_group(esp_ble_mesh_model_t *mod, uint16_t addr);

esp_ble_mesh_elem_t *btc_ble_mesh_elem_find(u16_t addr);

uint8_t btc_ble_mesh_elem_count(void);

esp_ble_mesh_model_t *btc_ble_mesh_model_find_vnd(const esp_ble_mesh_elem_t *elem,
        uint16_t company, uint16_t id);

esp_ble_mesh_model_t *btc_ble_mesh_model_find(const esp_ble_mesh_elem_t *elem,
        uint16_t id);

const esp_ble_mesh_comp_t *btc_ble_mesh_comp_get(void);

void btc_ble_mesh_model_call_handler(btc_msg_t *msg);
void btc_ble_mesh_model_cb_handler(btc_msg_t *msg);

void btc_ble_mesh_prov_call_handler(btc_msg_t *msg);
void btc_ble_mesh_prov_cb_handler(btc_msg_t *msg);

#endif /* _BTC_BLE_MESH_PROV_H_ */
