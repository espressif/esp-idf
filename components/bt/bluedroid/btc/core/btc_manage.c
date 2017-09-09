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
#include "bt_trace.h"
#include "thread.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"

static void *btc_profile_cb_tab[BTC_PID_NUM] = {};

void esp_profile_cb_reset(void)
{
    int i;

    for (i = 0; i < BTC_PID_NUM; i++) {
        btc_profile_cb_tab[i] = NULL;
    }
}

int btc_profile_cb_set(btc_pid_t profile_id, void *cb)
{
    if (profile_id < 0 || profile_id >= BTC_PID_NUM) {
        return -1;
    }

    btc_profile_cb_tab[profile_id] = cb;

    return 0;
}

void *btc_profile_cb_get(btc_pid_t profile_id)
{
    if (profile_id < 0 || profile_id >= BTC_PID_NUM) {
        return NULL;
    }

    return btc_profile_cb_tab[profile_id];
}


