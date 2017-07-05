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

#include "btc_task.h"
#include "esp_bt_defs.h"
#include "bta_api.h"

typedef enum {
    BTC_DM_SEC_ACT
} btc_dm_sec_act_t;

/* btc_dm_args_t */
typedef union {
    //BTC_DM_SEC_ACT
    tBTA_DM_SEC sec;
} btc_dm_sec_args_t;

// void btc_dm_call_handler(btc_msg_t *msg);
void btc_dm_sec_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *data);
void btc_dm_sec_cb_handler(btc_msg_t *msg);
void btc_dm_sec_arg_deep_copy(btc_msg_t *msg, void *dst, void *src);

bt_status_t btc_dm_enable_service(tBTA_SERVICE_ID service_id);
bt_status_t btc_dm_disable_service(tBTA_SERVICE_ID service_id);

#endif /* __BTC_DM_H__ */
