/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
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
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

static bt_mesh_mutex_t adv_buf_alloc_lock;
#if CONFIG_BLE_MESH_EXT_ADV
NET_BUF_POOL_DEFINE(ext_adv_buf_pool, CONFIG_BLE_MESH_EXT_ADV_BUF_COUNT,
                    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);
static bt_mesh_ext_adv_t ext_adv_pool[CONFIG_BLE_MESH_EXT_ADV_BUF_COUNT];
#if CONFIG_BLE_MESH_EXT_RELAY_ADV_BUF_COUNT
NET_BUF_POOL_DEFINE(ext_relay_adv_buf_pool, CONFIG_BLE_MESH_EXT_RELAY_ADV_BUF_COUNT,
    BLE_MESH_ADV_DATA_SIZE, BLE_MESH_ADV_USER_DATA_SIZE, NULL);
static bt_mesh_ext_adv_t ext_relay_adv_pool[CONFIG_BLE_MESH_EXT_RELAY_ADV_BUF_COUNT];
#endif /* CONFIG_BLE_MESH_EXT_RELAY_ADV_BUF_COUNT */
#if CONFIG_BLE_MESH_LONG_PACKET
NET_BUF_POOL_DEFINE(ext_long_adv_buf_pool, CONFIG_BLE_MESH_LONG_PACKET_TX_SEG_CNT,
                    CONFIG_BLE_MESH_LONG_PACKET_ADV_LEN, BLE_MESH_ADV_USER_DATA_SIZE, NULL);
static bt_mesh_ext_adv_t ext_long_adv_pool[CONFIG_BLE_MESH_LONG_PACKET_TX_SEG_CNT];
#if CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT
NET_BUF_POOL_DEFINE(ext_long_relay_adv_buf_pool, CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT,
                    CONFIG_BLE_MESH_LONG_PACKET_ADV_LEN, BLE_MESH_ADV_USER_DATA_SIZE, NULL);
static bt_mesh_ext_adv_t ext_long_relay_adv_pool[CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT];
#endif /* CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT */
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */

const uint8_t adv_type[] = {
    [BLE_MESH_ADV_PROV]   = BLE_MESH_DATA_MESH_PROV,
    [BLE_MESH_ADV_DATA]   = BLE_MESH_DATA_MESH_MESSAGE,
#if CONFIG_BLE_MESH_EXT_ADV
    [BLE_MESH_ADV_EXT_PROV] = BLE_MESH_DATA_MESH_PROV,
    [BLE_MESH_ADV_EXT_RELAY_DATA] = BLE_MESH_DATA_MESH_MESSAGE,
    [BLE_MESH_ADV_EXT_DATA] = BLE_MESH_DATA_MESH_MESSAGE,
#if CONFIG_BLE_MESH_LONG_PACKET
    [BLE_MESH_ADV_EXT_LONG_PROV] = BLE_MESH_DATA_MESH_PROV,
    [BLE_MESH_ADV_EXT_LONG_RELAY_DATA] = BLE_MESH_DATA_MESH_MESSAGE,
    [BLE_MESH_ADV_EXT_LONG_DATA] = BLE_MESH_DATA_MESH_MESSAGE,
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
#if CONFIG_BLE_MESH_FRIEND
    [BLE_MESH_ADV_FRIEND]     = BLE_MESH_DATA_MESH_MESSAGE,
#endif
#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    [BLE_MESH_ADV_RELAY_DATA] = BLE_MESH_DATA_MESH_MESSAGE,
#endif
    [BLE_MESH_ADV_BEACON]     = BLE_MESH_DATA_MESH_BEACON,
    [BLE_MESH_ADV_URI]        = BLE_MESH_DATA_URI,
};

static inline void init_adv_with_defaults(struct bt_mesh_adv *adv,
                                          enum bt_mesh_adv_type type)
{
    memset(adv, 0, sizeof(struct bt_mesh_adv));
    adv->type = type;
    adv->adv_itvl = BLE_MESH_ADV_ITVL_DEFAULT;
    adv->adv_cnt = BLE_MESH_ADV_CNT_DEFAULT;
    adv->channel_map = BLE_MESH_ADV_CHAN_DEFAULT;
}

#if CONFIG_BLE_MESH_FRIEND
/* We reserve one extra buffer for each friendship, since we need to be able
 * to resend the last sent PDU, which sits separately outside of the queue.
 */
#define FRIEND_BUF_COUNT    ((CONFIG_BLE_MESH_FRIEND_QUEUE_SIZE + 1) * \
                              CONFIG_BLE_MESH_FRIEND_LPN_COUNT)

NET_BUF_POOL_FIXED_DEFINE(friend_buf_pool, FRIEND_BUF_COUNT,
                          BLE_MESH_ADV_DATA_SIZE, NULL);

static bt_mesh_friend_adv_t frnd_adv_pool[FRIEND_BUF_COUNT];
#endif /* CONFIG_BLE_MESH_FRIEND */

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
    [BLE_MESH_ADV_INST] = {
        .id = CONFIG_BLE_MESH_ADV_INST_ID,
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
        .busy = false,
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
    },
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    [BLE_MESH_ADV_PROXY_INST] = {
        .id = CONFIG_BLE_MESH_PROXY_ADV_INST_ID,
        .busy = false,
    },
#endif
#if CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE
    [BLE_MESH_RELAY_ADV_INST] = {
        .id = CONFIG_BLE_MESH_RELAY_ADV_INST_ID,
        .busy = false,
    },
#endif /* CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE */
#if CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE
    [BLE_MESH_BLE_ADV_INST] = {
        .id = CONFIG_BLE_MESH_BLE_ADV_INST_ID,
        .busy = false,
    },
#endif /* CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE */
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
};

static struct bt_mesh_adv_inst *find_adv_inst_with_inst_id(uint8_t id)
{
    BT_DBG("FindAdvInstWithID, InstID %u", id);

    for (int i = 0; i < ARRAY_SIZE(adv_insts); i++) {
        if (adv_insts[i].id == id) {
            return &adv_insts[i];
        }
    }

    BT_WARN("NotFoundAdvInst, InstID %u", id);
    return NULL;
}

struct bt_mesh_adv_inst *bt_mesh_get_adv_insts_set(void)
{
    return adv_insts;
}

bool bt_mesh_is_adv_inst_used(uint8_t inst_id)
{
    BT_DBG("IsAdvInstUsed, InstID %u", inst_id);

    return (find_adv_inst_with_inst_id(inst_id) != NULL);
}

int bt_mesh_adv_inst_init(enum bt_mesh_adv_inst_type inst_type, uint8_t inst_id)
{
    BT_DBG("AdvInstInit, InstType %u InstID %u", inst_type, inst_id);

    if (inst_type >= BLE_MESH_ADV_INST_TYPES_NUM) {
        BT_ERR("InvalidAdvInstType %u", inst_type);
        return -EINVAL;
    }

    if (inst_id == BLE_MESH_ADV_INST_UNUSED) {
        BT_ERR("UnusedAdvInstID");
        return -EINVAL;
    }

    adv_insts[inst_type].id = inst_id;

    return 0;
}

int bt_mesh_adv_inst_deinit(enum bt_mesh_adv_inst_type inst_type)
{
    BT_DBG("AdvInstDeinit, InstType %u", inst_type);

    if (inst_type >= BLE_MESH_ADV_INST_TYPES_NUM) {
        BT_ERR("Invalid adv inst type %d", inst_type);
        return -EINVAL;
    }

    BT_DBG("InstID %u", adv_insts[inst_type].id);

    bt_le_ext_adv_stop(adv_insts[inst_type].id);

    adv_insts[inst_type].id = BLE_MESH_ADV_INST_UNUSED;
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    adv_insts[inst_type].spt_mask = 0;
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */

    return 0;
}
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

static struct bt_mesh_adv *adv_alloc(int id, enum bt_mesh_adv_type type)
{
    BT_DBG("AdvAlloc, ID %d", id);
    init_adv_with_defaults(&adv_pool[id], type);
    return &adv_pool[id];
}

