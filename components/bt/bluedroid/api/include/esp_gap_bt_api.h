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

#ifdef __cplusplus
extern "C" {
#endif


/// Discoverability and Connectability mode
typedef enum {
    ESP_BT_SCAN_MODE_NONE = 0,                      /*!< Neither discoverable nor connectable */
    ESP_BT_SCAN_MODE_CONNECTABLE,                   /*!< Connectable but not discoverable */
    ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE       /*!< both discoverable and connectaable */
} esp_bt_scan_mode_t;

/**
 * @brief           Set discoverability and connectability mode for legacy bluetooth. This function should
 *                  be called after esp_bluedroid_enable() completes successfully
 *
 * @param[in]       mode : one of the enums of bt_scan_mode_t
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_ARG: if argument invalid
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_set_scan_mode(esp_bt_scan_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GAP_BT_API_H__ */
