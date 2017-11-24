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

#ifndef __BTC_GAP_BT_H__
#define __BTC_GAP_BT_H__

#include "bt_target.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "btc_task.h"

#if (BTC_GAP_BT_INCLUDED == TRUE)

typedef enum {
    BTC_GAP_BT_ACT_SET_SCAN_MODE = 0,
    BTC_GAP_BT_ACT_REG_CB,
    BTC_GAP_BT_ACT_START_DISCOVERY,
    BTC_GAP_BT_ACT_SEARCH_DEVICES,
    BTC_GAP_BT_ACT_CANCEL_DISCOVERY,
    BTC_GAP_BT_ACT_GET_REMOTE_SERVICES,
    BTC_GAP_BT_ACT_SEARCH_SERVICES,
    BTC_GAP_BT_ACT_GET_REMOTE_SERVICE_RECORD,
    BTC_GAP_BT_ACT_SEARCH_SERVICE_RECORD,
} btc_gap_bt_act_t;

/* btc_bt_gap_args_t */
typedef union {
    // BTC_BT_GAP_ACT_SET_SCAN_MODE,
    struct set_bt_scan_mode_args {
        esp_bt_scan_mode_t mode;
    } set_scan_mode;

    // BTC_GAP_BT_ACT_START_DISCOVERY
    struct start_disc_args {
        esp_bt_inq_mode_t mode;
        uint8_t inq_len;
        uint8_t num_rsps;
    } start_disc;

    // BTC_BT_GAP_ACT_GET_REMOTE_SERVICES
    bt_bdaddr_t bda;

    // BTC_BT_GAP_ACT_GET_REMTOE_SERVICE_RECORD
    struct get_rmt_srv_rcd_args {
        bt_bdaddr_t bda;
        esp_bt_uuid_t uuid;
    } get_rmt_srv_rcd;
} btc_gap_bt_args_t;

void btc_gap_bt_call_handler(btc_msg_t *msg);

void btc_gap_bt_busy_level_updated(uint8_t bl_flags);

#endif /* #if BTC_GAP_BT_INCLUDED */

#endif /* __BTC_GAP_BT_H__ */
