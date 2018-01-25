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
#include "btc_dm.h"
#include "future.h"
#include "esp_err.h"
#include "btc_config.h"
#include "alarm.h"
#include "btc_ble_storage.h"
#include "btc_gatt_common.h"
#include "bta_gatt_common.h"


static void btc_set_local_mtu(uint16_t mtu)
{
    BTA_GATT_SetLocalMTU(mtu);
}

void btc_gatt_com_call_handler(btc_msg_t *msg)
{
    LOG_DEBUG("%s act %d\n", __func__, msg->act);
    switch (msg->act) {
    case BTC_GATT_ACT_SET_LOCAL_MTU:
    {
        btc_ble_gatt_com_args_t *arg = (btc_ble_gatt_com_args_t *)(msg->arg);
        btc_set_local_mtu(arg->set_mtu.mtu);
        break;
    }
    default:
        LOG_ERROR("%s UNKNOWN ACT %d\n", __func__, msg->act);
        break;
    }
}

