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

#if CONFIG_GATTS_ENABLE

#include "esp_gatts_api.h" //for the callback

/**
 * @brief HID BLE GATTS System Callback. Attach it in your code
 *        or call it from your gatts event handler to allow the HID stack to function
 * @param event     : Event type
 * @param gatts_if  : GATTS Interface ID
 * @param param     : Point to callback parameter, currently is union type
 */
void esp_hidd_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

#endif /* CONFIG_GATTS_ENABLE */

#ifdef __cplusplus
}
#endif
