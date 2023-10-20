/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_TIME_SCENE_MODEL_H_
#define _BTC_BLE_MESH_TIME_SCENE_MODEL_H_

#include "btc/btc_manage.h"
#include "esp_ble_mesh_time_scene_model_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_GET_STATE,
    BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_SET_STATE,
    BTC_BLE_MESH_ACT_TIME_SCENE_CLIENT_MAX,
} btc_ble_mesh_time_scene_client_act_t;

typedef union {
    struct ble_mesh_time_scene_client_get_state_reg_args {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_time_scene_client_get_state_t *get_state;
    } time_scene_client_get_state;
    struct ble_mesh_time_scene_client_set_state_reg_args {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_time_scene_client_set_state_t *set_state;
    } time_scene_client_set_state;
} btc_ble_mesh_time_scene_client_args_t;

typedef enum {
    BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_GET_STATE,
    BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_SET_STATE,
    BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_PUBLISH,
    BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_TIMEOUT,
    BTC_BLE_MESH_EVT_TIME_SCENE_CLIENT_MAX,
} btc_ble_mesh_time_scene_client_evt_t;

void btc_ble_mesh_time_scene_client_call_handler(btc_msg_t *msg);

void btc_ble_mesh_time_scene_client_cb_handler(btc_msg_t *msg);

void btc_ble_mesh_time_scene_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_ble_mesh_time_scene_client_arg_deep_free(btc_msg_t *msg);

void btc_ble_mesh_time_scene_client_publish_callback(uint32_t opcode, struct bt_mesh_model *model,
                                                     struct bt_mesh_msg_ctx *ctx,
                                                     struct net_buf_simple *buf);

void bt_mesh_time_scene_client_cb_evt_to_btc(uint32_t opcode, uint8_t evt_type,
                                             struct bt_mesh_model *model,
                                             struct bt_mesh_msg_ctx *ctx,
                                             const uint8_t *val, size_t len);

typedef enum {
    BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE,
    BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_GET_MSG,
    BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_SET_MSG,
    BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_RECV_STATUS_MSG,
    BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_MAX,
} btc_ble_mesh_time_scene_server_evt_t;

void bt_mesh_time_scene_server_cb_evt_to_btc(uint8_t evt_type, struct bt_mesh_model *model,
                                             struct bt_mesh_msg_ctx *ctx,
                                             const uint8_t *val, size_t len);

void btc_ble_mesh_time_scene_server_cb_handler(btc_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_TIME_SCENE_MODEL_H_ */
