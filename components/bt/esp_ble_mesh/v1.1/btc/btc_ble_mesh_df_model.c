/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_df_model.h"
#include "esp_ble_mesh_df_model_api.h"

#if CONFIG_BLE_MESH_DF_CLI

extern int bt_mesh_directed_control_get(void *param, uint16_t net_idx);
extern int bt_mesh_directed_control_set(void *param, void *set);
extern int bt_mesh_path_metric_get(void *param, uint16_t net_idx);
extern int bt_mesh_path_metric_set(void *param, void *set);
extern int bt_mesh_discovery_table_caps_get(void *param, uint16_t net_idx);
extern int bt_mesh_discovery_table_caps_set(void *param, void *set);
extern int bt_mesh_forwarding_table_add(void *param, void *add);
extern int bt_mesh_forwarding_table_del(void *param, void *del);
extern int bt_mesh_forwarding_table_deps_add(void *param, void *add);
extern int bt_mesh_forwarding_table_deps_del(void *param, void *del);
extern int bt_mesh_forwarding_table_entries_cnt_get(void *param, uint16_t net_idx);
extern int bt_mesh_forwarding_table_entries_get(void *param, void *get);
extern int bt_mesh_forwarding_table_deps_get(void *param, void *get);
extern int bt_mesh_wanted_lanes_get(void *param, uint16_t net_idx);
extern int bt_mesh_wanted_lanes_set(void *param, uint16_t net_idx, uint8_t wanted_lanes);
extern int bt_mesh_two_way_path_get(void *param, uint16_t net_idx);
extern int bt_mesh_two_way_path_set(void *param, uint16_t net_idx, uint8_t two_way_path);
extern int bt_mesh_path_echo_interval_get(void *param, uint16_t net_idx);
extern int bt_mesh_path_echo_interval_set(void *param, uint16_t net_idx,
                                          uint8_t unicast_echo_interval,
                                          uint8_t multicast_echo_interval);
extern int bt_mesh_directed_net_transmit_get(void *param);
extern int bt_mesh_directed_net_transmit_set(void *param, uint8_t transmit);
extern int bt_mesh_directed_relay_retransmit_get(void *param);
extern int bt_mesh_directed_relay_retransmit_set(void *param, uint8_t retransmit);
extern int bt_mesh_rssi_threshold_get(void *param);
extern int bt_mesh_rssi_threshold_set(void *param, uint8_t rssi_margin);
extern int bt_mesh_directed_paths_get(void *param);
extern int bt_mesh_directed_pub_policy_get(void *param, void *get);
extern int bt_mesh_directed_pub_policy_set(void *param, void *set);
extern int bt_mesh_path_discovery_timing_ctl_get(void *param);
extern int bt_mesh_path_discovery_timing_ctl_set(void *param, void *set);
extern int bt_mesh_directed_ctl_net_transmit_get(void *param);
extern int bt_mesh_directed_ctl_net_transmit_set(void *param, uint8_t transmit);
extern int bt_mesh_directed_ctl_relay_retransmit_get(void *param);
extern int bt_mesh_directed_ctl_relay_retransmit_set(void *param, uint8_t retransmit);

/* Directed Forwarding Config Client model related functions */

static inline void btc_ble_mesh_df_client_cb_to_app(esp_ble_mesh_df_client_cb_event_t event,
                                                    esp_ble_mesh_df_client_cb_param_t *param)
{
    esp_ble_mesh_df_client_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_DF_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_df_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_df_client_args_t *dst = p_dest;
    btc_ble_mesh_df_client_args_t *src = p_src;
    uint16_t length = 0;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DF_CLIENT_GET_STATE: {
        dst->df_get.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->df_get.params) {
            memcpy(dst->df_get.params, src->df_get.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        if (src->df_get.get) {
            dst->df_get.get = bt_mesh_calloc(sizeof(esp_ble_mesh_df_client_get_t));
            if (dst->df_get.get) {
                memcpy(dst->df_get.get, src->df_get.get,
                       sizeof(esp_ble_mesh_df_client_get_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    case BTC_BLE_MESH_ACT_DF_CLIENT_SET_STATE: {
        dst->df_set.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->df_set.set = bt_mesh_calloc(sizeof(esp_ble_mesh_df_client_set_t));
        if (dst->df_set.params && dst->df_set.set) {
            memcpy(dst->df_set.params, src->df_set.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->df_set.set, src->df_set.set,
                   sizeof(esp_ble_mesh_df_client_set_t));

            switch (src->df_set.params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_ADD:
                if (src->df_set.set->forwarding_table_deps_add.dep_origin_uar_list &&
                    src->df_set.set->forwarding_table_deps_add.dep_origin_uar_list_size) {
                    length = src->df_set.set->forwarding_table_deps_add.dep_origin_uar_list_size * sizeof(esp_ble_mesh_uar_t);
                    dst->df_set.set->forwarding_table_deps_add.dep_origin_uar_list = bt_mesh_calloc(length);
                    if (!dst->df_set.set->forwarding_table_deps_add.dep_origin_uar_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    memcpy(dst->df_set.set->forwarding_table_deps_add.dep_origin_uar_list,
                           src->df_set.set->forwarding_table_deps_add.dep_origin_uar_list,
                           length);
                }
                if (src->df_set.set->forwarding_table_deps_add.dep_target_uar_list &&
                    src->df_set.set->forwarding_table_deps_add.dep_target_uar_list_size) {
                    length = src->df_set.set->forwarding_table_deps_add.dep_target_uar_list_size * sizeof(esp_ble_mesh_uar_t);
                    dst->df_set.set->forwarding_table_deps_add.dep_target_uar_list = bt_mesh_calloc(length);
                    if (!dst->df_set.set->forwarding_table_deps_add.dep_target_uar_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    memcpy(dst->df_set.set->forwarding_table_deps_add.dep_target_uar_list,
                           src->df_set.set->forwarding_table_deps_add.dep_target_uar_list,
                           length);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_DEL:
                if (src->df_set.set->forwarding_table_deps_del.dep_origin_list &&
                    src->df_set.set->forwarding_table_deps_del.dep_origin_list_size) {
                    length = src->df_set.set->forwarding_table_deps_del.dep_origin_list_size * 2;
                    dst->df_set.set->forwarding_table_deps_del.dep_origin_list = bt_mesh_calloc(length);
                    if (!dst->df_set.set->forwarding_table_deps_del.dep_origin_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    memcpy(dst->df_set.set->forwarding_table_deps_del.dep_origin_list,
                           src->df_set.set->forwarding_table_deps_del.dep_origin_list,
                           length);
                }
                if (src->df_set.set->forwarding_table_deps_del.dep_target_list &&
                    src->df_set.set->forwarding_table_deps_del.dep_target_list_size) {
                    length = src->df_set.set->forwarding_table_deps_del.dep_target_list_size * 2;
                    dst->df_set.set->forwarding_table_deps_del.dep_target_list = bt_mesh_calloc(length);
                    if (!dst->df_set.set->forwarding_table_deps_del.dep_target_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    memcpy(dst->df_set.set->forwarding_table_deps_del.dep_target_list,
                           src->df_set.set->forwarding_table_deps_del.dep_target_list,
                           length);
                }
                break;
            default:
                break;
            }
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    }
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_df_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_df_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_df_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DF_CLIENT_GET_STATE:
        if (arg->df_get.params) {
            bt_mesh_free(arg->df_get.params);
        }
        if (arg->df_get.get) {
            bt_mesh_free(arg->df_get.get);
        }
        break;
    case BTC_BLE_MESH_ACT_DF_CLIENT_SET_STATE:
        if (arg->df_set.set) {
            if (arg->df_set.params) {
                switch (arg->df_set.params->opcode) {
                case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_ADD:
                    bt_mesh_free(arg->df_set.set->forwarding_table_deps_add.dep_origin_uar_list);
                    bt_mesh_free(arg->df_set.set->forwarding_table_deps_add.dep_target_uar_list);
                    break;
                case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_DEL:
                    bt_mesh_free(arg->df_set.set->forwarding_table_deps_del.dep_origin_list);
                    bt_mesh_free(arg->df_set.set->forwarding_table_deps_del.dep_target_list);
                    break;
                default:
                    break;
                }
            }
            bt_mesh_free(arg->df_set.set);
        }
        if (arg->df_set.params) {
            bt_mesh_free(arg->df_set.params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_df_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_df_client_cb_param_t *p_dest_data = p_dest;
    esp_ble_mesh_df_client_cb_param_t *p_src_data = p_src;
    uint16_t length = 0;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (p_src_data->params) {
        p_dest_data->params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!p_dest_data->params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        memcpy(p_dest_data->params, p_src_data->params, sizeof(esp_ble_mesh_client_common_param_t));
    }

    switch (msg->act) {
    case ESP_BLE_MESH_DF_CLIENT_RECV_GET_RSP_EVT:
    case ESP_BLE_MESH_DF_CLIENT_RECV_SET_RSP_EVT:
    case ESP_BLE_MESH_DF_CLIENT_RECV_PUB_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_GET:
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_STATUS:
                if (p_src_data->recv.forwarding_table_entries_status.entry_list) {
                    length = p_src_data->recv.forwarding_table_entries_status.entry_list_size * sizeof(esp_ble_mesh_forwarding_table_entry_t);
                    p_dest_data->recv.forwarding_table_entries_status.entry_list = bt_mesh_calloc(length);
                    if (!p_dest_data->recv.forwarding_table_entries_status.entry_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    memcpy(p_dest_data->recv.forwarding_table_entries_status.entry_list,
                           p_src_data->recv.forwarding_table_entries_status.entry_list,
                           length);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET:
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET_STATUS:
                if (p_src_data->recv.forwarding_table_deps_get_status.dep_origin_uar_list) {
                    length = p_src_data->recv.forwarding_table_deps_get_status.dep_origin_uar_list_size * sizeof(esp_ble_mesh_uar_t);
                    p_dest_data->recv.forwarding_table_deps_get_status.dep_origin_uar_list = bt_mesh_calloc(length);
                    if (!p_dest_data->recv.forwarding_table_deps_get_status.dep_origin_uar_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    memcpy(p_dest_data->recv.forwarding_table_deps_get_status.dep_origin_uar_list,
                           p_src_data->recv.forwarding_table_deps_get_status.dep_origin_uar_list,
                           length);
                }
                if (p_src_data->recv.forwarding_table_deps_get_status.dep_target_uar_list) {
                    length = p_src_data->recv.forwarding_table_deps_get_status.dep_target_uar_list_size * sizeof(esp_ble_mesh_uar_t);
                    p_dest_data->recv.forwarding_table_deps_get_status.dep_target_uar_list = bt_mesh_calloc(length);
                    if (!p_dest_data->recv.forwarding_table_deps_get_status.dep_target_uar_list) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }

                    memcpy(p_dest_data->recv.forwarding_table_deps_get_status.dep_target_uar_list,
                           p_src_data->recv.forwarding_table_deps_get_status.dep_target_uar_list,
                           length);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_DF_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_DF_CLIENT_SEND_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_df_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_df_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_df_client_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_DF_CLIENT_RECV_GET_RSP_EVT:
    case ESP_BLE_MESH_DF_CLIENT_RECV_SET_RSP_EVT:
    case ESP_BLE_MESH_DF_CLIENT_RECV_PUB_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_GET:
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_STATUS:
                bt_mesh_free(arg->recv.forwarding_table_entries_status.entry_list);
                break;
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET:
            case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET_STATUS:
                bt_mesh_free(arg->recv.forwarding_table_deps_get_status.dep_origin_uar_list);
                bt_mesh_free(arg->recv.forwarding_table_deps_get_status.dep_target_uar_list);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_DF_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_DF_CLIENT_SEND_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_df_client_cb(esp_ble_mesh_df_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_DF_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DF_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_df_client_cb_param_t),
                         btc_ble_mesh_df_client_copy_req_data,
                         btc_ble_mesh_df_client_free_req_data);
}

void bt_mesh_df_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                     struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     const uint8_t *val, size_t len)
{
    esp_ble_mesh_df_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0;

    if (!model || !ctx || len > sizeof(cb_params.recv)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_DF_CLIENT_SEND_TIMEOUT:
        act = ESP_BLE_MESH_DF_CLIENT_SEND_TIMEOUT_EVT;
        break;
    case BTC_BLE_MESH_EVT_DF_CLIENT_RECV_GET_RSP:
        act = ESP_BLE_MESH_DF_CLIENT_RECV_GET_RSP_EVT;
        break;
    case BTC_BLE_MESH_EVT_DF_CLIENT_RECV_SET_RSP:
        act = ESP_BLE_MESH_DF_CLIENT_RECV_SET_RSP_EVT;
        break;
    case BTC_BLE_MESH_EVT_DF_CLIENT_RECV_PUB:
        act = ESP_BLE_MESH_DF_CLIENT_RECV_PUB_EVT;
        break;
    default:
        BT_ERR("Unknown Directed Forward Config client event type %d", event);
        return;
    }

    params.opcode = opcode;
    params.model = (esp_ble_mesh_model_t *)model;
    params.ctx.net_idx = ctx->net_idx;
    params.ctx.app_idx = ctx->app_idx;
    params.ctx.addr = ctx->addr;
    params.ctx.recv_ttl = ctx->recv_ttl;
    params.ctx.recv_op = ctx->recv_op;
    params.ctx.recv_dst = ctx->recv_dst;
    params.ctx.recv_rssi = ctx->recv_rssi;
    params.ctx.send_ttl = ctx->send_ttl;

    cb_params.params = &params;

    if (val && len) {
        memcpy(&cb_params.recv, val, len);
    }

    btc_ble_mesh_df_client_cb(&cb_params, act);
}

void btc_ble_mesh_df_client_recv_pub_cb(uint32_t opcode,
                                        struct bt_mesh_model *model,
                                        struct bt_mesh_msg_ctx *ctx,
                                        struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_df_client_cb_evt_to_btc(opcode,
        BTC_BLE_MESH_EVT_DF_CLIENT_RECV_PUB,
        model, ctx, buf->data, buf->len);
}

static int btc_ble_mesh_df_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                            esp_ble_mesh_df_client_get_t *get)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_GET:
    case ESP_BLE_MESH_MODEL_OP_PATH_METRIC_GET:
    case ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_GET:
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_CNT_GET:
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_GET:
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET:
    case ESP_BLE_MESH_MODEL_OP_WANTED_LANES_GET:
    case ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_GET:
    case ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_GET:
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_GET:
        if (get == NULL) {
            BT_ERR("Invalid Directed Forward Config Get");
            return -EINVAL;
        }
        break;
    default:
        break;
    }

    btc_ble_mesh_set_client_common_param(params, &param, true);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_GET:
        return bt_mesh_directed_control_get(&param, get->directed_control_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_PATH_METRIC_GET:
        return bt_mesh_path_metric_get(&param, get->path_metric_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_GET:
        return bt_mesh_discovery_table_caps_get(&param, get->disc_table_caps_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_CNT_GET:
        return bt_mesh_forwarding_table_entries_cnt_get(&param, get->forwarding_table_entries_cnt_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_GET:
        return bt_mesh_forwarding_table_entries_get(&param, &get->forwarding_table_entries_get);
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET:
        return bt_mesh_forwarding_table_deps_get(&param, &get->forwarding_table_deps_get);
    case ESP_BLE_MESH_MODEL_OP_WANTED_LANES_GET:
        return bt_mesh_wanted_lanes_get(&param, get->wanted_lanes_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_GET:
        return bt_mesh_two_way_path_get(&param, get->two_way_path_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_GET:
        return bt_mesh_path_echo_interval_get(&param, get->path_echo_interval_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_NET_TRANSMIT_GET:
        return bt_mesh_directed_net_transmit_get(&param);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_RELAY_RETRANSMIT_GET:
        return bt_mesh_directed_relay_retransmit_get(&param);
    case ESP_BLE_MESH_MODEL_OP_RSSI_THRESHOLD_GET:
        return bt_mesh_rssi_threshold_get(&param);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_PATHS_GET:
        return bt_mesh_directed_paths_get(&param);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_GET:
        return bt_mesh_directed_pub_policy_get(&param, &get->directed_pub_policy_get);
    case ESP_BLE_MESH_MODEL_OP_PATH_DISCOVERY_TIMING_CTL_GET:
        return bt_mesh_path_discovery_timing_ctl_get(&param);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_NET_TRANSMIT_GET:
        return bt_mesh_directed_ctl_net_transmit_get(&param);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_RELAY_RETRANSMIT_GET:
        return bt_mesh_directed_ctl_relay_retransmit_get(&param);
    default:
        BT_ERR("Invalid Directed Forward Config Get opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

static int btc_ble_mesh_df_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                            esp_ble_mesh_df_client_set_t *set)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL || set == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    btc_ble_mesh_set_client_common_param(params, &param, true);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_SET:
        return bt_mesh_directed_control_set(&param, &set->directed_control_set);
    case ESP_BLE_MESH_MODEL_OP_PATH_METRIC_SET:
        return bt_mesh_path_metric_set(&param, &set->path_metric_set);
    case ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_SET:
        return bt_mesh_discovery_table_caps_set(&param, &set->disc_table_caps_set);
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ADD:
        return bt_mesh_forwarding_table_add(&param, &set->forwarding_table_add);
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEL:
        return bt_mesh_forwarding_table_del(&param, &set->forwarding_table_del);
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_ADD:
        return bt_mesh_forwarding_table_deps_add(&param, &set->forwarding_table_deps_add);
    case ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_DEL:
        return bt_mesh_forwarding_table_deps_del(&param, &set->forwarding_table_deps_del);
    case ESP_BLE_MESH_MODEL_OP_WANTED_LANES_SET:
        return bt_mesh_wanted_lanes_set(&param, set->wanted_lanes_set.net_idx,
                                        set->wanted_lanes_set.wanted_lanes);
    case ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_SET:
        return bt_mesh_two_way_path_set(&param, set->two_way_path_set.net_idx,
                                        set->two_way_path_set.two_way_path);
    case ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_SET:
        return bt_mesh_path_echo_interval_set(&param, set->path_echo_interval_set.net_idx,
                                              set->path_echo_interval_set.unicast_echo_interval,
                                              set->path_echo_interval_set.multicast_echo_interval);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_NET_TRANSMIT_SET:
        return bt_mesh_directed_net_transmit_set(&param, set->directed_net_transmit_set.net_transmit);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_RELAY_RETRANSMIT_SET:
        return bt_mesh_directed_relay_retransmit_set(&param, set->directed_relay_retransmit_set.relay_retransmit);
    case ESP_BLE_MESH_MODEL_OP_RSSI_THRESHOLD_SET:
        return bt_mesh_rssi_threshold_set(&param, set->rssi_threshold_set.rssi_margin);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_SET:
        return bt_mesh_directed_pub_policy_set(&param, &set->directed_pub_policy_set);
    case ESP_BLE_MESH_MODEL_OP_PATH_DISCOVERY_TIMING_CTL_SET:
        return bt_mesh_path_discovery_timing_ctl_set(&param, &set->path_disc_timing_ctl_set);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_NET_TRANSMIT_SET:
        return bt_mesh_directed_ctl_net_transmit_set(&param, set->directed_ctl_net_transmit_set.net_transmit);
    case ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_RELAY_RETRANSMIT_SET:
        return bt_mesh_directed_ctl_relay_retransmit_set(&param, set->directed_ctl_relay_retransmit_set.relay_retransmit);
    default:
        BT_ERR("Invalid Directed Forward Config Set opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

void btc_ble_mesh_df_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_df_client_cb_param_t cb = {0};
    btc_ble_mesh_df_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_df_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_DF_CLIENT_GET_STATE:
        cb.params = arg->df_get.params;
        cb.send.err_code = btc_ble_mesh_df_client_get_state(arg->df_get.params,
                                                            arg->df_get.get);
        btc_ble_mesh_df_client_cb(&cb,
            ESP_BLE_MESH_DF_CLIENT_SEND_COMP_EVT);
        break;
    case BTC_BLE_MESH_ACT_DF_CLIENT_SET_STATE:
        cb.params = arg->df_set.params;
        cb.send.err_code = btc_ble_mesh_df_client_set_state(arg->df_set.params,
                                                            arg->df_set.set);
        btc_ble_mesh_df_client_cb(&cb,
            ESP_BLE_MESH_DF_CLIENT_SEND_COMP_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_df_client_arg_deep_free(msg);
}

void btc_ble_mesh_df_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_df_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_df_client_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_DF_CLIENT_EVT_MAX) {
        btc_ble_mesh_df_client_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_df_client_free_req_data(msg);
}
#endif /* CONFIG_BLE_MESH_DF_CLI */

#if CONFIG_BLE_MESH_DF_SRV

/* Directed Forwarding Config Server model related functions */

static inline void btc_ble_mesh_df_server_cb_to_app(esp_ble_mesh_df_server_cb_event_t event,
                                                    esp_ble_mesh_df_server_cb_param_t *param)
{
    esp_ble_mesh_df_server_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_DF_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_df_server_cb(
                esp_ble_mesh_df_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_DF_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DF_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_df_server_cb_param_t), NULL, NULL);
}

void bt_mesh_df_server_cb_evt_to_btc(uint8_t event,
                                     struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     const uint8_t *val, size_t len)
{
    esp_ble_mesh_df_server_cb_param_t cb_params = {0};
    uint8_t act = 0;

    if (len > sizeof(cb_params.value)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_DF_SERVER_STATE_CHANGE:
        act = ESP_BLE_MESH_DF_SERVER_STATE_CHANGE_EVT;
        break;
    case BTC_BLE_MESH_EVT_DF_SERVER_TABLE_CHANGE:
        act = ESP_BLE_MESH_DF_SERVER_TABLE_CHANGE_EVT;
        break;
    default:
        BT_ERR("Unknown Directed Forward Config server event type %d", event);
        return;
    }

    if (model) {
        cb_params.model = (esp_ble_mesh_model_t *)model;
    }

    if (ctx) {
        cb_params.ctx.net_idx = ctx->net_idx;
        cb_params.ctx.app_idx = ctx->app_idx;
        cb_params.ctx.addr = ctx->addr;
        cb_params.ctx.recv_ttl = ctx->recv_ttl;
        cb_params.ctx.recv_op = ctx->recv_op;
        cb_params.ctx.recv_dst = ctx->recv_dst;
        cb_params.ctx.recv_rssi = ctx->recv_rssi;
        cb_params.ctx.send_ttl = ctx->send_ttl;
    }

    if (val && len) {
        memcpy(&cb_params.value, val, len);
    }

    btc_ble_mesh_df_server_cb(&cb_params, act);
}

void btc_ble_mesh_df_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_df_server_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_df_server_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_DF_SERVER_EVT_MAX) {
        btc_ble_mesh_df_server_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }
}
#endif /* CONFIG_BLE_MESH_DF_SRV */
