/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BLE_CTS_CENT_
#define H_BLE_CTS_CENT_

#include "modlog/modlog.h"
#include "esp_central.h"
#ifdef __cplusplus
extern "C" {
#endif

struct ble_hs_adv_fields;
struct ble_gap_conn_desc;
struct ble_hs_cfg;
union ble_store_value;
union ble_store_key;

/* 16 BIT CCCD UUID */
#define BLE_SVC_CTS_DSC_CLT_CFG_UUID16                       0x2902

#ifdef __cplusplus
}
#endif

#endif
