/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "mesh.h"
#include "tag.h"
#include "adv.h"
#include "lpn.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "mesh/main.h"
#include "mesh/common.h"
#include "fast_prov.h"
#include "pvnr_mgmt.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif /* CONFIG_BLE_MESH_V11_SUPPORT */

#define BLE_MESH_SDU_MAX_LEN    384

/* Extended SDU maximum length (message length + TransMIC size) calculation:
 * 32 segments maximum × (249 bytes max PDU - 17 bytes overhead)
 * 17 bytes = 9 bytes mesh header + 4 bytes transport overhead + 4-byte NetMIC */
#define BLE_MESH_EXT_SDU_MAX_LEN    (32*(249-17))

extern const struct bt_mesh_comp *comp_0;
static uint16_t dev_primary_addr;

static int model_send(const struct bt_mesh_model *model,
                      struct bt_mesh_net_tx *tx, bool implicit_bind,
                      struct net_buf_simple *msg,
                      const struct bt_mesh_send_cb *cb, void *cb_data);

void bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod,
                                        struct bt_mesh_elem *elem,
                                        bool vnd, bool primary,
                                        void *user_data),
                           void *user_data)
{
    int i, j;

    BT_DBG("ModelForeach");

    if (comp_0 == NULL) {
        BT_ERR("Invalid device composition");
        return;
    }

    for (i = 0; i < comp_0->elem_count; i++) {
        struct bt_mesh_elem *elem = &comp_0->elem[i];

        BT_DBG("Element %u", i);

        for (j = 0; j < elem->model_count; j++) {
            struct bt_mesh_model *model = &elem->models[j];

            BT_DBG("ID 0x%04x", model->id);

            func(model, elem, false, i == 0, user_data);
        }

        for (j = 0; j < elem->vnd_model_count; j++) {
            struct bt_mesh_model *model = &elem->vnd_models[j];

            BT_DBG("ID 0x%04x CID 0x%04x", model->vnd.id, model->vnd.company);

            func(model, elem, true, i == 0, user_data);
        }
    }
}

int32_t bt_mesh_model_pub_period_get(struct bt_mesh_model *mod)
{
    int period = 0;

    BT_DBG("ModelPubPeriodGet");

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

    BT_DBG("Period %ld FastPeriod %u PeriodDiv %u",
           period, mod->pub->fast_period, mod->pub->period_div);

    if (mod->pub->fast_period) {
        return period >> mod->pub->period_div;
    }

    return period;
}

static int32_t next_period(struct bt_mesh_model *mod)
{
    struct bt_mesh_model_pub *pub = mod->pub;
    uint32_t elapsed = 0U, period = 0U;

    BT_DBG("NextPeriod");

    if (!pub) {
        BT_ERR("Model has no publication support");
        return 0;
    }

    period = bt_mesh_model_pub_period_get(mod);
    if (!period) {
        BT_DBG("PeriodZero");
        return 0;
    }

    elapsed = k_uptime_get_32() - pub->period_start;

    BT_INFO("Elapsed %u Period %u", elapsed, period);

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

    BT_DBG("PublishSent, Err %d", err);

    if (!mod->pub) {
        BT_ERR("Model has no publication support");
        return;
    }

    if (mod->pub->count) {
        delay = BLE_MESH_PUB_TRANSMIT_INT(mod->pub->retransmit);
    } else {
        delay = next_period(mod);
    }

    BT_DBG("PubCount %u PubDelay %ld", mod->pub->count, delay);

    if (delay) {
        k_delayed_work_submit(&mod->pub->timer, delay);
    }
}

static void publish_start(uint16_t duration, int err, void *user_data)
{
    struct bt_mesh_model *mod = user_data;
    struct bt_mesh_model_pub *pub = mod->pub;

    BT_DBG("PublishStart, Err %d", err);

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
    struct bt_mesh_app_key *key = NULL;
    struct net_buf_simple *sdu = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    struct bt_mesh_net_tx tx = {
        .ctx  = &ctx,
        .src  = bt_mesh_model_elem(mod)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };
    int err = 0;

    BT_DBG("PublishRetransmit");

    if (!pub || !pub->msg) {
        BT_ERR("Model has no publication support");
        return -ENOTSUP;
    }

    key = bt_mesh_app_key_get(pub->key);
    if (!key) {
        BT_ERR("Publish, AppKey 0x%03x not found", pub->key);
        return -EADDRNOTAVAIL;
    }

    tx.sub = bt_mesh_subnet_get(key->net_idx);
    if (!tx.sub) {
        BT_ERR("Publish, NetKey 0x%04x not found", key->net_idx);
        return -EADDRNOTAVAIL;
    }

    ctx.addr = pub->addr;
    ctx.net_idx = key->net_idx;
    ctx.app_idx = key->app_idx;
    ctx.send_ttl = pub->ttl;
    ctx.send_cred = pub->cred ? BLE_MESH_FRIENDSHIP_CRED : BLE_MESH_FLOODING_CRED;
    ctx.send_szmic = pub->send_szmic;
    if (pub->send_rel) {
        /* Tag with send-segmented */
        ctx.send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
    }

    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Dst 0x%04x",
           ctx.net_idx, ctx.app_idx, ctx.addr);
    BT_DBG("TTL %u SendCred %u SendRel %u PubCount %u",
           ctx.send_ttl, ctx.send_cred, pub->send_rel, pub->count);

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_model_pub_use_directed(&tx, pub->directed_pub_policy);
#endif /* CONFIG_BLE_MESH_DF_SRV */

    sdu = bt_mesh_alloc_buf(pub->msg->len + BLE_MESH_MIC_LONG);
    if (!sdu) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    net_buf_simple_add_mem(sdu, pub->msg->data, pub->msg->len);

    pub->count--;

    err = model_send(mod, &tx, true, sdu, &pub_sent_cb, mod);

    bt_mesh_free_buf(sdu);
    return err;
}

static void publish_retransmit_end(int err, struct bt_mesh_model_pub *pub)
{
    BT_DBG("PublishRetransmitEnd, Err %d", err);

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

    period_ms = bt_mesh_model_pub_period_get(pub->mod);

    BT_INFO("ModPublish, Period %u", period_ms);

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

struct bt_mesh_elem *bt_mesh_model_elem(const struct bt_mesh_model *mod)
{
    BT_DBG("ModelElem, ElemIdx %u", mod->elem_idx);

    return &comp_0->elem[mod->elem_idx];
}

struct bt_mesh_model *bt_mesh_model_get(bool vnd, uint8_t elem_idx, uint8_t mod_idx)
{
    struct bt_mesh_elem *elem = NULL;

    BT_DBG("ModelGet, ElemIdx %u ModIdx %u Vnd %u", elem_idx, mod_idx, vnd);

    if (!comp_0) {
        BT_ERR("comp_0 not initialized");
        return NULL;
    }

    if (elem_idx >= comp_0->elem_count) {
        BT_ERR("Invalid element index %u", elem_idx);
        return NULL;
    }

    elem = &comp_0->elem[elem_idx];

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

    BT_DBG("ModInit, Vnd %u Primary %u", vnd, primary);

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
    mod->elem_idx = elem - comp_0->elem;
    if (vnd) {
        mod->model_idx = mod - elem->vnd_models;
    } else {
        mod->model_idx = mod - elem->models;
    }

    BT_DBG("ElemIdx %u ModelIdx %u", mod->elem_idx, mod->model_idx);

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

    BT_DBG("CompRegister, ElemCount %u", comp->elem_count);

    /* There must be at least one element */
    if (!comp->elem_count) {
        return -EINVAL;
    }

    comp_0 = comp;

    bt_mesh_model_foreach(mod_init, &err);

    return err;
}

#if CONFIG_BLE_MESH_DEINIT
static void mod_deinit(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                       bool vnd, bool primary, void *user_data)
{
    int *err = user_data;
    int i;

    BT_DBG("ModDeinit, Vnd %u Primary %u", vnd, primary);

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

    BT_DBG("CompDeregister");

    if (comp_0 == NULL) {
        return -EINVAL;
    }

    bt_mesh_model_foreach(mod_deinit, &err);

    comp_0 = NULL;

    return err;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_comp_provision(uint16_t addr)
{
    int i;

    dev_primary_addr = addr;

    BT_INFO("CompProvision, PrimaryAddr 0x%04x ElemCount %u", addr, comp_0->elem_count);

    for (i = 0; i < comp_0->elem_count; i++) {
        struct bt_mesh_elem *elem = &comp_0->elem[i];

        elem->addr = addr++;

        BT_DBG("ElemAddr 0x%04x ModCount %u VndModCount %u",
               elem->addr, elem->model_count, elem->vnd_model_count);
    }
}

void bt_mesh_comp_unprovision(void)
{
    BT_DBG("CompUnprovision");

    dev_primary_addr = BLE_MESH_ADDR_UNASSIGNED;
}

uint16_t bt_mesh_primary_addr(void)
{
    BT_DBG("PrimaryAddr 0x%04x", dev_primary_addr);

    return dev_primary_addr;
}

uint16_t *bt_mesh_model_find_group(struct bt_mesh_model *mod, uint16_t addr)
{
    int i;

    BT_DBG("ModelFindGroup, Addr 0x%04x", addr);

    for (i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] == addr) {
            BT_DBG("ModGroupFound");
            return &mod->groups[i];
        }
    }

    BT_DBG("ModGroupNotFound");
    return NULL;
}

static struct bt_mesh_model *bt_mesh_elem_find_group(struct bt_mesh_elem *elem,
                                                     uint16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    uint16_t *match = NULL;
    int i;

    BT_DBG("ElemFindGroup, Addr 0x%04x", group_addr);

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

    BT_DBG("ElemFind, Addr 0x%04x", addr);

    if (BLE_MESH_ADDR_IS_UNICAST(addr)) {
        index = (addr - comp_0->elem[0].addr);
        if (index < comp_0->elem_count) {
            return &comp_0->elem[index];
        }

        return NULL;
    }

    for (index = 0; index < comp_0->elem_count; index++) {
        struct bt_mesh_elem *elem = &comp_0->elem[index];

        if (bt_mesh_elem_find_group(elem, addr)) {
            return elem;
        }
    }

    return NULL;
}

bool bt_mesh_has_addr(uint16_t addr)
{
    uint16_t index;

    if (BLE_MESH_ADDR_IS_UNICAST(addr)) {
        return bt_mesh_elem_find(addr) != NULL;
    }

    for (index = 0; index < comp_0->elem_count; index++) {
        struct bt_mesh_elem *elem = &comp_0->elem[index];

        if (bt_mesh_elem_find_group(elem, addr)) {
            return true;
        }
    }

    return false;
}

uint8_t bt_mesh_elem_count(void)
{
    BT_DBG("ElemCount %u", comp_0->elem_count);

    return comp_0->elem_count;
}

static bool model_has_key(const struct bt_mesh_model *mod, uint16_t key)
{
    int i;

    BT_DBG("ModelHasKey, AppIdx 0x%04x", key);

    for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
        if (mod->keys[i] == key) {
            return true;
        }
    }

    return false;
}

static bool model_has_dst(struct bt_mesh_model *model,
                          struct bt_mesh_subnet *sub,
                          uint16_t dst)
{
    BT_DBG("ModelHasDst, Dst 0x%04x", dst);

    if (BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return (comp_0->elem[model->elem_idx].addr == dst);
    }

    if (BLE_MESH_ADDR_IS_GROUP(dst) || BLE_MESH_ADDR_IS_VIRTUAL(dst)) {
        return !!bt_mesh_model_find_group(model, dst);
    }

    return (model->elem_idx == 0 && (bt_mesh_fixed_group_match(dst) ||
                                     bt_mesh_fixed_direct_match(sub, dst)));
}

static const struct bt_mesh_model_op *find_op(struct bt_mesh_model *models,
                                              uint8_t model_count, uint32_t opcode,
                                              struct bt_mesh_model **model)
{
    int i;

    BT_DBG("FindOp, ModelCount %u Opcode 0x%08lx", model_count, opcode);

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

static int get_opcode(struct net_buf_simple *buf, uint32_t *opcode, bool pull_buf)
{
    BT_DBG("GetOpCode, PullBuf %u", pull_buf);

    switch (buf->data[0] >> 6) {
    case 0x00:
    case 0x01:
        if (buf->data[0] == 0x7f) {
            BT_ERR("Ignoring RFU OpCode");
            return -EINVAL;
        }

        *opcode = pull_buf ? net_buf_simple_pull_u8(buf) : buf->data[0];
        return 0;

    case 0x02:
        if (buf->len < 2) {
            BT_ERR("Too short payload for 2-octet OpCode");
            return -EINVAL;
        }

        *opcode = pull_buf ? net_buf_simple_pull_be16(buf) : sys_get_be16(buf->data);
        return 0;

    case 0x03:
        if (buf->len < 3) {
            BT_ERR("Too short payload for 3-octet OpCode");
            return -EINVAL;
        }

        if (pull_buf) {
            *opcode = net_buf_simple_pull_u8(buf) << 16;
            /* Using LE for the CID since the model layer is defined as
            * little-endian in the mesh spec and using BLE_MESH_MODEL_OP_3
            * will declare the opcode in this way.
            */
            *opcode |= net_buf_simple_pull_le16(buf);
        } else {
            *opcode = buf->data[0] << 16 | sys_get_le16(&buf->data[1]);
        }
        return 0;
    }

    return -EINVAL;
}

int bt_mesh_get_opcode(struct net_buf_simple *buf,
                       uint32_t *opcode, bool pull_buf)
{
    BT_DBG("GetOpCode");

    if (buf == NULL || buf->len == 0 || opcode == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    return get_opcode(buf, opcode, pull_buf);
}

bool bt_mesh_fixed_group_match(uint16_t addr)
{
    BT_DBG("FixedGroupMatch, Addr 0x%04x", addr);

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

bool bt_mesh_fixed_direct_match(struct bt_mesh_subnet *sub, uint16_t addr)
{
    /* A message sent to the all-directed-forwarding-nodes address
     * shall be processed by the primary element of all nodes that
     * have directed forwarding functionality enabled.
     */
    BT_DBG("FixedDirectMatch, Addr 0x%04x", addr);

#if CONFIG_BLE_MESH_DF_SRV
    if (addr == BLE_MESH_ADDR_DIRECTS && sub &&
        sub->directed_forwarding == BLE_MESH_DIRECTED_FORWARDING_ENABLED) {
        return true;
    }
#endif /* CONFIG_BLE_MESH_DF_SRV */

    return false;
}

void bt_mesh_model_recv(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf)
{
    struct bt_mesh_model *models = NULL, *model = NULL;
    const struct bt_mesh_model_op *op = NULL;
    uint32_t opcode = 0U;
    uint8_t count = 0U;
    int i;

    BT_INFO("ModelRecv");
    BT_INFO("AppIdx 0x%04x Src 0x%04x Dst 0x%04x",
            rx->ctx.app_idx, rx->ctx.addr, rx->ctx.recv_dst);
    BT_INFO("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    if (get_opcode(buf, &opcode, true) < 0) {
        BT_WARN("Unable to decode OpCode");
        return;
    }

    BT_DBG("OpCode 0x%08lx RecvCred %u", opcode, rx->ctx.recv_cred);

    for (i = 0; i < comp_0->elem_count; i++) {
        struct bt_mesh_elem *elem = &comp_0->elem[i];
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
            BT_DBG("ModelNotHasKey");
            continue;
        }

        if (!model_has_dst(model, rx->sub, rx->ctx.recv_dst)) {
            BT_DBG("ModelNotHasDst");
            continue;
        }

        if (buf->len < op->min_len) {
            BT_ERR("Too short message for OpCode 0x%08x", opcode);
            continue;
        }

        /* The following operation is added by Espressif.
         * Update the "recv_op" with the opcode got from the buf;
         */
        rx->ctx.recv_op = opcode;

        /* The message is transmitted by a model in response to a message that
         * it has received.
         *
         * The TTL field shall be set to the value of the Default TTL state.
         *
         * The response message shall use master security credentials. However,
         * the security credentials may be changed by a lower layer unless the
         * received message uses the master security credentials. If the received
         * message uses master security credentials, then the response message
         * shall be tagged with the immutable-credentials tag, and the security
         * credentials will not be changed by any lower layer.
         */
        rx->ctx.send_ttl = BLE_MESH_TTL_DEFAULT;
        if (rx->ctx.recv_cred == BLE_MESH_FLOODING_CRED) {
            rx->ctx.send_tag |= BLE_MESH_TAG_IMMUTABLE_CRED;
            rx->ctx.send_cred = BLE_MESH_FLOODING_CRED;
        }

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
    BT_DBG("ModelMsgInit, OpCode 0x%08lx", opcode);

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
         * little-endian in the mesh spec and using BLE_MESH_MODEL_OP_3
         * will declare the opcode in this way.
         */
        net_buf_simple_add_le16(msg, opcode & 0xffff);
        break;
    default:
        BT_WARN("Unknown opcode format");
        break;
    }
}

static bool ready_to_send(uint16_t dst)
{
    BT_DBG("IsReadyToSend, Dst 0x%04x", dst);

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        return true;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
        if (bt_mesh_provisioner_check_msg_dst(dst) == false &&
            bt_mesh_elem_find(dst) == false) {
            BT_ERR("Failed to find Dst 0x%04x", dst);
            return false;
        }
        return true;
    }

    return false;
}

#if !CONFIG_BLE_MESH_V11_SUPPORT
static bool use_friend_cred(uint16_t net_idx, uint16_t dst)
{
    BT_DBG("IsFrndCredUsed, NetIdx 0x%04x Dst 0x%04x", net_idx, dst);

    /* Currently LPN only supports using NetKey in bt_mesh.sub[0] */
    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        net_idx == 0 &&
        bt_mesh_lpn_match(dst)) {
        BT_DBG("LPNMatch");
        return true;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) &&
        bt_mesh_friend_match(net_idx, dst)) {
        BT_DBG("FrndMatch");
        return true;
    }

    return false;
}

bool bt_mesh_valid_security_cred(struct bt_mesh_net_tx *tx)
{
    /* If the message is tagged with immutable-credentials,
     * then the security credentials shall not be changed
     * in lower layers.
     * If not, later a better security credentials could be
     * chosen for the message.
     */
    BT_DBG("IsValidSecCred, NetIdx 0x%04x Tag 0x%02x Cred %u",
           tx->ctx->net_idx, tx->ctx->send_tag, tx->ctx->send_cred);

    if (!bt_mesh_tag_immutable_cred(tx->ctx->send_tag)) {
        return true;
    }

    if (tx->ctx->send_cred > BLE_MESH_FRIENDSHIP_CRED) {
        return false;
    }

    if (tx->ctx->send_cred == BLE_MESH_FRIENDSHIP_CRED &&
        !use_friend_cred(tx->ctx->net_idx, tx->ctx->addr)) {
        return false;
    }

    return true;
}

void bt_mesh_choose_better_security_cred(struct bt_mesh_net_tx *tx)
{
    uint8_t send_cred = 0U;
    uint8_t send_tag = 0U;
    uint16_t net_idx = 0U;
    uint16_t addr = 0U;

    send_cred = tx->ctx->send_cred;
    send_tag = tx->ctx->send_tag;
    net_idx = tx->ctx->net_idx;
    addr = tx->ctx->addr;

    BT_DBG("ChooseBetterSecCred");
    BT_DBG("NetIdx 0x%04x Dst 0x%04x Tag 0x%02x Cred %u",
           net_idx, addr, send_tag, send_cred);

    /* If the message is tagged with immutable-credentials,
     * then the security credentials shall not be changed.
     */
    if (bt_mesh_tag_immutable_cred(send_tag)) {
        BT_DBG("ImmutableCred");
        return;
    }

    if (send_cred > BLE_MESH_FRIENDSHIP_CRED) {
        BT_INFO("UseFloodingSecCred");
        tx->ctx->send_cred = BLE_MESH_FLOODING_CRED;
        return;
    }

    if (send_cred == BLE_MESH_FRIENDSHIP_CRED) {
        if (!use_friend_cred(net_idx, addr)) {
            BT_INFO("UseFloodingSecCred");
            tx->ctx->send_cred = BLE_MESH_FLOODING_CRED;
            tx->ctx->send_tag = send_tag | BLE_MESH_TAG_IMMUTABLE_CRED;
        } else {
            /* TODO:
             * For LPN, do we need to change the friendship security
             * credentials to managed flooding credentials?
             * If changed, this could increase the possibility that
             * the corresponding Friend node receives this message.
             */
        }
        return;
    }

    /* If the message is destinated to a LPN, the following could be
     * introduced to send the message with the friendship credentials.
     *
     * For LPN, this optimization should not be introduced, since it
     * may cause the message failed to received by the Friend node,
     * using friendship credentials will make the message can not be
     * relayed by other mesh nodes.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) &&
        BLE_MESH_ADDR_IS_UNICAST(addr) &&
        bt_mesh_friend_match(net_idx, addr)) {
        BT_INFO("UseFrndSecCred");
        tx->ctx->send_cred = BLE_MESH_FRIENDSHIP_CRED;
        tx->ctx->send_tag = send_tag | BLE_MESH_TAG_IMMUTABLE_CRED;
        return;
    }

    /* Spec 3.7.3.1
     * The Low power node in friendship should use friendship security
     * material.
     *
     * But in Spec 3.6.6.2
     * Depending on the value of the Publish Friendship Credentials Flag
     * (see Section 4.2.3.4), the Low Power node model publishes messages
     * using either the friendship security credentials or the managed
     * flooding security credentials (see Section 3.9.6.3.1).
     *
     * So use the BLE_MESH_TAG_IMMUTABLE_CRED to indicate that the
     * credentials of the message should not be changed when the
     * message is sent by model publishing, even though the spec
     * didn't require this flag to be set when model publishing.
    */

#if CONFIG_BLE_MESH_LOW_POWER
    if (BLE_MESH_ADDR_IS_UNICAST(addr) &&
        bt_mesh.lpn.frnd == addr &&
        !bt_mesh_tag_immutable_cred(send_tag)) {
        BT_INFO("UseFrndSecCred");
        tx->ctx->send_cred = BLE_MESH_FRIENDSHIP_CRED;
        tx->ctx->send_tag = send_tag | BLE_MESH_TAG_IMMUTABLE_CRED;
        return;
    }
#endif /* CONFIG_BLE_MESH_LOW_POWER */
}
#endif /* !CONFIG_BLE_MESH_V11_SUPPORT */

