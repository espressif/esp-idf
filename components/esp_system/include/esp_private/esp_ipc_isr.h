/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#define ESP_IPC_ISR_ARGS_CMD_OFFSET       0
#define ESP_IPC_ISR_ARGS_FUNC_OFFSET      4
#define ESP_IPC_ISR_ARGS_ARG_OFFSET       8
#define ESP_IPC_ISR_ARGS_RET_ADDR_OFFSET  12
#define ESP_IPC_ISR_ARGS_INTERRUPTED_CONTEXT_UNSAFE_OFFSET 16

#ifndef __ASSEMBLER__
#include <stddef.h>
#include <stdint.h>
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_IPC_ISR_ENABLE

/**
 * @brief IPC ISR command types.
 *
 * Enumerates the possible commands that caller CPU can issue to the stalled CPU.
 */
typedef enum {
    ESP_IPC_ISR_CMD_RESET_STATE = 0, /**< Reset the command state. */
    ESP_IPC_ISR_CMD_FINISH = 1,      /**< Signal that the ISR operation is finished. */
} esp_ipc_isr_cmd_t;

/**
 * @brief Arguments structure for IPC ISR functions.
 *
 * This structure holds the function pointer and its argument to be used in callbacks.
 */
typedef struct {
    esp_ipc_isr_cmd_t cmd;      /**< Command to control the IPC ISR operation. */
    void (*func)(void*);        /**< Pointer to the function to be called in the ISR. */
    void* arg;                  /**< Argument to be passed to the function. */
    void* save_ret_addr;        /**< Address to save the return address for the ISR (used by xtensa chips). */
    uint32_t interrupted_context_unsafe; /**< Non-zero if the interrupted CPU context is unsafe to stall. */
} esp_ipc_isr_args_t;

ESP_STATIC_ASSERT(offsetof(esp_ipc_isr_args_t, cmd) == ESP_IPC_ISR_ARGS_CMD_OFFSET, "Unexpected IPC ISR cmd offset");
ESP_STATIC_ASSERT(offsetof(esp_ipc_isr_args_t, func) == ESP_IPC_ISR_ARGS_FUNC_OFFSET, "Unexpected IPC ISR func offset");
ESP_STATIC_ASSERT(offsetof(esp_ipc_isr_args_t, arg) == ESP_IPC_ISR_ARGS_ARG_OFFSET, "Unexpected IPC ISR arg offset");
ESP_STATIC_ASSERT(offsetof(esp_ipc_isr_args_t, save_ret_addr) == ESP_IPC_ISR_ARGS_RET_ADDR_OFFSET, "Unexpected IPC ISR return address offset");
ESP_STATIC_ASSERT(offsetof(esp_ipc_isr_args_t, interrupted_context_unsafe) == ESP_IPC_ISR_ARGS_INTERRUPTED_CONTEXT_UNSAFE_OFFSET,
                  "Unexpected IPC ISR interrupted context unsafe offset");

/**
 * @brief Initialize the IPC ISR feature, must be called for each CPU
 *
 * @note This function is called from ipc_task().
 *
 * This function initializes the IPC ISR feature and must be called before any other esp_ipc_isr...() functions.
 * The IPC ISR feature allows for callbacks (written in assembly) to be run on a particular CPU in the context of a
 * High Priority Interrupt.
 *
 * - This function will register a High Priority Interrupt for a CPU where it is called. The priority of the interrupts is dependent on
 *   the CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL option.
 * - Callbacks written in assembly can then run in context of the registered High Priority Interrupts
 * - Callbacks can be executed by calling esp_ipc_isr_call() or esp_ipc_isr_call_blocking()
 */
void esp_ipc_isr_init(void);

#endif // CONFIG_ESP_IPC_ISR_ENABLE

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__
