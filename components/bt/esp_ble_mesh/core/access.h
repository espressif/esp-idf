/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ACCESS_H_
#define _ACCESS_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

/* bt_mesh_model.flags */
enum {
    BLE_MESH_MOD_BIND_PENDING = BIT(0),
    BLE_MESH_MOD_SUB_PENDING = BIT(1),
    BLE_MESH_MOD_PUB_PENDING = BIT(2),
};

void bt_mesh_elem_register(struct bt_mesh_elem *elem, uint8_t count);

uint8_t bt_mesh_elem_count(void);

/* Find local element based on unicast or group address */
struct bt_mesh_elem *bt_mesh_elem_find(uint16_t addr);

uint16_t *bt_mesh_model_find_group(struct bt_mesh_model *mod, uint16_t addr);

int bt_mesh_get_opcode(struct net_buf_simple *buf,
                       uint32_t *opcode, bool pull_buf);

bool bt_mesh_fixed_group_match(uint16_t addr);

bool bt_mesh_fixed_direct_match(struct bt_mesh_subnet *sub, uint16_t addr);

void bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod,
                                        struct bt_mesh_elem *elem,
                                        bool vnd, bool primary,
                                        void *user_data),
                           void *user_data);

int32_t bt_mesh_model_pub_period_get(struct bt_mesh_model *mod);

void bt_mesh_comp_provision(uint16_t addr);
void bt_mesh_comp_unprovision(void);

uint16_t bt_mesh_primary_addr(void);

const struct bt_mesh_comp *bt_mesh_comp_get(void);

struct bt_mesh_model *bt_mesh_model_get(bool vnd, uint8_t elem_idx, uint8_t mod_idx);

void bt_mesh_model_recv(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);

int bt_mesh_comp_register(const struct bt_mesh_comp *comp);
int bt_mesh_comp_deregister(void);

const uint8_t *bt_mesh_dev_key_get(uint16_t dst);

size_t bt_mesh_rx_netkey_size(void);

struct bt_mesh_subnet *bt_mesh_rx_netkey_get(size_t index);

size_t bt_mesh_rx_devkey_size(void);

const uint8_t *bt_mesh_rx_devkey_get(size_t index, uint16_t src);

size_t bt_mesh_rx_appkey_size(void);

struct bt_mesh_app_key *bt_mesh_rx_appkey_get(size_t index);

#ifdef __cplusplus
}
#endif

#endif /* _ACCESS_H_ */
