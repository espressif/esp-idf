/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handles any pending IPC functions
 *
 * @param cpuid Core that should handle the IPC functions
 * 
 * @note This function is called from esp_crosscore_isr() or ipc_task() depending on availability of RTOS on specified core.
 */
void ipc_handle(const int cpuid);

#ifdef __cplusplus
}
#endif
