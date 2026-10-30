/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Public API for printing a task backtrace on the FreeRTOS Linux simulator.
 *
 * This header is included by other components (e.g. esp_system) that need
 * to print the backtrace of the currently scheduled task without depending
 * on the FreeRTOS port's private utility headers.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print the backtrace of the currently scheduled FreeRTOS task.
 *
 * Sends SIGUSR1 to the scheduled task's pthread, whose handler writes the
 * backtrace to stderr. Intended for diagnostics such as watchdog timeouts.
 */
void freertos_linux_print_backtrace(void);

#ifdef __cplusplus
}
#endif
