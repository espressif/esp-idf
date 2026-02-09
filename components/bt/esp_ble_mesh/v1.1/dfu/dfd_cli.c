/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "dfu_slot.h"
#include "dfu.h"
#include "net.h"
#include "transport.h"
#include "mesh_v1.1/dfu/dfd.h"
#include "mesh_v1.1/dfu/dfu_cli.h"
#include "mesh_v1.1/mbt/blob_srv.h"
#include "btc_ble_mesh_dfu_model.h"

#include "mesh_v1.1/dfu/dfd_cli.h"

#if CONFIG_BLE_MESH_DFD_CLI
static const bt_mesh_client_op_pair_t dfd_cli_pair[] = {
    {BLE_MESH_DFD_OP_RECEIVERS_ADD,                 BLE_MESH_DFD_OP_RECEIVERS_STATUS},
    {BLE_MESH_DFD_OP_RECEIVERS_DELETE_ALL,          BLE_MESH_DFD_OP_RECEIVERS_STATUS},
    {BLE_MESH_DFD_OP_RECEIVERS_GET,                 BLE_MESH_DFD_OP_RECEIVERS_LIST},
    {BLE_MESH_DFD_OP_CAPABILITIES_GET,              BLE_MESH_DFD_OP_CAPABILITIES_STATUS},
    {BLE_MESH_DFD_OP_GET,                           BLE_MESH_DFD_OP_STATUS},
    {BLE_MESH_DFD_OP_START,                         BLE_MESH_DFD_OP_STATUS},
    {BLE_MESH_DFD_OP_SUSPEND,                       BLE_MESH_DFD_OP_STATUS},
    {BLE_MESH_DFD_OP_CANCEL,                        BLE_MESH_DFD_OP_STATUS},
    {BLE_MESH_DFD_OP_APPLY,                         BLE_MESH_DFD_OP_STATUS},
    {BLE_MESH_DFD_OP_UPLOAD_GET,                    BLE_MESH_DFD_OP_UPLOAD_STATUS},
    {BLE_MESH_DFD_OP_UPLOAD_START,                  BLE_MESH_DFD_OP_UPLOAD_STATUS},
    {BLE_MESH_DFD_OP_UPLOAD_START_OOB,              BLE_MESH_DFD_OP_UPLOAD_STATUS},
    {BLE_MESH_DFD_OP_UPLOAD_CANCEL,                 BLE_MESH_DFD_OP_UPLOAD_STATUS},
    {BLE_MESH_DFD_OP_FW_GET,                        BLE_MESH_DFD_OP_FW_STATUS},
    {BLE_MESH_DFD_OP_FW_GET_BY_INDEX,               BLE_MESH_DFD_OP_FW_STATUS},
    {BLE_MESH_DFD_OP_FW_DELETE,                     BLE_MESH_DFD_OP_FW_STATUS},
    {BLE_MESH_DFD_OP_FW_DELETE_ALL,                 BLE_MESH_DFD_OP_FW_STATUS},
};

static bt_mesh_mutex_t dfd_client_lock;

static void timeout_handler(struct k_work *work)
{
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;

    BT_WARN("DFDRspTmo");

    bt_mesh_mutex_lock(&dfd_client_lock);

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            bt_mesh_dfd_client_cb_evt_to_btc(BTC_BLE_MESH_EVT_DFD_CLIENT_TIMEOUT, node->model, &node->ctx, NULL, 0);
            bt_mesh_client_free_node(node);
        }
    }

    bt_mesh_mutex_unlock(&dfd_client_lock);

    return;
}

static void dfd_client_recv_status(struct bt_mesh_model *model,
                                    struct bt_mesh_msg_ctx *ctx,
                                    void *status, size_t len)
{
    bt_mesh_client_node_t *node = NULL;
    struct net_buf_simple buf = {0};
    btc_ble_mesh_dfd_client_cb_evt_t evt = 0;

    if (!model || !ctx) {
        BT_ERR("%s,InvParam", __func__);
        return;
    }

    buf.data = (uint8_t *)status;
    buf.len  = (uint16_t)len;

    bt_mesh_mutex_lock(&dfd_client_lock);

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, &buf, true);
    if (!node) {
        BT_DBG("UnexpDfdStus:0x%04x", ctx->recv_op);
    } else {
        switch (ctx->recv_op) {
            case BLE_MESH_DFD_OP_RECEIVERS_STATUS:
            case BLE_MESH_DFD_OP_CAPABILITIES_GET:
            case BLE_MESH_DFD_OP_RECEIVERS_LIST:
            case BLE_MESH_DFD_OP_CAPABILITIES_STATUS:
            case BLE_MESH_DFD_OP_STATUS:
            case BLE_MESH_DFD_OP_UPLOAD_STATUS:
            case BLE_MESH_DFD_OP_FW_STATUS:
                evt = BTC_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP;
            break;
            default:
                BT_ERR("UnkwnOpc:%04x", node->opcode);
            break;
        }

        if (!k_delayed_work_free(&node->timer)) {
            bt_mesh_dfd_client_cb_evt_to_btc(evt, model, ctx, (const uint8_t *)status, len);
            bt_mesh_client_free_node(node);
        }
    }

    bt_mesh_mutex_unlock(&dfd_client_lock);
}

static void handle_receiver_status(struct bt_mesh_model *model,
                                   struct bt_mesh_msg_ctx *ctx,
                                   struct net_buf_simple *buf)
{
    dfd_status_t status = {0};

    status.receiver_status.status = net_buf_simple_pull_u8(buf);

    if (status.receiver_status.status > BLE_MESH_DFD_ERR_SUSPEND_FAILED) {
        BT_ERR("InvSrvStu:%d", status.receiver_status.status);
        return;
    }

    if (status.receiver_status.status != BLE_MESH_DFD_SUCCESS) {
        BT_ERR("StusErr:%d", status);
        return;
    }

    status.receiver_status.receiver_list_cnt = net_buf_simple_pull_le16(buf);

    BT_DBG("RecvLstCnt:%d", status.receiver_status.receiver_list_cnt);

    dfd_client_recv_status(model, ctx, &status, sizeof(status.receiver_status));
}

static void handle_receiver_list(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    int i = 0;
    uint32_t target_info = 0;

    dfd_status_t status = {0};
    status.receiver_list.entries_cnt = net_buf_simple_pull_le16(buf);
    status.receiver_list.first_index = net_buf_simple_pull_le16(buf);
    status.receiver_list.entries = bt_mesh_calloc(status.receiver_list.entries_cnt * sizeof(target_node_entry_t));
    for (i = 0; i < status.receiver_list.entries_cnt; i++) {
        target_info = net_buf_simple_pull_le32(buf);
        status.receiver_list.entries[i].addr = TARGET_ADDR(target_info);
        status.receiver_list.entries[i].retrieved_update_phase = TARGET_UPDATE_PHASE(target_info);
        status.receiver_list.entries[i].update_status = TARGET_UPDATE_STATUS(target_info);
        status.receiver_list.entries[i].transfer_status = TARGET_TRANSFER_STATUS(target_info);
        status.receiver_list.entries[i].transfer_progress = TARGET_TRANSFER_PROGRESS(target_info);
        status.receiver_list.entries[i].update_fw_idx = net_buf_simple_pull_u8(buf);
    }
    dfd_client_recv_status(model, ctx, &status, sizeof(status.receiver_list));
    bt_mesh_free(status.receiver_list.entries);
    return;
}

static void handle_capabilities(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    dfd_status_t status = {0};
    struct net_buf_simple url_scheme_names = {0};

    status.dist_caps.max_receiver_list_sz = net_buf_simple_pull_le16(buf);
    status.dist_caps.max_fw_list_sz = net_buf_simple_pull_le16(buf);
    status.dist_caps.max_fw_sz = net_buf_simple_pull_le32(buf);
    status.dist_caps.max_upload_space = net_buf_simple_pull_le32(buf);
    status.dist_caps.remaining_upload_space = net_buf_simple_pull_le32(buf);
    status.dist_caps.oob_retrieval_supported = net_buf_simple_pull_le32(buf);
    if (buf->len) {
        status.dist_caps.supported_url_scheme_names = &url_scheme_names;
        net_buf_simple_init_with_data(status.dist_caps.supported_url_scheme_names, buf->data, buf->len);
        net_buf_simple_pull_mem(buf, buf->len);
    }
    dfd_client_recv_status(model, ctx, &status, sizeof(status.dist_caps));
    return;
}

static void handle_dfd_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    dfd_status_t status = {0};
    uint8_t trans_mode_policy = 0;

    status.dist_status.status = net_buf_simple_pull_u8(buf);
    status.dist_status.dist_phase = net_buf_simple_pull_u8(buf);
    if (buf->len != 0 && buf->len != 10) {
        BT_ERR("Invalid data");
        return;
    }
    status.dist_status.multicast_address = net_buf_simple_pull_le16(buf);
    status.dist_status.appkey_idx = net_buf_simple_pull_le16(buf);
    status.dist_status.ttl = net_buf_simple_pull_u8(buf);
    status.dist_status.timeout_base = net_buf_simple_pull_le16(buf);

    trans_mode_policy = net_buf_simple_pull_u8(buf);
    if ((trans_mode_policy & ~(BIT6 - 1)) != 0) {
        BT_ERR("RFU should be zero");
        return;
    }
    status.dist_status.trans_mode = trans_mode_policy >> 6;
    status.dist_status.update_policy = (trans_mode_policy >> 5) & 0x01;
    status.dist_status.RFU = trans_mode_policy & 0x1f;
    status.dist_status.fw_idx = net_buf_simple_pull_le16(buf);
    dfd_client_recv_status(model, ctx, &status, sizeof(status.dist_status));
    return;
}

static void handle_upload_status(struct bt_mesh_model *model,
                                 struct bt_mesh_msg_ctx *ctx,
                                 struct net_buf_simple *buf)
{
    dfd_status_t status = {0};
    uint8_t progress_type = 0;
    struct net_buf_simple buf_cache = {0};
    status.upload_status.status = net_buf_simple_pull_u8(buf);
    status.upload_status.upload_phase = net_buf_simple_pull_u8(buf);

    if (buf->len == 0) {
        status.upload_status.upload_progress = UPLOAD_PROGRESS_UNSET;
        dfd_client_recv_status(model, ctx, &status, sizeof(status.upload_status));
        return;
    }

    progress_type = net_buf_simple_pull_u8(buf);
    status.upload_status.upload_progress = progress_type>>1;

    if (status.upload_status.upload_progress >= UPLOAD_PROGRESS_UNSET) {
        BT_ERR("Invalid upload progress");
        return;
    }

    if (buf->len == 0) {
        BT_ERR("InvFwID");
        return;
    }

    status.upload_status.upload_type = progress_type & 0x01;
    if (status.upload_status.upload_type == UPLOAD_IN_BAND) {
        status.upload_status.fwid = &buf_cache;
        net_buf_simple_init_with_data(status.upload_status.fwid, buf->data, buf->len);
        net_buf_simple_pull_mem(buf, buf->len);
    } else if (status.upload_status.upload_type == UPLOAD_OOB){
        status.upload_status.oob_fwid = &buf_cache;
        net_buf_simple_init_with_data(status.upload_status.oob_fwid, buf->data, buf->len);
        net_buf_simple_pull_mem(buf, buf->len);
    } else {
        BT_ERR("Invalid upload type:%d", status.upload_status.upload_type);
        return;
    }

    dfd_client_recv_status(model, ctx, &status, sizeof(status.upload_status));
    return;
}

static void handle_fw_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    dfd_status_t status = {0};
    struct net_buf_simple buf_cache = {0};
    status.firmware_status.status = net_buf_simple_pull_u8(buf);
    status.firmware_status.entry_cnt = net_buf_simple_pull_le16(buf);
    status.firmware_status.firmware_image_index = net_buf_simple_pull_le16(buf);
    if (buf->len) {
        status.firmware_status.fwid = &buf_cache;
        net_buf_simple_init_with_data(status.firmware_status.fwid, buf->data, buf->len);
        net_buf_simple_pull_mem(buf, buf->len);
    }

    dfd_client_recv_status(model, ctx, &status, sizeof(status.firmware_status));
    return;
}

const struct bt_mesh_model_op _bt_mesh_dfd_cli_op[] = {
    { BLE_MESH_DFD_OP_RECEIVERS_STATUS,      3,     handle_receiver_status },
    { BLE_MESH_DFD_OP_RECEIVERS_LIST,        4 ,    handle_receiver_list   },
    { BLE_MESH_DFD_OP_CAPABILITIES_STATUS,   17,    handle_capabilities    },
    { BLE_MESH_DFD_OP_STATUS,                2,     handle_dfd_status      },
    { BLE_MESH_DFD_OP_UPLOAD_STATUS,         2,     handle_upload_status   },
    { BLE_MESH_DFD_OP_FW_STATUS,             5,     handle_fw_status       },
};

int bt_mesh_dfd_cli_receivers_add(bt_mesh_client_common_param_t *param, dfd_cli_receiver_entry_t *receivers, uint16_t receivers_cnt)
{
    uint16_t msg_length = 2;
    struct net_buf_simple *msg = NULL;
    dfd_cli_receiver_entry_t *entry = NULL;
    int err = 0;

    if (param == NULL) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }

    BT_INFO("AddedValidCnt:%d", receivers_cnt);
    msg_length += (receivers_cnt * 3);

    /* needs to confirm long or short mic */
    if (msg_length > BLE_MESH_MAX_PDU_LEN_WITH_SMIC) {
        BT_ERR("Too much receivers added once time");
        return -EINVAL;
    }

    msg = bt_mesh_alloc_buf(msg_length + BLE_MESH_MIC_SHORT);
    if (!msg) {
        BT_ERR("Failed to alloc buffer to send message");
        return -EINVAL;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_DFD_OP_RECEIVERS_ADD);

    for (int i = 0; i < receivers_cnt; i++) {
        entry = &receivers[i];
        if (BLE_MESH_ADDR_IS_UNICAST(entry->addr)) {
            net_buf_simple_add_le16(msg, entry->addr);
            net_buf_simple_add_u8(msg, entry->fw_idx);
            BT_INFO("AddedUnicastAddr:0x%04x,FwIdx:%d", entry->addr, entry->fw_idx);
        }
    }

    err = bt_mesh_client_send_msg(param, msg, true, timeout_handler);
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_dfd_cli_receivers_delete_all(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_RECEIVERS_DELETE_ALL, 0);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_RECEIVERS_DELETE_ALL);
    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_receivers_get(bt_mesh_client_common_param_t *param, uint16_t first_index, uint16_t entries_limit)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_RECEIVERS_GET, 4);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }

    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_RECEIVERS_GET);
    net_buf_simple_add_le16(&msg, first_index);
    net_buf_simple_add_le16(&msg, entries_limit);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_capabilities_get(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_CAPABILITIES_GET, 0);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_CAPABILITIES_GET);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_get(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_GET, 0);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_GET);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_start(bt_mesh_client_common_param_t *param,
                                       dfd_cli_dist_start_t *start)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_START, 2 + 1 + 2 + 1 + 2 + 16);
    if (!param || !start) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }

    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_START);
    net_buf_simple_add_le16(&msg, start->app_idx);
    net_buf_simple_add_u8(&msg, start->ttl);
    net_buf_simple_add_le16(&msg, start->timeout_base);
    net_buf_simple_add_u8(&msg, ((start->trans_mode) << 6) | ((start->update_policy) << 5));
    net_buf_simple_add_le16(&msg, start->fw_idx);

    if (start->is_va) {
        net_buf_simple_add_mem(&msg, &(start->label_uuid), 16);
    } else {
        net_buf_simple_add_le16(&msg, start->group_addr);
    }

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_suspend(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_SUSPEND, 0);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_SUSPEND);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_cancel(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_CANCEL, 0);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_CANCEL);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_apply(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_APPLY, 0);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_APPLY);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_upload_get(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_UPLOAD_GET, 0);
    if (!param) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_UPLOAD_GET);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_distribution_upload_start(bt_mesh_client_common_param_t *param,
                                              dfd_cli_dist_upload_start_t *start)
{
    struct net_buf_simple *msg = NULL;
    uint16_t msg_length = 2;
    int err = 0;

    if (!param || !start) {
        BT_ERR("Invalid param");
        return -EINVAL;
    }

    msg_length += (1 + 2 + 8 + 4 + 1 + start->fwid->len);
    if (start->fw_metadata) {
        msg_length += start->fw_metadata->len;
    }


    msg = bt_mesh_alloc_buf(msg_length + BLE_MESH_MIC_SHORT);
    if (!msg) {
        BT_ERR("Failed to alloc buffer to send message");
        return -EINVAL;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_DFD_OP_UPLOAD_START);
    net_buf_simple_add_u8(msg, start->ttl);
    net_buf_simple_add_le16(msg, start->timeout_base);
    net_buf_simple_add_le64(msg, start->blob_id);
    net_buf_simple_add_le32(msg, start->fw_size);
    if (start->fw_metadata) {
        net_buf_simple_add_u8(msg, start->fw_metadata->len);
        net_buf_simple_add_mem(msg, start->fw_metadata->data, start->fw_metadata->len);
    } else {
        net_buf_simple_add_u8(msg, 0);
    }
    net_buf_simple_add_mem(msg, start->fwid->data, start->fwid->len);

    err = bt_mesh_client_send_msg(param, msg, true, timeout_handler);
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_dfd_cli_distribution_upload_oob_start(bt_mesh_client_common_param_t *param,
                                                  dfd_cli_dist_upload_oob_start_t *start)
{
    int err = 0;
    struct net_buf_simple *msg = NULL;
    uint16_t msg_length = 2;

    if (!param || !start) {
        BT_ERR("Invalid param");
        return -1;
    }

    if (!start->url) {
        BT_ERR("Null url info");
        return -1;
    }

    if (!start->fwid) {
        BT_ERR("Invalid firmware id");
        return -1;
    }

    msg_length += (1 + start->url->len + start->fwid->len);

    msg = bt_mesh_alloc_buf(msg_length + BLE_MESH_MIC_SHORT);
    if (!msg) {
        BT_ERR("Failed to alloc buffer to send message");
        return -EINVAL;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_DFD_OP_UPLOAD_START_OOB);
    net_buf_simple_add_u8(msg, start->url->len);
    net_buf_simple_add_mem(msg, start->url->data, start->url->len);
    net_buf_simple_add_mem(msg, start->fwid->data, start->fwid->len);

    err = bt_mesh_client_send_msg(param, msg, true, timeout_handler);
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_dfd_cli_distribution_upload_oob_cancel(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_UPLOAD_CANCEL, 0);
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_UPLOAD_CANCEL);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_firmware_get(bt_mesh_client_common_param_t *param, struct net_buf_simple *fwid)
{
    struct net_buf_simple *msg = NULL;
    uint16_t msg_length = 2;
    int err;


    if (!fwid) {
        BT_ERR("NULL Firmware id");
        return -EINVAL;
    }

    msg_length += fwid->len;

    msg = bt_mesh_alloc_buf(msg_length + BLE_MESH_MIC_SHORT);
    if (!msg) {
        BT_ERR("Failed to alloc buffer to send message");
        return -EINVAL;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_DFD_OP_FW_GET);
    net_buf_simple_add_mem(msg, fwid->data, fwid->len);

    err = bt_mesh_client_send_msg(param, msg, true, timeout_handler);
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_dfd_cli_firmware_get_by_index(bt_mesh_client_common_param_t *param, uint16_t fw_id_index)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_FW_GET_BY_INDEX, 2);
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_FW_GET_BY_INDEX);

    net_buf_simple_add_le16(&msg, fw_id_index);
    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

int bt_mesh_dfd_cli_firmware_get_delete(bt_mesh_client_common_param_t *param, struct net_buf_simple *fwid)
{
    struct net_buf_simple *msg = NULL;
    uint16_t msg_length = 2;
    int err;


    if (!fwid) {
        BT_ERR("NULL Firmware id");
        return -EINVAL;
    }

    msg_length += fwid->len;

    msg = bt_mesh_alloc_buf(msg_length + BLE_MESH_MIC_SHORT);
    if (!msg) {
        BT_ERR("Failed to alloc buffer to send message");
        return -EINVAL;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_DFD_OP_FW_DELETE);
    net_buf_simple_add_mem(msg, fwid->data, fwid->len);

    err = bt_mesh_client_send_msg(param, msg, true, timeout_handler);
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_dfd_cli_firmware_delete_all(bt_mesh_client_common_param_t *param)
{
    BLE_MESH_MODEL_BUF_DEFINE(msg, BLE_MESH_DFD_OP_FW_DELETE_ALL, 0);
    bt_mesh_model_msg_init(&msg, BLE_MESH_DFD_OP_FW_DELETE_ALL);

    return bt_mesh_client_send_msg(param, &msg, true, timeout_handler);
}

static int dfd_cli_init(struct bt_mesh_model *model)
{
    bt_mesh_dfd_client_t *client = NULL;
    dfd_internal_data_t *internal = NULL;

    if (!model) {
        BT_ERR("Invalid Device Firmware Distribution Client model");
        return -EINVAL;
    }

    if (!bt_mesh_model_in_primary(model)) {
        BT_ERR("Device Firmware Distribution Client only allowed in primary element");
        return -EINVAL;
    }

    client = (bt_mesh_dfd_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Device Firmware Distribution Client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    internal = bt_mesh_calloc(sizeof(dfd_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->op_pair_size = ARRAY_SIZE(dfd_cli_pair);
    client->op_pair = dfd_cli_pair;
    client->internal_data = internal;
    BT_INFO("Dfd client initialized");
    bt_mesh_mutex_create(&dfd_client_lock);

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static int dfd_cli_deinit(struct bt_mesh_model *model)
{
    bt_mesh_dfd_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Device Firmware Distribution Client model");
        return -EINVAL;
    }

    client = (bt_mesh_dfd_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Device Firmware Distribution Client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        /* Remove items from the list */
        bt_mesh_client_clear_list(client->internal_data);

        /* Free the allocated internal data */
        bt_mesh_free(client->internal_data);
        client->internal_data = NULL;
    }

    bt_mesh_mutex_free(&dfd_client_lock);

    return 0;
}
#endif

const struct bt_mesh_model_cb _bt_mesh_dfd_cli_cb = {
    .init = dfd_cli_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = dfd_cli_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};
#endif
