/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_RPR_MODEL_H_
#define _BTC_BLE_MESH_RPR_MODEL_H_

#include "btc/btc_manage.h"
#include "esp_ble_mesh_rpr_model_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BTC_BLE_MESH_ACT_RPR_CLIENT_SEND,
    BTC_BLE_MESH_ACT_RPR_CLIENT_ACT,
    BTC_BLE_MESH_ACT_RPR_CLIENT_MAX,
} btc_ble_mesh_rpr_client_act_t;

typedef union {
    struct {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_rpr_client_msg_t *msg;
    } rpr_send;
    struct {
        esp_ble_mesh_rpr_client_act_type_t type;
        esp_ble_mesh_rpr_client_act_param_t *param;
    } rpr_act;
} btc_ble_mesh_rpr_client_args_t;

typedef enum {
    BTC_BLE_MESH_EVT_RPR_CLIENT_SEND_TIMEOUT,
    BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_RSP,
    BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_PUB,
    BTC_BLE_MESH_EVT_RPR_CLIENT_MAX,
} btc_ble_mesh_rpr_client_evt_t;

void btc_ble_mesh_rpr_client_call_handler(btc_msg_t *msg);

void btc_ble_mesh_rpr_client_cb_handler(btc_msg_t *msg);

void btc_ble_mesh_rpr_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_ble_mesh_rpr_client_arg_deep_free(btc_msg_t *msg);

void btc_ble_mesh_rpr_client_recv_pub_cb(uint32_t opcode,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         struct net_buf_simple *buf);

void btc_ble_mesh_rpr_client_link_close_cb(struct bt_mesh_model *model,
                                           uint16_t rpr_srv_addr, uint8_t reason);

void btc_ble_mesh_rpr_client_prov_comp_cb(struct bt_mesh_model *model, uint16_t rpr_srv_addr,
                                          uint8_t nppi, uint16_t index, uint8_t uuid[16],
                                          uint16_t unicast_addr, uint8_t element_num,
                                          uint16_t net_idx);

void bt_mesh_rpr_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const void *val, size_t len);

typedef enum {
    BTC_BLE_MESH_EVT_RPR_SERVER_SCAN_START,
    BTC_BLE_MESH_EVT_RPR_SERVER_SCAN_STOP,
    BTC_BLE_MESH_EVT_RPR_SERVER_EXT_SCAN_START,
    BTC_BLE_MESH_EVT_RPR_SERVER_EXT_SCAN_STOP,
    BTC_BLE_MESH_EVT_RPR_SERVER_LINK_OPEN,
    BTC_BLE_MESH_EVT_RPR_SERVER_LINK_CLOSE,
    BTC_BLE_MESH_EVT_RPR_SERVER_PROV_COMP,
    BTC_BLE_MESH_EVT_RPR_SERVER_MAX,
} btc_ble_mesh_rpr_server_evt_t;

void btc_ble_mesh_rpr_server_cb_handler(btc_msg_t *msg);

void bt_mesh_rpr_server_cb_evt_to_btc(uint8_t event, const void *val, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_RPR_MODEL_H_ */
