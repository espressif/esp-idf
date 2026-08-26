/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FRIEND_H_
#define _FRIEND_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

enum bt_mesh_friend_pdu_type {
    BLE_MESH_FRIEND_PDU_SINGLE,
    BLE_MESH_FRIEND_PDU_PARTIAL,
    BLE_MESH_FRIEND_PDU_COMPLETE,
};

bool bt_mesh_friend_match(uint16_t net_idx, uint16_t addr);
bool bt_mesh_friend_unicast_match(uint16_t net_idx, uint16_t addr, uint8_t *selem);

struct bt_mesh_friend *bt_mesh_friend_find(uint16_t net_idx, uint16_t lpn_addr,
                                           bool valid, bool established);

bool bt_mesh_friend_queue_has_space(uint16_t net_idx, uint16_t src, uint16_t dst,
                                    const uint64_t *seq_auth, uint8_t seg_count);

void bt_mesh_friend_enqueue_rx(struct bt_mesh_net_rx *rx,
                               enum bt_mesh_friend_pdu_type type,
                               const uint64_t *seq_auth, uint8_t seg_count,
                               struct net_buf_simple *sbuf);
bool bt_mesh_friend_enqueue_tx(struct bt_mesh_net_tx *tx,
                               enum bt_mesh_friend_pdu_type type,
                               const uint64_t *seq_auth, uint8_t seg_count,
                               struct net_buf_simple *sbuf);

void bt_mesh_friend_clear_incomplete(struct bt_mesh_subnet *sub, uint16_t src,
                                     uint16_t dst, const uint64_t *seq_auth);

void bt_mesh_friend_sec_update(uint16_t net_idx);

void bt_mesh_friend_clear_net_idx(uint16_t net_idx);

int bt_mesh_friend_poll(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);
int bt_mesh_friend_req(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);
int bt_mesh_friend_clear(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);
int bt_mesh_friend_clear_cfm(struct bt_mesh_net_rx *rx,
                             struct net_buf_simple *buf);
int bt_mesh_friend_sub_add(struct bt_mesh_net_rx *rx,
                           struct net_buf_simple *buf);
int bt_mesh_friend_sub_rem(struct bt_mesh_net_rx *rx,
                           struct net_buf_simple *buf);

int bt_mesh_friend_init(void);
int bt_mesh_friend_deinit(void);

void bt_mesh_friend_remove_lpn(uint16_t lpn_addr);

#ifdef __cplusplus
}
#endif

#endif /* _FRIEND_H_ */
