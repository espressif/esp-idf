/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADV_COMMON_H_
#define _ADV_COMMON_H_

#include "mesh/common.h"
#include "mesh/atomic.h"
#include "mesh/access.h"
#include "mesh/adapter.h"
#include "mesh/queue.h"
#include "mesh/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#if CONFIG_BLE_MESH_HCI_5_0
#define ADV_ITVL_MIN        20
#else
#define ADV_ITVL_MIN        100
#endif

/* Convert from ms to 0.625ms units */
#define ADV_SCAN_UNIT(_ms)  ((_ms) * 8 / 5)
/* Convert from 0.625ms units to interval(ms) */
#define ADV_SCAN_INT(val)   ((val) * 5 / 8)

/* Maximum advertising data payload for a single data type */
#define BLE_MESH_ADV_DATA_SIZE          29

/* The user data is a pointer (4 bytes) to struct bt_mesh_adv */
#define BLE_MESH_ADV_USER_DATA_SIZE     4

#define BLE_MESH_ADV(buf)               (*(struct bt_mesh_adv **)net_buf_user_data(buf))
#define BLE_MESH_ADV_BUSY(buf)          (BLE_MESH_ADV(buf)->busy)

#define BLE_MESH_MSG_NET_BUF(msg)       ((struct net_buf *)(msg->arg))

#define BLE_MESH_ADV_INS_UNUSED 0xFF

/* We reserve one queue item for bt_mesh_adv_update() */
#define BLE_MESH_ADV_QUEUE_SIZE     (CONFIG_BLE_MESH_ADV_BUF_COUNT + 1)

struct bt_mesh_adv {
    const struct bt_mesh_send_cb *cb;
    void *cb_data;

    uint8_t type;

    bt_mesh_atomic_t busy;

    uint8_t xmit;
};

#if CONFIG_BLE_MESH_FRIEND

#define FRIEND_ADV(buf)     CONTAINER_OF(BLE_MESH_ADV(buf), bt_mesh_friend_adv_t, adv)

typedef struct {
    struct bt_mesh_adv adv;
    uint16_t app_idx;
} bt_mesh_friend_adv_t;

#endif

enum {
#if CONFIG_BLE_MESH_USE_BLE_50
    ADV_TASK_MESH_ADV_INST_EVT = BIT(CONFIG_BLE_MESH_ADV_INST_ID),

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    ADV_TASK_PROX_ADV_INST_EVT = BIT(CONFIG_BLE_MESH_PROXY_ADV_INST_ID),
#endif

#if CONFIG_BLE_MESH_SEPARATE_RELAY_ADV_INSTANCE
    ADV_TASK_RELAY_ADV_INST_EVT = BIT(CONFIG_BLE_MESH_RELAY_ADV_INST_ID),
#endif

#if CONFIG_BLE_MESH_SEPARATE_BLE_ADV_INSTANCE
    ADV_TASK_BLE_ADV_INST_EVT = BIT(CONFIG_BLE_MESH_BLE_ADV_INST_ID),
#endif
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    ADV_TASK_PROXY_ADV_UPD_EVT = BIT(30),
#endif
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
    ADV_TASK_PKT_SEND_EVT = BIT(31),
    ADV_TASK_EVT_MAX,
};

uint16_t bt_mesh_pdu_duration(uint8_t xmit);

typedef struct bt_mesh_msg {
    bool     relay;     /* Flag indicates if the packet is a relayed one */
    void    *arg;       /* Pointer to the struct net_buf */
    uint16_t src;       /* Source address for relay packets */
    uint16_t dst;       /* Destination address for relay packets */
    uint32_t timestamp; /* Timestamp recorded when the relay packet is posted to queue */
} bt_mesh_msg_t;

typedef struct bt_mesh_adv *(*bt_mesh_pool_allocator_t)(int id);
typedef void (*bt_mesh_adv_queue_send_cb_t)(bt_mesh_msg_t *msg, uint32_t timeout, bool front);

struct bt_mesh_adv_queue {
    bt_mesh_queue_t q;
    bt_mesh_adv_queue_send_cb_t send;
};

struct bt_mesh_adv_inst {
    uint8_t id;
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bool busy;
    struct net_buf *sending_buf;

    /* indicates that which adv_type is supported by this instance */
    uint32_t spt_mask;
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
};

enum bt_mesh_adv_type {
    BLE_MESH_ADV_PROV,
    BLE_MESH_ADV_DATA,
#if CONFIG_BLE_MESH_FRIEND
    BLE_MESH_ADV_FRIEND,
#endif
#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    BLE_MESH_ADV_RELAY_DATA,
#endif
    BLE_MESH_ADV_BEACON,
    BLE_MESH_ADV_URI,
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
    BLE_MESH_ADV_PROXY_SOLIC,
#endif
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    BLE_MESH_ADV_BLE,
#endif
    BLE_MESH_ADV_TYPES_NUM,
};

typedef enum {
    BLE_MESH_BUF_REF_EQUAL,
    BLE_MESH_BUF_REF_SMALL,
    BLE_MESH_BUF_REF_MAX,
} bt_mesh_buf_ref_flag_t;

struct bt_mesh_adv_type_manager {
    struct bt_mesh_adv_queue *adv_q;
    struct net_buf_pool *pool;
   bt_mesh_pool_allocator_t pool_allocator;
};

