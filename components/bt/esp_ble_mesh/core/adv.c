/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
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

/* Convert from ms to 0.625ms units */
#define ADV_SCAN_UNIT(_ms)  ((_ms) * 8 / 5)
/* Convert from 0.625ms units to interval(ms) */
#define ADV_SCAN_INT(val)   ((val) * 5 / 8)

/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#if CONFIG_BLE_MESH_HCI_5_0
#define ADV_ITVL_MIN        20
#else
#define ADV_ITVL_MIN        100
#endif

static const uint8_t adv_type[] = {
    [BLE_MESH_ADV_PROV]   = BLE_MESH_DATA_MESH_PROV,
    [BLE_MESH_ADV_DATA]   = BLE_MESH_DATA_MESH_MESSAGE,
    [BLE_MESH_ADV_BEACON] = BLE_MESH_DATA_MESH_BEACON,
    [BLE_MESH_ADV_URI]    = BLE_MESH_DATA_URI,
};

NET_BUF_POOL_DEFINE(adv_buf_pool, CONFIG_BLE_MESH_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv adv_pool[CONFIG_BLE_MESH_ADV_BUF_COUNT];

struct bt_mesh_queue {
    QueueHandle_t handle;
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    StaticQueue_t *buffer;
    uint8_t *storage;
#endif
};

static struct bt_mesh_queue adv_queue;
/* We reserve one queue item for bt_mesh_adv_update() */
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
#define BLE_MESH_ADV_QUEUE_SIZE     (CONFIG_BLE_MESH_ADV_BUF_COUNT + CONFIG_BLE_MESH_BLE_ADV_BUF_COUNT + 1)
#else
#define BLE_MESH_ADV_QUEUE_SIZE     (CONFIG_BLE_MESH_ADV_BUF_COUNT + 1)
#endif

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
NET_BUF_POOL_DEFINE(relay_adv_buf_pool, CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv relay_adv_pool[CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT];

static struct bt_mesh_queue relay_queue;
#define BLE_MESH_RELAY_QUEUE_SIZE   CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT

static QueueSetHandle_t mesh_queue_set;
#define BLE_MESH_QUEUE_SET_SIZE     (BLE_MESH_ADV_QUEUE_SIZE + BLE_MESH_RELAY_QUEUE_SIZE)

#define BLE_MESH_RELAY_TIME_INTERVAL     K_SECONDS(6)
#define BLE_MESH_MAX_TIME_INTERVAL       0xFFFFFFFF

static bool ignore_relay_packet(uint32_t timestamp);
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

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

#if CONFIG_BLE_MESH_DEINIT
static void bt_mesh_ble_adv_deinit(void);
#endif /* CONFIG_BLE_MESH_DEINIT */
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

struct bt_mesh_adv_task {
    TaskHandle_t handle;
#if (CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && \
     (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && \
     CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY)
    StaticTask_t *task;
    StackType_t *stack;
#endif
};

static struct bt_mesh_adv_task adv_task;

static struct bt_mesh_adv *adv_alloc(int id)
{
    return &adv_pool[id];
}

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

uint16_t bt_mesh_pdu_duration(uint8_t xmit)
{
    uint16_t duration = 0U;
    uint16_t adv_int = 0U;

    adv_int = MAX(ADV_ITVL_MIN, BLE_MESH_TRANSMIT_INT(xmit));
    duration = (BLE_MESH_TRANSMIT_COUNT(xmit) + 1) * (adv_int + 10);

    return duration;
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

#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
        if (BLE_MESH_ADV(buf)->type == BLE_MESH_ADV_PROXY_SOLIC) {
            bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);
            struct bt_mesh_adv_data solic_ad[3] = {
                BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_FLAGS, (BLE_MESH_AD_GENERAL | BLE_MESH_AD_NO_BREDR)),
                BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x59, 0x18),
                BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, buf->data, buf->len),
            };
            err = bt_le_adv_start(&param, solic_ad, 3, NULL, 0);
        } else
#endif
        {
            bt_mesh_adv_buf_ref_debug(__func__, buf, 4U, BLE_MESH_BUF_REF_SMALL);
            err = bt_le_adv_start(&param, &ad, 1, NULL, 0);
        }
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    } else {
        struct bt_mesh_ble_adv_data data = {0};
        struct ble_adv_tx *tx = cb_data;

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

        err = bt_mesh_ble_adv_start(&tx->param, &data);
    }
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

    net_buf_unref(buf);
    adv_send_start(duration, err, cb, cb_data);
    if (err) {
        BT_ERR("Start advertising failed: err %d", err);
        return err;
    }

    BT_DBG("Advertising started. Sleeping %u ms", duration);

    k_sleep(K_MSEC(duration));

    err = bt_le_adv_stop();
    adv_send_end(err, cb, cb_data);
    if (err) {
        BT_ERR("Stop advertising failed: err %d", err);
        return 0;
    }

    BT_DBG("Advertising stopped");
    return 0;
}

