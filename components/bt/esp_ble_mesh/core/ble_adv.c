/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "ble_adv.h"
#include "mesh/common.h"
#include "mesh/buf.h"

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV

static struct bt_mesh_adv_queue ble_adv_queue;
#define BLE_MESH_BLE_ADV_QUEUE_SIZE (CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT + 1)
/* length + advertising data + length + scan response data */
NET_BUF_POOL_DEFINE(ble_adv_buf_pool, CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT,
                    ((BLE_MESH_ADV_DATA_SIZE + 3) << 1), BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv ble_adv_pool[CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT];

static struct bt_mesh_ble_adv_tx ble_adv_tx[CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT];

#define SEND_BLE_ADV_INFINITE    0xFFFF

static void bt_mesh_ble_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front);

static struct bt_mesh_adv *ble_adv_alloc(int id)
{
    return &ble_adv_pool[id];
}

static void bt_mesh_ble_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front)
{
    BT_DBG("%s", __func__);

    if (ble_adv_queue.q.handle == NULL) {
        BT_ERR("Invalid adv queue");
        return;
    }

    if (front) {
        if (xQueueSendToFront(ble_adv_queue.q.handle, msg, timeout) != pdTRUE) {
            BT_ERR("Failed to send item to adv queue front");
            bt_mesh_unref_buf(msg);
        }
    } else {
        if (xQueueSend(ble_adv_queue.q.handle, msg, timeout) != pdTRUE) {
            BT_ERR("Failed to send item to adv queue back");
            bt_mesh_unref_buf(msg);
        }
    }
}

static struct net_buf *bt_mesh_ble_adv_create(enum bt_mesh_adv_type type, int32_t timeout)
{
    return bt_mesh_adv_create_from_pool(type, timeout);
}

inline void bt_mesh_ble_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                                 void *cb_data, bool front)
{
    bt_mesh_generic_adv_send(buf, 0, cb, cb_data, BLE_MESH_ADDR_UNASSIGNED, BLE_MESH_ADDR_UNASSIGNED, front);
}

static void ble_adv_tx_reset(struct bt_mesh_ble_adv_tx *tx, bool unref)
{
    if (tx->buf == NULL) {
        return;
    }

    if (bt_mesh_atomic_test_bit(tx->flags, TIMER_INIT)) {
        k_delayed_work_free(&tx->resend);
    }
    bt_mesh_atomic_set(tx->flags, 0);
    memset(&tx->param, 0, sizeof(tx->param));
    bt_mesh_atomic_set(&BLE_MESH_ADV_BUSY(tx->buf), 0);
    if (unref) {
        net_buf_unref(tx->buf);
    }
    tx->buf = NULL;
}

static void ble_adv_send_start(uint16_t duration, int err, void *cb_data)
{
    struct bt_mesh_ble_adv_tx *tx = cb_data;

    BT_DBG("%s, duration %d, err %d", __func__, duration, err);

    /* If failed to send BLE adv packet, and param->count is not 0
     * which means the timer has been initialized, here we need to
     * free the timer.
     */
    if (err) {
        ble_adv_tx_reset(tx, true);
    }
}

static void ble_adv_send_end(int err, void *cb_data)
{
    struct bt_mesh_ble_adv_tx *tx = cb_data;

    BT_DBG("%s, err %d", __func__, err);

    if (err) {
        ble_adv_tx_reset(tx, true);
        return;
    }

    if (tx->param.count) {
        if (tx->param.period) {
            k_delayed_work_submit(&tx->resend, tx->param.period);
        } else {
            k_work_submit(&tx->resend.work);
        }
    } else {
        ble_adv_tx_reset(tx, true);
    }
}

static struct bt_mesh_send_cb ble_adv_send_cb = {
    .start = ble_adv_send_start,
    .end = ble_adv_send_end,
};

static void ble_adv_resend(struct k_work *work)
{
    struct bt_mesh_ble_adv_tx *tx = CONTAINER_OF(work, struct bt_mesh_ble_adv_tx, resend.work);
    bool front = false;

    if (tx->buf == NULL) {
        /* The advertising has been cancelled */
        return;
    }

    front = (tx->param.priority == BLE_MESH_BLE_ADV_PRIO_HIGH) ? true : false;
    bt_mesh_ble_adv_send(tx->buf, &ble_adv_send_cb, tx, front);

    if (tx->param.count == SEND_BLE_ADV_INFINITE) {
        /* Send the BLE advertising packet infinitely */
        return;
    }

    if (tx->param.count > 0U) {
        tx->param.count--;
    }
}

int bt_mesh_start_ble_advertising(const struct bt_mesh_ble_adv_param *param,
                                  const struct bt_mesh_ble_adv_data *data, uint8_t *index)
{
    struct bt_mesh_ble_adv_tx *tx = NULL;
    struct net_buf *buf = NULL;
    bool front = false;

    if (param == NULL || index == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (param->adv_type != BLE_MESH_ADV_DIRECT_IND &&
        (param->interval < 0x20 || param->interval > 0x4000)) {
        BT_ERR("Invalid adv interval 0x%04x", param->interval);
        return -EINVAL;
    }

    if (param->adv_type > BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) {
        BT_ERR("Invalid adv type 0x%02x", param->adv_type);
        return -EINVAL;
    }

    if (param->own_addr_type > BLE_MESH_ADDR_RANDOM_ID) {
        BT_ERR("Invalid own addr type 0x%02x", param->own_addr_type);
        return -EINVAL;
    }

    if ((param->own_addr_type == BLE_MESH_ADDR_PUBLIC_ID ||
        param->own_addr_type == BLE_MESH_ADDR_RANDOM_ID ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) &&
        param->peer_addr_type > BLE_MESH_ADDR_RANDOM) {
        BT_ERR("Invalid peer addr type 0x%02x", param->peer_addr_type);
        return -EINVAL;
    }

    if (data && (data->adv_data_len > 31 || data->scan_rsp_data_len > 31)) {
        BT_ERR("Invalid adv data length (adv %d, scan rsp %d)",
                data->adv_data_len, data->scan_rsp_data_len);
        return -EINVAL;
    }

    if (param->priority > BLE_MESH_BLE_ADV_PRIO_HIGH) {
        BT_ERR("Invalid adv priority %d", param->priority);
        return -EINVAL;
    }

    if (param->duration < ADV_SCAN_INT(param->interval)) {
        BT_ERR("Too small duration %dms", param->duration);
        return -EINVAL;
    }

    buf = bt_mesh_ble_adv_create(BLE_MESH_ADV_BLE, K_NO_WAIT);
    if (!buf) {
        BT_ERR("No empty ble adv buffer");
        return -ENOBUFS;
    }

    /* Set advertising data and scan response data */
    memset(buf->data, 0, buf->size);
    if (data) {
        net_buf_add_u8(buf, data->adv_data_len);
        if (data->adv_data_len) {
            net_buf_add_mem(buf, data->adv_data, data->adv_data_len);
        }
        net_buf_add_u8(buf, data->scan_rsp_data_len);
        if (data->scan_rsp_data_len) {
            net_buf_add_mem(buf, data->scan_rsp_data, data->scan_rsp_data_len);
        }
    }

    *index = net_buf_id(buf);
    tx = &ble_adv_tx[*index];
    tx->buf = buf;
    memcpy(&tx->param, param, sizeof(tx->param));

    front = (tx->param.priority == BLE_MESH_BLE_ADV_PRIO_HIGH) ? true : false;
    bt_mesh_ble_adv_send(buf, &ble_adv_send_cb, tx, front);
    if (param->count) {
        if (k_delayed_work_init(&tx->resend, ble_adv_resend)) {
            /* If failed to create a timer, the BLE adv packet will be
             * sent only once. Just give a warning here, and since the
             * BLE adv packet can be sent, return 0 here.
             */
            BT_WARN("Send BLE adv packet only once");
            tx->param.count = 0;
            net_buf_unref(buf);
            return 0;
        }
        bt_mesh_atomic_set_bit(tx->flags, TIMER_INIT);
    } else {
        /* Send the BLE advertising packet only once */
        net_buf_unref(buf);
    }

    return 0;
}

int bt_mesh_stop_ble_advertising(uint8_t index)
{
    struct bt_mesh_ble_adv_tx *tx = NULL;
    bool unref = true;

    if (index >= ARRAY_SIZE(ble_adv_tx)) {
        BT_ERR("Invalid adv index %d", index);
        return -EINVAL;
    }

    tx = &ble_adv_tx[index];

    if (tx->buf == NULL) {
        BT_WARN("Already stopped, index %d", index);
        return 0;
    }

    /* busy 1, ref 1; busy 1, ref 2;
     * busy 0, ref 0; busy 0, ref 1;
     */

    if (bt_mesh_atomic_get(&BLE_MESH_ADV_BUSY(tx->buf)) &&
        tx->buf->ref == 1U) {
        unref = false;
    }
    ble_adv_tx_reset(tx, unref);

    return 0;
}

void bt_mesh_ble_adv_init(void)
{
    bt_mesh_adv_queue_init(&ble_adv_queue, CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT, bt_mesh_ble_task_post);
    bt_mesh_adv_type_init(BLE_MESH_ADV_BLE, &ble_adv_queue, &ble_adv_buf_pool, ble_adv_alloc);
#if CONFIG_BLE_MESH_USE_BLE_50
#if CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE
    bt_mesh_adv_inst_init(BLE_MESH_BLE_ADV_INS, CONFIG_BLE_MESH_BLE_ADV_INST_ID);
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_BLE_ADV_INS, BLE_MESH_ADV_BLE);
#else
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_ADV_INS, BLE_MESH_ADV_BLE);
#endif
#endif /* CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_ble_adv_deinit(void)
{
    for (int i = 0; i < ARRAY_SIZE(ble_adv_tx); i++) {
        struct bt_mesh_ble_adv_tx *tx = &ble_adv_tx[i];
        ble_adv_tx_reset(tx, false);
    }
    bt_mesh_unref_buf_from_pool(&ble_adv_buf_pool);
    memset(ble_adv_pool, 0, sizeof(ble_adv_pool));

    bt_mesh_adv_queue_deinit(&ble_adv_queue);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_BLE);
#if CONFIG_BLE_MESH_USE_BLE_50
#if CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE
    bt_mesh_adv_inst_deinit(BLE_MESH_BLE_ADV_INS);
    bt_mesh_adv_inst_supported_adv_type_rm(BLE_MESH_BLE_ADV_INS, BLE_MESH_ADV_BLE);
#else
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_supported_adv_type_rm(BLE_MESH_ADV_INS, BLE_MESH_ADV_BLE);
#endif
#endif /* CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
}
#endif /* CONFIG_BLE_MESH_DEINIT */
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */
