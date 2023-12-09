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
#include "freertos/task.h"

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
 * thread safety behavior between single-core and SMP in ESP-IDF FreeRTOS.
 *
 * For thread saftey...
 *
 * - Single-core will use the following for thread safety (depending on situation)
 *      - `vTaskSuspendAll()`/`xTaskResumeAll()` for non-deterministic operations
 *      - Critical sections or disabling interrupts for deterministic operations
 * - SMP will always use critical sections (determinism is not supported)
 */
#if ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES > 1 ) )

/* Macros that use a critical section when building for SMP */
    #define prvENTER_CRITICAL_OR_SUSPEND_ALL( pxLock )    taskENTER_CRITICAL( ( pxLock ) )
    #define prvEXIT_CRITICAL_OR_RESUME_ALL( pxLock )      ( { taskEXIT_CRITICAL( ( pxLock ) ); pdFALSE; } )
    #define prvENTER_CRITICAL_OR_MASK_ISR( pxLock, uxStatus ) \
    {                                                         \
        taskENTER_CRITICAL_ISR( ( pxLock ) );                 \
        ( void ) ( uxStatus );                                \
    }
    #define prvEXIT_CRITICAL_OR_UNMASK_ISR( pxLock, uxStatus ) \
    {                                                          \
        taskEXIT_CRITICAL_ISR( ( pxLock ) );                   \
        ( void ) ( uxStatus );                                 \
    }

/* Macros that enter/exit a critical section only when building for SMP */
    #define prvENTER_CRITICAL_SMP_ONLY( pxLock )         taskENTER_CRITICAL( pxLock )
    #define prvEXIT_CRITICAL_SMP_ONLY( pxLock )          taskEXIT_CRITICAL( pxLock )
    #define prvENTER_CRITICAL_ISR_SMP_ONLY( pxLock )     taskENTER_CRITICAL_ISR( pxLock )
    #define prvEXIT_CRITICAL_ISR_SMP_ONLY( pxLock )      taskEXIT_CRITICAL_ISR( pxLock )
    #define prvENTER_CRITICAL_SAFE_SMP_ONLY( pxLock )    prvTaskEnterCriticalSafeSMPOnly( pxLock )
    #define prvEXIT_CRITICAL_SAFE_SMP_ONLY( pxLock )     prvTaskExitCriticalSafeSMPOnly( pxLock )

    static inline __attribute__( ( always_inline ) )
    void prvTaskEnterCriticalSafeSMPOnly( portMUX_TYPE * pxLock )
    {
        if( portCHECK_IF_IN_ISR() == pdFALSE )
        {
            taskENTER_CRITICAL( pxLock );
        }
        else
        {
            #ifdef __clang_analyzer__
                /* Teach clang-tidy that ISR version macro can be different */
                configASSERT( 1 );
            #endif
            taskENTER_CRITICAL_ISR( pxLock );
        }
    }

    static inline __attribute__( ( always_inline ) )
    void prvTaskExitCriticalSafeSMPOnly( portMUX_TYPE * pxLock )
    {
        if( portCHECK_IF_IN_ISR() == pdFALSE )
        {
            taskEXIT_CRITICAL( pxLock );
        }
        else
        {
            #ifdef __clang_analyzer__
                /* Teach clang-tidy that ISR version macro can be different */
                configASSERT( 1 );
            #endif
            taskEXIT_CRITICAL_ISR( pxLock );
        }
    }

/* Macros that enter/exit a critical section only when building for single-core */
    #define prvENTER_CRITICAL_SC_ONLY( pxLock )
    #define prvEXIT_CRITICAL_SC_ONLY( pxLock )

/* Macros that enable/disable interrupts only when building for SMP */
    #define prvDISABLE_INTERRUPTS_ISR_SMP_ONLY()             portSET_INTERRUPT_MASK_FROM_ISR()
    #define prvENABLE_INTERRUPTS_ISR_SMP_ONLY( uxStatus )    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxStatus )

#elif ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES == 1 ) )

/* Macros that suspend the scheduler or disables interrupts when building for single-core */
    #define prvENTER_CRITICAL_OR_SUSPEND_ALL( pxLock )    ( { vTaskSuspendAll(); ( void ) ( pxLock ); } )
    #define prvEXIT_CRITICAL_OR_RESUME_ALL( pxLock )      xTaskResumeAll()
    #define prvENTER_CRITICAL_OR_MASK_ISR( pxLock, uxStatus ) \
    {                                                         \
        ( uxStatus ) = portSET_INTERRUPT_MASK_FROM_ISR();     \
        ( void ) ( pxLock );                                  \
    }
    #define prvEXIT_CRITICAL_OR_UNMASK_ISR( pxLock, uxStatus ) \
    {                                                          \
        portCLEAR_INTERRUPT_MASK_FROM_ISR( ( uxStatus ) );     \
        ( void ) ( pxLock );                                   \
    }

/* Macros that enter/exit a critical section only when building for SMP */
    #define prvENTER_CRITICAL_SMP_ONLY( pxLock )
    #define prvEXIT_CRITICAL_SMP_ONLY( pxLock )
    #define prvENTER_CRITICAL_ISR_SMP_ONLY( pxLock )
    #define prvEXIT_CRITICAL_ISR_SMP_ONLY( pxLock )
    #define prvENTER_CRITICAL_SAFE_SMP_ONLY( pxLock )
    #define prvEXIT_CRITICAL_SAFE_SMP_ONLY( pxLock )

/* Macros that enter/exit a critical section only when building for single-core */
    #define prvENTER_CRITICAL_SC_ONLY( pxLock )              taskENTER_CRITICAL( pxLock )
    #define prvEXIT_CRITICAL_SC_ONLY( pxLock )               taskEXIT_CRITICAL( pxLock )

/* Macros that enable/disable interrupts only when building for SMP */
    #define prvDISABLE_INTERRUPTS_ISR_SMP_ONLY()             ( ( UBaseType_t ) 0 )
    #define prvENABLE_INTERRUPTS_ISR_SMP_ONLY( uxStatus )    ( ( void ) uxStatus )

#endif /* ( !CONFIG_FREERTOS_SMP && ( configNUM_CORES == 1 ) ) */

/*
 * In ESP-IDF FreeRTOS under SMP builds, spinlocks are to protect different
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

#if CONFIG_SPIRAM

/**
 * Create a new task with stack having the desired heap capabilities and add it to the
 * list of tasks that are ready to run.
 *
 * @note: This is an internal function and meant for usage by pthread only.
 * @note: Tasks with stacks not having the default heap capabilities for FreeRTOS may
 *        not be able to run while flash cache is disabled.
 *        This is the case if, e.g., any task on the chip is reading or writing
 *        flash memory.
 *
 * @note: The difference between this function and xTaskCreatePinnedToCoreWithCaps()
 *        is the following:
 *        * A FreeRTOS task created by this function is deleted by normal FreeRTOS deletion functions,
 *          i.e., vTaskDelete().
 *        * A FreeRTOS task created by this function can delete itself.
 *
 * This function behaves like xTaskCreateAffinitySet(), except that it allocates
 * the stack from the heap with the desired heap capabilities.
 *
 * @param pxTaskCode Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 *
 * @param pcName A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  Max length defined by configMAX_TASK_NAME_LEN - default
 * is 16.
 *
 * @param usStackDepth The size of the task stack specified as the number of bytes.
 *
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 *
 * @param uxPriority The priority at which the task should run.  Systems that
 * include MPU support can optionally create tasks in a privileged (system)
 * mode by setting bit portPRIVILEGE_BIT of the priority parameter.  For
 * example, to create a privileged task at priority 2 the uxPriority parameter
 * should be set to ( 2 | portPRIVILEGE_BIT ).
 *
 * @param xCoreID (only IDF SMP FreeRTOS)
 * The core to which the task is pinned to, or tskNO_AFFINITY if the task has no
 * core affinity.
 *
 * @param uxCoreAffinityMask (only Amazon SMP FreeRTOS)
 * A bitwise value that indicates the cores on which the task can run.
 * Cores are numbered from 0 to configNUM_CORES - 1.
 * For example, to ensure that a task can run on core 0 and core 1, set
 * uxCoreAffinityMask to 0x03. Note that only one of the cores will be used when
 * using the IDF SMP version of FreeRTOS instead of Amazon FreeRTOS SMP.
 *
 * @param uxStackMemoryCaps The heap caps bitfield describing the memory capabilities to
 * be used for stack memory. Currently, the capabilities have to include MALLOC_CAP_8BIT
 * and one of the following: MALLOC_CAP_SPIRAM, MALLOC_CAP_INTERNAL.
 *
 * @param pxCreatedTask Used to pass back a handle by which the created task
 * can be referenced.
 *
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 */
    #if CONFIG_FREERTOS_SMP
        BaseType_t prvTaskCreateDynamicAffinitySetWithCaps( TaskFunction_t pxTaskCode,
                                                            const char * const pcName,
                                                            const configSTACK_DEPTH_TYPE usStackDepth,
                                                            void * const pvParameters,
                                                            UBaseType_t uxPriority,
                                                            UBaseType_t uxCoreAffinityMask,
                                                            UBaseType_t uxStackMemoryCaps,
                                                            TaskHandle_t * const pxCreatedTask );
    #else
        BaseType_t prvTaskCreateDynamicPinnedToCoreWithCaps( TaskFunction_t pxTaskCode,
                                                             const char * const pcName,
                                                             const configSTACK_DEPTH_TYPE usStackDepth,
                                                             void * const pvParameters,
                                                             UBaseType_t uxPriority,
                                                             const BaseType_t xCoreID,
                                                             UBaseType_t uxStackMemoryCaps,
                                                             TaskHandle_t * const pxCreatedTask );
    #endif // CONFIG_FREERTOS_SMP

#endif // CONFIG_SPIRAM

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */
