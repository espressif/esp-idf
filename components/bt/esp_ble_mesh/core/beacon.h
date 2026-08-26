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

#define BEACON_TYPE_UNPROVISIONED   0x00
#define BEACON_TYPE_SECURE          0x01
#define BEACON_TYPE_PRIVATE         0x02

#define NET_IDX_SET(_val)           ((void *)((uint32_t)(_val)))
#define NET_IDX_GET(_ptr)           ((uint32_t)(_ptr))

/* If the interval has passed or is within 5 seconds from now send a beacon */
#define BEACON_THRESHOLD(last)      (K_SECONDS(10 * ((last) + 1)) - K_SECONDS(5))

void bt_mesh_secure_beacon_enable(void);
void bt_mesh_secure_beacon_disable(void);

void bt_mesh_beacon_ivu_initiator(bool enable);

void bt_mesh_beacon_recv(struct net_buf_simple *buf, int8_t rssi);

void bt_mesh_secure_beacon_create(struct bt_mesh_subnet *sub,
                                  struct net_buf_simple *buf);

void bt_mesh_beacon_init(void);

void bt_mesh_beacon_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _BEACON_H_ */
