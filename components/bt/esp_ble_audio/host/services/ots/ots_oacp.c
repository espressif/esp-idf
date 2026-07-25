/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/check.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/crc.h>

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/ots.h>
#include "ots_internal.h"
#include "ots_dir_list_internal.h"
#include "ots_obj_manager_internal.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(LEA_OTS, CONFIG_BT_ISO_LOG_LEVEL);

#define OACP_PROC_TYPE_SIZE 1

#if defined(CONFIG_BT_OTS_OACP_WRITE_SUPPORT)
static ssize_t oacp_write_proc_cb(struct bt_gatt_ots_l2cap *l2cap_ctx,
                                  struct bt_conn *conn, struct net_buf *buf);
#endif

static void oacp_l2cap_closed(struct bt_gatt_ots_l2cap *l2cap_ctx,
                              struct bt_conn *conn)
{
    struct bt_ots *ots;

    ots = CONTAINER_OF(l2cap_ctx, struct bt_ots, l2cap);

    /* Always drop procedure sinks — cur_obj may already be cleared (e.g.
     * deleted while idle after a failed send that left callbacks set). */
    l2cap_ctx->rx_done = NULL;
    l2cap_ctx->tx_done = NULL;

    if (!ots->cur_obj) {
        return;
    }

    ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
}

#if defined(CONFIG_BT_OTS_OACP_CREATE_SUPPORT)
static enum bt_gatt_ots_oacp_res_code oacp_create_proc_validate(
    struct bt_conn *conn,
    struct bt_ots *ots,
    struct bt_gatt_ots_oacp_proc *proc)
{
    char str[BT_UUID_STR_LEN];
    int err;
    struct bt_gatt_ots_object *obj;
    const struct bt_ots_obj_add_param param = {
        .size = proc->create_params.size,
        .type = proc->create_params.type,
    };

    bt_uuid_to_str(&param.type.uuid, str, BT_UUID_STR_LEN);
    LOG_DBG("OtsOacpValCreate[%08x][%s]", param.size, str);

    if (!BT_OTS_OACP_GET_FEAT_CREATE(ots->features.oacp)) {
        LOG_DBG("OtsOacpCreateNotSupp");
        return BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
    }

    if (ots->cur_obj &&
            ots->cur_obj->state.type != BT_GATT_OTS_OBJECT_IDLE_STATE) {
        LOG_DBG("OtsOacpCreateObjLocked");
        return BT_GATT_OTS_OACP_RES_OBJ_LOCKED;
    }

    err = bt_ots_obj_add_internal(ots, conn, &param, &obj);
    if (err) {
        goto exit;
    }

    /* Verify Initialization Metadata */
    if (strlen(obj->metadata.name) > 0) {
        LOG_ERR("OtsOacpObjNameNotEmpty");
        (void)bt_ots_obj_delete(ots, obj->id);
        err = -ECANCELED;
        goto exit;
    }

    if (obj->metadata.size.cur > 0) {
        LOG_ERR("OtsOacpObjCurSizeNotZero");
        (void)bt_ots_obj_delete(ots, obj->id);
        err = -ECANCELED;
        goto exit;
    }

    if (!BT_OTS_OBJ_GET_PROP_WRITE(obj->metadata.props)) {
        LOG_ERR("OtsOacpObjNoWrProp");
        (void)bt_ots_obj_delete(ots, obj->id);
        err = -ECANCELED;
        goto exit;
    }

    ots->cur_obj = obj;
    ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;

    LOG_DBG("OtsOacpCreateComp");

exit:
    switch (err) {
    case 0:
        return BT_GATT_OTS_OACP_RES_SUCCESS;
    case -ENOTSUP:
        return BT_GATT_OTS_OACP_RES_UNSUP_TYPE;
    case -ENOMEM:
        return BT_GATT_OTS_OACP_RES_INSUFF_RES;
    case -EINVAL:
        return BT_GATT_OTS_OACP_RES_INV_PARAM;
    case -ECANCELED:
    default:
        return BT_GATT_OTS_OACP_RES_OPER_FAILED;
    }
}
#endif

#if defined(CONFIG_BT_OTS_OACP_DELETE_SUPPORT)
static enum bt_gatt_ots_oacp_res_code oacp_delete_proc_validate(
    struct bt_conn *conn,
    struct bt_ots *ots,
    struct bt_gatt_ots_oacp_proc *proc)
{
    int err;

    if (!BT_OTS_OACP_GET_FEAT_DELETE(ots->features.oacp)) {
        LOG_DBG("OtsOacpDeleteNotSupp");
        return BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
    }

    if (!ots->cur_obj) {
        LOG_DBG("OtsOacpNoObjSel");
        return BT_GATT_OTS_OACP_RES_INV_OBJ;
    }

    if (!BT_OTS_OBJ_GET_PROP_DELETE(ots->cur_obj->metadata.props)) {
        LOG_DBG("OtsOacpDeleteNotPermitted");
        return BT_GATT_OTS_OACP_RES_NOT_PERMITTED;
    }

    err = bt_ots_obj_delete(ots, ots->cur_obj->id);
    if (err) {
        LOG_ERR("OtsOacpDeleteObjFail[%d]", err);
        goto exit;
    }

    LOG_DBG("OtsOacpDeleteComp");

exit:
    switch (err) {
    case 0:
        return BT_GATT_OTS_OACP_RES_SUCCESS;
    case -EBUSY:
        return BT_GATT_OTS_OACP_RES_OBJ_LOCKED;
    default:
        return BT_GATT_OTS_OACP_RES_OPER_FAILED;
    }
}
#endif

#if defined(CONFIG_BT_OTS_OACP_CHECKSUM_SUPPORT)
static enum bt_gatt_ots_oacp_res_code oacp_checksum_proc_validate(
    struct bt_conn *conn,
    struct bt_ots *ots,
    struct bt_gatt_ots_oacp_proc *proc,
    struct net_buf_simple *resp_param)
{
    struct bt_gatt_ots_oacp_cs_calc_params *params = &proc->cs_calc_params;
    void *obj_data;
    int err;
    uint32_t checksum;

    LOG_DBG("OtsOacpValChecksum[%08x][%08x]", params->offset, params->len);

    if (!ots->cur_obj) {
        return BT_GATT_OTS_OACP_RES_INV_OBJ;
    }

    if (params->offset > ots->cur_obj->metadata.size.cur) {
        return BT_GATT_OTS_OACP_RES_INV_PARAM;
    }

    if ((params->offset + (uint64_t) params->len) > ots->cur_obj->metadata.size.cur) {
        return BT_GATT_OTS_OACP_RES_INV_PARAM;
    }

    if (ots->cur_obj->state.type != BT_GATT_OTS_OBJECT_IDLE_STATE) {
        return BT_GATT_OTS_OACP_RES_OBJ_LOCKED;
    }

    if (ots->cb->obj_cal_checksum) {
        err = ots->cb->obj_cal_checksum(ots, conn, ots->cur_obj->id, params->offset,
                                        params->len, &obj_data);
        if (err != 0) {
            return BT_GATT_OTS_OACP_RES_OPER_FAILED;
        }

        checksum = bt_ots_client_calc_checksum((const uint8_t *)obj_data, params->len);
        net_buf_simple_add_le32(resp_param, checksum);
        LOG_DBG("OtsOacpChecksumCalc[%u][%u][%08x]", params->offset, params->len, checksum);
        return BT_GATT_OTS_OACP_RES_SUCCESS;
    } else {
        return BT_GATT_OTS_OACP_RES_OPER_FAILED;
    }
}
#endif

static enum bt_gatt_ots_oacp_res_code oacp_read_proc_validate(
    struct bt_conn *conn,
    struct bt_ots *ots,
    const struct bt_gatt_ots_oacp_proc *proc)
{
    const struct bt_gatt_ots_oacp_read_params *params = &proc->read_params;

    LOG_DBG("OtsOacpValRd[%08x][%08x]", params->offset, params->len);

    if (!BT_OTS_OACP_GET_FEAT_READ(ots->features.oacp)) {
        LOG_DBG("OtsOacpRdNotSupp");
        return BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
    }

    if (!ots->cur_obj) {
        return BT_GATT_OTS_OACP_RES_INV_OBJ;
    }

    if (!BT_OTS_OBJ_GET_PROP_READ(ots->cur_obj->metadata.props)) {
        return BT_GATT_OTS_OACP_RES_NOT_PERMITTED;
    }

    if (!bt_gatt_ots_l2cap_is_open(&ots->l2cap, conn)) {
        return BT_GATT_OTS_OACP_RES_CHAN_UNAVAIL;
    }

    if ((params->offset + (uint64_t) params->len) >
            ots->cur_obj->metadata.size.cur) {
        return BT_GATT_OTS_OACP_RES_INV_PARAM;
    }

    if (ots->cur_obj->state.type != BT_GATT_OTS_OBJECT_IDLE_STATE) {
        return BT_GATT_OTS_OACP_RES_OBJ_LOCKED;
    }

    ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_READ_OP_STATE;
    ots->cur_obj->state.read_op.sent_len = 0;
    memcpy(&ots->cur_obj->state.read_op.oacp_params, &proc->read_params,
           sizeof(ots->cur_obj->state.read_op.oacp_params));

    LOG_DBG("OtsOacpRdAccepted");

    return BT_GATT_OTS_OACP_RES_SUCCESS;
}

