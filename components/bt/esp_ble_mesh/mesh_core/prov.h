/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _PROV_H_
#define _PROV_H_

#include "mesh_main.h"
#include "mesh_bearer_adapt.h"

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf);

bool bt_prov_active(void);

int bt_mesh_pb_gatt_open(struct bt_mesh_conn *conn);
int bt_mesh_pb_gatt_close(struct bt_mesh_conn *conn);
int bt_mesh_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf);

int bt_mesh_set_oob_pub_key(const u8_t pub_key_x[32], const u8_t pub_key_y[32],
                            const u8_t pri_key[32]);

const struct bt_mesh_prov *bt_mesh_prov_get(void);

int bt_mesh_prov_init(const struct bt_mesh_prov *prov);
int bt_mesh_prov_deinit(void);

void bt_mesh_prov_complete(u16_t net_idx, const u8_t net_key[16], u16_t addr, u8_t flags, u32_t iv_index);
void bt_mesh_prov_reset(void);

#endif /* _PROV_H_ */
