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
 * @brief This API starts a task with a wrapper function of tud_task and default task parameters.
 *
 * The wrapper function basically wraps tud_task and some log. Default parameters: stack size and priority as configured, argument = NULL,
 * not pinned to any core.
 * If you have more requirements for this task, you can create your own task which calls tud_task as the last step.
 *
 * @return ESP_OK or ESP_FAIL
 */
esp_err_t tusb_run_task(void);

/**
 * @brief Stops a FreeRTOS task
 *
 * @return ESP_OK or ESP_FAIL
 */
esp_err_t tusb_stop_task(void);

#ifdef __cplusplus
}
#endif
