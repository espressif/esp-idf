/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bt_l2cap_chan;
struct net_buf;

int bt_le_bluedroid_l2cap_chan_connect(uint16_t conn_handle);

int bt_le_bluedroid_l2cap_chan_disconnect(struct bt_l2cap_chan *chan);

int bt_le_bluedroid_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf);

void bt_le_bluedroid_l2cap_accept_result(uint16_t conn_handle, uint8_t l2cap_id,
                                         uint16_t chan_handle, uint16_t result);

int bt_le_bluedroid_l2cap_init(void);

void bt_le_bluedroid_l2cap_deinit(void);

#ifdef __cplusplus
}
#endif