#if CONFIG_BLE_MESH_EXT_ADV
struct bt_mesh_adv *ext_adv_alloc(int id, enum bt_mesh_adv_type type)
{
    init_adv_with_defaults(&ext_adv_pool[id].adv, type);
    ext_adv_pool[id].primary_phy = BLE_MESH_ADV_PRI_PHY_DEFAULT;
    ext_adv_pool[id].secondary_phy = BLE_MESH_ADV_SEC_PHY_DEFAULT;
    ext_adv_pool[id].include_tx_power = BLE_MESH_TX_POWER_INCLUDE_DEFAULT;
    ext_adv_pool[id].tx_power = BLE_MESH_TX_POWER_DEFAULT;
    return &ext_adv_pool[id].adv;
}
#if CONFIG_BLE_MESH_EXT_RELAY_ADV_BUF_COUNT
struct bt_mesh_adv *ext_relay_adv_alloc(int id, enum bt_mesh_adv_type type)
{
    init_adv_with_defaults(&ext_relay_adv_pool[id].adv, type);
    ext_relay_adv_pool[id].primary_phy = BLE_MESH_ADV_PRI_PHY_DEFAULT;
    ext_relay_adv_pool[id].secondary_phy = BLE_MESH_ADV_SEC_PHY_DEFAULT;
    ext_relay_adv_pool[id].include_tx_power = BLE_MESH_TX_POWER_INCLUDE_DEFAULT;
    ext_relay_adv_pool[id].tx_power = BLE_MESH_TX_POWER_DEFAULT;
    return &ext_relay_adv_pool[id].adv;
}
#endif /* CONFIG_BLE_MESH_EXT_RELAY_ADV_BUF_COUNT */
#endif /* CONFIG_BLE_MESH_EXT_ADV */

#if CONFIG_BLE_MESH_LONG_PACKET
struct bt_mesh_adv *ext_long_adv_alloc(int id, enum bt_mesh_adv_type type)
{
    init_adv_with_defaults(&ext_long_adv_pool[id].adv, type);
    ext_long_adv_pool[id].primary_phy = BLE_MESH_ADV_PRI_PHY_DEFAULT;
    ext_long_adv_pool[id].secondary_phy = BLE_MESH_ADV_SEC_PHY_DEFAULT;
    ext_long_adv_pool[id].include_tx_power = BLE_MESH_TX_POWER_INCLUDE_DEFAULT;
    ext_long_adv_pool[id].tx_power = BLE_MESH_TX_POWER_DEFAULT;
    return &ext_long_adv_pool[id].adv;
}
#if CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT
struct bt_mesh_adv *ext_long_relay_adv_alloc(int id, enum bt_mesh_adv_type type)
{
    init_adv_with_defaults(&ext_long_relay_adv_pool[id].adv, type);
    ext_long_relay_adv_pool[id].primary_phy = BLE_MESH_ADV_PRI_PHY_DEFAULT;
    ext_long_relay_adv_pool[id].secondary_phy = BLE_MESH_ADV_SEC_PHY_DEFAULT;
    ext_long_relay_adv_pool[id].include_tx_power = BLE_MESH_TX_POWER_INCLUDE_DEFAULT;
    ext_long_relay_adv_pool[id].tx_power = BLE_MESH_TX_POWER_DEFAULT;
    return &ext_long_relay_adv_pool[id].adv;
}
#endif /* CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT */
#endif /* CONFIG_BLE_MESH_LONG_PACKET */

struct bt_mesh_adv_type_manager *bt_mesh_adv_types_mgmt_get(enum bt_mesh_adv_type adv_type)
{
    BT_DBG("AdvTypeMgmtGet, AdvType %u", adv_type);

    return &adv_types[adv_type];
}

