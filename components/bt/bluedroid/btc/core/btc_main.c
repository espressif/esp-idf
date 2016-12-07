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

#include "btc_task.h"
#include "btc_main.h"
#include "future.h"
#include "esp_err.h"

static future_t *main_future[BTC_MAIN_FUTURE_NUM];

extern int bte_main_boot_entry(void *cb);
extern int bte_main_shutdown(void);

future_t **btc_main_get_future_p(btc_main_future_type_t type)
{
    return &main_future[type];
}

static void btc_sec_callback(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data)
{
    switch (event) {
    case BTA_DM_ENABLE_EVT:
        future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_SUCCESS);
        break;
    case BTA_DM_DISABLE_EVT:
        future_ready(*btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE), FUTURE_SUCCESS);
        break;
    }
}

static void btc_enable_bluetooth(void)
{
    if (BTA_EnableBluetooth(btc_sec_callback) != BTA_SUCCESS) {
        future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_SUCCESS);
	}
}

static void btc_disable_bluetooth(void)
{
    if (BTA_DisableBluetooth() != BTA_SUCCESS) {
        future_ready(*btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE), FUTURE_SUCCESS);
	}
}

void btc_init_callback(void)
{
    future_ready(*btc_main_get_future_p(BTC_MAIN_INIT_FUTURE), FUTURE_SUCCESS);
}

static void btc_init_bluetooth(void)
{
    bte_main_boot_entry(btc_init_callback);
}


static void btc_deinit_bluetooth(void)
{
    bte_main_shutdown();
    future_ready(*btc_main_get_future_p(BTC_MAIN_DEINIT_FUTURE), FUTURE_SUCCESS);
}

void btc_main_call_handler(btc_msg_t *msg)
{
    LOG_DEBUG("%s act %d\n", __func__, msg->act);

    switch (msg->act) {
    case BTC_MAIN_ACT_INIT:
        btc_init_bluetooth();
        break;
    case BTC_MAIN_ACT_DEINIT:
        btc_deinit_bluetooth();
        break;
    case BTC_MAIN_ACT_ENABLE:
        btc_enable_bluetooth();
        break;
    case BTC_MAIN_ACT_DISABLE:
        btc_disable_bluetooth();
        break;
    default:
        LOG_ERROR("%s UNKNOWN ACT %d\n", __func__, msg->act);
        break;
    }
}

