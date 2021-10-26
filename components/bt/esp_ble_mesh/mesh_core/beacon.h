/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BEACON_H_
#define _BEACON_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_mesh_beacon_enable(void);
void bt_mesh_beacon_disable(void);

void bt_mesh_beacon_ivu_initiator(bool enable);

void bt_mesh_beacon_recv(struct net_buf_simple *buf, int8_t rssi);

void bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
                           struct net_buf_simple *buf);

void bt_mesh_beacon_init(void);
void bt_mesh_beacon_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _BEACON_H_ */
