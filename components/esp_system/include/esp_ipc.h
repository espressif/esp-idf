/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)

/*
 * Inter-processor call APIs
 *
 * FreeRTOS provides several APIs which can be used to communicate between different tasks, including tasks running on
 * different CPUs. This module provides additional APIs to run some code on the other CPU. These APIs can only be used
 * when FreeRTOS scheduler is running.
 */

/**
 * @brief IPC Callback
 *
 * A callback of this type should be provided as an argument when calling esp_ipc_call() or esp_ipc_call_blocking().
 */
typedef void (*esp_ipc_func_t)(void* arg);

/**
 * @brief Execute a callback on a given CPU
 *
 * Execute a given callback on a particular CPU. The callback must be of type "esp_ipc_func_t" and will be invoked in
 * the context of the target CPU's IPC task.
 *
 * - This function will block the target CPU's IPC task has begun execution of the callback
 * - If another IPC call is ongoing, this function will block until the ongoing IPC call completes
 * - The stack size of the IPC task can be configured via the CONFIG_ESP_IPC_TASK_STACK_SIZE option
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
 * @brief Execute a callback on a given CPU until and block until it completes
 *
 * This function is identical to esp_ipc_call() except that this function will block until the execution of the callback
 * completes.
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

#endif // !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)

#ifdef __cplusplus
}
#endif
