/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
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

    if (!ots->cur_obj) {
        return;
    }

    ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
    l2cap_ctx->rx_done = NULL;
    l2cap_ctx->tx_done = NULL;
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
    LOG_DBG("Validating Create procedure with size: 0x%08X and "
            "type: %s", param.size, str);

    if (!BT_OTS_OACP_GET_FEAT_CREATE(ots->features.oacp)) {
        LOG_DBG("Create Procedure is not supported.");
        return BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
    }

    err = bt_ots_obj_add_internal(ots, conn, &param, &obj);
    if (err) {
        goto exit;
    }

    /* Verify Initialization Metadata */
    if (strlen(obj->metadata.name) > 0) {
        LOG_ERR("Object name shall be a zero length string after object creation.");
        (void)bt_ots_obj_delete(ots, obj->id);
        err = -ECANCELED;
        goto exit;
    }

    if (obj->metadata.size.cur > 0) {
        LOG_ERR("Object current size must be 0.");
        (void)bt_ots_obj_delete(ots, obj->id);
        err = -ECANCELED;
        goto exit;
    }

    if (!BT_OTS_OBJ_GET_PROP_WRITE(obj->metadata.props)) {
        LOG_ERR("Created object must have write property.");
        (void)bt_ots_obj_delete(ots, obj->id);
        err = -ECANCELED;
        goto exit;
    }

    ots->cur_obj = obj;
    ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;

    LOG_DBG("Create procedure is complete");

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
        LOG_DBG("Delete Procedure is not supported.");
        return BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
    }

    if (!ots->cur_obj) {
        LOG_DBG("No object is selected.");
        return BT_GATT_OTS_OACP_RES_INV_OBJ;
    }

    if (!BT_OTS_OBJ_GET_PROP_DELETE(ots->cur_obj->metadata.props)) {
        LOG_DBG("Object properties do not permit deletion.");
        return BT_GATT_OTS_OACP_RES_NOT_PERMITTED;
    }

    err = bt_ots_obj_delete(ots, ots->cur_obj->id);
    if (err) {
        LOG_ERR("Deleting object during Delete procedure failed: %d", err);
        goto exit;
    }

    LOG_DBG("Delete procedure is complete");

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

    LOG_DBG("Validating Checksum procedure with offset: 0x%08X and "
            "length: 0x%08X", params->offset, params->len);

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
        LOG_DBG("Calculate from offset %u len %u checksum 0x%08x", params->offset,
                params->len, checksum);
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

    LOG_DBG("Validating Read procedure with offset: 0x%08X and "
            "length: 0x%08X", params->offset, params->len);

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

    LOG_DBG("Read procedure is accepted");

    return BT_GATT_OTS_OACP_RES_SUCCESS;
}

#if defined(CONFIG_BT_OTS_OACP_WRITE_SUPPORT)
static enum bt_gatt_ots_oacp_res_code oacp_write_proc_validate(
    struct bt_conn *conn,
    struct bt_ots *ots,
    struct bt_gatt_ots_oacp_proc *proc)
{
    struct bt_gatt_ots_oacp_write_params *params = &proc->write_params;

    LOG_DBG("Validating Write procedure with offset: 0x%08X and "
            "length: 0x%08X", params->offset, params->len);

    if (!ots->cur_obj) {
        return BT_GATT_OTS_OACP_RES_INV_OBJ;
    }

    if (!BT_OTS_OACP_GET_FEAT_WRITE(ots->features.oacp)) {
        LOG_DBG("Write Procedure is not supported.");
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

    /* append is not supported */
    if ((params->offset + (uint64_t) params->len) > ots->cur_obj->metadata.size.cur) {
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

    LOG_DBG("Write procedure is accepted");

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
        LOG_ERR("OTS Read operation failed: no current object");
        return;
    }

    if (ots->cb->obj_read == NULL &&
            !(IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) && ots->cur_obj->id == OTS_OBJ_ID_DIR_LIST)) {
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
        LOG_ERR("OTS Read operation failed: there is no OTS Read callback");

        return;
    }

    read_op = &ots->cur_obj->state.read_op;
    offset  = read_op->oacp_params.offset + read_op->sent_len;

    if (read_op->sent_len >= read_op->oacp_params.len) {
        LOG_DBG("OACP Read Op over L2CAP is completed");

        if (read_op->sent_len > read_op->oacp_params.len) {
            LOG_WRN("More bytes sent that the client requested");
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
        LOG_ERR("OCAP Read Op failed with error: %zd", len);

        bt_gatt_ots_l2cap_disconnect(&ots->l2cap);
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;

        return;
    }

    ots->l2cap.tx_done = oacp_read_proc_cb;
    ots->l2cap.closed = oacp_l2cap_closed;
    err = bt_gatt_ots_l2cap_send(&ots->l2cap, obj_chunk, len);
    if (err) {
        LOG_ERR("L2CAP CoC error: %d while trying to execute OACP "
                "Read procedure", err);
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
        LOG_ERR("Invalid Current Object on OACP Read procedure");
        return;
    }

    params = &ots->cur_obj->state.read_op.oacp_params;

    LOG_DBG("Executing Read procedure with offset: 0x%08X and "
            "length: 0x%08X", params->offset, params->len);

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
        LOG_ERR("Invalid Current Object on OACP Write procedure");
        return -ENODEV;
    }

    if (!ots->cb->obj_write) {
        LOG_ERR("OTS Write operation failed: "
                "there is no OTS Write callback");
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
        return -ENODEV;
    }

    write_op = &ots->cur_obj->state.write_op;
    offset = write_op->oacp_params.offset + write_op->recv_len;
    len = buf->len;
    if (write_op->recv_len + len > write_op->oacp_params.len) {
        LOG_WRN("More bytes received than the client indicated");
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
            LOG_ERR("Unsupported error code %zd returned by object write callback", rc);
        }

        LOG_ERR("OTS Write operation failed with error: %zd", rc);
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
    } else {
        /* Return -EIO as an error if all of data was not written */
        if (rc != len) {
            len = 0;
            rc = -EIO;
            LOG_ERR("OTS Write operation partially failed");
            ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
        }
    }

    write_op->recv_len += len;
    if (write_op->recv_len == write_op->oacp_params.len) {
        LOG_DBG("OACP Write Op over L2CAP is completed");
        ots->cur_obj->state.type = BT_GATT_OTS_OBJECT_IDLE_STATE;
    }

    if (offset + len > ots->cur_obj->metadata.size.cur) {
        ots->cur_obj->metadata.size.cur = offset + len;
    }

    return rc;
}
#endif

static void oacp_ind_cb(struct bt_conn *conn,
                        struct bt_gatt_indicate_params *params,
                        uint8_t err)
{
    struct bt_ots *ots = (struct bt_ots *) params->attr->user_data;

    LOG_DBG("Received OACP Indication ACK with status: 0x%04X", err);

    if (!ots->cur_obj) {
        LOG_DBG("There is no object associated with this ACK");
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
        LOG_ERR("Unsupported OTS state: %d", ots->cur_obj->state.type);
        break;
    }
}

static void oacp_ind_send(const struct bt_gatt_attr *oacp_attr,
                          struct bt_gatt_ots_oacp_proc oacp_proc,
                          enum bt_gatt_ots_oacp_res_code oacp_status,
                          struct net_buf_simple *resp_param)
{
    struct bt_ots *ots = (struct bt_ots *) oacp_attr->user_data;
    uint8_t *oacp_res = ots->oacp_ind.res;
    uint16_t oacp_res_len = 0;

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

    LOG_DBG("Sending OACP indication");

    k_work_submit(&ots->oacp_ind.work);
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

    LOG_DBG("Object Action Control Point GATT Write Operation");

    if (!ots->oacp_ind.is_enabled) {
        LOG_WRN("OACP indications not enabled");
        return BT_GATT_ERR(BT_ATT_ERR_CCC_IMPROPER_CONF);
    }

    if (offset != 0) {
        LOG_ERR("Invalid offset of OACP Write Request");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if (k_work_is_pending(&ots->oacp_ind.work)) {
        LOG_ERR("OACP Write received before indication sent");
        return BT_GATT_ERR(BT_ATT_ERR_PROCEDURE_IN_PROGRESS);
    }

    decode_status = oacp_command_decode(buf, len, &oacp_proc);
    switch (decode_status) {
    case 0:
        oacp_status = oacp_proc_validate(conn, ots, &oacp_proc, &resp_param);
        if (oacp_status != BT_GATT_OTS_OACP_RES_SUCCESS) {
            LOG_WRN("OACP Write error status: 0x%02X", oacp_status);
        }
        break;
    case -ENOTSUP:
        oacp_status = BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP;
        LOG_WRN("OACP unsupported procedure type: 0x%02X", oacp_proc.type);
        break;
    case -EBADMSG:
        LOG_ERR("Invalid length of OACP Write Request for 0x%02X "
                "Op Code", oacp_proc.type);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    case -ENODATA:
        LOG_ERR("Invalid length of OACP Write Request");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    default:
        LOG_ERR("Invalid return code from oacp_command_decode: %d", decode_status);
        return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
    }

    oacp_ind_send(attr, oacp_proc, oacp_status, &resp_param);
    return len;
}

void bt_gatt_ots_oacp_cfg_changed(const struct bt_gatt_attr *attr,
                                  uint16_t value)
{
    struct bt_gatt_ots_indicate *oacp_ind =
        CONTAINER_OF((struct bt_gatt_ccc_managed_user_data *) attr->user_data,
                     struct bt_gatt_ots_indicate, ccc);

    LOG_DBG("Object Action Control Point CCCD value: 0x%04X", value);

    oacp_ind->is_enabled = false;
    if (value == BT_GATT_CCC_INDICATE) {
        oacp_ind->is_enabled = true;
    }
}
