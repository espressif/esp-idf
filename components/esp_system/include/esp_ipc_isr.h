/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_IPC_ISR_ENABLE

/**
 * @brief IPC ISR Callback
 *
 * A callback of this type should be provided as an argument when calling esp_ipc_isr_asm_call() or
 * esp_ipc_isr_asm_call_blocking().
 */
typedef void (*esp_ipc_isr_func_t)(void* arg);

/**
 * @brief Execute an assembly callback on the other CPU
 *
 * Execute a given callback on the other CPU in the context of a High Priority Interrupt.
 *
 * - This function will busy-wait in a critical section until the other CPU has started execution of the callback
 * - The callback must be written in assembly, is invoked using a CALLX0 instruction, and has a2, a3, a4 as scratch
 *   registers. See docs for more details
 *
 * @note This function is not available in single-core mode.
 *
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 */
void esp_ipc_isr_asm_call(esp_ipc_isr_func_t func, void* arg);

/**
 * @brief Execute an assembly callback on the other CPU and busy-wait until it completes
 *
 * This function is identical to esp_ipc_isr_asm_call() except that this function will busy-wait until the execution of
 * the callback completes.
 *
 * @note This function is not available in single-core mode.
 *
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 */
void esp_ipc_isr_asm_call_blocking(esp_ipc_isr_func_t func, void* arg);

/**
 * @brief Stall the other CPU
 *
 * This function will stall the other CPU. The other CPU is stalled by busy-waiting in the context of a High Priority
 * Interrupt. The other CPU will not be resumed until esp_ipc_isr_release_other_cpu() is called.
 *
 * - This function is internally implemented using IPC ISR
 * - This function is used for DPORT workaround.
 * - If the stall feature is paused using esp_ipc_isr_stall_pause(), this function will have no effect
 *
 * @note This function is not available in single-core mode.
 * @note It is the caller's responsibility to avoid deadlocking on spinlocks
 */
void esp_ipc_isr_stall_other_cpu(void);

/**
 * @brief Release the other CPU
 *
 * This function will release the other CPU that was previously stalled from calling esp_ipc_isr_stall_other_cpu()
 *
 * - This function is used for DPORT workaround.
 * - If the stall feature is paused using esp_ipc_isr_stall_pause(), this function will have no effect
 *
 * @note This function is not available in single-core mode.
 */
void esp_ipc_isr_release_other_cpu(void);

/**
 * @brief Puase the CPU stall feature
 *
 * This function will pause the CPU stall feature. Once paused, calls to esp_ipc_isr_stall_other_cpu() and
 * esp_ipc_isr_release_other_cpu() will have no effect. If a IPC ISR call is already in progress, this function will
 * busy-wait until the call completes before pausing the CPU stall feature.
 */
void esp_ipc_isr_stall_pause(void);

/**
 * @brief Abort a CPU stall
 *
 * This function will abort any stalling routine of the other CPU due to a pervious call to
 * esp_ipc_isr_stall_other_cpu(). This function aborts the stall in a non-recoverable manner, thus should only be called
 * in case of a panic().
 *
 * - This function is used in panic handling code
 */
void esp_ipc_isr_stall_abort(void);

/**
 * @brief Resume the CPU stall feature
 *
 * This function will resume the CPU stall feature that was previously paused by calling esp_ipc_isr_stall_pause(). Once
 * resumed, calls to esp_ipc_isr_stall_other_cpu() and esp_ipc_isr_release_other_cpu() will have effect again.
 */
void esp_ipc_isr_stall_resume(void);

#else // CONFIG_ESP_IPC_ISR_ENABLE

#define esp_ipc_isr_stall_other_cpu()
#define esp_ipc_isr_release_other_cpu()
#define esp_ipc_isr_stall_pause()
#define esp_ipc_isr_stall_abort()
#define esp_ipc_isr_stall_resume()

#endif // CONFIG_ESP_IPC_ISR_ENABLE

#ifdef __cplusplus
}
#endif
