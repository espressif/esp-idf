/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "osi/thread.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLE_MESH_DEBUG_ADV)

#include "mesh.h"
#include "mesh_hci.h"
#include "mesh_common.h"
#include "adv.h"
#include "beacon.h"
#include "prov.h"
#include "foundation.h"
#include "proxy_server.h"
#include "proxy_client.h"
#include "provisioner_prov.h"
#include "mesh_bearer_adapt.h"

/* Convert from ms to 0.625ms units */
#define ADV_SCAN_UNIT(_ms) ((_ms) * 8 / 5)
/* Convert from 0.625ms units to interval(ms) */
#define ADV_SCAN_INT(val) ((val) * 5 / 8)

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

static const bt_mesh_addr_t *dev_addr;

static const u8_t adv_type[] = {
    [BLE_MESH_ADV_PROV]   = BLE_MESH_DATA_MESH_PROV,
    [BLE_MESH_ADV_DATA]   = BLE_MESH_DATA_MESH_MESSAGE,
    [BLE_MESH_ADV_BEACON] = BLE_MESH_DATA_MESH_BEACON,
    [BLE_MESH_ADV_URI]    = BLE_MESH_DATA_URI,
};

NET_BUF_POOL_DEFINE(adv_buf_pool, CONFIG_BLE_MESH_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv adv_pool[CONFIG_BLE_MESH_ADV_BUF_COUNT];

struct bt_mesh_queue {
    QueueHandle_t queue;
#if CONFIG_SPIRAM_USE_MALLOC
    StaticQueue_t *buffer;
    u8_t *storage;
#endif
};

static struct bt_mesh_queue xBleMeshQueue;
/* We reserve one queue for bt_mesh_adv_update() */
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
#define BLE_MESH_QUEUE_SIZE     (CONFIG_BLE_MESH_ADV_BUF_COUNT + CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT + 1)
#else
#define BLE_MESH_QUEUE_SIZE     (CONFIG_BLE_MESH_ADV_BUF_COUNT + 1)
#endif

#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
NET_BUF_POOL_DEFINE(relay_adv_buf_pool, CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv relay_adv_pool[CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT];

static struct bt_mesh_queue xBleMeshRelayQueue;
#define BLE_MESH_RELAY_QUEUE_SIZE   CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT

static QueueSetHandle_t xBleMeshQueueSet;
#define BLE_MESH_QUEUE_SET_SIZE     (BLE_MESH_QUEUE_SIZE + BLE_MESH_RELAY_QUEUE_SIZE)

#define BLE_MESH_RELAY_TIME_INTERVAL     K_SECONDS(6)
#define BLE_MESH_MAX_TIME_INTERVAL       0xFFFFFFFF

static bool ignore_relay_packet(u32_t timestamp);
#endif /* defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
/* length + advertising data + length + scan response data */
NET_BUF_POOL_DEFINE(ble_adv_buf_pool, CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT,
                    ((BLE_MESH_ADV_DATA_SIZE + 3) << 1), BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv ble_adv_pool[CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT];

enum {
    TIMER_INIT,  /* Resend timer is initialized */
    NUM_FLAGS,
};

static struct ble_adv_tx {
    struct bt_mesh_ble_adv_param param;
    struct net_buf              *buf;
    struct k_delayed_work        resend;
    BLE_MESH_ATOMIC_DEFINE(flags, NUM_FLAGS);
} ble_adv_tx[CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT];

#define SEND_BLE_ADV_INFINITE    0xFFFF

static void bt_mesh_ble_adv_deinit(void);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

struct bt_mesh_adv_task {
    TaskHandle_t handle;
#if CONFIG_SPIRAM_USE_MALLOC
    StaticTask_t *task;
    StackType_t *stack;
#endif
};

static struct bt_mesh_adv_task adv_task;

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
    u16_t duration = 0U, adv_int = 0U;
    struct bt_mesh_adv_data ad = {0};
    int err = 0;

    BT_DBG("type %u len %u: %s", BLE_MESH_ADV(buf)->type,
        buf->len, bt_hex(buf->data, buf->len));

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    if (BLE_MESH_ADV(buf)->type != BLE_MESH_ADV_BLE) {
#endif
        adv_int = MAX(adv_int_min,
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

        bt_mesh_adv_buf_ref_debug(__func__, buf, 4U, BLE_MESH_BUF_REF_SMALL);

        err = bt_le_adv_start(&param, &ad, 1, NULL, 0);
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    } else {
        struct bt_mesh_ble_adv_data data = {0};
        struct ble_adv_tx *tx = cb_data;

        if (tx == NULL) {
            BT_ERR("%s, Invalid adv user data", __func__);
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

        err = bt_mesh_ble_adv_start(&tx->param, &data);
    }
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

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
#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
    QueueSetMemberHandle_t handle = NULL;
#endif
    bt_mesh_msg_t msg = {0};
    struct net_buf **buf = NULL;

    buf = (struct net_buf **)(&msg.arg);

    BT_DBG("%s, starts", __func__);

    while (1) {
        *buf = NULL;
#if !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_SERVER
        xQueueReceive(xBleMeshQueue.queue, &msg, K_NO_WAIT);
        while (!(*buf)) {
            s32_t timeout;
            BT_DBG("Mesh Proxy Advertising start");
            timeout = bt_mesh_proxy_adv_start();
            BT_DBG("Mesh Proxy Advertising up to %d ms", timeout);
            xQueueReceive(xBleMeshQueue.queue, &msg, timeout);
            BT_DBG("Mesh Proxy Advertising stop");
            bt_mesh_proxy_adv_stop();
        }
#else
        xQueueReceive(xBleMeshQueue.queue, &msg, portMAX_DELAY);
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#else /* !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_SERVER
        handle = xQueueSelectFromSet(xBleMeshQueueSet, K_NO_WAIT);
        if (handle) {
            if (uxQueueMessagesWaiting(xBleMeshQueue.queue)) {
                xQueueReceive(xBleMeshQueue.queue, &msg, K_NO_WAIT);
            } else if (uxQueueMessagesWaiting(xBleMeshRelayQueue.queue)) {
                xQueueReceive(xBleMeshRelayQueue.queue, &msg, K_NO_WAIT);
            }
        } else {
            while (!(*buf)) {
                s32_t timeout = 0;
                BT_DBG("Mesh Proxy Advertising start");
                timeout = bt_mesh_proxy_adv_start();
                BT_DBG("Mesh Proxy Advertising up to %d ms", timeout);
                handle = xQueueSelectFromSet(xBleMeshQueueSet, timeout);
                BT_DBG("Mesh Proxy Advertising stop");
                bt_mesh_proxy_adv_stop();
                if (handle) {
                    if (uxQueueMessagesWaiting(xBleMeshQueue.queue)) {
                        xQueueReceive(xBleMeshQueue.queue, &msg, K_NO_WAIT);
                    } else if (uxQueueMessagesWaiting(xBleMeshRelayQueue.queue)) {
                        xQueueReceive(xBleMeshRelayQueue.queue, &msg, K_NO_WAIT);
                    }
                }
            }
        }
#else
        handle = xQueueSelectFromSet(xBleMeshQueueSet, portMAX_DELAY);
        if (handle) {
            if (uxQueueMessagesWaiting(xBleMeshQueue.queue)) {
                xQueueReceive(xBleMeshQueue.queue, &msg, K_NO_WAIT);
            } else if (uxQueueMessagesWaiting(xBleMeshRelayQueue.queue)) {
                xQueueReceive(xBleMeshRelayQueue.queue, &msg, K_NO_WAIT);
            }
        }
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#endif /* !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */

        if (*buf == NULL) {
            continue;
        }

        /* busy == 0 means this was canceled */
        if (BLE_MESH_ADV(*buf)->busy) {
            BLE_MESH_ADV(*buf)->busy = 0U;
#if !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
            if (adv_send(*buf)) {
                BT_WARN("%s, Failed to send adv packet", __func__);
            }
#else /* !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */
            if (msg.relay && ignore_relay_packet(msg.timestamp)) {
                /* If the interval between "current time - msg.timestamp" is bigger than
                 * BLE_MESH_RELAY_TIME_INTERVAL, this relay packet will not be sent.
                 */
                BT_INFO("%s, Ignore relay packet", __func__);
                net_buf_unref(*buf);
            } else {
                if (adv_send(*buf)) {
                    BT_WARN("%s, Failed to send adv packet", __func__);
                }
            }
#endif
        } else {
            bt_mesh_adv_buf_ref_debug(__func__, *buf, 1U, BLE_MESH_BUF_REF_EQUAL);
            net_buf_unref(*buf);
        }

        /* Give other threads a chance to run */
        taskYIELD();
    }
}

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
        bt_mesh_adv_alloc_t get_id,
        enum bt_mesh_adv_type type,
        u8_t xmit, s32_t timeout)
{
    struct bt_mesh_adv *adv = NULL;
    struct net_buf *buf = NULL;

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

void bt_mesh_unref_buf_from_pool(struct net_buf_pool *pool)
{
    int i;

    if (pool == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    for (i = 0; i < pool->buf_count; i++) {
        struct net_buf *buf = &pool->__bufs[i];
        if (buf->ref > 1U) {
            buf->ref = 1U;
        }
        net_buf_unref(buf);
    }
}

struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit,
                                   s32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&adv_buf_pool, adv_alloc, type,
                                        xmit, timeout);
}

void bt_mesh_adv_buf_ref_debug(const char *func, struct net_buf *buf,
                               u8_t ref_cmp, bt_mesh_buf_ref_flag_t flag)
{
    if (buf == NULL || func == NULL || flag >= BLE_MESH_BUF_REF_MAX) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (flag) {
    case BLE_MESH_BUF_REF_EQUAL:
        if (buf->ref != ref_cmp) {
            BT_ERR("Unexpected ref %d in %s, expect to equal to %d", buf->ref, func, ref_cmp);
        }
        break;
    case BLE_MESH_BUF_REF_SMALL:
        if (buf->ref >= ref_cmp) {
            BT_ERR("Unexpected ref %d in %s, expect to smaller than %d", buf->ref, func, ref_cmp);
        }
        break;
    default:
        break;
    }
}

static void bt_mesh_unref_buf(bt_mesh_msg_t *msg)
{
    struct net_buf *buf = NULL;

    if (msg->arg) {
        buf = (struct net_buf *)msg->arg;
        BLE_MESH_ADV(buf)->busy = 0U;
        if (buf->ref > 1U) {
            buf->ref = 1U;
        }
        net_buf_unref(buf);
    }

    return;
}

static void bt_mesh_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front)
{
    BT_DBG("%s", __func__);

    if (xBleMeshQueue.queue == NULL) {
        BT_ERR("%s, Invalid queue", __func__);
        return;
    }

    if (front) {
        if (xQueueSendToFront(xBleMeshQueue.queue, msg, timeout) != pdTRUE) {
            BT_ERR("%s, Failed to send item to queue front", __func__);
            bt_mesh_unref_buf(msg);
        }
    } else {
        if (xQueueSend(xBleMeshQueue.queue, msg, timeout) != pdTRUE) {
            BT_ERR("%s, Failed to send item to queue back", __func__);
            bt_mesh_unref_buf(msg);
        }
    }
}

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                      void *cb_data)
{
    bt_mesh_msg_t msg = {
        .relay = false,
    };

    BT_DBG("type 0x%02x len %u: %s", BLE_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BLE_MESH_ADV(buf)->cb = cb;
    BLE_MESH_ADV(buf)->cb_data = cb_data;
    BLE_MESH_ADV(buf)->busy = 1U;

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

    BT_DBG("%s", __func__);

    bt_mesh_task_post(&msg, K_NO_WAIT, false);
}

#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
static bool ignore_relay_packet(u32_t timestamp)
{
    u32_t now = k_uptime_get_32();
    u32_t interval = 0U;

    if (now >= timestamp) {
        interval = now - timestamp;
    } else {
        interval = BLE_MESH_MAX_TIME_INTERVAL - (timestamp - now) + 1;
    }

    return (interval >= BLE_MESH_RELAY_TIME_INTERVAL) ? true : false;
}

static struct bt_mesh_adv *relay_adv_alloc(int id)
{
    return &relay_adv_pool[id];
}

struct net_buf *bt_mesh_relay_adv_create(enum bt_mesh_adv_type type, u8_t xmit,
        s32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&relay_adv_buf_pool, relay_adv_alloc, type,
                                        xmit, timeout);
}

static void ble_mesh_relay_task_post(bt_mesh_msg_t *msg, uint32_t timeout)
{
    QueueSetMemberHandle_t handle = NULL;
    bt_mesh_msg_t old_msg = {0};

    BT_DBG("%s", __func__);

    if (xBleMeshRelayQueue.queue == NULL) {
        BT_ERR("%s, Invalid relay queue", __func__);
        return;
    }

    if (xQueueSend(xBleMeshRelayQueue.queue, msg, timeout) == pdTRUE) {
        return;
    }

    /**
     * If failed to send packet to the relay queue(queue is full), we will
     * remove the oldest packet in the queue and put the new one into it.
     */
    handle = xQueueSelectFromSet(xBleMeshQueueSet, K_NO_WAIT);
    if (handle && uxQueueMessagesWaiting(xBleMeshRelayQueue.queue)) {
        BT_INFO("%s, Full queue, remove the oldest relay packet", __func__);
        /* Remove the oldest relay packet from queue */
        if (xQueueReceive(xBleMeshRelayQueue.queue, &old_msg, K_NO_WAIT) != pdTRUE) {
            BT_ERR("%s, Failed to remove item from queue", __func__);
            bt_mesh_unref_buf(msg);
            return;
        }
        /* Unref buf used for the oldest relay packet */
        bt_mesh_unref_buf(&old_msg);
        /* Send the latest relay packet to queue */
        if (xQueueSend(xBleMeshRelayQueue.queue, msg, K_NO_WAIT) != pdTRUE) {
            BT_ERR("%s, Failed to send item to relay queue", __func__);
            bt_mesh_unref_buf(msg);
            return;
        }
    } else {
        BT_WARN("%s, Empty queue, but failed to send the relay packet", __func__);
        bt_mesh_unref_buf(msg);
    }
}

void bt_mesh_relay_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                            void *cb_data, u16_t src, u16_t dst)
{
    bt_mesh_msg_t msg = {
        .relay = true,
    };

    BT_DBG("type 0x%02x len %u: %s", BLE_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BLE_MESH_ADV(buf)->cb = cb;
    BLE_MESH_ADV(buf)->cb_data = cb_data;
    BLE_MESH_ADV(buf)->busy = 1U;

    msg.arg = (void *)net_buf_ref(buf);
    msg.src = src;
    msg.dst = dst;
    msg.timestamp = k_uptime_get_32();
    /* Use K_NO_WAIT here, if xBleMeshRelayQueue is full return immediately */
    ble_mesh_relay_task_post(&msg, K_NO_WAIT);
}

u16_t bt_mesh_get_stored_relay_count(void)
{
    return (u16_t)uxQueueMessagesWaiting(xBleMeshRelayQueue.queue);
}
#endif /* #if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */

const bt_mesh_addr_t *bt_mesh_pba_get_addr(void)
{
    return dev_addr;
}

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static bool bt_mesh_is_adv_flags_valid(struct net_buf_simple *buf)
{
    u8_t flags = 0U;

    if (buf->len != 1U) {
        BT_DBG("%s, Unexpected flags length", __func__);
        return false;
    }

    flags = net_buf_simple_pull_u8(buf);

    BT_DBG("Received adv pkt with flags: 0x%02x", flags);

    /* Flags context will not be checked curently */
    ((void) flags);

    return true;
}

static bool bt_mesh_is_adv_srv_uuid_valid(struct net_buf_simple *buf, u16_t *uuid)
{
    if (buf->len != 2U) {
        BT_DBG("Length not match mesh service uuid");
        return false;
    }

    *uuid = net_buf_simple_pull_le16(buf);

    BT_DBG("Received adv pkt with service UUID: %d", *uuid);

    if (*uuid != BLE_MESH_UUID_MESH_PROV_VAL &&
            *uuid != BLE_MESH_UUID_MESH_PROXY_VAL) {
        return false;
    }

    if (*uuid == BLE_MESH_UUID_MESH_PROV_VAL &&
            bt_mesh_is_provisioner_en() == false) {
        return false;
    }

    if (*uuid == BLE_MESH_UUID_MESH_PROXY_VAL &&
            !IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)) {
        return false;
    }

    return true;
}

#define BLE_MESH_PROV_SRV_DATA_LEN      0x12
#define BLE_MESH_PROXY_SRV_DATA_LEN1    0x09
#define BLE_MESH_PROXY_SRV_DATA_LEN2    0x11

static void bt_mesh_adv_srv_data_recv(struct net_buf_simple *buf, const bt_mesh_addr_t *addr, u16_t uuid, s8_t rssi)
{
    u16_t type = 0U;

    if (!buf || !addr) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    type = net_buf_simple_pull_le16(buf);
    if (type != uuid) {
        BT_DBG("%s, Invalid Mesh Service Data UUID 0x%04x", __func__, type);
        return;
    }

    switch (type) {
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
    case BLE_MESH_UUID_MESH_PROV_VAL:
        if (bt_mesh_is_provisioner_en()) {
            if (buf->len != BLE_MESH_PROV_SRV_DATA_LEN) {
                BT_WARN("%s, Invalid Mesh Prov Service Data length %d", __func__, buf->len);
                return;
            }

            BT_DBG("Start to handle Mesh Prov Service Data");
            bt_mesh_provisioner_prov_adv_ind_recv(buf, addr, rssi);
        }
        break;
#endif
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    case BLE_MESH_UUID_MESH_PROXY_VAL:
        if (buf->len != BLE_MESH_PROXY_SRV_DATA_LEN1 &&
                buf->len != BLE_MESH_PROXY_SRV_DATA_LEN2) {
            BT_WARN("%s, Invalid Mesh Proxy Service Data length %d", __func__, buf->len);
            return;
        }

        BT_DBG("Start to handle Mesh Proxy Service Data");
        bt_mesh_proxy_client_adv_ind_recv(buf, addr, rssi);
        break;
#endif
    default:
        break;
    }
}
#endif

static void bt_mesh_scan_cb(const bt_mesh_addr_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    u16_t uuid = 0U;
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
            if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node()) {
                bt_mesh_pb_adv_recv(buf);
            }
            if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
                bt_mesh_provisioner_pb_adv_recv(buf);
            }
            break;
#endif /* CONFIG_BLE_MESH_PB_ADV */
        case BLE_MESH_DATA_MESH_BEACON:
            bt_mesh_beacon_recv(buf, rssi);
            break;
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
        case BLE_MESH_DATA_FLAGS:
            if (!bt_mesh_is_adv_flags_valid(buf)) {
                BT_DBG("Adv Flags mismatch, ignore this adv pkt");
                return;
            }
            break;
        case BLE_MESH_DATA_UUID16_ALL:
            if (!bt_mesh_is_adv_srv_uuid_valid(buf, &uuid)) {
                BT_DBG("Adv Service UUID mismatch, ignore this adv pkt");
                return;
            }
            break;
        case BLE_MESH_DATA_SVC_DATA16:
            bt_mesh_adv_srv_data_recv(buf, addr, uuid, rssi);
            break;
#endif
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
#if !CONFIG_SPIRAM_USE_MALLOC
    xBleMeshQueue.queue = xQueueCreate(BLE_MESH_QUEUE_SIZE, sizeof(bt_mesh_msg_t));
    __ASSERT(xBleMeshQueue.queue, "%s, Failed to create queue", __func__);
#else
    xBleMeshQueue.buffer = heap_caps_calloc(1, sizeof(StaticQueue_t), MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM);
    __ASSERT(xBleMeshQueue.buffer, "%s, Failed to create queue buffer", __func__);
    xBleMeshQueue.storage = heap_caps_calloc(1, (BLE_MESH_QUEUE_SIZE * sizeof(bt_mesh_msg_t)), MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM);
    __ASSERT(xBleMeshQueue.storage, "%s, Failed to create queue storage", __func__);
    xBleMeshQueue.queue = xQueueCreateStatic(BLE_MESH_QUEUE_SIZE, sizeof(bt_mesh_msg_t), (uint8_t*)xBleMeshQueue.storage, xBleMeshQueue.buffer);
    __ASSERT(xBleMeshQueue.queue, "%s, Failed to create static queue", __func__);
#endif

#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
#if !CONFIG_SPIRAM_USE_MALLOC
    xBleMeshRelayQueue.queue = xQueueCreate(BLE_MESH_RELAY_QUEUE_SIZE, sizeof(bt_mesh_msg_t));
    __ASSERT(xBleMeshRelayQueue.queue, "%s, Failed to create relay queue", __func__);
#else
    xBleMeshRelayQueue.buffer = heap_caps_calloc(1, sizeof(StaticQueue_t), MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM);
    __ASSERT(xBleMeshRelayQueue.buffer, "%s, Failed to create relay queue buffer", __func__);
    xBleMeshRelayQueue.storage = heap_caps_calloc(1, (BLE_MESH_RELAY_QUEUE_SIZE * sizeof(bt_mesh_msg_t)), MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM);
    __ASSERT(xBleMeshRelayQueue.storage, "%s, Failed to create relay queue storage", __func__);
    xBleMeshRelayQueue.queue = xQueueCreateStatic(BLE_MESH_RELAY_QUEUE_SIZE, sizeof(bt_mesh_msg_t), (uint8_t*)xBleMeshRelayQueue.storage, xBleMeshRelayQueue.buffer);
    __ASSERT(xBleMeshRelayQueue.queue, "%s, Failed to create static relay queue", __func__);
#endif

    xBleMeshQueueSet = xQueueCreateSet(BLE_MESH_QUEUE_SET_SIZE);
    __ASSERT(xBleMeshQueueSet, "%s, Failed to create queue set", __func__);
    xQueueAddToSet(xBleMeshQueue.queue, xBleMeshQueueSet);
    xQueueAddToSet(xBleMeshRelayQueue.queue, xBleMeshQueueSet);
#endif /* defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */

#if !CONFIG_SPIRAM_USE_MALLOC
    int ret = xTaskCreatePinnedToCore(adv_thread, "BLE_Mesh_ADV_Task", BLE_MESH_ADV_TASK_STACK_SIZE, NULL,
                                      configMAX_PRIORITIES - 5, &adv_task.handle, BLE_MESH_ADV_TASK_CORE);
    __ASSERT(ret == pdTRUE, "%s, Failed to create adv thread", __func__);
#else
    adv_task.task = heap_caps_calloc(1, sizeof(StaticTask_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    __ASSERT(adv_task.task, "%s, Failed to create adv thread task", __func__);
#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
    adv_task.stack = heap_caps_calloc(1, BLE_MESH_ADV_TASK_STACK_SIZE * sizeof(StackType_t), MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM);
#else
    adv_task.stack = heap_caps_calloc(1, BLE_MESH_ADV_TASK_STACK_SIZE * sizeof(StackType_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(adv_task.stack, "%s, Failed to create adv thread stack", __func__);
    adv_task.handle = xTaskCreateStaticPinnedToCore(adv_thread, "BLE_Mesh_ADV_Task", BLE_MESH_ADV_TASK_STACK_SIZE, NULL,
                                  configMAX_PRIORITIES - 5, adv_task.stack, adv_task.task, BLE_MESH_ADV_TASK_CORE);
    __ASSERT(adv_task.handle, "%s, Failed to create static adv thread", __func__);
#endif
}

void bt_mesh_adv_deinit(void)
{
    if (xBleMeshQueue.queue == NULL) {
        return;
    }

    vTaskDelete(adv_task.handle);
    adv_task.handle = NULL;
#if CONFIG_SPIRAM_USE_MALLOC
    heap_caps_free(adv_task.stack);
    adv_task.stack = NULL;
    heap_caps_free(adv_task.task);
    adv_task.task = NULL;
#endif

#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
    xQueueRemoveFromSet(xBleMeshQueue.queue, xBleMeshQueueSet);
    xQueueRemoveFromSet(xBleMeshRelayQueue.queue, xBleMeshQueueSet);

    vQueueDelete(xBleMeshRelayQueue.queue);
    xBleMeshRelayQueue.queue = NULL;
#if CONFIG_SPIRAM_USE_MALLOC
    heap_caps_free(xBleMeshRelayQueue.buffer);
    xBleMeshRelayQueue.buffer = NULL;
    heap_caps_free(xBleMeshRelayQueue.storage);
    xBleMeshRelayQueue.storage = NULL;
#endif

    bt_mesh_unref_buf_from_pool(&relay_adv_buf_pool);
    memset(relay_adv_pool, 0, sizeof(relay_adv_pool));

    vQueueDelete(xBleMeshQueueSet);
    xBleMeshQueueSet = NULL;
#endif /* defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */

    vQueueDelete(xBleMeshQueue.queue);
    xBleMeshQueue.queue = NULL;
#if CONFIG_SPIRAM_USE_MALLOC
    heap_caps_free(xBleMeshQueue.buffer);
    xBleMeshQueue.buffer = NULL;
    heap_caps_free(xBleMeshQueue.storage);
    xBleMeshQueue.storage = NULL;
#endif

    bt_mesh_unref_buf_from_pool(&adv_buf_pool);
    memset(adv_pool, 0, sizeof(adv_pool));

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    bt_mesh_ble_adv_deinit();
#endif
}

int bt_mesh_scan_enable(void)
{
    int err = 0;

    struct bt_mesh_scan_param scan_param = {
        .type       = BLE_MESH_SCAN_PASSIVE,
#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_ENABLE,
#else
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_DISABLE,
#endif
        .interval   = MESH_SCAN_INTERVAL,
        .window     = MESH_SCAN_WINDOW,
        .scan_fil_policy = BLE_MESH_SP_ADV_ALL,
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
    int err = 0;

    BT_DBG("%s", __func__);

    err = bt_le_scan_stop();
    if (err && err != -EALREADY) {
        BT_ERR("stopping scan failed (err %d)", err);
        return err;
    }

    return 0;
}

#if CONFIG_BLE_MESH_TEST_USE_WHITE_LIST
int bt_mesh_scan_with_wl_enable(void)
{
    int err = 0;

    struct bt_mesh_scan_param scan_param = {
        .type       = BLE_MESH_SCAN_PASSIVE,
#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_ENABLE,
#else
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_DISABLE,
#endif
        .interval   = MESH_SCAN_INTERVAL,
        .window     = MESH_SCAN_WINDOW,
        .scan_fil_policy = BLE_MESH_SP_ADV_WL,
    };

    BT_DBG("%s", __func__);

    err = bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
    if (err) {
        BT_ERR("starting scan failed (err %d)", err);
        return err;
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_TEST_USE_WHITE_LIST */

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
static struct bt_mesh_adv *ble_adv_alloc(int id)
{
    return &ble_adv_pool[id];
}

static struct net_buf *bt_mesh_ble_adv_create(enum bt_mesh_adv_type type, u8_t xmit, s32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&ble_adv_buf_pool, ble_adv_alloc, type,
                                        xmit, timeout);
}

static void bt_mesh_ble_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                                 void *cb_data, bool front)
{
    bt_mesh_msg_t msg = {
        .relay = false,
    };

    BT_DBG("type 0x%02x len %u: %s", BLE_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BLE_MESH_ADV(buf)->cb = cb;
    BLE_MESH_ADV(buf)->cb_data = cb_data;
    BLE_MESH_ADV(buf)->busy = 1U;

    bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);

    msg.arg = (void *)net_buf_ref(buf);
    bt_mesh_task_post(&msg, portMAX_DELAY, front);
}

static void ble_adv_tx_reset(struct ble_adv_tx *tx, bool unref)
{
    if (tx->buf == NULL) {
        return;
    }

    if (bt_mesh_atomic_test_bit(tx->flags, TIMER_INIT)) {
        k_delayed_work_free(&tx->resend);
    }
    bt_mesh_atomic_set(tx->flags, 0);
    memset(&tx->param, 0, sizeof(tx->param));
    BLE_MESH_ADV(tx->buf)->busy = 0U;
    if (unref) {
        net_buf_unref(tx->buf);
    }
    tx->buf = NULL;
}

static void ble_adv_send_start(u16_t duration, int err, void *cb_data)
{
    struct ble_adv_tx *tx = cb_data;

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
    struct ble_adv_tx *tx = cb_data;

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
    struct ble_adv_tx *tx = CONTAINER_OF(work, struct ble_adv_tx, resend.work);
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
                                  const struct bt_mesh_ble_adv_data *data, u8_t *index)
{
    struct ble_adv_tx *tx = NULL;
    struct net_buf *buf = NULL;
    bool front = false;

    if (param == NULL || index == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (param->adv_type != BLE_MESH_ADV_DIRECT_IND &&
        (param->interval < 0x20 || param->interval > 0x4000)) {
        BT_ERR("%s, Invalid adv interval 0x%04x", __func__, param->interval);
        return -EINVAL;
    }

    if (param->adv_type > BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) {
        BT_ERR("%s, Invalid adv type 0x%02x", __func__, param->adv_type);
        return -EINVAL;
    }

    if (param->own_addr_type > BLE_MESH_ADDR_RANDOM_ID) {
        BT_ERR("%s, Invalid own addr type 0x%02x", __func__, param->own_addr_type);
        return -EINVAL;
    }

    if ((param->own_addr_type == BLE_MESH_ADDR_PUBLIC_ID ||
        param->own_addr_type == BLE_MESH_ADDR_RANDOM_ID ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) &&
        param->peer_addr_type > BLE_MESH_ADDR_RANDOM) {
        BT_ERR("%s, Invalid peer addr type 0x%02x", __func__, param->peer_addr_type);
        return -EINVAL;
    }

    if (data && (data->adv_data_len > 31 || data->scan_rsp_data_len > 31)) {
        BT_ERR("%s, Invalid adv data length, %d %d", __func__,
            data->adv_data_len, data->scan_rsp_data_len);
        return -EINVAL;
    }

    if (param->priority > BLE_MESH_BLE_ADV_PRIO_HIGH) {
        BT_ERR("%s, Invalid adv priority %d", __func__, param->priority);
        return -EINVAL;
    }

    if (param->duration < ADV_SCAN_INT(param->interval)) {
        BT_ERR("%s, Too small duration %dms", __func__, param->duration);
        return -EINVAL;
    }

    buf = bt_mesh_ble_adv_create(BLE_MESH_ADV_BLE, 0U, K_NO_WAIT);
    if (!buf) {
        BT_ERR("%s, Unable to allocate buffer", __func__);
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
        k_delayed_work_init(&tx->resend, ble_adv_resend);
        bt_mesh_atomic_set_bit(tx->flags, TIMER_INIT);
    } else {
        /* Send the BLE advertising packet only once */
        net_buf_unref(buf);
    }

    return 0;
}

int bt_mesh_stop_ble_advertising(u8_t index)
{
    struct ble_adv_tx *tx = NULL;
    bool unref = true;

    if (index >= ARRAY_SIZE(ble_adv_tx)) {
        BT_ERR("%s, Invalid index %d", __func__, index);
        return -EINVAL;
    }

    tx = &ble_adv_tx[index];

    if (tx->buf == NULL) {
        BT_WARN("%s, Already stopped, index %d", __func__, index);
        return 0;
    }

    /* busy 1, ref 1; busy 1, ref 2;
     * busy 0, ref 0; busy 0, ref 1;
     */
    if (BLE_MESH_ADV(tx->buf)->busy == 1U &&
        tx->buf->ref == 1U) {
        unref = false;
    }
    ble_adv_tx_reset(tx, unref);

    return 0;
}

static void bt_mesh_ble_adv_deinit(void)
{
    for (int i = 0; i < ARRAY_SIZE(ble_adv_tx); i++) {
        struct ble_adv_tx *tx = &ble_adv_tx[i];
        ble_adv_tx_reset(tx, false);
    }
    bt_mesh_unref_buf_from_pool(&ble_adv_buf_pool);
    memset(ble_adv_pool, 0, sizeof(ble_adv_pool));
}
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */
