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

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_config_model.h"
#include "foundation.h"
#include "esp_ble_mesh_config_model_api.h"

#if CONFIG_BLE_MESH_CFG_CLI
#include "cfg_cli.h"

/* Configuration Client Model related functions */

static inline void btc_ble_mesh_config_client_cb_to_app(esp_ble_mesh_cfg_client_cb_event_t event,
                                                        esp_ble_mesh_cfg_client_cb_param_t *param)
{
    esp_ble_mesh_cfg_client_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_cfg_client_cb_t)btc_profile_cb_get(BTC_PID_CONFIG_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_config_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_config_client_args_t *dst = (btc_ble_mesh_config_client_args_t *)p_dest;
    btc_ble_mesh_config_client_args_t *src = (btc_ble_mesh_config_client_args_t *)p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_GET_STATE: {
        dst->cfg_client_get_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->cfg_client_get_state.params) {
            memcpy(dst->cfg_client_get_state.params, src->cfg_client_get_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        if (src->cfg_client_get_state.get_state) {
            dst->cfg_client_get_state.get_state = (esp_ble_mesh_cfg_client_get_state_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_cfg_client_get_state_t));
            if (dst->cfg_client_get_state.get_state) {
                memcpy(dst->cfg_client_get_state.get_state, src->cfg_client_get_state.get_state,
                    sizeof(esp_ble_mesh_cfg_client_get_state_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_SET_STATE: {
        dst->cfg_client_set_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (dst->cfg_client_set_state.params) {
            memcpy(dst->cfg_client_set_state.params, src->cfg_client_set_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        if (src->cfg_client_set_state.set_state) {
            dst->cfg_client_set_state.set_state = (esp_ble_mesh_cfg_client_set_state_t *)bt_mesh_malloc(sizeof(esp_ble_mesh_cfg_client_set_state_t));
            if (dst->cfg_client_set_state.set_state) {
                memcpy(dst->cfg_client_set_state.set_state, src->cfg_client_set_state.set_state,
                    sizeof(esp_ble_mesh_cfg_client_set_state_t));
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    }
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

static void btc_ble_mesh_config_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_config_client_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_config_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_GET_STATE:
        if (arg->cfg_client_get_state.params) {
            bt_mesh_free(arg->cfg_client_get_state.params);
        }
        if (arg->cfg_client_get_state.get_state) {
            bt_mesh_free(arg->cfg_client_get_state.get_state);
        }
        break;
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_SET_STATE:
        if (arg->cfg_client_set_state.params) {
            bt_mesh_free(arg->cfg_client_set_state.params);
        }
        if (arg->cfg_client_set_state.set_state) {
            bt_mesh_free(arg->cfg_client_set_state.set_state);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_config_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_cfg_client_cb_param_t *p_dest_data = (esp_ble_mesh_cfg_client_cb_param_t *)p_dest;
    esp_ble_mesh_cfg_client_cb_param_t *p_src_data = (esp_ble_mesh_cfg_client_cb_param_t *)p_src;
    u16_t length = 0U;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (p_src_data->params) {
        p_dest_data->params = bt_mesh_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!p_dest_data->params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        memcpy(p_dest_data->params, p_src_data->params, sizeof(esp_ble_mesh_client_common_param_t));
    }

    switch (msg->act) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case OP_DEV_COMP_DATA_GET:
            case OP_DEV_COMP_DATA_STATUS:
                if (p_src_data->status_cb.comp_data_status.composition_data) {
                    length = p_src_data->status_cb.comp_data_status.composition_data->len;
                    p_dest_data->status_cb.comp_data_status.composition_data = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.comp_data_status.composition_data) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.comp_data_status.composition_data,
                                           p_src_data->status_cb.comp_data_status.composition_data->data,
                                           p_src_data->status_cb.comp_data_status.composition_data->len);
                }
                break;
            case OP_MOD_SUB_GET:
            case OP_MOD_SUB_GET_VND:
            case OP_MOD_SUB_LIST:
            case OP_MOD_SUB_LIST_VND:
                if (p_src_data->status_cb.model_sub_list.sub_addr) {
                    length = p_src_data->status_cb.model_sub_list.sub_addr->len;
                    p_dest_data->status_cb.model_sub_list.sub_addr = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.model_sub_list.sub_addr) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.model_sub_list.sub_addr,
                                           p_src_data->status_cb.model_sub_list.sub_addr->data,
                                           p_src_data->status_cb.model_sub_list.sub_addr->len);
                }
                break;
            case OP_NET_KEY_GET:
            case OP_NET_KEY_LIST:
                if (p_src_data->status_cb.netkey_list.net_idx) {
                    length = p_src_data->status_cb.netkey_list.net_idx->len;
                    p_dest_data->status_cb.netkey_list.net_idx = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.netkey_list.net_idx) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.netkey_list.net_idx,
                                           p_src_data->status_cb.netkey_list.net_idx->data,
                                           p_src_data->status_cb.netkey_list.net_idx->len);
                }
                break;
            case OP_APP_KEY_GET:
            case OP_APP_KEY_LIST:
                if (p_src_data->status_cb.appkey_list.app_idx) {
                    length = p_src_data->status_cb.appkey_list.app_idx->len;
                    p_dest_data->status_cb.appkey_list.app_idx = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.appkey_list.app_idx) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.appkey_list.app_idx,
                                           p_src_data->status_cb.appkey_list.app_idx->data,
                                           p_src_data->status_cb.appkey_list.app_idx->len);
                }
                break;
            case OP_SIG_MOD_APP_GET:
            case OP_VND_MOD_APP_GET:
            case OP_SIG_MOD_APP_LIST:
            case OP_VND_MOD_APP_LIST:
                if (p_src_data->status_cb.model_app_list.app_idx) {
                    length = p_src_data->status_cb.model_app_list.app_idx->len;
                    p_dest_data->status_cb.model_app_list.app_idx = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.model_app_list.app_idx) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.model_app_list.app_idx,
                                           p_src_data->status_cb.model_app_list.app_idx->data,
                                           p_src_data->status_cb.model_app_list.app_idx->len);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_config_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_cfg_client_cb_param_t *arg = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_cfg_client_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case OP_DEV_COMP_DATA_GET:
            case OP_DEV_COMP_DATA_STATUS:
                bt_mesh_free_buf(arg->status_cb.comp_data_status.composition_data);
                break;
            case OP_MOD_SUB_GET:
            case OP_MOD_SUB_GET_VND:
            case OP_MOD_SUB_LIST:
            case OP_MOD_SUB_LIST_VND:
                bt_mesh_free_buf(arg->status_cb.model_sub_list.sub_addr);
                break;
            case OP_NET_KEY_GET:
            case OP_NET_KEY_LIST:
                bt_mesh_free_buf(arg->status_cb.netkey_list.net_idx);
                break;
            case OP_APP_KEY_GET:
            case OP_APP_KEY_LIST:
                bt_mesh_free_buf(arg->status_cb.appkey_list.app_idx);
                break;
            case OP_SIG_MOD_APP_GET:
            case OP_VND_MOD_APP_GET:
            case OP_SIG_MOD_APP_LIST:
            case OP_VND_MOD_APP_LIST:
                bt_mesh_free_buf(arg->status_cb.model_app_list.app_idx);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_config_client_callback(esp_ble_mesh_cfg_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    BT_DBG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_CONFIG_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_CONFIG_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_cfg_client_cb_param_t),
                         btc_ble_mesh_config_client_copy_req_data);
}

void bt_mesh_config_client_cb_evt_to_btc(u32_t opcode, u8_t evt_type,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         const u8_t *val, size_t len)
{
    esp_ble_mesh_cfg_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0U;

    if (!model || !ctx) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_CONFIG_CLIENT_GET_STATE:
        act = ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_CONFIG_CLIENT_SET_STATE:
        act = ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_CONFIG_CLIENT_PUBLISH:
        act = ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT;
        break;
    case BTC_BLE_MESH_EVT_CONFIG_CLIENT_TIMEOUT:
        act = ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT;
        break;
    default:
        BT_ERR("Unknown Config client event type %d", evt_type);
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

    cb_params.error_code = 0;
    cb_params.params = &params;

    if (val && len) {
        memcpy(&cb_params.status_cb, val, MIN(len, sizeof(cb_params.status_cb)));
    }

    btc_ble_mesh_config_client_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_config_client_publish_callback(u32_t opcode, struct bt_mesh_model *model,
                                                 struct bt_mesh_msg_ctx *ctx,
                                                 struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_config_client_cb_evt_to_btc(opcode, BTC_BLE_MESH_EVT_CONFIG_CLIENT_PUBLISH,
                                        model, ctx, buf->data, buf->len);
    return;
}

static int btc_ble_mesh_config_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                                esp_ble_mesh_cfg_client_get_state_t *get)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET:
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET:
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET:
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET:
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_GET:
    case ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET:
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET:
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET:
    case ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET:
    case ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET:
        if (get == NULL) {
            BT_ERR("Invalid Configuration Get");
            return -EINVAL;
        }
        break;
    default:
        break;
    }

    param.opcode = params->opcode;
    param.model = (struct bt_mesh_model *)params->model;
    param.ctx.net_idx = params->ctx.net_idx;
    param.ctx.app_idx = BLE_MESH_KEY_DEV;
    param.ctx.addr = params->ctx.addr;
    param.ctx.send_rel = params->ctx.send_rel;
    param.ctx.send_ttl = params->ctx.send_ttl;
    param.msg_timeout = params->msg_timeout;
    param.msg_role = params->msg_role;

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_BEACON_GET:
        return bt_mesh_cfg_beacon_get(&param);
    case ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET:
        return bt_mesh_cfg_ttl_get(&param);
    case ESP_BLE_MESH_MODEL_OP_FRIEND_GET:
        return bt_mesh_cfg_friend_get(&param);
    case ESP_BLE_MESH_MODEL_OP_GATT_PROXY_GET:
        return bt_mesh_cfg_gatt_proxy_get(&param);
    case ESP_BLE_MESH_MODEL_OP_RELAY_GET:
        return bt_mesh_cfg_relay_get(&param);
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET:
        return bt_mesh_cfg_mod_pub_get(&param, get->model_pub_get.element_addr,
                                       get->model_pub_get.model_id,
                                       get->model_pub_get.company_id);
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET:
        return bt_mesh_cfg_hb_pub_get(&param);
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET:
        return bt_mesh_cfg_hb_sub_get(&param);
    case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET:
        return bt_mesh_cfg_comp_data_get(&param, get->comp_data_get.page);
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET:
        return bt_mesh_cfg_mod_sub_get(&param, get->sig_model_sub_get.element_addr,
                                       get->sig_model_sub_get.model_id);
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET:
        return bt_mesh_cfg_mod_sub_get_vnd(&param, get->vnd_model_sub_get.element_addr,
                                           get->vnd_model_sub_get.model_id,
                                           get->vnd_model_sub_get.company_id);
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_GET:
        return bt_mesh_cfg_net_key_get(&param);
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_GET:
        return bt_mesh_cfg_app_key_get(&param, get->app_key_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET:
        return bt_mesh_cfg_node_identity_get(&param, get->node_identity_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET:
        return bt_mesh_cfg_mod_app_get(&param, get->sig_model_app_get.element_addr,
                                       get->sig_model_app_get.model_id);
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET:
        return bt_mesh_cfg_mod_app_get_vnd(&param, get->vnd_model_app_get.element_addr,
                                           get->vnd_model_app_get.model_id,
                                           get->vnd_model_app_get.company_id);
    case ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET:
        return bt_mesh_cfg_kr_phase_get(&param, get->kr_phase_get.net_idx);
    case ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET:
        return bt_mesh_cfg_lpn_timeout_get(&param, get->lpn_pollto_get.lpn_addr);
    case ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_GET:
        return bt_mesh_cfg_net_transmit_get(&param);
    default:
        BT_ERR("Invalid Configuration Get opcode 0x%04x", param.opcode);
        return -EINVAL;
    }

    return 0;
}

