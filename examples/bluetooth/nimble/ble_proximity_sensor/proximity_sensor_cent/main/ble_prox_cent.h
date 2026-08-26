/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BLE_PROX_CENT_
#define H_BLE_PROX_CENT_

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

/* 16 Bit Proximity Sensor Service UUID */
#define BLE_SVC_IMMEDIATE_ALERT_UUID16                      0x1802
#define BLE_SVC_LINK_LOSS_UUID16                            0x1803
#define BLE_SVC_TX_POWER_UUID16                             0x1804

/* 16 Bit Proximity Sensor Service Characteristic UUIDs */
#define BLE_SVC_PROX_CHR_UUID16_ALERT_LVL                   0x2A06
#define BLE_SVC_PROX_CHR_UUID16_TX_PWR_LVL                  0x2A07

#define BLE_ADDR_LEN                                        6

struct ble_prox_cent_conn_peer {
    uint16_t val_handle;
    bool calc_path_loss;
};

struct ble_prox_cent_link_lost_peer {
    uint8_t *addr;
    bool link_lost;
};

#ifdef __cplusplus
}
#endif

#endif
