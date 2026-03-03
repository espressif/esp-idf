/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/ieee802154_common_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t bt_bb_get_rssi_comp(void);

#define IEEE802154_RSSI_COMPENSATION_VALUE bt_bb_get_rssi_comp()

#ifdef __cplusplus
}
#endif
