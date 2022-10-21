/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "mesh.h"
#include "mesh_config.h"
#include "access.h"
#include "mesh_common.h"
#include "generic_server.h"
#include "lighting_server.h"

#if CONFIG_BLE_MESH_SERVER_MODEL

/**
 * According to Mesh Model Spec:
 * If the Transition Time field is not present and the Generic Default Transition
 * Time state is supported, the Generic Default Transition Time state shall be
 * used. Otherwise the transition shall be instantaneous.
 */
#define INSTANTANEOUS_TRANS_TIME      0

uint8_t bt_mesh_get_default_trans_time(struct bt_mesh_model *model)
{
    /**
     * 1. If a Generic Default Transition Time Server model is present on the
     *    main element of the model, that model instance shall be used.
     * 2. If a Generic Default Transition Time Server model is not present on
     *    the main element of the model, then the Generic Default Transition
     *    Time Server model instance that is present on the element with the
     *    largest address that is smaller than the address of the main element
     *    of the node shall be used; if no model instance is present on any
     *    element with an address smaller than the address of the main element,
     *    then the Generic Default Transition Time Server is not supported.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    struct bt_mesh_gen_def_trans_time_srv *state = NULL;
    uint16_t primary_addr = bt_mesh_primary_addr();
    struct bt_mesh_model *srv = NULL;

    for (uint16_t addr = element->addr; addr >= primary_addr; addr--) {
        element = bt_mesh_elem_find(addr);
        if (element) {
            srv = bt_mesh_model_find(element, BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV);
            if (srv) {
                state = (struct bt_mesh_gen_def_trans_time_srv *)srv->user_data;
                if (state) {
                    return state->state.trans_time;
                }
            }
        }
    }

    return INSTANTANEOUS_TRANS_TIME;
}

int bt_mesh_get_light_lc_trans_time(struct bt_mesh_model *model, uint8_t *trans_time)
{
    struct bt_mesh_light_lc_srv *srv = NULL;
    uint32_t value = 0U;

    if (model == NULL || trans_time == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (model->id != BLE_MESH_MODEL_ID_LIGHT_LC_SRV) {
        BT_ERR("Invalid a Light LC Server 0x%04x", model->id);
        return -EINVAL;
    }

    srv = (struct bt_mesh_light_lc_srv *)model->user_data;
    if (srv == NULL) {
        BT_ERR("Invalid Light LC Server user data");
        return -EINVAL;
    }

    /**
     * 1. Set transition time to 0x54 for BQB test case MESH/SR/LLC/BV-04-C.
     *    Light LC Property Set: 0x3C, 0x004E20 -> Light LC Time Run On
     *    Light LC Property Set: 0x37, 0x004E20 -> Light LC Time Fade On
     *    Light LC Property Set: 0x39, 0x004E20 -> Light LC Time Fade Standby Manual
     *
     * 2. Set transition time to 0x0 for BQB test case MESH/SR/LLC/BV-08-C.
     *
     * TODO: Based on Light LC state and choose property property value as the
     * transition time. Currently directly use Light LC Time Run On property value.
     * Unit: Millisecond, range: [0, 16777214(0xFFFFFE)]
     */
    value = srv->lc->prop_state.time_run_on & 0xFFFFFF;

    /**
     * Convert value into Default Transition Time state format.
     * 0b00: 0 ~ 6.2s, 100 millisecond step resolution
     * 0b01: 0 ~ 62s,  1 second step resolution
     * 0b10: 0 ~ 620s, 10 seconds step resolution
     * 0b11: 0 ~ 620m, 10 minutes step resolution
     */
    if (value <= 6200) {
        *trans_time = (0 << 6) | (value / 100);
    } else if (value <= 62000) {
        *trans_time = (1 << 6) | (value / 1000);
    } else if (value <= 620000) {
        *trans_time = (2 << 6) | (value / 10000);
    } else {
        *trans_time = (3 << 6) | (value / 600000);
    }

    return 0;
}

int bt_mesh_server_get_optional(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf,
                                uint8_t *trans_time, uint8_t *delay,
                                bool *optional)
{
    if (model == NULL || buf == NULL || trans_time == NULL ||
            delay == NULL || optional == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (buf->len != 0x00 && buf->len != 0x02) {
        BT_ERR("Invalid optional message length %d", buf->len);
        return -EINVAL;
    }

    /* No optional fields are available */
    if (buf->len == 0x00) {
        if (model->id == BLE_MESH_MODEL_ID_LIGHT_LC_SRV) {
            /**
             * Both messages(i.e. Light LC OnOff Set/Set Unack) may optionally include
             * a Transition Time field indicating the transition time to the target state.
             * If the Transition Time is not included, the Light LC Server shall use
             * its appropriate transition times defined by the Light LC Property states.
             */
            if (bt_mesh_get_light_lc_trans_time(model, trans_time)) {
                BT_ERR("Failed to get Light LC transition time");
                return -EIO;
            }
        } else {
            *trans_time = bt_mesh_get_default_trans_time(model);
        }
        *delay = 0U;
        *optional = false;
        return 0;
    }

    /* Optional fields are available */
    *trans_time = net_buf_simple_pull_u8(buf);
    if ((*trans_time & 0x3F) == 0x3F) {
        BT_ERR("Invalid Transaction Number of Steps 0x3f");
        return -EINVAL;
    }

    *delay = net_buf_simple_pull_u8(buf);
    *optional = true;
    return 0;
}

void bt_mesh_server_alloc_ctx(struct k_work *work)
{
    /**
     * This function is used to allocate memory for storing "struct bt_mesh_msg_ctx"
     * of the received messages, because some server models will callback the "struct
     * bt_mesh_msg_ctx" info to the application layer after a certain delay.
     * Here we use the allocated heap memory to store the "struct bt_mesh_msg_ctx".
     */
    __ASSERT(work, "Invalid parameter");
    if (!work->_reserved) {
        work->_reserved = bt_mesh_calloc(sizeof(struct bt_mesh_msg_ctx));
        __ASSERT(work->_reserved, "Out of memory");
    }
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_server_free_ctx(struct k_work *work)
{
    __ASSERT(work, "Invalid parameter");
    if (work->_reserved) {
        bt_mesh_free(work->_reserved);
        work->_reserved = NULL;
    }
}
#endif /* CONFIG_BLE_MESH_DEINIT */

bool bt_mesh_is_server_recv_last_msg(struct bt_mesh_last_msg_info *last,
                                     uint8_t tid, uint16_t src, uint16_t dst, int64_t *now)
{
    *now = k_uptime_get();

    /* Currently we only compare msg info which dst is set to a unicast address */
    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return false;
    }

    if (last->tid == tid && last->src == src && last->dst == dst &&
            (*now - last->timestamp <= K_SECONDS(6))) {
        return true;
    }

    return false;
}

void bt_mesh_server_update_last_msg(struct bt_mesh_last_msg_info *last,
                                    uint8_t tid, uint16_t src, uint16_t dst, int64_t *now)
{
    /* Currently we only update msg info which dst is set to a unicast address */
    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return;
    }

    last->tid = tid;
    last->src = src;
    last->dst = dst;
    last->timestamp = *now;
    return;
}

struct net_buf_simple *bt_mesh_server_get_pub_msg(struct bt_mesh_model *model, uint16_t msg_len)
{
    struct net_buf_simple *buf = NULL;

    if (model == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return NULL;
    }

    if (model->pub == NULL || model->pub->msg == NULL ||
            model->pub->addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_DBG("No publication support, model id 0x%04x", model->id);
        return NULL;
    }

    buf = model->pub->msg;
    if (buf->size < msg_len) {
        BT_ERR("Too small publication msg size %d, model id 0x%04x",
                buf->size, model->id);
        return NULL;
    }

    return buf;
}

#endif /* CONFIG_BLE_MESH_SERVER_MODEL */
