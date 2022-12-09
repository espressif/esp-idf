/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LPN_H_
#define _LPN_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_mesh_lpn_friend_update(struct bt_mesh_net_rx *rx,
                              struct net_buf_simple *buf);
int bt_mesh_lpn_friend_offer(struct bt_mesh_net_rx *rx,
                             struct net_buf_simple *buf);
int bt_mesh_lpn_friend_clear_cfm(struct bt_mesh_net_rx *rx,
                                 struct net_buf_simple *buf);
int bt_mesh_lpn_friend_sub_cfm(struct bt_mesh_net_rx *rx,
                               struct net_buf_simple *buf);

static inline bool bt_mesh_lpn_established(void)
{
#if defined(CONFIG_BLE_MESH_LOW_POWER)
    return bt_mesh.lpn.established;
#else
    return false;
#endif
}

static inline bool bt_mesh_lpn_match(uint16_t addr)
{
#if defined(CONFIG_BLE_MESH_LOW_POWER)
    if (bt_mesh_lpn_established()) {
        return (addr == bt_mesh.lpn.frnd);
    }
#endif
    return false;
}

static inline bool bt_mesh_lpn_waiting_update(void)
{
#if defined(CONFIG_BLE_MESH_LOW_POWER)
    return (bt_mesh.lpn.state == BLE_MESH_LPN_WAIT_UPDATE);
#else
    return false;
#endif
}

static inline bool bt_mesh_lpn_timer(void)
{
#if defined(CONFIG_BLE_MESH_LPN_AUTO)
    return (bt_mesh.lpn.state == BLE_MESH_LPN_TIMER);
#else
    return false;
#endif
}

void bt_mesh_lpn_msg_received(struct bt_mesh_net_rx *rx);

void bt_mesh_lpn_group_add(uint16_t group);
void bt_mesh_lpn_group_del(uint16_t *groups, size_t group_count);

void bt_mesh_lpn_disable(bool force);

int bt_mesh_lpn_init(void);
int bt_mesh_lpn_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _LPN_H_ */
