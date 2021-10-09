// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "esp_hidd.h"
#include "esp_err.h"
#include "esp_hid_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BT_HID_DEVICE_ENABLED

esp_err_t esp_bt_hidd_dev_init(esp_hidd_dev_t *dev, const esp_hid_device_config_t *config, esp_event_handler_t callback);

#endif /* CONFIG_BT_HID_DEVICE_ENABLED */

#ifdef __cplusplus
}
#endif
