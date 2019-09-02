/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "osi/thread.h"
#include "sdkconfig.h"
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLE_MESH_DEBUG_ADV)

#include "mesh_util.h"
#include "mesh_buf.h"
#include "mesh_bearer_adapt.h"
#include "mesh_trace.h"
#include "mesh_hci.h"

#include "mesh.h"
#include "adv.h"
#include "net.h"
#include "foundation.h"
#include "beacon.h"
#include "prov.h"
#include "proxy.h"

#include "provisioner_prov.h"
#include "provisioner_proxy.h"
#include "provisioner_beacon.h"

/* Convert from ms to 0.625ms units */
#define ADV_SCAN_UNIT(_ms) ((_ms) * 8 / 5)

/* Window and Interval are equal for continuous scanning */
#define MESH_SCAN_INTERVAL 0x20
#define MESH_SCAN_WINDOW   0x20

/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#define ADV_INT_DEFAULT_MS 100
#define ADV_INT_FAST_MS    20

#if defined(CONFIG_BT_HOST_CRYPTO)
#define ADV_STACK_SIZE 1024
#else
#define ADV_STACK_SIZE 768
#endif

static xQueueHandle xBleMeshQueue;
static const bt_mesh_addr_t *dev_addr;

static const u8_t adv_type[] = {
    [BLE_MESH_ADV_PROV]   = BLE_MESH_DATA_MESH_PROV,
    [BLE_MESH_ADV_DATA]   = BLE_MESH_DATA_MESH_MESSAGE,
    [BLE_MESH_ADV_BEACON] = BLE_MESH_DATA_MESH_BEACON,
    [BLE_MESH_ADV_URI]    = BLE_MESH_DATA_URI,
};

NET_BUF_POOL_DEFINE(adv_buf_pool, CONFIG_BLE_MESH_ADV_BUF_COUNT + 3 * CONFIG_BLE_MESH_PBA_SAME_TIME,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv adv_pool[CONFIG_BLE_MESH_ADV_BUF_COUNT + 3 * CONFIG_BLE_MESH_PBA_SAME_TIME];

static struct bt_mesh_adv *adv_alloc(int id)
{
    return &adv_pool[id];
}

static inline void adv_send_start(u16_t duration, int err,
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
    const s32_t adv_int_min = ((bt_mesh_dev.hci_version >= BLE_MESH_HCI_VERSION_5_0) ?
                               ADV_INT_FAST_MS : ADV_INT_DEFAULT_MS);
    const struct bt_mesh_send_cb *cb = BLE_MESH_ADV(buf)->cb;
    void *cb_data = BLE_MESH_ADV(buf)->cb_data;
    struct bt_mesh_adv_param param = {0};
    u16_t duration, adv_int;
    struct bt_mesh_adv_data ad = {0};
    int err;

    adv_int = MAX(adv_int_min,
                  BLE_MESH_TRANSMIT_INT(BLE_MESH_ADV(buf)->xmit));
    duration = (BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1) *
                (adv_int + 10);

    BT_DBG("type %u len %u: %s", BLE_MESH_ADV(buf)->type,
           buf->len, bt_hex(buf->data, buf->len));
    BT_DBG("count %u interval %ums duration %ums",
           BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1, adv_int,
           duration);

    ad.type = adv_type[BLE_MESH_ADV(buf)->type];
    ad.data_len = buf->len;
    ad.data = buf->data;

    param.options = 0U;
    param.interval_min = ADV_SCAN_UNIT(adv_int);
    param.interval_max = param.interval_min;

    err = bt_le_adv_start(&param, &ad, 1, NULL, 0);
    net_buf_unref(buf);
    adv_send_start(duration, err, cb, cb_data);
    if (err) {
        BT_ERR("%s, Advertising failed: err %d", __func__, err);
        return err;
    }

    BT_DBG("Advertising started. Sleeping %u ms", duration);

    k_sleep(K_MSEC(duration));

    err = bt_le_adv_stop();
    adv_send_end(err, cb, cb_data);
    if (err) {
        BT_ERR("%s, Stop advertising failed: err %d", __func__, err);
        return 0;
    }

    BT_DBG("Advertising stopped");
    return 0;
}

static void adv_thread(void *p)
{
    struct net_buf **buf = NULL;
    bt_mesh_msg_t msg = {0};
    int status;

    BT_DBG("started");

    buf = (struct net_buf **)(&msg.arg);

    while (1) {
        *buf = NULL;
#if CONFIG_BLE_MESH_NODE
        if (IS_ENABLED(CONFIG_BLE_MESH_PROXY)) {
            xQueueReceive(xBleMeshQueue, &msg, K_NO_WAIT);
            while (!(*buf)) {
                s32_t timeout;
                BT_DBG("Proxy advertising start");
                timeout = bt_mesh_proxy_adv_start();
                BT_DBG("Proxy Advertising up to %d ms", timeout);
                xQueueReceive(xBleMeshQueue, &msg, timeout);
                BT_DBG("Proxy advertising stop");
                bt_mesh_proxy_adv_stop();
            }
        } else {
            xQueueReceive(xBleMeshQueue, &msg, (portTickType)portMAX_DELAY);
        }
#else
        xQueueReceive(xBleMeshQueue, &msg, (portTickType)portMAX_DELAY);
#endif

        if (!(*buf)) {
            continue;
        }

        /* busy == 0 means this was canceled */
        if (BLE_MESH_ADV(*buf)->busy) {
            BLE_MESH_ADV(*buf)->busy = 0U;
            status = adv_send(*buf);
            if (status) {
                if (xQueueSendToFront(xBleMeshQueue, &msg, K_NO_WAIT) != pdTRUE) {
                    BT_ERR("%s, xQueueSendToFront failed", __func__);
                }
            }
        } else {
            net_buf_unref(*buf);
        }

        /* Give other threads a chance to run */
        taskYIELD();
    }
}

void bt_mesh_adv_update(void)
{
    BT_DBG("%s", __func__);
    bt_mesh_msg_t msg = {0};
    bt_mesh_task_post(&msg, 0);
}

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
        bt_mesh_adv_alloc_t get_id,
        enum bt_mesh_adv_type type,
        u8_t xmit, s32_t timeout)
{
    struct bt_mesh_adv *adv;
    struct net_buf *buf;

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_SUSPENDED)) {
        BT_WARN("Refusing to allocate buffer while suspended");
        return NULL;
    }

    buf = net_buf_alloc(pool, timeout);
    if (!buf) {
        return NULL;
    }

    BT_DBG("%s, pool = %p, buf_count = %d, uinit_count = %d", __func__,
        buf->pool, pool->buf_count, pool->uninit_count);

    adv = get_id(net_buf_id(buf));
    BLE_MESH_ADV(buf) = adv;

    (void)memset(adv, 0, sizeof(*adv));

    adv->type         = type;
    adv->xmit         = xmit;

    return buf;
}

struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit,
                                   s32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&adv_buf_pool, adv_alloc, type,
                                        xmit, timeout);
}

void bt_mesh_task_post(bt_mesh_msg_t *msg, uint32_t timeout)
{
    BT_DBG("%s", __func__);
    if (xQueueSend(xBleMeshQueue, msg, timeout) != pdTRUE) {
        BT_ERR("%s, Failed to post msg to queue", __func__);
    }
}

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                      void *cb_data)
{
    BT_DBG("type 0x%02x len %u: %s", BLE_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BLE_MESH_ADV(buf)->cb = cb;
    BLE_MESH_ADV(buf)->cb_data = cb_data;
    BLE_MESH_ADV(buf)->busy = 1U;

    bt_mesh_msg_t msg = {0};
    msg.arg = (void *)net_buf_ref(buf);
    bt_mesh_task_post(&msg, portMAX_DELAY);
}

const bt_mesh_addr_t *bt_mesh_pba_get_addr(void)
{
    return dev_addr;
}

static void bt_mesh_scan_cb(const bt_mesh_addr_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
    u16_t uuid = 0;
#endif

    if (adv_type != BLE_MESH_ADV_NONCONN_IND && adv_type != BLE_MESH_ADV_IND) {
        return;
    }

    BT_DBG("%s, len %u: %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    dev_addr = addr;

    while (buf->len > 1) {
        struct net_buf_simple_state state;
        u8_t len, type;

        len = net_buf_simple_pull_u8(buf);
        /* Check for early termination */
        if (len == 0U) {
            return;
        }

        if (len > buf->len) {
            BT_WARN("AD malformed");
            return;
        }

        net_buf_simple_save(buf, &state);

        type = net_buf_simple_pull_u8(buf);

        buf->len = len - 1;

#if 0
        /* TODO: Check with BLE Mesh BQB test cases */
        if ((type == BLE_MESH_DATA_MESH_PROV || type == BLE_MESH_DATA_MESH_MESSAGE ||
            type == BLE_MESH_DATA_MESH_BEACON) && (adv_type != BLE_MESH_ADV_NONCONN_IND)) {
            BT_DBG("%s, ignore BLE Mesh packet (type 0x%02x) with adv_type 0x%02x",
                    __func__, type, adv_type);
            return;
        }
#endif

        switch (type) {
        case BLE_MESH_DATA_MESH_MESSAGE:
            bt_mesh_net_recv(buf, rssi, BLE_MESH_NET_IF_ADV);
            break;
#if CONFIG_BLE_MESH_PB_ADV
        case BLE_MESH_DATA_MESH_PROV:
#if CONFIG_BLE_MESH_NODE
            if (!bt_mesh_is_provisioner_en()) {
                bt_mesh_pb_adv_recv(buf);
            }
#endif
#if CONFIG_BLE_MESH_PROVISIONER
            if (bt_mesh_is_provisioner_en()) {
                provisioner_pb_adv_recv(buf);
            }
#endif
            break;
#endif /* CONFIG_BLE_MESH_PB_ADV */
        case BLE_MESH_DATA_MESH_BEACON:
#if CONFIG_BLE_MESH_NODE
            if (!bt_mesh_is_provisioner_en()) {
                bt_mesh_beacon_recv(buf);
            }
#endif
#if CONFIG_BLE_MESH_PROVISIONER
            if (bt_mesh_is_provisioner_en()) {
                provisioner_beacon_recv(buf);
            }
#endif
            break;
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
        case BLE_MESH_DATA_FLAGS:
            if (bt_mesh_is_provisioner_en()) {
                if (!provisioner_flags_match(buf)) {
                    BT_DBG("Flags mismatch, ignore this adv pkt");
                    return;
                }
            }
            break;
        case BLE_MESH_DATA_UUID16_ALL:
            if (bt_mesh_is_provisioner_en()) {
                uuid = provisioner_srv_uuid_recv(buf);
                if (!uuid) {
                    BT_DBG("Service UUID mismatch, ignore this adv pkt");
                    return;
                }
            }
            break;
        case BLE_MESH_DATA_SVC_DATA16:
            if (bt_mesh_is_provisioner_en()) {
                provisioner_srv_data_recv(buf, addr, uuid);
            }
            break;
#endif /* CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT */
        default:
            break;
        }

        net_buf_simple_restore(buf, &state);
        net_buf_simple_pull(buf, len);
    }

    return;
}

void bt_mesh_adv_init(void)
{
    xBleMeshQueue = xQueueCreate(150, sizeof(bt_mesh_msg_t));
    xTaskCreatePinnedToCore(adv_thread, "BLE_Mesh_ADV_Task", 3072, NULL,
                            configMAX_PRIORITIES - 7, NULL, TASK_PINNED_TO_CORE);
}

int bt_mesh_scan_enable(void)
{
    int err;

    struct bt_mesh_scan_param scan_param = {
        .type       = BLE_MESH_SCAN_PASSIVE,
#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_ENABLE,
#else
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_DISABLE,
#endif
        .interval   = MESH_SCAN_INTERVAL,
        .window     = MESH_SCAN_WINDOW
    };

    BT_DBG("%s", __func__);

    err = bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
    if (err && err != -EALREADY) {
        BT_ERR("starting scan failed (err %d)", err);
        return err;
    }

    return 0;
}

int bt_mesh_scan_disable(void)
{
    int err;

    BT_DBG("%s", __func__);

    err = bt_le_scan_stop();
    if (err && err != -EALREADY) {
        BT_ERR("stopping scan failed (err %d)", err);
        return err;
    }

    return 0;
}
