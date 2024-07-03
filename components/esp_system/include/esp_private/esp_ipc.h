/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "../esp_ipc.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)

/**
 * @brief Execute a callback on a given CPU without any blocking operations for the caller
 *
 * Since it does not have any blocking operations it is suitable to be run from interrupts
 * or even when the Scheduler on the current core is suspended.
 *
 * The function:
 * - does not wait for the callback to begin or complete execution,
 * - does not change the IPC priority.
 * The function returns after sending a notification to the IPC task to execute the callback.
 *
 * @param[in]   cpu_id  CPU where the given function should be executed (0 or 1)
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 *
 * @return
 *      - ESP_ERR_INVALID_ARG if cpu_id is invalid
 *      - ESP_ERR_INVALID_STATE 1. IPC tasks have not been initialized yet,
 *                              2. cpu_id requests IPC on the current core, but the FreeRTOS scheduler is not running on it
 *                              (the IPC task cannot be executed).
 *      - ESP_FAIL IPC is busy due to a previous call was not completed.
 *      - ESP_OK otherwise
 */
esp_err_t esp_ipc_call_nonblocking(uint32_t cpu_id, esp_ipc_func_t func, void* arg);

#endif // !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)

#ifdef __cplusplus
}
#endif
