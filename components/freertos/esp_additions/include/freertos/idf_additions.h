/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
 * This file contains the function prototypes of ESP-IDF specific API additions
 * to the FreeRTOS kernel. These API additions are not part of Vanilla (i.e.,
 * upstream) FreeRTOS and include things such as....
 *    - Various helper functions
 *    - API for ESP-IDF feature additions to FreeRTOS (such as TSLP deletion
 * call backs)
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"
#include "freertos/event_groups.h"
#include "esp_heap_caps.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */


/* -------------------------------------------------- Task Creation ------------------------------------------------- */

#if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

/**
 * @brief Create a new task that is pinned to a particular core
 *
 * This function is similar to xTaskCreate(), but allows the creation of a pinned
 * task. The task's pinned core is specified by the xCoreID argument. If xCoreID
 * is set to tskNO_AFFINITY, then the task is unpinned and can run on any core.
 *
 * @note If ( configNUMBER_OF_CORES == 1 ), setting xCoreID to tskNO_AFFINITY will be
 * be treated as 0.
 *
 * @param pxTaskCode Pointer to the task entry function.
 * @param pcName A descriptive name for the task.
 * @param ulStackDepth The size of the task stack specified as the NUMBER OF
 * BYTES. Note that this differs from vanilla FreeRTOS.
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 * @param uxPriority The priority at which the task should run.
 * @param pxCreatedTask Used to pass back a handle by which the created task can
 * be referenced.
 * @param xCoreID The core to which the task is pinned to, or tskNO_AFFINITY if
 * the task has no core affinity.
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 */
    BaseType_t xTaskCreatePinnedToCore( TaskFunction_t pxTaskCode,
                                        const char * const pcName,
                                        const uint32_t ulStackDepth,
                                        void * const pvParameters,
                                        UBaseType_t uxPriority,
                                        TaskHandle_t * const pxCreatedTask,
                                        const BaseType_t xCoreID );

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Create a new static task that is pinned to a particular core
 *
 * This function is similar to xTaskCreateStatic(), but allows the creation of a
 * pinned task. The task's pinned core is specified by the xCoreID argument. If
 * xCoreID is set to tskNO_AFFINITY, then the task is unpinned and can run on any
 * core.
 *
 * @note If ( configNUMBER_OF_CORES == 1 ), setting xCoreID to tskNO_AFFINITY will be
 * be treated as 0.
 *
 * @param pxTaskCode Pointer to the task entry function.
 * @param pcName A descriptive name for the task.
 * @param ulStackDepth The size of the task stack specified as the NUMBER OF
 * BYTES. Note that this differs from vanilla FreeRTOS.
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 * @param uxPriority The priority at which the task should run.
 * @param puxStackBuffer Must point to a StackType_t array that has at least
 * ulStackDepth indexes
 * @param pxTaskBuffer Must point to a variable of type StaticTask_t, which will
 * then be used to hold the task's data structures,
 * @param xCoreID The core to which the task is pinned to, or tskNO_AFFINITY if
 * the task has no core affinity.
 * @return The task handle if the task was created, NULL otherwise.
 */
    TaskHandle_t xTaskCreateStaticPinnedToCore( TaskFunction_t pxTaskCode,
                                                const char * const pcName,
                                                const uint32_t ulStackDepth,
                                                void * const pvParameters,
                                                UBaseType_t uxPriority,
                                                StackType_t * const puxStackBuffer,
                                                StaticTask_t * const pxTaskBuffer,
                                                const BaseType_t xCoreID );

#endif /* configSUPPORT_STATIC_ALLOCATION */

/* ------------------------------------------------- Task Utilities ------------------------------------------------- */

/**
 * @brief Get the current core ID of a particular task
 *
 * Helper function to get the core ID of a particular task. If the task is
 * pinned to a particular core, the core ID is returned. If the task is not
 * pinned to a particular core, tskNO_AFFINITY is returned.
 *
 * If CONFIG_FREERTOS_UNICORE is enabled, this function simply returns 0.
 *
 * [refactor-todo] See if this needs to be deprecated (IDF-8145)(IDF-8164)
 *
 * @note If CONFIG_FREERTOS_SMP is enabled, please call vTaskCoreAffinityGet()
 * instead.
 * @note In IDF FreerTOS when configNUMBER_OF_CORES == 1, this function will
 * always return 0,
 * @param xTask The task to query
 * @return The task's core ID or tskNO_AFFINITY
 */
BaseType_t xTaskGetCoreID( TaskHandle_t xTask );

/** @cond */
/* Todo: Deprecate this API in favor of xTaskGetIdleTaskHandleForCore (IDF-8163) */
static inline __attribute__( ( always_inline ) )
BaseType_t xTaskGetAffinity( TaskHandle_t xTask )
{
    return xTaskGetCoreID( xTask );
}
/** @endcond */

/**
 * @brief Get the handle of idle task for the given core.
 *
 * [refactor-todo] See if this needs to be deprecated (IDF-8145)
 *
 * @note If CONFIG_FREERTOS_SMP is enabled, please call xTaskGetIdleTaskHandle()
 * instead.
 * @param xCoreID The core to query
 * @return Handle of the idle task for the queried core
 */
TaskHandle_t xTaskGetIdleTaskHandleForCore( BaseType_t xCoreID );

/** @cond */
/* Todo: Deprecate this API in favor of xTaskGetIdleTaskHandleForCore (IDF-8163) */
static inline __attribute__( ( always_inline ) )
TaskHandle_t xTaskGetIdleTaskHandleForCPU( BaseType_t xCoreID )
{
    return xTaskGetIdleTaskHandleForCore( xCoreID );
}
/** @endcond */

/**
 * @brief Get the handle of the task currently running on a certain core
 *
 * Because of the nature of SMP processing, there is no guarantee that this
 * value will still be valid on return and should only be used for debugging
 * purposes.
 *
 * [refactor-todo] See if this needs to be deprecated (IDF-8145)
 *
 * @note If CONFIG_FREERTOS_SMP is enabled, please call xTaskGetCurrentTaskHandleCPU()
 * instead.
 * @param xCoreID The core to query
 * @return Handle of the current task running on the queried core
 */
TaskHandle_t xTaskGetCurrentTaskHandleForCore( BaseType_t xCoreID );

/** @cond */
/* Todo: Deprecate this API in favor of xTaskGetCurrentTaskHandleForCore (IDF-8163) */
static inline __attribute__( ( always_inline ) )
TaskHandle_t xTaskGetCurrentTaskHandleForCPU( BaseType_t xCoreID )
{
    return xTaskGetCurrentTaskHandleForCore( xCoreID );
}
/** @endcond */

#if ( !CONFIG_FREERTOS_SMP && ( configGENERATE_RUN_TIME_STATS == 1 ) && ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) )

/**
 * @brief Get the total execution of a particular core's idle task
 *
 * This function is equivalent to ulTaskGetIdleRunTimeCounter() but queries the
 * idle task of a particular core.
 *
 * @param xCoreID Core ID of the idle task to query
 * @return The total run time of the idle task
 */
    configRUN_TIME_COUNTER_TYPE ulTaskGetIdleRunTimeCounterForCore( BaseType_t xCoreID );

/**
 * @brief Get the percentage run time of a particular core's idle task
 *
 * This function is equivalent to ulTaskGetIdleRunTimePercent() but queries the
 * idle task of a particular core.
 *
 * @param xCoreID Core ID of the idle task to query
 * @return The percentage run time of the idle task
 */
    configRUN_TIME_COUNTER_TYPE ulTaskGetIdleRunTimePercentForCore( BaseType_t xCoreID );

#endif /* ( !CONFIG_FREERTOS_SMP && ( configGENERATE_RUN_TIME_STATS == 1 ) && ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) ) */

/**
 * Returns the start of the stack associated with xTask.
 *
 * Returns the lowest stack memory address, regardless of whether the stack
 * grows up or down.
 *
 * [refactor-todo] Change return type to StackType_t (IDF-8158)
 *
 * @param xTask Handle of the task associated with the stack returned.
 * Set xTask to NULL to return the stack of the calling task.
 *
 * @return A pointer to the start of the stack.
 */
uint8_t * pxTaskGetStackStart( TaskHandle_t xTask );

/* --------------------------------------------- TLSP Deletion Callbacks -------------------------------------------- */

#if CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS

/**
 * Prototype of local storage pointer deletion callback.
 */
    typedef void (* TlsDeleteCallbackFunction_t)( int,
                                                  void * );
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

#if CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS

/**
 * Set local storage pointer and deletion callback.
 *
 * Each task contains an array of pointers that is dimensioned by the
 * configNUM_THREAD_LOCAL_STORAGE_POINTERS setting in FreeRTOSConfig.h. The
 * kernel does not use the pointers itself, so the application writer can use
 * the pointers for any purpose they wish.
 *
 * Local storage pointers set for a task can reference dynamically allocated
 * resources. This function is similar to vTaskSetThreadLocalStoragePointer, but
 * provides a way to release these resources when the task gets deleted. For
 * each pointer, a callback function can be set. This function will be called
 * when task is deleted, with the local storage pointer index and value as
 * arguments.
 *
 * @param xTaskToSet  Task to set thread local storage pointer for
 * @param xIndex The index of the pointer to set, from 0 to
 * configNUM_THREAD_LOCAL_STORAGE_POINTERS - 1.
 * @param pvValue Pointer value to set.
 * @param pvDelCallback  Function to call to dispose of the local storage
 * pointer when the task is deleted.
 */
    void vTaskSetThreadLocalStoragePointerAndDelCallback( TaskHandle_t xTaskToSet,
                                                          BaseType_t xIndex,
                                                          void * pvValue,
                                                          TlsDeleteCallbackFunction_t pvDelCallback );

#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

/* -------------------------------------------- Creation With Memory Caps ----------------------------------------------
 * Helper functions to create various FreeRTOS objects (e.g., queues, semaphores) with specific memory capabilities
 * (e.g., MALLOC_CAP_INTERNAL).
 * ------------------------------------------------------------------------------------------------------------------ */

/* ---------------------------------- Tasks --------------------------------- */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a pinned task where its stack has specific memory capabilities
 *
 * This function is similar to xTaskCreatePinnedToCore(), except that it allows
 * the memory allocated for the task's stack to have specific capabilities
 * (e.g., MALLOC_CAP_SPIRAM).
 *
 * However, the specified capabilities will NOT apply to the task's TCB as a TCB
 * must always be in internal RAM.
 *
 * @param pvTaskCode Pointer to the task entry function
 * @param pcName A descriptive name for the task
 * @param usStackDepth The size of the task stack specified as the number of
 * bytes
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 * @param uxPriority The priority at which the task should run.
 * @param pvCreatedTask Used to pass back a handle by which the created task can
 * be referenced.
 * @param xCoreID Core to which the task is pinned to, or tskNO_AFFINITY if
 * unpinned.
 * @param uxMemoryCaps Memory capabilities of the task stack's memory (see
 * esp_heap_caps.h)
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 */
    BaseType_t xTaskCreatePinnedToCoreWithCaps( TaskFunction_t pvTaskCode,
                                                const char * const pcName,
                                                const configSTACK_DEPTH_TYPE usStackDepth,
                                                void * const pvParameters,
                                                UBaseType_t uxPriority,
                                                TaskHandle_t * const pvCreatedTask,
                                                const BaseType_t xCoreID,
                                                UBaseType_t uxMemoryCaps );

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a task where its stack has specific memory capabilities
 *
 * This function is similar to xTaskCreate(), except that it allows the memory
 * allocated for the task's stack to have specific capabilities (e.g.,
 * MALLOC_CAP_SPIRAM).
 *
 * However, the specified capabilities will NOT apply to the task's TCB as a TCB
 * must always be in internal RAM.
 *
 * @note A task created using this function must only be deleted using
 * vTaskDeleteWithCaps()
 * @param pvTaskCode Pointer to the task entry function
 * @param pcName A descriptive name for the task
 * @param usStackDepth The size of the task stack specified as the number of
 * bytes
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 * @param uxPriority The priority at which the task should run.
 * @param pvCreatedTask Used to pass back a handle by which the created task can
 * be referenced.
 * @param uxMemoryCaps Memory capabilities of the task stack's memory (see
 * esp_heap_caps.h)
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 */
    static inline BaseType_t xTaskCreateWithCaps( TaskFunction_t pvTaskCode,
                                                  const char * const pcName,
                                                  configSTACK_DEPTH_TYPE usStackDepth,
                                                  void * const pvParameters,
                                                  UBaseType_t uxPriority,
                                                  TaskHandle_t * pvCreatedTask,
                                                  UBaseType_t uxMemoryCaps )
    {
        return xTaskCreatePinnedToCoreWithCaps( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pvCreatedTask, tskNO_AFFINITY, uxMemoryCaps );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Deletes a task previously created using xTaskCreateWithCaps() or
 * xTaskCreatePinnedToCoreWithCaps()
 *
 * @param xTaskToDelete A handle to the task to be deleted
 */
    void vTaskDeleteWithCaps( TaskHandle_t xTaskToDelete );

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

/* ---------------------------------- Queue --------------------------------- */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a queue with specific memory capabilities
 *
 * This function is similar to xQueueCreate(), except that it allows the memory
 * allocated for the queue to have specific capabilities (e.g.,
 * MALLOC_CAP_INTERNAL).
 *
 * @note A queue created using this function must only be deleted using
 * vQueueDeleteWithCaps()
 * @param uxQueueLength The maximum number of items that the queue can contain.
 * @param uxItemSize The number of bytes each item in the queue will require.
 * @param uxMemoryCaps Memory capabilities of the queue's memory (see
 * esp_heap_caps.h)
 * @return Handle to the created queue or NULL on failure.
 */
    QueueHandle_t xQueueCreateWithCaps( UBaseType_t uxQueueLength,
                                        UBaseType_t uxItemSize,
                                        UBaseType_t uxMemoryCaps );

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Deletes a queue previously created using xQueueCreateWithCaps()
 *
 * @param xQueue A handle to the queue to be deleted.
 */
    void vQueueDeleteWithCaps( QueueHandle_t xQueue );

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

/* -------------------------------- Semaphore ------------------------------- */

/** @cond */ /* Doxygen command to hide this from docs */
SemaphoreHandle_t xSemaphoreCreateGenericWithCaps( UBaseType_t uxMaxCount,
                                                   UBaseType_t uxInitialCount,
                                                   const uint8_t ucQueueType,
                                                   UBaseType_t uxMemoryCaps );
/** @endcond */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a binary semaphore with specific memory capabilities
 *
 * This function is similar to vSemaphoreCreateBinary(), except that it allows
 * the memory allocated for the binary semaphore to have specific capabilities
 * (e.g., MALLOC_CAP_INTERNAL).
 *
 * @note A binary semaphore created using this function must only be deleted
 * using vSemaphoreDeleteWithCaps()
 * @param uxMemoryCaps Memory capabilities of the binary semaphore's memory (see
 * esp_heap_caps.h)
 * @return Handle to the created binary semaphore or NULL on failure.
 */
    static inline SemaphoreHandle_t xSemaphoreCreateBinaryWithCaps( UBaseType_t uxMemoryCaps )
    {
        return xSemaphoreCreateGenericWithCaps( 0, 0, queueQUEUE_TYPE_BINARY_SEMAPHORE, uxMemoryCaps );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a counting semaphore with specific memory capabilities
 *
 * This function is similar to xSemaphoreCreateCounting(), except that it allows
 * the memory allocated for the counting semaphore to have specific capabilities
 * (e.g., MALLOC_CAP_INTERNAL).
 *
 * @note A counting semaphore created using this function must only be deleted
 * using vSemaphoreDeleteWithCaps()
 * @param uxMaxCount The maximum count value that can be reached.
 * @param uxInitialCount The count value assigned to the semaphore when it is
 * created.
 * @param uxMemoryCaps Memory capabilities of the counting semaphore's memory
 * (see esp_heap_caps.h)
 * @return Handle to the created counting semaphore or NULL on failure.
 */
    static inline SemaphoreHandle_t xSemaphoreCreateCountingWithCaps( UBaseType_t uxMaxCount,
                                                                      UBaseType_t uxInitialCount,
                                                                      UBaseType_t uxMemoryCaps )
    {
        return xSemaphoreCreateGenericWithCaps( uxMaxCount, uxInitialCount, queueQUEUE_TYPE_COUNTING_SEMAPHORE, uxMemoryCaps );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a mutex semaphore with specific memory capabilities
 *
 * This function is similar to xSemaphoreCreateMutex(), except that it allows
 * the memory allocated for the mutex semaphore to have specific capabilities
 * (e.g., MALLOC_CAP_INTERNAL).
 *
 * @note A mutex semaphore created using this function must only be deleted
 * using vSemaphoreDeleteWithCaps()
 * @param uxMemoryCaps Memory capabilities of the mutex semaphore's memory (see
 * esp_heap_caps.h)
 * @return Handle to the created mutex semaphore or NULL on failure.
 */
    static inline SemaphoreHandle_t xSemaphoreCreateMutexWithCaps( UBaseType_t uxMemoryCaps )
    {
        return xSemaphoreCreateGenericWithCaps( 0, 0, queueQUEUE_TYPE_MUTEX, uxMemoryCaps );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a recursive mutex with specific memory capabilities
 *
 * This function is similar to xSemaphoreCreateRecursiveMutex(), except that it
 * allows the memory allocated for the recursive mutex to have specific
 * capabilities (e.g., MALLOC_CAP_INTERNAL).
 *
 * @note A recursive mutex created using this function must only be deleted
 * using vSemaphoreDeleteWithCaps()
 * @param uxMemoryCaps Memory capabilities of the recursive mutex's memory (see
 * esp_heap_caps.h)
 * @return Handle to the created recursive mutex or NULL on failure.
 */
    static inline SemaphoreHandle_t xSemaphoreCreateRecursiveMutexWithCaps( UBaseType_t uxMemoryCaps )
    {
        return xSemaphoreCreateGenericWithCaps( 0, 0, queueQUEUE_TYPE_RECURSIVE_MUTEX, uxMemoryCaps );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Deletes a semaphore previously created using one of the
 * xSemaphoreCreate...WithCaps() functions
 *
 * @param xSemaphore A handle to the semaphore to be deleted.
 */
    void vSemaphoreDeleteWithCaps( SemaphoreHandle_t xSemaphore );

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

/* ------------------------ Stream & Message Buffers ------------------------ */

/** @cond */ /* Doxygen command to hide this from docs */
StreamBufferHandle_t xStreamBufferGenericCreateWithCaps( size_t xBufferSizeBytes,
                                                         size_t xTriggerLevelBytes,
                                                         BaseType_t xIsMessageBuffer,
                                                         UBaseType_t uxMemoryCaps );

void vStreamBufferGenericDeleteWithCaps( StreamBufferHandle_t xStreamBuffer,
                                         BaseType_t xIsMessageBuffer );
/** @endcond */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a stream buffer with specific memory capabilities
 *
 * This function is similar to xStreamBufferCreate(), except that it allows the
 * memory allocated for the stream buffer to have specific capabilities (e.g.,
 * MALLOC_CAP_INTERNAL).
 *
 * @note A stream buffer created using this function must only be deleted using
 * vStreamBufferDeleteWithCaps()
 * @param xBufferSizeBytes The total number of bytes the stream buffer will be
 * able to hold at any one time.
 * @param xTriggerLevelBytes The number of bytes that must be in the stream
 * buffer before unblocking
 * @param uxMemoryCaps Memory capabilities of the stream buffer's memory (see
 * esp_heap_caps.h)
 * @return Handle to the created stream buffer or NULL on failure.
 */
    static inline StreamBufferHandle_t xStreamBufferCreateWithCaps( size_t xBufferSizeBytes,
                                                                    size_t xTriggerLevelBytes,
                                                                    UBaseType_t uxMemoryCaps )
    {
        return xStreamBufferGenericCreateWithCaps( xBufferSizeBytes, xTriggerLevelBytes, pdFALSE, uxMemoryCaps );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Deletes a stream buffer previously created using
 * xStreamBufferCreateWithCaps()
 *
 * @param xStreamBuffer A handle to the stream buffer to be deleted.
 */
    static inline void vStreamBufferDeleteWithCaps( StreamBufferHandle_t xStreamBuffer )
    {
        vStreamBufferGenericDeleteWithCaps( xStreamBuffer, pdFALSE );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates a message buffer with specific memory capabilities
 *
 * This function is similar to xMessageBufferCreate(), except that it allows the
 * memory allocated for the message buffer to have specific capabilities (e.g.,
 * MALLOC_CAP_INTERNAL).
 *
 * @note A message buffer created using this function must only be deleted using
 * vMessageBufferDeleteWithCaps()
 * @param xBufferSizeBytes The total number of bytes (not messages) the message
 * buffer will be able to hold at any one time.
 * @param uxMemoryCaps Memory capabilities of the message buffer's memory (see
 * esp_heap_caps.h)
 * @return Handle to the created message buffer or NULL on failure.
 */
    static inline MessageBufferHandle_t xMessageBufferCreateWithCaps( size_t xBufferSizeBytes,
                                                                      UBaseType_t uxMemoryCaps )
    {
        return ( MessageBufferHandle_t ) xStreamBufferGenericCreateWithCaps( xBufferSizeBytes, ( size_t ) 0, pdTRUE, uxMemoryCaps );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Deletes a stream buffer previously created using
 * xMessageBufferCreateWithCaps()
 *
 * @param xMessageBuffer A handle to the message buffer to be deleted.
 */
    static inline void vMessageBufferDeleteWithCaps( MessageBufferHandle_t xMessageBuffer )
    {
        vStreamBufferGenericDeleteWithCaps( ( StreamBufferHandle_t ) xMessageBuffer, pdTRUE );
    }

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

/* ------------------------------ Event Groups ------------------------------ */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Creates an event group with specific memory capabilities
 *
 * This function is similar to xEventGroupCreate(), except that it allows the
 * memory allocated for the event group to have specific capabilities (e.g.,
 * MALLOC_CAP_INTERNAL).
 *
 * @note An event group created using this function must only be deleted using
 * vEventGroupDeleteWithCaps()
 * @param uxMemoryCaps Memory capabilities of the event group's memory (see
 * esp_heap_caps.h)
 * @return Handle to the created event group or NULL on failure.
 */
    EventGroupHandle_t xEventGroupCreateWithCaps( UBaseType_t uxMemoryCaps );

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/**
 * @brief Deletes an event group previously created using
 * xEventGroupCreateWithCaps()
 *
 * @param xEventGroup A handle to the event group to be deleted.
 */
    void vEventGroupDeleteWithCaps( EventGroupHandle_t xEventGroup );

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */
