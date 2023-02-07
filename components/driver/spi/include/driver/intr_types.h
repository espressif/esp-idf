/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

///< Selecting the ISR to be registered on which core
typedef enum {
    INTR_CPU_ID_AUTO,       ///< Register intr ISR to core automatically select by FreeRTOS.
    INTR_CPU_ID_0,          ///< Register intr ISR to core 0.
    INTR_CPU_ID_1,          ///< Register intr ISR to core 1.
} intr_cpu_id_t;

#define INTR_CPU_CONVERT_ID(cpu_id)     ((cpu_id) - 1)

#ifdef __cplusplus
}
#endif
