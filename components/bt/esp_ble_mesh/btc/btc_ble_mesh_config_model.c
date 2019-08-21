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

#include "btc/btc_manage.h"
#include "osi/allocator.h"

#include "cfg_cli.h"
#include "mesh_common.h"
#include "btc_ble_mesh_config_model.h"
#include "esp_ble_mesh_config_model_api.h"

#define CID_NVAL 0xffff

extern s32_t config_msg_timeout;

static inline void btc_ble_mesh_cfg_client_cb_to_app(esp_ble_mesh_cfg_client_cb_event_t event,
        esp_ble_mesh_cfg_client_cb_param_t *param)
{
    esp_ble_mesh_cfg_client_cb_t btc_mesh_cb = (esp_ble_mesh_cfg_client_cb_t)btc_profile_cb_get(BTC_PID_CFG_CLIENT);
    if (btc_mesh_cb) {
        btc_mesh_cb(event, param);
    }
}

static inline void btc_ble_mesh_cfg_server_cb_to_app(esp_ble_mesh_cfg_server_cb_event_t event,
        esp_ble_mesh_cfg_server_cb_param_t *param)
{
    esp_ble_mesh_cfg_server_cb_t btc_mesh_cb = (esp_ble_mesh_cfg_server_cb_t)btc_profile_cb_get(BTC_PID_CFG_SERVER);
    if (btc_mesh_cb) {
        btc_mesh_cb(event, param);
    }
}

void btc_ble_mesh_cfg_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_cfg_client_args_t *dst = (btc_ble_mesh_cfg_client_args_t *)p_dest;
    btc_ble_mesh_cfg_client_args_t *src = (btc_ble_mesh_cfg_client_args_t *)p_src;

    if (!msg || !dst || !src) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_GET_STATE: {
        dst->cfg_client_get_state.params = (esp_ble_mesh_client_common_param_t *)osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->cfg_client_get_state.get_state = (esp_ble_mesh_cfg_client_get_state_t *)osi_malloc(sizeof(esp_ble_mesh_cfg_client_get_state_t));
        if (dst->cfg_client_get_state.params && dst->cfg_client_get_state.get_state) {
            memcpy(dst->cfg_client_get_state.params, src->cfg_client_get_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->cfg_client_get_state.get_state, src->cfg_client_get_state.get_state,
                   sizeof(esp_ble_mesh_cfg_client_get_state_t));
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_SET_STATE: {
        dst->cfg_client_set_state.params = (esp_ble_mesh_client_common_param_t *)osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
        dst->cfg_client_set_state.set_state = (esp_ble_mesh_cfg_client_set_state_t *)osi_malloc(sizeof(esp_ble_mesh_cfg_client_set_state_t));
        if (dst->cfg_client_set_state.params && dst->cfg_client_set_state.set_state) {
            memcpy(dst->cfg_client_set_state.params, src->cfg_client_set_state.params,
                   sizeof(esp_ble_mesh_client_common_param_t));
            memcpy(dst->cfg_client_set_state.set_state, src->cfg_client_set_state.set_state,
                   sizeof(esp_ble_mesh_cfg_client_set_state_t));
        } else {
            LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
        }
        break;
    }
    default:
        LOG_DEBUG("%s, Unknown deep copy act %d", __func__, msg->act);
        break;
    }
}

static void btc_ble_mesh_cfg_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_cfg_client_cb_param_t *p_dest_data = (esp_ble_mesh_cfg_client_cb_param_t *)p_dest;
    esp_ble_mesh_cfg_client_cb_param_t *p_src_data = (esp_ble_mesh_cfg_client_cb_param_t *)p_src;
    u32_t opcode;
    u16_t length;

    if (!msg || !p_src_data || !p_dest_data) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        if (p_src_data->params) {
            opcode = p_src_data->params->opcode;
            switch (opcode) {
            case OP_DEV_COMP_DATA_GET:
            case OP_DEV_COMP_DATA_STATUS:
                if (p_src_data->status_cb.comp_data_status.composition_data) {
                    length = p_src_data->status_cb.comp_data_status.composition_data->len;
                    p_dest_data->status_cb.comp_data_status.composition_data = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.comp_data_status.composition_data) {
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
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
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
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
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
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
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
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
                        LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
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
        if (p_src_data->params) {
            p_dest_data->params = osi_malloc(sizeof(esp_ble_mesh_client_common_param_t));
            if (p_dest_data->params) {
                memcpy(p_dest_data->params, p_src_data->params, sizeof(esp_ble_mesh_client_common_param_t));
            } else {
                LOG_ERROR("%s, Failed to allocate memory, act %d", __func__, msg->act);
            }
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_cfg_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_cfg_client_cb_param_t *arg = NULL;
    u32_t opcode;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_cfg_client_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        if (arg->params) {
            opcode = arg->params->opcode;
            switch (opcode) {
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
            osi_free(arg->params);
        }
        break;
    default:
        break;
    }
}

void btc_ble_mesh_cfg_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_cfg_client_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_cfg_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_GET_STATE:
        if (arg->cfg_client_get_state.params) {
            osi_free(arg->cfg_client_get_state.params);
        }
        if (arg->cfg_client_get_state.get_state) {
            osi_free(arg->cfg_client_get_state.get_state);
        }
        break;
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_SET_STATE:
        if (arg->cfg_client_set_state.params) {
            osi_free(arg->cfg_client_set_state.params);
        }
        if (arg->cfg_client_set_state.set_state) {
            osi_free(arg->cfg_client_set_state.set_state);
        }
        break;
    default:
        break;
    }

    return;
}

static void btc_mesh_cfg_client_callback(esp_ble_mesh_cfg_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    LOG_DEBUG("%s", __func__);

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_CFG_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params,
                         sizeof(esp_ble_mesh_cfg_client_cb_param_t), btc_ble_mesh_cfg_client_copy_req_data);
}

void bt_mesh_callback_config_status_to_btc(u32_t opcode, u8_t evt_type,
        struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        const u8_t *val, size_t len)
{
    esp_ble_mesh_cfg_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    size_t  length;
    uint8_t act;

    if (!model || !ctx) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case 0x00:
        act = ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT;
        break;
    case 0x01:
        act = ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT;
        break;
    case 0x02:
        act = ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT;
        break;
    case 0x03:
        act = ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT;
        break;
    default:
        LOG_ERROR("%s, Unknown config client event type %d", __func__, evt_type);
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

    cb_params.error_code = 0;
    cb_params.params = &params;

    if (val && len) {
        length = (len <= sizeof(cb_params.status_cb)) ? len : sizeof(cb_params.status_cb);
        memcpy(&cb_params.status_cb, val, length);
    }

    btc_mesh_cfg_client_callback(&cb_params, act);
}


void btc_mesh_cfg_client_publish_callback(u32_t opcode, struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_callback_config_status_to_btc(opcode, 0x02, model, ctx, buf->data, buf->len);
}

void btc_mesh_cfg_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_cfg_client_cb_param_t cfg_client_cb = {0};
    btc_ble_mesh_cfg_client_args_t *arg = NULL;
    bt_mesh_role_param_t role_param = {0};

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_cfg_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_GET_STATE: {
        cfg_client_cb.params = arg->cfg_client_get_state.params;
        role_param.model = (struct bt_mesh_model *)cfg_client_cb.params->model;
        role_param.role = cfg_client_cb.params->msg_role;
        if (bt_mesh_set_model_role(&role_param)) {
            LOG_ERROR("%s, Failed to set model role", __func__);
            return;
        }
        btc_ble_mesh_config_client_get_state(arg->cfg_client_get_state.params,
                                             arg->cfg_client_get_state.get_state,
                                             &cfg_client_cb);
        if (cfg_client_cb.error_code) {
            btc_mesh_cfg_client_callback(&cfg_client_cb, ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT);
        }
        break;
    }
    case BTC_BLE_MESH_ACT_CONFIG_CLIENT_SET_STATE: {
        cfg_client_cb.params = arg->cfg_client_set_state.params;
        role_param.model = (struct bt_mesh_model *)cfg_client_cb.params->model;
        role_param.role = cfg_client_cb.params->msg_role;
        if (bt_mesh_set_model_role(&role_param)) {
            LOG_ERROR("%s, Failed to set model role", __func__);
            return;
        }
        btc_ble_mesh_config_client_set_state(arg->cfg_client_set_state.params,
                                             arg->cfg_client_set_state.set_state,
                                             &cfg_client_cb);
        if (cfg_client_cb.error_code) {
            btc_mesh_cfg_client_callback(&cfg_client_cb, ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT);
        }
        break;
    }
    default:
        break;
    }

    btc_ble_mesh_cfg_client_arg_deep_free(msg);
}

void btc_mesh_cfg_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_cfg_client_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_cfg_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_CFG_CLIENT_EVT_MAX) {
        btc_ble_mesh_cfg_client_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }

    btc_ble_mesh_cfg_client_free_req_data(msg);
}

int btc_ble_mesh_config_client_get_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_cfg_client_get_state_t *get_state,
        esp_ble_mesh_cfg_client_cb_param_t *cfg_client_cb)
{
    struct bt_mesh_msg_ctx ctx = {0};

    if (!params || !cfg_client_cb) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    ctx.net_idx = params->ctx.net_idx;
    ctx.app_idx = BLE_MESH_KEY_DEV;
    ctx.addr = params->ctx.addr;
    ctx.send_rel = params->ctx.send_rel;
    ctx.send_ttl = params->ctx.send_ttl;

    config_msg_timeout = params->msg_timeout;

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_BEACON_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_beacon_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_ttl_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_FRIEND_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_friend_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_GATT_PROXY_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_gatt_proxy_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_RELAY_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_relay_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_pub_get(&ctx, get_state->model_pub_get.element_addr, get_state->model_pub_get.model_id,
                                            get_state->model_pub_get.company_id));
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_hb_pub_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_hb_sub_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_comp_data_get(&ctx, get_state->comp_data_get.page));
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_get(&ctx, get_state->sig_model_sub_get.element_addr, get_state->sig_model_sub_get.model_id));
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_get_vnd(&ctx, get_state->vnd_model_sub_get.element_addr,
                                                get_state->vnd_model_sub_get.model_id, get_state->vnd_model_sub_get.company_id));
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_net_key_get(&ctx));
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_app_key_get(&ctx, get_state->app_key_get.net_idx));
    case ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_node_identity_get(&ctx, get_state->node_identity_get.net_idx));
    case ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_app_get(&ctx, get_state->sig_model_app_get.element_addr, get_state->sig_model_app_get.model_id));
    case ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_app_get_vnd(&ctx, get_state->vnd_model_app_get.element_addr,
                                                get_state->vnd_model_app_get.model_id, get_state->vnd_model_app_get.company_id));
    case ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_kr_phase_get(&ctx, get_state->kr_phase_get.net_idx));
    case ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_lpn_timeout_get(&ctx, get_state->lpn_pollto_get.lpn_addr));
    case ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_GET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_net_transmit_get(&ctx));
    default:
        BT_WARN("%s, Invalid opcode 0x%x", __func__, params->opcode);
        return (cfg_client_cb->error_code = -EINVAL);
    }

    return 0;
}

int btc_ble_mesh_config_client_set_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_cfg_client_set_state_t *set_state,
        esp_ble_mesh_cfg_client_cb_param_t *cfg_client_cb)
{
    struct bt_mesh_msg_ctx ctx = {0};

    if (!params || !set_state || !cfg_client_cb) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    ctx.net_idx = params->ctx.net_idx;
    ctx.app_idx = BLE_MESH_KEY_DEV;
    ctx.addr = params->ctx.addr;
    ctx.send_rel = params->ctx.send_rel;
    ctx.send_ttl = params->ctx.send_ttl;

    config_msg_timeout = params->msg_timeout;

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_BEACON_SET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_beacon_set(&ctx, set_state->beacon_set.beacon));
    case ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_ttl_set(&ctx, set_state->default_ttl_set.ttl));
    case ESP_BLE_MESH_MODEL_OP_FRIEND_SET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_friend_set(&ctx, set_state->friend_set.friend_state));
    case ESP_BLE_MESH_MODEL_OP_GATT_PROXY_SET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_gatt_proxy_set(&ctx, set_state->gatt_proxy_set.gatt_proxy));
    case ESP_BLE_MESH_MODEL_OP_RELAY_SET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_relay_set(&ctx, set_state->relay_set.relay, set_state->relay_set.relay_retransmit));
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_net_key_add(&ctx, set_state->net_key_add.net_idx, &set_state->net_key_add.net_key[0]));
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_app_key_add(&ctx, set_state->app_key_add.net_idx,
                                            set_state->app_key_add.app_idx, &set_state->app_key_add.app_key[0]));
    case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_app_bind(&ctx, set_state->model_app_bind.element_addr, set_state->model_app_bind.model_app_idx,
                                             set_state->model_app_bind.model_id, set_state->model_app_bind.company_id));
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET: {
        struct bt_mesh_cfg_mod_pub model_pub = {
            .addr = set_state->model_pub_set.publish_addr,
            .app_idx = set_state->model_pub_set.publish_app_idx,
            .cred_flag = set_state->model_pub_set.cred_flag,
            .ttl = set_state->model_pub_set.publish_ttl,
            .period = set_state->model_pub_set.publish_period,
            .transmit = set_state->model_pub_set.publish_retransmit,
        };
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_pub_set(&ctx, set_state->model_pub_set.element_addr, set_state->model_pub_set.model_id,
                                            set_state->model_pub_set.company_id, &model_pub));
    }
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_add(&ctx, set_state->model_sub_add.element_addr, set_state->model_sub_add.sub_addr,
                                            set_state->model_sub_add.model_id, set_state->model_sub_add.company_id));
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_del(&ctx, set_state->model_sub_delete.element_addr, set_state->model_sub_delete.sub_addr,
                                            set_state->model_sub_delete.model_id, set_state->model_sub_delete.company_id));
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_overwrite(&ctx, set_state->model_sub_overwrite.element_addr, set_state->model_sub_overwrite.sub_addr,
                                                  set_state->model_sub_overwrite.model_id, set_state->model_sub_overwrite.company_id));
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_va_add(&ctx, set_state->model_sub_va_add.element_addr, &set_state->model_sub_va_add.label_uuid[0],
                                               set_state->model_sub_va_add.model_id, set_state->model_sub_va_add.company_id));
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_va_overwrite(&ctx, set_state->model_sub_va_overwrite.element_addr, &set_state->model_sub_va_overwrite.label_uuid[0],
                                                     set_state->model_sub_va_overwrite.model_id, set_state->model_sub_va_overwrite.company_id));
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_va_del(&ctx, set_state->model_sub_va_delete.element_addr, &set_state->model_sub_va_delete.label_uuid[0],
                                               set_state->model_sub_va_delete.model_id, set_state->model_sub_va_delete.company_id));
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_hb_sub_set(&ctx, (struct bt_mesh_cfg_hb_sub *)&set_state->heartbeat_sub_set));
    case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_hb_pub_set(&ctx, (const struct bt_mesh_cfg_hb_pub *)&set_state->heartbeat_pub_set));
    case ESP_BLE_MESH_MODEL_OP_NODE_RESET:
        return (cfg_client_cb->error_code = bt_mesh_cfg_node_reset(&ctx));
    case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_VIRTUAL_ADDR_SET: {
        struct bt_mesh_cfg_mod_pub model_pub = {
            .app_idx = set_state->model_pub_va_set.publish_app_idx,
            .cred_flag = set_state->model_pub_va_set.cred_flag,
            .ttl = set_state->model_pub_va_set.publish_ttl,
            .period = set_state->model_pub_va_set.publish_period,
            .transmit = set_state->model_pub_va_set.publish_retransmit,
        };
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_pub_va_set(&ctx, set_state->model_pub_va_set.element_addr, set_state->model_pub_va_set.model_id,
                                               set_state->model_pub_va_set.company_id, set_state->model_pub_va_set.label_uuid, &model_pub));
    }
    case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_sub_del_all(&ctx, set_state->model_sub_delete_all.element_addr,
                                                set_state->model_sub_delete_all.model_id, set_state->model_sub_delete_all.company_id));
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_UPDATE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_net_key_update(&ctx, set_state->net_key_update.net_idx, set_state->net_key_update.net_key));
    case ESP_BLE_MESH_MODEL_OP_NET_KEY_DELETE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_net_key_delete(&ctx, set_state->net_key_delete.net_idx));
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_UPDATE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_app_key_update(&ctx, set_state->app_key_update.net_idx, set_state->app_key_update.app_idx,
                                               set_state->app_key_update.app_key));
    case ESP_BLE_MESH_MODEL_OP_APP_KEY_DELETE:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_app_key_delete(&ctx, set_state->app_key_delete.net_idx, set_state->app_key_delete.app_idx));
    case ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_SET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_node_identity_set(&ctx, set_state->node_identity_set.net_idx, set_state->node_identity_set.identity));
    case ESP_BLE_MESH_MODEL_OP_MODEL_APP_UNBIND:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_mod_app_unbind(&ctx, set_state->model_app_unbind.element_addr, set_state->model_app_unbind.model_app_idx,
                                               set_state->model_app_unbind.model_id, set_state->model_app_unbind.company_id));
    case ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_SET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_kr_phase_set(&ctx, set_state->kr_phase_set.net_idx, set_state->kr_phase_set.transition));
    case ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_SET:
        return (cfg_client_cb->error_code =
                    bt_mesh_cfg_net_transmit_set(&ctx, set_state->net_transmit_set.net_transmit));
    default:
        BT_WARN("%s, Invalid opcode 0x%x", __func__, params->opcode);
        return (cfg_client_cb->error_code = -EINVAL);
    }

    return 0;
}

static void btc_mesh_cfg_server_callback(esp_ble_mesh_cfg_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    LOG_DEBUG("%s", __func__);

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_CFG_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_cfg_server_cb_param_t), NULL);
}

void bt_mesh_callback_cfg_server_event_to_btc(u8_t evt_type, struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        const u8_t *val, size_t len)
{
    esp_ble_mesh_cfg_server_cb_param_t cb_params = {0};
    size_t  length;
    uint8_t act;

    if (!model || !ctx) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case 0x00:
        act = ESP_BLE_MESH_CFG_SERVER_RECV_MSG_EVT;
        break;
    default:
        LOG_ERROR("%s, Unknown config server event type %d", __func__, evt_type);
        return;
    }

    cb_params.model = (esp_ble_mesh_model_t *)model;
    cb_params.ctx.net_idx = ctx->net_idx;
    cb_params.ctx.app_idx = ctx->app_idx;
    cb_params.ctx.addr = ctx->addr;
    cb_params.ctx.recv_ttl = ctx->recv_ttl;
    cb_params.ctx.recv_op = ctx->recv_op;
    cb_params.ctx.recv_dst = ctx->recv_dst;

    if (val && len) {
        length = (len <= sizeof(cb_params.status_cb)) ? len : sizeof(cb_params.status_cb);
        memcpy(&cb_params.status_cb, val, length);
    }

    btc_mesh_cfg_server_callback(&cb_params, act);
}

void btc_mesh_cfg_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_cfg_server_cb_param_t *param = NULL;

    if (!msg || !msg->arg) {
        LOG_ERROR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_cfg_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_CFG_SERVER_EVT_MAX) {
        btc_ble_mesh_cfg_server_cb_to_app(msg->act, param);
    } else {
        LOG_ERROR("%s, Unknown msg->act = %d", __func__, msg->act);
    }
}