static const uint8_t adv_type[] = {
    [BLE_MESH_ADV_PROV]   = BLE_MESH_DATA_MESH_PROV,
    [BLE_MESH_ADV_DATA]   = BLE_MESH_DATA_MESH_MESSAGE,
#if CONFIG_BLE_MESH_FRIEND
    [BLE_MESH_ADV_FRIEND]   = BLE_MESH_DATA_MESH_MESSAGE,
#endif
#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    [BLE_MESH_ADV_RELAY_DATA] = BLE_MESH_DATA_MESH_MESSAGE,
#endif
    [BLE_MESH_ADV_BEACON] = BLE_MESH_DATA_MESH_BEACON,
    [BLE_MESH_ADV_URI]    = BLE_MESH_DATA_URI,
};

static inline TickType_t K_WAIT(int32_t val)
{
    return (val == K_FOREVER) ? portMAX_DELAY : (val / portTICK_PERIOD_MS);
}

struct bt_mesh_adv_queue *bt_mesh_adv_queue_get(void);

struct net_buf *bt_mesh_adv_create_from_pool(enum bt_mesh_adv_type type,
                                             int32_t timeout);

static inline struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, int32_t timeout)
{
    return bt_mesh_adv_create_from_pool(type, timeout);
}

void bt_mesh_adv_buf_ref_debug(const char *func, struct net_buf *buf,
                               uint8_t ref_cmp, bt_mesh_buf_ref_flag_t flag);

struct bt_mesh_adv_type_manager *bt_mesh_adv_types_mgnt_get(enum bt_mesh_adv_type adv_type);

void bt_mesh_generic_adv_send(struct net_buf *buf, uint8_t xmit,
                              const struct bt_mesh_send_cb *cb,
                              void *cb_data, uint16_t src,
                              uint16_t dst, bool front);

void bt_mesh_unref_buf_from_pool(struct net_buf_pool *pool);
void bt_mesh_unref_buf(bt_mesh_msg_t *msg);

int bt_mesh_adv_queue_init(struct bt_mesh_adv_queue *adv_queue,
                           uint16_t queue_size,
                           bt_mesh_adv_queue_send_cb_t cb);

int bt_mesh_adv_queue_deinit(struct bt_mesh_adv_queue *adv_queue);

void bt_mesh_adv_type_init(enum bt_mesh_adv_type adv_type,
                          struct bt_mesh_adv_queue *adv_queue,
                          struct net_buf_pool *buf_pool,
                          bt_mesh_pool_allocator_t adv_alloc);

void bt_mesh_adv_type_deinit(enum bt_mesh_adv_type adv_type);

void bt_mesh_task_post(bt_mesh_msg_t *msg, uint32_t timeout, bool front);

#if CONFIG_BLE_MESH_USE_BLE_50
struct bt_mesh_adv_inst * bt_mesh_get_adv_insts_set(void);
int bt_mesh_adv_inst_init(enum bt_mesh_adv_inst_type inst_type, uint8_t inst_id);
int bt_mesh_adv_inst_deinit(enum bt_mesh_adv_inst_type inst_type);
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
void bt_mesh_adv_inst_supported_adv_type_add(enum bt_mesh_adv_inst_type inst_type,
                                             enum bt_mesh_adv_type  adv_type);

void bt_mesh_adv_inst_supported_adv_type_rm(enum bt_mesh_adv_inst_type inst_type,
                                            enum bt_mesh_adv_type  adv_type);

void bt_mesh_adv_inst_supported_adv_type_clear(enum bt_mesh_adv_inst_type inst_type,
                                               enum bt_mesh_adv_type  adv_type);
#endif

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
void bt_mesh_relay_adv_init(void);
bool bt_mesh_ignore_relay_packet(uint32_t timestamp);
struct net_buf *bt_mesh_relay_adv_create(enum bt_mesh_adv_type type, int32_t timeout);

static inline void bt_mesh_relay_adv_send(struct net_buf *buf, uint8_t xmit,
                                          uint16_t src, uint16_t dst,
                                          const struct bt_mesh_send_cb *cb,
                                          void *cb_data)
{
    bt_mesh_generic_adv_send(buf, xmit, cb, cb_data, src, dst, false);
};

uint16_t bt_mesh_get_stored_relay_count(void);

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_relay_adv_deinit(void);
#endif
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

#if CONFIG_BLE_MESH_FRIEND
struct bt_mesh_adv *bt_mesh_frnd_adv_buf_get(int id);
struct net_buf_pool *bt_mesh_frnd_adv_pool_get(void);
void bt_mesh_frnd_adv_init(void);
#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_frnd_adv_deinit(void);
#endif /* CONFIG_BLE_MESH_DEINIT */
#endif /* CONFIG_BLE_MESH_FRIEND */

void bt_mesh_adv_task_init(void adv_thread(void *p));
void bt_mesh_adv_common_init(void);

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_adv_task_deinit(void);
void bt_mesh_adv_common_deinit(void);
#endif

#if CONFIG_BLE_MESH_USE_BLE_50
bool bt_mesh_is_adv_inst_used(uint8_t adv_inst_id);
bool ble_mesh_adv_task_wait(uint32_t wait_bits, TickType_t timeout, uint32_t *notify);
int ble_mesh_adv_task_wakeup(uint32_t evt);
#else
bool ble_mesh_adv_task_wait(uint32_t timeout);
#endif

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
int bt_mesh_start_ble_advertising(const struct bt_mesh_ble_adv_param *param,
                                  const struct bt_mesh_ble_adv_data *data, uint8_t *index);

int bt_mesh_stop_ble_advertising(uint8_t index);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

#ifdef __cplusplus
}
#endif

#endif /* _ADV_COMMON_H_ */
