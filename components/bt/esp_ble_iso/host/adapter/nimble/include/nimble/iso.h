/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_ISO_H_
#define HOST_NIMBLE_ISO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_iso_cmd_send_sync(uint16_t opcode,
                                   struct net_buf *buf,
                                   struct net_buf **rsp);

int bt_le_nimble_iso_init(void);

void bt_le_nimble_iso_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_ISO_H_ */
