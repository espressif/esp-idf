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

#ifndef __ESP_GAP_BT_API_H__
#define __ESP_GAP_BT_API_H__

#include <stdint.h>

#include "esp_err.h"
#include "esp_bt_defs.h"

typedef enum {
    BT_SCAN_MODE_NONE = 0,
    BT_SCAN_MODE_CONNECTABLE,
    BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE
} bt_scan_mode_t;

esp_err_t esp_bt_gap_set_scan_mode(bt_scan_mode_t mode);

esp_err_t esp_bt_gap_set_device_name(const char *name);
#endif /* __ESP_GAP_BT_API_H__ */
