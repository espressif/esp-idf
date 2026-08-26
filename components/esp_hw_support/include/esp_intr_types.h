/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Function prototype for interrupt handler function */
typedef void (*intr_handler_t)(void *arg);

/** Handle to an interrupt handler */
typedef struct intr_handle_data_t *intr_handle_t;

/**
 * @brief Interrupt CPU core affinity
 *
 * This type specify the CPU core that the peripheral interrupt is connected to.
 */
typedef enum {
    ESP_INTR_CPU_AFFINITY_AUTO, ///< Install the peripheral interrupt to ANY CPU core, decided by on which CPU the interrupt allocator is running
    ESP_INTR_CPU_AFFINITY_0,    ///< Install the peripheral interrupt to CPU core 0
    ESP_INTR_CPU_AFFINITY_1,    ///< Install the peripheral interrupt to CPU core 1
} esp_intr_cpu_affinity_t;

/// Convert esp_intr_cpu_affinity_t to CPU core ID
#define ESP_INTR_CPU_AFFINITY_TO_CORE_ID(cpu_affinity) ((cpu_affinity) - 1)

#ifdef __cplusplus
}
#endif
