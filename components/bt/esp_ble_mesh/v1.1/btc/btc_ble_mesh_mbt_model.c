/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_mbt_model.h"
#include "esp_ble_mesh_mbt_model_api.h"

#if CONFIG_BLE_MESH_MBT_CLI

extern int bt_mesh_retrieve_capabilities(void *input);
extern int bt_mesh_transfer_blob(void *input);
extern int bt_mesh_send_block(void *input);
extern int bt_mesh_send_data(void *input);
extern int bt_mesh_determine_block_status(void *input);
extern int bt_mesh_determine_transfer_status(void *input);
extern int bt_mesh_cancel_transfer(void *input);
extern int bt_mesh_set_transfer_ttl_state(void *model, uint8_t transfer_ttl);
extern int bt_mesh_clear_transfer_ttl_state(void *model);
extern int bt_mesh_set_app_idx_state(void *model, uint16_t app_idx);
extern int bt_mesh_clear_app_idx_state(void *model);
extern int bt_mesh_set_multicast_addr_state(void *model, uint16_t multicast_addr);
extern int bt_mesh_clear_multicast_addr_state(void *model);
extern int bt_mesh_initialize_blob_receive(void *input);
extern int bt_mesh_cancel_blob_receive(void *input);
extern int bt_mesh_set_blob_capabilities(void *model, void *input);

static inline void btc_ble_mesh_blob_trans_client_cb_to_app(esp_ble_mesh_mbt_client_cb_event_t event,
                                                            esp_ble_mesh_mbt_client_cb_param_t *param)
{
    esp_ble_mesh_mbt_client_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_mbt_client_cb_t)btc_profile_cb_get(BTC_PID_MBT_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_mbt_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_mbt_client_args_t *dst = (btc_ble_mesh_mbt_client_args_t *)p_dest;
    btc_ble_mesh_mbt_client_args_t *src = (btc_ble_mesh_mbt_client_args_t *)p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MBT_CLIENT_RETRIEVE_CAPABILITIES:
        if (src->retrieve_capabilities.unicast_addr_count && src->retrieve_capabilities.unicast_addr) {
            dst->retrieve_capabilities.unicast_addr = bt_mesh_calloc(src->retrieve_capabilities.unicast_addr_count * 2);
            if (dst->retrieve_capabilities.unicast_addr) {
                memcpy(dst->retrieve_capabilities.unicast_addr, src->retrieve_capabilities.unicast_addr,
                       src->retrieve_capabilities.unicast_addr_count * 2);
                dst->retrieve_capabilities.unicast_addr_count = src->retrieve_capabilities.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_TRANSFER_BLOB:
        if (src->transfer_blob.unicast_addr_count && src->transfer_blob.unicast_addr) {
            dst->transfer_blob.unicast_addr = bt_mesh_calloc(src->transfer_blob.unicast_addr_count * 2);
            if (dst->transfer_blob.unicast_addr) {
                memcpy(dst->transfer_blob.unicast_addr, src->transfer_blob.unicast_addr,
                       src->transfer_blob.unicast_addr_count * 2);
                dst->transfer_blob.unicast_addr_count = src->transfer_blob.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS:
        if (src->determine_transfer_status.unicast_addr_count && src->determine_transfer_status.unicast_addr) {
            dst->determine_transfer_status.unicast_addr = bt_mesh_calloc(src->determine_transfer_status.unicast_addr_count * 2);
            if (dst->determine_transfer_status.unicast_addr) {
                memcpy(dst->determine_transfer_status.unicast_addr, src->determine_transfer_status.unicast_addr,
                       src->determine_transfer_status.unicast_addr_count * 2);
                dst->determine_transfer_status.unicast_addr_count = src->determine_transfer_status.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CANCEL_TRANSFER:
        if (src->cancel_transfer.unicast_addr_count && src->cancel_transfer.unicast_addr) {
            dst->cancel_transfer.unicast_addr = bt_mesh_calloc(src->cancel_transfer.unicast_addr_count * 2);
            if (dst->cancel_transfer.unicast_addr) {
                memcpy(dst->cancel_transfer.unicast_addr, src->cancel_transfer.unicast_addr,
                       src->cancel_transfer.unicast_addr_count * 2);
                dst->cancel_transfer.unicast_addr_count = src->cancel_transfer.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_mbt_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_mbt_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_mbt_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MBT_CLIENT_RETRIEVE_CAPABILITIES:
        bt_mesh_free(arg->retrieve_capabilities.unicast_addr);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_TRANSFER_BLOB:
        bt_mesh_free(arg->transfer_blob.unicast_addr);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS:
        bt_mesh_free(arg->determine_transfer_status.unicast_addr);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CANCEL_TRANSFER:
        bt_mesh_free(arg->cancel_transfer.unicast_addr);
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_blob_trans_client_copy_req_data(btc_msg_t *msg, void *p_dst, void *p_src)
{
    esp_ble_mesh_mbt_client_cb_param_t *dst = (esp_ble_mesh_mbt_client_cb_param_t *)p_dst;
    esp_ble_mesh_mbt_client_cb_param_t *src = (esp_ble_mesh_mbt_client_cb_param_t *)p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MBT_CLIENT_RETRIEVE_CAPABILITIES:
        if (src->value.retrieve_capabilities_status.input.unicast_addr_count &&
            src->value.retrieve_capabilities_status.input.unicast_addr) {
            dst->value.retrieve_capabilities_status.input.unicast_addr = bt_mesh_calloc(src->value.retrieve_capabilities_status.input.unicast_addr_count * 2);
            if (dst->value.retrieve_capabilities_status.input.unicast_addr) {
                memcpy(dst->value.retrieve_capabilities_status.input.unicast_addr, src->value.retrieve_capabilities_status.input.unicast_addr,
                       src->value.retrieve_capabilities_status.input.unicast_addr_count * 2);
                dst->value.retrieve_capabilities_status.input.unicast_addr_count = src->value.retrieve_capabilities_status.input.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_TRANSFER_BLOB:
        if (src->value.transfer_blob_status.input.unicast_addr_count &&
            src->value.transfer_blob_status.input.unicast_addr) {
            dst->value.transfer_blob_status.input.unicast_addr = bt_mesh_calloc(src->value.transfer_blob_status.input.unicast_addr_count * 2);
            if (dst->value.transfer_blob_status.input.unicast_addr) {
                memcpy(dst->value.transfer_blob_status.input.unicast_addr, src->value.transfer_blob_status.input.unicast_addr,
                       src->value.transfer_blob_status.input.unicast_addr_count * 2);
                dst->value.transfer_blob_status.input.unicast_addr_count = src->value.transfer_blob_status.input.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS:
        if (src->value.determine_transfer_status_status.input.unicast_addr_count &&
            src->value.determine_transfer_status_status.input.unicast_addr) {
            dst->value.determine_transfer_status_status.input.unicast_addr = bt_mesh_calloc(src->value.determine_transfer_status_status.input.unicast_addr_count * 2);
            if (dst->value.determine_transfer_status_status.input.unicast_addr) {
                memcpy(dst->value.determine_transfer_status_status.input.unicast_addr, src->value.determine_transfer_status_status.input.unicast_addr,
                       src->value.determine_transfer_status_status.input.unicast_addr_count * 2);
                dst->value.determine_transfer_status_status.input.unicast_addr_count = src->value.determine_transfer_status_status.input.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CANCEL_TRANSFER:
        if (src->value.cancel_transfer_status.input.unicast_addr_count &&
            src->value.cancel_transfer_status.input.unicast_addr) {
            dst->value.cancel_transfer_status.input.unicast_addr = bt_mesh_calloc(src->value.cancel_transfer_status.input.unicast_addr_count * 2);
            if (dst->value.cancel_transfer_status.input.unicast_addr) {
                memcpy(dst->value.cancel_transfer_status.input.unicast_addr, src->value.cancel_transfer_status.input.unicast_addr,
                       src->value.cancel_transfer_status.input.unicast_addr_count * 2);
                dst->value.cancel_transfer_status.input.unicast_addr_count = src->value.cancel_transfer_status.input.unicast_addr_count;
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

static void btc_ble_mesh_blob_trans_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_mbt_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_mbt_client_cb_param_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MBT_CLIENT_RETRIEVE_CAPABILITIES:
        bt_mesh_free(arg->value.retrieve_capabilities_status.input.unicast_addr);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_TRANSFER_BLOB:
        bt_mesh_free(arg->value.transfer_blob_status.input.unicast_addr);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS:
        bt_mesh_free(arg->value.determine_transfer_status_status.input.unicast_addr);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CANCEL_TRANSFER:
        bt_mesh_free(arg->value.cancel_transfer_status.input.unicast_addr);
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_blob_trans_client_callback(esp_ble_mesh_mbt_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_MBT_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_mbt_client_cb_param_t),
                         btc_ble_mesh_blob_trans_client_copy_req_data,
                         btc_ble_mesh_blob_trans_client_free_req_data);
}

void bt_mesh_mbt_client_cb_evt_to_btc(uint8_t event, uint8_t result,
                                      struct bt_mesh_model *model)
{
    esp_ble_mesh_mbt_client_cb_param_t cb_params = {0};
    uint8_t cb_event = 0;

    if (model == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_MBT_CLIENT_RETRIEVE_CAPABILITIES_COMP:
        cb_event = ESP_BLE_MESH_MBT_CLIENT_RETRIEVE_CAPABILITIES_COMP_EVT;
        cb_params.value.retrieve_capabilities_comp.model = (esp_ble_mesh_model_t *)model;
        cb_params.value.retrieve_capabilities_comp.result = result;
        break;
    case BTC_BLE_MESH_EVT_MBT_CLIENT_TRANSFER_BLOB_COMP:
        cb_event = ESP_BLE_MESH_MBT_CLIENT_TRANSFER_BLOB_COMP_EVT;
        cb_params.value.transfer_blob_comp.model = (esp_ble_mesh_model_t *)model;
        cb_params.value.transfer_blob_comp.result = result;
        break;
    case BTC_BLE_MESH_EVT_MBT_CLIENT_SEND_BLOCK_COMP:
        cb_event = ESP_BLE_MESH_MBT_CLIENT_SEND_BLOCK_COMP_EVT;
        cb_params.value.send_block_comp.model = (esp_ble_mesh_model_t *)model;
        cb_params.value.send_block_comp.result = result;
        break;
    case BTC_BLE_MESH_EVT_MBT_CLIENT_SEND_DATA_COMP:
        cb_event = ESP_BLE_MESH_MBT_CLIENT_SEND_DATA_COMP_EVT;
        cb_params.value.send_data_comp.model = (esp_ble_mesh_model_t *)model;
        cb_params.value.send_data_comp.result = result;
        break;
    case BTC_BLE_MESH_EVT_MBT_CLIENT_DETERMINE_BLOCK_STATUS_COMP:
        cb_event = ESP_BLE_MESH_MBT_CLIENT_DETERMINE_BLOCK_STATUS_COMP_EVT;
        cb_params.value.determine_block_status_comp.model = (esp_ble_mesh_model_t *)model;
        cb_params.value.determine_block_status_comp.result = result;
        break;
    case BTC_BLE_MESH_EVT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS_COMP:
        cb_event = ESP_BLE_MESH_MBT_CLIENT_DETERMINE_TRANSFER_STATUS_COMP_EVT;
        cb_params.value.determine_transfer_status_comp.model = (esp_ble_mesh_model_t *)model;
        cb_params.value.determine_transfer_status_comp.result = result;
        break;
    case BTC_BLE_MESH_EVT_MBT_CLIENT_CANCEL_TRANSFER_COMP:
        cb_event = ESP_BLE_MESH_MBT_CLIENT_CANCEL_TRANSFER_COMP_EVT;
        cb_params.value.cancel_transfer_comp.model = (esp_ble_mesh_model_t *)model;
        cb_params.value.cancel_transfer_comp.result = result;
        break;
    default:
        BT_ERR("Unknown Blob Transfer Client event type %d", event);
        return;
    }

    btc_ble_mesh_blob_trans_client_callback(&cb_params, cb_event);
}

void btc_ble_mesh_mbt_client_publish_callback(uint32_t opcode,
                                              struct bt_mesh_model *model,
                                              struct bt_mesh_msg_ctx *ctx,
                                              struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    /* TBD */
}

void btc_ble_mesh_mbt_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_mbt_client_cb_param_t cb = {0};
    btc_ble_mesh_mbt_client_args_t *arg = NULL;
    uint8_t event = 0;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_mbt_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MBT_CLIENT_RETRIEVE_CAPABILITIES:
        event = ESP_BLE_MESH_MBT_CLIENT_RETRIEVE_CAPABILITIES_STATUS_EVT;
        memcpy(&cb.value.retrieve_capabilities_status.input,
               &arg->retrieve_capabilities, sizeof(arg->retrieve_capabilities));
        cb.value.retrieve_capabilities_status.error_code =
            bt_mesh_retrieve_capabilities(&arg->retrieve_capabilities);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_TRANSFER_BLOB:
        event = ESP_BLE_MESH_MBT_CLIENT_TRANSFER_BLOB_STATUS_EVT;
        memcpy(&cb.value.transfer_blob_status.input, &arg->transfer_blob, sizeof(arg->transfer_blob));
        cb.value.transfer_blob_status.error_code = bt_mesh_transfer_blob(&arg->transfer_blob);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_SEND_BLOCK:
        event = ESP_BLE_MESH_MBT_CLIENT_SEND_BLOCK_STATUS_EVT;
        memcpy(&cb.value.send_block_status.input, &arg->send_block, sizeof(arg->send_block));
        cb.value.send_block_status.error_code = bt_mesh_send_block(&arg->send_block);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_SEND_DATA:
        event = ESP_BLE_MESH_MBT_CLIENT_SEND_DATA_STATUS_EVT;
        memcpy(&cb.value.send_data_status.input, &arg->send_data, sizeof(arg->send_data));
        cb.value.send_data_status.error_code = bt_mesh_send_data(&arg->send_data);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_BLOCK_STATUS:
        event = ESP_BLE_MESH_MBT_CLIENT_DETERMINE_BLOCK_STATUS_STATUS_EVT;
        memcpy(&cb.value.determine_block_status_status.input,
               &arg->determine_block_status, sizeof(arg->determine_block_status));
        cb.value.determine_block_status_status.error_code =
            bt_mesh_determine_block_status(&arg->determine_block_status);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS:
        event = ESP_BLE_MESH_MBT_CLIENT_DETERMINE_TRANSFER_STATUS_STATUS_EVT;
        memcpy(&cb.value.determine_transfer_status_status.input,
               &arg->determine_transfer_status, sizeof(arg->determine_transfer_status));
        cb.value.determine_transfer_status_status.error_code =
            bt_mesh_determine_transfer_status(&arg->determine_transfer_status);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CANCEL_TRANSFER:
        event = ESP_BLE_MESH_MBT_CLIENT_CANCEL_TRANSFER_STATUS_EVT;
        memcpy(&cb.value.cancel_transfer_status.input, &arg->cancel_transfer, sizeof(arg->cancel_transfer));
        cb.value.cancel_transfer_status.error_code = bt_mesh_cancel_transfer(&arg->cancel_transfer);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_SET_TRANSFER_TTL:
        event = ESP_BLE_MESH_MBT_CLIENT_SET_TRANSFER_TTL_COMP_EVT;
        cb.value.set_transfer_ttl.model = arg->set_transfer_ttl.model;
        cb.value.set_transfer_ttl.transfer_ttl = arg->set_transfer_ttl.transfer_ttl;
        cb.value.set_transfer_ttl.error_code =
            bt_mesh_set_transfer_ttl_state((struct bt_mesh_model *)arg->set_transfer_ttl.model,
                                           arg->set_transfer_ttl.transfer_ttl);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_TRANSFER_TTL:
        event = ESP_BLE_MESH_MBT_CLIENT_CLEAR_TRANSFER_TTL_COMP_EVT;
        cb.value.clear_transfer_ttl.model = arg->clear_transfer_ttl.model;
        cb.value.clear_transfer_ttl.error_code =
            bt_mesh_clear_transfer_ttl_state((struct bt_mesh_model *)arg->clear_transfer_ttl.model);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_SET_APP_IDX:
        event = ESP_BLE_MESH_MBT_CLIENT_SET_APP_IDX_COMP_EVT;
        cb.value.set_app_idx.model = arg->set_app_idx.model;
        cb.value.set_app_idx.app_idx = arg->set_app_idx.app_idx;
        cb.value.set_app_idx.error_code =
            bt_mesh_set_app_idx_state((struct bt_mesh_model *)arg->set_app_idx.model,
                                      arg->set_app_idx.app_idx);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_APP_IDX:
        event = ESP_BLE_MESH_MBT_CLIENT_CLEAR_APP_IDX_COMP_EVT;
        cb.value.clear_app_idx.model = arg->clear_app_idx.model;
        cb.value.clear_app_idx.error_code =
            bt_mesh_clear_app_idx_state((struct bt_mesh_model *)arg->clear_app_idx.model);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_SET_MULTICAST_ADDR:
        event = ESP_BLE_MESH_MBT_CLIENT_SET_MULTICAST_ADDR_COMP_EVT;
        cb.value.set_multicast_addr.model = arg->set_multicast_addr.model;
        cb.value.set_multicast_addr.multicast_addr = arg->set_multicast_addr.multicast_addr;
        cb.value.set_multicast_addr.error_code =
            bt_mesh_set_multicast_addr_state((struct bt_mesh_model *)arg->set_multicast_addr.model,
                                             arg->set_multicast_addr.multicast_addr);
        break;
    case BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_MULTICAST_ADDR:
        event = ESP_BLE_MESH_MBT_CLIENT_CLEAR_MULTICAST_ADDR_COMP_EVT;
        cb.value.clear_multicast_addr.model = arg->clear_multicast_addr.model;
        cb.value.clear_multicast_addr.error_code =
            bt_mesh_clear_multicast_addr_state((struct bt_mesh_model *)arg->clear_multicast_addr.model);
        break;
    default:
        BT_ERR("Unknown BLOB Transfer Client act %d", msg->act);
        return;
    }

    btc_ble_mesh_blob_trans_client_callback(&cb, event);

    btc_ble_mesh_mbt_client_arg_deep_free(msg);
}

void btc_ble_mesh_mbt_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_mbt_client_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_mbt_client_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_MBT_CLIENT_EVT_MAX) {
        btc_ble_mesh_blob_trans_client_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_blob_trans_client_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_MBT_CLI */

#if CONFIG_BLE_MESH_MBT_SRV

static inline void btc_ble_mesh_blob_trans_server_cb_to_app(esp_ble_mesh_mbt_server_cb_event_t event,
                                                            esp_ble_mesh_mbt_server_cb_param_t *param)
{
    esp_ble_mesh_mbt_server_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_mbt_server_cb_t)btc_profile_cb_get(BTC_PID_MBT_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_blob_trans_server_callback(esp_ble_mesh_mbt_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_MBT_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_MBT_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_mbt_server_cb_param_t), NULL, NULL);
}

void bt_mesh_mbt_server_cb_evt_to_btc(uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx)
{
    esp_ble_mesh_mbt_server_cb_param_t cb_params = {0};
    uint8_t cb_event = 0;

    if (model == NULL || (ctx == NULL &&
        event != BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_RECEIVE_TIMEOUT)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_GET:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_TRANSFER_GET_EVT;
        memcpy(&cb_params.value.blob_transfer_get.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_START:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_TRANSFER_START_EVT;
        memcpy(&cb_params.value.blob_transfer_start.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_CANCEL:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_TRANSFER_CANCEL_EVT;
        memcpy(&cb_params.value.blob_transfer_cancel.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_BLOCK_GET:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_BLOCK_GET_EVT;
        memcpy(&cb_params.value.blob_block_get.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_BLOCK_START:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_BLOCK_START_EVT;
        memcpy(&cb_params.value.blob_block_start.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_CHUNK_TRANSFER:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_CHUNK_TRANSFER_EVT;
        memcpy(&cb_params.value.blob_chunk_transfer.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_INFORMATION_GET:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_INFORMATION_GET_EVT;
        memcpy(&cb_params.value.blob_information_get.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOCK_RECEIVE_COMP:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOCK_RECEIVE_COMP_EVT;
        memcpy(&cb_params.value.block_receive_comp.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_RECEIVE_COMP:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_RECEIVE_COMP_EVT;
        memcpy(&cb_params.value.blob_receive_comp.ctx, ctx, sizeof(struct bt_mesh_msg_ctx));
        break;
    case BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_RECEIVE_TIMEOUT:
        cb_event = ESP_BLE_MESH_MBT_SERVER_BLOB_RECEIVE_TIMEOUT_EVT;
        break;
    default:
        BT_ERR("Unknown Blob Transfer server event type %d", event);
        return;
    }

    btc_ble_mesh_blob_trans_server_callback(&cb_params, cb_event);
}

void btc_ble_mesh_mbt_server_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_mbt_server_cb_param_t cb = {0};
    btc_ble_mesh_mbt_server_args_t *arg = NULL;
    uint8_t event = 0;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_mbt_server_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_MBT_SERVER_INITIALIZE_BLOB_RECEIVE:
        event = ESP_BLE_MESH_MBT_SERVER_INITIALIZE_BLOB_RECEIVE_COMP_EVT;
        memcpy(&cb.value.initialize_blob_receive_comp.input,
               &arg->initialize_blob_receive, sizeof(arg->initialize_blob_receive));
        cb.value.initialize_blob_receive_comp.error_code =
            bt_mesh_initialize_blob_receive(&arg->initialize_blob_receive);
        break;
    case BTC_BLE_MESH_ACT_MBT_SERVER_CANCEL_BLOB_RECEIVE:
        event = ESP_BLE_MESH_MBT_SERVER_CANCEL_BLOB_RECEIVE_COMP_EVT;
        memcpy(&cb.value.cancel_blob_receive_comp.input,
               &arg->cancel_blob_receive, sizeof(arg->cancel_blob_receive));
        cb.value.cancel_blob_receive_comp.error_code =
            bt_mesh_cancel_blob_receive(&arg->cancel_blob_receive);
        break;
    case BTC_BLE_MESH_ACT_MBT_SERVER_SET_BLOB_CAPABILITIES:
        event = ESP_BLE_MESH_MBT_SERVER_SET_BLOB_CAPABILITIES_COMP_EVT;
        memcpy(&cb.value.set_blob_capabilities_comp.input,
               &arg->set_blob_capabilities, sizeof(arg->set_blob_capabilities));
        cb.value.set_blob_capabilities_comp.error_code =
            bt_mesh_set_blob_capabilities((struct bt_mesh_model *)arg->set_blob_capabilities.model,
                                          &arg->set_blob_capabilities.caps);
        break;
    default:
        BT_ERR("Unknown BLOB Transfer Server act %d", msg->act);
        return;
    }

    btc_ble_mesh_blob_trans_server_callback(&cb, event);
}

void btc_ble_mesh_mbt_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_mbt_server_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_mbt_server_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_MBT_SERVER_EVT_MAX) {
        btc_ble_mesh_blob_trans_server_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }
}

#endif /* CONFIG_BLE_MESH_MBT_SRV */