static int btc_ble_mesh_config_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                                esp_ble_mesh_cfg_client_set_state_t *set)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (params->opcode != ESP_BLE_MESH_MODEL_OP_NODE_RESET && set == NULL) {
        BT_ERR("Invalid Configuration Set");
        return -EINVAL;
    }

    param.opcode = params->opcode;
    param.model = (struct bt_mesh_model *)params->model;
    param.ctx.net_idx = params->ctx.net_idx;
    param.ctx.app_idx = BLE_MESH_KEY_DEV;
    param.ctx.addr = params->ctx.addr;
    param.ctx.send_rel = params->ctx.send_rel;
    param.ctx.send_ttl = params->ctx.send_ttl;
    param.msg_timeout = params->msg_timeout;
    param.msg_role = params->msg_role;

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_BEACON_SET:
        return bt_mesh_cfg_beacon_set(&param, set->beacon_set.beacon);
    case ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET:
        return bt_mesh_cfg_ttl_set(&param, set->default_ttl_set.ttl);
    case ESP_BLE_MESH_MODEL_OP_FRIEND_SET:
        return bt_mesh_cfg_friend_set(&param, set->friend_set.friend_state);
    case ESP_BLE_MESH_MODEL_OP_GATT_PROXY_SET:
        return bt_mesh_cfg_gatt_proxy_set(&param, set->gatt_proxy_set.gatt_proxy);
    case ESP_BLE_MESH_MODEL_OP_RELAY_SET:
        return bt_mesh_cfg_relay_set(&param, set->relay_set.relay,
                                     set->relay_set.relay_retransmit);
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD:
        return bt_mesh_cfg_net_key_add(&param, set->net_key_add.net_idx,
                                       &set->net_key_add.net_key[0]);
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
        return bt_mesh_cfg_app_key_add(&param, set->app_key_add.net_idx,
                                       set->app_key_add.app_idx,
                                       &set->app_key_add.app_key[0]);
    case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
        return bt_mesh_cfg_mod_app_bind(&param, set->model_app_bind.element_addr,
                                        set->model_app_bind.model_app_idx,
                                        set->model_app_bind.model_id,
                                        set->model_app_bind.company_id);
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET: {
        struct bt_mesh_cfg_mod_pub model_pub = {
            .addr = set->model_pub_set.publish_addr,
            .app_idx = set->model_pub_set.publish_app_idx,
            .cred_flag = set->model_pub_set.cred_flag,
            .ttl = set->model_pub_set.publish_ttl,
            .period = set->model_pub_set.publish_period,
            .transmit = set->model_pub_set.publish_retransmit,
        };
        return bt_mesh_cfg_mod_pub_set(&param, set->model_pub_set.element_addr,
                                       set->model_pub_set.model_id,
                                       set->model_pub_set.company_id, &model_pub);
    }
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD:
        return bt_mesh_cfg_mod_sub_add(&param, set->model_sub_add.element_addr,
                                       set->model_sub_add.sub_addr,
                                       set->model_sub_add.model_id,
                                       set->model_sub_add.company_id);
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE:
        return bt_mesh_cfg_mod_sub_del(&param, set->model_sub_delete.element_addr,
                                       set->model_sub_delete.sub_addr,
                                       set->model_sub_delete.model_id,
                                       set->model_sub_delete.company_id);
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE:
        return bt_mesh_cfg_mod_sub_overwrite(&param, set->model_sub_overwrite.element_addr,
                                             set->model_sub_overwrite.sub_addr,
                                             set->model_sub_overwrite.model_id,
                                             set->model_sub_overwrite.company_id);
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD:
        return bt_mesh_cfg_mod_sub_va_add(&param, set->model_sub_va_add.element_addr,
                                          &set->model_sub_va_add.label_uuid[0],
                                          set->model_sub_va_add.model_id,
                                          set->model_sub_va_add.company_id);
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE:
        return bt_mesh_cfg_mod_sub_va_overwrite(&param, set->model_sub_va_overwrite.element_addr,
                                                &set->model_sub_va_overwrite.label_uuid[0],
                                                set->model_sub_va_overwrite.model_id,
                                                set->model_sub_va_overwrite.company_id);
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE:
        return bt_mesh_cfg_mod_sub_va_del(&param, set->model_sub_va_delete.element_addr,
                                          &set->model_sub_va_delete.label_uuid[0],
                                          set->model_sub_va_delete.model_id,
                                          set->model_sub_va_delete.company_id);
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET:
        return bt_mesh_cfg_hb_sub_set(&param, (struct bt_mesh_cfg_hb_sub *)&set->heartbeat_sub_set);
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET:
        return bt_mesh_cfg_hb_pub_set(&param, (struct bt_mesh_cfg_hb_pub *)&set->heartbeat_pub_set);
    case ESP_BLE_MESH_MODEL_OP_NODE_RESET:
        return bt_mesh_cfg_node_reset(&param);
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_VIRTUAL_ADDR_SET: {
        struct bt_mesh_cfg_mod_pub model_pub = {
            .app_idx = set->model_pub_va_set.publish_app_idx,
            .cred_flag = set->model_pub_va_set.cred_flag,
            .ttl = set->model_pub_va_set.publish_ttl,
            .period = set->model_pub_va_set.publish_period,
            .transmit = set->model_pub_va_set.publish_retransmit,
        };
        return bt_mesh_cfg_mod_pub_va_set(&param, set->model_pub_va_set.element_addr,
                                          set->model_pub_va_set.model_id,
                                          set->model_pub_va_set.company_id,
                                          set->model_pub_va_set.label_uuid, &model_pub);
    }
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL:
        return bt_mesh_cfg_mod_sub_del_all(&param, set->model_sub_delete_all.element_addr,
                                           set->model_sub_delete_all.model_id,
                                           set->model_sub_delete_all.company_id);
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_UPDATE:
        return bt_mesh_cfg_net_key_update(&param, set->net_key_update.net_idx,
                                          set->net_key_update.net_key);
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_DELETE:
        return bt_mesh_cfg_net_key_delete(&param, set->net_key_delete.net_idx);
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_UPDATE:
        return bt_mesh_cfg_app_key_update(&param, set->app_key_update.net_idx,
                                          set->app_key_update.app_idx,
                                          set->app_key_update.app_key);
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_DELETE:
        return bt_mesh_cfg_app_key_delete(&param, set->app_key_delete.net_idx,
                                          set->app_key_delete.app_idx);
    case ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_SET:
        return bt_mesh_cfg_node_identity_set(&param, set->node_identity_set.net_idx,
                                             set->node_identity_set.identity);
    case ESP_BLE_MESH_MODEL_OP_MODEL_APP_UNBIND:
        return bt_mesh_cfg_mod_app_unbind(&param, set->model_app_unbind.element_addr,
                                          set->model_app_unbind.model_app_idx,
                                          set->model_app_unbind.model_id,
                                          set->model_app_unbind.company_id);
    case ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_SET:
        return bt_mesh_cfg_kr_phase_set(&param, set->kr_phase_set.net_idx,
                                        set->kr_phase_set.transition);
    case ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_SET:
        return bt_mesh_cfg_net_transmit_set(&param, set->net_transmit_set.net_transmit);
    default:
        BT_ERR("Invalid Configuration Set opcode 0x%04x", param.opcode);
        return -EINVAL;
    }

    return 0;
}

