/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "btc_ble_mesh_config_model.h"

#include "mesh.h"
#include "mesh/config.h"
#include "foundation.h"
#include "mesh/common.h"
#include "mesh/cfg_cli.h"

#if CONFIG_BLE_MESH_CFG_CLI

static const bt_mesh_client_op_pair_t cfg_op_pair[] = {
    { OP_BEACON_GET,           OP_BEACON_STATUS        },
    { OP_BEACON_SET,           OP_BEACON_STATUS        },
    { OP_COMP_DATA_GET,        OP_COMP_DATA_STATUS     },
    { OP_DEFAULT_TTL_GET,      OP_DEFAULT_TTL_STATUS   },
    { OP_DEFAULT_TTL_SET,      OP_DEFAULT_TTL_STATUS   },
    { OP_GATT_PROXY_GET,       OP_GATT_PROXY_STATUS    },
    { OP_GATT_PROXY_SET,       OP_GATT_PROXY_STATUS    },
    { OP_RELAY_GET,            OP_RELAY_STATUS         },
    { OP_RELAY_SET,            OP_RELAY_STATUS         },
    { OP_MOD_PUB_GET,          OP_MOD_PUB_STATUS       },
    { OP_MOD_PUB_SET,          OP_MOD_PUB_STATUS       },
    { OP_MOD_PUB_VA_SET,       OP_MOD_PUB_STATUS       },
    { OP_MOD_SUB_ADD,          OP_MOD_SUB_STATUS       },
    { OP_MOD_SUB_VA_ADD,       OP_MOD_SUB_STATUS       },
    { OP_MOD_SUB_DEL,          OP_MOD_SUB_STATUS       },
    { OP_MOD_SUB_VA_DEL,       OP_MOD_SUB_STATUS       },
    { OP_MOD_SUB_OVERWRITE,    OP_MOD_SUB_STATUS       },
    { OP_MOD_SUB_VA_OVERWRITE, OP_MOD_SUB_STATUS       },
    { OP_MOD_SUB_DEL_ALL,      OP_MOD_SUB_STATUS       },
    { OP_MOD_SUB_GET,          OP_MOD_SUB_LIST         },
    { OP_MOD_SUB_GET_VND,      OP_MOD_SUB_LIST_VND     },
    { OP_NET_KEY_ADD,          OP_NET_KEY_STATUS       },
    { OP_NET_KEY_UPDATE,       OP_NET_KEY_STATUS       },
    { OP_NET_KEY_DEL,          OP_NET_KEY_STATUS       },
    { OP_NET_KEY_GET,          OP_NET_KEY_LIST         },
    { OP_APP_KEY_ADD,          OP_APP_KEY_STATUS       },
    { OP_APP_KEY_UPDATE,       OP_APP_KEY_STATUS       },
    { OP_APP_KEY_DEL,          OP_APP_KEY_STATUS       },
    { OP_APP_KEY_GET,          OP_APP_KEY_LIST         },
    { OP_NODE_IDENTITY_GET,    OP_NODE_IDENTITY_STATUS },
    { OP_NODE_IDENTITY_SET,    OP_NODE_IDENTITY_STATUS },
    { OP_MOD_APP_BIND,         OP_MOD_APP_STATUS       },
    { OP_MOD_APP_UNBIND,       OP_MOD_APP_STATUS       },
    { OP_SIG_MOD_APP_GET,      OP_SIG_MOD_APP_LIST     },
    { OP_VND_MOD_APP_GET,      OP_VND_MOD_APP_LIST     },
    { OP_NODE_RESET,           OP_NODE_RESET_STATUS    },
    { OP_FRIEND_GET,           OP_FRIEND_STATUS        },
    { OP_FRIEND_SET,           OP_FRIEND_STATUS        },
    { OP_KRP_GET,              OP_KRP_STATUS           },
    { OP_KRP_SET,              OP_KRP_STATUS           },
    { OP_HEARTBEAT_PUB_GET,    OP_HEARTBEAT_PUB_STATUS },
    { OP_HEARTBEAT_PUB_SET,    OP_HEARTBEAT_PUB_STATUS },
    { OP_HEARTBEAT_SUB_GET,    OP_HEARTBEAT_SUB_STATUS },
    { OP_HEARTBEAT_SUB_SET,    OP_HEARTBEAT_SUB_STATUS },
    { OP_LPN_TIMEOUT_GET,      OP_LPN_TIMEOUT_STATUS   },
    { OP_NET_TRANSMIT_GET,     OP_NET_TRANSMIT_STATUS  },
    { OP_NET_TRANSMIT_SET,     OP_NET_TRANSMIT_STATUS  },
};

static bt_mesh_mutex_t cfg_client_lock;

static void timeout_handler(struct k_work *work)
{
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    uint32_t opcode = 0U;

    BT_WARN("Receive configuration status message timeout");

    bt_mesh_mutex_lock(&cfg_client_lock);

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            memcpy(&ctx, &node->ctx, sizeof(ctx));
            opcode = node->opcode;
            model = node->model;
            bt_mesh_client_free_node(node);
            bt_mesh_config_client_cb_evt_to_btc(
                opcode, BTC_BLE_MESH_EVT_CONFIG_CLIENT_TIMEOUT, model, &ctx, NULL, 0);
        }
    }

    bt_mesh_mutex_unlock(&cfg_client_lock);
}

