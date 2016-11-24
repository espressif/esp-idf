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


#include "esp_blufi_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "btc_task.h"
#include "btc_blufi_prf.h"
#include "btc_manage.h"
#include "btc_main.h"
#include "future.h"

esp_err_t esp_blufi_register_callback(esp_profile_cb_t callback)
{
    return (btc_profile_cb_set(BTC_PID_BLUFI, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_blufi_send_config_state(esp_blufi_config_state_t state)
{
    btc_msg_t msg;
    btc_blufi_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_SEND_CFG_STATE;
    arg.cfg_state.state = state;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_blufi_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_blufi_profile_init(void)
{
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_INIT;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_blufi_profile_deinit(void)
{
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_DEINIT;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


