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
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/ots.h>
#include "ots_internal.h"
#include "ots_obj_manager_internal.h"
#include "ots_dir_list_internal.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(LEA_OTS, CONFIG_BT_ISO_LOG_LEVEL);

#define OLCP_PROC_TYPE_SIZE 1

static enum bt_gatt_ots_olcp_res_code obj_manager_to_olcp_err_map(int err)
{
    switch (-err) {
    case EINVAL:
        return BT_GATT_OTS_OLCP_RES_OBJECT_ID_NOT_FOUND;
    case ENFILE:
        return BT_GATT_OTS_OLCP_RES_OUT_OF_BONDS;
    case ENOENT:
    default:
        return BT_GATT_OTS_OLCP_RES_NO_OBJECT;
    }
}

static enum bt_gatt_ots_olcp_res_code olcp_first_proc_execute(
    struct bt_ots *ots)
{
    int err;
    struct bt_gatt_ots_object *first_obj;

    err = bt_gatt_ots_obj_manager_first_obj_get(ots->obj_manager,
                                                &first_obj);
    if (err) {
        return obj_manager_to_olcp_err_map(err);
    }

    ots->cur_obj = first_obj;

    return BT_GATT_OTS_OLCP_RES_SUCCESS;
}

static enum bt_gatt_ots_olcp_res_code olcp_last_proc_execute(
    struct bt_ots *ots)
{
    int err;
    struct bt_gatt_ots_object *last_obj;

    err = bt_gatt_ots_obj_manager_last_obj_get(ots->obj_manager,
                                               &last_obj);
    if (err) {
        return obj_manager_to_olcp_err_map(err);
    }

    ots->cur_obj = last_obj;

    return BT_GATT_OTS_OLCP_RES_SUCCESS;
}

static enum bt_gatt_ots_olcp_res_code olcp_prev_proc_execute(
    struct bt_ots *ots)
{
    int err;
    struct bt_gatt_ots_object *prev_obj;

    if (!ots->cur_obj) {
        return BT_GATT_OTS_OLCP_RES_OPERATION_FAILED;
    }
    err = bt_gatt_ots_obj_manager_prev_obj_get(ots->obj_manager,
                                               ots->cur_obj,
                                               &prev_obj);
    if (err) {
        return obj_manager_to_olcp_err_map(err);
    }

    ots->cur_obj = prev_obj;

    return BT_GATT_OTS_OLCP_RES_SUCCESS;
}

static enum bt_gatt_ots_olcp_res_code olcp_next_proc_execute(
    struct bt_ots *ots)
{
    int err;
    struct bt_gatt_ots_object *next_obj;

    if (!ots->cur_obj) {
        return BT_GATT_OTS_OLCP_RES_OPERATION_FAILED;
    }
    err = bt_gatt_ots_obj_manager_next_obj_get(ots->obj_manager,
                                               ots->cur_obj,
                                               &next_obj);
    if (err) {
        return obj_manager_to_olcp_err_map(err);
    }

    ots->cur_obj = next_obj;

    return BT_GATT_OTS_OLCP_RES_SUCCESS;
}

static enum bt_gatt_ots_olcp_res_code olcp_goto_proc_execute(
    struct bt_ots *ots, uint64_t id)
{
    int err;
    struct bt_gatt_ots_object *id_obj;

    if (!BT_OTS_VALID_OBJ_ID(id)) {
        LOG_DBG("Invalid object ID 0x%016llx", id);

        return BT_GATT_OTS_OLCP_RES_INVALID_PARAMETER;
    }

    err = bt_gatt_ots_obj_manager_obj_get(ots->obj_manager,
                                          id,
                                          &id_obj);
    if (err) {
        return obj_manager_to_olcp_err_map(err);
    }

    ots->cur_obj = id_obj;

    return BT_GATT_OTS_OLCP_RES_SUCCESS;
}

static enum bt_gatt_ots_olcp_res_code olcp_proc_execute(
    struct bt_ots *ots, struct bt_gatt_ots_olcp_proc *proc)
{
    LOG_DBG("Executing OLCP procedure with 0x%02X Op Code", proc->type);

    switch (proc->type) {
    case BT_GATT_OTS_OLCP_PROC_FIRST:
        return olcp_first_proc_execute(ots);
    case BT_GATT_OTS_OLCP_PROC_LAST:
        return olcp_last_proc_execute(ots);
    case BT_GATT_OTS_OLCP_PROC_PREV:
        return olcp_prev_proc_execute(ots);
    case BT_GATT_OTS_OLCP_PROC_NEXT:
        return olcp_next_proc_execute(ots);
    case BT_GATT_OTS_OLCP_PROC_GOTO:
        return olcp_goto_proc_execute(ots, proc->goto_params.id);
    case BT_GATT_OTS_OLCP_PROC_ORDER:
    case BT_GATT_OTS_OLCP_PROC_REQ_NUM_OBJS:
    case BT_GATT_OTS_OLCP_PROC_CLEAR_MARKING:
    default:
        return BT_GATT_OTS_OLCP_RES_PROC_NOT_SUP;
    }
};

static int olcp_command_decode(const uint8_t *buf, uint16_t len,
                               struct bt_gatt_ots_olcp_proc *proc)
{
    if (len < OLCP_PROC_TYPE_SIZE) {
        return -ENODATA;
    }

    memset(proc, 0, sizeof(*proc));

    proc->type = *buf++;
    len -= OLCP_PROC_TYPE_SIZE;

    switch (proc->type) {
    case BT_GATT_OTS_OLCP_PROC_FIRST:
    case BT_GATT_OTS_OLCP_PROC_LAST:
    case BT_GATT_OTS_OLCP_PROC_PREV:
    case BT_GATT_OTS_OLCP_PROC_NEXT:
        if (len != 0) {
            return -EBADMSG;
        }

        return 0;
    case BT_GATT_OTS_OLCP_PROC_GOTO:
        if (len != BT_GATT_OTS_OLCP_GOTO_PARAMS_SIZE) {
            return -EBADMSG;
        }
        proc->goto_params.id = sys_get_le48(buf);

        return 0;
    default:
        break;
    }

    return -ENOTSUP;
}

static void olcp_ind_cb(struct bt_conn *conn,
                        struct bt_gatt_indicate_params *params,
                        uint8_t err)
{
    LOG_DBG("Received OLCP Indication ACK with status: 0x%04X", err);
}

static void olcp_ind_send(const struct bt_gatt_attr *olcp_attr,
                          enum bt_gatt_ots_olcp_proc_type req_op_code,
                          enum bt_gatt_ots_olcp_res_code olcp_status)
{
    struct bt_ots *ots = (struct bt_ots *) olcp_attr->user_data;
    uint8_t *olcp_res = ots->olcp_ind.res;
    uint16_t olcp_res_len = 0;

    /* Encode OLCP Response */
    olcp_res[olcp_res_len++] = BT_GATT_OTS_OLCP_PROC_RESP;
    olcp_res[olcp_res_len++] = req_op_code;
    olcp_res[olcp_res_len++] = olcp_status;

    /* Prepare indication parameters */
    memset(&ots->olcp_ind.params, 0, sizeof(ots->olcp_ind.params));
    memcpy(&ots->olcp_ind.attr, olcp_attr, sizeof(ots->olcp_ind.attr));
    ots->olcp_ind.params.attr = olcp_attr;
    ots->olcp_ind.params.func = olcp_ind_cb;
    ots->olcp_ind.params.data = olcp_res;
    ots->olcp_ind.params.len  = olcp_res_len;
#if defined(CONFIG_BT_EATT)
    ots->olcp_ind.params.chan_opt = BT_ATT_CHAN_OPT_NONE;
#endif /* CONFIG_BT_EATT */

    LOG_DBG("Sending OLCP indication");

    k_work_submit(&ots->olcp_ind.work);
}

ssize_t bt_gatt_ots_olcp_write(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len,
                               uint16_t offset, uint8_t flags)
{
    struct bt_gatt_ots_object *old_obj;
    enum bt_gatt_ots_olcp_res_code olcp_status;
    int decode_status;
    struct bt_gatt_ots_olcp_proc olcp_proc;
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;

    LOG_DBG("Object List Control Point GATT Write Operation");

    if (!ots->olcp_ind.is_enabled) {
        LOG_WRN("OLCP indications not enabled");
        return BT_GATT_ERR(BT_ATT_ERR_CCC_IMPROPER_CONF);
    }

    if (offset != 0) {
        LOG_ERR("Invalid offset of OLCP Write Request");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if (k_work_is_pending(&ots->olcp_ind.work)) {
        LOG_ERR("OLCP Write received before indication sent");
        return BT_GATT_ERR(BT_ATT_ERR_PROCEDURE_IN_PROGRESS);
    }

    old_obj = ots->cur_obj;

    decode_status = olcp_command_decode(buf, len, &olcp_proc);
    switch (decode_status) {
    case 0:
        olcp_status = olcp_proc_execute(ots, &olcp_proc);
        if (olcp_status != BT_GATT_OTS_OLCP_RES_SUCCESS) {
            LOG_WRN("OLCP Write error status: 0x%02X", olcp_status);
        } else if (old_obj != ots->cur_obj) {
            char id[BT_OTS_OBJ_ID_STR_LEN];

            bt_ots_obj_id_to_str(ots->cur_obj->id, id,
                                 sizeof(id));
            LOG_DBG("Selecting a new Current Object with id: %s",
                    id);

            if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ)) {
                bt_ots_dir_list_selected(ots->dir_list, ots->obj_manager,
                                         ots->cur_obj);
            }

            if (ots->cb->obj_selected) {
                ots->cb->obj_selected(ots, conn, ots->cur_obj->id);
            }
        }
        break;
    case -ENOTSUP:
        olcp_status = BT_GATT_OTS_OLCP_RES_PROC_NOT_SUP;
        LOG_WRN("OLCP unsupported procedure type: 0x%02X", olcp_proc.type);
        break;
    case -EBADMSG:
        LOG_ERR("Invalid length of OLCP Write Request for 0x%02X "
                "Op Code", olcp_proc.type);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    case -ENODATA:
        LOG_ERR("Invalid size of OLCP Write Request");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    default:
        LOG_ERR("Invalid return code from olcp_command_decode: %d", decode_status);
        return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
    }

    olcp_ind_send(attr, olcp_proc.type, olcp_status);
    return len;
}

void bt_gatt_ots_olcp_cfg_changed(const struct bt_gatt_attr *attr,
                                  uint16_t value)
{
    struct bt_gatt_ots_indicate *olcp_ind =
        CONTAINER_OF((struct bt_gatt_ccc_managed_user_data *) attr->user_data,
                     struct bt_gatt_ots_indicate, ccc);

    LOG_DBG("Object List Control Point CCCD value: 0x%04X", value);

    olcp_ind->is_enabled = false;
    if (value == BT_GATT_CCC_INDICATE) {
        olcp_ind->is_enabled = true;
    }
}
