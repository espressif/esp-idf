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

#ifndef __BTC_BLUFI_PRF_H__
#define __BTC_BLUFI_PRF_H__

#include "bt_target.h"
#include "btc_task.h"
#include "esp_blufi_api.h"

typedef enum {
    BTC_BLUFI_ACT_INIT = 0,
    BTC_BLUFI_ACT_DEINIT,
    BTC_BLUFI_ACT_SEND_CFG_STATE,
} btc_blufi_act_t;

typedef enum {
    BTC_BLUFI_CB_ACT_INIT_FINISH = 0,
    BTC_BLUFI_CB_ACT_DEINIT_FINISH,
    BTC_BLUFI_CB_ACT_RECV_DATA,
} btc_blufi_cb_act_t;

typedef union {
#if 0
    //BTC_BLUFI_ACT_INIT = 0,
    struct blufi_init_param {
    } init;
    //BTC_BLUFI_ACT_DEINIT,
    struct blufi_deinit_param {
    } deinit;
#endif
    //BTC_BLUFI_ACT_SEND_CFG_STATE,
    struct blufi_send_cfg_state_pram {
        esp_blufi_config_state_t state;
    } cfg_state;
} btc_blufi_args_t;

void btc_blufi_cb_handler(btc_msg_t *msg);
void btc_blufi_call_handler(btc_msg_t *msg);

#endif /* __BTC_BLUFI_PRF_H__ */