#if defined(CONFIG_BT_OTS_OACP_WRITE_SUPPORT)
static enum bt_gatt_ots_oacp_res_code oacp_write_proc_validate(
    struct bt_conn *conn,
    struct bt_ots *ots,
    struct bt_gatt_ots_oacp_proc *proc)
{
    struct bt_gatt_ots_oacp_write_params *params = &proc->write_params;

    LOG_DBG("OtsOacpValWr[%08x][%08x]", params->offset, params->len);

    if (!ots->cur_obj) {
        return BT_GATT_OTS_OACP_RES_INV_OBJ;
    }

    if (!BT_OTS_OACP_GET_FEAT_WRITE(ots->features.oacp)) {
        LOG_DBG("OtsOacpWrNotSupp");
        return BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
    }

    if (!BT_OTS_OBJ_GET_PROP_WRITE(ots->cur_obj->metadata.props)) {
        return BT_GATT_OTS_OACP_RES_NOT_PERMITTED;
    }

    /* patching is attempted */
    if (params->offset < ots->cur_obj->metadata.size.cur) {
        if (!BT_OTS_OACP_GET_FEAT_PATCH(ots->features.oacp)) {
            return BT_GATT_OTS_OACP_RES_NOT_PERMITTED;
        }
        if (!BT_OTS_OBJ_GET_PROP_PATCH(ots->cur_obj->metadata.props)) {
            return BT_GATT_OTS_OACP_RES_NOT_PERMITTED;
        }
    }

    /* truncation is not supported */
    if (BT_GATT_OTS_OACP_PROC_WRITE_MODE_GET_TRUNC(params->mode)) {
        return BT_GATT_OTS_OACP_RES_NOT_PERMITTED;
    }

    if (!bt_gatt_ots_l2cap_is_open(&ots->l2cap, conn)) {
        return BT_GATT_OTS_OACP_RES_CHAN_UNAVAIL;
    }

    if (BT_GATT_OTS_OACP_PROC_WRITE_MODE_GET_RFU(params->mode)) {
        return BT_GATT_OTS_OACP_RES_INV_PARAM;
    }

    if (params->offset > ots->cur_obj->metadata.size.cur) {
        return BT_GATT_OTS_OACP_RES_INV_PARAM;
    }

    /* Growing the object is allowed (see the size.cur update in
     * oacp_write_proc_cb), but never past what the application allocated. */
    if ((params->offset + (uint64_t) params->len) > ots->cur_obj->metadata.size.alloc) {
        return BT_GATT_OTS_OACP_RES_INV_PARAM;
    }

    if (ots->cur_obj->state.type != BT_GATT_OTS_OBJECT_IDLE_STATE) {
        return BT_GATT_OTS_OACP_RES_OBJ_LOCKED;
    }

    ots->l2cap.rx_done = oacp_write_proc_cb;
    ots->l2cap.closed = oacp_l2cap_closed;
    ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_WRITE_OP_STATE;
    ots->cur_obj->state.write_op.recv_len = 0;
    memcpy(&ots->cur_obj->state.write_op.oacp_params, params,
           sizeof(ots->cur_obj->state.write_op.oacp_params));

    LOG_DBG("OtsOacpWrAccepted");

    return BT_GATT_OTS_OACP_RES_SUCCESS;
}
#endif

static enum bt_gatt_ots_oacp_res_code oacp_proc_validate(
    struct bt_conn *conn,
    struct bt_ots *ots,
    struct bt_gatt_ots_oacp_proc *proc,
    struct net_buf_simple *resp_param)
{
    switch (proc->type) {
    case BT_GATT_OTS_OACP_PROC_READ:
        return oacp_read_proc_validate(conn, ots, proc);
#if defined(CONFIG_BT_OTS_OACP_WRITE_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_WRITE:
        return oacp_write_proc_validate(conn, ots, proc);
#endif
#if defined(CONFIG_BT_OTS_OACP_CREATE_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_CREATE:
        return oacp_create_proc_validate(conn, ots, proc);
#endif
#if defined(CONFIG_BT_OTS_OACP_DELETE_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_DELETE:
        return oacp_delete_proc_validate(conn, ots, proc);
#endif
#if defined(CONFIG_BT_OTS_OACP_CHECKSUM_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_CHECKSUM_CALC:
        return oacp_checksum_proc_validate(conn, ots, proc, resp_param);
#endif
    case BT_GATT_OTS_OACP_PROC_EXECUTE:
    case BT_GATT_OTS_OACP_PROC_ABORT:
    default:
        return BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
    }
};

static int oacp_command_decode(const uint8_t *buf, uint16_t len,
                               struct bt_gatt_ots_oacp_proc *proc)
{
    struct net_buf_simple net_buf;

    if (len < OACP_PROC_TYPE_SIZE) {
        return -ENODATA;
    }

    net_buf_simple_init_with_data(&net_buf, (void *) buf, len);

    proc->type = net_buf_simple_pull_u8(&net_buf);
    switch (proc->type) {
#if defined(CONFIG_BT_OTS_OACP_CREATE_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_CREATE:
        if (net_buf.len < BT_GATT_OTS_OACP_CREATE_GENERIC_PARAMS_SIZE) {
            return -EBADMSG;
        }
        proc->create_params.size = net_buf_simple_pull_le32(&net_buf);
        if (!bt_uuid_create(&proc->create_params.type.uuid, net_buf.data,
                            net_buf.len)) {
            return -EBADMSG;
        }
        net_buf_simple_pull_mem(&net_buf, net_buf.len);

        /* Only 16-bit and 128-bit UUIDs are supported */
        switch (proc->create_params.type.uuid.type) {
        case BT_UUID_TYPE_16:
        case BT_UUID_TYPE_128:
            return 0;
        default:
            break;
        }

        return -EBADMSG;
#endif
#if defined(CONFIG_BT_OTS_OACP_DELETE_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_DELETE:
        if (net_buf.len != 0) {
            return -EBADMSG;
        }

        return 0;
#endif
#if defined(CONFIG_BT_OTS_OACP_CHECKSUM_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_CHECKSUM_CALC:
        if (net_buf.len != BT_GATT_OTS_OACP_CS_CALC_PARAMS_SIZE) {
            return -EBADMSG;
        }
        proc->cs_calc_params.offset =
            net_buf_simple_pull_le32(&net_buf);
        proc->cs_calc_params.len =
            net_buf_simple_pull_le32(&net_buf);

        return 0;
#endif
    case BT_GATT_OTS_OACP_PROC_EXECUTE:
        if (net_buf.len != 0) {
            return -EBADMSG;
        }

        return 0;
    case BT_GATT_OTS_OACP_PROC_READ:
        if (net_buf.len != BT_GATT_OTS_OACP_READ_PARAMS_SIZE) {
            return -EBADMSG;
        }
        proc->read_params.offset =
            net_buf_simple_pull_le32(&net_buf);
        proc->read_params.len =
            net_buf_simple_pull_le32(&net_buf);

        return 0;
#if defined(CONFIG_BT_OTS_OACP_WRITE_SUPPORT)
    case BT_GATT_OTS_OACP_PROC_WRITE:
        if (net_buf.len != BT_GATT_OTS_OACP_WRITE_PARAMS_SIZE) {
            return -EBADMSG;
        }
        proc->write_params.offset =
            net_buf_simple_pull_le32(&net_buf);
        proc->write_params.len =
            net_buf_simple_pull_le32(&net_buf);
        proc->write_params.mode =
            net_buf_simple_pull_u8(&net_buf);

        return 0;
#endif
    case BT_GATT_OTS_OACP_PROC_ABORT:
    default:
        break;
    }

    return -ENOTSUP;
}

static void oacp_read_proc_cb(struct bt_gatt_ots_l2cap *l2cap_ctx,
                              struct bt_conn *conn)
{
    int err;
    void *obj_chunk;
    off_t offset;
    ssize_t len;
    struct bt_ots *ots;
    struct bt_gatt_ots_object_read_op *read_op;

    ots = CONTAINER_OF(l2cap_ctx, struct bt_ots, l2cap);

    if (!ots->cur_obj) {
        LOG_ERR("OtsOacpRdNoCurObj");
        return;
    }

    if (ots->cb->obj_read == NULL &&
            !(IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) && ots->cur_obj->id == OTS_OBJ_ID_DIR_LIST)) {
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
        LOG_ERR("OtsOacpRdNoCb");

        return;
    }

    read_op = &ots->cur_obj->state.read_op;
    offset  = read_op->oacp_params.offset + read_op->sent_len;

    if (read_op->sent_len >= read_op->oacp_params.len) {
        LOG_DBG("OtsOacpRdL2capComp");

        if (read_op->sent_len > read_op->oacp_params.len) {
            LOG_WRN("OtsOacpSentMoreThanReq");
        }

        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;

        if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) &&
                ots->cur_obj->id == OTS_OBJ_ID_DIR_LIST) {
            return;
        }

        ots->cb->obj_read(ots, conn, ots->cur_obj->id, NULL, 0,
                          offset);
        return;
    }

    len = read_op->oacp_params.len - read_op->sent_len;
    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) &&
            ots->cur_obj->id == OTS_OBJ_ID_DIR_LIST) {
        len = bt_ots_dir_list_content_get(ots->dir_list, ots->obj_manager,
                                          &obj_chunk, len, offset);
    } else {
        len = ots->cb->obj_read(ots, conn, ots->cur_obj->id, &obj_chunk,
                                len, offset);
    }

    if (len < 0) {
        LOG_ERR("OtsOacpRdOpFail[%zd]", len);

        bt_gatt_ots_l2cap_disconnect(&ots->l2cap);
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;

        return;
    }

    /* Early EOF while bytes remain: do not L2CAP-send len 0 (tx_done would
     * re-enter with sent_len unchanged → busy loop).
     */
    if (len == 0) {
        LOG_WRN("OtsOacpRdEofEarly[%u][%u]", read_op->sent_len,
                read_op->oacp_params.len);

        bt_gatt_ots_l2cap_disconnect(&ots->l2cap);
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;

        if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) &&
                ots->cur_obj->id == OTS_OBJ_ID_DIR_LIST) {
            return;
        }

        ots->cb->obj_read(ots, conn, ots->cur_obj->id, NULL, 0, offset);
        return;
    }

    ots->l2cap.tx_done = oacp_read_proc_cb;
    ots->l2cap.closed = oacp_l2cap_closed;
    err = bt_gatt_ots_l2cap_send(&ots->l2cap, obj_chunk, len);
    if (err) {
        LOG_WRN("OtsOacpL2capErr[%d]", err);
        bt_gatt_ots_l2cap_disconnect(&ots->l2cap);
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
    } else {
        read_op->sent_len += len;
    }
}

static void oacp_read_proc_execute(struct bt_ots *ots,
                                   struct bt_conn *conn)
{
    struct bt_gatt_ots_oacp_read_params *params;

    if (!ots->cur_obj) {
        LOG_ERR("OtsOacpRdInvCurObj");
        return;
    }

    params = &ots->cur_obj->state.read_op.oacp_params;

    LOG_DBG("OtsOacpExecRd[%08x][%08x]", params->offset, params->len);

    oacp_read_proc_cb(&ots->l2cap, conn);
}

#if defined(CONFIG_BT_OTS_OACP_WRITE_SUPPORT)
static ssize_t oacp_write_proc_cb(struct bt_gatt_ots_l2cap *l2cap_ctx,
                                  struct bt_conn *conn, struct net_buf *buf)
{
    struct bt_gatt_ots_object_write_op *write_op;
    struct bt_ots *ots;
    uint32_t offset;
    size_t rem;
    size_t len;
    ssize_t rc;

    ots = CONTAINER_OF(l2cap_ctx, struct bt_ots, l2cap);

    if (!ots->cur_obj) {
        LOG_ERR("OtsOacpWrInvCurObj");
        return -ENODEV;
    }

    /* Reading state.write_op while another procedure owns the union would
     * confuse recv_len with the read op's fields and underflow len below. */
    if (ots->cur_obj->state.type != BT_GATT_OTS_OBJECT_WRITE_OP_STATE) {
        LOG_ERR("OtsOacpWrInvState[%d]", ots->cur_obj->state.type);
        return -EINVAL;
    }

    if (!ots->cb->obj_write) {
        LOG_ERR("OtsOacpWrNoCb");
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
        l2cap_ctx->rx_done = NULL;
        return -ENODEV;
    }

    write_op = &ots->cur_obj->state.write_op;
    offset = write_op->oacp_params.offset + write_op->recv_len;
    len = buf->len;
    if (write_op->recv_len + len > write_op->oacp_params.len) {
        LOG_WRN("OtsOacpRecvMoreThanInd");
        len = write_op->oacp_params.len - write_op->recv_len;
    }
    rem = write_op->oacp_params.len - (write_op->recv_len + len);

    rc = ots->cb->obj_write(ots, conn, ots->cur_obj->id, buf->data, len,
                            (off_t)offset, rem);

    if (rc < 0) {
        len = 0;

        /*
         * Returning an EINPROGRESS return code results in the write buffer not being
         * released by the l2cap layer. This is an unsupported use case at the moment.
         */
        if (rc == -EINPROGRESS) {
            LOG_ERR("OtsOacpWrCbUnsuppRc[%zd]", rc);
        }

        LOG_ERR("OtsOacpWrOpFail[%zd]", rc);
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
    } else {
        /* Return -EIO as an error if all of data was not written */
        if (rc != len) {
            len = 0;
            rc = -EIO;
            LOG_ERR("OtsOacpWrPartialFail");
            ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
        }
    }

    write_op->recv_len += len;
    if (write_op->recv_len == write_op->oacp_params.len) {
        LOG_DBG("OtsOacpWrL2capComp");
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
    }

    if (offset + len > ots->cur_obj->metadata.size.cur) {
        ots->cur_obj->metadata.size.cur = offset + len;
    }

    /* Back to idle means this write is over (completed or failed above). The
     * CoC stays open, so drop the sink or a later procedure's inbound data
     * would still land here. */
    if (ots->cur_obj->state.type == BT_GATT_OTS_OBJECT_IDLE_STATE) {
        l2cap_ctx->rx_done = NULL;
    }

    return rc;
}
#endif

static void oacp_ind_cb(struct bt_conn *conn,
                        struct bt_gatt_indicate_params *params,
                        uint8_t err)
{
    struct bt_ots *ots = (struct bt_ots *) params->attr->user_data;

    LOG_DBG("OtsOacpRecvIndAck[%04x]", err);

    ots->oacp_ind.ind_in_flight = false;
    ots->oacp_ind.conn = NULL;

    if (!ots->cur_obj) {
        LOG_DBG("OtsOacpNoObjForAck");
        return;
    }

    if (err) {
        /* Client did not ACK the OACP response — do not start L2CAP I/O.
         * Leave READ/WRITE would permanently OBJ_LOCKED. */
        LOG_WRN("OtsOacpIndFail[%02x][%d]", err, ots->cur_obj->state.type);
        if (ots->cur_obj->state.type == BT_GATT_OTS_OBJECT_READ_OP_STATE ||
                ots->cur_obj->state.type == BT_GATT_OTS_OBJECT_WRITE_OP_STATE) {
            ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
            ots->l2cap.rx_done = NULL;
            ots->l2cap.tx_done = NULL;
            (void)bt_gatt_ots_l2cap_disconnect(&ots->l2cap);
        }
        return;
    }

    switch (ots->cur_obj->state.type) {
    case BT_GATT_OTS_OBJECT_READ_OP_STATE:
        oacp_read_proc_execute(ots, conn);
        break;
    case BT_GATT_OTS_OBJECT_WRITE_OP_STATE:
        /* procedure execution is driven by L2CAP socket receive */
        break;
    case BT_GATT_OTS_OBJECT_IDLE_STATE:
        /* procedure is not in progress and was already completed */
        break;
    default:
        LOG_ERR("OtsOacpUnsuppState[%d]", ots->cur_obj->state.type);
        break;
    }
}

static void oacp_ind_send(struct bt_conn *conn,
                          const struct bt_gatt_attr *oacp_attr,
                          struct bt_gatt_ots_oacp_proc oacp_proc,
                          enum bt_gatt_ots_oacp_res_code oacp_status,
                          struct net_buf_simple *resp_param)
{
    struct bt_ots *ots = (struct bt_ots *) oacp_attr->user_data;
    uint8_t *oacp_res = ots->oacp_ind.res;
    uint16_t oacp_res_len = 0;
    int err;

    /* Encode OACP Response */
    oacp_res[oacp_res_len++] = BT_GATT_OTS_OACP_PROC_RESP;
    oacp_res[oacp_res_len++] = oacp_proc.type;
    oacp_res[oacp_res_len++] = oacp_status;

    if (oacp_proc.type == BT_GATT_OTS_OACP_PROC_CHECKSUM_CALC &&
            oacp_status == BT_GATT_OTS_OACP_RES_SUCCESS) {
        sys_put_le32(net_buf_simple_pull_le32(resp_param), (oacp_res + oacp_res_len));
        oacp_res_len += sizeof(uint32_t);
    }

    /* Prepare indication parameters */
    memset(&ots->oacp_ind.params, 0, sizeof(ots->oacp_ind.params));
    memcpy(&ots->oacp_ind.attr, oacp_attr, sizeof(ots->oacp_ind.attr));
    ots->oacp_ind.params.attr = &ots->oacp_ind.attr;
    ots->oacp_ind.params.func = oacp_ind_cb;
    ots->oacp_ind.params.data = oacp_res;
    ots->oacp_ind.params.len  = oacp_res_len;

    LOG_DBG("OtsOacpSendInd[%u]", conn->handle);

    ots->oacp_ind.conn = conn;
    ots->oacp_ind.ind_in_flight = true;
    err = k_work_schedule(&ots->oacp_ind.work, K_NO_WAIT_ASYNC);
    if (err < 0) {
        LOG_ERR("OtsOacpSchIndFail[%u][%d]", conn->handle, err);
        ots->oacp_ind.ind_in_flight = false;
        ots->oacp_ind.conn = NULL;
    }
}

ssize_t bt_gatt_ots_oacp_write(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len,
                               uint16_t offset, uint8_t flags)
{
    enum bt_gatt_ots_oacp_res_code oacp_status;
    int decode_status;
    struct bt_gatt_ots_oacp_proc oacp_proc = {0};
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;
    NET_BUF_SIMPLE_DEFINE(resp_param, sizeof(uint32_t));

    LOG_DBG("OtsOacpGattWr");

    if (!ots->oacp_ind.is_enabled) {
        LOG_WRN("OtsOacpIndNotEnabled");
        return BT_GATT_ERR(BT_ATT_ERR_CCC_IMPROPER_CONF);
    }

    if (offset != 0) {
        LOG_WRN("OtsOacpWrReqInvOft");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if (ots->oacp_ind.ind_in_flight ||
            k_work_is_pending(&ots->oacp_ind.work.work)) {
        LOG_WRN("OtsOacpWrBeforeIndSent");
        return BT_GATT_ERR(BT_ATT_ERR_PROCEDURE_IN_PROGRESS);
    }

    decode_status = oacp_command_decode(buf, len, &oacp_proc);
    switch (decode_status) {
    case 0:
        oacp_status = oacp_proc_validate(conn, ots, &oacp_proc, &resp_param);
        if (oacp_status != BT_GATT_OTS_OACP_RES_SUCCESS) {
            LOG_WRN("OtsOacpWrErrStatus[%02x]", oacp_status);
        }
        break;
    case -ENOTSUP:
        oacp_status = BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
        LOG_WRN("OtsOacpUnsuppProcType[%02x]", oacp_proc.type);
        break;
    case -EBADMSG:
        LOG_WRN("OtsOacpWrReqInvLen[%02x]", oacp_proc.type);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    case -ENODATA:
        LOG_WRN("OtsOacpWrReqNoData");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    default:
        LOG_ERR("OtsOacpDecodeInvRc[%d]", decode_status);
        return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
    }

    oacp_ind_send(conn, attr, oacp_proc, oacp_status, &resp_param);
    return len;
}

void bt_gatt_ots_oacp_cfg_changed(const struct bt_gatt_attr *attr,
                                  uint16_t value)
{
    struct bt_gatt_ots_indicate *oacp_ind =
        CONTAINER_OF((struct bt_gatt_ccc_managed_user_data *) attr->user_data,
                     struct bt_gatt_ots_indicate, ccc);

    LOG_DBG("OtsOacpCccd[%04x]", value);

    oacp_ind->is_enabled = false;
    if (value == BT_GATT_CCC_INDICATE) {
        oacp_ind->is_enabled = true;
    } else {
        LOG_DBG("OtsOacpIndClrOnCcc");
        (void)k_work_cancel_delayable(&oacp_ind->work);
        oacp_ind->ind_in_flight = false;
        oacp_ind->conn = NULL;
    }
}
