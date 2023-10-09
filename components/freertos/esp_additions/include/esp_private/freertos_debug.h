/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
 * This header contains private API used by various ESP-IDF debugging features (e.g., esp_gdbstub).
 */

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/* -------------------------------------------------- Task Snapshot ------------------------------------------------- */

/**
 * @brief Task Snapshot structure
 *
 * - Used with the uxTaskGetSnapshotAll() function to save memory snapshot of each task in the system.
 * - We need this structure because TCB_t is defined (hidden) in tasks.c.
 */
typedef struct xTASK_SNAPSHOT
{
    void * pxTCB;               /*!< Address of the task control block. */
    StackType_t * pxTopOfStack; /*!< Points to the location of the last item placed on the tasks stack. */
    StackType_t * pxEndOfStack; /*!< Points to the end of the stack. pxTopOfStack < pxEndOfStack, stack grows hi2lo
                                 *  pxTopOfStack > pxEndOfStack, stack grows lo2hi*/
} TaskSnapshot_t;

/**
 * @brief Iterate over all tasks in the system
 *
 * - This function can be used to iterate over every task in the system
 * - The first call to this function must set pxTask to NULL
 * - When all functions have been iterated, this function will return NULL.
 *
 * @note This function should only be called when FreeRTOS is no longer running (e.g., during a panic) as this function
 *       does not acquire any locks.
 * @param pxTask Handle of the previous task (or NULL on the first call of this function)
 * @return TaskHandle_t Handle of the next task (or NULL when all tasks have been iterated over)
 */
TaskHandle_t pxTaskGetNext( TaskHandle_t pxTask );

/**
 * @brief Fill a TaskSnapshot_t structure for specified task.
 *
 * - This function is used by the panic handler to get the snapshot of a particular task.
 *
 * @note This function should only be called when FreeRTOS is no longer running (e.g., during a panic) as this function
 *       does not acquire any locks.
 * @param[in] pxTask Task's handle
 * @param[out] pxTaskSnapshot Snapshot of the task
 * @return pdTRUE if operation was successful else pdFALSE
 */
BaseType_t vTaskGetSnapshot( TaskHandle_t pxTask,
                             TaskSnapshot_t * pxTaskSnapshot );

/**
 * @brief Fill an array of TaskSnapshot_t structures for every task in the system
 *
 * - This function is used by the panic handler to get a snapshot of all tasks in the system
 *
 * @note This function should only be called when FreeRTOS is no longer running (e.g., during a panic) as this function
 *        does not acquire any locks.
 * @param[out] pxTaskSnapshotArray Array of TaskSnapshot_t structures filled by this function
 * @param[in] uxArrayLength Length of the provided array
 * @param[out] pxTCBSize Size of the a task's TCB structure
 * @return UBaseType_t
 */
UBaseType_t uxTaskGetSnapshotAll( TaskSnapshot_t * const pxTaskSnapshotArray,
                                  const UBaseType_t uxArrayLength,
                                  UBaseType_t * const pxTCBSize );

/* ----------------------------------------------------- Misc ----------------------------------------------------- */

/**
 * @brief Get a void pointer to the current TCB of a particular core
 *
 * @note This function provides no guarantee that the return TCB will still be the current task (or that the task still
 * exists) when it returns. It is the caller's responsibility to ensure that the task does not get scheduled or deleted.
 * @param xCoreID The core to query
 * @return Void pointer to current TCB
 */
void * pvTaskGetCurrentTCBForCore( BaseType_t xCoreID );

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */
