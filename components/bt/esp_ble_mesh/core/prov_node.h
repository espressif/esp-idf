/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _PROV_NODE_H_
#define _PROV_NODE_H_

#include "mesh/main.h"
#include "mesh/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf);

bool bt_mesh_prov_active(void);

int bt_mesh_pb_gatt_open(struct bt_mesh_conn *conn);
int bt_mesh_pb_gatt_close(struct bt_mesh_conn *conn, uint8_t reason);
int bt_mesh_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf);

int bt_mesh_set_oob_pub_key(const uint8_t pub_key_x[32], const uint8_t pub_key_y[32],
                            const uint8_t pri_key[32]);

int bt_mesh_prov_init(void);
int bt_mesh_prov_deinit(void);

void bt_mesh_prov_complete(uint16_t net_idx, const uint8_t net_key[16],
                           uint16_t addr, uint8_t flags, uint32_t iv_index);
void bt_mesh_prov_reset(void);

struct bt_mesh_prov_link *bt_mesh_prov_node_get_link(void);

void bt_mesh_rpr_srv_reset_prov_link(struct bt_mesh_prov_link *link, uint8_t reason);

int bt_mesh_rpr_srv_nppi_pdu_recv(uint8_t type, const uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif /* _PROV_NODE_H_ */
