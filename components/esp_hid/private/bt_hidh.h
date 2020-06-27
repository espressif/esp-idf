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

#include "esp_hidh.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BT_HID_HOST_ENABLED

esp_err_t esp_bt_hidh_init(const esp_hidh_config_t *config);
esp_err_t esp_bt_hidh_deinit(void);

esp_hidh_dev_t *esp_bt_hidh_dev_open(esp_bd_addr_t bda);

#endif /* CONFIG_BT_HID_HOST_ENABLED */

#ifdef __cplusplus
}
#endif