static int model_send(const struct bt_mesh_model *model,
                      struct bt_mesh_net_tx *tx, bool implicit_bind,
                      struct net_buf_simple *msg,
                      const struct bt_mesh_send_cb *cb, void *cb_data)
{
    int err = 0;

    BT_INFO("ModelSend");
    BT_INFO("AppIdx 0x%04x Src 0x%04x Dst 0x%04x TTL %u",
            tx->ctx->app_idx, tx->src, tx->ctx->addr, tx->ctx->send_ttl);
    BT_INFO("Len %u: %s", msg->len, bt_hex(msg->data, msg->len));

    if (ready_to_send(tx->ctx->addr) == false) {
        BT_ERR("Not ready to send");
        return -EINVAL;
    }

    if (tx->ctx->send_ttl != BLE_MESH_TTL_DEFAULT &&
        tx->ctx->send_ttl > BLE_MESH_TTL_MAX) {
        BT_ERR("Too big send TTL 0x%02x", tx->ctx->send_ttl);
        return -EINVAL;
    }

    if (net_buf_simple_tailroom(msg) < BLE_MESH_MIC_SHORT) {
        BT_ERR("Not enough tailroom for TransMIC");
        return -EINVAL;
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    if (msg->len > MIN(BLE_MESH_EXT_TX_SDU_MAX, BLE_MESH_EXT_SDU_MAX_LEN) - BLE_MESH_MIC_SHORT) {
        BT_ERR("Too big ext message (len %d)", msg->len);
        return -EMSGSIZE;
    }

    if ((msg->len <= MIN(BLE_MESH_EXT_TX_SDU_MAX, BLE_MESH_EXT_SDU_MAX_LEN) - BLE_MESH_MIC_SHORT) &&
        (msg->len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SDU_MAX_LEN) - BLE_MESH_MIC_SHORT) &&
        !tx->ctx->enh.long_pkt_cfg_used) {
        BT_ERR("Extended message length %d requires long packet mode, but long_pkt_cfg_used is false", msg->len);
        return -EMSGSIZE;
    }
#else
    if (msg->len > MIN(BLE_MESH_TX_SDU_MAX, BLE_MESH_SDU_MAX_LEN) - BLE_MESH_MIC_SHORT) {
        BT_ERR("Too big message (len %d)", msg->len);
        return -EMSGSIZE;
    }
#endif

    if (!implicit_bind && !model_has_key(model, tx->ctx->app_idx)) {
        BT_ERR("Model not bound to AppKey 0x%04x", tx->ctx->app_idx);
        return -EINVAL;
    }

    if (bt_mesh_valid_security_cred(tx) == false) {
        BT_ERR("Can not use security cred 0x%02x", tx->ctx->send_cred);
        return -EIO;
    }

    bt_mesh_choose_better_security_cred(tx);

    err = bt_mesh_trans_send(tx, msg, cb, cb_data);

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_is_directed_path_needed(tx);
#endif /* CONFIG_BLE_MESH_DF_SRV */

    return err;
}

int bt_mesh_model_send_implicit(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,bool implicit_bind,
                                struct net_buf_simple *msg,
                                const struct bt_mesh_send_cb *cb, void *cb_data)
{
    struct bt_mesh_subnet *sub = NULL;

    BT_DBG("ModelSendImplicit");

    sub = bt_mesh_subnet_get(ctx->net_idx);
    if (!sub) {
        BT_ERR("NetIdx 0x%04x not found", ctx->net_idx);
        return -EADDRNOTAVAIL;
    }

    struct bt_mesh_net_tx tx = {
        .sub  = sub,
        .ctx  = ctx,
        .src  = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };

    return model_send(model, &tx, implicit_bind, msg, cb, cb_data);
}

int bt_mesh_model_send(const struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *msg,
                       const struct bt_mesh_send_cb *cb, void *cb_data)
{
    struct bt_mesh_subnet *sub = NULL;

    BT_DBG("ModelSend, NetIdx 0x%04x", ctx->net_idx);

    sub = bt_mesh_subnet_get(ctx->net_idx);
    if (!sub) {
        BT_ERR("NetIdx 0x%04x not found", ctx->net_idx);
        return -EADDRNOTAVAIL;
    }

    struct bt_mesh_net_tx tx = {
        .sub  = sub,
        .ctx  = ctx,
        .src  = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };

    return model_send(model, &tx, false, msg, cb, cb_data);
}

int bt_mesh_model_publish(struct bt_mesh_model *model)
{
    struct bt_mesh_model_pub *pub = model->pub;
    struct bt_mesh_app_key *key = NULL;
    struct net_buf_simple *sdu = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    struct bt_mesh_net_tx tx = {
        .ctx  = &ctx,
        .src  = bt_mesh_model_elem(model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };
    int err = 0;

    BT_DBG("ModelPublish");

    if (!pub || !pub->msg) {
        BT_ERR("Model has no publication support");
        return -ENOTSUP;
    }

    if (pub->addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_WARN("Unassigned publish address");
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

    key = bt_mesh_app_key_get(pub->key);
    if (!key) {
        BT_ERR("Publish, AppKey 0x%03x not found", pub->key);
        return -EADDRNOTAVAIL;
    }

    tx.sub = bt_mesh_subnet_get(key->net_idx);
    if (!tx.sub) {
        BT_ERR("Publish, NetKey 0x%04x not found", ctx.net_idx);
        return -EADDRNOTAVAIL;
    }

    ctx.addr = pub->addr;
    ctx.net_idx = key->net_idx;
    ctx.app_idx = key->app_idx;
    ctx.send_ttl = pub->ttl;
    ctx.send_cred = pub->cred ? BLE_MESH_FRIENDSHIP_CRED : BLE_MESH_FLOODING_CRED;
    ctx.send_szmic = pub->send_szmic;
#if 0
    ctx.send_tag |= BLE_MESH_TAG_IMMUTABLE_CRED;
#endif
    if (pub->send_rel) {
        /* Tag with send-segmented */
        ctx.send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
    }

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_model_pub_use_directed(&tx, pub->directed_pub_policy);
#endif /* CONFIG_BLE_MESH_DF_SRV */

    pub->count = BLE_MESH_PUB_TRANSMIT_COUNT(pub->retransmit);

    BT_INFO("PubCount %u PubInterval %u",
            pub->count, BLE_MESH_PUB_TRANSMIT_INT(pub->retransmit));

    sdu = bt_mesh_alloc_buf(pub->msg->len + BLE_MESH_MIC_LONG);
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

    BT_DBG("ModelFindVnd, ID 0x%04x CID 0x%04x", id, company);

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

    BT_DBG("ModelFind, ID 0x%04x", id);

    for (i = 0; i < elem->model_count; i++) {
        if (elem->models[i].id == id) {
            return &elem->models[i];
        }
    }

    return NULL;
}

const struct bt_mesh_comp *bt_mesh_comp_get(void)
{
    BT_DBG("CompGet %p", comp_0);

    return comp_0;
}

const uint8_t *bt_mesh_dev_key_get(uint16_t dst)
{
    const uint8_t *key = NULL;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        if (!IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV)) {
            key = bt_mesh.dev_key;
        } else {
            key = bt_mesh_fast_prov_dev_key_get(dst);
        }
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
        key = bt_mesh_provisioner_dev_key_get(dst);
    }

    BT_DBG("Dst 0x%04x DevKey %s", dst, key ? bt_hex(key, 16) : "");

    return key;
}

size_t bt_mesh_rx_netkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = ARRAY_SIZE(bt_mesh.sub);
    }
#endif /* CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER */

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = ARRAY_SIZE(bt_mesh.p_sub);
    }
#endif /* !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = ARRAY_SIZE(bt_mesh.sub);
    if (bt_mesh_is_provisioner_en()) {
        size += ARRAY_SIZE(bt_mesh.p_sub);
    }
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

    BT_DBG("RxNetKeySize %u", size);

    return size;
}

struct bt_mesh_subnet *bt_mesh_rx_netkey_get(size_t index)
{
    struct bt_mesh_subnet *sub = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        sub = &bt_mesh.sub[index];
    }
#endif /* CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER */

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        sub = bt_mesh.p_sub[index];
    }
#endif /* !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < ARRAY_SIZE(bt_mesh.sub)) {
        sub = &bt_mesh.sub[index];
    } else {
        sub = bt_mesh.p_sub[index - ARRAY_SIZE(bt_mesh.sub)];
    }
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

    BT_DBG("RxNetKeyGet, Index %u NetIdx 0x%04x",
           index, sub ? sub->net_idx : BLE_MESH_KEY_ANY);

    return sub;
}

size_t bt_mesh_rx_devkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = 1;
#if CONFIG_BLE_MESH_RPR_SRV
        if (bt_mesh_dev_key_ca_valid()) {
            size += 1;
        }
#endif /* CONFIG_BLE_MESH_RPR_SRV */
    }
#endif /* CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER */

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = 1;
    }
#endif /* !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = 1;
#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_dev_key_ca_valid()) {
        size += 1;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */
    if (bt_mesh_is_provisioner_en()) {
        size += 1;
    }
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

    BT_DBG("RxDevKeySize %u", size);

    return size;
}

const uint8_t *bt_mesh_rx_devkey_get(size_t index, uint16_t src)
{
    const uint8_t *key = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        if (index == 0) {
            key = bt_mesh.dev_key;
        } else if (index == 1) {
            key = bt_mesh.dev_key_ca;
        }
    }
#endif /* CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER */

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        key = bt_mesh_provisioner_dev_key_get(src);
    }
#endif /* !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index == 0) {
        key = bt_mesh.dev_key;
    } else
#if CONFIG_BLE_MESH_RPR_SRV
    if (index == 1 && bt_mesh_dev_key_ca_valid()) {
        /* If index == 1, there are two cases.
         *  1. bt_mesh_dev_key_ca_valid() is true, it should be return bt_mesh.dev_key_ca.
         *  2. bt_mesh_is_provisioner_en() is true, it should be return bt_mesh_provisioner_dev_key_get(src).
         *
         * If index == 2, that means bt_mesh_dev_key_ca_valid() and bt_mesh_is_provisioner_en() are true.
         * So the previous round of function bt_mesh_rx_devkey_get(1, src) will return bt_mesh.dev_key_ca.
         * Then this round of function bt_mesh_rx_devkey_get(2, src) will return bt_mesh_provisioner_dev_key_get(src).
         */
        key = bt_mesh.dev_key_ca;
    } else
