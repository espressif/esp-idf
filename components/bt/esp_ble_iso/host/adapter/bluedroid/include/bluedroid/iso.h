/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_ISO_H_
#define HOST_BLUEDROID_ISO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct net_buf;

int bt_le_bluedroid_iso_cmd_send_sync(uint16_t opcode,
                                      struct net_buf *buf,
                                      struct net_buf **rsp);

int bt_le_bluedroid_iso_disconnect(uint16_t conn_handle, uint8_t reason);

int bt_le_bluedroid_iso_init(void);

void bt_le_bluedroid_iso_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_ISO_H_ */
