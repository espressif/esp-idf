/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_IPC_ISR_ENABLE

/** @cond */
typedef void (*esp_ipc_isr_func_t)(void* arg);
/** @endcond */

/**
 * @brief Initialize inter-processor call module which based on #4 high-interrupt.
 *
 * This function is called on CPU start and should not be called from the application.
 *
 * This function starts two tasks, one on each CPU. These tasks register
 * #4 High-interrupt and after that, the tasks are deleted.
 * The next API functions work with this functionality:
 * esp_ipc_isr_asm_call
 * esp_ipc_isr_asm_call_blocking
 * They allow to run an asm function on other CPU.
 */
void esp_ipc_isr_init(void);

/**
 * @brief Execute an asm function on the other CPU (uses the #4 high-priority interrupt)
 *
 * @note In single-core mode, it is not available.
 * This function calls the #4 high-priority interrupt on the other CPU.
 * The given function is called in the context of the interrupt by CALLX0 command and
 * operates with registers a2, a3, a4.
 *
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 */
void esp_ipc_isr_asm_call(esp_ipc_isr_func_t func, void* arg);

/**
 * @brief Execute an asm function on the other CPU and blocks until it completes (uses the #4 high-priority interrupt)
 *
 * @note In single-core mode, it is not available.
 * This function calls the #4 high-priority interrupt on the other CPU.
 * The given function is called in the context of the interrupt by CALLX0 command.
 *
 * @param[in]   func    Pointer to a function of type void func(void* arg) to be executed
 * @param[in]   arg     Arbitrary argument of type void* to be passed into the function
 */
void esp_ipc_isr_asm_call_blocking(esp_ipc_isr_func_t func, void* arg);

/**
 * @brief Stall the other CPU and the current CPU disables interrupts with level 3 and lower.
 *
 * @note In single-core mode, it is not available.
 * This function calls the #4 high-priority interrupt on the other CPU.
 * The esp_ipc_isr_finish_cmd() function is called on the other CPU in the context of the #4 high-priority interrupt.
 * The esp_ipc_isr_finish_cmd is called by CALLX0 command.
 * It is waiting for the end command. The command will be sent by esp_ipc_isr_release_other_cpu().
 * This function is used for DPORT workaround.
 *
 * This function blocks other CPU until the release call esp_ipc_isr_release_other_cpu().
 *
 * This fucntion is used for the DPORT workaround: stall other cpu that this cpu is pending to access dport register start.
 */
void esp_ipc_isr_stall_other_cpu(void);

/**
 * @brief Release the other CPU
 *
 * @note In single-core mode, it is not available.
 * This function will send the end command to release the stall other CPU.
 * This function is used for DPORT workaround: stall other cpu that this cpu is pending to access dport register end.
 *
 */
void esp_ipc_isr_release_other_cpu(void);

/**
 * @brief Pause stall the other CPU
 */
void esp_ipc_isr_stall_pause(void);

/**
 * @brief Abort stall the other CPU
 *
 * This routine does not stop the stall routines in any way that is recoverable.
 * Please only call in case of panic().
 * Used in panic code: the enter_critical stuff may be messed up so we just stop everything without checking the mux.
 */
void esp_ipc_isr_stall_abort(void);

/**
 * @brief Resume stall the other CPU
 */
void esp_ipc_isr_stall_resume(void);

#else // not CONFIG_ESP_IPC_ISR_ENABLE

#define esp_ipc_isr_stall_other_cpu()
#define esp_ipc_isr_release_other_cpu()
#define esp_ipc_isr_stall_pause()
#define esp_ipc_isr_stall_abort()
#define esp_ipc_isr_stall_resume()

#endif // CONFIG_ESP_IPC_ISR_ENABLE

#ifdef __cplusplus
}
#endif
