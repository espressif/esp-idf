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

#ifndef __BTC_TASK_H__
#define __BTC_TASK_H__

#include <stdint.h>
#include "bt_target.h"
#include "bt_defs.h"
#include "thread.h"

typedef struct btc_msg {
    uint8_t sig;    //event signal
    uint8_t aid;    //application id
    uint8_t pid;    //profile id
    uint8_t act;    //profile action, defined in seprerate header files
    void   *arg;    //param for btc function or function param
} btc_msg_t;

typedef enum {
    BTC_SIG_API_CALL = 0,   // APP TO STACK
    BTC_SIG_API_CB,         // STACK TO APP
    BTC_SIG_NUM,
} btc_sig_t; //btc message type

typedef enum {
    BTC_PID_MAIN_INIT = 0,
    BTC_PID_DEV,
    BTC_PID_GATTS,
#if (GATTC_INCLUDED == TRUE)
    BTC_PID_GATTC,
#endif  ///GATTC_INCLUDED == TRUE
    BTC_PID_GAP_BLE,
    BTC_PID_BLE_HID,
    BTC_PID_SPPLIKE,
    BTC_PID_BLUFI,
    BTC_PID_DM_SEC,
    BTC_PID_ALARM,
#if CONFIG_CLASSIC_BT_ENABLED
    BTC_PID_GAP_BT,
    BTC_PID_PRF_QUE,
    BTC_PID_A2DP,
    BTC_PID_AVRC,
#endif  /* CONFIG_CLASSIC_BT_ENABLED */
    BTC_PID_NUM,
} btc_pid_t; //btc profile id

typedef struct {
    void (* btc_call)(btc_msg_t *msg);
    void (* btc_cb)(btc_msg_t *msg);
} btc_func_t;

typedef void (* btc_arg_deep_copy_t)(btc_msg_t *msg, void *dst, void *src);

bt_status_t btc_transfer_context(btc_msg_t *msg, void *arg, int arg_len, btc_arg_deep_copy_t copy_func);

int btc_init(void);
void btc_deinit(void);

#endif /* __BTC_TASK_H__ */
