/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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

#ifdef CONFIG_FREERTOS_OPTIMIZED_SCHEDULER
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#else
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION    0
#endif /* CONFIG_FREERTOS_OPTIMIZED_SCHEDULER */
#define configMINIMAL_STACK_SIZE                       ( CONFIG_FREERTOS_IDLE_TASK_STACKSIZE + configSTACK_OVERHEAD_TOTAL )
#define configMAX_API_CALL_INTERRUPT_PRIORITY          0

/* ---------------- Amazon SMP FreeRTOS -------------------- */

#if CONFIG_FREERTOS_SMP

/* This is always enabled to call IDF style idle hooks, by can be "--Wl,--wrap"
 * if users enable CONFIG_FREERTOS_USE_PASSIVE_IDLE_HOOK. */
    #define configUSE_PASSIVE_IDLE_HOOK    1
#endif

/* ----------------------- System -------------------------- */

#if CONFIG_LIBC_NEWLIB
#define configUSE_NEWLIB_REENTRANT    1
#else
#define configUSE_NEWLIB_REENTRANT    0
#endif

/* - FreeRTOS provides default for configTLS_BLOCK_TYPE.
 * - We simply provide our own INIT and DEINIT functions
 * - We set "SET" to a blank macro since there is no need to set the reentrancy
 * pointer. All newlib functions calls __getreent. */
#if CONFIG_FREERTOS_SMP
    #define configINIT_TLS_BLOCK( xTLSBlock, pxTopOfStack )    esp_reent_init( &( xTLSBlock ) )
#else /* CONFIG_FREERTOS_SMP */
    #define configINIT_TLS_BLOCK( xTLSBlock )                  esp_reent_init( &( xTLSBlock ) )
#endif /* CONFIG_FREERTOS_SMP */
#define configSET_TLS_BLOCK( xTLSBlock )
#define configDEINIT_TLS_BLOCK( xTLSBlock )                    _reclaim_reent( &( xTLSBlock ) )

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

/* Todo: Reconcile INCLUDE_option differences (IDF-8186) */
#define INCLUDE_xTaskDelayUntil                 1
#define INCLUDE_uxTaskGetStackHighWaterMark2    1

/* ------------------------------------------------ ESP-IDF Additions --------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#ifndef configISR_STACK_SIZE
    #define configISR_STACK_SIZE    ( CONFIG_FREERTOS_ISR_STACKSIZE )
#endif
