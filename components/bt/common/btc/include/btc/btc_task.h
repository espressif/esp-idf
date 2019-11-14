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
#include "bt_common.h"
#include "osi/thread.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "common/bt_target.h"
#endif

typedef struct btc_msg {
    uint8_t sig;    //event signal
    uint8_t aid;    //application id
    uint8_t pid;    //profile id
    uint8_t act;    //profile action, defined in seprerate header files
    void   *arg;    //param for btc function or function param
} btc_msg_t;

typedef struct btc_adv_packet {
    uint8_t addr[6];
    uint8_t addr_type;
} btc_adv_packet_t;

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
    BTC_PID_GATT_COMMON,
    BTC_PID_GAP_BLE,
    BTC_PID_BLE_HID,
    BTC_PID_SPPLIKE,
    BTC_PID_BLUFI,
    BTC_PID_DM_SEC,
    BTC_PID_ALARM,
#if (CLASSIC_BT_INCLUDED == TRUE)
    BTC_PID_GAP_BT,
    BTC_PID_PRF_QUE,
    BTC_PID_A2DP,
    BTC_PID_AVRC_CT,
    BTC_PID_AVRC_TG,
    BTC_PID_SPP,
#if (BTC_HF_CLIENT_INCLUDED == TRUE)
    BTC_PID_HF_CLIENT,
#endif /* BTC_HF_CLIENT_INCLUDED */
#endif  /* CLASSIC_BT_INCLUDED */
#if CONFIG_BLE_MESH
    BTC_PID_PROV,
    BTC_PID_MODEL,
    BTC_PID_HEALTH_CLIENT,
    BTC_PID_HEALTH_SERVER,
    BTC_PID_CONFIG_CLIENT,
    BTC_PID_CONFIG_SERVER,
    BTC_PID_GENERIC_CLIENT,
    BTC_PID_LIGHTING_CLIENT,
    BTC_PID_SENSOR_CLIENT,
    BTC_PID_TIME_SCENE_CLIENT,
    BTC_PID_GENERIC_SERVER,
    BTC_PID_LIGHTING_SERVER,
    BTC_PID_SENSOR_SERVER,
    BTC_PID_TIME_SCENE_SERVER,
#endif /* CONFIG_BLE_MESH */
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
bool btc_check_queue_is_congest(void);

#endif /* __BTC_TASK_H__ */
