/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FREERTOS_CONFIG_RISCV_H
#define FREERTOS_CONFIG_RISCV_H

//RISC-V Archiecture specific configuration. This file is included in the common FreeRTOSConfig.h.

#include "sdkconfig.h"

/* ------------------------------------------------- FreeRTOS Config ---------------------------------------------------
 * - All Vanilla FreeRTOS configuration goes into this section
 * ------------------------------------------------------------------------------------------------------------------ */

// ------------------ Scheduler Related --------------------

#define configUSE_PORT_OPTIMISED_TASK_SELECTION             0
#define configMAX_API_CALL_INTERRUPT_PRIORITY               0

/* ------------------------------------------------ ESP-IDF Additions --------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#ifndef configISR_STACK_SIZE
#define configISR_STACK_SIZE                                (CONFIG_FREERTOS_ISR_STACKSIZE)
#endif

#endif // FREERTOS_CONFIG_RISCV_H
