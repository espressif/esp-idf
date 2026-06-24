/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_IPC_ISR_ENABLE

/**
 * @brief IPC ISR Callback
 *
 * The callback must be written:
 *    - in assembly for XTENSA chips (such as ESP32, ESP32S3).
 *    - in C or assembly for RISCV chips (such as ESP32P4).
 *
 * A callback of this type should be provided as an argument when calling esp_ipc_isr_call() or
 * esp_ipc_isr_call_blocking().
 */
typedef void (*esp_ipc_isr_func_t)(void* arg);

/**
 * @brief Execute an ISR callback on the other CPU
 *
 * Execute a given callback on the other CPU in the context of a High Priority Interrupt.
 *
 * - This function will busy-wait in a critical section until the other CPU has started execution of the callback
 * - The callback must be written:
 *    - in assembly for XTENSA chips (such as ESP32, ESP32S3).
 *      The function is invoked using a CALLX0 instruction and can use only a2, a3, a4 registers.
 *      See :doc:`IPC in Interrupt Context </api-reference/system/ipc>` doc for more details.
 *    - in C or assembly for RISCV chips (such as ESP32P4).
 *
 * @note This function is not available in single-core mode.
 * @note This function can be called even if the other core is stalled
 *       (either via esp_ipc_isr_stall_other_cpu() or esp_ipc_isr_stall_other_cpu_safe()).
 *       This allows safely stalling the other CPU and executing one or more callbacks before releasing it.
 *
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 */
void esp_ipc_isr_call(esp_ipc_isr_func_t func, void* arg);

/**
 * @brief Execute an ISR callback on the other CPU
 * See esp_ipc_isr_call().
 */
#define esp_ipc_isr_asm_call(func, arg) esp_ipc_isr_call(func, arg)

/**
 * @brief Execute an ISR callback on the other CPU and busy-wait until it completes
 *
 * This function is identical to esp_ipc_isr_call() except that this function will busy-wait until the execution of
 * the callback completes.
 *
 * @note This function is not available in single-core mode.
 * @note This function can be called even if the other core is stalled
 *       (either via esp_ipc_isr_stall_other_cpu() or esp_ipc_isr_stall_other_cpu_safe()).
 *       This allows safely stalling the other CPU and executing one or more callbacks before releasing it.
 *
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 */
void esp_ipc_isr_call_blocking(esp_ipc_isr_func_t func, void* arg);

/**
 * @brief Execute an ISR callback on the other CPU and busy-wait until it completes
 * See esp_ipc_isr_call_blocking().
 */
#define esp_ipc_isr_asm_call_blocking(func, arg) esp_ipc_isr_call_blocking(func, arg)

/**
 * @brief Stall the other CPU unconditionally
 *
 * This function forces the other CPU to enter a busy-wait loop within a High Priority Interrupt context,
 * effectively stalling its execution until esp_ipc_isr_release_other_cpu() is called.
 *
 * Typical use cases include:
 *  - DPORT workaround (e.g., DPORT or APB registers) on dual-core ESP32 chips prior to v2.0.
 *  - Scenarios where the state of the other CPU does not need to be checked, stalling unconditionally.
 *
 * Implementation details:
 *  - The function is initialized after FreeRTOS startup.
 *  - When invoked, it signals the other CPU to enter a high-priority interrupt and remain stalled.
 *  - If the other CPU is already in a high-priority interrupt, it is considered stalled.
 *  - The other CPU will remain stalled until esp_ipc_isr_release_other_cpu() is called.
 *  - This function does not check the current state (e.g., critical section or ISR) of the other CPU.
 *    To avoid potential deadlocks on spinlocks, use esp_ipc_isr_stall_other_cpu_safe() instead.
 *  - If the stall feature is paused via esp_ipc_isr_stall_pause(), this function has no effect.
 *
 * @note Not available in single-core mode.
 * @note The caller is responsible for ensuring no deadlocks on spinlocks occur. Use esp_ipc_isr_stall_other_cpu_safe() for safer operation.
 */
void esp_ipc_isr_stall_other_cpu(void);

/**
 * @brief Safely stall the other CPU
 *
 * Attempts to stall the other CPU only if it is not currently in a critical section or ISR context.
 * If the other CPU is in a critical section or ISR, the function will return an error.
 *
 * This helps to prevent potential deadlocks when both CPUs may access shared resources/spinlocks.
 *
 * @note This function is intended for scenarios where safe stalling is required and the state of the other CPU must be checked.
 *
 * @return
 *      - ESP_OK: The other CPU was successfully stalled.
 *      - ESP_ERR_NOT_ALLOWED: The other CPU is in a critical section or ISR context and cannot be stalled.
 */
esp_err_t esp_ipc_isr_stall_other_cpu_safe(void);

/**
 * @brief Check whether the other CPU is currently stalled
 *
 * @return true if the other CPU has entered the IPC ISR stall loop, false otherwise.
 */
bool esp_ipc_isr_is_other_cpu_stalled(void);

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
 * This function will abort any stalling routine of the other CPU due to a previous call to
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
#define esp_ipc_isr_stall_other_cpu_safe() (ESP_OK)
#define esp_ipc_isr_is_other_cpu_stalled() (false)
#define esp_ipc_isr_release_other_cpu()
#define esp_ipc_isr_stall_pause()
#define esp_ipc_isr_stall_abort()
#define esp_ipc_isr_stall_resume()

#endif // CONFIG_ESP_IPC_ISR_ENABLE

#ifdef __cplusplus
}
#endif
