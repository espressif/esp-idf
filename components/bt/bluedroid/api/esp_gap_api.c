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

#include "esp_gap_api.h"
#include "bta_api.h"
#include "bt_trace.h"

esp_err_t esp_bt_gap_set_scan_mode(bt_scan_mode_t mode)
{
    tBTA_DM_DISC disc_mode;
    tBTA_DM_CONN conn_mode;

    switch(mode) {
    case BT_SCAN_MODE_NONE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_NON_CONN;
        break;

    case BT_SCAN_MODE_CONNECTABLE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    case BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE:
        disc_mode = BTA_DM_GENERAL_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    default:
        // BTIF_TRACE_ERROR("invalid scan mode (0x%x)", mode);
        return ESP_ERR_INVALID_ARG;
    }

    // BTIF_TRACE_EVENT("set property scan mode : %x", mode);
    BTA_DmSetVisibility(disc_mode, conn_mode, BTA_DM_IGNORE, BTA_DM_IGNORE);
    
    return ESP_OK;
}
