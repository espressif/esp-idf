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
