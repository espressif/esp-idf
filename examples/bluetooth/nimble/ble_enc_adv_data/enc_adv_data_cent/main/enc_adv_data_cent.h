/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef H_ENC_ADV_DATA_CENT_
#define H_ENC_ADV_DATA_CENT_

#if CONFIG_EXAMPLE_ENC_ADV_DATA
#include "modlog/modlog.h"
#include "esp_central.h"
#include "host/ble_ead.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ble_hs_adv_fields;
struct ble_gap_conn_desc;
struct ble_hs_cfg;
union ble_store_value;
union ble_store_key;

#define BLE_SVC_GAP_UUID16                                  0x1800
#define BLE_SVC_GAP_CHR_UUID16_DEVICE_NAME                  0x2a00

#define BLE_SVC_GAP_CHR_UUID16_KEY_MATERIAL                 0x2B88

struct km_peer {
    bool key_material_exist;
    uint8_t peer_addr[PEER_ADDR_VAL_SIZE];
};

#ifdef __cplusplus
}
#endif
#endif
#endif
