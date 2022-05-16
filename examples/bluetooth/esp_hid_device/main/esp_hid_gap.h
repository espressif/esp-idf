// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_HID_GAP_H_
#define _ESP_HID_GAP_H_

#define HIDD_IDLE_MODE 0x00
#define HIDD_BLE_MODE 0x01
#define HIDD_BT_MODE 0x02
#define HIDD_BTDM_MODE 0x03

#if CONFIG_BT_HID_DEVICE_ENABLED
#if CONFIG_BT_BLE_ENABLED
#define HID_DEV_MODE HIDD_BTDM_MODE
#else
#define HID_DEV_MODE HIDD_BT_MODE
#endif
#elif CONFIG_BT_BLE_ENABLED
#define HID_DEV_MODE HIDD_BLE_MODE
#else
#define HID_DEV_MODE HIDD_IDLE_MODE
#endif

#include "esp_err.h"
#include "esp_log.h"

#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_hid_common.h"
#if CONFIG_BT_BLE_ENABLED
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_gap_ble_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_hidh_scan_result_s {
    struct esp_hidh_scan_result_s *next;

    esp_bd_addr_t bda;
    const char *name;
    int8_t rssi;
    esp_hid_usage_t usage;
    esp_hid_transport_t transport; //BT, BLE or USB
    union {
        struct {
            esp_bt_cod_t cod;
            esp_bt_uuid_t uuid;
        } bt;
        struct {
            esp_ble_addr_type_t addr_type;
            uint16_t appearance;
        } ble;
    };
} esp_hid_scan_result_t;

esp_err_t esp_hid_gap_init(uint8_t mode);
esp_err_t esp_hid_scan(uint32_t seconds, size_t *num_results, esp_hid_scan_result_t **results);
void esp_hid_scan_results_free(esp_hid_scan_result_t *results);

esp_err_t esp_hid_ble_gap_adv_init(uint16_t appearance, const char *device_name);
esp_err_t esp_hid_ble_gap_adv_start(void);

void print_uuid(esp_bt_uuid_t *uuid);
const char *ble_addr_type_str(esp_ble_addr_type_t ble_addr_type);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_HIDH_GAP_H_ */
