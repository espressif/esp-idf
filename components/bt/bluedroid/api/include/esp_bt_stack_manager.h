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

#ifndef __ESP_BT_STACK_MANAGER_H__
#define __ESP_BT_STACK_MANAGER_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @brief           This function is called to initialize the bluetooth stack
 *
 * @return          
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if already initialized
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_init_stack(void);


/**
 *
 * @brief           Deinit and free the resource for bluetooth
 *
 * @return          
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if not initailized yet
 *
 */
esp_err_t esp_bt_deinit_stack(void);


/**
 *
 * @brief           Enable bluetooth, must after esp_bt_init_stack()
 *
 * @return          
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if not initialized yet or already enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_enable_stack(void);


/**
 * @brief           Disable bluetooth, must prior to esp_deinit_stack()
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if not initialized yet or already enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_disable_stack(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_STACK_MANAGER_H__ */