void bt_mesh_adv_buf_ref_debug(const char *func, struct net_buf *buf,
                               uint8_t ref_cmp, bt_mesh_buf_ref_flag_t flag)
{
    if (buf == NULL || func == NULL || flag >= BLE_MESH_BUF_REF_MAX) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("AdvBufRefDebug, BufRef %u RefCmp %u", buf->ref, ref_cmp);

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
void bt_mesh_adv_inst_type_add(enum bt_mesh_adv_inst_type inst_type,
                               enum bt_mesh_adv_type adv_type)
{
    BT_DBG("AdvInstTypeAdd, InstType %u AdvType %u", inst_type, adv_type);

    if (inst_type >= BLE_MESH_ADV_INST_TYPES_NUM) {
        BT_ERR("Invalid adv inst type %d", inst_type);
        return;
    }

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("Invalid adv type %d", adv_type);
        return;
    }

    adv_insts[inst_type].spt_mask |= BIT(adv_type);
}

void bt_mesh_adv_inst_type_rem(enum bt_mesh_adv_inst_type inst_type,
                               enum bt_mesh_adv_type adv_type)
{
    BT_DBG("AdvInstTypeRem, InstType %u AdvType %u", inst_type, adv_type);

    if (inst_type >= BLE_MESH_ADV_INST_TYPES_NUM) {
        BT_ERR("Invalid adv inst type %d", inst_type);
        return;
    }

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("Invalid adv type %d", adv_type);
        return;
    }

    adv_insts[inst_type].spt_mask &= ~BIT(adv_type);
}

void bt_mesh_adv_inst_type_clear(enum bt_mesh_adv_inst_type inst_type,
                                 enum bt_mesh_adv_type adv_type)
{
    BT_DBG("AdvInstTypeClear, InstType %u AdvType %u", inst_type, adv_type);

    if (inst_type >= BLE_MESH_ADV_INST_TYPES_NUM) {
        BT_ERR("Invalid adv inst type %d", inst_type);
        return;
    }

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("Invalid adv type %d", adv_type);
        return;
    }

    adv_insts[inst_type].spt_mask = 0;
}
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */

int bt_mesh_adv_queue_init(struct bt_mesh_adv_queue *adv_queue, uint16_t queue_size,
                           bt_mesh_adv_queue_send_cb_t cb)
{
    BT_DBG("AdvQueueInit, QueueSize %u", queue_size);

    if (!adv_queue || !queue_size || !cb) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_queue_init(&adv_queue->q, queue_size, sizeof(bt_mesh_msg_t));
    adv_queue->send = cb;

    return 0;
}

int bt_mesh_adv_queue_deinit(struct bt_mesh_adv_queue *adv_queue)
{
    BT_DBG("AdvQueueDeinit");

    if (!adv_queue) {
        BT_ERR("%s, Invalid parameter", __func__);
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
    BT_DBG("AdvTypeInit, AdvType %u", adv_type);

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("%s, Invalid adv type %d", __func__, adv_type);
        return;
    }

    if (!adv_queue || !buf_pool || !adv_alloc) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    adv_types[adv_type].adv_q = adv_queue;
    adv_types[adv_type].pool = buf_pool;
    adv_types[adv_type].pool_allocator = adv_alloc;
}

void bt_mesh_adv_type_deinit(enum bt_mesh_adv_type adv_type)
{
    BT_DBG("AdvTypeDeinit, AdvType %u", adv_type);

    if (adv_type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("%s, Invalid adv type %d", __func__, adv_type);
        return;
    }

    adv_types[adv_type].adv_q = NULL;
    adv_types[adv_type].pool = NULL;
    adv_types[adv_type].pool_allocator = NULL;
}

#if CONFIG_BLE_MESH_USE_BLE_50
int bt_mesh_adv_task_wakeup(uint32_t evt)
{
    BT_DBG("AdvTypeWakeup, Evt 0x%08lx", evt);

    xTaskNotify(adv_task.handle, evt, eSetBits);
    return 0;
}

bool bt_mesh_adv_task_wait(uint32_t wait_bits, uint32_t timeout, uint32_t *notify)
{
    BT_DBG("AdvTypeWait, WaitBits 0x%08lx Timeout %lu", wait_bits, timeout);

    return (xTaskNotifyWait(wait_bits, UINT32_MAX, notify, K_WAIT(timeout)) == pdTRUE);
}
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
bool bt_mesh_adv_task_wait(uint32_t timeout)
{
    BT_DBG("AdvTypeWait, Timeout %lu", timeout);

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

    BT_DBG("PDUDuration %u", duration);

    return duration;
}

struct net_buf *bt_mesh_adv_create_from_pool(enum bt_mesh_adv_type type, int32_t timeout)
{
    struct bt_mesh_adv *adv = NULL;
    struct net_buf *buf = NULL;

    BT_DBG("AdvCreateFromPool, Type %u", type);

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_SUSPENDED)) {
        BT_WARN("Refusing to allocate buffer while suspended");
        return NULL;
    }

    if (type >= BLE_MESH_ADV_TYPES_NUM) {
        BT_ERR("%s, Invalid adv type %u", __func__, type);
        return NULL;
    }

    if (adv_types[type].pool == NULL || adv_types[type].pool_allocator == NULL) {
        BT_ERR("Uninitialized adv type %d", type);
        return NULL;
    }

    bt_mesh_r_mutex_lock(&adv_buf_alloc_lock);

    buf = net_buf_alloc(adv_types[type].pool, timeout);
    if (!buf) {
        BT_WARN("net buf alloc failed");
        bt_mesh_r_mutex_unlock(&adv_buf_alloc_lock);
        return NULL;
    }

    BT_DBG("Pool %p BufCount %u UinitCount %u BufID %d Ref %u",
           adv_types[type].pool, adv_types[type].pool->buf_count,
           adv_types[type].pool->uninit_count, net_buf_id(buf),
           buf->ref);

    adv = adv_types[type].pool_allocator(net_buf_id(buf), type);
    BLE_MESH_ADV(buf) = adv;
    bt_mesh_r_mutex_unlock(&adv_buf_alloc_lock);

    return buf;
}

void bt_mesh_unref_buf_from_pool(struct net_buf_pool *pool)
{
    BT_DBG("UnrefBufFromPool");

    if (pool == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    for (int i = 0; i < pool->buf_count; i++) {
        struct net_buf *buf = &pool->__bufs[i];

        BT_DBG("%u: Buf %p Ref %u", i, buf, buf->ref);

        if (buf->ref > 1U) {
            buf->ref = 1U;
        }
        net_buf_unref(buf);
    }
}

void bt_mesh_unref_buf(bt_mesh_msg_t *msg)
{
    struct net_buf *buf = msg->arg;

    BT_DBG("UnRefBuf %p", buf);

    if (buf) {
        BT_DBG("Ref %u", buf->ref);

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

    BT_DBG("GenericAdvSend");
    BT_DBG("Src 0x%04x Dst 0x%04x Type 0x%02x",
           src, dst, BLE_MESH_ADV(buf)->type);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

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

        BT_DBG("RelayAdvData, Timestamp %lu", msg.timestamp);
    }
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

    assert(adv_types[BLE_MESH_ADV(buf)->type].adv_q);
    assert(adv_types[BLE_MESH_ADV(buf)->type].adv_q->send);

    adv_types[BLE_MESH_ADV(buf)->type].adv_q->send(&msg, portMAX_DELAY, front);

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_task_wakeup(ADV_TASK_PKT_SEND_EVT);
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
}

struct bt_mesh_adv_queue *bt_mesh_adv_queue_get(void)
{
    return &adv_queue;
}

void bt_mesh_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front)
{
    BT_DBG("TaskPost, Front %u", front);

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

    BT_DBG("IgnoreRelayPacket");
    BT_DBG("Now %lu Timestamp %lu Interval %lu", now, timestamp, interval);

    return ((interval >= BLE_MESH_RELAY_TIME_INTERVAL) ? true : false);
}

static struct bt_mesh_adv *relay_adv_alloc(int id, enum bt_mesh_adv_type type)
{
    BT_DBG("RelayAdvAlloc, ID %d", id);
    memset(&relay_adv_pool[id], 0, sizeof(struct bt_mesh_adv));
    init_adv_with_defaults(&relay_adv_pool[id], type);
    return &relay_adv_pool[id];
}

static void bt_mesh_relay_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front)
{
    bt_mesh_msg_t old_msg = {0};

    BT_DBG("RelayTaskPost, Front %u", front);

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
    uint16_t count = (uint16_t)uxQueueMessagesWaiting(relay_adv_queue.q.handle);

    BT_DBG("StoredRelayCount %u", count);

    return count;
}

void bt_mesh_relay_adv_init(void)
{
    BT_DBG("RelayAdvInit");
    bt_mesh_adv_queue_init(&relay_adv_queue, bt_mesh_relay_adv_buf_count_get(),
                           bt_mesh_relay_task_post);
    bt_mesh_adv_type_init(BLE_MESH_ADV_RELAY_DATA, &relay_adv_queue,
                          &relay_adv_buf_pool, &relay_adv_alloc);
#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_RELAY_DATA, &relay_adv_queue,
                          &ext_adv_buf_pool, &ext_relay_adv_alloc);
#if CONFIG_BLE_MESH_LONG_PACKET && CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_LONG_RELAY_DATA, &relay_adv_queue,
                          &ext_long_relay_adv_buf_pool, ext_long_relay_adv_alloc);
#endif /* CONFIG_BLE_MESH_LONG_PACKET && CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT */
#endif /* CONFIG_BLE_MESH_EXT_ADV */

#if CONFIG_BLE_MESH_USE_BLE_50
#if CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE
    bt_mesh_adv_inst_init(BLE_MESH_RELAY_ADV_INST,
                          CONFIG_BLE_MESH_RELAY_ADV_INST_ID);
    bt_mesh_adv_inst_type_add(BLE_MESH_RELAY_ADV_INST, BLE_MESH_ADV_RELAY_DATA);

#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_inst_type_add(BLE_MESH_RELAY_ADV_INST, BLE_MESH_ADV_EXT_RELAY_DATA);
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_inst_type_add(BLE_MESH_RELAY_ADV_INST, BLE_MESH_ADV_EXT_LONG_RELAY_DATA);
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
#else  /* CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE */
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_RELAY_DATA);
#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_RELAY_DATA);
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_LONG_RELAY_DATA);
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
#endif /* CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_relay_adv_deinit(void)
{
    BT_DBG("RelayAdvDeinit");

    bt_mesh_adv_queue_deinit(&relay_adv_queue);

    bt_mesh_adv_type_deinit(BLE_MESH_ADV_RELAY_DATA);
#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_RELAY_DATA);
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_LONG_RELAY_DATA);
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */

#if CONFIG_BLE_MESH_USE_BLE_50
#if CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE
    bt_mesh_adv_inst_type_rem(BLE_MESH_RELAY_ADV_INST, BLE_MESH_ADV_RELAY_DATA);
#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_inst_type_rem(BLE_MESH_RELAY_ADV_INST, BLE_MESH_ADV_EXT_RELAY_DATA);
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_inst_type_rem(BLE_MESH_RELAY_ADV_INST, BLE_MESH_ADV_EXT_LONG_RELAY_DATA);
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
    bt_mesh_adv_inst_deinit(BLE_MESH_RELAY_ADV_INST);
#else /* CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE */
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_type_rem(BLE_MESH_ADV_INST, BLE_MESH_ADV_RELAY_DATA);
#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_inst_type_rem(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_RELAY_DATA);
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_inst_type_rem(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_LONG_RELAY_DATA);
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
#endif /* CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

    bt_mesh_unref_buf_from_pool(&relay_adv_buf_pool);
    memset(relay_adv_pool, 0, sizeof(relay_adv_pool));
}
#endif /* CONFIG_BLE_MESH_DEINIT */
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

#if CONFIG_BLE_MESH_FRIEND
static struct bt_mesh_adv *bt_mesh_frnd_adv_buf_get(int idx, enum bt_mesh_adv_type type)
{
    BT_DBG("FrndAdvBufGet, Idx %d", idx);

    memset(&frnd_adv_pool[idx].adv, 0, sizeof(struct bt_mesh_adv));
    init_adv_with_defaults(&frnd_adv_pool[idx].adv, type);
    frnd_adv_pool[idx].app_idx = BLE_MESH_KEY_UNUSED;
    return &frnd_adv_pool[idx].adv;
}

