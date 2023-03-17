/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* RISC-V Architecture specific configuration. This file is included in the common FreeRTOSConfig.h. */

#include "sdkconfig.h"

/* ------------------------------------------------- FreeRTOS Config ---------------------------------------------------
 * - All Vanilla FreeRTOS configuration goes into this section
 * ------------------------------------------------------------------------------------------------------------------ */

/* ------------------ Scheduler Related -------------------- */

#define configMAX_PRIORITIES                           ( 25 )
#ifdef CONFIG_FREERTOS_OPTIMIZED_SCHEDULER
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#else
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION    0
#endif /* CONFIG_FREERTOS_OPTIMIZED_SCHEDULER */
#define configMINIMAL_STACK_SIZE                       ( CONFIG_FREERTOS_IDLE_TASK_STACKSIZE + configSTACK_OVERHEAD_TOTAL )
#define configMAX_API_CALL_INTERRUPT_PRIORITY          0

/* ----------------------- System -------------------------- */

#define configUSE_NEWLIB_REENTRANT                   1
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H    1

/* ----------------------- Memory  ------------------------- */

/* This isn't used as FreeRTOS will only allocate from internal memory (see
 * heap_idf.c). We simply define this macro to span all non-statically-allocated
 * shared RAM. */
#define configTOTAL_HEAP_SIZE    ( &_heap_end - &_heap_start )

/* ------------------- Run-time Stats ---------------------- */

#if CONFIG_FREERTOS_USE_TRACE_FACILITY
    /* Used by uxTaskGetSystemState(), and other trace facility functions */
    #define configUSE_TRACE_FACILITY    1
#endif /* CONFIG_FREERTOS_USE_TRACE_FACILITY */

/* -------------------- API Includes ----------------------- */

#define INCLUDE_xTaskDelayUntil              1
#define INCLUDE_xTaskGetCurrentTaskHandle    1

/* ------------------------------------------------ ESP-IDF Additions --------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#ifndef configISR_STACK_SIZE
    #define configISR_STACK_SIZE    ( CONFIG_FREERTOS_ISR_STACKSIZE )
#endif
