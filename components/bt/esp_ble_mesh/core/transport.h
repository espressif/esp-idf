/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRANS_SEQ_AUTH_NVAL             0xffffffffffffffff

#define BLE_MESH_SDU_UNSEG_MAX          11
#define BLE_MESH_CTL_SEG_SDU_MAX        8
#define BLE_MESH_APP_SEG_SDU_MAX        12
#define BLE_MESH_TX_SDU_MAX             (CONFIG_BLE_MESH_TX_SEG_MAX * 12)

#define TRANS_SEQ_ZERO_MASK             ((uint16_t)BIT_MASK(13))
#define TRANS_CTL_OP_MASK               ((uint8_t)BIT_MASK(7))
#define TRANS_CTL_OP(data)              ((data)[0] & TRANS_CTL_OP_MASK)
#define TRANS_CTL_HDR(op, seg)          (((op) & TRANS_CTL_OP_MASK) | ((seg) << 7))

#define TRANS_CTL_OP_ACK                0x00
#define TRANS_CTL_OP_FRIEND_POLL        0x01
#define TRANS_CTL_OP_FRIEND_UPDATE      0x02
#define TRANS_CTL_OP_FRIEND_REQ         0x03
#define TRANS_CTL_OP_FRIEND_OFFER       0x04
#define TRANS_CTL_OP_FRIEND_CLEAR       0x05
#define TRANS_CTL_OP_FRIEND_CLEAR_CFM   0x06
#define TRANS_CTL_OP_FRIEND_SUB_ADD     0x07
#define TRANS_CTL_OP_FRIEND_SUB_REM     0x08
#define TRANS_CTL_OP_FRIEND_SUB_CFM     0x09
#define TRANS_CTL_OP_HEARTBEAT          0x0A
#define TRANS_CTL_OP_PATH_REQ           0x0B
#define TRANS_CTL_OP_PATH_REPLY         0x0C
#define TRANS_CTL_OP_PATH_CFM           0x0D
#define TRANS_CTL_OP_PATH_ECHO_REQ      0x0E
#define TRANS_CTL_OP_PATH_ECHO_REPLY    0x0F
#define TRANS_CTL_OP_DEP_NODE_UPDATE    0x10
#define TRANS_CTL_OP_PATH_REQ_SOLIC     0x11

struct bt_mesh_ctl_friend_poll {
    uint8_t  fsn;
} __attribute__((packed));

struct bt_mesh_ctl_friend_update {
    uint8_t  flags;
    uint32_t iv_index;
    uint8_t  md;
} __attribute__((packed));

struct bt_mesh_ctl_friend_req {
    uint8_t  criteria;
    uint8_t  recv_delay;
    uint8_t  poll_to[3];
    uint16_t prev_addr;
    uint8_t  num_elem;
    uint16_t lpn_counter;
} __attribute__((packed));

struct bt_mesh_ctl_friend_offer {
    uint8_t  recv_win;
    uint8_t  queue_size;
    uint8_t  sub_list_size;
    int8_t   rssi;
    uint16_t frnd_counter;
} __attribute__((packed));

struct bt_mesh_ctl_friend_clear {
    uint16_t lpn_addr;
    uint16_t lpn_counter;
} __attribute__((packed));

struct bt_mesh_ctl_friend_clear_confirm {
    uint16_t lpn_addr;
    uint16_t lpn_counter;
} __attribute__((packed));

#define BLE_MESH_FRIEND_SUB_MIN_LEN (1 + 2)
struct bt_mesh_ctl_friend_sub {
    uint8_t  xact;
    uint16_t addr_list[5];
} __attribute__((packed));

struct bt_mesh_ctl_friend_sub_confirm {
    uint8_t xact;
} __attribute__((packed));

uint8_t bt_mesh_get_seg_rtx_num(void);

int32_t bt_mesh_get_seg_rtx_timeout(uint16_t dst, uint8_t ttl);

struct bt_mesh_app_key *bt_mesh_app_key_get(uint16_t app_idx);

int bt_mesh_upper_key_get(const struct bt_mesh_subnet *subnet, uint16_t app_idx,
                          const uint8_t **key, uint8_t *aid, uint16_t dst);

bool bt_mesh_tx_in_progress(void);

void bt_mesh_rx_reset(void);
void bt_mesh_tx_reset(void);
void bt_mesh_rx_reset_single(uint16_t src);
void bt_mesh_tx_reset_single(uint16_t dst);

int bt_mesh_ctl_send(struct bt_mesh_net_tx *tx, uint8_t ctl_op, void *data,
                     size_t data_len, const struct bt_mesh_send_cb *cb,
                     void *cb_data);

int bt_mesh_trans_send(struct bt_mesh_net_tx *tx, struct net_buf_simple *msg,
                       const struct bt_mesh_send_cb *cb, void *cb_data);

int bt_mesh_trans_recv(struct net_buf_simple *buf, struct bt_mesh_net_rx *rx);

void bt_mesh_trans_init(void);
void bt_mesh_trans_deinit(bool erase);

#ifdef __cplusplus
}
#endif

#endif /* _TRANSPORT_H_ */
