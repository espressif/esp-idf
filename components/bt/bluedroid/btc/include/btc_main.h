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

#ifndef __BTC_BT_MAIN_H__
#define __BTC_BT_MAIN_H__

#include "future.h"
#include "bt_types.h"
#include "bta_api.h"
#include "btc_main.h"
#include "btc_task.h"

typedef enum {
    BTC_MAIN_ACT_INIT = 0,
    BTC_MAIN_ACT_DEINIT,
    BTC_MAIN_ACT_ENABLE,
    BTC_MAIN_ACT_DISABLE,
    BTC_GATT_ACT_SET_LOCAL_MTU,
} btc_main_act_t;

typedef enum {
    BTC_MAIN_INIT_FUTURE = 0,
    BTC_MAIN_DEINIT_FUTURE,
    BTC_MAIN_ENABLE_FUTURE,
    BTC_MAIN_DISABLE_FUTURE,
    BTC_MAIN_FUTURE_NUM,
} btc_main_future_type_t;

future_t **btc_main_get_future_p(btc_main_future_type_t type);

#if 0
typedef union {
    struct btc_main_init_args {
        future_t *future;
    } init;
    struct btc_main_deinit_args {
        future_t *future;
    } deinit;
    struct btc_main_init_args {
        future_t *future;
    } enable;
    struct btc_main_init_args {
        future_t *future;
    } disable;
} btc_main_args_t;

bt_status_t btc_enable_bluetooth(future_t *future);
void btc_disable_bluetooth(future_t *future);
bt_status_t btc_init_bluetooth(future_t *future);
void btc_deinit_bluetooth(future_t *future);
#endif

/* btc_ble_gattc_args_t */
typedef union {
    //BTC_GATT_ACT_SET_LOCAL_MTU,
    struct set_mtu_arg {
        uint16_t mtu;
    } set_mtu;
} btc_ble_main_args_t;

void btc_main_call_handler(btc_msg_t *msg);
#endif /* __BTC_BT_MAIN_H__ */
