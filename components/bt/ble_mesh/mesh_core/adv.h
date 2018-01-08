/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh_bearer_adapt.h"

/* Maximum advertising data payload for a single data type */
#define BT_MESH_ADV_DATA_SIZE 29

/* The user data is a pointer (4 bytes) to struct bt_mesh_adv */
#define BT_MESH_ADV_USER_DATA_SIZE 4

#define BT_MESH_ADV(buf) (*(struct bt_mesh_adv **)net_buf_user_data(buf))

typedef struct ble_mesh_msg {
    uint8_t sig;    //event signal
    uint8_t aid;    //application id
    uint8_t pid;    //profile id
    uint8_t act;    //profile action, defined in seprerate header files
    void   *arg;    //param for btc function or function param
} ble_mesh_msg_t;

enum bt_mesh_adv_type {
    BT_MESH_ADV_PROV,
    BT_MESH_ADV_DATA,
    BT_MESH_ADV_BEACON,
};

typedef void (*bt_mesh_adv_func_t)(struct net_buf *buf, u16_t duration,
                                   int err, void *user_data);

struct bt_mesh_adv {
    const struct bt_mesh_send_cb *cb;
    void *cb_data;

    u8_t      type: 2,
              busy: 1;
    u8_t      count: 3,
              adv_int: 5;
    union {
        /* Address, used e.g. for Friend Queue messages */
        u16_t addr;

        /* For transport layer segment sending */
        struct {
            u8_t attempts;
        } seg;
    };
};

typedef struct bt_mesh_adv *(*bt_mesh_adv_alloc_t)(int id);

/* xmit_count: Number of retransmissions, i.e. 0 == 1 transmission */
struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit_count,
                                   u8_t xmit_int, s32_t timeout);

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
        bt_mesh_adv_alloc_t get_id,
        enum bt_mesh_adv_type type,
        u8_t xmit_count, u8_t xmit_int,
        s32_t timeout);

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                      void *cb_data);

const bt_addr_le_t *bt_mesh_pba_get_addr(void);

void bt_mesh_adv_update(void);

void bt_mesh_adv_init(void);

int bt_mesh_scan_enable(void);

int bt_mesh_scan_disable(void);

void ble_mesh_task_post(ble_mesh_msg_t *msg, uint32_t timeout);