static void cfg_client_recv_status(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   void *status, size_t len)
{
    bt_mesh_client_node_t *node = NULL;
    struct net_buf_simple buf = {0};
    uint8_t evt_type = 0xFF;

    BT_DBG("CfgClientRecvStatus");

    if (!model || !ctx) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    /* If it is a publish message, sent to the user directly. */
    buf.data = (uint8_t *)status;
    buf.len  = (uint16_t)len;

    bt_mesh_mutex_lock(&cfg_client_lock);

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, &buf, true);
    if (!node) {
        BT_DBG("Unexpected Config Status 0x%04x", ctx->recv_op);
    } else {
        BT_DBG("OpCode 0x%08lx RecvOp 0x%08lx", node->opcode, ctx->recv_op);

        switch (node->opcode) {
        case OP_BEACON_GET:
        case OP_COMP_DATA_GET:
        case OP_DEFAULT_TTL_GET:
        case OP_GATT_PROXY_GET:
        case OP_RELAY_GET:
        case OP_MOD_PUB_GET:
        case OP_MOD_SUB_GET:
        case OP_MOD_SUB_GET_VND:
        case OP_NET_KEY_GET:
        case OP_APP_KEY_GET:
        case OP_NODE_IDENTITY_GET:
        case OP_SIG_MOD_APP_GET:
        case OP_VND_MOD_APP_GET:
        case OP_FRIEND_GET:
        case OP_KRP_GET:
        case OP_HEARTBEAT_PUB_GET:
        case OP_HEARTBEAT_SUB_GET:
        case OP_LPN_TIMEOUT_GET:
        case OP_NET_TRANSMIT_GET:
            evt_type = BTC_BLE_MESH_EVT_CONFIG_CLIENT_GET_STATE;
            break;
        case OP_BEACON_SET:
        case OP_DEFAULT_TTL_SET:
        case OP_GATT_PROXY_SET:
        case OP_RELAY_SET:
        case OP_MOD_PUB_SET:
        case OP_MOD_PUB_VA_SET:
        case OP_MOD_SUB_ADD:
        case OP_MOD_SUB_VA_ADD:
        case OP_MOD_SUB_DEL:
        case OP_MOD_SUB_VA_DEL:
        case OP_MOD_SUB_OVERWRITE:
        case OP_MOD_SUB_VA_OVERWRITE:
        case OP_MOD_SUB_DEL_ALL:
        case OP_NET_KEY_ADD:
        case OP_NET_KEY_UPDATE:
        case OP_NET_KEY_DEL:
        case OP_APP_KEY_ADD:
        case OP_APP_KEY_UPDATE:
        case OP_APP_KEY_DEL:
        case OP_NODE_IDENTITY_SET:
        case OP_MOD_APP_BIND:
        case OP_MOD_APP_UNBIND:
        case OP_NODE_RESET:
        case OP_FRIEND_SET:
        case OP_KRP_SET:
        case OP_HEARTBEAT_PUB_SET:
        case OP_HEARTBEAT_SUB_SET:
        case OP_NET_TRANSMIT_SET:
            evt_type = BTC_BLE_MESH_EVT_CONFIG_CLIENT_SET_STATE;
            break;
        default:
            break;
        }

        if (!k_delayed_work_free(&node->timer)) {
            uint32_t opcode = node->opcode;
            bt_mesh_client_free_node(node);
            bt_mesh_config_client_cb_evt_to_btc(
                opcode, evt_type, model, ctx, (const uint8_t *)status, len);
        }
    }

    bt_mesh_mutex_unlock(&cfg_client_lock);

    switch (ctx->recv_op) {
    case OP_COMP_DATA_STATUS: {
        struct bt_mesh_cfg_comp_data_status *val = status;
        bt_mesh_free_buf(val->comp_data);
        break;
    }
    case OP_MOD_SUB_LIST:
    case OP_MOD_SUB_LIST_VND: {
        struct bt_mesh_cfg_mod_sub_list *val = status;
        bt_mesh_free_buf(val->addr);
        break;
    }
    case OP_NET_KEY_LIST: {
        struct bt_mesh_cfg_net_key_list *val = status;
        bt_mesh_free_buf(val->net_idx);
        break;
    }
    case OP_APP_KEY_LIST: {
        struct bt_mesh_cfg_app_key_list *val = status;
        bt_mesh_free_buf(val->app_idx);
        break;
    }
    case OP_SIG_MOD_APP_LIST:
    case OP_VND_MOD_APP_LIST: {
        struct bt_mesh_cfg_mod_app_list *val = status;
        bt_mesh_free_buf(val->app_idx);
        break;
    }
    default:
        break;
    }
}

static void comp_data_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_comp_data_status status = {0};

    BT_DBG("CompDataStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.page = net_buf_simple_pull_u8(buf);
    status.comp_data = bt_mesh_alloc_buf(buf->len);
    if (!status.comp_data) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }

    net_buf_simple_add_mem(status.comp_data, buf->data, buf->len);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_comp_data_status));
}

static void state_status_u8(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    uint8_t status = 0U;

    BT_DBG("StateStatusU8");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status = net_buf_simple_pull_u8(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(uint8_t));
}

static void beacon_status(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("BeaconStatus");

    state_status_u8(model, ctx, buf);
}

static void ttl_status(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf)
{
    BT_DBG("TTLStatus");

    state_status_u8(model, ctx, buf);
}

static void friend_status(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    BT_DBG("FrndStatus");

    state_status_u8(model, ctx, buf);
}

static void gatt_proxy_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    BT_DBG("GattProxyStatus");

    state_status_u8(model, ctx, buf);
}

static void relay_status(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_relay_status status = {0};

    BT_DBG("RelayStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.relay      = net_buf_simple_pull_u8(buf);
    status.retransmit = net_buf_simple_pull_u8(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_relay_status));
}

static void net_key_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_netkey_status status = {0};

    BT_DBG("NetKeyStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status = net_buf_simple_pull_u8(buf);
    status.net_idx = net_buf_simple_pull_le16(buf) & 0xfff;

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_netkey_status));
}

static void app_key_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_appkey_status status = {0};

    BT_DBG("AppKeyStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status = net_buf_simple_pull_u8(buf);
    key_idx_unpack(buf, &status.net_idx, &status.app_idx);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_appkey_status));
}

static void mod_app_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_mod_app_status status = {0};

    BT_DBG("ModAppStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status    = net_buf_simple_pull_u8(buf);
    status.elem_addr = net_buf_simple_pull_le16(buf);
    status.app_idx   = net_buf_simple_pull_le16(buf);
    if (buf->len >= 4) {
        status.cid = net_buf_simple_pull_le16(buf);
    } else {
        status.cid = BLE_MESH_CID_NVAL;
    }
    status.mod_id = net_buf_simple_pull_le16(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_mod_app_status));
}

static void mod_pub_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_mod_pub_status status = {0};

    BT_DBG("ModPubStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status    = net_buf_simple_pull_u8(buf);
    status.elem_addr = net_buf_simple_pull_le16(buf);
    status.addr      = net_buf_simple_pull_le16(buf);
    status.app_idx   = net_buf_simple_pull_le16(buf);
    status.cred_flag = (status.app_idx & BIT(12));
    status.app_idx  &= BIT_MASK(12);
    status.ttl       = net_buf_simple_pull_u8(buf);
    status.period    = net_buf_simple_pull_u8(buf);
    status.transmit  = net_buf_simple_pull_u8(buf);
    if (buf->len >= 4) {
        status.cid = net_buf_simple_pull_le16(buf);
    } else {
        status.cid = BLE_MESH_CID_NVAL;
    }
    status.mod_id = net_buf_simple_pull_le16(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_mod_pub_status));
}

static void mod_sub_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_mod_sub_status status = {0};

    BT_DBG("ModSubStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status    = net_buf_simple_pull_u8(buf);
    status.elem_addr = net_buf_simple_pull_le16(buf);
    status.sub_addr  = net_buf_simple_pull_le16(buf);
    if (buf->len >= 4) {
        status.cid = net_buf_simple_pull_le16(buf);
    } else {
        status.cid = BLE_MESH_CID_NVAL;
    }
    status.mod_id = net_buf_simple_pull_le16(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_mod_sub_status));
}

static void hb_sub_status(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_hb_sub_status status = {0};

    BT_DBG("HbSubStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status = net_buf_simple_pull_u8(buf);
    status.src    = net_buf_simple_pull_le16(buf);
    status.dst    = net_buf_simple_pull_le16(buf);
    status.period = net_buf_simple_pull_u8(buf);
    status.count  = net_buf_simple_pull_u8(buf);
    status.min    = net_buf_simple_pull_u8(buf);
    status.max    = net_buf_simple_pull_u8(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_hb_sub_status));
}

static void hb_pub_status(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_hb_pub_status status = {0};

    BT_DBG("HbPubStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status  = net_buf_simple_pull_u8(buf);
    status.dst     = net_buf_simple_pull_le16(buf);
    status.count   = net_buf_simple_pull_u8(buf);
    status.period  = net_buf_simple_pull_u8(buf);
    status.ttl     = net_buf_simple_pull_u8(buf);
    status.feat    = net_buf_simple_pull_le16(buf);
    status.net_idx = net_buf_simple_pull_le16(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_hb_sub_status));
}

static void node_reset_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    BT_DBG("NodeResetStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    cfg_client_recv_status(model, ctx, NULL, 0);
}

static void mod_sub_list(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_mod_sub_list list = {0};

    BT_DBG("ModSubList");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    list.status = net_buf_simple_pull_u8(buf);
    list.elem_addr = net_buf_simple_pull_le16(buf);
    if (ctx->recv_op == OP_MOD_SUB_LIST_VND) {
        list.cid = net_buf_simple_pull_le16(buf);
    } else {
        list.cid = BLE_MESH_CID_NVAL;
    }
    list.mod_id = net_buf_simple_pull_le16(buf);

    list.addr = bt_mesh_alloc_buf(buf->len);
    if (!list.addr) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }
    net_buf_simple_add_mem(list.addr, buf->data, buf->len);

    cfg_client_recv_status(model, ctx, &list, sizeof(struct bt_mesh_cfg_mod_sub_list));
}

static void net_key_list(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_net_key_list list = {0};

    BT_DBG("NetKeyList");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    list.net_idx = bt_mesh_alloc_buf(buf->len);
    if (!list.net_idx) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }
    net_buf_simple_add_mem(list.net_idx, buf->data, buf->len);

    cfg_client_recv_status(model, ctx, &list, sizeof(struct bt_mesh_cfg_net_key_list));
}

static void app_key_list(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_app_key_list list = {0};

    BT_DBG("AppKeyList");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    list.status = net_buf_simple_pull_u8(buf);
    list.net_idx = net_buf_simple_pull_le16(buf);
    list.app_idx = bt_mesh_alloc_buf(buf->len);
    if (!list.app_idx) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }
    net_buf_simple_add_mem(list.app_idx, buf->data, buf->len);

    cfg_client_recv_status(model, ctx, &list, sizeof(struct bt_mesh_cfg_app_key_list));
}

static void node_id_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_node_id_status status = {0};

    BT_DBG("NodeIDStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status = net_buf_simple_pull_u8(buf);
    status.net_idx = net_buf_simple_pull_le16(buf);
    status.identity = net_buf_simple_pull_u8(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_node_id_status));
}

static void mod_app_list(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_mod_app_list list = {0};

    BT_DBG("ModAppList");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    list.status = net_buf_simple_pull_u8(buf);
    list.elem_addr = net_buf_simple_pull_le16(buf);
    if (ctx->recv_op == OP_VND_MOD_APP_LIST) {
        list.cid = net_buf_simple_pull_le16(buf);
    } else {
        list.cid = BLE_MESH_CID_NVAL;
    }
    list.mod_id = net_buf_simple_pull_le16(buf);

    list.app_idx = bt_mesh_alloc_buf(buf->len);
    if (!list.app_idx) {
        BT_ERR("%s, Out of memory", __func__);
        return;
    }
    net_buf_simple_add_mem(list.app_idx, buf->data, buf->len);

    cfg_client_recv_status(model, ctx, &list, sizeof(struct bt_mesh_cfg_mod_app_list));
}

static void kr_phase_status(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_key_refresh_status status = {0};

    BT_DBG("KrPhaseStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.status = net_buf_simple_pull_u8(buf);
    status.net_idx = net_buf_simple_pull_le16(buf);
    status.phase = net_buf_simple_pull_u8(buf);

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_key_refresh_status));
}

static void lpn_pollto_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_lpn_pollto_status status = {0};

    BT_DBG("LPNPollToStatus");
    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Src 0x%04x",
           ctx->net_idx, ctx->app_idx, ctx->addr);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    status.lpn_addr = net_buf_simple_pull_le16(buf);
    status.timeout  = net_buf_simple_pull_u8(buf);
    status.timeout |= net_buf_simple_pull_u8(buf) << 8;
    status.timeout |= net_buf_simple_pull_u8(buf) << 16;

    cfg_client_recv_status(model, ctx, &status, sizeof(struct bt_mesh_cfg_lpn_pollto_status));
}

static void net_trans_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    BT_DBG("NetTransStatus");

    state_status_u8(model, ctx, buf);
}

const struct bt_mesh_model_op bt_mesh_cfg_cli_op[] = {
    { OP_COMP_DATA_STATUS,       15,  comp_data_status  },
    { OP_BEACON_STATUS,          1,   beacon_status     },
    { OP_DEFAULT_TTL_STATUS,     1,   ttl_status        },
    { OP_FRIEND_STATUS,          1,   friend_status     },
    { OP_GATT_PROXY_STATUS,      1,   gatt_proxy_status },
    { OP_RELAY_STATUS,           2,   relay_status      },
    { OP_NET_KEY_STATUS,         3,   net_key_status    },
    { OP_APP_KEY_STATUS,         4,   app_key_status    },
    { OP_MOD_APP_STATUS,         7,   mod_app_status    },
    { OP_MOD_PUB_STATUS,         12,  mod_pub_status    },
    { OP_MOD_SUB_STATUS,         7,   mod_sub_status    },
    { OP_HEARTBEAT_SUB_STATUS,   9,   hb_sub_status     },
    { OP_HEARTBEAT_PUB_STATUS,   10,  hb_pub_status     },
    { OP_NODE_RESET_STATUS,      0,   node_reset_status },
    { OP_MOD_SUB_LIST,           5,   mod_sub_list      },
    { OP_MOD_SUB_LIST_VND,       7,   mod_sub_list      },
    { OP_NET_KEY_LIST,           2,   net_key_list      },
    { OP_APP_KEY_LIST,           3,   app_key_list      },
    { OP_NODE_IDENTITY_STATUS,   4,   node_id_status    },
    { OP_SIG_MOD_APP_LIST,       5,   mod_app_list      },
    { OP_VND_MOD_APP_LIST,       7,   mod_app_list      },
    { OP_KRP_STATUS,             4,   kr_phase_status   },
    { OP_LPN_TIMEOUT_STATUS,     5,   lpn_pollto_status },
    { OP_NET_TRANSMIT_STATUS,    1,   net_trans_status  },
    BLE_MESH_MODEL_OP_END,
};

static int send_msg_with_none(bt_mesh_client_common_param_t *param, uint32_t op)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, op, 0);

    BT_DBG("SendMsgWithNone");
    BT_DBG("Dst 0x%04x Op 0x%08lx", param->ctx.addr, op);

    bt_mesh_model_msg_init(&msg, op);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

static int send_msg_with_u8(bt_mesh_client_common_param_t *param, uint32_t op, uint8_t val)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, op, 1);

    BT_DBG("SendMsgWithU8");
    BT_DBG("Dst 0x%04x Op 0x%08lx Val 0x%02x", param->ctx.addr, op, val);

    bt_mesh_model_msg_init(&msg, op);
    net_buf_simple_add_u8(&msg, val);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

static int send_msg_with_le16(bt_mesh_client_common_param_t *param, uint32_t op, uint16_t val)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, op, 2);

    BT_DBG("SendMsgWithLE16");
    BT_DBG("Dst 0x%04x Op 0x%08lx Val 0x%04x", param->ctx.addr, op, val);

    bt_mesh_model_msg_init(&msg, op);
    net_buf_simple_add_le16(&msg, val);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_comp_data_get(bt_mesh_client_common_param_t *param, uint8_t page)
{
    BT_DBG("CompDataGet, Page %u", page);

    return send_msg_with_u8(param, OP_COMP_DATA_GET, page);
}

int bt_mesh_cfg_beacon_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("BeaconGet");

    return send_msg_with_none(param, OP_BEACON_GET);
}

