/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_rpr_model.h"
#include "esp_ble_mesh_rpr_model_api.h"

#if CONFIG_BLE_MESH_RPR_CLI

extern int bt_mesh_rpr_scan_caps_get(void *param);
extern int bt_mesh_rpr_scan_get(void *param);
extern int bt_mesh_rpr_scan_start(void *param, void *start);
extern int bt_mesh_rpr_scan_stop(void *param);
extern int bt_mesh_rpr_ext_scan_start(void *param, void *start);
extern int bt_mesh_rpr_link_get(void *param);
extern int bt_mesh_rpr_link_open(void *param, void *open);
extern int bt_mesh_rpr_link_close(void *param, uint8_t reason);
extern int bt_mesh_rpr_start_prov(void *model, uint16_t rp_srv_addr);

/* Remote Provisioning Client model related functions */

static inline void btc_ble_mesh_rpr_client_cb_to_app(esp_ble_mesh_rpr_client_cb_event_t event,
                                                     esp_ble_mesh_rpr_client_cb_param_t *param)
{
    esp_ble_mesh_rpr_client_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_RPR_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_rpr_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_rpr_client_args_t *dst = p_dest;
    btc_ble_mesh_rpr_client_args_t *src = p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_RPR_CLIENT_SEND:
        dst->rpr_send.params = NULL;
        dst->rpr_send.msg = NULL;

        dst->rpr_send.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!dst->rpr_send.params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            break;
        }
        memcpy(dst->rpr_send.params, src->rpr_send.params, sizeof(esp_ble_mesh_client_common_param_t));

        if (src->rpr_send.msg) {
            dst->rpr_send.msg = bt_mesh_calloc(sizeof(esp_ble_mesh_rpr_client_msg_t));
            if (!dst->rpr_send.msg) {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                /* Free the previously allocated resources */
                bt_mesh_free(dst->rpr_send.params);
                dst->rpr_send.params = NULL;
                break;
            }
            memcpy(dst->rpr_send.msg, src->rpr_send.msg, sizeof(esp_ble_mesh_rpr_client_msg_t));
        }
        break;
    case BTC_BLE_MESH_ACT_RPR_CLIENT_ACT:
        dst->rpr_act.param = bt_mesh_calloc(sizeof(esp_ble_mesh_rpr_client_act_param_t));
        if (dst->rpr_act.param) {
            memcpy(dst->rpr_act.param, src->rpr_act.param,
                   sizeof(esp_ble_mesh_rpr_client_act_param_t));
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_rpr_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_rpr_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_rpr_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_RPR_CLIENT_SEND:
        if (arg->rpr_send.params) {
            bt_mesh_free(arg->rpr_send.params);
        }
        if (arg->rpr_send.msg) {
            bt_mesh_free(arg->rpr_send.msg);
        }
        break;
    case BTC_BLE_MESH_ACT_RPR_CLIENT_ACT:
        if (arg->rpr_act.param) {
            bt_mesh_free(arg->rpr_act.param);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_rpr_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_rpr_client_cb_param_t *p_dest_data = p_dest;
    esp_ble_mesh_rpr_client_cb_param_t *p_src_data = p_src;
    uint16_t length = 0;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_RPR_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_RPR_CLIENT_SEND_TIMEOUT_EVT:
        if (p_src_data->send.params) {
            p_dest_data->send.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
            if (!p_dest_data->send.params) {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                return;
            }

            memcpy(p_dest_data->send.params, p_src_data->send.params, sizeof(esp_ble_mesh_client_common_param_t));
        }
        break;
    case ESP_BLE_MESH_RPR_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_RPR_CLIENT_RECV_PUB_EVT:
        if (p_src_data->recv.params) {
            p_dest_data->recv.params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
            if (!p_dest_data->recv.params) {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                return;
            }

            memcpy(p_dest_data->recv.params, p_src_data->recv.params, sizeof(esp_ble_mesh_client_common_param_t));

            /* Remote Provisioning Extended Scan Start is an unacknowledged message,
             * so the corresponding report could only be received through the recv
             * publish event.
             */
            if (msg->act == ESP_BLE_MESH_RPR_CLIENT_RECV_PUB_EVT &&
                p_src_data->recv.params->opcode == ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_REPORT) {
                if (p_src_data->recv.val.ext_scan_report.adv_structures) {
                    length = p_src_data->recv.val.ext_scan_report.adv_structures->len;
                    p_dest_data->recv.val.ext_scan_report.adv_structures = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->recv.val.ext_scan_report.adv_structures) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->recv.params);
                        p_dest_data->recv.params = NULL;
                        return;
                    }

                    net_buf_simple_add_mem(p_dest_data->recv.val.ext_scan_report.adv_structures,
                                           p_src_data->recv.val.ext_scan_report.adv_structures->data,
                                           p_src_data->recv.val.ext_scan_report.adv_structures->len);
                }
            }
        }
        break;
    case ESP_BLE_MESH_RPR_CLIENT_ACT_COMP_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_rpr_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_rpr_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_rpr_client_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_RPR_CLIENT_SEND_COMP_EVT:
    case ESP_BLE_MESH_RPR_CLIENT_SEND_TIMEOUT_EVT:
        if (arg->send.params) {
            bt_mesh_free(arg->send.params);
        }
        break;
    case ESP_BLE_MESH_RPR_CLIENT_RECV_RSP_EVT:
    case ESP_BLE_MESH_RPR_CLIENT_RECV_PUB_EVT:
        if (arg->recv.params &&
            msg->act == ESP_BLE_MESH_RPR_CLIENT_RECV_PUB_EVT &&
            arg->recv.params->opcode == ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_REPORT) {
            bt_mesh_free_buf(arg->recv.val.ext_scan_report.adv_structures);
        }
        if (arg->recv.params) {
            bt_mesh_free(arg->recv.params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_rpr_client_cb(esp_ble_mesh_rpr_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_RPR_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_RPR_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_rpr_client_cb_param_t),
                         btc_ble_mesh_rpr_client_copy_req_data,
                         btc_ble_mesh_rpr_client_free_req_data);
}

void bt_mesh_rpr_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const void *val, size_t len)
{
    esp_ble_mesh_rpr_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0;

    if (model == NULL || ctx == NULL ||
        ((event == BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_RSP ||
          event == BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_PUB) &&
         (len > sizeof(cb_params.recv.val)))) {
        BT_ERR("%s, Invalid parameter", __func__);
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

    switch (event) {
    case BTC_BLE_MESH_EVT_RPR_CLIENT_SEND_TIMEOUT:
        act = ESP_BLE_MESH_RPR_CLIENT_SEND_TIMEOUT_EVT;
        cb_params.send.params = &params;
        break;
    case BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_RSP:
        act = ESP_BLE_MESH_RPR_CLIENT_RECV_RSP_EVT;
        cb_params.recv.params = &params;
        if (val && len) {
            memcpy(&cb_params.recv.val, val, len);
        }
        break;
    case BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_PUB:
        act = ESP_BLE_MESH_RPR_CLIENT_RECV_PUB_EVT;
        cb_params.recv.params = &params;
        if (val && len) {
            memcpy(&cb_params.recv.val, val, len);
        }
        break;
    default:
        BT_ERR("Unknown Remote Provisioning client event type %d", event);
        return;
    }

    btc_ble_mesh_rpr_client_cb(&cb_params, act);
}

void btc_ble_mesh_rpr_client_recv_pub_cb(uint32_t opcode,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_rpr_client_cb_evt_to_btc(opcode, BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_PUB,
                                     model, ctx, buf->data, buf->len);
}

void btc_ble_mesh_rpr_client_link_close_cb(struct bt_mesh_model *model,
                                           uint16_t rpr_srv_addr, uint8_t reason)
{
    esp_ble_mesh_rpr_client_cb_param_t cb_params = {0};

    cb_params.link_close.model = (esp_ble_mesh_model_t *)model;
    cb_params.link_close.rpr_srv_addr = rpr_srv_addr;
    cb_params.link_close.reason = reason;

    btc_ble_mesh_rpr_client_cb(&cb_params, ESP_BLE_MESH_RPR_CLIENT_LINK_CLOSE_EVT);
}

void btc_ble_mesh_rpr_client_prov_comp_cb(struct bt_mesh_model *model, uint16_t rpr_srv_addr,
                                          uint8_t nppi, uint16_t index, uint8_t uuid[16],
                                          uint16_t unicast_addr, uint8_t element_num,
                                          uint16_t net_idx)
{
    esp_ble_mesh_rpr_client_cb_param_t cb_params = {0};

    cb_params.prov.model = (esp_ble_mesh_model_t *)model;
    cb_params.prov.rpr_srv_addr = rpr_srv_addr;
    cb_params.prov.nppi = nppi;
    cb_params.prov.index = index;
    cb_params.prov.unicast_addr = unicast_addr;
    cb_params.prov.element_num = element_num;
    cb_params.prov.net_idx = net_idx;
    memcpy(cb_params.prov.uuid, uuid, 16);

    btc_ble_mesh_rpr_client_cb(&cb_params, ESP_BLE_MESH_RPR_CLIENT_PROV_COMP_EVT);
}

static int btc_ble_mesh_rpr_client_send(esp_ble_mesh_client_common_param_t *params,
                                        esp_ble_mesh_rpr_client_msg_t *msg)
{
    bt_mesh_client_common_param_t param = {0};

    if (params == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    switch (params->opcode) {
    case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_START:
    case ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_START:
    case ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN:
    case ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE:
        if (msg == NULL) {
            BT_ERR("Invalid Remote Provisioning message opcode 0x%04x", params->opcode);
            return -EINVAL;
        }
        break;
    default:
        break;
    }

    btc_ble_mesh_set_client_common_param(params, &param, true);

    switch (param.opcode) {
    case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_CAPS_GET:
        return bt_mesh_rpr_scan_caps_get(&param);
    case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_GET:
        return bt_mesh_rpr_scan_get(&param);
    case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_START:
        return bt_mesh_rpr_scan_start(&param, &msg->scan_start);
    case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_STOP:
        return bt_mesh_rpr_scan_stop(&param);
    case ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_START:
        return bt_mesh_rpr_ext_scan_start(&param, &msg->ext_scan_start);
    case ESP_BLE_MESH_MODEL_OP_RPR_LINK_GET:
        return bt_mesh_rpr_link_get(&param);
    case ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN:
        return bt_mesh_rpr_link_open(&param, &msg->link_open);
    case ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE:
        return bt_mesh_rpr_link_close(&param, msg->link_close.reason);
    case ESP_BLE_MESH_MODEL_OP_RPR_PDU_SEND:
        BT_WARN("Remote Provisioning PDU Send will be sent internally");
        return 0;
    default:
        BT_ERR("Invalid Remote Provisioning msg opcode 0x%04x", param.opcode);
        return -EINVAL;
    }
}

static int btc_ble_mesh_rpr_client_act(esp_ble_mesh_rpr_client_act_type_t type,
                                       esp_ble_mesh_rpr_client_act_param_t *param,
                                       esp_ble_mesh_rpr_client_cb_param_t *cb)
{
    if (param == NULL || cb == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    switch (type) {
    case ESP_BLE_MESH_RPR_CLIENT_ACT_START_RPR:
        cb->act.sub_evt = ESP_BLE_MESH_START_RPR_COMP_SUB_EVT;
        cb->act.start_rpr_comp.model = param->start_rpr.model;
        cb->act.start_rpr_comp.rpr_srv_addr = param->start_rpr.rpr_srv_addr;
        cb->act.start_rpr_comp.err_code =
            bt_mesh_rpr_start_prov((struct bt_mesh_model *)param->start_rpr.model,
                                   param->start_rpr.rpr_srv_addr);
        break;
    default:
        BT_ERR("Invalid Remote Provisioning action 0x%02x", type);
        return -EINVAL;
    }

    return 0;
}

void btc_ble_mesh_rpr_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_rpr_client_cb_param_t cb = {0};
    btc_ble_mesh_rpr_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_rpr_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_RPR_CLIENT_SEND:
        cb.send.params = arg->rpr_send.params;
        cb.send.err_code = btc_ble_mesh_rpr_client_send(arg->rpr_send.params,
                                                        arg->rpr_send.msg);
        btc_ble_mesh_rpr_client_cb(&cb, ESP_BLE_MESH_RPR_CLIENT_SEND_COMP_EVT);
        break;
    case BTC_BLE_MESH_ACT_RPR_CLIENT_ACT:
        if (btc_ble_mesh_rpr_client_act(arg->rpr_act.type,
                                        arg->rpr_act.param, &cb) == 0) {
            btc_ble_mesh_rpr_client_cb(&cb, ESP_BLE_MESH_RPR_CLIENT_ACT_COMP_EVT);
        }
        break;
    default:
        break;
    }

    btc_ble_mesh_rpr_client_arg_deep_free(msg);
}

void btc_ble_mesh_rpr_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_rpr_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_rpr_client_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_RPR_CLIENT_EVT_MAX) {
        btc_ble_mesh_rpr_client_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_rpr_client_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_RPR_CLI */

#if CONFIG_BLE_MESH_RPR_SRV

/* Remote Provisioning Server model related functions */
extern int bt_mesh_rpr_srv_scan_set_dev_uuid_match(uint8_t offset, uint8_t length,const uint8_t *match);

void btc_ble_mesh_rpr_server_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_rpr_server_args_t *dst = p_dest;
    btc_ble_mesh_rpr_server_args_t *src = p_src;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch(msg->act) {
    case BTC_BLE_MESH_ACT_RPR_SRV_SET_UUID_MATCH:
        dst->set_uuid_match.match_val = bt_mesh_calloc(src->set_uuid_match.match_len);
        if (dst->set_uuid_match.match_val) {
            memcpy(dst->set_uuid_match.match_val, src->set_uuid_match.match_val, src->set_uuid_match.match_len);
            dst->set_uuid_match.match_len = src->set_uuid_match.match_len;
            dst->set_uuid_match.offset = src->set_uuid_match.offset;
        } else {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
        }
        break;
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }

    return;
}

void btc_ble_mesh_rpr_server_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_rpr_server_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_rpr_server_args_t *)msg->arg;

    switch(msg->act) {
    case BTC_BLE_MESH_ACT_RPR_SRV_SET_UUID_MATCH:
        if (arg->set_uuid_match.match_val) {
            bt_mesh_free(arg->set_uuid_match.match_val);
        }
        break;
    default:
        break;
    }

    return;
}

static inline void btc_ble_mesh_rpr_server_cb_to_app(esp_ble_mesh_rpr_server_cb_event_t event,
                                                     esp_ble_mesh_rpr_server_cb_param_t *param)
{
    esp_ble_mesh_rpr_server_cb_t btc_ble_mesh_cb =
        btc_profile_cb_get(BTC_PID_RPR_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_rpr_server_cb(esp_ble_mesh_rpr_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_RPR_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_RPR_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_rpr_server_cb_param_t), NULL, NULL);
}

void bt_mesh_rpr_server_cb_evt_to_btc(uint8_t event, const void *val, size_t len)
{
    esp_ble_mesh_rpr_server_cb_param_t cb_params = {0};
    uint8_t act = 0;

    if (len > sizeof(cb_params)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (event) {
    case BTC_BLE_MESH_EVT_RPR_SERVER_SCAN_START:
        act = ESP_BLE_MESH_RPR_SERVER_SCAN_START_EVT;
        break;
    case BTC_BLE_MESH_EVT_RPR_SERVER_SCAN_STOP:
        act = ESP_BLE_MESH_RPR_SERVER_SCAN_STOP_EVT;
        break;
    case BTC_BLE_MESH_EVT_RPR_SERVER_EXT_SCAN_START:
        act = ESP_BLE_MESH_RPR_SERVER_EXT_SCAN_START_EVT;
        break;
    case BTC_BLE_MESH_EVT_RPR_SERVER_EXT_SCAN_STOP:
        act = ESP_BLE_MESH_RPR_SERVER_EXT_SCAN_STOP_EVT;
        break;
    case BTC_BLE_MESH_EVT_RPR_SERVER_LINK_OPEN:
        act = ESP_BLE_MESH_RPR_SERVER_LINK_OPEN_EVT;
        break;
    case BTC_BLE_MESH_EVT_RPR_SERVER_LINK_CLOSE:
        act = ESP_BLE_MESH_RPR_SERVER_LINK_CLOSE_EVT;
        break;
    case BTC_BLE_MESH_EVT_RPR_SERVER_PROV_COMP:
        act = ESP_BLE_MESH_RPR_SERVER_PROV_COMP_EVT;
        break;
    default:
        BT_ERR("Unknown Remote Provisioning server event type %d", event);
        return;
    }

    if (val && len) {
        memcpy(&cb_params, val, len);
    }

    btc_ble_mesh_rpr_server_cb(&cb_params, act);
}

void btc_ble_mesh_rpr_server_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_rpr_server_cb_param_t cb = {0};
    btc_ble_mesh_rpr_server_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_rpr_server_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_RPR_SRV_SET_UUID_MATCH:
        cb.set_uuid_match_comp.err_code = bt_mesh_rpr_srv_scan_set_dev_uuid_match(arg->set_uuid_match.offset,
                                                                                  arg->set_uuid_match.match_len,
                                                                                  arg->set_uuid_match.match_val);

        btc_ble_mesh_rpr_server_cb(&cb, ESP_BLE_MESH_RPR_SERVER_SET_UUID_MATCH_COMP_EVT);
        break;
    default:
        break;
    }

    btc_ble_mesh_rpr_server_arg_deep_free(msg);
}

void btc_ble_mesh_rpr_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_rpr_server_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_rpr_server_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_RPR_SERVER_EVT_MAX) {
        btc_ble_mesh_rpr_server_cb_to_app(msg->act, arg);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }
}

#endif /* CONFIG_BLE_MESH_RPR_SRV */
