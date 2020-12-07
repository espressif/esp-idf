/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "mesh.h"
#include "adv.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "mesh_main.h"
#include "mesh_common.h"
#include "fast_prov.h"
#include "provisioner_main.h"

#define BLE_MESH_SDU_MAX_LEN    384

static const struct bt_mesh_comp *dev_comp;
static uint16_t dev_primary_addr;

void bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod,
                                        struct bt_mesh_elem *elem,
                                        bool vnd, bool primary,
                                        void *user_data),
                           void *user_data)
{
    int i, j;

    if (dev_comp == NULL) {
        BT_ERR("Invalid device composition");
        return;
    }

    for (i = 0; i < dev_comp->elem_count; i++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[i];

        for (j = 0; j < elem->model_count; j++) {
            struct bt_mesh_model *model = &elem->models[j];

            func(model, elem, false, i == 0, user_data);
        }

        for (j = 0; j < elem->vnd_model_count; j++) {
            struct bt_mesh_model *model = &elem->vnd_models[j];

            func(model, elem, true, i == 0, user_data);
        }
    }
}

int32_t bt_mesh_model_pub_period_get(struct bt_mesh_model *mod)
{
    int period = 0;

    if (!mod->pub) {
        BT_ERR("Model has no publication support");
        return 0;
    }

    switch (mod->pub->period >> 6) {
    case 0x00:
        /* 1 step is 100 ms */
        period = K_MSEC((mod->pub->period & BIT_MASK(6)) * 100U);
        break;
    case 0x01:
        /* 1 step is 1 second */
        period = K_SECONDS(mod->pub->period & BIT_MASK(6));
        break;
    case 0x02:
        /* 1 step is 10 seconds */
        period = K_SECONDS((mod->pub->period & BIT_MASK(6)) * 10U);
        break;
    case 0x03:
        /* 1 step is 10 minutes */
        period = K_MINUTES((mod->pub->period & BIT_MASK(6)) * 10U);
        break;
    default:
        BT_ERR("Unknown model publication period");
        return 0;
    }

    if (mod->pub->fast_period) {
        return period >> mod->pub->period_div;
    } else {
        return period;
    }
}

static int32_t next_period(struct bt_mesh_model *mod)
{
    struct bt_mesh_model_pub *pub = mod->pub;
    uint32_t elapsed = 0U, period = 0U;

    if (!pub) {
        BT_ERR("Model has no publication support");
        return -ENOTSUP;
    }

    period = bt_mesh_model_pub_period_get(mod);
    if (!period) {
        return 0;
    }

    elapsed = k_uptime_get_32() - pub->period_start;

    BT_INFO("Publishing took %ums", elapsed);

    if (elapsed >= period) {
        BT_WARN("Publication sending took longer than the period");
        /* Return smallest positive number since 0 means disabled */
        return K_MSEC(1);
    }

    return period - elapsed;
}

static void publish_sent(int err, void *user_data)
{
    struct bt_mesh_model *mod = user_data;
    int32_t delay = 0;

    BT_DBG("err %d", err);

    if (!mod->pub) {
        BT_ERR("Model has no publication support");
        return;
    }

    if (mod->pub->count) {
        delay = BLE_MESH_PUB_TRANSMIT_INT(mod->pub->retransmit);
    } else {
        delay = next_period(mod);
    }

    if (delay) {
        BT_INFO("Publishing next time in %dms", delay);
        k_delayed_work_submit(&mod->pub->timer, delay);
    }
}

static void publish_start(uint16_t duration, int err, void *user_data)
{
    struct bt_mesh_model *mod = user_data;
    struct bt_mesh_model_pub *pub = mod->pub;

    if (err) {
        BT_ERR("Failed to publish: err %d", err);
        return;
    }

    /* Initialize the timestamp for the beginning of a new period */
    if (pub->count == BLE_MESH_PUB_TRANSMIT_COUNT(pub->retransmit)) {
        pub->period_start = k_uptime_get_32();
    }
}

static const struct bt_mesh_send_cb pub_sent_cb = {
    .start = publish_start,
    .end = publish_sent,
};

static int publish_retransmit(struct bt_mesh_model *mod)
{
    struct bt_mesh_model_pub *pub = mod->pub;
    if (!pub) {
        BT_ERR("Model has no publication support");
        return -ENOTSUP;
    }

    struct bt_mesh_app_key *key = NULL;
    struct net_buf_simple *sdu = NULL;
    struct bt_mesh_msg_ctx ctx = {
        .addr = pub->addr,
        .send_ttl = pub->ttl,
        .model = mod,
        .srv_send = (pub->dev_role == NODE ? true : false),
    };
    struct bt_mesh_net_tx tx = {
        .ctx = &ctx,
        .src = bt_mesh_model_elem(mod)->addr,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = pub->cred,
    };
    int err = 0;

    key = bt_mesh_tx_appkey_get(pub->dev_role, pub->key);
    if (!key) {
        BT_ERR("AppKey 0x%03x not exists", pub->key);
        return -EADDRNOTAVAIL;
    }

    tx.sub = bt_mesh_tx_netkey_get(pub->dev_role, key->net_idx);
    if (!tx.sub) {
        BT_ERR("Subnet 0x%04x not exists", key->net_idx);
        return -EADDRNOTAVAIL;
    }

    ctx.net_idx = key->net_idx;
    ctx.app_idx = key->app_idx;

    sdu = bt_mesh_alloc_buf(pub->msg->len + BLE_MESH_MIC_SHORT);
    if (!sdu) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    net_buf_simple_add_mem(sdu, pub->msg->data, pub->msg->len);

    pub->count--;

    err = bt_mesh_trans_send(&tx, sdu, &pub_sent_cb, mod);

    bt_mesh_free_buf(sdu);
    return err;
}

static void publish_retransmit_end(int err, struct bt_mesh_model_pub *pub)
{
    /* Cancel all retransmits for this publish attempt */
    pub->count = 0U;
    /* Make sure the publish timer gets reset */
    publish_sent(err, pub->mod);
}

static void mod_publish(struct k_work *work)
{
    struct bt_mesh_model_pub *pub = CONTAINER_OF(work,
                                    struct bt_mesh_model_pub,
                                    timer.work);
    int32_t period_ms = 0;
    int err = 0;

    BT_DBG("%s", __func__);

    period_ms = bt_mesh_model_pub_period_get(pub->mod);
    BT_INFO("Publish period %u ms", period_ms);

    if (pub->count) {
        err = publish_retransmit(pub->mod);
        if (err) {
            BT_ERR("Failed to retransmit (err %d)", err);

            pub->count = 0U;

            /* Continue with normal publication */
            if (period_ms) {
                k_delayed_work_submit(&pub->timer, period_ms);
            }
        }

        return;
    }

    if (!period_ms) {
        return;
    }

    /* Callback the model publish update event to the application layer.
     * In the event, users can update the context of the publish message
     * which will be published in the next period.
     */
    if (pub->update && pub->update(pub->mod)) {
        /* Cancel this publish attempt. */
        BT_ERR("Update failed, skipping publish (err %d)", err);
        pub->period_start = k_uptime_get_32();
        publish_retransmit_end(err, pub);
        return;
    }

    err = bt_mesh_model_publish(pub->mod);
    if (err) {
        BT_ERR("Publishing failed (err %d)", err);
    }
}

struct bt_mesh_elem *bt_mesh_model_elem(struct bt_mesh_model *mod)
{
    return &dev_comp->elem[mod->elem_idx];
}

struct bt_mesh_model *bt_mesh_model_get(bool vnd, uint8_t elem_idx, uint8_t mod_idx)
{
    struct bt_mesh_elem *elem = NULL;

    if (!dev_comp) {
        BT_ERR("dev_comp not initialized");
        return NULL;
    }

    if (elem_idx >= dev_comp->elem_count) {
        BT_ERR("Invalid element index %u", elem_idx);
        return NULL;
    }

    elem = &dev_comp->elem[elem_idx];

    if (vnd) {
        if (mod_idx >= elem->vnd_model_count) {
            BT_ERR("Invalid vendor model index %u", mod_idx);
            return NULL;
        }

        return &elem->vnd_models[mod_idx];
    } else {
        if (mod_idx >= elem->model_count) {
            BT_ERR("Invalid SIG model index %u", mod_idx);
            return NULL;
        }

        return &elem->models[mod_idx];
    }
}

static void mod_init(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                     bool vnd, bool primary, void *user_data)
{
    int *err = user_data;
    int i;

    if (!user_data) {
        BT_ERR("Invalid model init user data");
        return;
    }

    if (*err) {
        BT_ERR("Model init failed (err %d)", *err);
        return;
    }

    mod->elem = elem;

    if (mod->pub) {
        mod->pub->mod = mod;
        k_delayed_work_init(&mod->pub->timer, mod_publish);
    }

    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        mod->keys[i] = BLE_MESH_KEY_UNUSED;
    }

    mod->flags = 0;
    mod->elem_idx = elem - dev_comp->elem;
    if (vnd) {
        mod->model_idx = mod - elem->vnd_models;
    } else {
        mod->model_idx = mod - elem->models;
    }

    if (vnd) {
        return;
    }

    if (mod->cb && mod->cb->init) {
        *err = mod->cb->init(mod);
    }
}

int bt_mesh_comp_register(const struct bt_mesh_comp *comp)
{
    int err = 0;

    /* There must be at least one element */
    if (!comp->elem_count) {
        return -EINVAL;
    }

    dev_comp = comp;

    bt_mesh_model_foreach(mod_init, &err);

    return err;
}

#if CONFIG_BLE_MESH_DEINIT
static void mod_deinit(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                       bool vnd, bool primary, void *user_data)
{
    int *err = user_data;
    int i;

    if (!user_data) {
        BT_ERR("Invalid model deinit user data");
        return;
    }

    if (*err) {
        BT_ERR("Model deinit failed (err %d)", *err);
        return;
    }

    mod->elem = NULL;

    if (mod->pub) {
        mod->pub->mod = NULL;
        k_delayed_work_free(&mod->pub->timer);
    }

    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        mod->keys[i] = BLE_MESH_KEY_UNUSED;
    }

    mod->flags = 0U;
    mod->elem_idx = 0U;
    mod->model_idx = 0U;

    if (vnd) {
        return;
    }

    if (mod->cb && mod->cb->deinit) {
        *err = mod->cb->deinit(mod);
    }
}

int bt_mesh_comp_deregister(void)
{
    int err = 0;

    if (dev_comp == NULL) {
        return -EINVAL;
    }

    bt_mesh_model_foreach(mod_deinit, &err);

    dev_comp = NULL;

    return err;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_comp_provision(uint16_t addr)
{
    int i;

    dev_primary_addr = addr;

    BT_INFO("Primary address 0x%04x, element count %u", addr, dev_comp->elem_count);

    for (i = 0; i < dev_comp->elem_count; i++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[i];

        elem->addr = addr++;

        BT_DBG("addr 0x%04x mod_count %u vnd_mod_count %u",
               elem->addr, elem->model_count, elem->vnd_model_count);
    }
}

void bt_mesh_comp_unprovision(void)
{
    BT_DBG("%s", __func__);

    dev_primary_addr = BLE_MESH_ADDR_UNASSIGNED;
}

uint16_t bt_mesh_primary_addr(void)
{
    return dev_primary_addr;
}

uint16_t *bt_mesh_model_find_group(struct bt_mesh_model *mod, uint16_t addr)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] == addr) {
            return &mod->groups[i];
        }
    }

    return NULL;
}

static struct bt_mesh_model *bt_mesh_elem_find_group(struct bt_mesh_elem *elem,
                                                     uint16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    uint16_t *match = NULL;
    int i;

    for (i = 0; i < elem->model_count; i++) {
        model = &elem->models[i];

        match = bt_mesh_model_find_group(model, group_addr);
        if (match) {
            return model;
        }
    }

    for (i = 0; i < elem->vnd_model_count; i++) {
        model = &elem->vnd_models[i];

        match = bt_mesh_model_find_group(model, group_addr);
        if (match) {
            return model;
        }
    }

    return NULL;
}

struct bt_mesh_elem *bt_mesh_elem_find(uint16_t addr)
{
    uint16_t index = 0U;

    if (BLE_MESH_ADDR_IS_UNICAST(addr)) {
        index = (addr - dev_comp->elem[0].addr);
        if (index < dev_comp->elem_count) {
            return &dev_comp->elem[index];
        } else {
            return NULL;
        }
    }

    for (index = 0; index < dev_comp->elem_count; index++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[index];

        if (bt_mesh_elem_find_group(elem, addr)) {
            return elem;
        }
    }

    return NULL;
}

uint8_t bt_mesh_elem_count(void)
{
    return dev_comp->elem_count;
}

static bool model_has_key(struct bt_mesh_model *mod, uint16_t key)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        if (mod->keys[i] == key) {
            return true;
        }
    }

    return false;
}

static bool model_has_dst(struct bt_mesh_model *model, uint16_t dst)
{
    if (BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return (dev_comp->elem[model->elem_idx].addr == dst);
    } else if (BLE_MESH_ADDR_IS_GROUP(dst) || BLE_MESH_ADDR_IS_VIRTUAL(dst)) {
        return !!bt_mesh_model_find_group(model, dst);
    }

    return (model->elem_idx == 0 && bt_mesh_fixed_group_match(dst));
}

static const struct bt_mesh_model_op *find_op(struct bt_mesh_model *models,
                                              uint8_t model_count, uint32_t opcode,
                                              struct bt_mesh_model **model)
{
    int i;

    for (i = 0; i < model_count; i++) {
        const struct bt_mesh_model_op *op;

        *model = &models[i];

        for (op = (*model)->op; op->func; op++) {
            if (op->opcode == opcode) {
                return op;
            }
        }
    }

    *model = NULL;
    return NULL;
}

static int get_opcode(struct net_buf_simple *buf, uint32_t *opcode)
{
    switch (buf->data[0] >> 6) {
    case 0x00:
    case 0x01:
        if (buf->data[0] == 0x7f) {
            BT_ERR("Ignoring RFU OpCode");
            return -EINVAL;
        }

        *opcode = net_buf_simple_pull_u8(buf);
        return 0;
    case 0x02:
        if (buf->len < 2) {
            BT_ERR("Too short payload for 2-octet OpCode");
            return -EINVAL;
        }

        *opcode = net_buf_simple_pull_be16(buf);
        return 0;
    case 0x03:
        if (buf->len < 3) {
            BT_ERR("Too short payload for 3-octet OpCode");
            return -EINVAL;
        }

        *opcode = net_buf_simple_pull_u8(buf) << 16;
        /* Using LE for the CID since the model layer is defined as
         * little-endian in the mesh spec and using BT_MESH_MODEL_OP_3
         * will declare the opcode in this way.
         */
        *opcode |= net_buf_simple_pull_le16(buf);
        return 0;
    }

    return -EINVAL;
}

bool bt_mesh_fixed_group_match(uint16_t addr)
{
    /* Check for fixed group addresses */
    switch (addr) {
    case BLE_MESH_ADDR_ALL_NODES:
        return true;
    case BLE_MESH_ADDR_PROXIES:
        return (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED);
    case BLE_MESH_ADDR_FRIENDS:
        return (bt_mesh_friend_get() == BLE_MESH_FRIEND_ENABLED);
    case BLE_MESH_ADDR_RELAYS:
        return (bt_mesh_relay_get() == BLE_MESH_RELAY_ENABLED);
    default:
        return false;
    }
}

void bt_mesh_model_recv(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf)
{
    struct bt_mesh_model *models = NULL, *model = NULL;
    const struct bt_mesh_model_op *op = NULL;
    uint32_t opcode = 0U;
    uint8_t count = 0U;
    int i;

    BT_INFO("recv, app_idx 0x%04x src 0x%04x dst 0x%04x", rx->ctx.app_idx,
           rx->ctx.addr, rx->ctx.recv_dst);
    BT_INFO("recv, len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    if (get_opcode(buf, &opcode) < 0) {
        BT_WARN("Unable to decode OpCode");
        return;
    }

    BT_DBG("OpCode 0x%08x", opcode);

    for (i = 0; i < dev_comp->elem_count; i++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[i];
        struct net_buf_simple_state state = {0};

        /* SIG models cannot contain 3-byte (vendor) OpCodes, and
         * vendor models cannot contain SIG (1- or 2-byte) OpCodes, so
         * we only need to do the lookup in one of the model lists.
         */
        if (BLE_MESH_MODEL_OP_LEN(opcode) < 3) {
            models = elem->models;
            count = elem->model_count;
        } else {
            models = elem->vnd_models;
            count = elem->vnd_model_count;
        }

        op = find_op(models, count, opcode, &model);
        if (!op) {
            BT_DBG("No OpCode 0x%08x for elem %d", opcode, i);
            continue;
        }

        if (!model_has_key(model, rx->ctx.app_idx)) {
            continue;
        }

        if (!model_has_dst(model, rx->ctx.recv_dst)) {
            continue;
        }

        if (buf->len < op->min_len) {
            BT_ERR("Too short message for OpCode 0x%08x", opcode);
            continue;
        }

        /* The following three operations are added by Espressif.
         * 1. Update the "recv_op" with the opcode got from the buf;
         * 2. Update the model pointer with the found model;
         * 3. Update the "srv_send" to be true when received a message.
         *    This flag will be used when a server model sends a status
         *    message, and has no impact on the client messages.
         * Most of these info will be used by the application layer.
         */
        rx->ctx.recv_op = opcode;
        rx->ctx.model = model;
        rx->ctx.srv_send = true;

        /* The callback will likely parse the buffer, so store
         * the parsing state in case multiple models receive
         * the message.
         */
        net_buf_simple_save(buf, &state);
        op->func(model, &rx->ctx, buf);
        net_buf_simple_restore(buf, &state);
    }
}

void bt_mesh_model_msg_init(struct net_buf_simple *msg, uint32_t opcode)
{
    net_buf_simple_init(msg, 0);

    switch (BLE_MESH_MODEL_OP_LEN(opcode)) {
    case 1:
        net_buf_simple_add_u8(msg, opcode);
        break;
    case 2:
        net_buf_simple_add_be16(msg, opcode);
        break;
    case 3:
        net_buf_simple_add_u8(msg, ((opcode >> 16) & 0xff));
        /* Using LE for the CID since the model layer is defined as
         * little-endian in the mesh spec and using BT_MESH_MODEL_OP_3
         * will declare the opcode in this way.
         */
        net_buf_simple_add_le16(msg, opcode & 0xffff);
        break;
    default:
        BT_WARN("Unknown opcode format");
        break;
    }
}

static bool ready_to_send(uint8_t role, uint16_t dst)
{
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        return true;
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        if (!bt_mesh_provisioner_check_msg_dst(dst)) {
            BT_ERR("Failed to find DST 0x%04x", dst);
            return false;
        }
        return true;
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        return true;
    }

    return false;
}

static int model_send(struct bt_mesh_model *model,
                      struct bt_mesh_net_tx *tx, bool implicit_bind,
                      struct net_buf_simple *msg,
                      const struct bt_mesh_send_cb *cb, void *cb_data)
{
    uint8_t role = 0U;

    role = bt_mesh_get_device_role(model, tx->ctx->srv_send);
    if (role == ROLE_NVAL) {
        BT_ERR("Failed to get model role");
        return -EINVAL;
    }

    BT_INFO("send, app_idx 0x%04x src 0x%04x dst 0x%04x",
        tx->ctx->app_idx, tx->src, tx->ctx->addr);
    BT_INFO("send, len %u: %s", msg->len, bt_hex(msg->data, msg->len));

    if (!ready_to_send(role, tx->ctx->addr)) {
        BT_ERR("Not ready to send");
        return -EINVAL;
    }

    if (net_buf_simple_tailroom(msg) < BLE_MESH_MIC_SHORT) {
        BT_ERR("Not enough tailroom for TransMIC");
        return -EINVAL;
    }

    if (msg->len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SDU_MAX_LEN) - BLE_MESH_MIC_SHORT) {
        BT_ERR("Too big message (len %d)", msg->len);
        return -EMSGSIZE;
    }

    if (!implicit_bind && !model_has_key(model, tx->ctx->app_idx)) {
        BT_ERR("Model not bound to AppKey 0x%04x", tx->ctx->app_idx);
        return -EINVAL;
    }

    return bt_mesh_trans_send(tx, msg, cb, cb_data);
}

int bt_mesh_model_send(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *msg,
                       const struct bt_mesh_send_cb *cb, void *cb_data)
{
    struct bt_mesh_subnet *sub = NULL;
    uint8_t role = 0U;

    role = bt_mesh_get_device_role(model, ctx->srv_send);
    if (role == ROLE_NVAL) {
        BT_ERR("Failed to get model role");
        return -EINVAL;
    }

    sub = bt_mesh_tx_netkey_get(role, ctx->net_idx);
    if (!sub) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", ctx->net_idx);
        return -EINVAL;
    }

    ctx->model = model;

    struct bt_mesh_net_tx tx = {
        .sub = sub,
        .ctx = ctx,
        .src = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = 0,
    };

    return model_send(model, &tx, false, msg, cb, cb_data);
}

int bt_mesh_model_publish(struct bt_mesh_model *model)
{
    struct bt_mesh_model_pub *pub = model->pub;
    struct bt_mesh_app_key *key = NULL;
    struct net_buf_simple *sdu = NULL;
    struct bt_mesh_msg_ctx ctx = {
        .model = model,
    };
    struct bt_mesh_net_tx tx = {
        .sub = NULL,
        .ctx = &ctx,
        .src = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };
    int err = 0;

    BT_DBG("%s", __func__);

    if (!pub || !pub->msg) {
        BT_ERR("Model has no publication support");
        return -ENOTSUP;
    }

    if (pub->addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_WARN("Unassigned publish address");
        return -EADDRNOTAVAIL;
    }

    key = bt_mesh_tx_appkey_get(pub->dev_role, pub->key);
    if (!key) {
        BT_ERR("Invalid AppKeyIndex 0x%03x", pub->key);
        return -EADDRNOTAVAIL;
    }

    if (pub->msg->len + BLE_MESH_MIC_SHORT > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SDU_MAX_LEN)) {
        BT_ERR("Message does not fit maximum SDU size");
        return -EMSGSIZE;
    }

    if (pub->count) {
        BT_WARN("Clearing publish retransmit timer");
        k_delayed_work_cancel(&pub->timer);
    }

    ctx.addr = pub->addr;
    ctx.send_rel = pub->send_rel;
    ctx.send_ttl = pub->ttl;
    ctx.net_idx = key->net_idx;
    ctx.app_idx = key->app_idx;
    ctx.srv_send = pub->dev_role == NODE ? true : false;

    tx.friend_cred = pub->cred;

    tx.sub = bt_mesh_tx_netkey_get(pub->dev_role, ctx.net_idx);
    if (!tx.sub) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", ctx.net_idx);
        return -EADDRNOTAVAIL;
    }

    pub->count = BLE_MESH_PUB_TRANSMIT_COUNT(pub->retransmit);

    BT_INFO("Publish Retransmit Count %u Interval %ums", pub->count,
           BLE_MESH_PUB_TRANSMIT_INT(pub->retransmit));

    sdu = bt_mesh_alloc_buf(pub->msg->len + BLE_MESH_MIC_SHORT);
    if (!sdu) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    net_buf_simple_add_mem(sdu, pub->msg->data, pub->msg->len);

    err = model_send(model, &tx, true, sdu, &pub_sent_cb, model);
    if (err) {
        publish_retransmit_end(err, pub);
    }

    bt_mesh_free_buf(sdu);
    return err;
}

struct bt_mesh_model *bt_mesh_model_find_vnd(struct bt_mesh_elem *elem,
                                             uint16_t company, uint16_t id)
{
    int i;

    for (i = 0; i < elem->vnd_model_count; i++) {
        if (elem->vnd_models[i].vnd.company == company &&
                elem->vnd_models[i].vnd.id == id) {
            return &elem->vnd_models[i];
        }
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_model_find(struct bt_mesh_elem *elem, uint16_t id)
{
    int i;

    for (i = 0; i < elem->model_count; i++) {
        if (elem->models[i].id == id) {
            return &elem->models[i];
        }
    }

    return NULL;
}

const struct bt_mesh_comp *bt_mesh_comp_get(void)
{
    return dev_comp;
}

/* APIs used by messages encryption in upper transport layer & network layer */
struct bt_mesh_subnet *bt_mesh_tx_netkey_get(uint8_t role, uint16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        sub = bt_mesh_subnet_get(net_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        sub = bt_mesh_provisioner_subnet_get(net_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        sub = bt_mesh_fast_prov_subnet_get(net_idx);
    }

    return sub;
}

const uint8_t *bt_mesh_tx_devkey_get(uint8_t role, uint16_t dst)
{
    const uint8_t *key = NULL;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        key = bt_mesh.dev_key;
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        key = bt_mesh_provisioner_dev_key_get(dst);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        key = bt_mesh_fast_prov_dev_key_get(dst);
    }

    return key;
}

struct bt_mesh_app_key *bt_mesh_tx_appkey_get(uint8_t role, uint16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() && role == NODE) {
        key = bt_mesh_app_key_find(app_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en() && role == PROVISIONER) {
        key = bt_mesh_provisioner_app_key_find(app_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && bt_mesh_is_provisioned() && role == FAST_PROV) {
        key = bt_mesh_fast_prov_app_key_find(app_idx);
    }

    return key;
}

/* APIs used by messages decryption in network layer & upper transport layer */
size_t bt_mesh_rx_netkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = ARRAY_SIZE(bt_mesh.sub);
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = ARRAY_SIZE(bt_mesh.p_sub);
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = ARRAY_SIZE(bt_mesh.sub);
    if (bt_mesh_is_provisioner_en()) {
        size += ARRAY_SIZE(bt_mesh.p_sub);
    }
#endif

    return size;
}

struct bt_mesh_subnet *bt_mesh_rx_netkey_get(size_t index)
{
    struct bt_mesh_subnet *sub = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        sub = &bt_mesh.sub[index];
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        sub = bt_mesh.p_sub[index];
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < ARRAY_SIZE(bt_mesh.sub)) {
        sub = &bt_mesh.sub[index];
    } else {
        sub = bt_mesh.p_sub[index - ARRAY_SIZE(bt_mesh.sub)];
    }
#endif

    return sub;
}

size_t bt_mesh_rx_devkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = 1;
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = 1;
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = 1;
    if (bt_mesh_is_provisioner_en()) {
        size += 1;
    }
#endif

    return size;
}

const uint8_t *bt_mesh_rx_devkey_get(size_t index, uint16_t src)
{
    const uint8_t *key = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        key = bt_mesh.dev_key;
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        key = bt_mesh_provisioner_dev_key_get(src);
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < 1) {
        key = bt_mesh.dev_key;
    } else {
        key = bt_mesh_provisioner_dev_key_get(src);
    }
#endif

    return key;
}

size_t bt_mesh_rx_appkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = ARRAY_SIZE(bt_mesh.app_keys);
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = ARRAY_SIZE(bt_mesh.p_app_keys);
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = ARRAY_SIZE(bt_mesh.app_keys);
    if (bt_mesh_is_provisioner_en()) {
        size += ARRAY_SIZE(bt_mesh.p_app_keys);
    }
#endif

    return size;
}

struct bt_mesh_app_key *bt_mesh_rx_appkey_get(size_t index)
{
    struct bt_mesh_app_key *key = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        key = &bt_mesh.app_keys[index];
    }
#endif

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        key = bt_mesh.p_app_keys[index];
    }
#endif

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < ARRAY_SIZE(bt_mesh.app_keys)) {
        key = &bt_mesh.app_keys[index];
    } else {
        key = bt_mesh.p_app_keys[index - ARRAY_SIZE(bt_mesh.app_keys)];
    }
#endif

    return key;
}