int bt_mesh_cfg_beacon_set(bt_mesh_client_common_param_t *param, uint8_t val)
{
    BT_DBG("BeaconSet, Val 0x%02x", val);

    if (val > 0x01) {
        BT_ERR("Invalid beacon state 0x%02x", val);
        return -EINVAL;
    }

    return send_msg_with_u8(param, OP_BEACON_SET, val);
}

int bt_mesh_cfg_ttl_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("TTLGet");

    return send_msg_with_none(param, OP_DEFAULT_TTL_GET);
}

int bt_mesh_cfg_ttl_set(bt_mesh_client_common_param_t *param, uint8_t val)
{
    BT_DBG("TTLSet, Val 0x%02x", val);

    return send_msg_with_u8(param, OP_DEFAULT_TTL_SET, val);
}

int bt_mesh_cfg_friend_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("FrndGet");

    return send_msg_with_none(param, OP_FRIEND_GET);
}

int bt_mesh_cfg_friend_set(bt_mesh_client_common_param_t *param, uint8_t val)
{
    BT_DBG("FrndSet, Val 0x%02x", val);

    if (val > 0x01) {
        BT_ERR("InvalidFriendState 0x%02x", val);
        return -EINVAL;
    }

    return send_msg_with_u8(param, OP_FRIEND_SET, val);
}

int bt_mesh_cfg_gatt_proxy_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("GattProxyGet");

    return send_msg_with_none(param, OP_GATT_PROXY_GET);
}

int bt_mesh_cfg_gatt_proxy_set(bt_mesh_client_common_param_t *param, uint8_t val)
{
    BT_DBG("GattProxySet, Val 0x%02x", val);

    return send_msg_with_u8(param, OP_GATT_PROXY_SET, val);
}

int bt_mesh_cfg_relay_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("RelayGet");

    return send_msg_with_none(param, OP_RELAY_GET);
}

int bt_mesh_cfg_relay_set(bt_mesh_client_common_param_t *param,
                          uint8_t relay, uint8_t retransmit)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_RELAY_SET, 2);

    BT_DBG("RelaySet, Relay 0x%02x Retransmit 0x%02x", relay, retransmit);

    bt_mesh_model_msg_init(&msg, OP_RELAY_SET);
    net_buf_simple_add_u8(&msg, relay);
    net_buf_simple_add_u8(&msg, retransmit);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_net_key_add(bt_mesh_client_common_param_t *param,
                            uint16_t net_idx, const uint8_t net_key[16])
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_NET_KEY_ADD, 18);

    BT_DBG("NetKeyAdd");

    if (!net_key) {
        BT_ERR("Invalid NetKey");
        return -EINVAL;
    }

    BT_DBG("NetIdx 0x%04x NetKey %s", net_idx, bt_hex(net_key, 16));

    bt_mesh_model_msg_init(&msg, OP_NET_KEY_ADD);
    net_buf_simple_add_le16(&msg, net_idx);
    net_buf_simple_add_mem(&msg, net_key, 16);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_app_key_add(bt_mesh_client_common_param_t *param,
                            uint16_t net_idx, uint16_t app_idx,
                            const uint8_t app_key[16])
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_APP_KEY_ADD, 19);

    BT_DBG("AppKeyAdd");

    if (!app_key) {
        BT_ERR("Invalid AppKey");
        return -EINVAL;
    }

    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x AppKey %s",
           net_idx, app_idx, bt_hex(app_key, 16));

    bt_mesh_model_msg_init(&msg, OP_APP_KEY_ADD);
    key_idx_pack(&msg, net_idx, app_idx);
    net_buf_simple_add_mem(&msg, app_key, 16);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_app_bind(bt_mesh_client_common_param_t *param,
                             uint16_t elem_addr, uint16_t app_idx,
                             uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_MOD_APP_BIND, 8);

    BT_DBG("ModAppBind");
    BT_DBG("ElemAddr 0x%04x AppIdx 0x%04x ModID 0x%04x CID 0x%04x",
           elem_addr, app_idx, mod_id, cid);

    bt_mesh_model_msg_init(&msg, OP_MOD_APP_BIND);
    net_buf_simple_add_le16(&msg, elem_addr);
    net_buf_simple_add_le16(&msg, app_idx);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

static int mod_sub(bt_mesh_client_common_param_t *param, uint32_t op,
                   uint16_t elem_addr, uint16_t sub_addr,
                   uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, op, 8);

    BT_DBG("ModSub");
    BT_DBG("ElemAddr 0x%04x SubAddr 0x%04x ModID 0x%04x CID 0x%04x",
           elem_addr, sub_addr, mod_id, cid);

    bt_mesh_model_msg_init(&msg, op);
    net_buf_simple_add_le16(&msg, elem_addr);
    net_buf_simple_add_le16(&msg, sub_addr);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_sub_add(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t sub_addr,
                            uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModSubAdd");

    return mod_sub(param, OP_MOD_SUB_ADD, elem_addr, sub_addr, mod_id, cid);
}

int bt_mesh_cfg_mod_sub_del(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t sub_addr,
                            uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModSubDel");

    return mod_sub(param, OP_MOD_SUB_DEL, elem_addr, sub_addr, mod_id, cid);
}

int bt_mesh_cfg_mod_sub_overwrite(bt_mesh_client_common_param_t *param,
                                  uint16_t elem_addr, uint16_t sub_addr,
                                  uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModSubOverwrite");

    return mod_sub(param, OP_MOD_SUB_OVERWRITE, elem_addr, sub_addr, mod_id, cid);
}

static int mod_sub_va(bt_mesh_client_common_param_t *param, uint32_t op,
                      uint16_t elem_addr, const uint8_t label[16],
                      uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, op, 22);

    BT_DBG("ModSubVa");

    if (!label) {
        BT_ERR("Invalid label uuid");
        return -EINVAL;
    }

    BT_DBG("ElemAddr 0x%04x ModID 0x%04x CID 0x%04x", elem_addr, mod_id, cid);
    BT_DBG("Label %s", bt_hex(label, 16));

    bt_mesh_model_msg_init(&msg, op);
    net_buf_simple_add_le16(&msg, elem_addr);
    net_buf_simple_add_mem(&msg, label, 16);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_sub_va_add(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, const uint8_t label[16],
                               uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModSubVaAdd");

    return mod_sub_va(param, OP_MOD_SUB_VA_ADD, elem_addr, label, mod_id, cid);
}

int bt_mesh_cfg_mod_sub_va_del(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, const uint8_t label[16],
                               uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModSubVaDel");

    return mod_sub_va(param, OP_MOD_SUB_VA_DEL, elem_addr, label, mod_id, cid);
}

int bt_mesh_cfg_mod_sub_va_overwrite(bt_mesh_client_common_param_t *param,
                                     uint16_t elem_addr, const uint8_t label[16],
                                     uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModSubVaOverwrite");

    return mod_sub_va(param, OP_MOD_SUB_VA_OVERWRITE, elem_addr, label, mod_id, cid);
}

int bt_mesh_cfg_mod_pub_get(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_MOD_PUB_GET, 6);

    BT_DBG("ModPubGet");
    BT_DBG("ElemAddr 0x%04x ModID 0x%04x CID 0x%04x", elem_addr, mod_id, cid);

    bt_mesh_model_msg_init(&msg, OP_MOD_PUB_GET);
    net_buf_simple_add_le16(&msg, elem_addr);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_pub_set(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id, uint16_t cid,
                            struct bt_mesh_cfg_mod_pub *pub)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_MOD_PUB_SET, 13);

    BT_DBG("ModPubSet");
    BT_DBG("ElemAddr 0x%04x ModID 0x%04x CID 0x%04x", elem_addr, mod_id, cid);

    if (!pub) {
        BT_ERR("Invalid model pub set");
        return -EINVAL;
    }

    BT_DBG("Addr 0x%04x AppIdx 0x%04x TTL %u Period 0x%02x Transmit 0x%02x",
           pub->addr, pub->app_idx, pub->ttl, pub->period, pub->transmit);

    bt_mesh_model_msg_init(&msg, OP_MOD_PUB_SET);
    net_buf_simple_add_le16(&msg, elem_addr);
    net_buf_simple_add_le16(&msg, pub->addr);
    net_buf_simple_add_le16(&msg, (pub->app_idx | (pub->cred_flag << 12)));
    net_buf_simple_add_u8(&msg, pub->ttl);
    net_buf_simple_add_u8(&msg, pub->period);
    net_buf_simple_add_u8(&msg, pub->transmit);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_hb_sub_set(bt_mesh_client_common_param_t *param,
                           struct bt_mesh_cfg_hb_sub *sub)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEARTBEAT_SUB_SET, 5);

    BT_DBG("HbSubSet");

    if (!sub) {
        BT_ERR("Invalid heartbeat sub set");
        return -EINVAL;
    }

    BT_DBG("Src 0x%04x Dst 0x%04x Period 0x%02x", sub->src, sub->dst, sub->period);

    bt_mesh_model_msg_init(&msg, OP_HEARTBEAT_SUB_SET);
    net_buf_simple_add_le16(&msg, sub->src);
    net_buf_simple_add_le16(&msg, sub->dst);
    net_buf_simple_add_u8(&msg, sub->period);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_hb_sub_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("HbSubGet");

    return send_msg_with_none(param, OP_HEARTBEAT_SUB_GET);
}

int bt_mesh_cfg_hb_pub_set(bt_mesh_client_common_param_t *param,
                           struct bt_mesh_cfg_hb_pub *pub)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_HEARTBEAT_PUB_SET, 9);

    BT_DBG("HbPubSet");

    if (!pub) {
        BT_ERR("Invalid heartbeat pub set");
        return -EINVAL;
    }

    BT_DBG("Dst 0x%04x Count 0x%02x Period 0x%02x TTL %u Feat 0x%04x NetIdx 0x%04x",
           pub->dst, pub->count, pub->period, pub->ttl, pub->feat, pub->net_idx);

    bt_mesh_model_msg_init(&msg, OP_HEARTBEAT_PUB_SET);
    net_buf_simple_add_le16(&msg, pub->dst);
    net_buf_simple_add_u8(&msg, pub->count);
    net_buf_simple_add_u8(&msg, pub->period);
    net_buf_simple_add_u8(&msg, pub->ttl);
    net_buf_simple_add_le16(&msg, pub->feat);
    net_buf_simple_add_le16(&msg, pub->net_idx);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_hb_pub_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("HbPubGet");

    return send_msg_with_none(param, OP_HEARTBEAT_PUB_GET);
}

int bt_mesh_cfg_node_reset(bt_mesh_client_common_param_t *param)
{
    BT_DBG("NodeReset");

    return send_msg_with_none(param, OP_NODE_RESET);
}

int bt_mesh_cfg_mod_pub_va_set(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, uint16_t mod_id,
                               uint16_t cid, const uint8_t label[16],
                               struct bt_mesh_cfg_mod_pub *pub)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_MOD_PUB_VA_SET, 27);

    BT_DBG("ModPubVaSet");
    BT_DBG("ElemAddr 0x%04x ModID 0x%04x CID 0x%04x", elem_addr, mod_id, cid);

    if (!label || !pub) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("Label %s", bt_hex(label, 16));
    BT_DBG("AppIdx 0x%04x TTL %u Period 0x%02x Transmit 0x%02x",
           pub->app_idx, pub->ttl, pub->period, pub->transmit);

    bt_mesh_model_msg_init(&msg, OP_MOD_PUB_VA_SET);
    net_buf_simple_add_le16(&msg, elem_addr);
    net_buf_simple_add_mem(&msg, label, 16);
    net_buf_simple_add_le16(&msg, (pub->app_idx | (pub->cred_flag << 12)));
    net_buf_simple_add_u8(&msg, pub->ttl);
    net_buf_simple_add_u8(&msg, pub->period);
    net_buf_simple_add_u8(&msg, pub->transmit);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_sub_del_all(bt_mesh_client_common_param_t *param,
                                uint16_t elem_addr, uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_MOD_SUB_DEL_ALL, 6);

    BT_DBG("ModSubDelAll");
    BT_DBG("ElemAddr 0x%04x ModID 0x%04x CID 0x%04x", elem_addr, mod_id, cid);

    bt_mesh_model_msg_init(&msg, OP_MOD_SUB_DEL_ALL);
    net_buf_simple_add_le16(&msg, elem_addr);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

static int mod_sub_get(bt_mesh_client_common_param_t *param, uint32_t op,
                       uint16_t elem_addr, uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, op, 6);

    bt_mesh_model_msg_init(&msg, op);
    net_buf_simple_add_le16(&msg, elem_addr);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_sub_get(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id)
{
    BT_DBG("ModSubGet");
    BT_DBG("ElemAddr 0x%04x ModID 0x%04x", elem_addr, mod_id);

    return mod_sub_get(param, OP_MOD_SUB_GET, elem_addr, mod_id, BLE_MESH_CID_NVAL);
}

int bt_mesh_cfg_mod_sub_get_vnd(bt_mesh_client_common_param_t *param,
                                uint16_t elem_addr, uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModSubGetVnd");
    BT_DBG("ElemAddr 0x%04x ModID 0x%04x CID 0x%04x", elem_addr, mod_id, cid);

    if (cid == BLE_MESH_CID_NVAL) {
        BT_ERR("Invalid company id");
        return -EINVAL;
    }
    return mod_sub_get(param, OP_MOD_SUB_GET_VND, elem_addr, mod_id, cid);
}

int bt_mesh_cfg_net_key_update(bt_mesh_client_common_param_t *param,
                               uint16_t net_idx, const uint8_t net_key[16])
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_NET_KEY_UPDATE, 18);

    BT_DBG("NetKeyUpdate");

    if (!net_key) {
        BT_ERR("Invalid NetKey");
        return -EINVAL;
    }

    BT_DBG("NetIdx 0x%04x NetKey %s", net_idx, bt_hex(net_key, 16));

    bt_mesh_model_msg_init(&msg, OP_NET_KEY_UPDATE);
    net_buf_simple_add_le16(&msg, net_idx);
    net_buf_simple_add_mem(&msg, net_key, 16);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_net_key_delete(bt_mesh_client_common_param_t *param, uint16_t net_idx)
{
    BT_DBG("NetKeyDelete, NetIdx 0x%04x", net_idx);

    return send_msg_with_le16(param, OP_NET_KEY_DEL, net_idx);
}

int bt_mesh_cfg_net_key_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("NetKeyGet");

    return send_msg_with_none(param, OP_NET_KEY_GET);
}

int bt_mesh_cfg_app_key_update(bt_mesh_client_common_param_t *param,
                               uint16_t net_idx, uint16_t app_idx,
                               const uint8_t app_key[16])
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_APP_KEY_UPDATE, 19);

    BT_DBG("AppKeyUpdate");

    if (!app_key) {
        BT_ERR("Invalid AppKey");
        return -EINVAL;
    }

    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x AppKey %s",
           net_idx, app_idx, bt_hex(app_key, 16));

    bt_mesh_model_msg_init(&msg, OP_APP_KEY_UPDATE);
    key_idx_pack(&msg, net_idx, app_idx);
    net_buf_simple_add_mem(&msg, app_key, 16);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_app_key_delete(bt_mesh_client_common_param_t *param,
                               uint16_t net_idx, uint16_t app_idx)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_APP_KEY_DEL, 3);

    BT_DBG("AppKeyDelete, NetIdx 0x%04x AppIdx 0x%04x", net_idx, app_idx);

    bt_mesh_model_msg_init(&msg, OP_APP_KEY_DEL);
    key_idx_pack(&msg, net_idx, app_idx);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_app_key_get(bt_mesh_client_common_param_t *param, uint16_t net_idx)
{
    BT_DBG("AppKeyGet, NetIdx 0x%04x", net_idx);

    return send_msg_with_le16(param, OP_APP_KEY_GET, net_idx);
}

int bt_mesh_cfg_node_identity_get(bt_mesh_client_common_param_t *param, uint16_t net_idx)
{
    BT_DBG("NodeIdentityGet, NetIdx 0x%04x", net_idx);

    return send_msg_with_le16(param, OP_NODE_IDENTITY_GET, net_idx);
}

int bt_mesh_cfg_node_identity_set(bt_mesh_client_common_param_t *param,
                                  uint16_t net_idx, uint8_t identity)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_NODE_IDENTITY_SET, 3);

    BT_DBG("NodeIdentitySet, NetIdx 0x%04x Identity 0x%02x", net_idx, identity);

    if (identity > 0x02) {
        BT_ERR("Invalid node identity 0x%02x", identity);
        return -EINVAL;
    }

    bt_mesh_model_msg_init(&msg, OP_NODE_IDENTITY_SET);
    net_buf_simple_add_le16(&msg, net_idx);
    net_buf_simple_add_u8(&msg, identity);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_app_unbind(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, uint16_t app_idx,
                               uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_MOD_APP_UNBIND, 8);

    BT_DBG("ModAppUnbind");
    BT_DBG("ElemAddr 0x%04x AppIdx 0x%04x ModID 0x%04x CID 0x%04x",
           elem_addr, app_idx, mod_id, cid);

    bt_mesh_model_msg_init(&msg, OP_MOD_APP_UNBIND);
    net_buf_simple_add_le16(&msg, elem_addr);
    net_buf_simple_add_le16(&msg, app_idx);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

static int mod_app_get(bt_mesh_client_common_param_t *param, uint32_t op,
                       uint16_t elem_addr, uint16_t mod_id, uint16_t cid)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, op, 6);

    bt_mesh_model_msg_init(&msg, op);
    net_buf_simple_add_le16(&msg, elem_addr);
    if (cid != BLE_MESH_CID_NVAL) {
        net_buf_simple_add_le16(&msg, cid);
    }
    net_buf_simple_add_le16(&msg, mod_id);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_mod_app_get(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id)
{
    BT_DBG("ModAppGet, ElemAddr 0x%04x ModID 0x%04x", elem_addr, mod_id);

    return mod_app_get(param, OP_SIG_MOD_APP_GET, elem_addr, mod_id, BLE_MESH_CID_NVAL);
}

int bt_mesh_cfg_mod_app_get_vnd(bt_mesh_client_common_param_t *param,
                                uint16_t elem_addr, uint16_t mod_id, uint16_t cid)
{
    BT_DBG("ModAppGetVnd");
    BT_DBG("ElemAddr 0x%04x ModID 0x%04x CID 0x%04x", elem_addr, mod_id, cid);

    if (cid == BLE_MESH_CID_NVAL) {
        BT_ERR("Invalid company id");
        return -EINVAL;
    }

    return mod_app_get(param, OP_VND_MOD_APP_GET, elem_addr, mod_id, cid);
}

int bt_mesh_cfg_kr_phase_get(bt_mesh_client_common_param_t *param, uint16_t net_idx)
{
    BT_DBG("KrPhaseGet, NetIdx 0x%04x", net_idx);

    return send_msg_with_le16(param, OP_KRP_GET, net_idx);
}

int bt_mesh_cfg_kr_phase_set(bt_mesh_client_common_param_t *param,
                             uint16_t net_idx, uint8_t transition)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, OP_KRP_SET, 3);

    BT_DBG("KrPhaseSet, NetIdx 0x%04x Transition 0x%02x", net_idx, transition);

    if (transition > 0x03) {
        BT_ERR("Invalid kr phase transition 0x%02x", transition);
        return -EINVAL;
    }

    bt_mesh_model_msg_init(&msg, OP_KRP_SET);
    net_buf_simple_add_le16(&msg, net_idx);
    net_buf_simple_add_u8(&msg, transition);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_cfg_lpn_timeout_get(bt_mesh_client_common_param_t *param, uint16_t lpn_addr)
{
    BT_DBG("LPNTimeoutGet, LPN 0x%04x", lpn_addr);

    return send_msg_with_le16(param, OP_LPN_TIMEOUT_GET, lpn_addr);
}

int bt_mesh_cfg_net_transmit_get(bt_mesh_client_common_param_t *param)
{
    BT_DBG("NetTransmitGet");

    return send_msg_with_none(param, OP_NET_TRANSMIT_GET);
}

int bt_mesh_cfg_net_transmit_set(bt_mesh_client_common_param_t *param, uint8_t transmit)
{
    BT_DBG("NetTransmitSet, Transmit 0x%02x", transmit);

    return send_msg_with_u8(param, OP_NET_TRANSMIT_SET, transmit);
}

static int cfg_cli_init(struct bt_mesh_model *model)
{
    config_internal_data_t *internal = NULL;
    bt_mesh_config_client_t *client = NULL;

    BT_DBG("CfgCliInit");

    if (!model) {
        BT_ERR("Invalid Configuration Client model");
        return -EINVAL;
    }

    if (!bt_mesh_model_in_primary(model)) {
        BT_ERR("Configuration Client only allowed in primary element");
        return -EINVAL;
    }

    client = (bt_mesh_config_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Configuration Client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    internal = bt_mesh_calloc(sizeof(config_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->op_pair_size = ARRAY_SIZE(cfg_op_pair);
    client->op_pair = cfg_op_pair;
    client->internal_data = internal;

    /* Configuration Model security is device-key based */
    model->keys[0] = BLE_MESH_KEY_DEV;

    bt_mesh_mutex_create(&cfg_client_lock);

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static int cfg_cli_deinit(struct bt_mesh_model *model)
{
    bt_mesh_config_client_t *client = NULL;

    BT_DBG("CfgCliDeinit");

    if (!model) {
        BT_ERR("Invalid Configuration Client model");
        return -EINVAL;
    }

    if (!bt_mesh_model_in_primary(model)) {
        BT_ERR("Configuration Client only allowed in primary element");
        return -EINVAL;
    }

    client = (bt_mesh_config_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Configuration Client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        /* Remove items from the list */
        bt_mesh_client_clear_list(client->internal_data);

        /* Free the allocated internal data */
        bt_mesh_free(client->internal_data);
        client->internal_data = NULL;
    }

    bt_mesh_mutex_free(&cfg_client_lock);

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_cfg_cli_cb = {
    .init = cfg_cli_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = cfg_cli_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_CFG_CLI */
