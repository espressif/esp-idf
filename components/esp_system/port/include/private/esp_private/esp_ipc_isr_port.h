/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
 * @brief Initialize the IPC ISR interrupt for a specific CPU.
 *
 * This function initializes the IPC ISR (Inter-Processor Communication Interrupt)
 * for a specific CPU core. It configures the interrupt source and enables the
 * IPC ISR interrupt for the specified CPU.
 *
 * @param[in] cpuid The ID of the CPU core to initialize IPC ISR for.
 */
void esp_ipc_isr_port_init(const int cpuid);

/**
 * @brief Trigger an interrupt on a specific CPU core.
 *
 * @param[in] cpuid The ID of the CPU core to trigger the interrupt on (0 or 1).
 */
void esp_ipc_isr_port_int_trigger(const int cpuid);

#endif // CONFIG_ESP_IPC_ISR_ENABLE

#ifdef __cplusplus
}
#endif
