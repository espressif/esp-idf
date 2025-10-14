/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "adv.h"
#include "mesh.h"
#include "mesh/hci.h"
#include "mesh/kernel.h"
#include "mesh/common.h"
#include "beacon.h"
#include "prov_node.h"
#include "foundation.h"
#include "proxy_server.h"
#include "proxy_client.h"
#include "prov_pvnr.h"
#include "mesh/adapter.h"
#include "adv_common.h"
#include "ble_adv.h"

static struct bt_mesh_adv_queue *adv_queue;

#if CONFIG_BLE_MESH_RELAY_ADV_BUF

#define BLE_MESH_RELAY_QUEUE_SIZE   CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT

static QueueSetHandle_t mesh_queue_set;
#define BLE_MESH_QUEUE_SET_SIZE     (BLE_MESH_ADV_QUEUE_SIZE + BLE_MESH_RELAY_QUEUE_SIZE)

#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

static inline void adv_send_start(uint16_t duration, int err,
                                  const struct bt_mesh_send_cb *cb,
                                  void *cb_data)
{
    if (cb && cb->start) {
        cb->start(duration, err, cb_data);
    }
}

static inline void adv_send_end(int err, const struct bt_mesh_send_cb *cb,
                                void *cb_data)
{
    if (cb && cb->end) {
        cb->end(err, cb_data);
    }
}

static inline int adv_send(struct net_buf *buf)
{
    const struct bt_mesh_send_cb *cb = BLE_MESH_ADV(buf)->cb;
    void *cb_data = BLE_MESH_ADV(buf)->cb_data;
    struct bt_mesh_adv_param param = {0};
    uint16_t duration = 0U, adv_int = 0U;
    struct bt_mesh_adv_data ad = {0};
    int err = 0;

    BT_DBG("type %u len %u: %s", BLE_MESH_ADV(buf)->type,
        buf->len, bt_hex(buf->data, buf->len));

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    if (BLE_MESH_ADV(buf)->type != BLE_MESH_ADV_BLE) {
#endif
        adv_int = MAX(ADV_ITVL_MIN,
                      BLE_MESH_TRANSMIT_INT(BLE_MESH_ADV(buf)->xmit));
        duration = (BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1) *
                   (adv_int + 10);

        BT_DBG("count %u interval %ums duration %ums",
               BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1, adv_int,
               duration);

        ad.type = adv_type[BLE_MESH_ADV(buf)->type];
        ad.data_len = buf->len;
        ad.data = buf->data;

        param.options = 0U;
        param.interval_min = ADV_SCAN_UNIT(adv_int);
        param.interval_max = param.interval_min;

#if CONFIG_BLE_MESH_USE_BLE_50
        param.adv_duration = duration;
        param.adv_count = BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1;
#endif

#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
        if (BLE_MESH_ADV(buf)->type == BLE_MESH_ADV_PROXY_SOLIC) {
            bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);
            struct bt_mesh_adv_data solic_ad[2] = {
                BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x59, 0x18),
                BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, buf->data, buf->len),
            };
#if CONFIG_BLE_MESH_USE_BLE_50
            param.primary_phy = BLE_MESH_ADV_PHY_1M;
            param.secondary_phy = BLE_MESH_ADV_PHY_1M;
            err = bt_le_ext_adv_start(CONFIG_BLE_MESH_ADV_INST_ID, &param, solic_ad, ARRAY_SIZE(solic_ad), NULL, 0);
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
            err = bt_le_adv_start(&param, solic_ad, ARRAY_SIZE(solic_ad), NULL, 0);
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
        } else
#endif
        {
            bt_mesh_adv_buf_ref_debug(__func__, buf, 4U, BLE_MESH_BUF_REF_SMALL);
#if CONFIG_BLE_MESH_USE_BLE_50
            param.primary_phy = BLE_MESH_ADV_PHY_1M;
            param.secondary_phy = BLE_MESH_ADV_PHY_1M;
            err = bt_le_ext_adv_start(CONFIG_BLE_MESH_ADV_INST_ID, &param, &ad, 1, NULL, 0);
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
            err = bt_le_adv_start(&param, &ad, 1, NULL, 0);
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
        }
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    } else {
        struct bt_mesh_ble_adv_data data = {0};
        struct bt_mesh_ble_adv_tx *tx = cb_data;

        if (tx == NULL) {
            BT_ERR("Invalid adv user data");
            net_buf_unref(buf);
            return -EINVAL;
        }

        BT_DBG("interval %dms, duration %dms, period %dms, count %d",
            ADV_SCAN_INT(tx->param.interval), tx->param.duration,
            tx->param.period, tx->param.count);

        data.adv_data_len = tx->buf->data[0];
        if (data.adv_data_len) {
            memcpy(data.adv_data, tx->buf->data + 1, data.adv_data_len);
        }
        data.scan_rsp_data_len = tx->buf->data[data.adv_data_len + 1];
        if (data.scan_rsp_data_len) {
            memcpy(data.scan_rsp_data, tx->buf->data + data.adv_data_len + 2, data.scan_rsp_data_len);
        }
        duration = tx->param.duration;

        bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);

