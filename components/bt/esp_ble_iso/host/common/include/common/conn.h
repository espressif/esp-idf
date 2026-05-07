/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2025 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_CONN_H_
#define HOST_COMMON_CONN_H_

#include <stdint.h>

#include "sdkconfig.h"

#include "nimble/gap.h"
#include "nimble/iso.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_conn_get_acl_conns(struct bt_conn **conns, uint8_t *count);

struct bt_conn *bt_le_acl_conn_find(uint16_t conn_handle);

struct bt_conn *bt_le_acl_conn_find_safe(uint16_t conn_handle);

int bt_le_acl_conn_new(uint16_t conn_handle,
                       uint8_t role,
                       bt_addr_le_t *dst,
                       uint8_t sec_level);

int bt_le_acl_conn_delete(uint16_t conn_handle);

int bt_le_acl_conn_update(uint16_t conn_handle,
                          uint8_t sec_level,
                          bool *encrypted);

int bt_le_acl_conn_connected_listener(uint16_t conn_handle);

int bt_le_acl_conn_disconnected_listener(uint16_t conn_handle, uint8_t reason);

int bt_le_acl_conn_security_changed_listener(uint16_t conn_handle, bt_security_t level);

int bt_le_acl_conn_identity_resolved_listener(uint16_t conn_handle,
                                              const bt_addr_le_t *rpa,
                                              const bt_addr_le_t *identity);

int bt_le_acl_conn_pairing_completed_listener(uint16_t conn_handle, bool bonded);

int bt_le_acl_conn_bond_deleted_listener(uint8_t id, const bt_addr_le_t *peer);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_CONN_H_ */
