// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#pragma once

#include "esp_err.h"
#include "esp_openthread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief OpenThread task declaration
*
*/
typedef void (*esp_openthread_task_t)(void *);

/**
* @brief This function allocs and initializes the OpenThread task queue.
*
* @return
*   - ESP_OK on success
*   - ESP_ERR_NO_MEM on queue allocation failure
*   - ESP_FAIL on other failures
*
*/
esp_err_t esp_openthread_task_queue_init(void);

/**
* @brief This function posts a task to the OpenThread task queue.
*
* @param[in] task   The task to execute.
* @param[in] arg    The context argument to be passed to the task.
*
* @return
*   - ESP_OK
*   - ESP_FAIL
*
*/
esp_err_t esp_openthread_task_queue_post(esp_openthread_task_t task, void *arg);

/**
* @brief This function updates the task queue inner fd to the main loop.
*
* @param[inout]    mainloop    The main loop context.
*
*/
void esp_openthread_task_queue_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function drives the execution of the task queue.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
esp_err_t esp_openthread_task_queue_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function deinitializes the task queue.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
esp_err_t esp_openthread_task_queue_deinit(void);

#ifdef __cplusplus
}
#endif
