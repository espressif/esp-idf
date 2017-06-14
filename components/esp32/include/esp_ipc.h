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

#ifndef __ESP_IPC_H__
#define __ESP_IPC_H__

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*esp_ipc_func_t)(void* arg);

/**
 * @brief Inter-processor call APIs
 *
 * FreeRTOS provides several APIs which can be used to communicate between
 * different tasks, including tasks running on different CPUs.
 * This module provides additional APIs to run some code on the other CPU.
 *
 * These APIs can only be used when FreeRTOS scheduler is running.
 */


/**
 * @brief Initialize inter-processor call module.
 *
 * This function start two tasks, one on each CPU. These tasks are started
 * with high priority. These tasks are normally inactive, waiting until one of
 * the esp_ipc_call_* functions to be used. One of these tasks will be
 * woken up to execute the callback provided to esp_ipc_call_nonblocking or
 * esp_ipc_call_blocking.
 */
void esp_ipc_init();


/**
 * @brief Execute function on the given CPU
 *
 * This will wake a high-priority task on CPU indicated by cpu_id argument,
 * and run func(arg) in the context of that task.
 * This function returns as soon as the high-priority task is woken up.
 * If another IPC call is already being executed, this function will also wait
 * for it to complete.
 *
 * In single-core mode, returns ESP_ERR_INVALID_ARG for cpu_id 1.
 *
 * @param cpu_id CPU where function should be executed (0 or 1)
 * @param func pointer to a function which should be executed
 * @param arg arbitrary argument to be passed into function
 *
 * @return ESP_ERR_INVALID_ARG if cpu_id is invalid
 *         ESP_ERR_INVALID_STATE if FreeRTOS scheduler is not running
 *         ESP_OK otherwise
 */
esp_err_t esp_ipc_call(uint32_t cpu_id, esp_ipc_func_t func, void* arg);


/**
 * @brief Execute function on the given CPU and wait for it to finish
 *
 * This will wake a high-priority task on CPU indicated by cpu_id argument,
 * and run func(arg) in the context of that task.
 * This function waits for func to return.
 *
 * In single-core mode, returns ESP_ERR_INVALID_ARG for cpu_id 1.
 *
 * @param cpu_id CPU where function should be executed (0 or 1)
 * @param func pointer to a function which should be executed
 * @param arg arbitrary argument to be passed into function
 *
 * @return ESP_ERR_INVALID_ARG if cpu_id is invalid
 *         ESP_ERR_INVALID_STATE if FreeRTOS scheduler is not running
 *         ESP_OK otherwise
 */
esp_err_t esp_ipc_call_blocking(uint32_t cpu_id, esp_ipc_func_t func, void* arg);


#ifdef __cplusplus
}
#endif

#endif /* __ESP_IPC_H__ */