void btc_ble_mesh_config_client_call_handler(btc_msg_t *msg)
{
    btc_ble_mesh_config_client_args_t *arg = NULL;
    esp_ble_mesh_cfg_client_cb_param_t cb = {0};

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_config_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_GET_STATE: {
        cb.params = arg->cfg_client_get_state.params;
        cb.error_code = btc_ble_mesh_config_client_get_state(arg->cfg_client_get_state.params,
                                                             arg->cfg_client_get_state.get_state);
        if (cb.error_code) {
            btc_ble_mesh_config_client_callback(&cb, ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_SET_STATE: {
        cb.params = arg->cfg_client_set_state.params;
        cb.error_code = btc_ble_mesh_config_client_set_state(arg->cfg_client_set_state.params,
                                                             arg->cfg_client_set_state.set_state);
        if (cb.error_code) {
            btc_ble_mesh_config_client_callback(&cb, ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT);
        }
        break;
    }
    default:
        break;
    }

    btc_ble_mesh_config_client_arg_deep_free(msg);
    return;
}

void btc_ble_mesh_config_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_cfg_client_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_cfg_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_CFG_CLIENT_EVT_MAX) {
        btc_ble_mesh_config_client_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_config_client_free_req_data(msg);
    return;
}

#endif /* CONFIG_BLE_MESH_CFG_CLI */

/* Configuration Server Model related functions */

static inline void btc_ble_mesh_config_server_cb_to_app(esp_ble_mesh_cfg_server_cb_event_t event,
                                                        esp_ble_mesh_cfg_server_cb_param_t *param)
{
    esp_ble_mesh_cfg_server_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_cfg_server_cb_t)btc_profile_cb_get(BTC_PID_CONFIG_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_config_server_callback(esp_ble_mesh_cfg_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    BT_DBG("%s", __func__);

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_CONFIG_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_CONFIG_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_cfg_server_cb_param_t), NULL);
}

void bt_mesh_config_server_cb_evt_to_btc(u8_t evt_type, struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         const u8_t *val, size_t len)
{
    esp_ble_mesh_cfg_server_cb_param_t cb_params = {0};
    uint8_t act = 0U;

    if (!model || !ctx) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_CONFIG_SERVER_STATE_CHANGE:
        act = ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT;
        break;
    default:
        BT_ERR("Unknown Config server event type %d", evt_type);
        return;
    }

    cb_params.model = (esp_ble_mesh_model_t *)model;
    cb_params.ctx.net_idx = ctx->net_idx;
    cb_params.ctx.app_idx = ctx->app_idx;
    cb_params.ctx.addr = ctx->addr;
    cb_params.ctx.recv_ttl = ctx->recv_ttl;
    cb_params.ctx.recv_op = ctx->recv_op;
    cb_params.ctx.recv_dst = ctx->recv_dst;
    cb_params.ctx.recv_rssi = ctx->recv_rssi;
    cb_params.ctx.send_ttl = ctx->send_ttl;

    if (val && len) {
        memcpy(&cb_params.value, val, MIN(len, sizeof(cb_params.value)));
    }

    btc_ble_mesh_config_server_callback(&cb_params, act);
    return;
}

void btc_ble_mesh_config_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_cfg_server_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_cfg_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_CFG_SERVER_EVT_MAX) {
        btc_ble_mesh_config_server_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }
}
