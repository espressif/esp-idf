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

#ifndef __BTC_GATT_COMMON_H__
#define __BTC_GATT_COMMON_H__

#include "future.h"
#include "bt_types.h"
#include "bta_api.h"
#include "btc_main.h"
#include "btc_task.h"

typedef enum {
    BTC_GATT_ACT_SET_LOCAL_MTU = 0,
} btc_gatt_com_act_t;

/* btc_ble_gattc_args_t */
typedef union {
    //BTC_GATT_ACT_SET_LOCAL_MTU,
    struct set_mtu_arg {
        uint16_t mtu;
    } set_mtu;
} btc_ble_gatt_com_args_t;

void btc_gatt_com_call_handler(btc_msg_t *msg);
#endif /* __BTC_GATT_COMMON_H__ */
