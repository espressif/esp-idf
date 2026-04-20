/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_SERVER_H_
#define HOST_NIMBLE_SERVER_H_

#include <stdint.h>
#include <stdbool.h>

#include "host/ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_gatts_access_cb_safe(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctx, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_SERVER_H_ */
