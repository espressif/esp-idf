/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#if CONFIG_BT_MESH

#include "osi/thread.h"

#include "mesh_util.h"

#include "mesh_buf.h"
#include "mesh.h"
#include "mesh_bearer_adapt.h"
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_ADV)
#include "mesh_trace.h"

#include "adv.h"
#include "foundation.h"
#include "net.h"
#include "beacon.h"
#include "prov.h"
#include "proxy.h"
#include "mesh_hci.h"

#include "sdkconfig.h"
#include "provisioner_prov.h"
#include "provisioner_proxy.h"
#include "provisioner_beacon.h"

#define BT_DATA_MESH_PROV               0x29 /* Mesh Provisioning PDU */
#define BT_DATA_MESH_MESSAGE            0x2a /* Mesh Networking PDU */
#define BT_DATA_MESH_BEACON             0x2b /* Mesh Beacon */


/* Window and Interval are equal for continuous scanning */
#define MESH_SCAN_INTERVAL 0x20//0x10 /*Change the scan interval to 20ms here just to reduce the packet loss rate */
#define MESH_SCAN_WINDOW   0x20//0x10 /*Change the scan window to 20ms here just to reduce the packet loss rate */

/* Convert from ms to 0.625ms units */
#define ADV_INT(_ms) ((_ms) * 8 / 5)

/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#define ADV_INT_DEFAULT  K_MSEC(100)
#define ADV_INT_FAST     K_MSEC(20)

/* TinyCrypt PRNG consumes a lot of stack space, so we need to have
 * an increased call stack whenever it's used.
 */
#if defined(CONFIG_BT_HOST_CRYPTO)
#define ADV_STACK_SIZE 768
#else
#define ADV_STACK_SIZE 512
#endif

static xQueueHandle xBleMeshQueue = 0;
static const bt_addr_le_t *dev_addr;

static const u8_t adv_type[] = {
    [BT_MESH_ADV_PROV]   = BT_DATA_MESH_PROV,
    [BT_MESH_ADV_DATA]   = BT_DATA_MESH_MESSAGE,
    [BT_MESH_ADV_BEACON] = BT_DATA_MESH_BEACON,
};

NET_BUF_POOL_DEFINE(adv_buf_pool, CONFIG_BT_MESH_ADV_BUF_COUNT + 3 * CONFIG_BT_MESH_PBA_SAME_TIME,
                    BT_MESH_ADV_DATA_SIZE, BT_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv adv_pool[CONFIG_BT_MESH_ADV_BUF_COUNT + 3 * CONFIG_BT_MESH_PBA_SAME_TIME];

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
    const s32_t adv_int_min = ((bt_dev.hci_version >= BT_HCI_VERSION_5_0) ?
                               ADV_INT_FAST : ADV_INT_DEFAULT);
    const struct bt_mesh_send_cb *cb = BT_MESH_ADV(buf)->cb;
    void *cb_data = BT_MESH_ADV(buf)->cb_data;
    struct bt_le_adv_param param;
    u16_t duration, adv_int;
    struct bt_data ad;
    int err;

    adv_int = max(adv_int_min, BT_MESH_ADV(buf)->adv_int);
    duration = (BT_MESH_ADV(buf)->count + 1) * (adv_int + 10);

    BT_DBG("type %u len %u: %s", BT_MESH_ADV(buf)->type,
           buf->len, bt_hex(buf->data, buf->len));
    BT_DBG("count %u interval %ums duration %ums",
           BT_MESH_ADV(buf)->count + 1, adv_int, duration);

    ad.type = adv_type[BT_MESH_ADV(buf)->type];
    ad.data_len = buf->len;
    ad.data = buf->data;

    param.options = 0;
    param.interval_min = ADV_INT(adv_int);
    param.interval_max = param.interval_min;
    param.own_addr = NULL;

    err = bt_le_adv_start(&param, &ad, 1, NULL, 0);
    net_buf_unref(buf);
    adv_send_start(duration, err, cb, cb_data);
    if (err) {
        BT_ERR("Advertising failed: err %d", err);
        return err;
    }

    BT_DBG("Advertising started. Sleeping %u ms", duration);

    k_sleep(duration);

    err = bt_le_adv_stop();
    adv_send_end(err, cb, cb_data);
    if (err) {
        BT_ERR("Stopping advertising failed: err %d", err);
        /* If start adv successfully but stop failed, we think the data has been sent successfully */
        return 0;
    }

    BT_DBG("Advertising stopped");
    return 0;
}

/* Change by Espressif. The implementation of this function needs to be modified to
 * FreeRTOS task implementation to use on ESP-IDF */
static void adv_thread(void *p)
{
    BT_DBG("started");
    int status;
    struct net_buf **buf;
    ble_mesh_msg_t msg = {0};
    buf = (struct net_buf **)(&msg.arg);

    while (1) {
        *buf = NULL;
#if CONFIG_BT_MESH_NODE
        if (IS_ENABLED(CONFIG_BT_MESH_PROXY)) {
            xQueueReceive(xBleMeshQueue, &msg, K_NO_WAIT);
            while (!(*buf)) {
                s32_t timeout;
                BT_DBG("========");
                timeout = bt_mesh_proxy_adv_start();
                BT_DBG("=========Proxy Advertising up to %d ms", timeout);
                xQueueReceive(xBleMeshQueue, &msg, timeout);
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
        if (BT_MESH_ADV(*buf)->busy) {
            BT_MESH_ADV(*buf)->busy = 0;
            /*TODO: Currently we check status of adv_send, which has changed the original
             * code of Zephyr, we need to find a better way in the future
             * */
            status = adv_send(*buf);
            if (status) {
                status = xQueueSendToFront(xBleMeshQueue, &msg, K_NO_WAIT);
                if (status) {
                    BT_ERR("adv_send, xQueueSendToFront failed, status=0x%x", status);
                }
            }
        }

        /* Give other threads a chance to run */
        taskYIELD();
    }
}

void bt_mesh_adv_update(void)
{
    BT_DBG("%s", __func__);
    ble_mesh_msg_t msg = {0};
    msg.arg = NULL;
    // Change by Espressif, should used the QueueSend in the ESP-IDF architecture.
    ble_mesh_task_post(&msg, 0);
}

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
        bt_mesh_adv_alloc_t get_id,
        enum bt_mesh_adv_type type,
        u8_t xmit_count, u8_t xmit_int,
        s32_t timeout)
{
    struct bt_mesh_adv *adv;
    struct net_buf *buf;

    buf = net_buf_alloc(pool, timeout);
    if (!buf) {
        return NULL;
    }

    BT_DBG("%s, pool_id = %p, buf_count = %d, uinit_count = %d", __func__,
           buf->pool_id, pool->buf_count, pool->uninit_count);
    // adv = get_id(pool->buf_count - pool->uninit_count);
    /* Change by Espressif. Use buf->index to get corresponding adv_pool */
    adv = get_id(buf->index);
    BT_MESH_ADV(buf) = adv;

    memset(adv, 0, sizeof(*adv));

    adv->type         = type;
    adv->count        = xmit_count;
    adv->adv_int      = xmit_int;

    return buf;
}

struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit_count,
                                   u8_t xmit_int, s32_t timeout)
{
    return bt_mesh_adv_create_from_pool(&adv_buf_pool, adv_alloc, type,
                                        xmit_count, xmit_int, timeout);
}

void ble_mesh_task_post(ble_mesh_msg_t *msg, uint32_t timeout)
{
    BT_DBG("%s", __func__);
    if (xQueueSend(xBleMeshQueue, msg, timeout) != pdTRUE) {
        BT_ERR("Ble Mesh Post failed\n");
        return;
    }
}

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                      void *cb_data)
{
    ble_mesh_msg_t msg = {0};
    struct net_buf *send_buf = NULL;
    BT_DBG("type 0x%02x len %u: %s", BT_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BT_MESH_ADV(buf)->cb = cb;
    BT_MESH_ADV(buf)->cb_data = cb_data;
    BT_MESH_ADV(buf)->busy = 1;
    send_buf = net_buf_ref(buf);
    msg.arg = (void *)send_buf;
    //net_buf_put(NULL, net_buf_ref(buf));
    /* Change by Espressif. The ESP-IDF should used the QueueSend to sent the msg. */
    ble_mesh_task_post(&msg, portMAX_DELAY);
    /* Deleted the net_buf_put function, we don't used it in the ESP-IDF architecture */
    //net_buf_put(NULL, net_buf_ref(buf));
}

const bt_addr_le_t *bt_mesh_pba_get_addr(void)
{
    return dev_addr;
}

static void bt_mesh_scan_cb(const bt_addr_le_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
#if CONFIG_BT_MESH_PROVISIONER
    u16_t uuid = 0;
#endif

    if (adv_type != BT_LE_ADV_NONCONN_IND && adv_type != BT_LE_ADV_IND) {
        return;
    }

    BT_DBG("%s, len %u: %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    dev_addr = addr;

    while (buf->len > 1) {
        struct net_buf_simple_state state;
        u8_t len, type;

        len = net_buf_simple_pull_u8(buf);
        /* Check for early termination */
        if (len == 0) {
            return;
        }

        if (len > buf->len || buf->len < 1) {
            BT_WARN("AD malformed");
            return;
        }

        net_buf_simple_save(buf, &state);

        type = net_buf_simple_pull_u8(buf);
        BT_DBG("%s, type = %x", __func__, type);
        buf->len = len - 1;

        switch (type) {
            if (adv_type == BT_LE_ADV_NONCONN_IND) {
            case BT_DATA_MESH_MESSAGE:
                bt_mesh_net_recv(buf, rssi, BT_MESH_NET_IF_ADV);
                break;
#if CONFIG_BT_MESH_PB_ADV
            case BT_DATA_MESH_PROV:
#if CONFIG_BT_MESH_NODE
                if (!bt_mesh_is_provisioner_en()) {
                    bt_mesh_pb_adv_recv(buf);
                }
#endif
#if CONFIG_BT_MESH_PROVISIONER
                if (bt_mesh_is_provisioner_en()) {
                    provisioner_pb_adv_recv(buf);
                }
#endif
                break;
#endif /* CONFIG_BT_MESH_PB_ADV */
            case BT_DATA_MESH_BEACON:
#if CONFIG_BT_MESH_NODE
                if (!bt_mesh_is_provisioner_en()) {
                    bt_mesh_beacon_recv(buf);
                }
#endif
#if CONFIG_BT_MESH_PROVISIONER
                if (bt_mesh_is_provisioner_en()) {
                    provisioner_beacon_recv(buf);
                }
#endif
                break;
            } else if (adv_type == BT_LE_ADV_IND) {
#if CONFIG_BT_MESH_PROVISIONER && CONFIG_BT_MESH_PB_GATT
            case BT_DATA_FLAGS:
                if (bt_mesh_is_provisioner_en()) {
                    if (!provisioner_flags_match(buf)) {
                        BT_DBG("Flags mismatch, ignore this adv pkt");
                        return;
                    }
                }
                break;
            case BT_DATA_SERVICE_UUID:
                if (bt_mesh_is_provisioner_en()) {
                    uuid = provisioner_srv_uuid_recv(buf);
                    if (!uuid) {
                        BT_DBG("Service UUID mismatch, ignore this adv pkt");
                        return;
                    }
                }
                break;
            case BT_DATA_SERVICE_DATA:
                if (bt_mesh_is_provisioner_en()) {
                    provisioner_srv_data_recv(buf, addr, uuid);
                }
                break;
#endif /* CONFIG_BT_MESH_PROVISIONER && CONFIG_BT_MESH_PB_GATT */
            }
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
    /* Change by Espressif, we should used the FreeRTOS create task method to use task */
    xBleMeshQueue = xQueueCreate(150, sizeof(ble_mesh_msg_t));
    xTaskCreatePinnedToCore(adv_thread, "BLE_Mesh_Adv_task", 2048, NULL,
                            configMAX_PRIORITIES - 7, NULL, TASK_PINNED_TO_CORE);
}

int bt_mesh_scan_enable(void)
{
    struct bt_le_scan_param scan_param = {
        .type       = BT_HCI_LE_SCAN_PASSIVE,
        .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
        .interval   = MESH_SCAN_INTERVAL,
        .window     = MESH_SCAN_WINDOW
    };

    BT_DBG("%s", __func__);

    return bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
}

int bt_mesh_scan_disable(void)
{
    BT_DBG("%s", __func__);

    return bt_le_scan_stop();
}

#endif /* #if CONFIG_BT_MESH */