#endif /* CONFIG_BLE_MESH_RPR_SRV */
    {
        key = bt_mesh_provisioner_dev_key_get(src);
    }
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

    BT_DBG("RxDevKeyGet, Index %u Src 0x%04x", index, src);

    return key;
}

size_t bt_mesh_rx_appkey_size(void)
{
    size_t size = 0U;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        size = ARRAY_SIZE(bt_mesh.app_keys);
    }
#endif /* CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER */

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        size = ARRAY_SIZE(bt_mesh.p_app_keys);
    }
#endif /* !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    size = ARRAY_SIZE(bt_mesh.app_keys);
    if (bt_mesh_is_provisioner_en()) {
        size += ARRAY_SIZE(bt_mesh.p_app_keys);
    }
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

    BT_DBG("RxAppKeySize %u", size);

    return size;
}

struct bt_mesh_app_key *bt_mesh_rx_appkey_get(size_t index)
{
    struct bt_mesh_app_key *key = NULL;

#if CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioned()) {
        key = &bt_mesh.app_keys[index];
    }
#endif /* CONFIG_BLE_MESH_NODE && !CONFIG_BLE_MESH_PROVISIONER */

#if !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner_en()) {
        key = bt_mesh.p_app_keys[index];
    }
#endif /* !CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER
    if (index < ARRAY_SIZE(bt_mesh.app_keys)) {
        key = &bt_mesh.app_keys[index];
    } else {
        key = bt_mesh.p_app_keys[index - ARRAY_SIZE(bt_mesh.app_keys)];
    }
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PROVISIONER */

    BT_DBG("RxAppKeyGet, Index %u AppIdx 0x%04x",
           index, key ? key->app_idx : BLE_MESH_KEY_ANY);

    return key;
}

struct bt_mesh_app_key *bt_mesh_app_key_get(uint16_t app_idx)
{
    BT_DBG("AppKeyGet, AppIdx 0x%04x", app_idx);

    if (bt_mesh_is_provisioned()) {
#if CONFIG_BLE_MESH_NODE
        if (!IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV)) {
            for (int i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
                if (bt_mesh.app_keys[i].net_idx != BLE_MESH_KEY_UNUSED &&
                    bt_mesh.app_keys[i].app_idx == app_idx) {
                    BT_DBG("NodeAppKey");
                    return &bt_mesh.app_keys[i];
                }
            }
        } else {
            BT_DBG("FastProvAppKey");
            return bt_mesh_fast_prov_app_key_find(app_idx);
        }
#endif /* CONFIG_BLE_MESH_NODE */
    } else if (bt_mesh_is_provisioner_en()) {
#if CONFIG_BLE_MESH_PROVISIONER
        for (int i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
            if (bt_mesh.p_app_keys[i] &&
                bt_mesh.p_app_keys[i]->net_idx != BLE_MESH_KEY_UNUSED &&
                bt_mesh.p_app_keys[i]->app_idx == app_idx) {
                BT_DBG("PvnrAppKey");
                return bt_mesh.p_app_keys[i];
            }
        }
#endif /* CONFIG_BLE_MESH_PROVISIONER */
    }

    return NULL;
}

int bt_mesh_upper_key_get(const struct bt_mesh_subnet *subnet, uint16_t app_idx,
                          const uint8_t **key, uint8_t *aid, uint16_t dst)
{
    struct bt_mesh_app_key *app_key = NULL;

    BT_DBG("UpperKeyGet, AppIdx 0x%04x Dst 0x%04x", app_idx, dst);

    if (app_idx == BLE_MESH_KEY_DEV) {
        *key = bt_mesh_dev_key_get(dst);
        if (!*key) {
            BT_ERR("DevKeyNotFound 0x%04x", dst);
            return -EINVAL;
        }

        *aid = 0U;
        return 0;
    }

    if (!subnet) {
        BT_ERR("InvalidSubnet");
        return -EINVAL;
    }

    app_key = bt_mesh_app_key_get(app_idx);
    if (!app_key) {
        BT_ERR("AppKeyNotFound 0x%04x", app_idx);
        return -ENOENT;
    }

    if (subnet->kr_phase == BLE_MESH_KR_PHASE_2 && app_key->updated) {
        BT_DBG("NewAppKey");
        *key = app_key->keys[1].val;
        *aid = app_key->keys[1].id;
    } else {
        BT_DBG("OldAppKey");
        *key = app_key->keys[0].val;
        *aid = app_key->keys[0].id;
    }

    return 0;
}
