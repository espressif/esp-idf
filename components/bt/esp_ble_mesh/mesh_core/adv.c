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
#include "proxy_server.h"

#include "provisioner_prov.h"
#include "proxy_client.h"
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

static QueueHandle_t xBleMeshQueue;
#define BLE_MESH_QUEUE_SIZE         150

#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
NET_BUF_POOL_DEFINE(relay_adv_buf_pool, CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv relay_adv_pool[CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT];

static QueueHandle_t xBleMeshRelayQueue;
#define BLE_MESH_RELAY_QUEUE_SIZE   150

static QueueSetHandle_t xBleMeshQueueSet;
#define BLE_MESH_QUEUE_SET_SIZE     (BLE_MESH_QUEUE_SIZE + BLE_MESH_RELAY_QUEUE_SIZE)

#define BLE_MESH_RELAY_TIME_INTERVAL     K_SECONDS(6)
#define BLE_MESH_MAX_TIME_INTERVAL       0xFFFFFFFF

static bool ignore_relay_packet(u32_t timestamp);
#endif /* defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */

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
#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
    QueueSetMemberHandle_t handle;
#endif
    bt_mesh_msg_t msg = {0};
    struct net_buf **buf;

    buf = (struct net_buf **)(&msg.arg);

    BT_DBG("%s, starts", __func__);

    while (1) {
        *buf = NULL;
#if !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
#if CONFIG_BLE_MESH_NODE
        if (IS_ENABLED(CONFIG_BLE_MESH_PROXY)) {
            xQueueReceive(xBleMeshQueue, &msg, K_NO_WAIT);
            while (!(*buf)) {
                s32_t timeout;
                BT_DBG("Mesh Proxy Advertising start");
                timeout = bt_mesh_proxy_adv_start();
                BT_DBG("Mesh Proxy Advertising up to %d ms", timeout);
                xQueueReceive(xBleMeshQueue, &msg, timeout);
                BT_DBG("Mesh Proxy Advertising stop");
                bt_mesh_proxy_adv_stop();
            }
        } else {
            xQueueReceive(xBleMeshQueue, &msg, portMAX_DELAY);
        }
#else
        xQueueReceive(xBleMeshQueue, &msg, portMAX_DELAY);
#endif
#else /* !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */
#if CONFIG_BLE_MESH_NODE
        if (IS_ENABLED(CONFIG_BLE_MESH_PROXY)) {
            handle = xQueueSelectFromSet(xBleMeshQueueSet, K_NO_WAIT);
            if (handle) {
                if (uxQueueMessagesWaiting(xBleMeshQueue)) {
                    xQueueReceive(xBleMeshQueue, &msg, K_NO_WAIT);
                } else if (uxQueueMessagesWaiting(xBleMeshRelayQueue)) {
                    xQueueReceive(xBleMeshRelayQueue, &msg, K_NO_WAIT);
                }
            } else {
                while (!(*buf)) {
                    s32_t timeout;
                    BT_DBG("Mesh Proxy Advertising start");
                    timeout = bt_mesh_proxy_adv_start();
                    BT_DBG("Mesh Proxy Advertising up to %d ms", timeout);
                    handle = xQueueSelectFromSet(xBleMeshQueueSet, timeout);
                    BT_DBG("Mesh Proxy Advertising stop");
                    bt_mesh_proxy_adv_stop();
                    if (handle) {
                        if (uxQueueMessagesWaiting(xBleMeshQueue)) {
                            xQueueReceive(xBleMeshQueue, &msg, K_NO_WAIT);
                        } else if (uxQueueMessagesWaiting(xBleMeshRelayQueue)) {
                            xQueueReceive(xBleMeshRelayQueue, &msg, K_NO_WAIT);
                        }
                    }
                }
            }
        } else {
            handle = xQueueSelectFromSet(xBleMeshQueueSet, portMAX_DELAY);
            if (handle) {
                if (uxQueueMessagesWaiting(xBleMeshQueue)) {
                    xQueueReceive(xBleMeshQueue, &msg, K_NO_WAIT);
                } else if (uxQueueMessagesWaiting(xBleMeshRelayQueue)) {
                    xQueueReceive(xBleMeshRelayQueue, &msg, K_NO_WAIT);
                }
            }
        }
#else
        handle = xQueueSelectFromSet(xBleMeshQueueSet, portMAX_DELAY);
        if (handle) {
            if (uxQueueMessagesWaiting(xBleMeshQueue)) {
                xQueueReceive(xBleMeshQueue, &msg, K_NO_WAIT);
            } else if (uxQueueMessagesWaiting(xBleMeshRelayQueue)) {
                xQueueReceive(xBleMeshRelayQueue, &msg, K_NO_WAIT);
            }
        }
#endif
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
                BT_DBG("%s, Ignore relay packet", __func__);
                net_buf_unref(*buf);
            } else {
                if (adv_send(*buf)) {
                    BT_WARN("%s, Failed to send adv packet", __func__);
                }
            }
#endif
        } else {
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

static void bt_mesh_unref_buf(bt_mesh_msg_t *msg)
{
    struct net_buf *buf;

    if (msg->arg) {
        buf = (struct net_buf *)msg->arg;
        BLE_MESH_ADV(buf)->busy = 0U;
        net_buf_unref(buf);
    }

    return;
}

static void bt_mesh_task_post(bt_mesh_msg_t *msg, uint32_t timeout)
{
    BT_DBG("%s", __func__);
    if (xQueueSend(xBleMeshQueue, msg, timeout) != pdTRUE) {
        BT_ERR("%s, Failed to send item to queue", __func__);
        bt_mesh_unref_buf(msg);
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

    msg.arg = (void *)net_buf_ref(buf);
    bt_mesh_task_post(&msg, portMAX_DELAY);
}

void bt_mesh_adv_update(void)
{
    bt_mesh_msg_t msg = {
        .relay = false,
        .arg = NULL,
    };

    BT_DBG("%s", __func__);

    bt_mesh_task_post(&msg, K_NO_WAIT);
}

#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
static bool ignore_relay_packet(u32_t timestamp)
{
    u32_t now = k_uptime_get_32();
    u32_t interval;

    if (now > timestamp) {
        interval = now - timestamp;
    } else if (now == timestamp) {
        interval = BLE_MESH_MAX_TIME_INTERVAL;
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
    QueueSetMemberHandle_t handle;
    bt_mesh_msg_t old_msg = {0};

    BT_DBG("%s", __func__);

    if (xQueueSend(xBleMeshRelayQueue, msg, timeout) == pdTRUE) {
        return;
    }

    /**
     * If failed to send packet to the relay queue(queue is full), we will
     * remove the oldest packet in the queue and put the new one into it.
     */
    handle = xQueueSelectFromSet(xBleMeshQueueSet, K_NO_WAIT);
    if (handle && uxQueueMessagesWaiting(xBleMeshRelayQueue)) {
        BT_DBG("%s, Full queue, remove the oldest relay packet", __func__);
        /* Remove the oldest relay packet from queue */
        if (xQueueReceive(xBleMeshRelayQueue, &old_msg, K_NO_WAIT) != pdTRUE) {
            BT_ERR("%s, Failed to remove item from queue", __func__);
            bt_mesh_unref_buf(msg);
            return;
        }
        /* Unref buf used for the oldest relay packet */
        bt_mesh_unref_buf(&old_msg);
        /* Send the latest relay packet to queue */
        if (xQueueSend(xBleMeshRelayQueue, msg, K_NO_WAIT) != pdTRUE) {
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
    return (u16_t)uxQueueMessagesWaiting(xBleMeshRelayQueue);
}
#endif /* #if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */

const bt_mesh_addr_t *bt_mesh_pba_get_addr(void)
{
    return dev_addr;
}

#if (CONFIG_BLE_MESH_PROVISIONER && COFNIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static bool bt_mesh_is_adv_flags_valid(struct net_buf_simple *buf)
{
    u8_t flags;

    if (buf->len != 1U) {
        BT_DBG("%s, Unexpected flags length", __func__);
        return false;
    }

    flags = net_buf_simple_pull_u8(buf);

    BT_DBG("Received adv pkt with flags: 0x%02x", flags);

    /* Flags context will not be checked curently */

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

static void bt_mesh_adv_srv_data_recv(struct net_buf_simple *buf, const bt_mesh_addr_t *addr, u16_t uuid)
{
    u16_t type;

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
            provisioner_prov_adv_ind_recv(buf, addr);
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
        proxy_client_adv_ind_recv(buf, addr);
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
#if (CONFIG_BLE_MESH_PROVISIONER && COFNIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    u16_t uuid;
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
#if (CONFIG_BLE_MESH_PROVISIONER && COFNIG_BLE_MESH_PB_GATT) || \
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
            bt_mesh_adv_srv_data_recv(buf, addr, uuid);
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
    xBleMeshQueue = xQueueCreate(BLE_MESH_QUEUE_SIZE, sizeof(bt_mesh_msg_t));
    configASSERT(xBleMeshQueue);
#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
    xBleMeshRelayQueue = xQueueCreate(BLE_MESH_RELAY_QUEUE_SIZE, sizeof(bt_mesh_msg_t));
    configASSERT(xBleMeshRelayQueue);
    xBleMeshQueueSet = xQueueCreateSet(BLE_MESH_QUEUE_SET_SIZE);
    configASSERT(xBleMeshQueueSet);
    xQueueAddToSet(xBleMeshQueue, xBleMeshQueueSet);
    xQueueAddToSet(xBleMeshRelayQueue, xBleMeshQueueSet);
#endif /* defined(CONFIG_BLE_MESH_RELAY_ADV_BUF) */
    int ret = xTaskCreatePinnedToCore(adv_thread, "BLE_Mesh_ADV_Task", 3072, NULL,
                                      configMAX_PRIORITIES - 7, NULL, ADV_TASK_CORE);
    configASSERT(ret == pdTRUE);
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
