// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This helper function creates and starts a task which wraps `tud_task()`.
 *
 * The wrapper function basically wraps tud_task and some log.
 * Default parameters: stack size and priority as configured, argument = NULL, not pinned to any core.
 * If you have more requirements for this task, you can create your own task which calls tud_task as the last step.
 *
 * @retval ESP_OK run tinyusb main task successfully
 * @retval ESP_FAIL run tinyusb main task failed of internal error
 * @retval ESP_ERR_INVALID_STATE tinyusb main task has been created before
 */
esp_err_t tusb_run_task(void);

/**
 * @brief This helper function stops and destroys the task created by `tusb_run_task()`
 *
 * @retval ESP_OK stop and destory tinyusb main task successfully
 * @retval ESP_ERR_INVALID_STATE tinyusb main task hasn't been created yet
 */
esp_err_t tusb_stop_task(void);

#ifdef __cplusplus
}
#endif
