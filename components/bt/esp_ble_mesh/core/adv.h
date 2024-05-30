/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADV_H_
#define _ADV_H_

#include "mesh/atomic.h"
#include "mesh/access.h"
#include "mesh/adapter.h"
#include "mesh/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum advertising data payload for a single data type */
#define BLE_MESH_ADV_DATA_SIZE          29

/* The user data is a pointer (4 bytes) to struct bt_mesh_adv */
#define BLE_MESH_ADV_USER_DATA_SIZE     4

#define BLE_MESH_ADV(buf)               (*(struct bt_mesh_adv **)net_buf_user_data(buf))
#define BLE_MESH_ADV_BUSY(buf)          (BLE_MESH_ADV(buf)->busy)

uint16_t bt_mesh_pdu_duration(uint8_t xmit);

typedef struct bt_mesh_msg {
    bool     relay;     /* Flag indicates if the packet is a relayed one */
    void    *arg;       /* Pointer to the struct net_buf */
    uint16_t src;       /* Source address for relay packets */
    uint16_t dst;       /* Destination address for relay packets */
    uint32_t timestamp; /* Timestamp recorded when the relay packet is posted to queue */
} bt_mesh_msg_t;

enum bt_mesh_adv_type {
    BLE_MESH_ADV_PROV,
    BLE_MESH_ADV_DATA,
    BLE_MESH_ADV_BEACON,
    BLE_MESH_ADV_URI,
    BLE_MESH_ADV_BLE,
    BLE_MESH_ADV_PROXY_SOLIC,
};

struct bt_mesh_adv {
    const struct bt_mesh_send_cb *cb;
    void *cb_data;

    uint8_t type:3;

    bt_mesh_atomic_t busy;

    uint8_t xmit;
};

typedef struct bt_mesh_adv *(*bt_mesh_adv_alloc_t)(int id);

struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, int32_t timeout);

typedef enum {
    BLE_MESH_BUF_REF_EQUAL,
    BLE_MESH_BUF_REF_SMALL,
    BLE_MESH_BUF_REF_MAX,
} bt_mesh_buf_ref_flag_t;

#if CONFIG_BLE_MESH_USE_BLE_50

#define BLE_MESH_ADV_INS_UNUSED 0xFF
#define BLE_MESH_ADV_INS_CNT    1

struct bt_mesh_adv_inst {
    uint8_t id;
};

int ble_mesh_adv_task_wakeup(uint16_t adv_inst_id);
bool bt_mesh_is_adv_inst_used(uint8_t adv_inst_id);
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

void bt_mesh_adv_buf_ref_debug(const char *func, struct net_buf *buf,
                               uint8_t ref_cmp, bt_mesh_buf_ref_flag_t flag);

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
                                             bt_mesh_adv_alloc_t get_id,
                                             enum bt_mesh_adv_type type,
                                             int32_t timeout);

void bt_mesh_unref_buf_from_pool(struct net_buf_pool *pool);

void bt_mesh_adv_send(struct net_buf *buf, uint8_t xmit,
                      const struct bt_mesh_send_cb *cb,
                      void *cb_data);

#if CONFIG_BLE_MESH_USE_BLE_50 && (CONFIG_BLE_MESH_GATT_PROXY_SERVER || CONFIG_BLE_MESH_PB_GATT)
uint8_t bt_mesh_get_proxy_inst(void);
#endif

struct net_buf *bt_mesh_relay_adv_create(enum bt_mesh_adv_type type, int32_t timeout);

void bt_mesh_relay_adv_send(struct net_buf *buf, uint8_t xmit,
                            uint16_t src, uint16_t dst,
                            const struct bt_mesh_send_cb *cb,
                            void *cb_data);

uint16_t bt_mesh_get_stored_relay_count(void);

void bt_mesh_adv_update(void);

void bt_mesh_adv_init(void);
void bt_mesh_adv_deinit(void);

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
int bt_mesh_start_ble_advertising(const struct bt_mesh_ble_adv_param *param,
                                  const struct bt_mesh_ble_adv_data *data, uint8_t *index);

int bt_mesh_stop_ble_advertising(uint8_t index);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

#ifdef __cplusplus
}
#endif

#endif /* _ADV_H_ */
