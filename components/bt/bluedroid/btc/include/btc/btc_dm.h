// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __BTC_DM_H__
#define __BTC_DM_H__

#include "btc/btc_task.h"
#include "esp_bt_defs.h"
#include "bta/bta_api.h"

typedef enum {
    BTC_DM_SEC_ACT
} btc_dm_sec_act_t;

/* btc_dm_args_t */
typedef union {
    //BTC_DM_SEC_ACT
    tBTA_DM_SEC sec;
} btc_dm_sec_args_t;

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
    bt_bdaddr_t            static_bdaddr;
    BD_ADDR                bd_addr;
    btc_dm_ble_cb_t        ble;
} btc_dm_pairing_cb_t;

typedef struct
{
    uint8_t                ir[BT_OCTET16_LEN];
    uint8_t                irk[BT_OCTET16_LEN];
    uint8_t                dhk[BT_OCTET16_LEN];
} btc_dm_local_key_id_t;

typedef struct
{
    bool                 is_er_rcvd;
    uint8_t              er[BT_OCTET16_LEN];
    bool                 is_id_keys_rcvd;
    btc_dm_local_key_id_t  id_keys;  /* ID kyes */
} btc_dm_local_key_cb_t;



// void btc_dm_call_handler(btc_msg_t *msg);
void btc_dm_sec_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *data);
void btc_dm_sec_cb_handler(btc_msg_t *msg);
void btc_dm_sec_arg_deep_copy(btc_msg_t *msg, void *dst, void *src);

bt_status_t btc_dm_enable_service(tBTA_SERVICE_ID service_id);
bt_status_t btc_dm_disable_service(tBTA_SERVICE_ID service_id);

#if (SMP_INCLUDED == TRUE)
void btc_dm_load_ble_local_keys(void);

void btc_dm_get_ble_local_keys(tBTA_DM_BLE_LOCAL_KEY_MASK *p_key_mask, BT_OCTET16 er,
                                                            tBTA_BLE_LOCAL_ID_KEYS *p_id_keys);
#endif

#endif /* __BTC_DM_H__ */
