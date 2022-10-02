/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef H_BLESPPCLIENT_
#define H_BLESPPCLIENT_

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

#define GATT_SVR_SVC_ALERT_UUID              0x1811
#define GATT_SVR_CHR_SUP_NEW_ALERT_CAT_UUID  0x2A47
#define GATT_SVR_CHR_NEW_ALERT               0x2A46
#define GATT_SVR_CHR_SUP_UNR_ALERT_CAT_UUID  0x2A48
#define GATT_SVR_CHR_UNR_ALERT_STAT_UUID     0x2A45
#define GATT_SVR_CHR_ALERT_NOT_CTRL_PT       0x2A44

#ifdef __cplusplus
}
#endif

#endif
