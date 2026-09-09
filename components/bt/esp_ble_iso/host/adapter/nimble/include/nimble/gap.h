/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_APP_GAP_H_
#define HOST_NIMBLE_APP_GAP_H_

#include <stdint.h>

#include <zephyr/bluetooth/bluetooth.h>

#include "host/ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_le_nimble_gap_post_event(void *param);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_APP_GAP_H_ */
