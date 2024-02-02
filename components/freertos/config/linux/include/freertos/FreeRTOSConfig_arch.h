/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* Linux POSIX simulator specific configuration. This file is included in the common FreeRTOSConfig.h. */

#include "sdkconfig.h"

/* ------------------------------------------------- FreeRTOS Config ---------------------------------------------------
 * - All Vanilla FreeRTOS configuration goes into this section
 * - Match upstream POSIX simulator example FreeRTOSConfig.h where possible. See following link for more details.
 *   https://github.com/FreeRTOS/FreeRTOS/blob/main/FreeRTOS/Demo/Posix_GCC/FreeRTOSConfig.h
 * ------------------------------------------------------------------------------------------------------------------ */

/* ------------------ Scheduler Related -------------------- */

#define configUSE_PORT_OPTIMISED_TASK_SELECTION    0

/* The stack allocated by FreeRTOS will be passed to a pthread.
 * pthread has a minimal stack size which currently is 16KB.
 * The rest is for additional structures of the POSIX/Linux port.
 * This is a magic number since PTHREAD_STACK_MIN seems to not be a constant. */
#define configMINIMAL_STACK_SIZE                   ( ( StackType_t ) ( 0x4000 + 40 ) / sizeof( StackType_t ) )
/* Currently not used in Linux POSIX simulator */
#define configMAX_API_CALL_INTERRUPT_PRIORITY      0

/* ---------------- Amazon SMP FreeRTOS -------------------- */

#if CONFIG_FREERTOS_SMP
    #define configUSE_PASSIVE_IDLE_HOOK    0             /* Not implemented yet, TODO IDF-6654 */
#endif

/* ----------------------- System -------------------------- */

/* On the Linux simulator, we use the system-provided libc */
#define configUSE_NEWLIB_REENTRANT                   0

#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H    1

/* ----------------------- Memory  ------------------------- */

#define configTOTAL_HEAP_SIZE    ( ( size_t ) ( 65 * 1024 ) )

/* ------------------- Run-time Stats ---------------------- */

#define configUSE_TRACE_FACILITY    1

/* -------------------- API Includes ----------------------- */

/* Todo: Reconcile INCLUDE_option differences (IDF-8186) */
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_uxTaskGetStackHighWaterMark2    0

/* ------------------------------------------------ ESP-IDF Additions --------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */
