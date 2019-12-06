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

#ifndef _BTC_BLE_MESH_HEALTH_MODEL_H_
#define _BTC_BLE_MESH_HEALTH_MODEL_H_

#include <stdint.h>
#include "btc/btc_task.h"
#include "esp_ble_mesh_health_model_api.h"

typedef enum {
    BTC_BLE_MESH_ACT_HEALTH_CLIENT_GET_STATE,
    BTC_BLE_MESH_ACT_HEALTH_CLIENT_SET_STATE,
    BTC_BLE_MESH_ACT_HEALTH_CLIENT_MAX,
} btc_ble_mesh_health_client_act_t;

typedef union {
    struct ble_mesh_health_client_get_state_reg_args {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_health_client_get_state_t *get_state;
    } health_client_get_state;
    struct ble_mesh_health_client_set_state_reg_args {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_health_client_set_state_t *set_state;
    } health_client_set_state;
} btc_ble_mesh_health_client_args_t;

typedef enum {
    BTC_BLE_MESH_EVT_HEALTH_CLIENT_GET_STATE,
    BTC_BLE_MESH_EVT_HEALTH_CLIENT_SET_STATE,
    BTC_BLE_MESH_EVT_HEALTH_CLIENT_PUBLISH,
    BTC_BLE_MESH_EVT_HEALTH_CLIENT_TIMEOUT,
    BTC_BLE_MESH_EVT_HEALTH_CLIENT_MAX,
} btc_ble_mesh_health_client_evt_t;

void btc_ble_mesh_health_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_ble_mesh_health_client_call_handler(btc_msg_t *msg);

void btc_ble_mesh_health_client_cb_handler(btc_msg_t *msg);

void btc_ble_mesh_health_publish_callback(u32_t opcode,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf);

void bt_mesh_health_client_cb_evt_to_btc(u32_t opcode, u8_t evt_type,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        const u8_t *val, u16_t len);

typedef enum {
    BTC_BLE_MESH_ACT_HEALTH_SERVER_FAULT_UPDATE,
    BTC_BLE_MESH_ACT_HEALTH_SERVER_MAX,
} btc_ble_mesh_health_server_act_t;

typedef union {
    struct ble_mesh_health_server_fault_update_args {
        esp_ble_mesh_elem_t *element;
    } health_fault_update;
} btc_ble_mesh_health_server_args_t;

void btc_ble_mesh_health_server_call_handler(btc_msg_t *msg);

void btc_ble_mesh_health_server_cb_handler(btc_msg_t *msg);

void btc_ble_mesh_health_server_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_ble_mesh_health_server_fault_clear(struct bt_mesh_model *model, u16_t company_id);

void btc_ble_mesh_health_server_fault_test(struct bt_mesh_model *model, u8_t test_id, u16_t company_id);

void btc_ble_mesh_health_server_attention_on(struct bt_mesh_model *model, u8_t time);

void btc_ble_mesh_health_server_attention_off(struct bt_mesh_model *model);

#endif /* _BTC_BLE_MESH_HEALTH_MODEL_H_ */
