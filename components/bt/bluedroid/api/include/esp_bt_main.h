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

#ifndef __ESP_BT_MAIN_H__
#define __ESP_BT_MAIN_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Enable bluetooth, must after esp_init_bluetooth()
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_enable_bluetooth(void);

/**
 * @brief     Disable bluetooth, must prior to esp_deinit_bluetooth()
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_disable_bluetooth(void);

/**
 * @brief     Init and alloc the resource for bluetooth, must be prior to every bluetooth stuff
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_init_bluetooth(void);

/**
 * @brief     Deinit and free the resource for bluetooth, must be after every bluetooth stuff
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_deinit_bluetooth(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_MAIN_H__ */
