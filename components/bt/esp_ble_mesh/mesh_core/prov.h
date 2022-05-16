/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _PROV_H_
#define _PROV_H_

#include "mesh_main.h"
#include "mesh_bearer_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf);

bool bt_prov_active(void);

int bt_mesh_pb_gatt_open(struct bt_mesh_conn *conn);
int bt_mesh_pb_gatt_close(struct bt_mesh_conn *conn);
int bt_mesh_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf);

int bt_mesh_set_oob_pub_key(const uint8_t pub_key_x[32], const uint8_t pub_key_y[32],
                            const uint8_t pri_key[32]);

const struct bt_mesh_prov *bt_mesh_prov_get(void);

int bt_mesh_prov_init(const struct bt_mesh_prov *prov);
int bt_mesh_prov_deinit(void);

void bt_mesh_prov_complete(uint16_t net_idx, const uint8_t net_key[16],
                           uint16_t addr, uint8_t flags, uint32_t iv_index);
void bt_mesh_prov_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROV_H_ */