void bt_mesh_frnd_adv_init(void)
{
    BT_DBG("FrndAdvInit");

    bt_mesh_adv_type_init(BLE_MESH_ADV_FRIEND, &adv_queue, &friend_buf_pool, bt_mesh_frnd_adv_buf_get);

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_FRIEND);
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
}

void bt_mesh_frnd_adv_deinit(void)
{
    BT_DBG("FrndAdvDeinit");

    bt_mesh_adv_type_deinit(BLE_MESH_ADV_FRIEND);

#if CONFIG_BLE_MESH_FRIEND && CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_adv_inst_type_rem(BLE_MESH_ADV_INST, BLE_MESH_ADV_FRIEND);
#endif /* CONFIG_BLE_MESH_FRIEND */

    bt_mesh_unref_buf_from_pool(&friend_buf_pool);
    memset(frnd_adv_pool, 0, sizeof(frnd_adv_pool));
}
#endif /* CONFIG_BLE_MESH_FRIEND */

void bt_mesh_adv_task_init(void adv_thread(void *p))
{
    BT_DBG("AdvTaskInit");

    if (!adv_thread) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

#if (CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && \
     (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && \
     CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY)
    adv_task.task = heap_caps_calloc(1, sizeof(StaticTask_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    assert(adv_task.task);

    adv_task.stack = heap_caps_calloc_prefer(1, BLE_MESH_ADV_TASK_STACK_SIZE * sizeof(StackType_t),
                                             2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT,
                                             MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    assert(adv_task.stack);

    adv_task.handle = xTaskCreateStaticPinnedToCore(adv_thread, BLE_MESH_ADV_TASK_NAME,
                                                    BLE_MESH_ADV_TASK_STACK_SIZE, NULL,
                                                    BLE_MESH_ADV_TASK_PRIO, adv_task.stack,
                                                    adv_task.task, BLE_MESH_ADV_TASK_CORE);
    assert(adv_task.handle);
#else /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY */
    int ret = xTaskCreatePinnedToCore(adv_thread, BLE_MESH_ADV_TASK_NAME, BLE_MESH_ADV_TASK_STACK_SIZE, NULL,
                                      BLE_MESH_ADV_TASK_PRIO, &adv_task.handle, BLE_MESH_ADV_TASK_CORE);
#if CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE
    if (ret != pdTRUE) {
        BT_ERR("xTaskCreatePinnedToCore failed, ret %d", ret);
        return;
    }
#else
    assert(ret == pdTRUE);
#endif /* CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE */
#endif /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY */
}

void bt_mesh_adv_common_init(void)
{
    BT_DBG("AdvCommonInit");

    bt_mesh_r_mutex_create(&adv_buf_alloc_lock);
    bt_mesh_adv_queue_init(&adv_queue, bt_mesh_adv_buf_count_get(), bt_mesh_task_post);
    bt_mesh_adv_type_init(BLE_MESH_ADV_PROV, &adv_queue, &adv_buf_pool, adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_DATA, &adv_queue, &adv_buf_pool, adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_BEACON, &adv_queue, &adv_buf_pool, adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_URI, &adv_queue, &adv_buf_pool, adv_alloc);
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
    bt_mesh_adv_type_init(BLE_MESH_ADV_PROXY_SOLIC, &adv_queue, &adv_buf_pool, adv_alloc);
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX */

#if CONFIG_BLE_MESH_USE_BLE_50
    bt_mesh_adv_inst_init(BLE_MESH_ADV_INST, CONFIG_BLE_MESH_ADV_INST_ID);
#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_PROV, &adv_queue, &ext_adv_buf_pool, ext_adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_DATA, &adv_queue, &ext_adv_buf_pool, ext_adv_alloc);
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF && CONFIG_BLE_MESH_EXT_RELAY_ADV_BUF_COUNT
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_RELAY_DATA, &adv_queue, &ext_relay_adv_buf_pool, ext_relay_adv_alloc);
#endif
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_LONG_PROV, &adv_queue, &ext_long_adv_buf_pool, ext_long_adv_alloc);
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_LONG_DATA, &adv_queue, &ext_long_adv_buf_pool, ext_long_adv_alloc);
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF && CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT
    bt_mesh_adv_type_init(BLE_MESH_ADV_EXT_LONG_RELAY_DATA, &adv_queue, &ext_long_relay_adv_buf_pool, ext_long_relay_adv_alloc);
#endif /* !CONFIG_BLE_MESH_RELAY_ADV_BUF && CONFIG_BLE_MESH_LONG_PACKET_RELAY_ADV_BUF_COUNT */
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    /* Due to the limitation of the sequence number in the network layer,
     * it is not possible to use multiple advertising instances to process
     * data from the same message queue when sending mesh packets.
     *
     * Therefore, shall to check whether there are duplicates in the queue
     * buffer corresponding to each advertising instance.
     */
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_PROV);
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_DATA);
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_BEACON);
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_URI);
#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_PROV);
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_DATA);
#if CONFIG_BLE_MESH_RELAY && !CONFIG_BLE_MESH_RELAY_ADV_BUF
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_RELAY_DATA);
#endif
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_LONG_PROV);
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_LONG_DATA);
#if CONFIG_BLE_MESH_RELAY && !CONFIG_BLE_MESH_RELAY_ADV_BUF
    bt_mesh_adv_inst_type_add(BLE_MESH_ADV_INST, BLE_MESH_ADV_EXT_LONG_RELAY_DATA);
#endif /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_adv_task_deinit(void)
{
    BT_DBG("AdvTaskDeinit");

    if (adv_task.handle) {
        vTaskDelete(adv_task.handle);
        adv_task.handle = NULL;
    }

#if (CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL && \
    (CONFIG_SPIRAM_CACHE_WORKAROUND || !CONFIG_IDF_TARGET_ESP32) && \
     CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY)
    /* Under specific configurations, bt_mesh_adv_task_deinit immediately releases
     * adv_task.stack and adv_task.task (StaticTask_t) using heap_caps_free.
     * However, vTaskDelete(adv_task.handle) only marks the task for deletion and
     * adds it to the xTasksWaitingTermination list, which will be processed later
     * by the Idle task (calling prvDeleteTCB). Even though static tasks are not
     * automatically released by FreeRTOS, the Idle task will still access the TCB
     * (checking fields such as ucStaticallyAllocated, resetting states). Premature
     * release leads to a use-after-free (UAF) by the Idle task.
     * Additionally, if the task is still running (possible in multi-core scenarios),
     * releasing the stack may cause the task to execute with an invalid stack.
     */
    vTaskDelay(pdMS_TO_TICKS(100));
    if (adv_task.stack) {
        heap_caps_free(adv_task.stack);
        adv_task.stack = NULL;
    }
    if (adv_task.task) {
        heap_caps_free(adv_task.task);
        adv_task.task = NULL;
    }
#endif
}

void bt_mesh_adv_common_deinit(void)
{
    BT_DBG("AdvCommonDeinit");

    bt_mesh_adv_type_deinit(BLE_MESH_ADV_PROV);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_DATA);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_BEACON);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_URI);

#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_PROXY_SOLIC);
#endif

#if CONFIG_BLE_MESH_EXT_ADV
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_PROV);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_DATA);
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_RELAY_DATA);
#endif
#if CONFIG_BLE_MESH_LONG_PACKET
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_LONG_PROV);
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_LONG_DATA);
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF
    bt_mesh_adv_type_deinit(BLE_MESH_ADV_EXT_LONG_RELAY_DATA);
#endif
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */

    bt_mesh_adv_queue_deinit(&adv_queue);

#if CONFIG_BLE_MESH_USE_BLE_50
    bt_mesh_adv_inst_deinit(BLE_MESH_ADV_INST);
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

    bt_mesh_unref_buf_from_pool(&adv_buf_pool);
    memset(adv_pool, 0, sizeof(adv_pool));

    bt_mesh_r_mutex_free(&adv_buf_alloc_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */
