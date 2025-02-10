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

#include "adv_common.h"
#include "net.h"
#include "ble_adv.h"

NET_BUF_POOL_DEFINE(adv_buf_pool, CONFIG_BLE_MESH_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv adv_pool[CONFIG_BLE_MESH_ADV_BUF_COUNT];

static struct bt_mesh_adv_queue adv_queue;

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
NET_BUF_POOL_DEFINE(relay_adv_buf_pool, CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);

static struct bt_mesh_adv relay_adv_pool[CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT];
struct bt_mesh_adv_queue relay_adv_queue;

#define BLE_MESH_RELAY_TIME_INTERVAL     K_SECONDS(6)
#define BLE_MESH_MAX_TIME_INTERVAL       0xFFFFFFFF

#endif

#if CONFIG_BLE_MESH_FRIEND
/* We reserve one extra buffer for each friendship, since we need to be able
 * to resend the last sent PDU, which sits separately outside of the queue.
 */
#define FRIEND_BUF_COUNT    ((CONFIG_BLE_MESH_FRIEND_QUEUE_SIZE + 1) * \
                              CONFIG_BLE_MESH_FRIEND_LPN_COUNT)

NET_BUF_POOL_FIXED_DEFINE(friend_buf_pool, FRIEND_BUF_COUNT,
                          BLE_MESH_ADV_DATA_SIZE, NULL);

bt_mesh_friend_adv_t frnd_adv_pool[FRIEND_BUF_COUNT];

struct bt_mesh_adv *bt_mesh_frnd_adv_buf_get(int idx)
{
    frnd_adv_pool[idx].app_idx = BLE_MESH_KEY_UNUSED;
    return &frnd_adv_pool[idx].adv;
}
#endif

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
static struct bt_mesh_adv_type_manager adv_types[BLE_MESH_ADV_TYPES_NUM];

#if CONFIG_BLE_MESH_USE_BLE_50
static struct bt_mesh_adv_inst adv_insts[] = {
    [BLE_MESH_ADV_INS] = {
        .id = CONFIG_BLE_MESH_ADV_INST_ID,
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
        .busy = false,
#endif
    },
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    [BLE_MESH_ADV_PROXY_INS] = {
        .id = CONFIG_BLE_MESH_PROXY_ADV_INST_ID,
        .busy = false,
    },
#endif
#if CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE
    [BLE_MESH_RELAY_ADV_INS] = {
        .id = CONFIG_BLE_MESH_RELAY_ADV_INST_ID,
        .busy = false,
    },
#endif
#if CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE
    [BLE_MESH_BLE_ADV_INS] = {
        .id = CONFIG_BLE_MESH_BLE_ADV_INST_ID,
        .busy = false,
    },
#endif
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
};

static struct bt_mesh_adv_inst *find_adv_inst_with_inst_id(uint8_t id)
{
    for (int i = 0; i < ARRAY_SIZE(adv_insts); i++) {
        if (adv_insts[i].id == id) {
            return &adv_insts[i];
        }
    }

    return NULL;
}

struct bt_mesh_adv_inst *bt_mesh_get_adv_insts_set(void)
{
    return adv_insts;
}

bool bt_mesh_is_adv_inst_used(uint8_t adv_inst_id)
{
    return (find_adv_inst_with_inst_id(adv_inst_id) != NULL);
}

int bt_mesh_adv_inst_init(enum bt_mesh_adv_inst_type inst_type, uint8_t inst_id)
{
    if (inst_type >= BLE_MESH_ADV_INS_TYPES_NUM) {
        BT_ERR("Invalid instance type %d", inst_type);
        return -EINVAL;
    }

    if (inst_id == BLE_MESH_ADV_INS_UNUSED) {
        BT_ERR("Invalid instance id %d", inst_id);
        return -EINVAL;
    }

    adv_insts[inst_type].id = inst_id;
    return 0;
}

int bt_mesh_adv_inst_deinit(enum bt_mesh_adv_inst_type inst_type)
{
    if (inst_type >= BLE_MESH_ADV_INS_TYPES_NUM) {
        BT_ERR("Invalid instance type %d", inst_type);
        return -EINVAL;
    }

    bt_le_ext_adv_stop(adv_insts[inst_type].id);

    adv_insts[inst_type].id = BLE_MESH_ADV_INS_UNUSED;
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    adv_insts[inst_type].spt_mask = 0;
#endif
    return 0;
}

#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

struct bt_mesh_adv *adv_alloc(int id)
{
    return &adv_pool[id];
}

struct bt_mesh_adv_type_manager *bt_mesh_adv_types_mgnt_get(enum bt_mesh_adv_type adv_type)
{
    return &adv_types[adv_type];
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

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
void bt_mesh_adv_inst_supported_adv_type_add(enum bt_mesh_adv_inst_type inst_type,
                                             enum bt_mesh_adv_type adv_type)
{
    if (inst_type >= BLE_MESH_ADV_INS_TYPES_NUM) {
        BT_ERR("Invalid instance type %d", inst_type);
        return;
    }

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("Invalid adv type %d", adv_type);
        return;
    }

    adv_insts[inst_type].spt_mask |= BIT(adv_type);
}

void bt_mesh_adv_inst_supported_adv_type_rm(enum bt_mesh_adv_inst_type inst_type,
                                            enum bt_mesh_adv_type adv_type)
{
    if (inst_type >= BLE_MESH_ADV_INS_TYPES_NUM) {
        BT_ERR("Invalid instance type %d", inst_type);
        return;
    }

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("Invalid adv type %d", adv_type);
        return;
    }

    adv_insts[inst_type].spt_mask &= ~BIT(adv_type);
}

void bt_mesh_adv_inst_supported_adv_type_clear(enum bt_mesh_adv_inst_type inst_type,
                                               enum bt_mesh_adv_type adv_type)
{
    if (inst_type >= BLE_MESH_ADV_INS_TYPES_NUM) {
        BT_ERR("Invalid instance type %d", inst_type);
        return;
    }

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("Invalid adv type %d", adv_type);
        return;
    }

    adv_insts[inst_type].spt_mask = 0;
}
#endif

int bt_mesh_adv_queue_init(struct bt_mesh_adv_queue *adv_queue, uint16_t queue_size,
                           bt_mesh_adv_queue_send_cb_t cb)
{
    if (!adv_queue || !queue_size || !cb) {
        BT_ERR("Invalid param %s", __func__);
        return -EINVAL;
    }

    bt_mesh_queue_init(&adv_queue->q, queue_size, sizeof(bt_mesh_msg_t));

    adv_queue->send = cb;

    return 0;
}

int bt_mesh_adv_queue_deinit(struct bt_mesh_adv_queue *adv_queue)
{
    if (!adv_queue) {
        BT_ERR("Invalid param %s", __func__);
        return -EINVAL;
    }

    bt_mesh_queue_deinit(&adv_queue->q);

    adv_queue->send = NULL;

    return 0;
}

void bt_mesh_adv_type_init(enum bt_mesh_adv_type adv_type,
                           struct bt_mesh_adv_queue *adv_queue,
                           struct net_buf_pool *buf_pool,
                           bt_mesh_pool_allocator_t adv_alloc)
{
    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("%s Invalid adv type %d",__func__, adv_type);
        return;
    }

    if (!adv_queue || !buf_pool || !adv_alloc) {
        BT_ERR("Invalid parameters %s", __func__);
        return;
    }

    adv_types[adv_type].adv_q = adv_queue;
    adv_types[adv_type].pool = buf_pool;
    adv_types[adv_type].pool_allocator = adv_alloc;
}

void bt_mesh_adv_type_deinit(enum bt_mesh_adv_type adv_type)
{
    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("%s Invalid adv type %d",__func__, adv_type);
        return;
    }

    adv_types[adv_type].adv_q = NULL;
    adv_types[adv_type].pool = NULL;
    adv_types[adv_type].pool_allocator = NULL;
}

#if CONFIG_BLE_MESH_USE_BLE_50
int ble_mesh_adv_task_wakeup(uint32_t evt)
{
    xTaskNotify(adv_task.handle, evt, eSetBits);
    return 0;
}

