/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_DF_MODEL_H_
#define _BTC_BLE_MESH_DF_MODEL_H_

#include "btc/btc_manage.h"
#include "esp_ble_mesh_df_model_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BTC_BLE_MESH_ACT_DF_CLIENT_GET_STATE,
    BTC_BLE_MESH_ACT_DF_CLIENT_SET_STATE,
    BTC_BLE_MESH_ACT_DF_CLIENT_MAX,
} btc_ble_mesh_df_client_act_t;

typedef union {
    struct {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_df_client_get_t *get;
    } df_get;
    struct {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_df_client_set_t *set;
    } df_set;
} btc_ble_mesh_df_client_args_t;

typedef enum {
    BTC_BLE_MESH_EVT_DF_CLIENT_SEND_TIMEOUT,
    BTC_BLE_MESH_EVT_DF_CLIENT_RECV_GET_RSP,
    BTC_BLE_MESH_EVT_DF_CLIENT_RECV_SET_RSP,
    BTC_BLE_MESH_EVT_DF_CLIENT_RECV_PUB,
    BTC_BLE_MESH_EVT_DF_CLIENT_MAX,
} btc_ble_mesh_df_client_evt_t;

void btc_ble_mesh_df_client_call_handler(btc_msg_t *msg);

void btc_ble_mesh_df_client_cb_handler(btc_msg_t *msg);

void btc_ble_mesh_df_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_ble_mesh_df_client_arg_deep_free(btc_msg_t *msg);

void btc_ble_mesh_df_client_recv_pub_cb(uint32_t opcode,
                                        struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        struct net_buf_simple *buf);

void bt_mesh_df_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                     struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     const uint8_t *val, size_t len);

typedef enum {
    BTC_BLE_MESH_EVT_DF_SERVER_STATE_CHANGE,
    BTC_BLE_MESH_EVT_DF_SERVER_TABLE_CHANGE,
    BTC_BLE_MESH_EVT_DF_SERVER_MAX,
} btc_ble_mesh_df_server_evt_t;

void btc_ble_mesh_df_server_cb_handler(btc_msg_t *msg);

void bt_mesh_df_server_cb_evt_to_btc(uint8_t event,
                                     struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     const uint8_t *val, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_DF_MODEL_H_ */
