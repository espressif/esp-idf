/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
 * This file is like "idf_additions.h" but for private API (i.e., only meant to
 * be called internally by other ESP-IDF components.
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/*------------------------------------------------------------------------------
 * KERNEL CONTROL (PRIVATE)
 *----------------------------------------------------------------------------*/

/*
 * The following macros are convenience macros used to account for different
 * thread safety behavior between Vanilla FreeRTOS (i.e., single-core) and ESP-IDF
 * FreeRTOS (i.e., multi-core SMP).
 *
 * For thread saftey...
 *
 * - Vanilla FreeRTOS will use the following for thread safety (depending on situation)
 *      - `vTaskSuspendAll()`/`xTaskResumeAll()` for non-deterministic operations
 *      - Critical sections or disabling interrupts for deterministic operations
 * - ESP-IDF FreeRTOS will always use critical sections (determinism is not supported)
 *
 * [refactor-todo]: Define these locally in each kernel source file (IDF-8161)
 */
#if ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) )

    #define prvENTER_CRITICAL_OR_SUSPEND_ALL( x )    taskENTER_CRITICAL( ( x ) )
    #define prvEXIT_CRITICAL_OR_RESUME_ALL( x )      ( { taskEXIT_CRITICAL( ( x ) ); pdFALSE; } )
    #define prvENTER_CRITICAL_OR_MASK_ISR( pxLock, uxInterruptStatus ) \
    {                                                                  \
        taskENTER_CRITICAL_ISR( ( pxLock ) );                          \
        ( void ) ( uxInterruptStatus );                                \
    }
    #define prvEXIT_CRITICAL_OR_UNMASK_ISR( pxLock, uxInterruptStatus ) \
    {                                                                   \
        taskEXIT_CRITICAL_ISR( ( pxLock ) );                            \
        ( void ) ( uxInterruptStatus );                                 \
    }

#elif ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES == 1 ) )

    #define prvENTER_CRITICAL_OR_SUSPEND_ALL( x )    ( { vTaskSuspendAll(); ( void ) ( x ); } )
    #define prvEXIT_CRITICAL_OR_RESUME_ALL( x )      xTaskResumeAll()
    #define prvENTER_CRITICAL_OR_MASK_ISR( pxLock, uxInterruptStatus )  \
    {                                                                   \
        ( uxInterruptStatus ) = portSET_INTERRUPT_MASK_FROM_ISR();      \
        ( void ) ( pxLock );                                            \
    }
    #define prvEXIT_CRITICAL_OR_UNMASK_ISR( pxLock, uxInterruptStatus )  \
    {                                                                    \
        portCLEAR_INTERRUPT_MASK_FROM_ISR( ( uxInterruptStatus ) );      \
        ( void ) ( pxLock );                                             \
    }

#endif /* ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES == 1 ) ) */

/*
 * In ESP-IDF FreeRTOS (i.e., multi-core SMP) uses spinlocks to protect different
 * groups of data. This function is a wrapper to take the "xKernelLock" spinlock
 * of tasks.c.
 *
 * This lock is taken whenever any of the kernel's data structures are
 * accessed/modified, such as when adding/removing tasks to/from the delayed
 * task list or various event lists.
 *
 * In more cases, kernel data structures are not accessed by functions outside
 * tasks.c. Thus, all accesses of the kernel data structures inside tasks.c will
 * handle the taking/releasing of the "xKerneLock".
 *
 * This functions is meant to be called by xEventGroupSetBits() and
 * vEventGroupDelete() as both those functions will directly access event lists
 * (which are kernel data structures). Thus, a wrapper function must be provided
 * to take/release the "xKernelLock" from outside tasks.c.
 *
 * [refactor-todo]: Extern this locally in event groups (IDF-8161)
 */
#if ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) )

    void prvTakeKernelLock( void );
    void prvReleaseKernelLock( void );

#endif /* ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) ) */

#if ( CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) )

/**
 * @brief Workaround for non-thread safe multi-core OS startup (see IDF-4524)
 *
 * This function must be called with interrupts disabled on all cores other than
 * core 0 during startup.
 */
    void prvStartSchedulerOtherCores( void );

#endif /* ( CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) ) */

/*
 * In ESP-IDF FreeRTOS (i.e., multi-core SMP), core 0 manages the the FreeRTOS
 * tick count. Thus only core 0 calls xTaskIncrementTick().
 *
 * However, all other cores also receive a periodic tick interrupt. Thus all
 * other cores should call this function instead.
 *
 * This function will check if the current core requires time slicing, and also
 * call the application tick hook. However, the tick count will remain unchanged.
 */
#if ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) )

    BaseType_t xTaskIncrementTickOtherCores( void );

#endif /* ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) ) */

/*------------------------------------------------------------------------------
 * TASK UTILITIES (PRIVATE)
 *----------------------------------------------------------------------------*/
#if ( INCLUDE_vTaskPrioritySet == 1 )

/**
 * @brief Structure to save a task's previous priority
 *
 * This structure is meant to be used with prvTaskPriorityRaise() and
 * prvTaskPriorityRestore().
 */
    typedef struct
    {
        UBaseType_t uxPriority;
        #if ( configUSE_MUTEXES == 1 )
            UBaseType_t uxBasePriority;
        #endif
    } prvTaskSavedPriority_t;

#endif /* INCLUDE_vTaskPrioritySet == 1 */

#if ( INCLUDE_vTaskPrioritySet == 1 )

/**
 * INCLUDE_vTaskPrioritySet must be defined as 1 for this function to be
 * available. See the configuration section for more information.
 *
 * Saves the current priority and current base priority of a task, then raises
 * the task's current and base priority to uxNewPriority if uxNewPriority is of
 * a higher priority.
 *
 * Once a task's priority has been raised with this function, the priority
 * can be restored by calling prvTaskPriorityRestore()
 *
 * - Note that this function differs from vTaskPrioritySet() as the task's
 *   current priority will be modified even if the task has already
 *   inherited a priority.
 * - This function is intended for special circumstance where a task must be
 *   forced immediately to a higher priority.
 *
 * For configUSE_MUTEXES == 0: A context switch will occur before the
 * function returns if the priority being set is higher than the priority of the
 * currently executing task.
 *
 * @note This functions is private and should only be called internally
 * within various IDF components. Users should never call this function from
 * their application.
 *
 * @note vTaskPrioritySet() should not be called while a task's priority is
 * already raised via this function
 *
 * @param pxSavedPriority returns base and current priorities
 *
 * @param uxNewPriority The priority to which the task's priority will be
 * set.
 */
    void prvTaskPriorityRaise( prvTaskSavedPriority_t * pxSavedPriority,
                               UBaseType_t uxNewPriority );

#endif /* INCLUDE_vTaskPrioritySet == 1 */

#if ( INCLUDE_vTaskPrioritySet == 1 )

/**
 * INCLUDE_vTaskPrioritySet must be defined as 1 for this function to be
 * available. See the configuration section for more information.
 *
 * Restore a task's priority that was previously raised by
 * prvTaskPriorityRaise().
 *
 * For configUSE_MUTEXES == 0: A context switch will occur before the function
 * returns if the priority being set is higher than the priority of the currently
 * executing task.
 *
 * @note This functions is private and should only be called internally within
 * various IDF components. Users should never call this function from their
 * application.
 *
 * @param pxSavedPriority previously saved base and current priorities that need
 * to be restored
 */
    void prvTaskPriorityRestore( prvTaskSavedPriority_t * pxSavedPriority );

#endif /* INCLUDE_vTaskPrioritySet == 1 */

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */
