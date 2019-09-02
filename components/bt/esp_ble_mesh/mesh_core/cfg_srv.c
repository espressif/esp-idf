/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "sdkconfig.h"
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLE_MESH_DEBUG_MODEL)

#include "mesh_types.h"
#include "mesh_util.h"
#include "mesh_main.h"
#include "mesh_trace.h"
#include "cfg_srv.h"
#include "settings.h"

#include "mesh.h"
#include "adv.h"
#include "net.h"
#include "lpn.h"
#include "transport.h"
#include "crypto.h"
#include "access.h"
#include "beacon.h"
#include "proxy.h"
#include "foundation.h"
#include "friend.h"
#include "settings.h"

#include "mesh_common.h"
#include "btc_ble_mesh_config_model.h"

#define DEFAULT_TTL 7

/* Maximum message length is 384 in BLE Mesh. Here for composition data,
 * due to 1 octet opcode and 4 octets TransMIC, 379 octets can be used to
 * store device composition data.
 */
#define COMP_DATA_MAX_LEN   379

static struct bt_mesh_cfg_srv *conf;

static struct label {
    u16_t ref;
    u16_t addr;
    u8_t  uuid[16];
} labels[CONFIG_BLE_MESH_LABEL_COUNT];

static int comp_add_elem(struct net_buf_simple *buf, struct bt_mesh_elem *elem,
                         bool primary)
{
    struct bt_mesh_model *mod;
    int i;

    if (net_buf_simple_tailroom(buf) <
            4 + (elem->model_count * 2U) + (elem->vnd_model_count * 2U)) {
        BT_ERR("%s, Too large device composition", __func__);
        return -E2BIG;
    }

    net_buf_simple_add_le16(buf, elem->loc);

    net_buf_simple_add_u8(buf, elem->model_count);
    net_buf_simple_add_u8(buf, elem->vnd_model_count);

    for (i = 0; i < elem->model_count; i++) {
        mod = &elem->models[i];
        net_buf_simple_add_le16(buf, mod->id);
    }

    for (i = 0; i < elem->vnd_model_count; i++) {
        mod = &elem->vnd_models[i];
        net_buf_simple_add_le16(buf, mod->vnd.company);
        net_buf_simple_add_le16(buf, mod->vnd.id);
    }

    return 0;
}

static int comp_get_page_0(struct net_buf_simple *buf)
{
    u16_t feat = 0U;
    const struct bt_mesh_comp *comp;
    int i;

    comp = bt_mesh_comp_get();

    if (IS_ENABLED(CONFIG_BLE_MESH_RELAY)) {
        feat |= BLE_MESH_FEAT_RELAY;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY)) {
        feat |= BLE_MESH_FEAT_PROXY;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        feat |= BLE_MESH_FEAT_FRIEND;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        feat |= BLE_MESH_FEAT_LOW_POWER;
    }

    net_buf_simple_add_le16(buf, comp->cid);
    net_buf_simple_add_le16(buf, comp->pid);
    net_buf_simple_add_le16(buf, comp->vid);
    net_buf_simple_add_le16(buf, CONFIG_BLE_MESH_CRPL);
    net_buf_simple_add_le16(buf, feat);

    for (i = 0; i < comp->elem_count; i++) {
        int err;

        err = comp_add_elem(buf, &comp->elem[i], i == 0);
        if (err) {
            return err;
        }
    }

    return 0;
}

static void dev_comp_data_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct net_buf_simple *sdu = NULL;
    u8_t page;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    page = net_buf_simple_pull_u8(buf);
    if (page != 0U) {
        BT_WARN("Composition page %u not available", page);
        page = 0U;
    }

    sdu = bt_mesh_alloc_buf(MIN(BLE_MESH_TX_SDU_MAX, COMP_DATA_MAX_LEN));
    if (!sdu) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return;
    }

    bt_mesh_model_msg_init(sdu, OP_DEV_COMP_DATA_STATUS);

    net_buf_simple_add_u8(sdu, page);
    if (comp_get_page_0(sdu) < 0) {
        BT_ERR("%s, Unable to get composition page 0", __func__);
        bt_mesh_free_buf(sdu);
        return;
    }

    if (bt_mesh_model_send(model, ctx, sdu, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Composition Data Status", __func__);
    }

    bt_mesh_free_buf(sdu);
    return;
}

static struct bt_mesh_model *get_model(struct bt_mesh_elem *elem,
                                       struct net_buf_simple *buf, bool *vnd)
{
    if (buf->len < 4) {
        u16_t id;

        id = net_buf_simple_pull_le16(buf);

        BT_DBG("ID 0x%04x addr 0x%04x", id, elem->addr);

        *vnd = false;

        return bt_mesh_model_find(elem, id);
    } else {
        u16_t company, id;

        company = net_buf_simple_pull_le16(buf);
        id = net_buf_simple_pull_le16(buf);

        BT_DBG("Company 0x%04x ID 0x%04x addr 0x%04x", company, id,
               elem->addr);

        *vnd = true;

        return bt_mesh_model_find_vnd(elem, company, id);
    }
}

static bool app_key_is_valid(u16_t app_idx)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

        if (key->net_idx != BLE_MESH_KEY_UNUSED &&
                key->app_idx == app_idx) {
            return true;
        }
    }

    return false;
}

static u8_t _mod_pub_set(struct bt_mesh_model *model, u16_t pub_addr,
                         u16_t app_idx, u8_t cred_flag, u8_t ttl, u8_t period,
                         u8_t retransmit, bool store)
{
    if (!model->pub) {
        return STATUS_NVAL_PUB_PARAM;
    }

    if (!IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) && cred_flag) {
        return STATUS_FEAT_NOT_SUPP;
    }

    if (!model->pub->update && period) {
        return STATUS_NVAL_PUB_PARAM;
    }

    if (pub_addr == BLE_MESH_ADDR_UNASSIGNED) {
        if (model->pub->addr == BLE_MESH_ADDR_UNASSIGNED) {
            return STATUS_SUCCESS;
        }

        model->pub->addr = BLE_MESH_ADDR_UNASSIGNED;
        model->pub->key = 0U;
        model->pub->cred = 0U;
        model->pub->ttl = 0U;
        model->pub->period = 0U;
        model->pub->retransmit = 0U;
        model->pub->count = 0U;

        if (model->pub->update) {
            k_delayed_work_cancel(&model->pub->timer);
        }

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
            bt_mesh_store_mod_pub(model);
        }

        return STATUS_SUCCESS;
    }

    if (!bt_mesh_app_key_find(app_idx)) {
        return STATUS_INVALID_APPKEY;
    }

    model->pub->addr = pub_addr;
    model->pub->key = app_idx;
    model->pub->cred = cred_flag;
    model->pub->ttl = ttl;
    model->pub->period = period;
    model->pub->retransmit = retransmit;

    if (model->pub->update) {
        s32_t period_ms;

        period_ms = bt_mesh_model_pub_period_get(model);
        BT_DBG("period %u ms", period_ms);

        if (period_ms) {
            k_delayed_work_submit(&model->pub->timer, period_ms);
        } else {
            k_delayed_work_cancel(&model->pub->timer);
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
        bt_mesh_store_mod_pub(model);
    }

    return STATUS_SUCCESS;
}

static u8_t mod_bind(struct bt_mesh_model *model, u16_t key_idx)
{
    int i;

    BT_DBG("model %p key_idx 0x%03x", model, key_idx);

    if (!app_key_is_valid(key_idx)) {
        return STATUS_INVALID_APPKEY;
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        /* Treat existing binding as success */
        if (model->keys[i] == key_idx) {
            return STATUS_SUCCESS;
        }
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] == BLE_MESH_KEY_UNUSED) {
            model->keys[i] = key_idx;

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_mod_bind(model);
            }

            return STATUS_SUCCESS;
        }
    }

    return STATUS_INSUFF_RESOURCES;
}

static u8_t mod_unbind(struct bt_mesh_model *model, u16_t key_idx, bool store)
{
    int i;

    BT_DBG("model %p key_idx 0x%03x store %u", model, key_idx, store);

    if (!app_key_is_valid(key_idx)) {
        return STATUS_INVALID_APPKEY;
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] != key_idx) {
            continue;
        }

        model->keys[i] = BLE_MESH_KEY_UNUSED;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
            bt_mesh_store_mod_bind(model);
        }

        if (model->pub && model->pub->key == key_idx) {
            _mod_pub_set(model, BLE_MESH_ADDR_UNASSIGNED,
                         0, 0, 0, 0, 0, store);
        }
    }

    return STATUS_SUCCESS;
}

struct bt_mesh_app_key *bt_mesh_app_key_alloc(u16_t app_idx)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

        if (key->net_idx == BLE_MESH_KEY_UNUSED) {
            return key;
        }
    }

    return NULL;
}

static u8_t app_key_set(u16_t net_idx, u16_t app_idx, const u8_t val[16],
                        bool update)
{
    struct bt_mesh_app_keys *keys;
    struct bt_mesh_app_key *key;
    struct bt_mesh_subnet *sub;

    BT_DBG("net_idx 0x%04x app_idx %04x update %u val %s",
           net_idx, app_idx, update, bt_hex(val, 16));

    sub = bt_mesh_subnet_get(net_idx);
    if (!sub) {
        return STATUS_INVALID_NETKEY;
    }

    key = bt_mesh_app_key_find(app_idx);
    if (update) {
        if (!key) {
            return STATUS_INVALID_APPKEY;
        }

        if (key->net_idx != net_idx) {
            return STATUS_INVALID_BINDING;
        }

        keys = &key->keys[1];

        /* The AppKey Update message shall generate an error when node
         * is in normal operation, Phase 2, or Phase 3 or in Phase 1
         * when the AppKey Update message on a valid AppKeyIndex when
         * the AppKey value is different.
         */
        if (sub->kr_phase != BLE_MESH_KR_PHASE_1) {
            return STATUS_CANNOT_UPDATE;
        }

        if (key->updated) {
            if (memcmp(keys->val, val, 16)) {
                return STATUS_CANNOT_UPDATE;
            } else {
                return STATUS_SUCCESS;
            }
        }

        key->updated = true;
    } else {
        if (key) {
            if (key->net_idx == net_idx &&
                    !memcmp(key->keys[0].val, val, 16)) {
                return STATUS_SUCCESS;
            }

            if (key->net_idx == net_idx) {
                return STATUS_IDX_ALREADY_STORED;
            } else {
                return STATUS_INVALID_NETKEY;
            }
        }

        key = bt_mesh_app_key_alloc(app_idx);
        if (!key) {
            return STATUS_INSUFF_RESOURCES;
        }

        keys = &key->keys[0];
    }

    if (bt_mesh_app_id(val, &keys->id)) {
        if (update) {
            key->updated = false;
        }

        return STATUS_STORAGE_FAIL;
    }

    BT_DBG("app_idx 0x%04x AID 0x%02x", app_idx, keys->id);

    key->net_idx = net_idx;
    key->app_idx = app_idx;
    memcpy(keys->val, val, 16);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        BT_DBG("Storing AppKey persistently");
        bt_mesh_store_app_key(key);
    }

    return STATUS_SUCCESS;
}

static void app_key_add(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 4 + 4);
    u16_t key_net_idx, key_app_idx;
    u8_t status;

    key_idx_unpack(buf, &key_net_idx, &key_app_idx);

    BT_DBG("AppIdx 0x%04x NetIdx 0x%04x", key_app_idx, key_net_idx);

    bt_mesh_model_msg_init(&msg, OP_APP_KEY_STATUS);

    status = app_key_set(key_net_idx, key_app_idx, buf->data, false);
    BT_DBG("status 0x%02x", status);
    net_buf_simple_add_u8(&msg, status);

    key_idx_pack(&msg, key_net_idx, key_app_idx);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config AppKey Status", __func__);
        return;
    }

#if defined(CONFIG_BLE_MESH_FAST_PROV)
    bt_mesh_config_server_cb_evt_to_btc(BTC_BLE_MESH_EVT_CONFIG_SERVER_RECV_MSG,
        model, ctx, (u8_t *)&key_app_idx, sizeof(u16_t));
#endif
}

static void app_key_update(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 4 + 4);
    u16_t key_net_idx, key_app_idx;
    u8_t status;

    key_idx_unpack(buf, &key_net_idx, &key_app_idx);

    BT_DBG("AppIdx 0x%04x NetIdx 0x%04x", key_app_idx, key_net_idx);

    bt_mesh_model_msg_init(&msg, OP_APP_KEY_STATUS);

    status = app_key_set(key_net_idx, key_app_idx, buf->data, true);
    BT_DBG("status 0x%02x", status);
    net_buf_simple_add_u8(&msg, status);

    key_idx_pack(&msg, key_net_idx, key_app_idx);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config AppKey Status", __func__);
    }
}

struct unbind_data {
    u16_t app_idx;
    bool store;
};

static void _mod_unbind(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                        bool vnd, bool primary, void *user_data)
{
    struct unbind_data *data = user_data;

    mod_unbind(mod, data->app_idx, data->store);
}

void bt_mesh_app_key_del(struct bt_mesh_app_key *key, bool store)
{
    struct unbind_data data = { .app_idx = key->app_idx, .store = store };

    BT_DBG("AppIdx 0x%03x store %u", key->app_idx, store);

    bt_mesh_model_foreach(_mod_unbind, &data);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
        bt_mesh_clear_app_key(key);
    }

    key->net_idx = BLE_MESH_KEY_UNUSED;
    (void)memset(key->keys, 0, sizeof(key->keys));
}

static void app_key_del(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 4 + 4);
    u16_t key_net_idx, key_app_idx;
    struct bt_mesh_app_key *key;
    u8_t status;

    key_idx_unpack(buf, &key_net_idx, &key_app_idx);

    BT_DBG("AppIdx 0x%04x NetIdx 0x%04x", key_app_idx, key_net_idx);

    if (!bt_mesh_subnet_get(key_net_idx)) {
        status = STATUS_INVALID_NETKEY;
        goto send_status;
    }

    key = bt_mesh_app_key_find(key_app_idx);
    if (!key) {
        /* Treat as success since the client might have missed a
         * previous response and is resending the request.
         */
        status = STATUS_SUCCESS;
        goto send_status;
    }

    if (key->net_idx != key_net_idx) {
        status = STATUS_INVALID_BINDING;
        goto send_status;
    }

    bt_mesh_app_key_del(key, true);
    status = STATUS_SUCCESS;

send_status:
    bt_mesh_model_msg_init(&msg, OP_APP_KEY_STATUS);

    net_buf_simple_add_u8(&msg, status);

    key_idx_pack(&msg, key_net_idx, key_app_idx);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config AppKey Status", __func__);
    }
}

/* Index list length: 3 bytes for every pair and 2 bytes for an odd idx */
#define IDX_LEN(num) (((num) / 2) * 3 + ((num) % 2) * 2)

static void app_key_get(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 3 + 4 +
                          IDX_LEN(CONFIG_BLE_MESH_APP_KEY_COUNT));
    u16_t get_idx, i, prev;
    u8_t status;

    get_idx = net_buf_simple_pull_le16(buf);
    if (get_idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, get_idx);
        return;
    }

    BT_DBG("idx 0x%04x", get_idx);

    bt_mesh_model_msg_init(&msg, OP_APP_KEY_LIST);

    if (!bt_mesh_subnet_get(get_idx)) {
        status = STATUS_INVALID_NETKEY;
    } else {
        status = STATUS_SUCCESS;
    }

    net_buf_simple_add_u8(&msg, status);
    net_buf_simple_add_le16(&msg, get_idx);

    if (status != STATUS_SUCCESS) {
        goto send_status;
    }

    prev = BLE_MESH_KEY_UNUSED;
    for (i = 0U; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

        if (key->net_idx != get_idx) {
            continue;
        }

        if (prev == BLE_MESH_KEY_UNUSED) {
            prev = key->app_idx;
            continue;
        }

        key_idx_pack(&msg, prev, key->app_idx);
        prev = BLE_MESH_KEY_UNUSED;
    }

    if (prev != BLE_MESH_KEY_UNUSED) {
        net_buf_simple_add_le16(&msg, prev);
    }

send_status:
    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config AppKey List", __func__);
    }
}

static void beacon_get(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    bt_mesh_model_msg_init(&msg, OP_BEACON_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_beacon_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Beacon Status", __func__);
    }
}

static void beacon_set(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    if (!cfg) {
        BT_WARN("No Configuration Server context available");
    } else if (buf->data[0] == 0x00 || buf->data[0] == 0x01) {
        if (buf->data[0] != cfg->beacon) {
            cfg->beacon = buf->data[0];

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_cfg();
            }

#if CONFIG_BLE_MESH_NODE
            if (cfg->beacon) {
                bt_mesh_beacon_enable();
            } else {
                bt_mesh_beacon_disable();
            }
#endif
        }
    } else {
        BT_WARN("Invalid Config Beacon value 0x%02x", buf->data[0]);
        return;
    }

    bt_mesh_model_msg_init(&msg, OP_BEACON_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_beacon_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Beacon Status", __func__);
    }
}

static void default_ttl_get(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    bt_mesh_model_msg_init(&msg, OP_DEFAULT_TTL_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_default_ttl_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Default TTL Status", __func__);
    }
}

static void default_ttl_set(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    if (!cfg) {
        BT_WARN("No Configuration Server context available");
    } else if (buf->data[0] <= BLE_MESH_TTL_MAX && buf->data[0] != 0x01) {
        if (cfg->default_ttl != buf->data[0]) {
            cfg->default_ttl = buf->data[0];

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_cfg();
            }
        }
    } else {
        BT_WARN("Prohibited Default TTL value 0x%02x", buf->data[0]);
        return;
    }

    bt_mesh_model_msg_init(&msg, OP_DEFAULT_TTL_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_default_ttl_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Default TTL Status", __func__);
    }
}

static void send_gatt_proxy_status(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);

    bt_mesh_model_msg_init(&msg, OP_GATT_PROXY_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_gatt_proxy_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config GATT Proxy Status", __func__);
    }
}

static void gatt_proxy_get(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    send_gatt_proxy_status(model, ctx);
}

static void gatt_proxy_set(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    if (buf->data[0] != 0x00 && buf->data[0] != 0x01) {
        BT_WARN("Invalid GATT Proxy value 0x%02x", buf->data[0]);
        return;
    }

    if (!IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY) ||
            bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_NOT_SUPPORTED) {
        goto send_status;
    }

    if (!cfg) {
        BT_WARN("No Configuration Server context available");
        goto send_status;
    }

    BT_DBG("GATT Proxy 0x%02x -> 0x%02x", cfg->gatt_proxy, buf->data[0]);

    if (cfg->gatt_proxy == buf->data[0]) {
        goto send_status;
    }

    cfg->gatt_proxy = buf->data[0];

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_cfg();
    }

#if CONFIG_BLE_MESH_NODE
    if (cfg->gatt_proxy == BLE_MESH_GATT_PROXY_DISABLED) {
        int i;

        /* Section 4.2.11.1: "When the GATT Proxy state is set to
         * 0x00, the Node Identity state for all subnets shall be set
         * to 0x00 and shall not be changed."
         */
        for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
            struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

            if (sub->net_idx != BLE_MESH_KEY_UNUSED) {
                bt_mesh_proxy_identity_stop(sub);
            }
        }

        /* Section 4.2.11: "Upon transition from GATT Proxy state 0x01
         * to GATT Proxy state 0x00 the GATT Bearer Server shall
         * disconnect all GATT Bearer Clients.
         */
        bt_mesh_proxy_gatt_disconnect();
    }

    bt_mesh_adv_update();
#endif

    if (cfg->hb_pub.feat & BLE_MESH_FEAT_PROXY) {
        bt_mesh_heartbeat_send();
    }

send_status:
    send_gatt_proxy_status(model, ctx);
}

static void net_transmit_get(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    bt_mesh_model_msg_init(&msg, OP_NET_TRANSMIT_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_net_transmit_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Network Transmit Status", __func__);
    }
}

static void net_transmit_set(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    BT_DBG("Transmit 0x%02x (count %u interval %ums)", buf->data[0],
           BLE_MESH_TRANSMIT_COUNT(buf->data[0]),
           BLE_MESH_TRANSMIT_INT(buf->data[0]));

    if (!cfg) {
        BT_WARN("No Configuration Server context available");
    } else {
        cfg->net_transmit = buf->data[0];

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_cfg();
        }
    }

    bt_mesh_model_msg_init(&msg, OP_NET_TRANSMIT_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_net_transmit_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Network Transmit Status", __func__);
    }
}

static void relay_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 2 + 4);

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    bt_mesh_model_msg_init(&msg, OP_RELAY_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_relay_get());
    net_buf_simple_add_u8(&msg, bt_mesh_relay_retransmit_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Relay Status", __func__);
    }
}

static void relay_set(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 2 + 4);
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    if (!cfg) {
        BT_WARN("No Configuration Server context available");
    } else if (buf->data[0] == 0x00 || buf->data[0] == 0x01) {
        bool change;

        if (cfg->relay == BLE_MESH_RELAY_NOT_SUPPORTED) {
            change = false;
        } else {
            change = (cfg->relay != buf->data[0]);
            cfg->relay = buf->data[0];
            cfg->relay_retransmit = buf->data[1];

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_cfg();
            }
        }

        BT_DBG("Relay 0x%02x (%s) xmit 0x%02x (count %u interval %u)",
               cfg->relay, change ? "changed" : "not changed",
               cfg->relay_retransmit,
               BLE_MESH_TRANSMIT_COUNT(cfg->relay_retransmit),
               BLE_MESH_TRANSMIT_INT(cfg->relay_retransmit));

        if ((cfg->hb_pub.feat & BLE_MESH_FEAT_RELAY) && change) {
            bt_mesh_heartbeat_send();
        }
    } else {
        BT_WARN("Invalid Relay value 0x%02x", buf->data[0]);
        return;
    }

    bt_mesh_model_msg_init(&msg, OP_RELAY_STATUS);
    net_buf_simple_add_u8(&msg, bt_mesh_relay_get());
    net_buf_simple_add_u8(&msg, bt_mesh_relay_retransmit_get());

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Relay Status", __func__);
    }
}

static void send_mod_pub_status(struct bt_mesh_model *cfg_mod,
                                struct bt_mesh_msg_ctx *ctx,
                                u16_t elem_addr, u16_t pub_addr,
                                bool vnd, struct bt_mesh_model *mod,
                                u8_t status, u8_t *mod_id)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 14 + 4);

    bt_mesh_model_msg_init(&msg, OP_MOD_PUB_STATUS);

    net_buf_simple_add_u8(&msg, status);
    net_buf_simple_add_le16(&msg, elem_addr);

    if (status != STATUS_SUCCESS) {
        (void)memset(net_buf_simple_add(&msg, 7), 0, 7);
    } else {
        u16_t idx_cred;

        net_buf_simple_add_le16(&msg, pub_addr);

        idx_cred = mod->pub->key | (u16_t)mod->pub->cred << 12;
        net_buf_simple_add_le16(&msg, idx_cred);
        net_buf_simple_add_u8(&msg, mod->pub->ttl);
        net_buf_simple_add_u8(&msg, mod->pub->period);
        net_buf_simple_add_u8(&msg, mod->pub->retransmit);
    }

    if (vnd) {
        memcpy(net_buf_simple_add(&msg, 4), mod_id, 4);
    } else {
        memcpy(net_buf_simple_add(&msg, 2), mod_id, 2);
    }

    if (bt_mesh_model_send(cfg_mod, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Model Publication Status", __func__);
    }
}

static void mod_pub_get(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    u16_t elem_addr, pub_addr = 0U;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id, status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    mod_id = buf->data;

    BT_DBG("elem_addr 0x%04x", elem_addr);

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    if (!mod->pub) {
        status = STATUS_NVAL_PUB_PARAM;
        goto send_status;
    }

    pub_addr = mod->pub->addr;
    status = STATUS_SUCCESS;

send_status:
    send_mod_pub_status(model, ctx, elem_addr, pub_addr, vnd, mod,
                        status, mod_id);
}

static void mod_pub_set(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    u8_t retransmit, status, pub_ttl, pub_period, cred_flag;
    u16_t elem_addr, pub_addr, pub_app_idx;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    pub_addr = net_buf_simple_pull_le16(buf);
    pub_app_idx = net_buf_simple_pull_le16(buf);
    cred_flag = ((pub_app_idx >> 12) & BIT_MASK(1));
    pub_app_idx &= BIT_MASK(12);

    pub_ttl = net_buf_simple_pull_u8(buf);
    if (pub_ttl > BLE_MESH_TTL_MAX && pub_ttl != BLE_MESH_TTL_DEFAULT) {
        BT_ERR("%s, Invalid TTL value 0x%02x", __func__, pub_ttl);
        return;
    }

    pub_period = net_buf_simple_pull_u8(buf);
    retransmit = net_buf_simple_pull_u8(buf);
    mod_id = buf->data;

    BT_DBG("elem_addr 0x%04x pub_addr 0x%04x cred_flag %u",
           elem_addr, pub_addr, cred_flag);
    BT_DBG("pub_app_idx 0x%03x, pub_ttl %u pub_period 0x%02x",
           pub_app_idx, pub_ttl, pub_period);
    BT_DBG("retransmit 0x%02x (count %u interval %ums)", retransmit,
           BLE_MESH_PUB_TRANSMIT_COUNT(retransmit),
           BLE_MESH_PUB_TRANSMIT_INT(retransmit));

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = _mod_pub_set(mod, pub_addr, pub_app_idx, cred_flag, pub_ttl,
                          pub_period, retransmit, true);

send_status:
    send_mod_pub_status(model, ctx, elem_addr, pub_addr, vnd, mod,
                        status, mod_id);
}

#if CONFIG_BLE_MESH_LABEL_COUNT > 0
static u8_t va_add(u8_t *label_uuid, u16_t *addr)
{
    struct label *free_slot = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(labels); i++) {
        if (!labels[i].ref) {
            free_slot = &labels[i];
            continue;
        }

        if (!memcmp(labels[i].uuid, label_uuid, 16)) {
            *addr = labels[i].addr;
            labels[i].ref++;
            return STATUS_SUCCESS;
        }
    }

    if (!free_slot) {
        return STATUS_INSUFF_RESOURCES;
    }

    if (bt_mesh_virtual_addr(label_uuid, addr) < 0) {
        return STATUS_UNSPECIFIED;
    }

    free_slot->ref = 1U;
    free_slot->addr = *addr;
    memcpy(free_slot->uuid, label_uuid, 16);

    return STATUS_SUCCESS;
}

static u8_t va_del(u8_t *label_uuid, u16_t *addr)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(labels); i++) {
        if (!memcmp(labels[i].uuid, label_uuid, 16)) {
            if (addr) {
                *addr = labels[i].addr;
            }

            labels[i].ref--;
            return STATUS_SUCCESS;
        }
    }

    if (addr) {
        *addr = BLE_MESH_ADDR_UNASSIGNED;
    }

    return STATUS_CANNOT_REMOVE;
}

static size_t mod_sub_list_clear(struct bt_mesh_model *mod)
{
    u8_t *label_uuid;
    size_t clear_count;
    int i;

    /* Unref stored labels related to this model */
    for (i = 0, clear_count = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (!BLE_MESH_ADDR_IS_VIRTUAL(mod->groups[i])) {
            if (mod->groups[i] != BLE_MESH_ADDR_UNASSIGNED) {
                mod->groups[i] = BLE_MESH_ADDR_UNASSIGNED;
                clear_count++;
            }

            continue;
        }

        label_uuid = bt_mesh_label_uuid_get(mod->groups[i]);

        mod->groups[i] = BLE_MESH_ADDR_UNASSIGNED;
        clear_count++;

        if (label_uuid) {
            va_del(label_uuid, NULL);
        } else {
            BT_ERR("%s, Label UUID not found", __func__);
        }
    }

    return clear_count;
}

static void mod_pub_va_set(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    u8_t retransmit, status, pub_ttl, pub_period, cred_flag;
    u16_t elem_addr, pub_addr, pub_app_idx;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *label_uuid;
    u8_t *mod_id;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    label_uuid = net_buf_simple_pull_mem(buf, 16);
    pub_app_idx = net_buf_simple_pull_le16(buf);
    cred_flag = ((pub_app_idx >> 12) & BIT_MASK(1));
    pub_app_idx &= BIT_MASK(12);
    pub_ttl = net_buf_simple_pull_u8(buf);
    if (pub_ttl > BLE_MESH_TTL_MAX && pub_ttl != BLE_MESH_TTL_DEFAULT) {
        BT_ERR("%s, Invalid TTL value 0x%02x", __func__, pub_ttl);
        return;
    }

    pub_period = net_buf_simple_pull_u8(buf);
    retransmit = net_buf_simple_pull_u8(buf);
    mod_id = buf->data;

    BT_DBG("elem_addr 0x%04x cred_flag %u", elem_addr, cred_flag);
    BT_DBG("pub_app_idx 0x%03x, pub_ttl %u pub_period 0x%02x",
           pub_app_idx, pub_ttl, pub_period);
    BT_DBG("retransmit 0x%02x (count %u interval %ums)", retransmit,
           BLE_MESH_PUB_TRANSMIT_COUNT(retransmit),
           BLE_MESH_PUB_TRANSMIT_INT(retransmit));

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        pub_addr = 0U;
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        pub_addr = 0U;
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = va_add(label_uuid, &pub_addr);
    if (status == STATUS_SUCCESS) {
        status = _mod_pub_set(mod, pub_addr, pub_app_idx, cred_flag,
                              pub_ttl, pub_period, retransmit, true);
    }

send_status:
    send_mod_pub_status(model, ctx, elem_addr, pub_addr, vnd, mod,
                        status, mod_id);
}
#else
static size_t mod_sub_list_clear(struct bt_mesh_model *mod)
{
    size_t clear_count;
    int i;

    /* Unref stored labels related to this model */
    for (i = 0, clear_count = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] != BLE_MESH_ADDR_UNASSIGNED) {
            mod->groups[i] = BLE_MESH_ADDR_UNASSIGNED;
            clear_count++;
        }
    }

    return clear_count;
}

static void mod_pub_va_set(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    u8_t *mod_id, status;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u16_t elem_addr, pub_addr = 0U;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    net_buf_simple_pull(buf, 16);
    mod_id = net_buf_simple_pull(buf, 4);

    BT_DBG("elem_addr 0x%04x", elem_addr);

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    if (!mod->pub) {
        status = STATUS_NVAL_PUB_PARAM;
        goto send_status;
    }

    pub_addr = mod->pub->addr;
    status = STATUS_INSUFF_RESOURCES;

send_status:
    send_mod_pub_status(model, ctx, elem_addr, pub_addr, vnd, mod,
                        status, mod_id);
}
#endif /* CONFIG_BLE_MESH_LABEL_COUNT > 0 */

static void send_mod_sub_status(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx, u8_t status,
                                u16_t elem_addr, u16_t sub_addr, u8_t *mod_id,
                                bool vnd)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 9 + 4);

    BT_DBG("status 0x%02x elem_addr 0x%04x sub_addr 0x%04x", status,
           elem_addr, sub_addr);

    bt_mesh_model_msg_init(&msg, OP_MOD_SUB_STATUS);

    net_buf_simple_add_u8(&msg, status);
    net_buf_simple_add_le16(&msg, elem_addr);
    net_buf_simple_add_le16(&msg, sub_addr);

    if (vnd) {
        memcpy(net_buf_simple_add(&msg, 4), mod_id, 4);
    } else {
        memcpy(net_buf_simple_add(&msg, 2), mod_id, 2);
    }

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Model Subscription Status", __func__);
    }
}

static void mod_sub_add(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    u16_t elem_addr, sub_addr;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id;
    u8_t status;
    bool vnd;
    int i;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    sub_addr = net_buf_simple_pull_le16(buf);

    BT_DBG("elem_addr 0x%04x, sub_addr 0x%04x", elem_addr, sub_addr);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    if (!BLE_MESH_ADDR_IS_GROUP(sub_addr)) {
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    if (bt_mesh_model_find_group(mod, sub_addr)) {
        /* Tried to add existing subscription */
        BT_DBG("found existing subscription");
        status = STATUS_SUCCESS;
        goto send_status;
    }

    for (i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] == BLE_MESH_ADDR_UNASSIGNED) {
            mod->groups[i] = sub_addr;
            break;
        }
    }

    if (i == ARRAY_SIZE(mod->groups)) {
        status = STATUS_INSUFF_RESOURCES;
    } else {
        status = STATUS_SUCCESS;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_mod_sub(mod);
        }

        if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
            bt_mesh_lpn_group_add(sub_addr);
        }
    }

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr, sub_addr,
                        mod_id, vnd);
}

static void mod_sub_del(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    u16_t elem_addr, sub_addr;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id;
    u16_t *match;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    sub_addr = net_buf_simple_pull_le16(buf);

    BT_DBG("elem_addr 0x%04x sub_addr 0x%04x", elem_addr, sub_addr);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    if (!BLE_MESH_ADDR_IS_GROUP(sub_addr)) {
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    /* An attempt to remove a non-existing address shall be treated
     * as a success.
     */
    status = STATUS_SUCCESS;

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(&sub_addr, 1);
    }

    match = bt_mesh_model_find_group(mod, sub_addr);
    if (match) {
        *match = BLE_MESH_ADDR_UNASSIGNED;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_mod_sub(mod);
        }
    }

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr, sub_addr,
                        mod_id, vnd);
}

static void mod_sub_overwrite(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    u16_t elem_addr, sub_addr;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    sub_addr = net_buf_simple_pull_le16(buf);

    BT_DBG("elem_addr 0x%04x sub_addr 0x%04x", elem_addr, sub_addr);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    if (!BLE_MESH_ADDR_IS_GROUP(sub_addr)) {
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(mod->groups, ARRAY_SIZE(mod->groups));
    }

    mod_sub_list_clear(mod);

    if (ARRAY_SIZE(mod->groups) > 0) {
        mod->groups[0] = sub_addr;
        status = STATUS_SUCCESS;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_mod_sub(mod);
        }

        if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
            bt_mesh_lpn_group_add(sub_addr);
        }
    } else {
        status = STATUS_INSUFF_RESOURCES;
    }


send_status:
    send_mod_sub_status(model, ctx, status, elem_addr, sub_addr,
                        mod_id, vnd);
}

static void mod_sub_del_all(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u16_t elem_addr;
    u8_t *mod_id;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    BT_DBG("elem_addr 0x%04x", elem_addr);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(mod->groups, ARRAY_SIZE(mod->groups));
    }

    mod_sub_list_clear(mod);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_mod_sub(mod);
    }

    status = STATUS_SUCCESS;

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr,
                        BLE_MESH_ADDR_UNASSIGNED, mod_id, vnd);
}

static void mod_sub_get(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 5 + 4 +
                          CONFIG_BLE_MESH_MODEL_GROUP_COUNT * 2);
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u16_t addr, id;
    int i;

    addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, addr);
        return;
    }

    id = net_buf_simple_pull_le16(buf);

    BT_DBG("addr 0x%04x id 0x%04x", addr, id);

    bt_mesh_model_msg_init(&msg, OP_MOD_SUB_LIST);

    elem = bt_mesh_elem_find(addr);
    if (!elem) {
        net_buf_simple_add_u8(&msg, STATUS_INVALID_ADDRESS);
        net_buf_simple_add_le16(&msg, addr);
        net_buf_simple_add_le16(&msg, id);
        goto send_list;
    }

    mod = bt_mesh_model_find(elem, id);
    if (!mod) {
        net_buf_simple_add_u8(&msg, STATUS_INVALID_MODEL);
        net_buf_simple_add_le16(&msg, addr);
        net_buf_simple_add_le16(&msg, id);
        goto send_list;
    }

    net_buf_simple_add_u8(&msg, STATUS_SUCCESS);

    net_buf_simple_add_le16(&msg, addr);
    net_buf_simple_add_le16(&msg, id);

    for (i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] != BLE_MESH_ADDR_UNASSIGNED) {
            net_buf_simple_add_le16(&msg, mod->groups[i]);
        }
    }

send_list:
    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Model Subscription List", __func__);
    }
}

static void mod_sub_get_vnd(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 7 + 4 +
                          CONFIG_BLE_MESH_MODEL_GROUP_COUNT * 2);
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u16_t company, addr, id;
    int i;

    addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, addr);
        return;
    }

    company = net_buf_simple_pull_le16(buf);
    id = net_buf_simple_pull_le16(buf);

    BT_DBG("addr 0x%04x company 0x%04x id 0x%04x", addr, company, id);

    bt_mesh_model_msg_init(&msg, OP_MOD_SUB_LIST_VND);

    elem = bt_mesh_elem_find(addr);
    if (!elem) {
        net_buf_simple_add_u8(&msg, STATUS_INVALID_ADDRESS);
        net_buf_simple_add_le16(&msg, addr);
        net_buf_simple_add_le16(&msg, company);
        net_buf_simple_add_le16(&msg, id);
        goto send_list;
    }

    mod = bt_mesh_model_find_vnd(elem, company, id);
    if (!mod) {
        net_buf_simple_add_u8(&msg, STATUS_INVALID_MODEL);
        net_buf_simple_add_le16(&msg, addr);
        net_buf_simple_add_le16(&msg, company);
        net_buf_simple_add_le16(&msg, id);
        goto send_list;
    }

    net_buf_simple_add_u8(&msg, STATUS_SUCCESS);

    net_buf_simple_add_le16(&msg, addr);
    net_buf_simple_add_le16(&msg, company);
    net_buf_simple_add_le16(&msg, id);

    for (i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] != BLE_MESH_ADDR_UNASSIGNED) {
            net_buf_simple_add_le16(&msg, mod->groups[i]);
        }
    }

send_list:
    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Vendor Model Subscription List", __func__);
    }
}

#if CONFIG_BLE_MESH_LABEL_COUNT > 0
static void mod_sub_va_add(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    u16_t elem_addr, sub_addr;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *label_uuid;
    u8_t *mod_id;
    u8_t status;
    bool vnd;
    int i;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    label_uuid = net_buf_simple_pull_mem(buf, 16);

    BT_DBG("elem_addr 0x%04x", elem_addr);

    mod_id = buf->data;
    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        sub_addr = BLE_MESH_ADDR_UNASSIGNED;
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        sub_addr = BLE_MESH_ADDR_UNASSIGNED;
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = va_add(label_uuid, &sub_addr);
    if (status != STATUS_SUCCESS) {
        goto send_status;
    }

    if (bt_mesh_model_find_group(mod, sub_addr)) {
        /* Tried to add existing subscription */
        status = STATUS_SUCCESS;
        goto send_status;
    }

    for (i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] == BLE_MESH_ADDR_UNASSIGNED) {
            mod->groups[i] = sub_addr;
            break;
        }
    }

    if (i == ARRAY_SIZE(mod->groups)) {
        status = STATUS_INSUFF_RESOURCES;
    } else {
        if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
            bt_mesh_lpn_group_add(sub_addr);
        }

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_mod_sub(mod);
        }

        status = STATUS_SUCCESS;
    }

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr, sub_addr,
                        mod_id, vnd);
}

static void mod_sub_va_del(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    u16_t elem_addr, sub_addr;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *label_uuid;
    u8_t *mod_id;
    u16_t *match;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    label_uuid = net_buf_simple_pull_mem(buf, 16);

    BT_DBG("elem_addr 0x%04x", elem_addr);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        sub_addr = BLE_MESH_ADDR_UNASSIGNED;
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        sub_addr = BLE_MESH_ADDR_UNASSIGNED;
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = va_del(label_uuid, &sub_addr);
    if (sub_addr == BLE_MESH_ADDR_UNASSIGNED) {
        goto send_status;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(&sub_addr, 1);
    }

    match = bt_mesh_model_find_group(mod, sub_addr);
    if (match) {
        *match = BLE_MESH_ADDR_UNASSIGNED;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_mod_sub(mod);
        }

        status = STATUS_SUCCESS;
    } else {
        status = STATUS_CANNOT_REMOVE;
    }

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr, sub_addr,
                        mod_id, vnd);
}

static void mod_sub_va_overwrite(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    u16_t elem_addr, sub_addr = BLE_MESH_ADDR_UNASSIGNED;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *label_uuid;
    u8_t *mod_id;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    label_uuid = net_buf_simple_pull_mem(buf, 16);

    BT_DBG("elem_addr 0x%04x", elem_addr);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(mod->groups, ARRAY_SIZE(mod->groups));
    }

    mod_sub_list_clear(mod);

    if (ARRAY_SIZE(mod->groups) > 0) {
        status = va_add(label_uuid, &sub_addr);
        if (status == STATUS_SUCCESS) {
            mod->groups[0] = sub_addr;

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_mod_sub(mod);
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
                bt_mesh_lpn_group_add(sub_addr);
            }
        }
    } else {
        status = STATUS_INSUFF_RESOURCES;
    }

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr, sub_addr,
                        mod_id, vnd);
}
#else
static void mod_sub_va_add(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u16_t elem_addr;
    u8_t *mod_id;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    net_buf_simple_pull(buf, 16);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = STATUS_INSUFF_RESOURCES;

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr,
                        BLE_MESH_ADDR_UNASSIGNED, mod_id, vnd);
}

static void mod_sub_va_del(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_elem *elem;
    u16_t elem_addr;
    u8_t *mod_id;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    net_buf_simple_pull(buf, 16);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    if (!get_model(elem, buf, &vnd)) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = STATUS_INSUFF_RESOURCES;

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr,
                        BLE_MESH_ADDR_UNASSIGNED, mod_id, vnd);
}

static void mod_sub_va_overwrite(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    struct bt_mesh_elem *elem;
    u16_t elem_addr;
    u8_t *mod_id;
    u8_t status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    net_buf_simple_pull(buf, 18);

    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    if (!get_model(elem, buf, &vnd)) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = STATUS_INSUFF_RESOURCES;

send_status:
    send_mod_sub_status(model, ctx, status, elem_addr,
                        BLE_MESH_ADDR_UNASSIGNED, mod_id, vnd);
}
#endif /* CONFIG_BLE_MESH_LABEL_COUNT > 0 */

static void send_net_key_status(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                u16_t idx, u8_t status)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 3 + 4);

    bt_mesh_model_msg_init(&msg, OP_NET_KEY_STATUS);

    net_buf_simple_add_u8(&msg, status);
    net_buf_simple_add_le16(&msg, idx);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config NetKey Status", __func__);
    }
}

static void net_key_add(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    struct bt_mesh_subnet *sub;
    u16_t idx;
    int err;

    idx = net_buf_simple_pull_le16(buf);
    if (idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, idx);
        return;
    }

    BT_DBG("idx 0x%04x", idx);

    sub = bt_mesh_subnet_get(idx);
    if (!sub) {
        int i;

        for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
            if (bt_mesh.sub[i].net_idx == BLE_MESH_KEY_UNUSED) {
                sub = &bt_mesh.sub[i];
                break;
            }
        }

        if (!sub) {
            send_net_key_status(model, ctx, idx,
                                STATUS_INSUFF_RESOURCES);
            return;
        }
    }

    /* Check for already existing subnet */
    if (sub->net_idx == idx) {
        u8_t status;

        if (memcmp(buf->data, sub->keys[0].net, 16)) {
            status = STATUS_IDX_ALREADY_STORED;
        } else {
            status = STATUS_SUCCESS;
        }

        send_net_key_status(model, ctx, idx, status);
        return;
    }

    err = bt_mesh_net_keys_create(&sub->keys[0], buf->data);
    if (err) {
        send_net_key_status(model, ctx, idx, STATUS_UNSPECIFIED);
        return;
    }

    sub->net_idx = idx;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        BT_DBG("Storing NetKey persistently");
        bt_mesh_store_subnet(sub);
    }

    /* Make sure we have valid beacon data to be sent */
    bt_mesh_net_beacon_update(sub);

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY)) {
        sub->node_id = BLE_MESH_NODE_IDENTITY_STOPPED;
#if CONFIG_BLE_MESH_NODE
        bt_mesh_proxy_beacon_send(sub);
        bt_mesh_adv_update();
#endif
    } else {
        sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;
    }

    send_net_key_status(model, ctx, idx, STATUS_SUCCESS);
}

static void net_key_update(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_subnet *sub;
    u16_t idx;
    int err;

    idx = net_buf_simple_pull_le16(buf);
    if (idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, idx);
        return;
    }

    BT_DBG("idx 0x%04x", idx);

    sub = bt_mesh_subnet_get(idx);
    if (!sub) {
        send_net_key_status(model, ctx, idx, STATUS_INVALID_NETKEY);
        return;
    }

    /* The node shall successfully process a NetKey Update message on a
     * valid NetKeyIndex when the NetKey value is different and the Key
     * Refresh procedure has not been started, or when the NetKey value is
     * the same in Phase 1. The NetKey Update message shall generate an
     * error when the node is in Phase 2, or Phase 3.
     */
    switch (sub->kr_phase) {
    case BLE_MESH_KR_NORMAL:
        if (!memcmp(buf->data, sub->keys[0].net, 16)) {
            return;
        }
        break;
    case BLE_MESH_KR_PHASE_1:
        if (!memcmp(buf->data, sub->keys[1].net, 16)) {
            send_net_key_status(model, ctx, idx, STATUS_SUCCESS);
            return;
        }
    /* fall through */
    case BLE_MESH_KR_PHASE_2:
    case BLE_MESH_KR_PHASE_3:
        send_net_key_status(model, ctx, idx, STATUS_CANNOT_UPDATE);
        return;
    }

    err = bt_mesh_net_keys_create(&sub->keys[1], buf->data);
    if (!err && (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) ||
                 IS_ENABLED(CONFIG_BLE_MESH_FRIEND))) {
        err = friend_cred_update(sub);
    }

    if (err) {
        send_net_key_status(model, ctx, idx, STATUS_UNSPECIFIED);
        return;
    }

    sub->kr_phase = BLE_MESH_KR_PHASE_1;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        BT_DBG("Storing NetKey persistently");
        bt_mesh_store_subnet(sub);
    }

    bt_mesh_net_beacon_update(sub);

    send_net_key_status(model, ctx, idx, STATUS_SUCCESS);
}

static void hb_pub_disable(struct bt_mesh_cfg_srv *cfg)
{
    BT_DBG("%s", __func__);

    cfg->hb_pub.dst = BLE_MESH_ADDR_UNASSIGNED;
    cfg->hb_pub.count = 0U;
    cfg->hb_pub.ttl = 0U;
    cfg->hb_pub.period = 0U;

    k_delayed_work_cancel(&cfg->hb_pub.timer);
}

static void net_key_del(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    struct bt_mesh_subnet *sub;
    u16_t del_idx;
    u8_t status;

    del_idx = net_buf_simple_pull_le16(buf);
    if (del_idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, del_idx);
        return;
    }

    BT_DBG("idx 0x%04x", del_idx);

    sub = bt_mesh_subnet_get(del_idx);
    if (!sub) {
        /* This could be a retry of a previous attempt that had its
         * response lost, so pretend that it was a success.
         */
        status = STATUS_SUCCESS;
        goto send_status;
    }

    /* The key that the message was encrypted with cannot be removed.
     * The NetKey List must contain a minimum of one NetKey.
     */
    if (ctx->net_idx == del_idx) {
        status = STATUS_CANNOT_REMOVE;
        goto send_status;
    }

    bt_mesh_subnet_del(sub, true);
    status = STATUS_SUCCESS;

send_status:
    send_net_key_status(model, ctx, del_idx, status);
}

static void net_key_get(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg,
                          2 + 4 + IDX_LEN(CONFIG_BLE_MESH_SUBNET_COUNT));
    u16_t prev, i;

    bt_mesh_model_msg_init(&msg, OP_NET_KEY_LIST);

    prev = BLE_MESH_KEY_UNUSED;
    for (i = 0U; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        if (prev == BLE_MESH_KEY_UNUSED) {
            prev = sub->net_idx;
            continue;
        }

        key_idx_pack(&msg, prev, sub->net_idx);
        prev = BLE_MESH_KEY_UNUSED;
    }

    if (prev != BLE_MESH_KEY_UNUSED) {
        net_buf_simple_add_le16(&msg, prev);
    }

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config NetKey List", __func__);
    }
}

static void node_identity_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 4 + 4);
    struct bt_mesh_subnet *sub;
    u8_t node_id;
    u16_t idx;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    idx = net_buf_simple_pull_le16(buf);
    if (idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, idx);
        return;
    }

    bt_mesh_model_msg_init(&msg, OP_NODE_IDENTITY_STATUS);

    sub = bt_mesh_subnet_get(idx);
    if (!sub) {
        net_buf_simple_add_u8(&msg, STATUS_INVALID_NETKEY);
        node_id = 0x00;
    } else {
        net_buf_simple_add_u8(&msg, STATUS_SUCCESS);
        node_id = sub->node_id;
    }

    net_buf_simple_add_le16(&msg, idx);
    net_buf_simple_add_u8(&msg, node_id);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Node Identity Status", __func__);
    }
}

static void node_identity_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 4 + 4);
    struct bt_mesh_subnet *sub;
    u8_t node_id;
    u16_t idx;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    idx = net_buf_simple_pull_le16(buf);
    if (idx > 0xfff) {
        BT_WARN("%s, Invalid NetKeyIndex 0x%04x", __func__, idx);
        return;
    }

    node_id = net_buf_simple_pull_u8(buf);
    if (node_id != 0x00 && node_id != 0x01) {
        BT_WARN("%s, Invalid Node ID value 0x%02x", __func__, node_id);
        return;
    }

    bt_mesh_model_msg_init(&msg, OP_NODE_IDENTITY_STATUS);

    sub = bt_mesh_subnet_get(idx);
    if (!sub) {
        net_buf_simple_add_u8(&msg, STATUS_INVALID_NETKEY);
        net_buf_simple_add_le16(&msg, idx);
        net_buf_simple_add_u8(&msg, node_id);
    } else  {
        net_buf_simple_add_u8(&msg, STATUS_SUCCESS);
        net_buf_simple_add_le16(&msg, idx);
#if CONFIG_BLE_MESH_NODE
        /* Section 4.2.11.1: "When the GATT Proxy state is set to
         * 0x00, the Node Identity state for all subnets shall be set
         * to 0x00 and shall not be changed."
         */
        if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY) &&
                bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED) {
            if (node_id) {
                bt_mesh_proxy_identity_start(sub);
            } else {
                bt_mesh_proxy_identity_stop(sub);
            }
            bt_mesh_adv_update();
        }
#endif
        net_buf_simple_add_u8(&msg, sub->node_id);
    }

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Node Identity Status", __func__);
    }
}

static void create_mod_app_status(struct net_buf_simple *msg,
                                  struct bt_mesh_model *mod, bool vnd,
                                  u16_t elem_addr, u16_t app_idx,
                                  u8_t status, u8_t *mod_id)
{
    bt_mesh_model_msg_init(msg, OP_MOD_APP_STATUS);

    net_buf_simple_add_u8(msg, status);
    net_buf_simple_add_le16(msg, elem_addr);
    net_buf_simple_add_le16(msg, app_idx);

    if (vnd) {
        memcpy(net_buf_simple_add(msg, 4), mod_id, 4);
    } else {
        memcpy(net_buf_simple_add(msg, 2), mod_id, 2);
    }
}

static void mod_app_bind(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 9 + 4);
    u16_t elem_addr, key_app_idx;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id, status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    key_app_idx = net_buf_simple_pull_le16(buf);
    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    /* Configuration Server only allows device key based access */
    if (model == mod) {
        BT_ERR("%s, Client tried to bind AppKey to Configuration Model", __func__);
        status = STATUS_CANNOT_BIND;
        goto send_status;
    }

    status = mod_bind(mod, key_app_idx);

send_status:
    BT_DBG("status 0x%02x", status);
    create_mod_app_status(&msg, mod, vnd, elem_addr, key_app_idx, status,
                          mod_id);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Model App Bind Status", __func__);
    }
}

static void mod_app_unbind(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 9 + 4);
    u16_t elem_addr, key_app_idx;
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id, status;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    key_app_idx = net_buf_simple_pull_le16(buf);
    mod_id = buf->data;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_status;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_status;
    }

    status = mod_unbind(mod, key_app_idx, true);

send_status:
    BT_DBG("status 0x%02x", status);
    create_mod_app_status(&msg, mod, vnd, elem_addr, key_app_idx, status,
                          mod_id);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Model App Unbind Status", __func__);
    }
}

#define KEY_LIST_LEN (CONFIG_BLE_MESH_MODEL_KEY_COUNT * 2)

static void mod_app_get(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 9 + KEY_LIST_LEN + 4);
    struct bt_mesh_model *mod;
    struct bt_mesh_elem *elem;
    u8_t *mod_id, status;
    u16_t elem_addr;
    bool vnd;

    elem_addr = net_buf_simple_pull_le16(buf);
    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Prohibited element address 0x%04x", __func__, elem_addr);
        return;
    }

    mod_id = buf->data;

    BT_DBG("elem_addr 0x%04x", elem_addr);

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        mod = NULL;
        vnd = (buf->len == 4U);
        status = STATUS_INVALID_ADDRESS;
        goto send_list;
    }

    mod = get_model(elem, buf, &vnd);
    if (!mod) {
        status = STATUS_INVALID_MODEL;
        goto send_list;
    }

    status = STATUS_SUCCESS;

send_list:
    if (vnd) {
        bt_mesh_model_msg_init(&msg, OP_VND_MOD_APP_LIST);
    } else {
        bt_mesh_model_msg_init(&msg, OP_SIG_MOD_APP_LIST);
    }

    net_buf_simple_add_u8(&msg, status);
    net_buf_simple_add_le16(&msg, elem_addr);

    if (vnd) {
        net_buf_simple_add_mem(&msg, mod_id, 4);
    } else {
        net_buf_simple_add_mem(&msg, mod_id, 2);
    }

    if (mod) {
        int i;

        for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
            if (mod->keys[i] != BLE_MESH_KEY_UNUSED) {
                net_buf_simple_add_le16(&msg, mod->keys[i]);
            }
        }
    }

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Model Application List", __func__);
    }
}

static void node_reset(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 0 + 4);

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));


    bt_mesh_model_msg_init(&msg, OP_NODE_RESET_STATUS);

    /* Send the response first since we wont have any keys left to
     * send it later.
     */
    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Node Reset Status", __func__);
    }

#if CONFIG_BLE_MESH_NODE
    bt_mesh_reset();
#endif
}

static void send_friend_status(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    bt_mesh_model_msg_init(&msg, OP_FRIEND_STATUS);
    net_buf_simple_add_u8(&msg, cfg->frnd);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Friend Status", __func__);
    }
}

static void friend_get(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf)
{
    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    send_friend_status(model, ctx);
}

static void friend_set(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
           ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
           bt_hex(buf->data, buf->len));

    if (buf->data[0] != 0x00 && buf->data[0] != 0x01) {
        BT_WARN("Invalid Friend value 0x%02x", buf->data[0]);
        return;
    }

    if (!cfg) {
        BT_WARN("No Configuration Server context available");
        goto send_status;
    }

    BT_DBG("Friend 0x%02x -> 0x%02x", cfg->frnd, buf->data[0]);

    if (cfg->frnd == buf->data[0]) {
        goto send_status;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        cfg->frnd = buf->data[0];

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_cfg();
        }

        if (cfg->frnd == BLE_MESH_FRIEND_DISABLED) {
            bt_mesh_friend_clear_net_idx(BLE_MESH_KEY_ANY);
        }
    }

    if (cfg->hb_pub.feat & BLE_MESH_FEAT_FRIEND) {
        bt_mesh_heartbeat_send();
    }

send_status:
    send_friend_status(model, ctx);
}

static void lpn_timeout_get(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 5 + 4);
    struct bt_mesh_friend *frnd;
    u16_t lpn_addr;
    s32_t timeout;

    lpn_addr = net_buf_simple_pull_le16(buf);

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x lpn_addr 0x%02x",
           ctx->net_idx, ctx->app_idx, ctx->addr, lpn_addr);

    if (!BLE_MESH_ADDR_IS_UNICAST(lpn_addr)) {
        BT_WARN("Invalid LPNAddress; ignoring msg");
        return;
    }

    bt_mesh_model_msg_init(&msg, OP_LPN_TIMEOUT_STATUS);
    net_buf_simple_add_le16(&msg, lpn_addr);

    if (!IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        timeout = 0;
        goto send_rsp;
    }

    frnd = bt_mesh_friend_find(BLE_MESH_KEY_ANY, lpn_addr, true, true);
    if (!frnd) {
        timeout = 0;
        goto send_rsp;
    }

    timeout = k_delayed_work_remaining_get(&frnd->timer) / 100;

send_rsp:
    net_buf_simple_add_u8(&msg, timeout);
    net_buf_simple_add_u8(&msg, timeout >> 8);
    net_buf_simple_add_u8(&msg, timeout >> 16);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config LPN PollTimeout Status", __func__);
    }
}

static void send_krp_status(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            u16_t idx, u8_t phase, u8_t status)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 4 + 4);

    bt_mesh_model_msg_init(&msg, OP_KRP_STATUS);

    net_buf_simple_add_u8(&msg, status);
    net_buf_simple_add_le16(&msg, idx);
    net_buf_simple_add_u8(&msg, phase);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Key Refresh Phase Status", __func__);
    }
}

static void krp_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                    struct net_buf_simple *buf)
{
    struct bt_mesh_subnet *sub;
    u16_t idx;

    idx = net_buf_simple_pull_le16(buf);
    if (idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, idx);
        return;
    }

    BT_DBG("idx 0x%04x", idx);

    sub = bt_mesh_subnet_get(idx);
    if (!sub) {
        send_krp_status(model, ctx, idx, 0x00, STATUS_INVALID_NETKEY);
    } else {
        send_krp_status(model, ctx, idx, sub->kr_phase,
                        STATUS_SUCCESS);
    }
}

static void krp_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                    struct net_buf_simple *buf)
{
    struct bt_mesh_subnet *sub;
    u8_t phase;
    u16_t idx;

    idx = net_buf_simple_pull_le16(buf);
    phase = net_buf_simple_pull_u8(buf);

    if (idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, idx);
        return;
    }

    BT_DBG("idx 0x%04x transition 0x%02x", idx, phase);

    sub = bt_mesh_subnet_get(idx);
    if (!sub) {
        send_krp_status(model, ctx, idx, 0x00, STATUS_INVALID_NETKEY);
        return;
    }

    BT_DBG("%u -> %u", sub->kr_phase, phase);

    if (phase < BLE_MESH_KR_PHASE_2 || phase > BLE_MESH_KR_PHASE_3 ||
            (sub->kr_phase == BLE_MESH_KR_NORMAL &&
             phase == BLE_MESH_KR_PHASE_2)) {
        BT_WARN("%s, Prohibited transition %u -> %u", __func__, sub->kr_phase, phase);
        return;
    }

    if (sub->kr_phase == BLE_MESH_KR_PHASE_1 &&
            phase == BLE_MESH_KR_PHASE_2) {
        sub->kr_phase = BLE_MESH_KR_PHASE_2;
        sub->kr_flag = 1;
        bt_mesh_net_beacon_update(sub);
    } else if ((sub->kr_phase == BLE_MESH_KR_PHASE_1 ||
                sub->kr_phase == BLE_MESH_KR_PHASE_2) &&
               phase == BLE_MESH_KR_PHASE_3) {
        bt_mesh_net_revoke_keys(sub);
        if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) ||
                IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
            friend_cred_refresh(ctx->net_idx);
        }
        sub->kr_phase = BLE_MESH_KR_NORMAL;
        sub->kr_flag = 0;
        bt_mesh_net_beacon_update(sub);
    }

    send_krp_status(model, ctx, idx, sub->kr_phase, STATUS_SUCCESS);
}

static u8_t hb_log(u16_t val)
{
    if (!val) {
        return 0x00;
    } else if (val == 0xffff) {
        return 0xff;
    } else {
        return 32 - __builtin_clz(val);
    }
}

static u8_t hb_pub_count_log(u16_t val)
{
    if (!val) {
        return 0x00;
    } else if (val == 0x01) {
        return 0x01;
    } else if (val == 0xffff) {
        return 0xff;
    } else {
        return 32 - __builtin_clz(val - 1) + 1;
    }
}

static u16_t hb_pwr2(u8_t val, u8_t sub)
{
    if (!val) {
        return 0x0000;
    } else if (val == 0xff || val == 0x11) {
        return 0xffff;
    } else {
        return (1 << (val - sub));
    }
}

struct hb_pub_param {
    u16_t dst;
    u8_t  count_log;
    u8_t  period_log;
    u8_t  ttl;
    u16_t feat;
    u16_t net_idx;
} __packed;

static void hb_pub_send_status(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx, u8_t status,
                               struct hb_pub_param *orig_msg)
{
    /* Needed size: opcode (1 byte) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 1 + 10 + 4);
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    BT_DBG("src 0x%04x status 0x%02x", ctx->addr, status);

    bt_mesh_model_msg_init(&msg, OP_HEARTBEAT_PUB_STATUS);

    net_buf_simple_add_u8(&msg, status);

    if (orig_msg) {
        memcpy(net_buf_simple_add(&msg, sizeof(*orig_msg)), orig_msg,
               sizeof(*orig_msg));
        goto send;
    }

    net_buf_simple_add_le16(&msg, cfg->hb_pub.dst);
    net_buf_simple_add_u8(&msg, hb_pub_count_log(cfg->hb_pub.count));
    net_buf_simple_add_u8(&msg, cfg->hb_pub.period);
    net_buf_simple_add_u8(&msg, cfg->hb_pub.ttl);
    net_buf_simple_add_le16(&msg, cfg->hb_pub.feat);
    net_buf_simple_add_le16(&msg, cfg->hb_pub.net_idx);

send:
    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Heartbeat Publication Status", __func__);
    }
}

static void heartbeat_pub_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    BT_DBG("src 0x%04x", ctx->addr);

    hb_pub_send_status(model, ctx, STATUS_SUCCESS, NULL);
}

static void heartbeat_pub_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct hb_pub_param *param = (void *)buf->data;
    struct bt_mesh_cfg_srv *cfg = model->user_data;
    u16_t dst, feat, idx;
    u8_t status;

    BT_DBG("src 0x%04x", ctx->addr);

    dst = sys_le16_to_cpu(param->dst);
    /* All other address types but virtual are valid */
    if (BLE_MESH_ADDR_IS_VIRTUAL(dst)) {
        status = STATUS_INVALID_ADDRESS;
        goto failed;
    }

    if (param->count_log > 0x11 && param->count_log != 0xff) {
        status = STATUS_CANNOT_SET;
        goto failed;
    }

    if (param->period_log > 0x10) {
        status = STATUS_CANNOT_SET;
        goto failed;
    }

    if (param->ttl > BLE_MESH_TTL_MAX && param->ttl != BLE_MESH_TTL_DEFAULT) {
        BT_ERR("%s, Invalid TTL value 0x%02x", __func__, param->ttl);
        return;
    }

    feat = sys_le16_to_cpu(param->feat);

    idx = sys_le16_to_cpu(param->net_idx);
    if (idx > 0xfff) {
        BT_ERR("%s, Invalid NetKeyIndex 0x%04x", __func__, idx);
        return;
    }

    if (!bt_mesh_subnet_get(idx)) {
        status = STATUS_INVALID_NETKEY;
        goto failed;
    }

    cfg->hb_pub.dst = dst;
    cfg->hb_pub.period = param->period_log;
    cfg->hb_pub.feat = feat & BLE_MESH_FEAT_SUPPORTED;
    cfg->hb_pub.net_idx = idx;

    if (dst == BLE_MESH_ADDR_UNASSIGNED) {
        hb_pub_disable(cfg);
    } else {
        /* 2^(n-1) */
        cfg->hb_pub.count = hb_pwr2(param->count_log, 1);
        cfg->hb_pub.ttl = param->ttl;

        BT_DBG("period %u ms", hb_pwr2(param->period_log, 1) * 1000U);

        /* Note: Send heartbeat message here will cause wrong heartbeat status message */
#if 0
        /* The first Heartbeat message shall be published as soon
         * as possible after the Heartbeat Publication Period state
         * has been configured for periodic publishing.
         */
        if (param->period_log && param->count_log) {
            k_work_submit(&cfg->hb_pub.timer.work);
        } else {
            k_delayed_work_cancel(&cfg->hb_pub.timer);
        }
#endif
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_hb_pub();
    }

    hb_pub_send_status(model, ctx, STATUS_SUCCESS, NULL);

    /* The first Heartbeat message shall be published as soon
     * as possible after the Heartbeat Publication Period state
     * has been configured for periodic publishing.
     */
    if (dst != BLE_MESH_ADDR_UNASSIGNED) {
        if (param->period_log && param->count_log) {
            k_work_submit(&cfg->hb_pub.timer.work);
        } else {
            k_delayed_work_cancel(&cfg->hb_pub.timer);
        }
    }

    return;

failed:
    hb_pub_send_status(model, ctx, status, param);
}

static void hb_sub_send_status(struct bt_mesh_model *model,
                               struct bt_mesh_msg_ctx *ctx, u8_t status)
{
    /* Needed size: opcode (2 bytes) + msg + MIC */
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 9 + 4);
    struct bt_mesh_cfg_srv *cfg = model->user_data;
    u16_t period;
    s64_t uptime;

    BT_DBG("src 0x%04x status 0x%02x", ctx->addr, status);

    uptime = k_uptime_get();
    if (uptime > cfg->hb_sub.expiry) {
        period = 0U;
    } else {
        period = (cfg->hb_sub.expiry - uptime) / 1000;
    }

    bt_mesh_model_msg_init(&msg, OP_HEARTBEAT_SUB_STATUS);

    net_buf_simple_add_u8(&msg, status);
    net_buf_simple_add_le16(&msg, cfg->hb_sub.src);
    net_buf_simple_add_le16(&msg, cfg->hb_sub.dst);
    net_buf_simple_add_u8(&msg, hb_log(period));
    net_buf_simple_add_u8(&msg, hb_log(cfg->hb_sub.count));
    net_buf_simple_add_u8(&msg, cfg->hb_sub.min_hops);
    net_buf_simple_add_u8(&msg, cfg->hb_sub.max_hops);

    if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
        BT_ERR("%s, Unable to send Config Heartbeat Subscription Status", __func__);
    }
}

static void heartbeat_sub_get(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    BT_DBG("src 0x%04x", ctx->addr);

    hb_sub_send_status(model, ctx, STATUS_SUCCESS);
}

static void heartbeat_sub_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_cfg_srv *cfg = model->user_data;
    u16_t sub_src, sub_dst;
    u8_t sub_period;
    s32_t period_ms;

    BT_DBG("src 0x%04x", ctx->addr);

    sub_src = net_buf_simple_pull_le16(buf);
    sub_dst = net_buf_simple_pull_le16(buf);
    sub_period = net_buf_simple_pull_u8(buf);

    BT_DBG("sub_src 0x%04x sub_dst 0x%04x period 0x%02x",
           sub_src, sub_dst, sub_period);

    if (sub_src != BLE_MESH_ADDR_UNASSIGNED &&
            !BLE_MESH_ADDR_IS_UNICAST(sub_src)) {
        BT_WARN("Prohibited source address");
        return;
    }

    if (BLE_MESH_ADDR_IS_VIRTUAL(sub_dst) || BLE_MESH_ADDR_IS_RFU(sub_dst) ||
            (BLE_MESH_ADDR_IS_UNICAST(sub_dst) &&
             sub_dst != bt_mesh_primary_addr())) {
        BT_WARN("Prohibited destination address");
        return;
    }

    if (sub_period > 0x11) {
        BT_WARN("Prohibited subscription period 0x%02x", sub_period);
        return;
    }

    if (sub_src == BLE_MESH_ADDR_UNASSIGNED ||
            sub_dst == BLE_MESH_ADDR_UNASSIGNED ||
            sub_period == 0x00) {
        /* Only an explicit address change to unassigned should
         * trigger clearing of the values according to
         * MESH/NODE/CFG/HBS/BV-02-C.
         */
        if (sub_src == BLE_MESH_ADDR_UNASSIGNED ||
            sub_dst == BLE_MESH_ADDR_UNASSIGNED) {
            cfg->hb_sub.src = BLE_MESH_ADDR_UNASSIGNED;
            cfg->hb_sub.dst = BLE_MESH_ADDR_UNASSIGNED;
            cfg->hb_sub.min_hops = BLE_MESH_TTL_MAX;
            cfg->hb_sub.max_hops = 0U;
            cfg->hb_sub.count = 0U;
        }

        period_ms = 0;
    } else {
        cfg->hb_sub.src = sub_src;
        cfg->hb_sub.dst = sub_dst;
        cfg->hb_sub.min_hops = BLE_MESH_TTL_MAX;
        cfg->hb_sub.max_hops = 0U;
        cfg->hb_sub.count = 0U;
        period_ms = hb_pwr2(sub_period, 1) * 1000U;
    }

    /* Let the transport layer know it needs to handle this address */
    bt_mesh_set_hb_sub_dst(cfg->hb_sub.dst);

    BT_DBG("period_ms %u", period_ms);

    if (period_ms) {
        cfg->hb_sub.expiry = k_uptime_get() + period_ms;
    } else {
        cfg->hb_sub.expiry = 0;
    }

    hb_sub_send_status(model, ctx, STATUS_SUCCESS);

    /* MESH/NODE/CFG/HBS/BV-01-C expects the MinHops to be 0x7f after
     * disabling subscription, but 0x00 for subsequent Get requests.
     */
    if (!period_ms) {
        cfg->hb_sub.min_hops = 0U;
    }
}

const struct bt_mesh_model_op bt_mesh_cfg_srv_op[] = {
    { OP_DEV_COMP_DATA_GET,        1,   dev_comp_data_get },
    { OP_APP_KEY_ADD,              19,  app_key_add },
    { OP_APP_KEY_UPDATE,           19,  app_key_update },
    { OP_APP_KEY_DEL,              3,   app_key_del },
    { OP_APP_KEY_GET,              2,   app_key_get },
    { OP_BEACON_GET,               0,   beacon_get },
    { OP_BEACON_SET,               1,   beacon_set },
    { OP_DEFAULT_TTL_GET,          0,   default_ttl_get },
    { OP_DEFAULT_TTL_SET,          1,   default_ttl_set },
    { OP_GATT_PROXY_GET,           0,   gatt_proxy_get },
    { OP_GATT_PROXY_SET,           1,   gatt_proxy_set },
    { OP_NET_TRANSMIT_GET,         0,   net_transmit_get },
    { OP_NET_TRANSMIT_SET,         1,   net_transmit_set },
    { OP_RELAY_GET,                0,   relay_get },
    { OP_RELAY_SET,                2,   relay_set },
    { OP_MOD_PUB_GET,              4,   mod_pub_get },
    { OP_MOD_PUB_SET,              11,  mod_pub_set },
    { OP_MOD_PUB_VA_SET,           24,  mod_pub_va_set },
    { OP_MOD_SUB_ADD,              6,   mod_sub_add },
    { OP_MOD_SUB_VA_ADD,           20,  mod_sub_va_add },
    { OP_MOD_SUB_DEL,              6,   mod_sub_del },
    { OP_MOD_SUB_VA_DEL,           20,  mod_sub_va_del },
    { OP_MOD_SUB_OVERWRITE,        6,   mod_sub_overwrite },
    { OP_MOD_SUB_VA_OVERWRITE,     20,  mod_sub_va_overwrite },
    { OP_MOD_SUB_DEL_ALL,          4,   mod_sub_del_all },
    { OP_MOD_SUB_GET,              4,   mod_sub_get },
    { OP_MOD_SUB_GET_VND,          6,   mod_sub_get_vnd },
    { OP_NET_KEY_ADD,              18,  net_key_add },
    { OP_NET_KEY_UPDATE,           18,  net_key_update },
    { OP_NET_KEY_DEL,              2,   net_key_del },
    { OP_NET_KEY_GET,              0,   net_key_get },
    { OP_NODE_IDENTITY_GET,        2,   node_identity_get },
    { OP_NODE_IDENTITY_SET,        3,   node_identity_set },
    { OP_MOD_APP_BIND,             6,   mod_app_bind },
    { OP_MOD_APP_UNBIND,           6,   mod_app_unbind },
    { OP_SIG_MOD_APP_GET,          4,   mod_app_get },
    { OP_VND_MOD_APP_GET,          6,   mod_app_get },
    { OP_NODE_RESET,               0,   node_reset },
    { OP_FRIEND_GET,               0,   friend_get },
    { OP_FRIEND_SET,               1,   friend_set },
    { OP_LPN_TIMEOUT_GET,          2,   lpn_timeout_get },
    { OP_KRP_GET,                  2,   krp_get },
    { OP_KRP_SET,                  3,   krp_set },
    { OP_HEARTBEAT_PUB_GET,        0,   heartbeat_pub_get },
    { OP_HEARTBEAT_PUB_SET,        9,   heartbeat_pub_set },
    { OP_HEARTBEAT_SUB_GET,        0,   heartbeat_sub_get },
    { OP_HEARTBEAT_SUB_SET,        5,   heartbeat_sub_set },
    BLE_MESH_MODEL_OP_END,
};

static void hb_publish(struct k_work *work)
{
    struct bt_mesh_cfg_srv *cfg = CONTAINER_OF(work,
                                  struct bt_mesh_cfg_srv,
                                  hb_pub.timer.work);
    struct bt_mesh_subnet *sub;
    u16_t period_ms;

    BT_DBG("hb_pub.count: %u", cfg->hb_pub.count);

    sub = bt_mesh_subnet_get(cfg->hb_pub.net_idx);
    if (!sub) {
        BT_ERR("%s, No matching subnet for idx 0x%02x",
               __func__, cfg->hb_pub.net_idx);
        cfg->hb_pub.dst = BLE_MESH_ADDR_UNASSIGNED;
        return;
    }

    if (cfg->hb_pub.count == 0U) {
        return;
    }

    period_ms = hb_pwr2(cfg->hb_pub.period, 1) * 1000U;
    if (period_ms && cfg->hb_pub.count > 1) {
        k_delayed_work_submit(&cfg->hb_pub.timer, period_ms);
    }

    bt_mesh_heartbeat_send();

    if (cfg->hb_pub.count != 0xffff) {
        cfg->hb_pub.count--;
    }
}

static bool conf_is_valid(struct bt_mesh_cfg_srv *cfg)
{
    if (cfg->relay > 0x02) {
        return false;
    }

    if (cfg->beacon > 0x01) {
        return false;
    }

    if (cfg->default_ttl > BLE_MESH_TTL_MAX) {
        return false;
    }

    return true;
}

int bt_mesh_cfg_srv_init(struct bt_mesh_model *model, bool primary)
{
    struct bt_mesh_cfg_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("%s, No Configuration Server context provided", __func__);
        return -EINVAL;
    }

    if (!conf_is_valid(cfg)) {
        BT_ERR("%s, Invalid values in configuration", __func__);
        return -EINVAL;
    }

    /* Configuration Model security is device-key based */
    model->keys[0] = BLE_MESH_KEY_DEV;

    if (!IS_ENABLED(CONFIG_BLE_MESH_RELAY)) {
        cfg->relay = BLE_MESH_RELAY_NOT_SUPPORTED;
    }

    if (!IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        cfg->frnd = BLE_MESH_FRIEND_NOT_SUPPORTED;
    }

    if (!IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY)) {
        cfg->gatt_proxy = BLE_MESH_GATT_PROXY_NOT_SUPPORTED;
    }

    k_delayed_work_init(&cfg->hb_pub.timer, hb_publish);
    cfg->hb_pub.net_idx = BLE_MESH_KEY_UNUSED;
    cfg->hb_sub.expiry = 0;

    cfg->model = model;

    conf = cfg;

    return 0;
}

static void mod_reset(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                      bool vnd, bool primary, void *user_data)
{
    size_t clear_count;

    /* Clear model state that isn't otherwise cleared. E.g. AppKey
     * binding and model publication is cleared as a consequence
     * of removing all app keys, however model subscription clearing
     * must be taken care of here.
     */

    clear_count = mod_sub_list_clear(mod);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && clear_count) {
        bt_mesh_store_mod_sub(mod);
    }
}

void bt_mesh_cfg_reset(void)
{
    struct bt_mesh_cfg_srv *cfg = conf;
    int i;

    BT_DBG("%s", __func__);

    if (!cfg) {
        return;
    }

    bt_mesh_set_hb_sub_dst(BLE_MESH_ADDR_UNASSIGNED);

    cfg->hb_sub.src = BLE_MESH_ADDR_UNASSIGNED;
    cfg->hb_sub.dst = BLE_MESH_ADDR_UNASSIGNED;
    cfg->hb_sub.expiry = 0;

    /* Delete all net keys, which also takes care of all app keys which
     * are associated with each net key.
     */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx != BLE_MESH_KEY_UNUSED) {
            bt_mesh_subnet_del(sub, true);
        }
    }

    bt_mesh_model_foreach(mod_reset, NULL);

    (void)memset(labels, 0, sizeof(labels));
}

void bt_mesh_heartbeat(u16_t src, u16_t dst, u8_t hops, u16_t feat)
{
    struct bt_mesh_cfg_srv *cfg = conf;

    if (!cfg) {
        BT_WARN("No configuaration server context available");
        return;
    }

    if (src != cfg->hb_sub.src || dst != cfg->hb_sub.dst) {
        BT_WARN("No subscription for received heartbeat");
        return;
    }

    if (k_uptime_get() > cfg->hb_sub.expiry) {
        BT_WARN("Heartbeat subscription period expired");
        return;
    }

    cfg->hb_sub.min_hops = MIN(cfg->hb_sub.min_hops, hops);
    cfg->hb_sub.max_hops = MAX(cfg->hb_sub.max_hops, hops);

    if (cfg->hb_sub.count < 0xffff) {
        cfg->hb_sub.count++;
    }

    BT_DBG("src 0x%04x dst 0x%04x hops %u min %u max %u count %u", src,
           dst, hops, cfg->hb_sub.min_hops, cfg->hb_sub.max_hops,
           cfg->hb_sub.count);

    if (cfg->hb_sub.func) {
        cfg->hb_sub.func(hops, feat);
    }
}

u8_t bt_mesh_net_transmit_get(void)
{
    if (conf) {
        return conf->net_transmit;
    }

    return 0;
}

u8_t bt_mesh_relay_get(void)
{
    if (conf) {
        return conf->relay;
    }

    return BLE_MESH_RELAY_NOT_SUPPORTED;
}

u8_t bt_mesh_friend_get(void)
{
    if (conf) {
        BT_DBG("conf %p conf->frnd 0x%02x", conf, conf->frnd);
        return conf->frnd;
    }

    return BLE_MESH_FRIEND_NOT_SUPPORTED;
}

u8_t bt_mesh_relay_retransmit_get(void)
{
    if (conf) {
        return conf->relay_retransmit;
    }

    return 0;
}

u8_t bt_mesh_beacon_get(void)
{
    if (conf) {
        return conf->beacon;
    }

    return BLE_MESH_BEACON_DISABLED;
}

u8_t bt_mesh_gatt_proxy_get(void)
{
    if (conf) {
        return conf->gatt_proxy;
    }

    return BLE_MESH_GATT_PROXY_NOT_SUPPORTED;
}

u8_t bt_mesh_default_ttl_get(void)
{
    if (conf) {
        return conf->default_ttl;
    }

    return DEFAULT_TTL;
}

u8_t *bt_mesh_label_uuid_get(u16_t addr)
{
    int i;

    BT_DBG("addr 0x%04x", addr);

    for (i = 0; i < ARRAY_SIZE(labels); i++) {
        if (labels[i].addr == addr) {
            BT_DBG("Found Label UUID for 0x%04x: %s", addr,
                   bt_hex(labels[i].uuid, 16));
            return labels[i].uuid;
        }
    }

    BT_WARN("No matching Label UUID for 0x%04x", addr);

    return NULL;
}

struct bt_mesh_hb_pub *bt_mesh_hb_pub_get(void)
{
    if (!conf) {
        return NULL;
    }

    return &conf->hb_pub;
}

void bt_mesh_hb_pub_disable(void)
{
    if (conf) {
        hb_pub_disable(conf);
    }
}

struct bt_mesh_cfg_srv *bt_mesh_cfg_get(void)
{
    return conf;
}

void bt_mesh_subnet_del(struct bt_mesh_subnet *sub, bool store)
{
    int i;

    BT_DBG("NetIdx 0x%03x store %u", sub->net_idx, store);

    if (conf && conf->hb_pub.net_idx == sub->net_idx) {
        hb_pub_disable(conf);

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
            bt_mesh_store_hb_pub();
        }
    }

    /* Delete any app keys bound to this NetKey index */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

        if (key->net_idx == sub->net_idx) {
            bt_mesh_app_key_del(key, store);
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_clear_net_idx(sub->net_idx);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
        bt_mesh_clear_subnet(sub);
    }

    (void)memset(sub, 0, sizeof(*sub));
    sub->net_idx = BLE_MESH_KEY_UNUSED;
}
