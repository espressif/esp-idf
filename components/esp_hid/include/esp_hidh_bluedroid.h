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

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#if CONFIG_BLUEDROID_ENABLED

#include "esp_bt_defs.h"

/**
 * @brief Open BlueTooth HID Device using BlueDroid
 * @param bda               : BT Device Address
 * @param transport         : BT Device Protocol (Classic/HID)
 * @param remote_addr_type  : BLE Remote address type
 *
 * @return: ESP_OK on success
 */
esp_hidh_dev_t *esp_hidh_dev_open(esp_bd_addr_t bda, esp_hid_transport_t transport, uint8_t remote_addr_type);

#endif /* CONFIG_BLUEDROID_ENABLED */

#ifdef __cplusplus
}
#endif
