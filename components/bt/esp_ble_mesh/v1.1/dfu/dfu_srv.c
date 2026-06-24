/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh_v1.1/dfu/dfu.h"
#include "mesh_v1.1/dfu/dfu_srv.h"

#include "dfu.h"
#include "blob.h"
#include "access.h"

#if CONFIG_BLE_MESH_SETTINGS
#include "settings.h"
#endif

#if CONFIG_BLE_MESH_DFU_SRV
#define UPDATE_IDX_NONE 0xff

_Static_assert((DFU_UPDATE_START_MSG_MAXLEN + BLE_MESH_MODEL_OP_LEN(BLE_MESH_DFU_OP_UPDATE_START) +
                BLE_MESH_MIC_SHORT) <= BLE_MESH_RX_SDU_MAX,
               "The Firmware Update Start message does not fit into the maximum incoming SDU size.");

_Static_assert((DFU_UPDATE_INFO_STATUS_MSG_MINLEN +
                BLE_MESH_MODEL_OP_LEN(BLE_MESH_DFU_OP_UPDATE_INFO_STATUS) + BLE_MESH_MIC_SHORT)
               <= BLE_MESH_TX_SDU_MAX,
               "The Firmware Update Info Status message does not fit into the maximum outgoing SDU "
               "size.");

static void store_state(struct bt_mesh_dfu_srv *srv)
{
#if CONFIG_BLE_MESH_SETTINGS
    bt_mesh_model_data_store(srv->mod, false, NULL, &srv->update,
                             sizeof(srv->update));
#endif
}

static void erase_state(struct bt_mesh_dfu_srv *srv)
{
#if CONFIG_BLE_MESH_SETTINGS
    bt_mesh_model_data_store(srv->mod, false, NULL, NULL, 0);
#endif
}

static void xfer_failed(struct bt_mesh_dfu_srv *srv)
{
    if (!bt_mesh_dfu_srv_is_busy(srv) ||
            srv->update.idx >= srv->img_count) {
        return;
    }

    erase_state(srv);

    if (srv->cb->end) {
        srv->cb->end(srv, &srv->imgs[srv->update.idx], false);
    }
}

static enum bt_mesh_dfu_status metadata_check(struct bt_mesh_dfu_srv *srv,
                                              uint8_t idx,
                                              struct net_buf_simple *buf,
                                              enum bt_mesh_dfu_effect *effect)
{
    *effect = BLE_MESH_DFU_EFFECT_NONE;

    if (idx >= srv->img_count) {
        return BLE_MESH_DFU_ERR_FW_IDX;
    }

    if (!srv->cb->check) {
        return BLE_MESH_DFU_SUCCESS;
    }

    if (srv->cb->check(srv, &srv->imgs[idx], buf, effect)) {
        *effect = BLE_MESH_DFU_EFFECT_NONE;
        return BLE_MESH_DFU_ERR_METADATA;
    }

    return BLE_MESH_DFU_SUCCESS;
}

static void apply_rsp_sent(int err, void *cb_params)
{
    struct bt_mesh_dfu_srv *srv = cb_params;

    if (err) {
        /* return phase back to give client one more chance. */
        srv->update.phase = BLE_MESH_DFU_PHASE_VERIFY_OK;
        BT_WARN("Apply response failed, wait for retry (err %d)", err);
        return;
    }

    BT_DBG("");

    if (!srv->cb->apply || srv->update.idx == UPDATE_IDX_NONE) {
        srv->update.phase = BLE_MESH_DFU_PHASE_IDLE;
        store_state(srv);
        BT_DBG("Prerequisites for apply callback are wrong");
        return;
    }

    store_state(srv);

    err = srv->cb->apply(srv, &srv->imgs[srv->update.idx]);
    if (err) {
        srv->update.phase = BLE_MESH_DFU_PHASE_IDLE;
        store_state(srv);
        BT_DBG("Application apply callback failed (err %d)", err);
    }
}

static void apply_rsp_sending(uint16_t duration, int err, void *cb_params)
{
    if (err) {
        apply_rsp_sent(err, cb_params);
    }
}

static void verify(struct bt_mesh_dfu_srv *srv)
{
    srv->update.phase = BLE_MESH_DFU_PHASE_VERIFY;

    if (srv->update.idx >= srv->img_count) {
        bt_mesh_dfu_srv_rejected(srv);
        return;
    }

    if (!srv->cb->end) {
        bt_mesh_dfu_srv_verified(srv);
        return;
    }

    srv->cb->end(srv, &srv->imgs[srv->update.idx], true);
    if (srv->update.phase == BLE_MESH_DFU_PHASE_VERIFY) {
        store_state(srv);
    }
}

static int handle_info_get(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;
    uint8_t idx, limit;

    if (srv->update.phase == BLE_MESH_DFU_PHASE_APPLYING) {
        BT_INFO("Still applying, not responding");
        return -EBUSY;
    }

    idx = net_buf_simple_pull_u8(buf);
    limit = net_buf_simple_pull_u8(buf);

    BT_DBG("from %u (limit: %u)", idx, limit);

    NET_BUF_SIMPLE_DEFINE(rsp, BLE_MESH_TX_SDU_MAX);
    bt_mesh_model_msg_init(&rsp, BLE_MESH_DFU_OP_UPDATE_INFO_STATUS);
    net_buf_simple_add_u8(&rsp, srv->img_count);
    net_buf_simple_add_u8(&rsp, idx);

    for (; idx < srv->img_count && limit > 0; ++idx) {
        uint32_t entry_len;

        if (!srv->imgs[idx].fwid) {
            continue;
        }

        entry_len = 2 + srv->imgs[idx].fwid_len;
        if (srv->imgs[idx].uri) {
            entry_len += strlen(srv->imgs[idx].uri);
        }

        if (net_buf_simple_tailroom(&rsp) + BLE_MESH_MIC_SHORT < entry_len) {
            break;
        }

        net_buf_simple_add_u8(&rsp, srv->imgs[idx].fwid_len);
        net_buf_simple_add_mem(&rsp, srv->imgs[idx].fwid,
                               srv->imgs[idx].fwid_len);

        if (srv->imgs[idx].uri) {
            size_t len = strlen(srv->imgs[idx].uri);

            net_buf_simple_add_u8(&rsp, len);
            net_buf_simple_add_mem(&rsp, srv->imgs[idx].uri, len);
        } else {
            net_buf_simple_add_u8(&rsp, 0);
        }

        limit--;
    }

    if (srv->update.phase != BLE_MESH_DFU_PHASE_IDLE) {
        ctx->send_ttl = srv->update.ttl;
    }

    bt_mesh_model_send(mod, ctx, &rsp, NULL, NULL);

    return 0;
}

static int handle_metadata_check(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;
    enum bt_mesh_dfu_status status;
    enum bt_mesh_dfu_effect effect;
    uint8_t idx;

    BLE_MESH_MODEL_BUF_DEFINE(rsp, BLE_MESH_DFU_OP_UPDATE_METADATA_STATUS, 2);
    bt_mesh_model_msg_init(&rsp, BLE_MESH_DFU_OP_UPDATE_METADATA_STATUS);

    idx = net_buf_simple_pull_u8(buf);
    status = metadata_check(srv, idx, buf, &effect);

    BT_DBG("%u", idx);

    net_buf_simple_add_u8(&rsp, (status & BIT_MASK(3)) | (effect << 3));
    net_buf_simple_add_u8(&rsp, idx);

    if (srv->update.phase != BLE_MESH_DFU_PHASE_IDLE) {
        ctx->send_ttl = srv->update.ttl;
    }

    bt_mesh_model_send(mod, ctx, &rsp, NULL, NULL);

    return 0;
}

static void update_status_rsp(struct bt_mesh_dfu_srv *srv,
                              struct bt_mesh_msg_ctx *ctx,
                              enum bt_mesh_dfu_status status,
                              const struct bt_mesh_send_cb *send_cb)
{
    BLE_MESH_MODEL_BUF_DEFINE(buf, BLE_MESH_DFU_OP_UPDATE_STATUS, 14);
    bt_mesh_model_msg_init(&buf, BLE_MESH_DFU_OP_UPDATE_STATUS);

    net_buf_simple_add_u8(&buf, ((status & BIT_MASK(3)) |
                                 (srv->update.phase << 5)));

    if (srv->update.phase != BLE_MESH_DFU_PHASE_IDLE) {
        net_buf_simple_add_u8(&buf, srv->update.ttl);
        net_buf_simple_add_u8(&buf, srv->update.effect);
        net_buf_simple_add_le16(&buf, srv->update.timeout_base);
        net_buf_simple_add_le64(&buf, srv->blob.state.xfer.id);
        net_buf_simple_add_u8(&buf, srv->update.idx);

        ctx->send_ttl = srv->update.ttl;
    }

    bt_mesh_model_send(srv->mod, ctx, &buf, send_cb, srv);
}

static int handle_get(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;

    BT_DBG("");

    update_status_rsp(srv, ctx, BLE_MESH_DFU_SUCCESS, NULL);

    return 0;
}

static inline bool is_active_update(struct bt_mesh_dfu_srv *srv, uint8_t idx,
                                    uint16_t timeout_base,
                                    const uint64_t *blob_id, uint8_t ttl,
                                    uint16_t meta_checksum)
{
    return (srv->update.idx != idx || srv->blob.state.xfer.id != *blob_id ||
            srv->update.ttl != ttl ||
            srv->update.timeout_base != timeout_base ||
            srv->update.meta != meta_checksum);
}

static int handle_start(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;
    const struct bt_mesh_blob_io *io;
    uint16_t timeout_base, meta_checksum;
    enum bt_mesh_dfu_status status;
    uint8_t ttl, idx;
    uint64_t blob_id;
    int err;
    struct net_buf_simple_state buf_state;

    ttl = net_buf_simple_pull_u8(buf);
    timeout_base = net_buf_simple_pull_le16(buf);
    blob_id = net_buf_simple_pull_le64(buf);
    idx = net_buf_simple_pull_u8(buf);
    meta_checksum = dfu_metadata_checksum(buf);

    BT_DBG("%u ttl: %u extra time: %u", idx, ttl, timeout_base);

    if ((!buf->len || meta_checksum == srv->update.meta) &&
            srv->update.phase == BLE_MESH_DFU_PHASE_TRANSFER_ERR &&
            srv->update.ttl == ttl &&
            srv->update.timeout_base == timeout_base &&
            srv->update.idx == idx &&
            srv->blob.state.xfer.id == blob_id) {
        srv->update.phase = BLE_MESH_DFU_PHASE_TRANSFER_ACTIVE;
        status = BLE_MESH_DFU_SUCCESS;
        store_state(srv);
        /* blob srv will resume the transfer. */
        BT_DBG("Resuming transfer");
        goto rsp;
    }

    if (bt_mesh_dfu_srv_is_busy(srv)) {
        if (is_active_update(srv, idx, timeout_base, &blob_id, ttl,
                             meta_checksum)) {
            status = BLE_MESH_DFU_ERR_WRONG_PHASE;
        } else {
            status = BLE_MESH_DFU_SUCCESS;
            srv->update.ttl = ttl;
            srv->blob.state.xfer.id = blob_id;
        }

        BT_WARN("Busy. Phase: %u", srv->update.phase);
        goto rsp;
    }

    net_buf_simple_save(buf, &buf_state);
    status = metadata_check(srv, idx, buf,
                            (enum bt_mesh_dfu_effect *)&srv->update.effect);
    net_buf_simple_restore(buf, &buf_state);
    if (status != BLE_MESH_DFU_SUCCESS) {
        goto rsp;
    }

    srv->update.ttl = ttl;
    srv->update.timeout_base = timeout_base;
    srv->update.meta = meta_checksum;

    io = NULL;
    err = srv->cb->start(srv, &srv->imgs[idx], buf, &io);
    if (err == -EALREADY || (!err && bt_mesh_has_addr(ctx->addr))) {
        /* This image has already been received or this is a
         * self-update. Skip the transfer phase and proceed to
         * verifying update.
         */
        status = BLE_MESH_DFU_SUCCESS;
        srv->update.idx = idx;
        srv->blob.state.xfer.id = blob_id;
        srv->update.phase = BLE_MESH_DFU_PHASE_VERIFY;
        update_status_rsp(srv, ctx, status, NULL);
        verify(srv);
        return 0;
    }

    if (err == -ENOMEM) {
        status = BLE_MESH_DFU_ERR_RESOURCES;
        goto rsp;
    }

    if (err == -EBUSY) {
        status = BLE_MESH_DFU_ERR_TEMPORARILY_UNAVAILABLE;
        goto rsp;
    }

    if (err || !io || !io->wr) {
        status = BLE_MESH_DFU_ERR_INTERNAL;
        goto rsp;
    }

    err = bt_mesh_blob_srv_recv(&srv->blob, blob_id, io,
                                ttl, timeout_base);
    if (err) {
        status = BLE_MESH_DFU_ERR_BLOB_XFER_BUSY;
        goto rsp;
    }

    srv->update.idx = idx;
    srv->update.phase = BLE_MESH_DFU_PHASE_TRANSFER_ACTIVE;
    status = BLE_MESH_DFU_SUCCESS;
    store_state(srv);

rsp:
    update_status_rsp(srv, ctx, status, NULL);

    return 0;
}

static int handle_cancel(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;

    if (srv->update.idx == UPDATE_IDX_NONE) {
        goto rsp;
    }

    BT_DBG("");

    bt_mesh_blob_srv_cancel(&srv->blob);
    srv->update.phase = BLE_MESH_DFU_PHASE_IDLE;
    xfer_failed(srv);

rsp:
    update_status_rsp(srv, ctx, BLE_MESH_DFU_SUCCESS, NULL);

    return 0;
}

static int handle_apply(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                        struct net_buf_simple *buf)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;
    static const struct bt_mesh_send_cb send_cb = {
        .start = apply_rsp_sending,
        .end = apply_rsp_sent,
    };

    BT_DBG("");

    if (srv->update.phase == BLE_MESH_DFU_PHASE_APPLYING) {
        update_status_rsp(srv, ctx, BLE_MESH_DFU_SUCCESS, NULL);
        return 0;
    }

    if (srv->update.phase != BLE_MESH_DFU_PHASE_VERIFY_OK) {
        BT_WARN("Apply: Invalid phase %u", srv->update.phase);
        update_status_rsp(srv, ctx, BLE_MESH_DFU_ERR_WRONG_PHASE, NULL);
        return 0;
    }

    /* Postponing the apply callback until the response has been sent, in
     * case it triggers a reboot:
     */
    srv->update.phase = BLE_MESH_DFU_PHASE_APPLYING;
    update_status_rsp(srv, ctx, BLE_MESH_DFU_SUCCESS, &send_cb);

    return 0;
}

const struct bt_mesh_model_op _bt_mesh_dfu_srv_op[] = {
    { BLE_MESH_DFU_OP_UPDATE_INFO_GET, 2, (void *)handle_info_get },
    { BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK, 1, (void *)handle_metadata_check },
    { BLE_MESH_DFU_OP_UPDATE_GET, 0, (void *)handle_get },
    { BLE_MESH_DFU_OP_UPDATE_START, 12, (void *)handle_start },
    { BLE_MESH_DFU_OP_UPDATE_CANCEL, 0, (void *)handle_cancel },
    { BLE_MESH_DFU_OP_UPDATE_APPLY, 0, (void *)handle_apply },
    BLE_MESH_MODEL_OP_END,
};

static int dfu_srv_init(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;

    srv->mod = mod;
    srv->update.idx = UPDATE_IDX_NONE;
    srv->blob.cb = &_bt_mesh_dfu_srv_blob_cb;

    if (!srv->cb || !srv->cb->start || !srv->imgs || srv->img_count == 0 ||
            srv->img_count == UPDATE_IDX_NONE) {
        BT_ERR("Invalid DFU Server initialization");
        return -EINVAL;
    }

    return 0;
}

__attribute__((unused))
static int dfu_srv_settings_set(const struct bt_mesh_model *mod, const char *name,
                                size_t len_rd, settings_read_cb read_cb,
                                void *cb_arg)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;
    ssize_t len;

    if (len_rd < sizeof(srv->update)) {
        return -EINVAL;
    }

    len = read_cb(cb_arg, &srv->update, sizeof(srv->update));
    if (len < 0) {
        return len;
    }

    BT_DBG("Recovered transfer (phase: %u, idx: %u)", srv->update.phase,
           srv->update.idx);
    if (srv->update.phase == BLE_MESH_DFU_PHASE_TRANSFER_ACTIVE) {
        BT_DBG("Settings recovered mid-transfer, setting transfer error");
        srv->update.phase = BLE_MESH_DFU_PHASE_TRANSFER_ERR;
    } else if (srv->update.phase == BLE_MESH_DFU_PHASE_VERIFY_OK) {
        BT_DBG("Settings recovered before application, setting verification fail");
        srv->update.phase = BLE_MESH_DFU_PHASE_VERIFY_FAIL;
    }

    return 0;
}

static void dfu_srv_reset(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfu_srv *srv = mod->user_data;

    srv->update.phase = BLE_MESH_DFU_PHASE_IDLE;
    erase_state(srv);
}

static int dfu_srv_deinit(struct bt_mesh_model *mod)
{
    dfu_srv_reset(mod);
    return 0;
}

const struct bt_mesh_model_cb _bt_mesh_dfu_srv_cb = {
    .init = dfu_srv_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = dfu_srv_deinit,
#endif
};

static void blob_suspended(struct bt_mesh_blob_srv *b)
{
    struct bt_mesh_dfu_srv *srv = CONTAINER_OF(b, struct bt_mesh_dfu_srv, blob);

    srv->update.phase = BLE_MESH_DFU_PHASE_TRANSFER_ERR;
    store_state(srv);
}

static void blob_end(struct bt_mesh_blob_srv *b, uint64_t id, bool success)
{
    struct bt_mesh_dfu_srv *srv =
        CONTAINER_OF(b, struct bt_mesh_dfu_srv, blob);

    BT_DBG("success: %u", success);

    if (!success) {
        /**
         * Changed by Espressif,
         * The xfer_failed must be executed before updating the state;
         * otherwise, the end_cb inside xfer_failed will never be delivered.
         */
        xfer_failed(srv);
        srv->update.phase = BLE_MESH_DFU_PHASE_TRANSFER_ERR;
        return;
    }

    verify(srv);
}

static int blob_recover(struct bt_mesh_blob_srv *b,
                        struct bt_mesh_blob_xfer *xfer,
                        const struct bt_mesh_blob_io **io)
{
    struct bt_mesh_dfu_srv *srv =
        CONTAINER_OF(b, struct bt_mesh_dfu_srv, blob);

    if (!srv->cb->recover ||
            srv->update.phase != BLE_MESH_DFU_PHASE_TRANSFER_ERR ||
            srv->update.idx >= srv->img_count) {
        return -ENOTSUP;
    }

    return srv->cb->recover(srv, &srv->imgs[srv->update.idx], io);
}

const struct bt_mesh_blob_srv_cb _bt_mesh_dfu_srv_blob_cb = {
    .suspended = blob_suspended,
    .end = blob_end,
    .recover = blob_recover,
};

void bt_mesh_dfu_srv_verified(struct bt_mesh_dfu_srv *srv)
{
    if (srv->update.phase != BLE_MESH_DFU_PHASE_VERIFY) {
        BT_WARN("Wrong state");
        return;
    }

    BT_DBG("");

    srv->update.phase = BLE_MESH_DFU_PHASE_VERIFY_OK;
    store_state(srv);
}

void bt_mesh_dfu_srv_rejected(struct bt_mesh_dfu_srv *srv)
{
    if (srv->update.phase != BLE_MESH_DFU_PHASE_VERIFY) {
        BT_WARN("Wrong state");
        return;
    }

    BT_DBG("");

    srv->update.phase = BLE_MESH_DFU_PHASE_VERIFY_FAIL;
    store_state(srv);
}

void bt_mesh_dfu_srv_cancel(struct bt_mesh_dfu_srv *srv)
{
    if (srv->update.phase == BLE_MESH_DFU_PHASE_IDLE) {
        BT_WARN("Wrong state");
        return;
    }

    (void)bt_mesh_blob_srv_cancel(&srv->blob);
}

void bt_mesh_dfu_srv_applied(struct bt_mesh_dfu_srv *srv)
{
    if (srv->update.phase != BLE_MESH_DFU_PHASE_APPLYING) {
        BT_WARN("Wrong state");
        return;
    }

    BT_DBG("");

    srv->update.phase = BLE_MESH_DFU_PHASE_IDLE;
    store_state(srv);
}

bool bt_mesh_dfu_srv_is_busy(const struct bt_mesh_dfu_srv *srv)
{
    return srv->update.phase != BLE_MESH_DFU_PHASE_IDLE &&
           srv->update.phase != BLE_MESH_DFU_PHASE_TRANSFER_ERR &&
           srv->update.phase != BLE_MESH_DFU_PHASE_VERIFY_FAIL;
}

uint8_t bt_mesh_dfu_srv_progress(const struct bt_mesh_dfu_srv *srv)
{
    if (!bt_mesh_dfu_srv_is_busy(srv)) {
        return 0U;
    }

    if (srv->update.phase == BLE_MESH_DFU_PHASE_TRANSFER_ACTIVE) {
        return bt_mesh_blob_srv_progress(&srv->blob);
    }

    return 100U;
}
#endif /* CONFIG_BLE_MESH_DFU_SRV */
