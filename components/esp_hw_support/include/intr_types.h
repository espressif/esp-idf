/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interrupt core ID type
 *
 * This type represents on which core your ISR is registered
 */
typedef enum {
    INTR_CPU_ID_AUTO,       ///< Register intr ISR to core automatically, this means the core on which you call `esp_intr_alloc`
    INTR_CPU_ID_0,          ///< Register intr ISR to core 0.
    INTR_CPU_ID_1,          ///< Register intr ISR to core 1.
} intr_cpu_id_t;

#define INTR_CPU_CONVERT_ID(cpu_id)     ((cpu_id) - 1)

#ifdef __cplusplus
}
#endif