#if CONFIG_BLE_MESH_USE_BLE_50
        err = bt_mesh_ble_ext_adv_start(CONFIG_BLE_MESH_ADV_INST_ID, &tx->param, &data);
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
        err = bt_mesh_ble_adv_start(&tx->param, &data);
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
    }
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

    net_buf_unref(buf);
    adv_send_start(duration, err, cb, cb_data);
    if (err) {
        BT_ERR("Start advertising failed: err %d", err);
        return err;
    }

    BT_DBG("Advertising started. Sleeping %u ms", duration);

#if CONFIG_BLE_MESH_USE_BLE_50
    if (!ble_mesh_adv_task_wait(UINT32_MAX, K_FOREVER, NULL)) {
        BT_WARN("Advertising didn't finish on time");
        bt_le_ext_adv_stop(CONFIG_BLE_MESH_ADV_INST_ID);
    }
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
    ble_mesh_adv_task_wait(K_MSEC(duration));
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

#if !CONFIG_BLE_MESH_USE_BLE_50
    err = bt_le_adv_stop();
#endif

    adv_send_end(err, cb, cb_data);
    if (err) {
        BT_ERR("Stop advertising failed: err %d", err);
        return 0;
    }

    BT_DBG("Advertising stopped");
    return 0;
}

static void adv_thread(void *p)
{
#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    QueueSetMemberHandle_t handle = NULL;
    QueueHandle_t relay_adv_handle =
        bt_mesh_adv_types_mgnt_get(BLE_MESH_ADV_RELAY_DATA)->adv_q->q.handle;
#endif
    bt_mesh_msg_t msg = {0};
    struct net_buf **buf = NULL;

    buf = (struct net_buf **)(&msg.arg);

    BT_DBG("%s, starts", __func__);

    while (1) {
        *buf = NULL;
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
        xQueueReceive(adv_queue->q.handle, &msg, K_NO_WAIT);
        while (!(*buf)) {
            int32_t timeout = 0;
            BT_DBG("Mesh Proxy Advertising start");
            timeout = bt_mesh_proxy_server_adv_start();
            BT_DBG("Mesh Proxy Advertising up to %d ms", timeout);
            xQueueReceive(adv_queue->q.handle, &msg, K_WAIT(timeout));
            BT_DBG("Mesh Proxy Advertising stop");
            bt_mesh_proxy_server_adv_stop();
        }
#else
        xQueueReceive(adv_queue->q.handle, &msg, portMAX_DELAY);
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#else /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
        handle = xQueueSelectFromSet(mesh_queue_set, K_NO_WAIT);
        if (handle) {
            if (uxQueueMessagesWaiting(adv_queue->q.handle)) {
                xQueueReceive(adv_queue->q.handle, &msg, K_NO_WAIT);
            } else if (uxQueueMessagesWaiting(relay_adv_handle)) {
                xQueueReceive(relay_adv_handle, &msg, K_NO_WAIT);
            }
        } else {
            while (!(*buf)) {
                int32_t timeout = 0;
                BT_DBG("Mesh Proxy Advertising start");
                timeout = bt_mesh_proxy_server_adv_start();
                BT_DBG("Mesh Proxy Advertising up to %d ms", timeout);
                handle = xQueueSelectFromSet(mesh_queue_set, K_WAIT(timeout));
                BT_DBG("Mesh Proxy Advertising stop");
                bt_mesh_proxy_server_adv_stop();
                if (handle) {
                    if (uxQueueMessagesWaiting(adv_queue->q.handle)) {
                        xQueueReceive(adv_queue->q.handle, &msg, K_NO_WAIT);
                    } else if (uxQueueMessagesWaiting(relay_adv_handle)) {
                        xQueueReceive(relay_adv_handle, &msg, K_NO_WAIT);
                    }
                }
            }
        }
#else
        handle = xQueueSelectFromSet(mesh_queue_set, portMAX_DELAY);
        if (handle) {
            if (uxQueueMessagesWaiting(adv_queue->q.handle)) {
                xQueueReceive(adv_queue->q.handle, &msg, K_NO_WAIT);
            } else if (uxQueueMessagesWaiting(relay_adv_handle)) {
                xQueueReceive(relay_adv_handle, &msg, K_NO_WAIT);
            }
        }
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#endif /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */

        if (*buf == NULL) {
            continue;
        }

        /* busy == 0 means this was canceled */
        if (bt_mesh_atomic_cas(&BLE_MESH_ADV_BUSY(*buf), 1, 0)) {
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF
            if (adv_send(*buf)) {
                BT_WARN("Failed to send adv packet");
            }
#else /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
            if (msg.relay && bt_mesh_ignore_relay_packet(msg.timestamp)) {
                /* If the interval between "current time - msg.timestamp" is bigger than
                 * BLE_MESH_RELAY_TIME_INTERVAL, this relay packet will not be sent.
                 */
                BT_INFO("Ignore relay packet");
                net_buf_unref(*buf);
            } else {
                if (adv_send(*buf)) {
                    BT_WARN("Failed to send adv packet");
                }
            }
#endif /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
        } else {
            bt_mesh_adv_buf_ref_debug(__func__, *buf, 1U, BLE_MESH_BUF_REF_EQUAL);
            net_buf_unref(*buf);
        }

        /* Give other threads a chance to run */
        taskYIELD();
    }
}

void bt_mesh_adv_send(struct net_buf *buf, uint8_t xmit,
                      const struct bt_mesh_send_cb *cb,
                      void *cb_data)
{
    bt_mesh_msg_t msg = {
        .relay = false,
    };

    BT_DBG("type 0x%02x len %u: %s", BLE_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BLE_MESH_ADV(buf)->cb = cb;
    BLE_MESH_ADV(buf)->cb_data = cb_data;
    bt_mesh_atomic_set(&BLE_MESH_ADV_BUSY(buf), 1);
    BLE_MESH_ADV(buf)->xmit = xmit;

    bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);

    msg.arg = (void *)net_buf_ref(buf);
    bt_mesh_task_post(&msg, portMAX_DELAY, false);
}

void bt_mesh_adv_update(void)
{
    bt_mesh_msg_t msg = {
        .relay = false,
        .arg = NULL,
    };

    bt_mesh_task_post(&msg, K_NO_WAIT, false);
}

void bt_mesh_adv_init(void)
{

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    bt_mesh_relay_adv_init();
#endif

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    bt_mesh_ble_adv_init();
#endif
    bt_mesh_adv_common_init();
    adv_queue = bt_mesh_adv_queue_get();

    assert(adv_queue && adv_queue->q.handle && adv_queue->send);

#if CONFIG_BLE_MESH_RELAY_ADV_BUF && !CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    QueueHandle_t relay_adv_handle =
        bt_mesh_adv_types_mgnt_get(BLE_MESH_ADV_RELAY_DATA)->adv_q->q.handle;
    mesh_queue_set = xQueueCreateSet(BLE_MESH_QUEUE_SET_SIZE);
    __ASSERT(mesh_queue_set, "Failed to create queue set");
    xQueueAddToSet(adv_queue->q.handle, mesh_queue_set);
    xQueueAddToSet(relay_adv_handle, mesh_queue_set);
#endif

    bt_mesh_adv_task_init(adv_thread);
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_adv_deinit(void)
{
    /* Adv task must be deinit first */
    bt_mesh_adv_task_deinit();

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    QueueHandle_t relay_adv_handle =
        bt_mesh_adv_types_mgnt_get(BLE_MESH_ADV_RELAY_DATA)->adv_q->q.handle;

    xQueueRemoveFromSet(adv_queue->q.handle, mesh_queue_set);
    xQueueRemoveFromSet(relay_adv_handle, mesh_queue_set);
    vQueueDelete(mesh_queue_set);
    mesh_queue_set = NULL;

    bt_mesh_relay_adv_deinit();
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    bt_mesh_ble_adv_deinit();
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

    bt_mesh_adv_common_deinit();
}
#endif /* CONFIG_BLE_MESH_DEINIT */
