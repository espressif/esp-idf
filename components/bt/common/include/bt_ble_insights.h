/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BT_BLE_INSIGHTS_H__
#define __BT_BLE_INSIGHTS_H__

#if __has_include("ble_insights.h")
#include "ble_insights.h"
#define BT_BLE_INSIGHTS_AVAILABLE 1
#else
#define BT_BLE_INSIGHTS_AVAILABLE 0
static inline void ble_insights_log(const char *log)
{
    (void)log;
}
#endif

#endif /* __BT_BLE_INSIGHTS_H__ */
