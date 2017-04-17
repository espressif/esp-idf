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

#include "esp_gap_bt_api.h"
#include "btc_gap_bt.h"
#include "bta_api.h"
#include "bt_trace.h"
#include <string.h>
#include "bt_target.h"

#if BTC_GAP_BT_INCLUDED
void btc_gap_bt_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    switch (msg->act) {
    default:
        // LOG_ERROR("Unhandled deep copy %d\n", msg->act);
        break;
    }
}

static void btc_gap_bt_arg_deep_free(btc_msg_t *msg)
{
    LOG_DEBUG("%s \n", __func__);
    switch (msg->act) {
    default:
        // LOG_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

static void btc_bt_set_scan_mode(esp_bt_scan_mode_t mode)
{
    tBTA_DM_DISC disc_mode;
    tBTA_DM_CONN conn_mode;

    switch (mode) {
    case ESP_BT_SCAN_MODE_NONE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_NON_CONN;
        break;

    case ESP_BT_SCAN_MODE_CONNECTABLE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    case ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE:
        disc_mode = BTA_DM_GENERAL_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    default:
        LOG_WARN("invalid scan mode (0x%x)", mode);
        return;
    }

    BTA_DmSetVisibility(disc_mode, conn_mode, BTA_DM_IGNORE, BTA_DM_IGNORE);
    return;
}

void btc_gap_bt_call_handler(btc_msg_t *msg)
{
    btc_gap_bt_args_t *arg = (btc_gap_bt_args_t *)msg->arg;
    LOG_DEBUG("%s act %d\n", __func__, msg->act);
    switch (msg->act) {
    case BTC_GAP_BT_ACT_SET_SCAN_MODE: {
        btc_bt_set_scan_mode(arg->set_scan_mode.mode);
        break;
    }
    default:
        break;
    }
    btc_gap_bt_arg_deep_free(msg);
}

#endif /* #if BTC_GAP_BT_INCLUDED */