bool ble_mesh_adv_task_wait(uint32_t wait_bits, uint32_t timeout, uint32_t *notify)
{
    return (xTaskNotifyWait(wait_bits, UINT32_MAX, notify, K_WAIT(timeout)) == pdTRUE);
}
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
bool ble_mesh_adv_task_wait(uint32_t timeout)
{
    vTaskDelay(K_WAIT(timeout));
    return true;
}
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

uint16_t bt_mesh_pdu_duration(uint8_t xmit)
{
    uint16_t duration = 0U;
    uint16_t adv_int = 0U;

    adv_int = MAX(ADV_ITVL_MIN, BLE_MESH_TRANSMIT_INT(xmit));
    duration = (BLE_MESH_TRANSMIT_COUNT(xmit) + 1) * (adv_int + 10);

    return duration;
}

struct net_buf *bt_mesh_adv_create_from_pool(enum bt_mesh_adv_type type,
                                             int32_t timeout)
{
    struct bt_mesh_adv *adv = NULL;
    struct net_buf *buf = NULL;
    struct net_buf_pool *pool = adv_types[type].pool;

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_SUSPENDED)) {
        BT_WARN("Refusing to allocate buffer while suspended");
        return NULL;
    }

    if (!pool || !adv_types[type].pool_allocator) {
        BT_ERR("Uninitialized adv type %d", type);
        return NULL;
    }

    buf = net_buf_alloc(pool, timeout);
    if (!buf) {
        BT_WARN("Buf alloc failed");
        return NULL;
    }

    BT_DBG("pool %p, buf_count %d, uinit_count %d, ref %d",
            buf->pool, pool->buf_count, pool->uninit_count, buf->ref);

    adv = adv_types[type].pool_allocator(net_buf_id(buf));
    BLE_MESH_ADV(buf) = adv;

    (void)memset(adv, 0, sizeof(*adv));

    adv->type = type;

    return buf;
}

void bt_mesh_unref_buf_from_pool(struct net_buf_pool *pool)
{
    if (pool == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    for (int i = 0; i < pool->buf_count; i++) {
        struct net_buf *buf = &pool->__bufs[i];
        if (buf->ref > 1U) {
            buf->ref = 1U;
        }
        net_buf_unref(buf);
    }
}

void bt_mesh_unref_buf(bt_mesh_msg_t *msg)
{
    struct net_buf *buf = NULL;

    if (msg->arg) {
        buf = (struct net_buf *)msg->arg;
        bt_mesh_atomic_set(&BLE_MESH_ADV_BUSY(buf), 0);
        if (buf->ref > 1U) {
            buf->ref = 1U;
        }
        net_buf_unref(buf);
    }
}

void bt_mesh_generic_adv_send(struct net_buf *buf, uint8_t xmit,
                              const struct bt_mesh_send_cb *cb,
                              void *cb_data, uint16_t src,
                              uint16_t dst, bool front)
{
    bt_mesh_msg_t msg = {
        .relay = false, /* useless flag in multi-instance mode */
    };

    BT_DBG("type 0x%02x len %u: %s", BLE_MESH_ADV(buf)->type, buf->len,
           bt_hex(buf->data, buf->len));

    BLE_MESH_ADV(buf)->cb = cb;
    BLE_MESH_ADV(buf)->cb_data = cb_data;
    bt_mesh_atomic_set(&BLE_MESH_ADV_BUSY(buf), 1);
    BLE_MESH_ADV(buf)->xmit = xmit;

    bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);

    msg.arg = (void *)net_buf_ref(buf);

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    if (BLE_MESH_ADV(buf)->type == BLE_MESH_ADV_RELAY_DATA) {
        msg.relay = true;
        msg.src = src;
        msg.dst = dst;
        msg.timestamp = k_uptime_get_32();
    }
#endif

    assert(adv_types[BLE_MESH_ADV(buf)->type].adv_q && adv_types[BLE_MESH_ADV(buf)->type].adv_q->send);

    adv_types[BLE_MESH_ADV(buf)->type].adv_q->send(&msg, portMAX_DELAY, front);

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    ble_mesh_adv_task_wakeup(ADV_TASK_PKT_SEND_EVT);
#endif
}

struct bt_mesh_adv_queue *bt_mesh_adv_queue_get(void)
{
    return &adv_queue;
}

void bt_mesh_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front)
{
    if (adv_queue.q.handle == NULL) {
        BT_ERR("Invalid adv queue");
        return;
    }

    if (front) {
        if (xQueueSendToFront(adv_queue.q.handle, msg, timeout) != pdTRUE) {
            BT_ERR("Failed to send item to adv queue front");
            bt_mesh_unref_buf(msg);
        }
    } else {
        if (xQueueSend(adv_queue.q.handle, msg, timeout) != pdTRUE) {
            BT_ERR("Failed to send item to adv queue back");
            bt_mesh_unref_buf(msg);
        }
    }
}

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
bool bt_mesh_ignore_relay_packet(uint32_t timestamp)
{
    uint32_t now = k_uptime_get_32();
    uint32_t interval = 0U;

    if (now >= timestamp) {
        interval = now - timestamp;
    } else {
        interval = BLE_MESH_MAX_TIME_INTERVAL - (timestamp - now) + 1;
    }

    return ((interval >= BLE_MESH_RELAY_TIME_INTERVAL) ? true : false);
}

static struct bt_mesh_adv *relay_adv_alloc(int id)
{
    return &relay_adv_pool[id];
}

struct net_buf *bt_mesh_relay_adv_create(enum bt_mesh_adv_type type, int32_t timeout)
{
    return bt_mesh_adv_create_from_pool(type, timeout);
}

static void ble_mesh_relay_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front)
{
    bt_mesh_msg_t old_msg = {0};

    ARG_UNUSED(front);

    if (relay_adv_queue.q.handle == NULL) {
        BT_ERR("Invalid relay queue");
        return;
    }

    if (xQueueSend(relay_adv_queue.q.handle, msg, timeout) == pdTRUE) {
        return;
    }

    /* If failed to send packet to the relay queue(queue is full), we will
     * remove the oldest packet in the queue and put the new one into it.
     */
    if (uxQueueMessagesWaiting(relay_adv_queue.q.handle)) {
        BT_INFO("Full queue, remove the oldest relay packet");
        /* Remove the oldest relay packet from queue */
        if (xQueueReceive(relay_adv_queue.q.handle, &old_msg, K_NO_WAIT) != pdTRUE) {
            BT_ERR("Failed to remove item from relay queue");
            bt_mesh_unref_buf(msg);
            return;
        }
        /* Unref buf used for the oldest relay packet */
        bt_mesh_unref_buf(&old_msg);
        /* Send the latest relay packet to queue */
        if (xQueueSend(relay_adv_queue.q.handle, msg, K_NO_WAIT) != pdTRUE) {
            BT_ERR("Failed to send item to relay queue");
            bt_mesh_unref_buf(msg);
            return;
        }
    } else {
        BT_WARN("Empty queue, but failed to send the relay packet");
        bt_mesh_unref_buf(msg);
    }
}

uint16_t bt_mesh_get_stored_relay_count(void)
{
    return (uint16_t)uxQueueMessagesWaiting(relay_adv_queue.q.handle);
}

void bt_mesh_relay_adv_init(void)
{
    bt_mesh_adv_queue_init(&relay_adv_queue, CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT,
                           ble_mesh_relay_task_post);
    bt_mesh_adv_type_init(BLE_MESH_ADV_RELAY_DATA, &relay_adv_queue,
                          &relay_adv_buf_pool, &relay_adv_alloc);
#if CONFIG_BLE_MESH_USE_BLE_50
#if CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE
    bt_mesh_adv_inst_init(BLE_MESH_RELAY_ADV_INS,
                          CONFIG_BLE_MESH_RELAY_ADV_INST_ID);
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_RELAY_ADV_INS, BLE_MESH_ADV_RELAY_DATA);
#else
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_ADV_INS, BLE_MESH_ADV_RELAY_DATA);
#endif
#endif /* CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
}
#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_relay_adv_deinit(void)
{
    bt_mesh_adv_queue_deinit(&relay_adv_queue);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_RELAY_DATA);
#if CONFIG_BLE_MESH_USE_BLE_50
#if CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE
    bt_mesh_adv_inst_supported_adv_type_rm(BLE_MESH_RELAY_ADV_INS, BLE_MESH_ADV_RELAY_DATA);
    bt_mesh_adv_inst_deinit(BLE_MESH_RELAY_ADV_INS);
#else
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_supported_adv_type_rm(BLE_MESH_ADV_INS, BLE_MESH_ADV_RELAY_DATA);
#endif
#endif /* CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
    bt_mesh_unref_buf_from_pool(&relay_adv_buf_pool);
    memset(relay_adv_pool, 0, sizeof(relay_adv_pool));
}
#endif /* CONFIG_BLE_MESH_DEINIT */
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

#if CONFIG_BLE_MESH_FRIEND
struct net_buf_pool *bt_mesh_frnd_adv_pool_get(void)
{
    return &friend_buf_pool;
}

void bt_mesh_frnd_adv_init(void)
{
    bt_mesh_adv_type_init(BLE_MESH_ADV_FRIEND, &adv_queue, &friend_buf_pool, bt_mesh_frnd_adv_buf_get);
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_ADV_INS, BLE_MESH_ADV_FRIEND);
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
}

void bt_mesh_frnd_adv_deinit(void)
{
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_FRIEND);

#if CONFIG_BLE_MESH_FRIEND && CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_supported_adv_type_rm(BLE_MESH_ADV_INS, BLE_MESH_ADV_FRIEND);
#endif /* CONFIG_BLE_MESH_FRIEND */

    bt_mesh_unref_buf_from_pool(&friend_buf_pool);
    memset(frnd_adv_pool, 0, sizeof(frnd_adv_pool));
}
#endif /* CONFIG_BLE_MESH_FRIEND */

void bt_mesh_adv_task_init(void adv_thread(void *p))
{
    if (!adv_thread) {
        BT_ERR("Invalid param %s", __func__);
        return;
    }

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

void bt_mesh_adv_common_init(void)
{
    bt_mesh_adv_queue_init(&adv_queue, BLE_MESH_ADV_QUEUE_SIZE, bt_mesh_task_post);
    bt_mesh_adv_type_init(BLE_MESH_ADV_PROV, &adv_queue, &adv_buf_pool, adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_DATA, &adv_queue, &adv_buf_pool, adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_BEACON, &adv_queue, &adv_buf_pool, adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_URI, &adv_queue, &adv_buf_pool, adv_alloc);
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
    bt_mesh_adv_type_init(BLE_MESH_ADV_PROXY_SOLIC, &adv_queue, &adv_buf_pool, adv_alloc);
#endif

#if CONFIG_BLE_MESH_USE_BLE_50
    bt_mesh_adv_inst_init(BLE_MESH_ADV_INS, CONFIG_BLE_MESH_ADV_INST_ID);
#endif

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    /**
     * Due to the limitation of the sequence number in the network layer,
     * it is not possible to use multiple advertising instances to process
     * data from the same message queue when sending mesh packets.
     *
     * Therefore, shall to check whether there are
     * duplicates in the queue buffer corresponding to each advertising instance.
    */
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_ADV_INS, BLE_MESH_ADV_PROV);
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_ADV_INS, BLE_MESH_ADV_DATA);
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_ADV_INS, BLE_MESH_ADV_BEACON);
    bt_mesh_adv_inst_supported_adv_type_add(BLE_MESH_ADV_INS, BLE_MESH_ADV_URI);
#endif
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_adv_task_deinit(void)
{
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
}

void bt_mesh_adv_common_deinit(void)
{
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_PROV);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_DATA);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_BEACON);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_URI);

    bt_mesh_adv_queue_deinit(&adv_queue);
#if CONFIG_BLE_MESH_USE_BLE_50
    bt_mesh_adv_inst_deinit(BLE_MESH_ADV_INS);
#endif

    bt_mesh_unref_buf_from_pool(&adv_buf_pool);
    memset(adv_pool, 0, sizeof(adv_pool));
}
#endif /* CONFIG_BLE_MESH_DEINIT */
