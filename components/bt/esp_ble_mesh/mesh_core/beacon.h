/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BEACON_H_
#define _BEACON_H_

void bt_mesh_beacon_enable(void);
void bt_mesh_beacon_disable(void);

void bt_mesh_beacon_ivu_initiator(bool enable);

void bt_mesh_beacon_recv(struct net_buf_simple *buf);

void bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
                           struct net_buf_simple *buf);

void bt_mesh_beacon_init(void);

#endif /* _BEACON_H_ */
