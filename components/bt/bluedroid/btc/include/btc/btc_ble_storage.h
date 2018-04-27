// Copyright (C) 2014 The Android Open Source Project
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
#ifndef __BTC_BLE_STORAGE_H__
#define __BTC_BLE_STORAGE_H__
#include "stack/bt_types.h"
#include "common/bt_target.h"
#include "esp_gap_ble_api.h"

#if (SMP_INCLUDED == TRUE)
#define BTC_LE_LOCAL_KEY_IR       (1<<0)
#define BTC_LE_LOCAL_KEY_IRK      (1<<1)
#define BTC_LE_LOCAL_KEY_DHK      (1<<2)
#define BTC_LE_LOCAL_KEY_ER       (1<<3)

#define BTC_BLE_STORAGE_DEV_TYPE_STR                "DevType"
#define BTC_BLE_STORAGE_ADDR_TYPE_STR               "AddrType"
#define BTC_BLE_STORAGE_LINK_KEY_STR                "LinkKey"
#define BTC_BLE_STORAGE_LE_KEY_PENC_STR             "LE_KEY_PENC"
#define BTC_BLE_STORAGE_LE_KEY_PID_STR              "LE_KEY_PID"
#define BTC_BLE_STORAGE_LE_KEY_PCSRK_STR            "LE_KEY_PCSRK"
#define BTC_BLE_STORAGE_LE_KEY_LENC_STR             "LE_KEY_LENC"
#define BTC_BLE_STORAGE_LE_KEY_LID_STR              "LE_KEY_LID"
#define BTC_BLE_STORAGE_LE_KEY_LCSRK_STR            "LE_KEY_LCSRK"

#define BTC_BLE_STORAGE_LOCAL_ADAPTER_STR           "Adapter"
#define BTC_BLE_STORAGE_LE_LOCAL_KEY_IR_STR         "LE_LOCAL_KEY_IR"
#define BTC_BLE_STORAGE_LE_LOCAL_KEY_IRK_STR        "LE_LOCAL_KEY_IRK"
#define BTC_BLE_STORAGE_LE_LOCAL_KEY_DHK_STR        "LE_LOCAL_KEY_DHK"
#define BTC_BLE_STORAGE_LE_LOCAL_KEY_ER_STR         "LE_LOCAL_KEY_ER"

/************************************************************************************
**  Local type definitions
************************************************************************************/
typedef struct
{
    BT_OCTET16 sp_c;
    BT_OCTET16 sp_r;
    BD_ADDR  oob_bdaddr;  /* peer bdaddr*/
} btc_dm_oob_cb_t;


void btc_storage_save(void);

bt_status_t btc_storage_add_ble_bonding_key( bt_bdaddr_t *remote_bd_addr, char *key, uint8_t key_type, uint8_t key_length);

bt_status_t btc_storage_get_ble_bonding_key(bt_bdaddr_t *remote_bd_addr, uint8_t key_type, char *key_value, int key_length);

bt_status_t btc_storage_remove_ble_bonding_keys(bt_bdaddr_t *remote_bd_addr);

bool btc_storage_compare_address_key_value(bt_bdaddr_t *remote_bd_addr, uint8_t key_type, void *key_value, int key_length);

bt_status_t btc_storage_add_ble_local_key(char *key, uint8_t key_type, uint8_t key_length);

bt_status_t btc_storage_remove_ble_local_keys(void);

bt_status_t btc_storage_get_ble_local_key(uint8_t key_type, char *key_value, int key_len);

bt_status_t btc_storage_get_remote_addr_type(bt_bdaddr_t *remote_bd_addr, int *addr_type);

bt_status_t btc_storage_set_remote_addr_type(bt_bdaddr_t *remote_bd_addr, uint8_t addr_type, bool flush);

bt_status_t btc_storage_remove_remote_addr_type(bt_bdaddr_t *remote_bd_addr, bool flush);

bt_status_t btc_storage_set_ble_dev_type(bt_bdaddr_t *bd_addr, bool flush);

bt_status_t btc_storage_remove_ble_dev_type(bt_bdaddr_t *remote_bd_addr, bool flush);

bt_status_t btc_storage_load_bonded_ble_devices(void);

bt_status_t btc_storage_get_bonded_ble_devices_list(esp_ble_bond_dev_t *bond_dev, int dev_num);

int btc_storage_get_num_ble_bond_devices(void);

#endif  ///SMP_INCLUDED == TRUE
#endif  ///__BTC_BLE_STORAGE_H__
