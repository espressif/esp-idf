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

#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "btc_task.h"

typedef enum {
    BTC_GAP_BT_ACT_SET_SCAN_MODE = 0,
} btc_gap_bt_act_t;

/* btc_bt_gap_args_t */
typedef union {
    // BTC_BT_GAP_ACT_SET_SCAN_MODE,
    struct set_bt_scan_mode_args {
        esp_bt_scan_mode_t mode;
    } set_scan_mode;
} btc_gap_bt_args_t;

void btc_gap_bt_call_handler(btc_msg_t *msg);

void btc_gap_bt_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

#endif /* __BTC_GAP_BT_H__ */
