/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_MBT_MODEL_H_
#define _BTC_BLE_MESH_MBT_MODEL_H_

#include "btc/btc_manage.h"
#include "esp_ble_mesh_mbt_model_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BTC_BLE_MESH_ACT_MBT_CLIENT_RETRIEVE_CAPABILITIES,
    BTC_BLE_MESH_ACT_MBT_CLIENT_TRANSFER_BLOB,
    BTC_BLE_MESH_ACT_MBT_CLIENT_SEND_BLOCK,
    BTC_BLE_MESH_ACT_MBT_CLIENT_SEND_DATA,
    BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_BLOCK_STATUS,
    BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS,
    BTC_BLE_MESH_ACT_MBT_CLIENT_CANCEL_TRANSFER,
    BTC_BLE_MESH_ACT_MBT_CLIENT_SET_TRANSFER_TTL,
    BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_TRANSFER_TTL,
    BTC_BLE_MESH_ACT_MBT_CLIENT_SET_APP_IDX,
    BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_APP_IDX,
    BTC_BLE_MESH_ACT_MBT_CLIENT_SET_MULTICAST_ADDR,
    BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_MULTICAST_ADDR,
    BTC_BLE_MESH_ACT_MBT_CLIENT_MAX,
} btc_ble_mesh_mbt_client_act_t;

typedef union {
    esp_ble_mesh_retrieve_capabilities_t retrieve_capabilities;
    esp_ble_mesh_transfer_blob_t transfer_blob;
    esp_ble_mesh_send_block_t send_block;
    esp_ble_mesh_send_data_t send_data;
    esp_ble_mesh_determine_block_status_t determine_block_status;
    esp_ble_mesh_determine_transfer_status_t determine_transfer_status;
    esp_ble_mesh_cancel_transfer_t cancel_transfer;
    struct {
        esp_ble_mesh_model_t *model;
        uint8_t transfer_ttl;
    } set_transfer_ttl;
    struct {
        esp_ble_mesh_model_t *model;
    } clear_transfer_ttl;
    struct {
        esp_ble_mesh_model_t *model;
        uint16_t app_idx;
    } set_app_idx;
    struct {
        esp_ble_mesh_model_t *model;
    } clear_app_idx;
    struct {
        esp_ble_mesh_model_t *model;
        uint16_t multicast_addr;
    } set_multicast_addr;
    struct {
        esp_ble_mesh_model_t *model;
    } clear_multicast_addr;
} btc_ble_mesh_mbt_client_args_t;

#define BTC_BLE_MESH_MBT_CLIENT_RESULT_COMPLETE     0x00
#define BTC_BLE_MESH_MBT_CLIENT_RESULT_FAIL         0x01

typedef enum {
    BTC_BLE_MESH_EVT_MBT_CLIENT_RETRIEVE_CAPABILITIES_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_TRANSFER_BLOB_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_SEND_BLOCK_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_SEND_DATA_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_DETERMINE_BLOCK_STATUS_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_CANCEL_TRANSFER_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_SET_TRANSFER_TTL_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_CLEAR_TRANSFER_TTL_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_SET_APP_IDX_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_CLEAR_APP_IDX_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_SET_MULTICAST_ADDR_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_CLEAR_MULTICAST_ADDR_COMP,
    BTC_BLE_MESH_EVT_MBT_CLIENT_MAX,
} btc_ble_mesh_mbt_client_evt_t;

void btc_ble_mesh_mbt_client_call_handler(btc_msg_t *msg);

void btc_ble_mesh_mbt_client_cb_handler(btc_msg_t *msg);

void btc_ble_mesh_mbt_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_ble_mesh_mbt_client_arg_deep_free(btc_msg_t *msg);

void btc_ble_mesh_mbt_client_publish_callback(uint32_t opcode,
                                              struct bt_mesh_model *model,
                                              struct bt_mesh_msg_ctx *ctx,
                                              struct net_buf_simple *buf);

void bt_mesh_mbt_client_cb_evt_to_btc(uint8_t event, uint8_t result,
                                      struct bt_mesh_model *model);

typedef enum {
    BTC_BLE_MESH_ACT_MBT_SERVER_INITIALIZE_BLOB_RECEIVE,
    BTC_BLE_MESH_ACT_MBT_SERVER_CANCEL_BLOB_RECEIVE,
    BTC_BLE_MESH_ACT_MBT_SERVER_SET_BLOB_CAPABILITIES,
    BTC_BLE_MESH_ACT_MBT_SERVER_MAX,
} btc_ble_mesh_mbt_server_act_t;

typedef union {
    esp_ble_mesh_initialize_blob_receive_t initialize_blob_receive;
    esp_ble_mesh_cancel_blob_receive_t cancel_blob_receive;
    esp_ble_mesh_set_blob_capabilities_t set_blob_capabilities;
} btc_ble_mesh_mbt_server_args_t;

typedef enum {
    BTC_BLE_MESH_EVT_MBT_SERVER_INITIALIZE_BLOB_RECEIVE_COMP,
    BTC_BLE_MESH_EVT_MBT_SERVER_CANCEL_BLOB_RECEIVE_COMP,
    BTC_BLE_MESH_EVT_MBT_SERVER_SET_BLOB_CAPABILITIES_COMP,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_GET,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_START,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_CANCEL,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_BLOCK_GET,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_BLOCK_START,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_CHUNK_TRANSFER,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_INFORMATION_GET,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOCK_RECEIVE_COMP,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_RECEIVE_COMP,
    BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_RECEIVE_TIMEOUT,
    BTC_BLE_MESH_EVT_MBT_SERVER_MAX,
} btc_ble_mesh_mbt_server_evt_t;

void btc_ble_mesh_mbt_server_call_handler(btc_msg_t *msg);

void btc_ble_mesh_mbt_server_cb_handler(btc_msg_t *msg);

void bt_mesh_mbt_server_cb_evt_to_btc(uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx);

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_MBT_MODEL_H_ */
