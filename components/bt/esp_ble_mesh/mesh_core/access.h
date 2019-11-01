/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ACCESS_H_
#define _ACCESS_H_

#include "mesh_access.h"
#include "mesh_buf.h"
#include "net.h"

/* bt_mesh_model.flags */
enum {
    BLE_MESH_MOD_BIND_PENDING = BIT(0),
    BLE_MESH_MOD_SUB_PENDING = BIT(1),
    BLE_MESH_MOD_PUB_PENDING = BIT(2),
};

void bt_mesh_elem_register(struct bt_mesh_elem *elem, u8_t count);

u8_t bt_mesh_elem_count(void);

/* Find local element based on unicast or group address */
struct bt_mesh_elem *bt_mesh_elem_find(u16_t addr);

struct bt_mesh_model *bt_mesh_model_find_vnd(struct bt_mesh_elem *elem,
        u16_t company, u16_t id);
struct bt_mesh_model *bt_mesh_model_find(struct bt_mesh_elem *elem,
        u16_t id);

u16_t *bt_mesh_model_find_group(struct bt_mesh_model *mod, u16_t addr);

bool bt_mesh_fixed_group_match(u16_t addr);

void bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod,
                                        struct bt_mesh_elem *elem,
                                        bool vnd, bool primary,
                                        void *user_data),
                           void *user_data);

s32_t bt_mesh_model_pub_period_get(struct bt_mesh_model *mod);

void bt_mesh_comp_provision(u16_t addr);
void bt_mesh_comp_unprovision(void);

u16_t bt_mesh_primary_addr(void);

const struct bt_mesh_comp *bt_mesh_comp_get(void);

struct bt_mesh_model *bt_mesh_model_get(bool vnd, u8_t elem_idx, u8_t mod_idx);

void bt_mesh_model_recv(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);

int bt_mesh_comp_register(const struct bt_mesh_comp *comp);

struct bt_mesh_subnet *bt_mesh_tx_netkey_get(u8_t role, u16_t net_idx);

const u8_t *bt_mesh_tx_devkey_get(u8_t role, u16_t dst);

struct bt_mesh_app_key *bt_mesh_tx_appkey_get(u8_t role, u16_t app_idx, u16_t net_idx);

size_t bt_mesh_rx_netkey_size(void);

struct bt_mesh_subnet *bt_mesh_rx_netkey_get(size_t index);

size_t bt_mesh_rx_devkey_size(void);

const u8_t *bt_mesh_rx_devkey_get(size_t index, u16_t src);

size_t bt_mesh_rx_appkey_size(void);

struct bt_mesh_app_key *bt_mesh_rx_appkey_get(size_t index);

#endif /* _ACCESS_H_ */
