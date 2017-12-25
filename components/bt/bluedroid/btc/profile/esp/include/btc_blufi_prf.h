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
    BTC_BLUFI_ACT_SEND_CFG_REPORT,
    BTC_BLUFI_ACT_SEND_WIFI_LIST,
} btc_blufi_act_t;

typedef union {
    struct blufi_cfg_report {
        wifi_mode_t opmode;
        esp_blufi_sta_conn_state_t sta_conn_state;
        uint8_t softap_conn_num;
        esp_blufi_extra_info_t *extra_info;
        int extra_info_len;
    } wifi_conn_report;
    /*
        BTC_BLUFI_ACT_SEND_WIFI_LIST
    */
    struct blufi_wifi_list {
        uint16_t apCount;
        esp_blufi_ap_record_t *list;
    } wifi_list;
} btc_blufi_args_t;

void btc_blufi_cb_handler(btc_msg_t *msg);
void btc_blufi_call_handler(btc_msg_t *msg);
void btc_blufi_set_callbacks(esp_blufi_callbacks_t *callbacks);

void btc_blufi_call_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_blufi_call_deep_free(btc_msg_t *msg);

uint16_t btc_blufi_get_version(void);

#endif /* __BTC_BLUFI_PRF_H__ */
