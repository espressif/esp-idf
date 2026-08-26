/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_L2CAP_H_
#define HOST_NIMBLE_L2CAP_H_

#include <stdint.h>

#include <zephyr/bluetooth/l2cap.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_l2cap_chan_connect(uint16_t conn_handle);

int bt_le_nimble_l2cap_chan_disconnect(struct bt_l2cap_chan *chan);

int bt_le_nimble_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf);

int bt_le_nimble_l2cap_init(void);

void bt_le_nimble_l2cap_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_L2CAP_H_ */