static inline TickType_t K_WAIT(int32_t val)
{
    return (val == K_FOREVER) ? portMAX_DELAY : (val / portTICK_PERIOD_MS);
}

static void adv_thread(void *p)
{
#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    QueueSetMemberHandle_t handle = NULL;
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
        xQueueReceive(adv_queue.handle, &msg, K_NO_WAIT);
        while (!(*buf)) {
            int32_t timeout = 0;
            BT_DBG("Mesh Proxy Advertising start");
            timeout = bt_mesh_proxy_server_adv_start();
            BT_DBG("Mesh Proxy Advertising up to %d ms", timeout);
            xQueueReceive(adv_queue.handle, &msg, K_WAIT(timeout));
            BT_DBG("Mesh Proxy Advertising stop");
            bt_mesh_proxy_server_adv_stop();
        }
#else
        xQueueReceive(adv_queue.handle, &msg, portMAX_DELAY);
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#else /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
        handle = xQueueSelectFromSet(mesh_queue_set, K_NO_WAIT);
        if (handle) {
            if (uxQueueMessagesWaiting(adv_queue.handle)) {
                xQueueReceive(adv_queue.handle, &msg, K_NO_WAIT);
            } else if (uxQueueMessagesWaiting(relay_queue.handle)) {
                xQueueReceive(relay_queue.handle, &msg, K_NO_WAIT);
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
                    if (uxQueueMessagesWaiting(adv_queue.handle)) {
                        xQueueReceive(adv_queue.handle, &msg, K_NO_WAIT);
                    } else if (uxQueueMessagesWaiting(relay_queue.handle)) {
                        xQueueReceive(relay_queue.handle, &msg, K_NO_WAIT);
                    }
                }
            }
        }
#else
        handle = xQueueSelectFromSet(mesh_queue_set, portMAX_DELAY);
        if (handle) {
            if (uxQueueMessagesWaiting(adv_queue.handle)) {
                xQueueReceive(adv_queue.handle, &msg, K_NO_WAIT);
            } else if (uxQueueMessagesWaiting(relay_queue.handle)) {
                xQueueReceive(relay_queue.handle, &msg, K_NO_WAIT);
            }
        }
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
#endif /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */

        if (*buf == NULL) {
            continue;
        }

        /* busy == 0 means this was canceled */
        if (BLE_MESH_ADV(*buf)->busy) {
            BLE_MESH_ADV(*buf)->busy = 0U;
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF
            if (adv_send(*buf)) {
                BT_WARN("Failed to send adv packet");
            }
#else /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
            if (msg.relay && ignore_relay_packet(msg.timestamp)) {
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

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
                                             bt_mesh_adv_alloc_t get_id,
                                             enum bt_mesh_adv_type type,
                                             int32_t timeout)
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

    BT_DBG("pool %p, buf_count %d, uinit_count %d",
            buf->pool, pool->buf_count, pool->uninit_count);

    adv = get_id(net_buf_id(buf));
    BLE_MESH_ADV(buf) = adv;

    (void)memset(adv, 0, sizeof(*adv));

    adv->type = type;

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

struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, int32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&adv_buf_pool, adv_alloc,
                                        type, timeout);
}

void bt_mesh_adv_buf_ref_debug(const char *func, struct net_buf *buf,
                               uint8_t ref_cmp, bt_mesh_buf_ref_flag_t flag)
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
}

static void bt_mesh_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front)
{
    if (adv_queue.handle == NULL) {
        BT_ERR("Invalid adv queue");
        return;
    }

    if (front) {
        if (xQueueSendToFront(adv_queue.handle, msg, timeout) != pdTRUE) {
            BT_ERR("Failed to send item to adv queue front");
            bt_mesh_unref_buf(msg);
        }
    } else {
        if (xQueueSend(adv_queue.handle, msg, timeout) != pdTRUE) {
            BT_ERR("Failed to send item to adv queue back");
            bt_mesh_unref_buf(msg);
        }
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
    BLE_MESH_ADV(buf)->busy = 1U;
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

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
static bool ignore_relay_packet(uint32_t timestamp)
{
    uint32_t now = k_uptime_get_32();
    uint32_t interval = 0U;

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

struct net_buf *bt_mesh_relay_adv_create(enum bt_mesh_adv_type type, int32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&relay_adv_buf_pool, relay_adv_alloc,
                                        type, timeout);
}

static void ble_mesh_relay_task_post(bt_mesh_msg_t *msg, uint32_t timeout)
{
    QueueSetMemberHandle_t handle = NULL;
    bt_mesh_msg_t old_msg = {0};

    if (relay_queue.handle == NULL) {
        BT_ERR("Invalid relay queue");
        return;
    }

    if (xQueueSend(relay_queue.handle, msg, timeout) == pdTRUE) {
        return;
    }

    /* If failed to send packet to the relay queue(queue is full), we will
     * remove the oldest packet in the queue and put the new one into it.
     */
    handle = xQueueSelectFromSet(mesh_queue_set, K_NO_WAIT);
    if (handle && uxQueueMessagesWaiting(relay_queue.handle)) {
        BT_INFO("Full queue, remove the oldest relay packet");
        /* Remove the oldest relay packet from queue */
        if (xQueueReceive(relay_queue.handle, &old_msg, K_NO_WAIT) != pdTRUE) {
            BT_ERR("Failed to remove item from relay queue");
            bt_mesh_unref_buf(msg);
            return;
        }
        /* Unref buf used for the oldest relay packet */
        bt_mesh_unref_buf(&old_msg);
        /* Send the latest relay packet to queue */
        if (xQueueSend(relay_queue.handle, msg, K_NO_WAIT) != pdTRUE) {
            BT_ERR("Failed to send item to relay queue");
            bt_mesh_unref_buf(msg);
            return;
        }
    } else {
        BT_WARN("Empty queue, but failed to send the relay packet");
        bt_mesh_unref_buf(msg);
    }
}

void bt_mesh_relay_adv_send(struct net_buf *buf, uint8_t xmit,
                            uint16_t src, uint16_t dst,
                            const struct bt_mesh_send_cb *cb,
                            void *cb_data)
{
    bt_mesh_msg_t msg = {
        .relay = true,
    };

    BT_DBG("type 0x%02x len %u: %s", BLE_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BLE_MESH_ADV(buf)->cb = cb;
    BLE_MESH_ADV(buf)->cb_data = cb_data;
    BLE_MESH_ADV(buf)->busy = 1U;
    BLE_MESH_ADV(buf)->xmit = xmit;

    msg.arg = (void *)net_buf_ref(buf);
    msg.src = src;
    msg.dst = dst;
    msg.timestamp = k_uptime_get_32();
    /* Use K_NO_WAIT here, if relay_queue is full return immediately */
    ble_mesh_relay_task_post(&msg, K_NO_WAIT);
}

uint16_t bt_mesh_get_stored_relay_count(void)
{
    return (uint16_t)uxQueueMessagesWaiting(relay_queue.handle);
}
#endif /* #if CONFIG_BLE_MESH_RELAY_ADV_BUF */

void bt_mesh_adv_init(void)
{
#if !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    adv_queue.handle = xQueueCreate(BLE_MESH_ADV_QUEUE_SIZE, sizeof(bt_mesh_msg_t));
    __ASSERT(adv_queue.handle, "Failed to create queue");
#else /* !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    adv_queue.buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    adv_queue.buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(adv_queue.buffer, "Failed to create queue buffer");
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    adv_queue.storage = heap_caps_calloc_prefer(1, (BLE_MESH_ADV_QUEUE_SIZE * sizeof(bt_mesh_msg_t)), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    adv_queue.storage = heap_caps_calloc_prefer(1, (BLE_MESH_ADV_QUEUE_SIZE * sizeof(bt_mesh_msg_t)), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(adv_queue.storage, "Failed to create queue storage");
    adv_queue.handle = xQueueCreateStatic(BLE_MESH_ADV_QUEUE_SIZE, sizeof(bt_mesh_msg_t), (uint8_t*)adv_queue.storage, adv_queue.buffer);
    __ASSERT(adv_queue.handle, "Failed to create static queue");
#endif /* !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
#if !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    relay_queue.handle = xQueueCreate(BLE_MESH_RELAY_QUEUE_SIZE, sizeof(bt_mesh_msg_t));
    __ASSERT(relay_queue.handle, "Failed to create relay queue");
#else /* !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    relay_queue.buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    relay_queue.buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(relay_queue.buffer, "Failed to create relay queue buffer");
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    relay_queue.storage = heap_caps_calloc_prefer(1, (BLE_MESH_RELAY_QUEUE_SIZE * sizeof(bt_mesh_msg_t)), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    relay_queue.storage = heap_caps_calloc_prefer(1, (BLE_MESH_RELAY_QUEUE_SIZE * sizeof(bt_mesh_msg_t)), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(relay_queue.storage, "Failed to create relay queue storage");
    relay_queue.handle = xQueueCreateStatic(BLE_MESH_RELAY_QUEUE_SIZE, sizeof(bt_mesh_msg_t), (uint8_t*)relay_queue.storage, relay_queue.buffer);
    __ASSERT(relay_queue.handle, "Failed to create static relay queue");
#endif /* !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */

    mesh_queue_set = xQueueCreateSet(BLE_MESH_QUEUE_SET_SIZE);
    __ASSERT(mesh_queue_set, "Failed to create queue set");
    xQueueAddToSet(adv_queue.handle, mesh_queue_set);
    xQueueAddToSet(relay_queue.handle, mesh_queue_set);
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

#if (CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && \
    (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && \
     CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY)
    adv_task.task = heap_caps_calloc(1, sizeof(StaticTask_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    __ASSERT(adv_task.task, "Failed to create adv thread task");
    adv_task.stack = heap_caps_calloc_prefer(1, BLE_MESH_ADV_TASK_STACK_SIZE * sizeof(StackType_t), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    __ASSERT(adv_task.stack, "Failed to create adv thread stack");
    adv_task.handle = xTaskCreateStaticPinnedToCore(adv_thread, BLE_MESH_ADV_TASK_NAME, BLE_MESH_ADV_TASK_STACK_SIZE, NULL,
                                  BLE_MESH_ADV_TASK_PRIO, adv_task.stack, adv_task.task, BLE_MESH_ADV_TASK_CORE);
    __ASSERT(adv_task.handle, "Failed to create static adv thread");
#else /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY */
    int ret = xTaskCreatePinnedToCore(adv_thread, BLE_MESH_ADV_TASK_NAME, BLE_MESH_ADV_TASK_STACK_SIZE, NULL,
                                      BLE_MESH_ADV_TASK_PRIO, &adv_task.handle, BLE_MESH_ADV_TASK_CORE);
    __ASSERT(ret == pdTRUE, "Failed to create adv thread");
    (void)ret;
#endif /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY */
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_adv_deinit(void)
{
    if (adv_queue.handle == NULL) {
        return;
    }

    vTaskDelete(adv_task.handle);
    adv_task.handle = NULL;
#if (CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && \
    (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && \
     CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY)
    heap_caps_free(adv_task.stack);
    adv_task.stack = NULL;
    heap_caps_free(adv_task.task);
    adv_task.task = NULL;
#endif

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    xQueueRemoveFromSet(adv_queue.handle, mesh_queue_set);
    xQueueRemoveFromSet(relay_queue.handle, mesh_queue_set);

    vQueueDelete(relay_queue.handle);
    relay_queue.handle = NULL;
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    heap_caps_free(relay_queue.buffer);
    relay_queue.buffer = NULL;
    heap_caps_free(relay_queue.storage);
    relay_queue.storage = NULL;
#endif /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */

    bt_mesh_unref_buf_from_pool(&relay_adv_buf_pool);
    memset(relay_adv_pool, 0, sizeof(relay_adv_pool));

    vQueueDelete(mesh_queue_set);
    mesh_queue_set = NULL;
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

    vQueueDelete(adv_queue.handle);
    adv_queue.handle = NULL;
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    heap_caps_free(adv_queue.buffer);
    adv_queue.buffer = NULL;
    heap_caps_free(adv_queue.storage);
    adv_queue.storage = NULL;
#endif /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */

    bt_mesh_unref_buf_from_pool(&adv_buf_pool);
    memset(adv_pool, 0, sizeof(adv_pool));

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    bt_mesh_ble_adv_deinit();
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */
}
#endif /* CONFIG_BLE_MESH_DEINIT */

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
static struct bt_mesh_adv *ble_adv_alloc(int id)
{
    return &ble_adv_pool[id];
}

static struct net_buf *bt_mesh_ble_adv_create(enum bt_mesh_adv_type type, int32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&ble_adv_buf_pool, ble_adv_alloc,
                                        type, timeout);
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

static void ble_adv_send_start(uint16_t duration, int err, void *cb_data)
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
        k_delayed_work_submit(&tx->resend, tx->param.period);
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
    struct ble_adv_tx *tx = CONTAINER_OF(work,
                            struct ble_adv_tx,
                            resend.work);
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
    struct ble_adv_tx *tx = NULL;
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
    struct ble_adv_tx *tx = NULL;
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
    if (BLE_MESH_ADV(tx->buf)->busy == 1U &&
        tx->buf->ref == 1U) {
        unref = false;
    }
    ble_adv_tx_reset(tx, unref);

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static void bt_mesh_ble_adv_deinit(void)
{
    for (int i = 0; i < ARRAY_SIZE(ble_adv_tx); i++) {
        struct ble_adv_tx *tx = &ble_adv_tx[i];
        ble_adv_tx_reset(tx, false);
    }
    bt_mesh_unref_buf_from_pool(&ble_adv_buf_pool);
    memset(ble_adv_pool, 0, sizeof(ble_adv_pool));
}
#endif /* CONFIG_BLE_MESH_DEINIT */
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */
