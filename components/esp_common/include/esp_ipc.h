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
/** @cond */
typedef void (*esp_ipc_func_t)(void* arg);
/** @endcond */
/*
 * Inter-processor call APIs
 *
 * FreeRTOS provides several APIs which can be used to communicate between
 * different tasks, including tasks running on different CPUs.
 * This module provides additional APIs to run some code on the other CPU.
 *
 * These APIs can only be used when FreeRTOS scheduler is running.
 */

/**
 * @brief Execute a function on the given CPU
 *
 * Run a given function on a particular CPU. The given function must accept a
 * void* argument and return void. The given function is run in the context of
 * the IPC task of the CPU specified by the cpu_id parameter. The calling task
 * will be blocked until the IPC task begins executing the given function. If
 * another IPC call is ongoing, the calling task will block until the other IPC
 * call completes. The stack size allocated for the IPC task can be configured
 * in the "Inter-Processor Call (IPC) task stack size" setting in menuconfig.
 * Increase this setting if the given function requires more stack than default.
 *
 * @note In single-core mode, returns ESP_ERR_INVALID_ARG for cpu_id 1.
 *
 * @param[in]   cpu_id  CPU where the given function should be executed (0 or 1)
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 *
 * @return
 *      - ESP_ERR_INVALID_ARG if cpu_id is invalid
 *      - ESP_ERR_INVALID_STATE if the FreeRTOS scheduler is not running
 *      - ESP_OK otherwise
 */
esp_err_t esp_ipc_call(uint32_t cpu_id, esp_ipc_func_t func, void* arg);


/**
 * @brief Execute a function on the given CPU and blocks until it completes
 *
 * Run a given function on a particular CPU. The given function must accept a
 * void* argument and return void. The given function is run in the context of
 * the IPC task of the CPU specified by the cpu_id parameter. The calling task
 * will be blocked until the IPC task completes execution of the given function.
 * If another IPC call is ongoing, the calling task will block until the other
 * IPC call completes. The stack size allocated for the IPC task can be
 * configured in the "Inter-Processor Call (IPC) task stack size" setting in
 * menuconfig. Increase this setting if the given function requires more stack
 * than default.
 *
 * @note    In single-core mode, returns ESP_ERR_INVALID_ARG for cpu_id 1.
 *
 * @param[in]   cpu_id  CPU where the given function should be executed (0 or 1)
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 *
 * @return
 *      - ESP_ERR_INVALID_ARG if cpu_id is invalid
 *      - ESP_ERR_INVALID_STATE if the FreeRTOS scheduler is not running
 *      - ESP_OK otherwise
 */
esp_err_t esp_ipc_call_blocking(uint32_t cpu_id, esp_ipc_func_t func, void* arg);


#ifdef __cplusplus
}
#endif

#endif /* __ESP_IPC_H__ */
