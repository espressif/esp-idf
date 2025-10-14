/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef BLE_MULTICONN_CENT_DEMO_H
#define BLE_MULTICONN_CENT_DEMO_H
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_random.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define MAX_CONN_NUM (CONFIG_BT_ACL_CONNECTIONS)

/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    HRS_IDX_NB,
};

enum
{
    DISABLED,
    PENDING,
    ENABLED,
};

typedef struct
{
    esp_gatt_if_t gattc_if;
    uint16_t conn_id;
    uint16_t conn_handle;
    uint16_t char_handle;
    esp_bd_addr_t peer_addr;

} Peer;

void peer_manager_init(void);

void traverse_send_peer(uint16_t len, uint8_t *value);

esp_err_t peer_add(Peer *peer);

esp_err_t peer_remove(uint16_t conn_id);

Peer *find_peer(uint16_t conn_id);

#endif
