/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "idf_additions_inc.h"

#if ( INCLUDE_vTaskPrioritySet == 1 )

/**
 * INCLUDE_vTaskPrioritySet must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Saves the current priority and current base priority of a task, then raises the tasks
 * current and base priority to uxNewPriority if uxNewPriority is of a higher priority.
 * Once a task's priority has been raised with this function, the priority can be restored
 * by calling prvTaskPriorityRestore()
 * - Note that this function differs from vTaskPrioritySet() as the task's current priority
 *   will be modified even if the task has already inherited a priority.
 * - This function is intended for special circumstance where a task must be forced immediately
 *   to a higher priority.
 *
 * For configUSE_MUTEXES == 0: A context switch will occur before the function returns if the priority
 * being set is higher than the currently executing task.
 *
 * @note This functions is private is only be called internally within various IDF components.
 *       Users should never call this function from their application.
 *
 * @note vTaskPrioritySet() should not be called while a task's priority is already raised via this function
 *
 * @param pxSavedPriority returns base and current priorities
 *
 * @param uxNewPriority The priority to which the task will be set.
 */
void prvTaskPriorityRaise( prvTaskSavedPriority_t * pxSavedPriority, UBaseType_t uxNewPriority );

/**
 * INCLUDE_vTaskPrioritySet must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Restore a task's priority that was previously raised by prvTaskPriorityRaise().
 *
 * For configUSE_MUTEXES == 0: A context switch will occur before the function returns if the priority
 * being set is higher than the currently executing task.
 *
 * @note This functions is private is only be called internally within various IDF components.
 *       Users should never call this function from their application.
 *
 * @param pxSavedPriority previously saved base and current priorities that need to be restored
 */
void prvTaskPriorityRestore( prvTaskSavedPriority_t * pxSavedPriority );

#endif // ( INCLUDE_vTaskPrioritySet == 1)
