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

#ifndef __BTC_DEV_H__
#define __BTC_DEV_H__

#include "esp_bt_defs.h"
#include "esp_bt_device.h"
#include "btc_task.h"

typedef enum {
    BTC_DEV_ACT_SET_DEVICE_NAME
} btc_dev_act_t;

/* btc_dev_args_t */
typedef union {
    // BTC_BT_GAP_ACT_SET_DEV_NAME
    struct set_bt_dev_name_args {
#define ESP_DEV_DEVICE_NAME_MAX (32)
        char device_name[ESP_DEV_DEVICE_NAME_MAX + 1];
    } set_dev_name;
} btc_dev_args_t;

void btc_dev_call_handler(btc_msg_t *msg);

#endif /* __BTC_DEV_H__ */

