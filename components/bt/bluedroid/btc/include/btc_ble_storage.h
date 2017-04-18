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

#include "bt_types.h"
#include "bt_target.h"

#if (SMP_INCLUDED == TRUE)
#define BTC_LE_LOCAL_KEY_IR       (1<<0)
#define BTC_LE_LOCAL_KEY_IRK      (1<<1)
#define BTC_LE_LOCAL_KEY_DHK      (1<<2)
#define BTC_LE_LOCAL_KEY_ER       (1<<3)

/************************************************************************************
**  Local type definitions
************************************************************************************/
typedef struct
{
    uint32_t num_devices;
    bt_bdaddr_t devices[BTM_SEC_MAX_DEVICE_RECORDS];
} btc_bonded_devices_t;

typedef struct
{
    bool                   is_penc_key_rcvd;
    tBTM_LE_PENC_KEYS         penc_key;       /* received peer encryption key */
    bool                   is_pcsrk_key_rcvd;
    tBTM_LE_PCSRK_KEYS        pcsrk_key;       /* received peer device SRK */
    bool                   is_pid_key_rcvd;
    tBTM_LE_PID_KEYS          pid_key;        /* peer device ID key */
    bool                   is_lenc_key_rcvd;
    tBTM_LE_LENC_KEYS         lenc_key;       /* local encryption reproduction keys LTK = = d1(ER,DIV,0)*/
    bool                   is_lcsrk_key_rcvd;
    tBTM_LE_LCSRK_KEYS        lcsrk_key;      /* local device CSRK = d1(ER,DIV,1)*/
    bool                   is_lidk_key_rcvd;   /* local identity key received */
} btc_dm_ble_cb_t;

typedef struct
{
    bt_bdaddr_t static_bdaddr;
    BD_ADDR bd_addr;
    btc_dm_ble_cb_t ble;
} btc_dm_pairing_cb_t;

typedef struct
{
    uint8_t       ir[BT_OCTET16_LEN];
    uint8_t       irk[BT_OCTET16_LEN];
    uint8_t       dhk[BT_OCTET16_LEN];
}btc_dm_local_key_id_t;

typedef struct
{
    bool                 is_er_rcvd;
    uint8_t             er[BT_OCTET16_LEN];
    bool                 is_id_keys_rcvd;
    btc_dm_local_key_id_t  id_keys;  /* ID kyes */
}btc_dm_local_key_cb_t;

typedef struct
{
    BT_OCTET16 sp_c;
    BT_OCTET16 sp_r;
    BD_ADDR  oob_bdaddr;  /* peer bdaddr*/
} btc_dm_oob_cb_t;


extern btc_dm_pairing_cb_t pairing_cb;
extern btc_dm_local_key_cb_t ble_local_key_cb;

bt_status_t btc_storage_load_bonded_ble_devices(void);

bt_status_t btc_in_fetch_bonded_ble_devices(int add);

void btc_dm_remove_ble_bonding_keys(void);

bt_status_t btc_storage_add_ble_bonding_key( bt_bdaddr_t *remote_bd_addr,
                                                                                      char *key,
                                                                                      uint8_t key_type,
                                                                                      uint8_t key_length);

void btc_save_ble_bonding_keys(void);

bt_status_t btc_in_fetch_bonded_ble_device(const char *remote_bd_addr, int add, 
                                           btc_bonded_devices_t *p_bonded_devices);

bt_status_t btc_storage_get_ble_bonding_key(bt_bdaddr_t *remote_bd_addr,
                                                                uint8_t key_type,
                                                                char *key_value,
                                                                int key_length);

bt_status_t btc_storage_add_ble_local_key(char *key,
                                                                              uint8_t key_type,
                                                                              uint8_t key_length);

bt_status_t btc_storage_remove_ble_bonding_keys(bt_bdaddr_t *remote_bd_addr);

bt_status_t btc_storage_remove_ble_local_keys(void);

bt_status_t btc_storage_get_ble_local_key(uint8_t key_type,
                                                                             char *key_value,
                                                                             int key_len);

bt_status_t btc_storage_get_remote_addr_type(bt_bdaddr_t *remote_bd_addr,
                                                                                     int *addr_type);

bt_status_t btc_storage_set_remote_addr_type(bt_bdaddr_t *remote_bd_addr,
                                                                                     uint8_t addr_type);

void btc_dm_load_ble_local_keys(void);

void btc_dm_get_ble_local_keys(tBTA_DM_BLE_LOCAL_KEY_MASK *p_key_mask, BT_OCTET16 er,
                                                            tBTA_BLE_LOCAL_ID_KEYS *p_id_keys);
#endif  ///SMP_INCLUDED == TRUE